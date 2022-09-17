/* srt_match.c - srt_match */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>

/*------------------------------------------------------------------------
 * srt_match - check if a variable exists in the current Routing Table
 *------------------------------------------------------------------------
 */
int
srt_match(bindl, rtp, rtl, field)
struct snbentry	*bindl;
struct route	**rtp;
int				*rtl;
int				*field;
{
	int 	oidi;
	int		i;
	Bool	found;

	oidi = SRT_OIDLEN;

	if ((*field = bindl->sb_oid.id[oidi++]) > SNUMF_RTTAB)
		return SYSERR;

	if (bindl->sb_oid.len == 4)
	{
		*rtl = SPECIAL_CASE_RT_1;
		return SYSERR;
	}

	/*
	 *	oidi points to IP address to match in the routing table.
	 */
	for (found=FALSE, i=0; !found && i < RT_TSIZE; i++)  
	{
		for (*rtp = rttable[i]; *rtp; *rtp = (*rtp)->rt_next)
		{
			if (found=soipequ(&bindl->sb_oid.id[oidi],(*rtp)->rt_net,IP_ALEN))
				break;
		}
	}

	/*
	 *	If not there OR not end of object id
	 */
	if (!found || oidi + IP_ALEN != bindl->sb_oid.len)
		return SYSERR;

	*rtl = i - 1;

	return OK;
}
