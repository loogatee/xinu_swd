/* $Id: x_enttyb.c,v 1.1 1992/11/12 23:11:22 johnr Exp $ */
/* x_enttyb.c - x_enttyb */

#include "conf.h"
#include "kernel.h"
#include "tty.h"
#include "slu.h"
#include "zsreg.h"

/*------------------------------------------------------------------------
 *  x_ttyb  -  blast output test on port ttyb
 *------------------------------------------------------------------------
 */
COMMAND	x_enttyb(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
				struct tty		*tty_ptr;
				struct devsw	*devptr;
	volatile	char			*regptr;

	tty_ptr = &tty[1];
	devptr  = &devtab[1];
	regptr  = (char *)devptr->dvcsr;

	*regptr = 1;			/* register 1 */
	DELAY(2);
	*regptr = ZSWR1_SIE | ZSWR1_TIE | ZSWR1_RIE;		/* enable interrupts */
	DELAY(2);
	
}


/*
 * $Log: x_enttyb.c,v $
 * Revision 1.1  1992/11/12  23:11:22  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/05  17:00:07  johnr
 * JR: Initial entries for both these files.  From the 7.1 code.
 *
 * Revision 1.1  1992/07/09  17:15:02  johnr
 * JR: This shell command turns on transmit and receive interrupts for ttyb
 *
 *
 */
