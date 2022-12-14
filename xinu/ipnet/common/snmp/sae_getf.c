/* sae_getf.c - sae_getf */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * sae_getf - perform a getfirst on a variable in the IPAddr Entry Table
 *------------------------------------------------------------------------
 */
sae_getf(bindl, mip, numifaces)
struct snbentry		*bindl;
struct mib_info		*mip;
int					numifaces;
{
	int 	iface;
	int		oidi;

	iface = sae_findnext(-1, numifaces);

	/*
	 *	write the objid into the bindings list and call get func
	 */
	blkcopy(bindl->sb_oid.id, mip->mi_objid.id, mip->mi_objid.len*2);

	oidi                     = mip->mi_objid.len;
	bindl->sb_oid.id[oidi++] = (u_short)1;					/* field */

	sip2ocpy(&bindl->sb_oid.id[oidi], nif[iface].ni_ip);

	bindl->sb_oid.len = oidi + IP_ALEN;
	
	return(sae_get(bindl, numifaces));
}
