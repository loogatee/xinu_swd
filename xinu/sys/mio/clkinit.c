/* clkinit.c - clkinit, updateleds, dog_timeout */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <ivect.h>
#include <mioaddrs.h>

typedef unsigned long	ulong;
typedef unsigned char	uchar;

extern	void		rtcintr();

/* real-time clock variables and sleeping process queue pointers	*/
    
int		count10;		/* counts in 10ths of second 10-0	*/
int		count100;		/* counts in 100ths of second 10-0	*/
ulong	ticks_100ms;	/* counts in 10ths of second 0-INF	*/
ulong	ticks_10ms;		/* counts in 100ths of second 0-INF	*/
int		clmutex;		/* mutual exclusion for time-of-day	*/
long	clktime;		/* current time in seconds since 1/1/70	*/
int     defclk;			/* non-zero, then deferring clock count */
int     clkdiff;		/* deferred clock ticks			*/
int     slnempty;		/* FALSE if the sleep queue is empty	*/
int     *sltop;			/* address of key part of top entry in	*/
						/* the sleep queue if slnonempty==TRUE	*/
int     clockq;			/* head of queue of sleeping processes  */
int		preempt;		/* preemption counter.	Current process */
						/* is preempted when it reaches zero;	*/
						/* set in resched; counts in ticks	*/
int		clkruns;		/* set TRUE iff clock exists by setclkr	*/

uchar	miscport_ram;

c_clockisr()
{
	volatile	register	uchar	*mp_ptr;		/* misc port pointer */

	++ticks_10ms;

	/*
	 *	Every 10 ticks is 1/10th of a second.
	 */
	if (++count100 == 10)
		count100 = 0;
	else
		return;

	if (++count10 == 10)
	{
		count10 = 0;
		++clktime;
	}

	/*
	 *	Blink the green led.
	 */
	mp_ptr = (uchar *)IOREG;
	if (count10 & 1)
	{
		miscport_ram = miscport_ram & 0xfe;
		miscport_ram = miscport_ram | 2;
		*mp_ptr      = miscport_ram;
	}
	else
	{
		miscport_ram = miscport_ram | 1;
		*mp_ptr      = miscport_ram;
	}

	if (slnempty == TRUE)
	{
		if (--(*sltop) == 0)
			wakeup();
	}

	if (--preempt == 0)
		resched();
}

/*
 *------------------------------------------------------------------------
 * clkinit - initialize the clock and sleep queue (called at startup)
 *------------------------------------------------------------------------
 */
clkinit()
{
	ulong	*sys_ptr;	/* pointer to system address table */
	ulong	(*fptr)();	/* pointer to a function */
	ulong	bavecs;		/* base address of interrupt vectors */
	ulong	l1iv_addr;	/* address of level 1 interrupt vector */

	volatile unsigned char	*mp_ptr;

	count100     = 0;
	count10      = 0;
	clkruns      = 1;
	ticks_100ms  = 0;
	clockq       = newqueue();
	preempt      = QUANTUM;		/* initial time quantum		*/
	miscport_ram = 0;

	sys_ptr   = (ulong *)ADDR_SAT;
	fptr      = (ulong(*)())*sys_ptr;	/* assign function pointer */
	bavecs    = (*fptr)();			/* get base address */
	l1iv_addr = bavecs + L1IV + JMPOFF;	/* calculate address */

	*((ulong *)l1iv_addr) = (ulong)rtcintr;

	mp_ptr = (unsigned char *)IOREG;

	l1iv_addr = *mp_ptr;
}


xf_timestamp()
{
	return(ticks_10ms);
}
