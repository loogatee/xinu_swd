/* snfreebl.c - snfreebl */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>

/*------------------------------------------------------------------------
 * snfreebl - free memory used for ASN.1 strings and snbentry nodes
 *------------------------------------------------------------------------
 */
snfreebl(bl)
struct snbentry		*bl;
{
	register	struct snbentry		*pbl;

	if (bl == 0)
		return;

	for (pbl = bl, bl = bl->sb_next; bl; pbl = bl, bl = bl->sb_next)
	{
		xp_tracev(80,"snfreebl: 1 freemem  %d\n",pbl->sb_a1slen);
		freemem(pbl->sb_a1str, pbl->sb_a1slen);
		xp_tracev(80,"snfreebl: 2 freemem  %d\n",sizeof(struct snbentry));
		freemem(pbl, sizeof(struct snbentry));
	}
	xp_tracev(80,"snfreebl: 3 freemem  %d\n",pbl->sb_a1slen);
	freemem(pbl->sb_a1str, pbl->sb_a1slen);
	xp_tracev(80,"snfreebl: 4 freemem  %d\n",sizeof(struct snbentry));
	freemem(pbl, sizeof(struct snbentry));
}
