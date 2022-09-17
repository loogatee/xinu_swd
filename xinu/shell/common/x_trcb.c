/* $Id: x_trcb.c,v 1.1 1992/11/12 23:11:55 johnr Exp $ */
/****************************************************************/
/* x_trcb.c - Clear out the trace buffer						*/
/****************************************************************/
/*																*/
/* This routine is a Xinu shell command to clear out the		*/
/* trace buffer.												*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include "conf.h"
#include "kernel.h"

/*------------------------------------------------------------------------
 *  x_trcb  -  clear the trace buffer
 *------------------------------------------------------------------------
 */
COMMAND
x_trcb(stdin,stdout,stderr,nargs,args)
int		stdin;
int		stdout;
int		stderr;
int		nargs;
char	*args[];
{
	STATWORD	pc;

	disable(pc);
	xf_trnewqueue();
	restore(pc);

	fprintf(stdout,"the trace buffer has been re-initialized\n");
}

/*
 * MODIFICATION HISTORY:
 *
 * $Log: x_trcb.c,v $
 * Revision 1.1  1992/11/12  23:11:55  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/03  21:57:49  johnr
 * JR: Initial entry for all these files.
 *
 *
 */
