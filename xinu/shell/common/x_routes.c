/* x_routes.c - x_routes */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_routes  -  (command routes) format and print routing entries
 *------------------------------------------------------------------------
 */
COMMAND	x_routes(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char netname[64], gwname[64];
	char str[80];
	struct route *srt;
	int	i, usedots;

	if (nargs > 2) {
		fprintf (stdout, "usage: routes [-n]\n");
		return OK;
	}
	usedots = nargs == 2;
	
	fprintf(stdout, "%-16s %-8s %-16s metric intf ttl refcnt usecnt\n",
			"net", "mask", "gateway");

	for(i=0; i<RT_TSIZE; ++i) {
		for (srt=rttable[i]; srt != NULL; srt=srt->rt_next) {

			if (usedots) {
				ip2dot(netname, srt->rt_net);
				ip2dot(gwname, srt->rt_gw);
			} else {
				ip2name(srt->rt_net, netname);
				ip2name(srt->rt_gw, gwname);
			}
			netname[16] = gwname[16] = '\0';
			fprintf(stdout, "%-16s %08x %-16s %6d   %1d  ",
				netname, *((int *)srt->rt_mask), gwname,
				srt->rt_metric, srt->rt_ifnum);

			if (srt->rt_ttl >= RT_INF)
				fprintf(stdout, " -  %6d %6d\n",
					srt->rt_refcnt, srt->rt_usecnt);
			else
				fprintf(stdout, "%3d %6d %6d\n", srt->rt_ttl,
					srt->rt_refcnt, srt->rt_usecnt);
		}
	}
	if (Route.ri_default) {
		srt = Route.ri_default;
		if (usedots) {
			ip2dot(netname, srt->rt_net);
			ip2dot(gwname, srt->rt_gw);
		} else {
			ip2name(srt->rt_net, netname);
			ip2name(srt->rt_gw, gwname);
		}
		netname[16] = gwname[16] = '\0';
		fprintf(stdout, "%-16s %08x %-16s %6d   %1d  ",
			netname, *((int *)srt->rt_mask), gwname,
			srt->rt_metric, srt->rt_ifnum);

		if (srt->rt_ttl >= RT_INF)
			fprintf(stdout, " -  %6d %6d\n",
				srt->rt_refcnt, srt->rt_usecnt);
		else
			fprintf(stdout, "%3d %6d %6d\n", srt->rt_ttl,
				srt->rt_refcnt, srt->rt_usecnt);
	}
}
