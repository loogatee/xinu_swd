/* x_dg.c - x_dg */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_dg  -  (command dg) print info for currently open datagram devices
 *------------------------------------------------------------------------
 */
COMMAND
x_dg(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	struct	dgblk	*pdg;
	char	str[80];
	int	i;

	for (i=0 ; i<Ndg ; i++) {
		pdg = &dgtab[i];
		if (pdg->dg_state == DGS_FREE)
			continue;
		sprintf(str, "Dev=%2d: lport=%4d, fport=%4d, ",
			pdg->dg_dnum, pdg->dg_lport, pdg->dg_fport);

		sprintf(&str[strlen(str)],
			"mode=%03o, xport=%2d addr=%d.%d.%d.%d\n",
				pdg->dg_mode, pdg->dg_xport,
				pdg->dg_fip[0] &0377,
				pdg->dg_fip[1] &0377,
				pdg->dg_fip[2] &0377,
				pdg->dg_fip[3] &0377);
		fprintf(stdout, "%s", str);
	}
	return(OK);
}
