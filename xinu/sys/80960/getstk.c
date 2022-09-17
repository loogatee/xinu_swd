#ifndef lint
static char rcsid[] = "$Id: getstk.c,v 1.1 1992/11/10 21:39:57 johnr Exp $";
#endif
/* getstk.c - xp_getstk */

#include <conf.h>
#include <kernel.h>
#include <xmem.h>

/*------------------------------------------------------------------------
 * xp_getstk  --  allocate stack memory, returning address of topmost WORD
 *------------------------------------------------------------------------
 */
WORD
*xp_getstk(nbytes)
register	unsigned int	nbytes;
{
				PCWORD			ps;    
	register	struct mblock	*p;
	register	struct mblock	*q;
	register	struct mblock	*fits;
	register	struct mblock	*fitsq;
	register	WORD			len;

	xa_disable(ps);

	if (nbytes == 0)
	{
		xa_restore(ps);
		xp_kprintf("xp_getstk: nbytes == 0    returning SYSERR\n");
		return((WORD *)XC_SYSERR);
	}

	nbytes = (unsigned int)xm_roundew(nbytes);
	fits   = (struct mblock *)XC_NULL;
	q      = &xv_memlist;

	for (p = q->mnext; p != (struct mblock *)XC_NULL; q=p, p=p->mnext)
	{
		if (p->mlen >= nbytes)
		{
			fitsq = q;
			fits  = p;
		}
	}

	if (fits == (struct mblock *)XC_NULL)
	{
		xa_restore(ps);
		xp_kprintf("xp_getstk:  fits == NULL    returning SYSERR\n");
		return((WORD *)XC_SYSERR);
	}

	if (nbytes == fits->mlen)
		fitsq->mnext = fits->mnext;
	else
	{
		fits->mlen = len = (fits->mlen - nbytes);
		fits             = (struct mblock *)((WORD)fits + len);
	}

	*((WORD *)fits) = nbytes;

	xa_restore(ps);
	return((WORD *)fits);
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: getstk.c,v $
 * Revision 1.1  1992/11/10  21:39:57  johnr
 * JR: 1st entry for all these files.
 *
 * Revision 1.1.1.1  1991/11/12  02:20:29  johnr
 * JRs original Xinu distribution
 *
 * Revision 1.1  1991/11/12  02:20:29  johnr
 * Initial revision
 *
 *
 */
