/* ni_in.c - ni_in */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ni_in - network interface input function
 *------------------------------------------------------------------------
 */
int ni_in(pni, pep, len)
struct	netif	*pni;		/* the interface	*/
struct	ep	*pep;		/* the packet		*/
int		len;		/* length, in octets	*/
{
	int	rv;

	switch (pep->ep_type) {
	case EPT_ARP:	rv = arp_in(pni, pep);	break;
	case EPT_RARP:	rv = rarp_in(pni, pep);	break;
	case EPT_IP:	rv = ip_in(pni, pep);	break;
	default:
		pni->ni_iunkproto++;
		freebuf(pep);
		return OK;
	}
	pni->ni_ioctets += len;
	if (blkequ(pni->ni_hwa.ha_addr, pep->ep_dst, EP_ALEN))
		pni->ni_iucast++;
	else
		pni->ni_inucast++;
	return rv;
}
