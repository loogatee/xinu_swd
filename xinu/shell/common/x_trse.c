/* $Id: x_trse.c,v 1.1 1992/11/12 23:11:58 johnr Exp $ */
/****************************************************************/
/* trse.c - set, get, or return the trace exclusive flag		*/
/****************************************************************/
/*																*/
/* This file contains a Xinu shell command which sets, gets,	*/
/* or returns the current setting of the trace exclusive flag.	*/
/* The exclusive flag controls the placement of tracev			*/
/* statements in the trace buffer.  With the exlusive flag on,	*/
/* only those statements with a priority EQUAL TO the global	*/
/* trace priority (xv_tracepri) will be placed into the buffer.	*/
/* With the exclusive flag off, tracev statements are placed	*/
/* in the buffer when the priority is LESS THAN or EQUAL TO the	*/
/* global trace priority.										*/
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
extern int	xv_exclusive;


COMMAND
x_trse(stdin,stdout,stderr,nargs,args)
int		stdin;
int		stdout;
int		stderr;
int		nargs;
char	*args[];
{
	int		type;

	type = 0;

	if (nargs == 1)
		type = 1;
	else if (nargs != 2)
	{
		fprintf(stdout,"Usage:  trse < \"on\" | \"off\" >\n");
		return;
	}

	if (type == 0)
	{
		if (strcmp(args[1],"on") == 0)
			xv_exclusive = 1;
		else if (strcmp(args[1],"off") == 0)
			xv_exclusive = 0;
		else
		{
			fprintf(stdout,"Usage:  trse < \"on\" | \"off\" >\n");
			return;
		}
	}
	else
	{
		if (xv_exclusive == 1)
			fprintf(stdout,"exclusive flag is on\n");
		else
			fprintf(stdout,"exclusive flag is off\n");
	}
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: x_trse.c,v $
 * Revision 1.1  1992/11/12  23:11:58  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/03  21:57:52  johnr
 * JR: Initial entry for all these files.
 *
 *
 */
