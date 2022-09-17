/* x_arp.c - x_arp */

#include <conf.h>
#include <kernel.h>

static parp(), xarpusage();

/*------------------------------------------------------------------------
 *  x_arp  -  print ARP tables
 *------------------------------------------------------------------------
 */
COMMAND	x_arp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	if (nargs != 1)
	{
		if (nargs == 2)
		{
			if (strcmp(args[1],"flush") == 0)
			{
				arpflush();
				return OK;
			}
		}

		return xarpusage(stderr);
	}
	arpprint(stdout);
	return OK;
}


static
xarpusage(stderr)
{
	fprintf(stderr,"usage: arp [flush]\n");
	return OK;
}
