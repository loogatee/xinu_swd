/* tcpinp.c - tcpinp */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpinp  -  handle TCP segment coming in from IP
 *------------------------------------------------------------------------
 */
PROCESS tcpinp()
{
	struct	ep	*pep;
	struct	ip	*pip;
	struct	tcp	*ptcp;
	struct	tcb	*ptcb;

	tcps_iport = pcreate(TCPQLEN);
	signal(Net.sema);
	while (TRUE) {
		pep = (struct ep *)preceive(tcps_iport);
		if ((unsigned)pep == SYSERR)
			break;
		pip = (struct ip *)pep->ep_data;
		if (tcpcksum(pip)) {
			freebuf(pep);
			continue;
		}
		ptcp = (struct tcp *)pip->ip_data;
		tcpnet2h(ptcp); /* convert all fields to host order */
		ptcb = (struct tcb *)tcpdemux(pep);
		if (ptcb == 0) {
			tcpreset(pep);
			freebuf(pep);
			continue;
		}
		if (!tcpok(ptcb, pep))
			tcpackit(ptcb, pep);
		else {
			tcpopts(ptcb, pep);
			tcpswitch[ptcb->tcb_state](ptcb, pep);
		}
		if (ptcb->tcb_state != TCPS_FREE)
			signal(ptcb->tcb_mutex);
		freebuf(pep);
	}
}

int tcps_oport, tcps_iport, tcps_lqsize, tcps_tmutex;
