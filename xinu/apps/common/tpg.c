#ifndef lint
static char rcsid[] = "$Id: tpg.c,v 1.1 1993/03/09 00:25:53 johnr Exp $";
#endif

#include <conf.h>
#include <kernel.h>

int		tpg_sem;



PROCESS
testpage_d()
{
	tpg_sem = screate(0);

	while (1)
	{
		wait(tpg_sem);

		printf("\n\nTestpage Executed\n\r");

	}
}




/*
 * $Log: tpg.c,v $
 * Revision 1.1  1993/03/09  00:25:53  johnr
 * JR:  This is the testpage process without any of the spooler stuff in it.
 *      It is meant for the "standalone MIO" build, that does not include the
 *      real MIO backend or any of the spooler stuff.
 *
 *
 */
