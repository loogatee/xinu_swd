/* sae_getn.c - sae_getn */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * sae_getn - perform a getnext on a variable in the IPAddr Entry Table
 *------------------------------------------------------------------------
 */
int
sae_getn(bindl, mip, numifaces)
struct snbentry		*bindl;
struct mib_info		*mip;
int					numifaces;
{
	int 	field;
	int		iface;
	int		oidi;
	int		entry;

	xp_tracev(80,"sae_getn: entering\n",0);

	entry = 0;

	if (sae_match(bindl, &iface, &field, &entry, numifaces) == SYSERR)
	{
		if (entry == SPECIAL_CASE_AE_1)
		{
			iface = 1;
			goto try1;
		}

		xp_tracev(80,"sae_getn: returning SERR_NO_SUCH\n",0);
		return SERR_NO_SUCH;
	}

	if ((iface = sae_findnext(iface, numifaces)) == -1)
	{
		iface = sae_findnext(-1, numifaces);	
		if (++field > SNUMF_AETAB) 
			return((*mip->mi_next->mi_func)(bindl, mip->mi_next, SOP_GETF));
	}

try1:

	/*
	 *	The fixed part of the objid is correct. Update the rest
	 */
	oidi                     = SAE_OIDLEN;
	bindl->sb_oid.id[oidi++] = (u_short)field;

	sip2ocpy(&bindl->sb_oid.id[oidi], nif[iface].ni_ip);

	bindl->sb_oid.len = oidi + IP_ALEN;

	return(sae_get(bindl, numifaces));
}
