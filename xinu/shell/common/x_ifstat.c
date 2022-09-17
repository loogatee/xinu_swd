/* x_ifstat.c - x_ifstat */

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	char *sn[] = { "DOWN", "UP", "UNUSED" };

/*------------------------------------------------------------------------
 *  x_ifstat  -  print interface status information
 *------------------------------------------------------------------------
 */
COMMAND	x_ifstat(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char	str[80];
	char	*dn;
	struct	netif	*pni;
	int	iface;

	if (nargs != 2) {
		fprintf(stdout, "usage: ifstat <intf>\n");
		return OK;
	}
	iface = atoi(args[1]);
	if (iface < 0 || iface >= Net.nif) {
		fprintf(stdout, "ifstat: illegal interface (0-%d)\n", Net.nif-1);
		return OK;
	}
	if (iface == NI_LOCAL) {
		fprintf(stdout, "Local interface state: %s\n",
			sn[nif[NI_LOCAL].ni_state]);
		return OK;
	}
	pni = &nif[iface];
	switch(pni->ni_dev) {
	case ETHER:	dn = "le0";
			break;
	default:
		dn = "??";
	}
	fprintf(stdout,"%s: state=%x<%s>\n",dn,pni->ni_state,sn[pni->ni_state]);
	if (pni->ni_ivalid)
		fprintf(stdout, "IP %u.%u.%u.%u NAME ", pni->ni_ip[0]&0xff,
			pni->ni_ip[1]&0xff, pni->ni_ip[2]&0xff,
			pni->ni_ip[3]&0xff);
	else
		fprintf(stdout, "IP <invalid> NAME ");

	if (pni->ni_nvalid)
		fprintf(stdout, "\"%s\"\n", pni->ni_name);
	else
		fprintf(stdout, "<invalid>\n");

	if (pni->ni_svalid)
		fprintf(stdout, "MASK %X BROADCAST %u.%u.%u.%u\n",
			*((int *)pni->ni_mask), pni->ni_brc[0]&0xff,
			pni->ni_brc[1]&0xff, pni->ni_brc[2]&0xff,
			pni->ni_brc[3]&0xff);
	else
		fprintf(stdout, "MASK <invalid> BROADCAST <invalid>\n");

	fprintf(stdout,"MTU %d HADDR %x:%x:%x:%x:%x:%x HBCAST %x:%x:%x:%x:%x:%x\n",
		pni->ni_mtu, pni->ni_hwa.ha_addr[0]&0xff,
		pni->ni_hwa.ha_addr[1]&0xff, pni->ni_hwa.ha_addr[2]&0xff,
		pni->ni_hwa.ha_addr[3]&0xff, pni->ni_hwa.ha_addr[4]&0xff,
		pni->ni_hwa.ha_addr[5]&0xff, pni->ni_hwb.ha_addr[0]&0xff,
		pni->ni_hwb.ha_addr[1]&0xff, pni->ni_hwb.ha_addr[2]&0xff,
		pni->ni_hwb.ha_addr[3]&0xff, pni->ni_hwb.ha_addr[4]&0xff,
		pni->ni_hwb.ha_addr[5]&0xff);

	fprintf(stdout, "inq %d\n", pni->ni_ipinq);
	return(OK);
}
