/* $Id: x_trkp.c,v 1.1 1992/11/12 23:11:56 johnr Exp $ */
/****************************************************************/
/* x_trkp.c - set, get, or return the trace kprintf-flag		*/
/****************************************************************/
/*																*/
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
extern int	xv_trkpflag;


COMMAND
x_trkp(stdin,stdout,stderr,nargs,args)
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
		fprintf(stdout,"Usage:  trkp < \"on\" | \"off\" >\n");
		return;
	}

	if (type == 0)
	{
		if (strcmp(args[1],"on") == 0)
			xv_trkpflag = 1;
		else if (strcmp(args[1],"off") == 0)
			xv_trkpflag = 0;
		else
		{
			fprintf(stdout,"Usage:  trkp < \"on\" | \"off\" >\n");
			return;
		}
	}
	else
	{
		if (xv_trkpflag == 1)
			fprintf(stdout,"trace kprintf-flag is on\n");
		else
			fprintf(stdout,"trace kprintf-flag is off\n");
	}
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: x_trkp.c,v $
 * Revision 1.1  1992/11/12  23:11:56  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/03  21:57:51  johnr
 * JR: Initial entry for all these files.
 *
 *
 */
