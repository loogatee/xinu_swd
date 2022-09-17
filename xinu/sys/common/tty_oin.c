/* ttyoin.c - ttyoin */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <sem.h>

/*------------------------------------------------------------------------
 *  ttyoin  --  lower-half tty device driver for output interrupts
 *------------------------------------------------------------------------
 */
INTPROC	ttyoin(devm)
	long	devm;
{
	register struct	tty   *iptr;
	STATWORD ps;    
	int	ct;

	disable(ps);	

	iptr = &tty[devm];

	if (iptr->ehead	!= iptr->etail)	{
		ttyoin_ssp0(iptr);
		if (iptr->etail	>= EBUFLEN)
			iptr->etail = 0;
		restore(ps);
		return;
	}
	if (iptr->oheld) {			/* honor flow control	*/
		ttyostop(iptr);
		restore(ps);
		return;
	}
	if ((ct=semaph[iptr->osem].semcnt) < OBUFLEN) {
		ttyoin_ssp1(iptr);
		if (iptr->otail	>= OBUFLEN)
			iptr->otail = 0;
		if (ct > OBMINSP)
			signal(iptr->osem);
		else if	( ++(iptr->odsend) == OBMINSP) {
			iptr->odsend = 0;
			signaln(iptr->osem, OBMINSP);
		}
	} else
	        ttyostop(iptr);

	restore(ps);
}
