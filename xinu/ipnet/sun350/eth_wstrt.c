/* ethwstrt.c - ethwstrt */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <systypes.h>
#include <lereg.h>
#include <ether_bsp.h>

extern int udpouttmp;

/*------------------------------------------------------------------------
 *  ethwstrt - start an ethernet write operation on the LANCE
 *------------------------------------------------------------------------
 */
int
ethwstrt(pet)
struct	etblk	*pet;
{
	struct	le_device	*le;
	struct etblk_bsp	*etbsp;
	struct	le_md		*pmdxmit;
	STATWORD		ps;
	struct	ep		*pep;
	int			len;
	Bool			bcast;

	disable(ps);

	etbsp = (struct etblk_bsp *)pet->etbsp;

	if (pet->etxpending) {
		restore(ps);
		return OK;
	}
	pet->etxpending++;
	pep = (struct ep *)deq(pet->etoutq);
	if (pep == 0) {
		pet->etxpending--;
		restore(ps);
		return OK;
	}
	bcast = blkequ(pep->ep_dst, pet->etbcast, EP_ALEN);
	len = pep->ep_len;
	le = (struct le_device *)pet->etle;
	pmdxmit = etbsp->etbrt[0].pmd;
	pet->etwtry = EP_RETRY;
	blkcopy(etbsp->etbrt[0].buf, &pep->ep_eh, pep->ep_len);
	pmdxmit->lmd_bcnt = -pep->ep_len;
	pmdxmit->lmd_mcnt = 0;
	pmdxmit->lmd_flags = LMD_OWN | LMD_STP | LMD_ENP;
        le->le_csr = LE_TDMD | LE_INEA;
	restore(ps);
	freebuf(pep);
	if (bcast)
		nif[pet->etintf].ni_onucast++;
	else
		nif[pet->etintf].ni_oucast++;
	nif[pet->etintf].ni_ooctets += len;
}
