/* sat_getn.c - sat_getn */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * sat_getn - do a getnext on a variable in the Address Translation Table
 *------------------------------------------------------------------------
 */
int
sat_getn(bindl, mip, numifaces)
struct snbentry		*bindl;
struct mib_info		*mip;
int					numifaces;
{
	int 	entry;
	int		iface;
	int		field;
	int		oidi;
	int		i;
	int		tmp_entry;

	if (sat_match(bindl, &iface, &entry, &field, numifaces) == SYSERR)
	{
		/*
		 *	OID was specified this way:
		 *		prefix.at.atTable.atEntry.field
		 *
		 *		Set iface to 1 (there's only 1 iface)
		 *		Get a valid entry
		 *		Go get a table entry
		 */
		if (entry == SPECIAL_CASE_AT_1)
		{
			iface = 1;
			entry = sat_findnext(-1,iface);
			goto try1;
		}
		/*
		 *	OID was specified this way:
		 *		prefix.at.atTable.atEntry.field.iface
		 *
		 *		Get a valid entry
		 *		Go get a table entry
		 */
		else if (entry == SPECIAL_CASE_AT_2)
		{
			entry = sat_findnext(-1,iface);
			goto try1;
		}

		return SERR_NO_SUCH;
	}

	for (i=1; field <= SNUMF_ATTAB && i <= numifaces; ++i)
	{
		if ((tmp_entry = sat_findnext(entry, iface)) >= 0)
		{
			entry = tmp_entry;
			break;
		}
		else
			entry = sat_findnext(-1,iface);

		if (++iface > numifaces)
		{
			iface = 1;
			++field;
		}
	}

	if (field > SNUMF_ATTAB)
		entry = tmp_entry;

try1:

	if (entry < 0)
		return((*mip->mi_next->mi_func)(bindl, mip->mi_next, SOP_GETF));


	oidi                     = SAT_OIDLEN;				/* 3.1.1 */
	bindl->sb_oid.id[oidi++] = (u_short)field;
	bindl->sb_oid.id[oidi++] = (u_short)iface;
	bindl->sb_oid.id[oidi++] = (u_short)1;

	sip2ocpy(&bindl->sb_oid.id[oidi], arptable[entry].ae_pra);

	bindl->sb_oid.len = oidi + IP_ALEN;
	
	return(sat_get(bindl, numifaces));
}
