/* $Id: errlog.c,v 1.1 1992/11/10 21:13:25 johnr Exp $ */
/****************************************************************/
/* errlog.c - stuff supporting errlog statements				*/
/****************************************************************/
/*																*/
/* The code in this file implements a buffer-based error		*/
/* logging facility.											*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include <kernel.h>
#include <systypes.h>
#include <errlog.h>


/*
 *	EXPORTS
 */
elent	xv_elq[EL_ENTRYS+2];
int		xv_eltail;
int		xv_elhead;
int		xv_elavail;
int		xv_elwrap;
int		xv_elkpflag;			/* kprintf flag */


void
xf_elnewqueue()
{
	register	elent			*hptr;
	register	elent			*tptr;

	xv_elhead   = EL_ENTRYS;
	xv_eltail   = EL_ENTRYS+1;
	xv_elavail  = 0;
	xv_elwrap   = 0;
	xv_elkpflag = 0;

	hptr = &xv_elq[xv_elhead];
	tptr = &xv_elq[xv_eltail];

	hptr->el_qnext = xv_eltail;
	hptr->el_qprev = hptr->el_time = hptr->el_val = -1;
	hptr->el_str   = (char *)(-1);

	tptr->el_qprev = xv_elhead;
	tptr->el_qnext = tptr->el_time = tptr->el_val = -1;
	hptr->el_str   = (char *)(-1);
}

static
el_enqueue(str,val)
register	char	*str;
register	u_long	val;
{
	register	elent			*tptr;
	register	elent			*mptr;

	tptr = &xv_elq[xv_eltail];
	mptr = &xv_elq[xv_elavail];

	if (xv_elwrap == 1)
	{
		xv_elq[mptr->el_qprev].el_qnext = mptr->el_qnext;
		xv_elq[mptr->el_qnext].el_qprev = mptr->el_qprev;
	}

	mptr->el_qnext = xv_eltail;
	mptr->el_qprev = tptr->el_qprev;
	mptr->el_str   = str;
	mptr->el_val   = val;
	mptr->el_time  = xf_timestamp();

	xv_elq[tptr->el_qprev].el_qnext = xv_elavail;
	tptr->el_qprev                  = xv_elavail;

	if (++xv_elavail == EL_ENTRYS)
	{
		xv_elavail = 0;
		xv_elwrap  = 1;
	}
}

void
xp_errlog(str,val)
register	char	*str;
register	u_long	val;
{
	STATWORD	ps;

	disable(ps);

	el_enqueue(str,val);

	if (xv_elkpflag == 1)
		kprintf(str,val);

	restore(ps);
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: errlog.c,v $
 * Revision 1.1  1992/11/10  21:13:25  johnr
 * JR: Initial entry for all these files.
 *
 * Revision 1.1  1992/08/05  17:03:18  johnr
 * JR: Initial entry for this file.
 *
 *
 */
