#ifndef lint
static char rcsid[] = "$Id: resched.c,v 1.1 1992/11/10 21:40:01 johnr Exp $";
#endif
/* resched.c  -  xf_resched */

#include <conf.h>
#include <st.std.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <i960.h>
#include <tracev.h>

extern	bit32	xv_rescflag;
static	bit32	cspx;

/*------------------------------------------------------------------------
 * xf_resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, xv_currpid gives current process id.
 *			xv_proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than XC_PRREADY.
 *------------------------------------------------------------------------
 */
int	xf_resched()
{
	register	bit32			ps;		/* process controls reg */
	register	bit32			csp;	/* current stack pointer */
	register	struct	pentry	*optr;	/* pointer to old process entry */
	register	struct	pentry	*nptr;	/* pointer to new process entry */

	ps = xa_getpc();

	if (ps & PC_STATE)	/* if != 0, interrupted state */
	{
		xv_rescflag = 1;
		return;
	}

	/*
	 *	No switch is needed if the current process priority
	 *		is higher than next.
	 */
	optr = &xv_proctab[xv_currpid];
	if ((optr->pstate == XC_PRCURR) && (xm_lastkey(xv_rdytail) < optr->pprio))
		return(XC_OK);

/*	tracev(trv_xinu,"xf_resched:  xv_currpid   = %d\n",xv_currpid); */

#ifdef XC_STKCHK
	csp = xa_getsp();

	if ((csp > ((unsigned)optr->plimit)) || (csp < ((unsigned)optr->pbase)))
	{
		xp_kprintf("Bad SP pid=%d (%s), lim=0x%lx will be 0x%lx\n",
				xv_currpid,optr->pname,(bit32)optr->plimit,csp);
		xp_panic("current process stack overflow");
	}
#endif	

	/*
	 *	Force a context switch.
	 */
	if (optr->pstate == XC_PRCURR)
	{
		optr->pstate = XC_PRREADY;
		xf_insert(xv_currpid,xv_rdyhead,optr->pprio);
	}

	/*
	 *	Remove the highest priority process at the end of the
	 *		ready list, and mark it as currently running.
	 */
	nptr         = &xv_proctab[(xv_currpid = xf_getlast(xv_rdytail))];
	nptr->pstate = XC_PRCURR;

/*	tracev(trv_xinu,"xf_resched:  newpid   = %d\n",xv_currpid); */

#ifdef	XC_STKCHK
	if (*((bit32 *)nptr->pbase) != XC_MAGIC)
	{
		xp_kprintf("BAD MAGIC pid=%d (%s), value=0x%lx, at 0x%lx\n",xv_currpid,
			nptr->pname,*((bit32 *)nptr->pbase),(bit32)nptr->pbase);
		xp_panic("stack corrupted");
	}

	csp = (bit32)nptr->pregs[XC_SP];

	if ((csp > ((unsigned)nptr->plimit)) || (csp < ((unsigned)nptr->pbase)))
	{
		xp_kprintf("Bad SP pid=%d (%s), lim=0x%lx will be 0x%lx\n",
				xv_currpid,nptr->pname,(bit32)nptr->plimit,csp);
		xp_panic("new process stack overflow");
	}
#endif

	xv_preempt = XC_QUANTUM;		/* reset preemption counter	*/

	xa_ctxsw(optr->pregs,nptr->pregs);
	
	/* The OLD process returns here when resumed. */

	return(XC_OK);
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: resched.c,v $
 * Revision 1.1  1992/11/10  21:40:01  johnr
 * JR: 1st entry for all these files.
 *
 * Revision 1.1.1.1  1991/11/12  02:20:41  johnr
 * JRs original Xinu distribution
 *
 * Revision 1.1  1991/11/12  02:20:40  johnr
 * Initial revision
 *
 *
 */
