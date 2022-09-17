/* inithost.c - inithost */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	int		getsim();
		int		efaceinit();

static	int	itod[] = { -1, ETHER };

/*------------------------------------------------------------------------
 * inithost  --  initialize a gateway's interface structures
 *------------------------------------------------------------------------
 */
int inithost()
{
	int	i;

	Net.netpool = mkpool(MAXNETBUF, NETBUFS);
	Net.lrgpool = mkpool(MAXLRGBUF, LRGBUFS);
	Net.sema = screate(1);
	Net.nif = NIF;

	for (i=0; i<Net.nif; ++i) {
		/* start with everything 0 */
		bzero(&nif[i], sizeof(nif[i]));
		nif[i].ni_state = NIS_DOWN;
		nif[i].ni_ivalid = nif[i].ni_nvalid = FALSE;
		nif[i].ni_svalid = FALSE;
		nif[i].ni_dev = itod[i];
		if (i == NI_LOCAL) {
			/* maxbuf - ether header - CRC - nexthop */
			nif[i].ni_mtu = MAXLRGBUF-EP_HLEN-EP_CRC-IP_ALEN;
			blkcopy(nif[i].ni_ip, ip_anyaddr, IP_ALEN);
			continue;
		}
		switch(nif[i].ni_dev) {
		case ETHER:	efaceinit(i);	/* for the simulation */
				break;
		};
	}

	return OK;
}



efaceinit(iface)
int	iface;
{
	IPaddr	junk;
	struct etblk *etptr = (struct etblk *)
			devtab[nif[iface].ni_dev].dvioblk;

	blkcopy(nif[iface].ni_hwa.ha_addr, etptr->etpaddr, EP_ALEN);
	blkcopy(nif[iface].ni_hwb.ha_addr, etptr->etbcast, EP_ALEN);
	nif[iface].ni_descr = etptr->etdescr;
	nif[iface].ni_mtype = 6;	/* RFC 1156, Ethernet CSMA/CD	*/
	nif[iface].ni_speed = 10000000;	/* bits per second		*/
	nif[iface].ni_hwa.ha_len = EP_ALEN;
	nif[iface].ni_hwb.ha_len = EP_ALEN;
	nif[iface].ni_mtu = EP_DLEN;
	nif[iface].ni_outq = etptr->etoutq;
	etptr->etintf = iface;
}
