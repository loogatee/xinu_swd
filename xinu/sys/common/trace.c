/* $Id: trace.c,v 1.1 1992/11/10 21:14:58 johnr Exp $ */
/****************************************************************/
/* trace.c - stuff supporting tracev statements					*/
/****************************************************************/
/*																*/
/* The code in this file implements a buffer-based tracing		*/
/* facility.													*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include "kernel.h"
#include "systypes.h"
#include "trace.h"


/*
 *	EXPORTS
 */
trent	xv_trq[TR_ENTRYS+2];
int		xv_trtail;
int		xv_trhead;
int		xv_travail;
int		xv_trwrap;
int		xv_tracepri;
int		xv_exclusive;
int		xv_trkpflag;		/* kprintf flag */

int		tr_enqflag;


void
xf_trnewqueue()
{
	register	trent			*hptr;
	register	trent			*tptr;
	static		int				trfirst = 0;

	if (trfirst++ == 0)
	{
		tr_enqflag   = 0;
		xv_tracepri  = 80;
		xv_exclusive = 0;
		xv_trkpflag  = 0;
	}

	xv_trhead   = TR_ENTRYS;
	xv_trtail   = TR_ENTRYS+1;
	xv_travail  = 0;
	xv_trwrap   = 0;

	hptr = &xv_trq[xv_trhead];
	tptr = &xv_trq[xv_trtail];

	hptr->qnext = xv_trtail;
	hptr->qprev = hptr->time = hptr->val = -1;
	hptr->str   = (char *)(-1);

	tptr->qprev = xv_trhead;
	tptr->qnext = tptr->time = tptr->val = -1;
	hptr->str   = (char *)(-1);
}

static
tr_enqueue(str,val)
register	char	*str;
register	u_long	val;
{
	register	trent			*tptr;
	register	trent			*mptr;

	tptr = &xv_trq[xv_trtail];
	mptr = &xv_trq[xv_travail];

	if (xv_trwrap == 1)
	{
		xv_trq[mptr->qprev].qnext = mptr->qnext;
		xv_trq[mptr->qnext].qprev = mptr->qprev;
	}

	mptr->qnext = xv_trtail;
	mptr->qprev = tptr->qprev;
	mptr->str   = str;
	mptr->val   = val;
	mptr->time  = xf_timestamp();

	xv_trq[tptr->qprev].qnext = xv_travail;
	tptr->qprev               = xv_travail;

	if (++xv_travail == TR_ENTRYS)
	{
		xv_travail = 0;
		xv_trwrap  = 1;
	}
}

void
xp_tracev(pri,str,val)
register	int		pri;
register	char	*str;
register	u_long	val;
{
	STATWORD	ps;

	disable(ps);

	if (tr_enqflag == 0)
	{
		if (xv_exclusive == 1)
		{
			if (pri == xv_tracepri)
			{
				tr_enqueue(str,val);
				if (xv_trkpflag == 1)
					kprintf(str,val);
			}
		}
		else
		{
			if (pri <= xv_tracepri)
			{
				tr_enqueue(str,val);
				if (xv_trkpflag == 1)
					kprintf(str,val);
			}
		}
	}

	restore(ps);
}




/*------------------------------------------------------------------------
 *  kptrace  -  print out the trace buffer
 *------------------------------------------------------------------------
 */
kptrace()
{
	register	trent	*tptr;

	tptr = &xv_trq[xv_trtail];

	kprintf("\n\n<------------------TOP------------------->\n");

	while (1)
	{
		tptr = &xv_trq[tptr->qprev];

		if (tptr->qprev == -1)
		{
			kprintf("<----------------BOTTOM------------------>\n");
			break;
		}

		kprintf("%08x ",tptr->time);
		kprintf(tptr->str,tptr->val);
	}
}





/*
 * MODIFICATION HISTORY:
 *
 * $Log: trace.c,v $
 * Revision 1.1  1992/11/10  21:14:58  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.2  1992/10/13  20:20:24  johnr
 * JR:  Added function kptrace.
 *
 * Revision 1.1  1992/08/03  23:32:38  johnr
 * JR: Initial version of all these files.
 *
 * Revision 1.2  1992/06/04  20:37:52  johnr
 * JR: added systypes.h to includes.  Changed type of val in a couple of places
 * from long to u_long.
 *
 * Revision 1.1  1992/06/03  18:49:14  johnr
 * JR: Initial rev.  Tracev stuff.
 *
 */
