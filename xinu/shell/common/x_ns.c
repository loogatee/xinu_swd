/* x_ns.c - x_ns */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_ns  -  (command ns) interact with the host name server cache
 *------------------------------------------------------------------------
 */
COMMAND	x_ns(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char	str[128], ipstr[64];
	IPaddr	ip;
	int	ac;

	for (ac=1; ac < nargs; ++ac) {
		if (name2ip(ip, args[ac]) == SYSERR)
			strcpy(ipstr, "not resolved");
		else
			sprintf(ipstr, "%u.%u.%u.%u", BYTE(ip, 0), BYTE(ip, 1),
				BYTE(ip, 2), BYTE(ip, 3));
		sprintf(str, "\"%s\": %s\n", args[ac], ipstr);
		fprintf(stdout, "%s", str);
	}
	return OK;
}
