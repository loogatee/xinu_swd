/* x_who.c - x_who */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <date.h>
#include <shell.h>
/* #include <network.h> */

/*------------------------------------------------------------------------
 *  x_who  -  (command who) print name user is logged in under
 *------------------------------------------------------------------------
 */
COMMAND	x_who(stdin, stdout, stderr)
int	stdin, stdout, stderr;
{
	char	machine[64];
	char	str[80];
	int	i;
	struct shvars	*shptr;

	i = proctab[getpid()].pshdev;
	shptr = &Shl[i];

	if (marked(shptr->shmark) && shptr->shused) {
		getname(machine);
		if (i == 0)
			fprintf(stdout, "%10s!%-10s Console : ", machine, shptr->shuser);
		else
			fprintf(stdout, "%10s!%-10s Network : ", machine, shptr->shuser);

		ascdate(shptr->shlogon, str);
		strcat(str, "\n");
		fprintf(stdout, "%s", str);
	}
	return(OK);
}
