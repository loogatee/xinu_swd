/* snrslv.c - snrslv */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*
 *	Set the error status and error index in a request descriptor.
 */
#define seterr(errval)		rqdp->err_stat = errval;	\
							rqdp->err_idx = i;

/*------------------------------------------------------------------------
 * snrslv - resolve the list of specified variable bindings
 *------------------------------------------------------------------------
 */
snrslv(rqdp)
struct req_desc		*rqdp;
{
	struct snbentry		*bl;
	struct mib_info		*np;
	struct mib_info		*getmib();
	int					i;
	int					op;
	int					err;

/*	xp_tracev(80,"snrslv: entering\n",0); */

	for (bl=rqdp->bindlf, i=1; bl; bl=bl->sb_next, i++)
	{
		/*
		 *	use getmib to look up object id
		 */
		if ((np = getmib(&bl->sb_oid)) == 0)
		{
			seterr(SERR_NO_SUCH);
			xp_tracev(80,"snrslv: SERR_NO_SUCH\n",0);
			return SYSERR;
		}

		/*
		 *	call function to apply specified operation
		 */
		if (np->mi_func == 0)			/* objid is an aggregate */
		{
			/*
			 *	only getnext allows nonexistent names
			 */
			if (rqdp->reqtype != PDU_GETN)
			{
				seterr(SERR_NO_SUCH);
				return SYSERR;
			}
/*
			xp_tracev(80,"snrslv: calling mi_func, arg = SOP_GETF\n",0);
			xp_tracev(80,"snrslv: calling mi_func, = 0x%x\n",np->mi_next->mi_func);
*/
			/*
			 *	use getfirst for getnext on an aggregate
			 */
			if (err = ((*np->mi_next->mi_func)(bl, np->mi_next, SOP_GETF)))
			{
				seterr(err);
				return SYSERR;
			}
		}
		else			/* function in table ==> single item or table */
		{
/*			xp_tracev(80,"snrslv: reqtype = %d\n",rqdp->reqtype); */

			switch (rqdp->reqtype)
			{
				case PDU_GET:	op = SOP_GET;	break;
				case PDU_GETN:	op = SOP_GETN;	break;
				case PDU_SET:	op = SOP_SET;	break;
			}

			/*
			 *	use getfirst for getnext on table entry
			 */
			if (oidequ(&bl->sb_oid,&np->mi_objid) && np->mi_vartype == T_TABLE)
			{
				if (op == SOP_GETN)
				{
				    op = SOP_GETF;
/*					xp_tracev(80,"snrslv: switching op to GETF\n",0); */
				}
			}

/*			xp_tracev(80,"snrslv: mi_func = 0x%x\n",np->mi_func); */

			if (err = ((*np->mi_func)(bl, np, op)))
			{
				seterr(err);
				xp_tracev(80,"snrslv: returning SYSERR\n",0);
				return SYSERR;
			}
		}
	}
	return OK;
}
