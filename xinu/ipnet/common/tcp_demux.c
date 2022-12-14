/* tcpdemux.c - tcpdemux */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpdemux -  do TCP port demultiplexing
 *------------------------------------------------------------------------
 */
struct tcb *tcpdemux(pep)
struct	ep	*pep;
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	tcp	*ptcp = (struct tcp *)pip->ip_data;
	struct	tcb	*ptcb;
	int		tcbn, lstcbn;

	wait(tcps_tmutex);
	for (tcbn=0, lstcbn = -1; tcbn<Ntcp; ++tcbn) {
		if (tcbtab[tcbn].tcb_state == TCPS_FREE)
			continue;
		if (ptcp->tcp_dport == tcbtab[tcbn].tcb_lport &&
		    ptcp->tcp_sport == tcbtab[tcbn].tcb_rport &&
		    blkequ(pip->ip_src, tcbtab[tcbn].tcb_rip, IP_ALEN) &&
		    blkequ(pip->ip_dst, tcbtab[tcbn].tcb_lip, IP_ALEN)) {
			break;
		}
		if (tcbtab[tcbn].tcb_state == TCPS_LISTEN &&
		    ptcp->tcp_dport == tcbtab[tcbn].tcb_lport)
			lstcbn = tcbn;
	}
	if (tcbn >= Ntcp)
		if (ptcp->tcp_code & TCPF_SYN)
			tcbn =  lstcbn;
		else
			tcbn = -1;
	signal(tcps_tmutex);
	if (tcbn < 0)
		return 0;
	wait(tcbtab[tcbn].tcb_mutex);
	if (tcbtab[tcbn].tcb_state == TCPS_FREE)
		return 0;			/* OOPS! Lost it... */
	return &tcbtab[tcbn];
}
