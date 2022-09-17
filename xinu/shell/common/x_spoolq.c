/* x_spoolq.c - x_spoolq */

#include "conf.h"
#include "kernel.h"

/*------------------------------------------------------------------------
 *  x_spoolq  -  (command spoolq) show whats in the spooler
 *------------------------------------------------------------------------
 */
COMMAND	x_spoolq(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char	str[80];
	int	i;

	spool_stats(stdout);

	return(OK);
}
