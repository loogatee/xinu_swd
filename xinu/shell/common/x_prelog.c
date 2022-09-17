/* $Id: x_prelog.c,v 1.1 1992/11/12 23:11:38 johnr Exp $ */
/****************************************************************/
/* x_prelog.c - Print the error-log buffer on stdout			*/
/****************************************************************/
/*																*/
/* This routine is a Xinu shell command which prints the		*/
/* error-log buffer on standard out								*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include "conf.h"
#include "kernel.h"
#include "systypes.h"
#include "errlog.h"

/*
 *	IMPORTS
 */
extern	elent	xv_elq[];
extern	int		xv_eltail;
extern	int		xv_elhead;

/*------------------------------------------------------------------------
 *  x_prelog  -  print out the error-log buffer
 *------------------------------------------------------------------------
 */
COMMAND
x_prelog(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	register	elent	*tptr;

	if (nargs == 2 && !strcmp(args[1],"rev"))
	{
		tptr = &xv_elq[xv_elhead];
		fprintf(stdout,"\n\n<----------------BOTTOM------------------>\n");

		while (1)
		{
			tptr = &xv_elq[tptr->el_qnext];

			if (tptr->el_qnext == -1)
			{
				fprintf(stdout,"<------------------TOP------------------->\n");
				break;
			}

			fprintf(stdout,"%08x ",tptr->el_time);
			fprintf(stdout,tptr->el_str,tptr->el_val);
		}
	}
	else
	{
		tptr = &xv_elq[xv_eltail];

		fprintf(stdout,"\n\n<------------------TOP------------------->\n");

		while (1)
		{
			tptr = &xv_elq[tptr->el_qprev];

			if (tptr->el_qprev == -1)
			{
				fprintf(stdout,"<----------------BOTTOM------------------>\n");
				break;
			}

			fprintf(stdout,"%08x ",tptr->el_time);
			fprintf(stdout,tptr->el_str,tptr->el_val);
		}
	}
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: x_prelog.c,v $
 * Revision 1.1  1992/11/12  23:11:38  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/10/22  22:07:34  johnr
 * JR: Shell command for printing the error log.
 *
 *
 */
