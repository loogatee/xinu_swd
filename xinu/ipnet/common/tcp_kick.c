/* tcpkick.c - tcpkick */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  tcpkick -  make sure we send a packet soon
 *------------------------------------------------------------------------
 */
int tcpkick(ptcb)
struct	tcb	*ptcb;
{
	int	tcbnum = ptcb - &tcbtab[0];	/* for MKEVENT() */
	int	tv;

	tv = MKEVENT(SEND, tcbnum);
	if (ptcb->tcb_flags & TCBF_DELACK && !tmleft(tcps_oport, tv))
		tmset(tcps_oport, TCPQLEN, tv, TCP_ACKDELAY);
	else if (pcount(tcps_oport) < TCPQLEN)
		psend(tcps_oport, tv);	/* send now */
	return OK;
}
