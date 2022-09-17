/* srt_getn.c - srt_getn */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

struct route	*srt_findnext();

/*------------------------------------------------------------------------
 * srt_getn - perform a getnext on a variable in the Routing Table
 *------------------------------------------------------------------------
 */
int
srt_getn(bindl, mip, numifaces)
struct snbentry		*bindl;
struct mib_info		*mip;
int					numifaces;
{
	struct route	*rtp;
	int				rtl;
	int				field;
	int				oidi;

	if (srt_match(bindl, &rtp, &rtl, &field) == SYSERR)
	{
		if (rtl == SPECIAL_CASE_RT_1)
		{
			rtp = (struct route *) NULL;
			rtl = 0;					/* set route hash table list to 0 */
			rtp = srt_findnext(rtp, &rtl);
			goto try1;
		}

		return SERR_NO_SUCH;
	}

	if ((rtp = srt_findnext(rtp, &rtl)) == 0)
	{
		rtp = (struct route *) NULL;
		rtl = 0;						/* set route hash table list to 0 */
		rtp = srt_findnext(rtp, &rtl);

		if (++field > SNUMF_RTTAB) 
			return((*mip->mi_next->mi_func)(bindl, mip->mi_next, SOP_GETF));
	}

try1:

	oidi = SRT_OIDLEN;	/* 4.21.1 */

	bindl->sb_oid.id[oidi++] = field;
	sip2ocpy(&bindl->sb_oid.id[oidi], rtp->rt_net, IP_ALEN);
	bindl->sb_oid.len = oidi + IP_ALEN;

	return srt_get(bindl, numifaces);
}
