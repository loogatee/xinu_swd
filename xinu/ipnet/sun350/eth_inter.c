/* ethinter.c - ethinter */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <systypes.h>
#include <lereg.h>
#include <ether_bsp.h>

/*------------------------------------------------------------------------
 *  ethinter - ethernet interrupt processing procedure
 *
 *   Interrupts are disabled before this routine is called
 *------------------------------------------------------------------------
 */
int ethinter(etptr)
struct	etblk	*etptr;
{
	struct etblk_bsp	*etbsp;
	register struct le_device *le;
	struct	netif	*pni = &nif[etptr->etintf];
	struct	le_md	*pmd;
	struct	bre	*pbre;
	short		csr;
	int		i, j, oldprio;

	etbsp = (struct etblk_bsp *)etptr->etbsp;
	le = (struct le_device *)etptr->etle;
	le->le_rap = LE_CSR0;

	/* see if a hardware error has occurred */
	if ( ((csr = le->le_rdp)&LE_ERR) != 0) {
#ifdef PRINTERRORS
		if (csr&LE_BABL)
			printcsr0(le,"LEerr: Babbling xmitter");
		if (csr&LE_CERR)
			printcsr0(le,"LEerr: Collision error");
		if (csr&LE_MISS)
			printcsr0(le,"LEerr: Missed packet");
#endif	PRINTERRORS
		if (csr&LE_BABL)
			pni->ni_oerrors++;
		else
			pni->ni_ierrors++;
		/* reset the interrupt */
		le->le_csr = LE_BABL|LE_CERR|LE_MISS|LE_MERR|LE_INEA;
	}
 	if ( ((csr = le->le_rdp) & LE_TINT) != 0) {
		/* reset the interrupt */
		le->le_csr = LE_TINT|LE_INEA;

		pbre = &etbsp->etbrt[0];
		pmd  = pbre->pmd;
		if ( (pmd->lmd_flags&LMD_ERR) == 0)
			etptr->etxpending--;
		else if (etptr->etwtry-- > 0) { /* retry on error */
			pmd->lmd_flags = LMD_OWN;
			pmd->lmd_bcnt  = 0;
			pmd->lmd_flags = LMD_OWN | LMD_STP | LMD_ENP;
			le->le_csr = LE_TDMD | LE_INEA;
		} else
			etptr->etxpending--;
	}

	/* check to see if a receive has completed */
	if (le->le_rdp & LE_RINT) {
	/* the chip won't tell us which buffer it filled in */
	/* But, etnextbuf always points to the next buf after */
	/* the last buffer we examined */

	/* reset the interrupt */
	le->le_csr = LE_RINT | LE_INEA;

	if (etptr->etrpending)
		goto out;
	etptr->etrpending++;
	oldprio = getprio(currpid);
	if (oldprio < NETIPRI)
		oldprio = chprio(currpid, NETIPRI);

	pbre = &etbsp->etbrr[etptr->etnextbuf];
	pmd  = pbre->pmd;
	while(((pmd->lmd_flags&LMD_OWN) == 0) && (pbre->flags == 0)) {
		if ((pmd->lmd_flags&LMD_ERR) == 0) {
			pbre->flags = 1;
			ethdemux(etptr);
		} else {	/* error, so retry operation */
			pmd->lmd_flags = LMD_OWN;
			pmd->lmd_bcnt  = 0;
		}
		etptr->etnextbuf = (etptr->etnextbuf + 1) % EP_NUMRCV;
		pbre = &etbsp->etbrr[etptr->etnextbuf];
		pmd  = pbre->pmd;
	}
	etptr->etrpending--;
	chprio(currpid, oldprio);
	}
out:
	ethwstrt(etptr);
}
