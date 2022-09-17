/* $Id: x_trsp.c,v 1.1 1992/11/12 23:11:59 johnr Exp $ */
/****************************************************************/
/* x_trsp.c - Set or Display the trace priority value			*/
/****************************************************************/
/*																*/
/* This routine is a Xinu shell command to Set or Display the	*/
/* current global trace priority value							*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include "conf.h"
#include "kernel.h"


/*
 *	IMPORTS
 */
extern int	xv_tracepri;

/*------------------------------------------------------------------------
 *  x_trsp  -  set or read the current trace priority value
 *------------------------------------------------------------------------
 */
COMMAND
x_trsp(stdin,stdout,stderr,nargs,args)
int		stdin;
int		stdout;
int		stderr;
int		nargs;
char	*args[];
{
	if (nargs == 2)
		xv_tracepri = atoi(args[1]);
	else if (nargs != 1)
	{
		fprintf(stdout,"Usage:   trsp [priority value]\n");
		fprintf(stdout,"Where [priority value] is a decimal integer\n");
		return;
	}

	fprintf(stdout,"current trace priority = %d\n",xv_tracepri);
}

/*
 * MODIFICATION HISTORY:
 *
 * $Log: x_trsp.c,v $
 * Revision 1.1  1992/11/12  23:11:59  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/03  21:57:54  johnr
 * JR: Initial entry for all these files.
 *
 *
 */
