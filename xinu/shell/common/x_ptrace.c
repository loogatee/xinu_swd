/* $Id: x_ptrace.c,v 1.1 1992/11/12 23:11:41 johnr Exp $ */
/****************************************************************/
/* x_ptrace.c - Print the trace buffer on stdout				*/
/****************************************************************/
/*																*/
/* This routine is a Xinu shell command which prints the		*/
/* trace buffer on standard out									*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include "conf.h"
#include "kernel.h"
#include "systypes.h"
#include "trace.h"

/*
 *	IMPORTS
 */
extern	trent	xv_trq[];
extern	int		xv_trtail;
extern	int		xv_trhead;
extern	int		tr_enqflag;

/*------------------------------------------------------------------------
 *  x_ptrace  -  print out the trace buffer
 *------------------------------------------------------------------------
 */
COMMAND
x_ptrace(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	register	trent	*tptr;

	tr_enqflag = 1;

	if (nargs == 2 && !strcmp(args[1],"rev"))
	{
		tptr = &xv_trq[xv_trhead];
		fprintf(stdout,"\n\n<----------------BOTTOM------------------>\n");

		while (1)
		{
			tptr = &xv_trq[tptr->qnext];

			if (tptr->qnext == -1)
			{
				fprintf(stdout,"<------------------TOP------------------->\n");
				break;
			}

			fprintf(stdout,"%08x ",tptr->time);
			fprintf(stdout,tptr->str,tptr->val);
		}
	}
	else
	{
		tptr = &xv_trq[xv_trtail];

		fprintf(stdout,"\n\n<------------------TOP------------------->\n");

		while (1)
		{
			tptr = &xv_trq[tptr->qprev];

			if (tptr->qprev == -1)
			{
				fprintf(stdout,"<----------------BOTTOM------------------>\n");
				break;
			}

			fprintf(stdout,"%08x ",tptr->time);
			fprintf(stdout,tptr->str,tptr->val);
		}
	}

	tr_enqflag = 0;
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: x_ptrace.c,v $
 * Revision 1.1  1992/11/12  23:11:41  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/03  21:57:55  johnr
 * JR: Initial entry for all these files.
 *
 *
 */
