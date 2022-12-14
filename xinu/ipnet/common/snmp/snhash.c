/* snhash.c - getmib, hashoid, hashinit */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <snhash.h>


		struct snhnode		*snhtab[S_HTABSIZ];
extern	struct tab_info		tabtab[];

/*------------------------------------------------------------------------
 * getmib - find mib record for the given object id
 *------------------------------------------------------------------------
 */
struct mib_info *
getmib(oip)
struct oid	*oip;
{
	struct snhnode	*hp;
	int				loc, i;

	xp_tracev(90,"getmib: oid.len = %d\n",oip->len);
	for (i=0; i < oip->len; ++i)
		xp_tracev(90,"getmib: oid.id[x] = %d\n",oip->id[i]);

	loc = hashoid(oip);			/* try the regular hash table */

	for (hp=snhtab[loc]; hp; hp=hp->sh_next)
	{
		if (oidequ(oip, &hp->sh_mip->mi_objid))
			return(hp->sh_mip);
	}

	for (i=0; i < S_NUMTABS; ++i)			/* try the table table */
	{
		if (blkequ(tabtab[i].ti_mip->mi_objid.id, oip->id,
									tabtab[i].ti_mip->mi_objid.len * 2))
			return(tabtab[i].ti_mip);
	}
	return 0;
}

/*------------------------------------------------------------------------
 * hashoid - hash the object id
 *------------------------------------------------------------------------
 */
int
hashoid(oip)
struct oid	*oip;
{
	register	unsigned	tot;
	register	int			i;

	for (tot=0, i=oip->len-1; i >= 0; i--) 
		tot = tot * S_HTRADIX + oip->id[i];
	return(tot % S_HTABSIZ);
}

/*------------------------------------------------------------------------
 * hashinit - initialize the hash table
 *------------------------------------------------------------------------
 */
hashinit()
{
	register	struct snhnode		**ht;
	register	struct mib_info		*mp;
				int					i;
				struct mib_info		*lastnodep;
				struct snhnode		*hp;
				int					loc;
				int					tabtabct;

	printf("mib_entries = %d\n",mib_entries);

	/*
	 *	clear the hash table
	 */
	for (ht=snhtab, i=0; i < S_HTABSIZ; i++)
		*ht++ = 0;

	lastnodep = 0;
	tabtabct  = 0;

	for (i=0, mp=&mib[mib_entries - 1]; i < mib_entries; i++, mp--)
	{
		loc         = hashoid(&mp->mi_objid);
		hp          = (struct snhnode *)getmem(sizeof(struct snhnode));
		hp->sh_mip  = mp;
		hp->sh_next = snhtab[loc];
		snhtab[loc] = hp;
		mp->mi_next = lastnodep;

		/*
		 *	IF (node == table)
		 *		insert into array of tables
		 */
		if (mp->mi_vartype == T_TABLE) 
			tabtab[tabtabct++].ti_mip = mp;

		if (mp->mi_varleaf || mp->mi_vartype == T_TABLE)
			lastnodep = mp;
	}

	mp = &mib[0];

	while (1)
	{
		xp_tracev(80,"name = %s\n",mp->mi_name);

		mp = mp->mi_next;

		if (mp == 0)
			break;
	}
}
