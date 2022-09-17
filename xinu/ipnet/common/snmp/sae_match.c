/* sae_match.c - sae_match */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>
#include <snhash.h>

/*------------------------------------------------------------------------
 * sae_match - check if a variable exists in the IP Address Entry Table
 *------------------------------------------------------------------------
 */
int
sae_match(bindl, iface, field, entry, numifaces)
struct snbentry		*bindl;
int					*iface;
int					*field;
int					*entry;
int					numifaces;
{
	int 	oidi;

	oidi = SAE_OIDLEN;	/* skip over fixed part of objid */

	if ((*field = bindl->sb_oid.id[oidi++]) > SNUMF_AETAB)
		return SYSERR;

	if (bindl->sb_oid.len == 4)
	{
		*entry = SPECIAL_CASE_AE_1;
		return SYSERR;
	}

	for (*iface=1; *iface <= numifaces; (*iface)++)
	{
		if (soipequ(&bindl->sb_oid.id[oidi], nif[*iface].ni_ip, IP_ALEN))
			break;
	}

	if (*iface > numifaces)
		return SYSERR;

	oidi += IP_ALEN;

	/*
	 *	verify oidi at end of objid
	 */
	if (oidi != bindl->sb_oid.len)
		return SYSERR;

	return OK;
}
