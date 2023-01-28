/* initialize.c - nulluser, sysinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <sleep.h>    
#include <mem.h>
#include <tty.h>
#include <q.h>
#include <io.h>
#include <version.h>
#include <mioaddrs.h>
#include <systypes.h>

#define DETAIL

LOCAL	sysinit();
extern	int	main(), start();
extern	int	start();

/* Declarations of major kernel variables */

struct pentry	proctab[NPROC]; /* process table */
int				nextproc;		/* next process slot to use in create */
struct sentry	semaph[NSEM];	/* semaphore table */
int				nextsem;		/* next sempahore slot to use in screate */
struct qent		q[NQENT];		/* q table (see queue.c) */
int				nextqueue;		/* next slot in q structure to use */
WORD			*maxaddr;		/* max memory address (set by sizmem) */
struct mblock	memlist;		/* list of free memory blocks */
struct tty		tty[Ntty];		/* SLU buffers and mode control */

/* active system status */

int				numproc;		/* number of live user processes */
int				currpid;		/* id of currently running process */
int				reboot = 0;		/* non-zero after first boot */

int				rdyhead,rdytail;		/* head/tail of ready list */
char			vers[100];				/* Xinu version printed at startup */
int				revision = REVISION;	/* the revision level */

/* These variables were defined in usrmain. */

int				sem;
int				pid1, pid2;
int				ptid;

extern	u_long	rom;
extern	u_long	edata;
extern	u_long	ram;
extern	u_long	etext;



/************************************************************************/
/***							NOTE:							      ***/
/***																  ***/
/***   This is where the system begins after the C environment has    ***/
/***   been established.  Interrupts are initially DISABLED, and      ***/
/***   must eventually be enabled explicitly.  This routine turns     ***/
/***   itself into the null process after initialization.  Because    ***/
/***   the null process must always remain ready to run, it cannot    ***/
/***   execute code that might cause it to be suspended, wait for a   ***/
/***   semaphore, or put to sleep, or exit.  In particular, it must   ***/
/***   not do I/O unless it uses kprintf for polled output.           ***/
/***																  ***/
/************************************************************************/

/*------------------------------------------------------------------------
 *  nulluser  -- initialize system and become the null process (id==0)
 *------------------------------------------------------------------------
 */
nulluser()				/* babysit CPU when no one home */
{
        int userpid;
		long	data_size;

	xf_elnewqueue();
	xf_trnewqueue();

	init_kprintf();

	sprintf(vers, "Xinu Version %s", VERSION);
	kprintf("\n\n%s\n", vers);
	if (reboot++ < 1)
		kprintf("\n");
	else
		kprintf("   (reboot %d)\n", reboot);

	initevec();

	sysinit();

	kprintf("%d bytes real mem\n",(u_long)maxaddr+4-(u_long)&rom);
#ifdef DETAIL	
	kprintf("    0x%x", (u_long)&rom);
	kprintf(" to 0x%x\n", (u_long)maxaddr+3);
#endif	

	data_size = (u_long)&edata - (u_long)&ram;

	kprintf("%d bytes Xinu code\n",
		(u_long)((u_long)&etext -(u_long)&rom + data_size));
#ifdef DETAIL
	kprintf("    0x%x", (u_long)&rom);
	kprintf(" to 0x%x\n", (u_long)&etext-1+data_size);

	kprintf("%d bytes .text segment\n",(u_long)&etext - (u_long)&rom);
	kprintf("    0x%x", (u_long)&rom);
	kprintf(" to 0x%x\n", (unsigned long) &etext-1);

	kprintf("%d bytes rom .data segment\n",data_size);
	kprintf("    0x%x", (u_long)&etext);
	kprintf(" to 0x%x\n", (unsigned long) &etext-1+data_size);

	kprintf("%d bytes ram .data segment\n",data_size);
	kprintf("    0x%x", (u_long)&ram);
	kprintf(" to 0x%x\n", (unsigned long) &edata-1);

	kprintf("%d bytes .bss segment\n",(u_long)&end-(u_long)&edata);
	kprintf("    0x%x", (u_long)&edata);
	kprintf(" to 0x%x\n", (unsigned long) &end-1);
#endif

#ifdef DETAIL	
	kprintf("%d bytes user stack/heap space\n",
		(u_long)((u_long)maxaddr+4 - (u_long)&end));
	kprintf("    0x%x", (u_long)&end);
	kprintf(" to 0x%x\n", (u_long)maxaddr+3);
#endif	
	
	kprintf("clock %sabled\n", clkruns == 1?"en":"dis");

	enable();		/* enable interrupts */


	/* create a process to execute the user's main program */
	userpid = create(main,INITSTK,INITPRIO,INITNAME,INITARGS);

#ifdef	NETDAEMON
	/* start the network input daemon process */
	resume(
	  create(netstart, NETSTK, NETPRI, NETNAM, NETARGC, userpid)
	);
#else
	resume( userpid );
#endif
	
	while (TRUE) {		/* run forever without actually */
	    pause();		/* executing instructions */
	}
}

/*------------------------------------------------------------------------
 *  sysinit  --  initialize all Xinu data structeres and devices
 *------------------------------------------------------------------------
 */
LOCAL	sysinit()
{

	int	i,j;
	struct	pentry	*pptr;
	struct	sentry	*sptr;
	struct	mblock	*mptr;

	numproc = 0;			/* initialize system variables */
	nextproc = NPROC-1;
	nextsem = NSEM-1;
	nextqueue = NPROC;		/* q[0..NPROC-1] are processes */

	memlist.mnext = mptr =		/* initialize free memory list */
	  (struct mblock *) roundmb(&end);
	mptr->mnext = (struct mblock *)NULL;
	mptr->mlen = (int) truncew((unsigned)maxaddr
	      -NULLSTK-(unsigned)&end);
	

	for (i=0 ; i<NPROC ; i++)	/* initialize process table */
		proctab[i].pstate = PRFREE;

	pptr = &proctab[NULLPROC];	/* initialize null process entry */
	pptr->pstate = PRCURR;
	for (j=0; j<7; j++)
		pptr->pname[j] = "prnull"[j];
	pptr->plimit = (WORD)(maxaddr + 1) - NULLSTK;
	pptr->pbase = (WORD) maxaddr;
	*( (int *)pptr->pbase ) = MAGIC;
	pptr->paddr = (WORD) nulluser;
	pptr->pargs = 0;
	pptr->pprio = 0;
	pptr->pregs[SSP] = 10;
	currpid = NULLPROC;

	for (i=0 ; i<NSEM ; i++) {	/* initialize semaphores */
		(sptr = &semaph[i])->sstate = SFREE;
		sptr->sqtail = 1 + (sptr->sqhead = newqueue());
	}

	rdytail = 1 + (rdyhead=newqueue());/* initialize ready list */

	
	_mkinit();			/* initialize memory marking */

	clkinit();			/* initialize r.t.clock	*/

	for ( i=0 ; i<NDEVS ; i++ ) {
	    init(i);
	}

	return(OK);
}
