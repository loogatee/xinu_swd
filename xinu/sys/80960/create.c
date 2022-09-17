#ifndef lint
static char rcsid[] = "$Id: create.c,v 1.1 1992/11/10 21:39:53 johnr Exp $";
#endif
/* create.c - xp_create, xf_newpid */

#include <conf.h>
#include <st.std.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <xmem.h>
#include <io.h>
#include <i960.h>

extern	bit32	XC_INITRET();
extern	WORD	*xp_getstk();


/*------------------------------------------------------------------------
 * xf_newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
XC_LOCAL
xf_newpid()
{
	register	int		pid;			/* process id to return		*/
	register	int		i;

	for (i=0; i < XC_NPROC; ++i)
	{
		if ((pid = xv_nextproc--) <= 0)
			xv_nextproc = XC_NPROC - 1;

		if (xv_proctab[pid].pstate == XC_PRFREE)
			return(pid);
	}

	return(XC_SYSERR);
}


/*------------------------------------------------------------------------
 *  xp_create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
XC_SYSCALL
xp_create(procaddr,ssize,priority,name,nargs,x0,x1,x2,x3,x4,x5,x6)
int		*procaddr;				/* procedure address */
int		ssize;					/* stack size in words */
int		priority;				/* process priority > 0 */
char	*name;					/* name (for debugging) */
int		nargs;					/* number of args that follow */
bit32	x0,x1,x2,x3,x4,x5,x6;	/* the arguments */
{
				PCWORD			ps;				/* saved processor status */
	register	int				pid;			/* stores new process id */
	register	struct pentry	*pptr;			/* ptr to proc. table entry */
	register	int				i;
	register	WORD			*saddr;			/* stack address */
	register	bit32			*stack0;
	register	bit32			*stack1;

	xa_disable(ps);

	ssize = (int)xm_roundew(ssize);

/*
	if (ssize < XC_MINSTK || ((saddr=xp_getstk(ssize)) == (WORD *)XC_SYSERR) ||
			(pid=xf_newpid()) == XC_SYSERR || xm_isodd(procaddr) ||
			priority < 1 || nargs > 7)
	{
		xa_restore(ps);
		return(XC_SYSERR);
	}
*/

	if (ssize < XC_MINSTK)
	{
		xp_kprintf("xp_create: ERROR - ssize < MC_MINSTK\n");
		xa_restore(ps);
		return(XC_SYSERR);
	}

	if ((saddr=xp_getstk(ssize)) == (WORD *)XC_SYSERR)
	{
		xp_kprintf("xp_create: ERROR - getstk() returned an error\n");
		xa_restore(ps);
		return(XC_SYSERR);
	}

	if ((pid=xf_newpid()) == XC_SYSERR)
	{
		xp_kprintf("xp_create: ERROR - newpid() returned an error\n");
		xa_restore(ps);
		return(XC_SYSERR);
	}

	if (xm_isodd(procaddr))
	{
		xp_kprintf("xp_create: ERROR - procaddr is odd\n");
		xa_restore(ps);
		return(XC_SYSERR);
	}

	if (priority < 1)
	{
		xp_kprintf("xp_create: ERROR - priority < 1\n");
		xa_restore(ps);
		return(XC_SYSERR);
	}

	if (nargs > 7)
	{
		xp_kprintf("xp_create: ERROR - nargs > 7   nargs = %d\n",nargs);
		xa_restore(ps);
		return(XC_SYSERR);
	}





	xv_numproc++;
	pptr = &xv_proctab[pid];

	pptr->fildes[0] = 0;	/* stdin set to console */
	pptr->fildes[1] = 0;	/* stdout set to console */
	pptr->fildes[2] = 0;	/* stderr set to console */

	for (i=3; i < XC_NFILE; i++)	/* others set to unused */
		pptr->fildes[i] = XC_FDFREE;

	for (i=0; i < XC_PNMLEN && (int)(pptr->pname[i] = name[i]) != 0; i++)
		;

	pptr->pstate    = XC_PRSUSP;
	pptr->pprio     = priority;
	pptr->pbase     = (WORD)saddr;
	pptr->pstklen   = ssize;
	pptr->psem      = 0;
	pptr->phasmsg   = XC_FALSE;
	pptr->plimit    = pptr->pbase + ssize - sizeof(WORD);	
	*saddr          = XC_MAGIC;
	pptr->pargs     = nargs;
	pptr->pnxtkin   = XC_BADPID;
	pptr->pdevs[0]  = pptr->pdevs[1] = XC_BADDEV;
	pptr->paddr     = (WORD)procaddr;

	for (i=0; i < XC_PNREGS; i++)
		pptr->pregs[i] = XC_INITREG;

	pptr->pregs[XC_RIP] = 0;
	pptr->pregs[XC_PC]  = PC_INITPC;

	/*
	 *	This next assignment may be misleading.  The SP that is initially
	 *		loaded isn't really used for anything.  A good value is
	 *		written here so that the first reschedule for this process
	 *		will get by the stack checks.
	 */
	pptr->pregs[XC_SP]  = (WORD)saddr + 0x40;

	/*
	 *	Set up stack stuff
	 */
	stack0        = (bit32 *)(((WORD)saddr + 0x10) & ~0xf);
	*(stack0)     = 0;									/* pfp */
	*(stack0 + 1) =	(unsigned)stack0 + 64;				/* sp */
	*(stack0 + 2) =	(bit32)XC_INITRET;					/* rip */

	stack1        = (bit32 *)((unsigned)stack0 + 80);
	*(stack1)     = (bit32)stack0;						/* pfp */
	*(stack1 + 1) = (unsigned)stack1 + 64;				/* sp */
	*(stack1 + 2) = (bit32)procaddr;					/* rip */

	pptr->pregs[XC_PFP] = (WORD)stack1;

	pptr->pregs[0] = x0;
	pptr->pregs[6] = x6;
	pptr->pregs[4] = x4;
	pptr->pregs[1] = x1;
	pptr->pregs[3] = x3;
	pptr->pregs[5] = x5;
	pptr->pregs[2] = x2;

	xa_restore(ps);
	return(pid);
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: create.c,v $
 * Revision 1.1  1992/11/10  21:39:53  johnr
 * JR: 1st entry for all these files.
 *
 * Revision 1.2  1992/04/21  17:34:54  johnr
 * moved newpid up to the beginning of the file
 *
 * Revision 1.1.1.1  1991/11/12  02:20:23  johnr
 * JRs original Xinu distribution
 *
 * Revision 1.1  1991/11/12  02:20:22  johnr
 * Initial revision
 *
 *
 */
