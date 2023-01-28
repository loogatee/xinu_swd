/* ttyutils.c */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>

int fttyoint[2] = { FALSE, FALSE };

/*
 * ttyostop -- turn off tty xmit interrupts
 */
ttyostop(iptr)
register	struct tty	*iptr;
{
	STATWORD			ps;

	disable(ps);
	fttyoint[iptr->dev] = FALSE;
	restore(ps);
}


/*
 * ttyostart -- turn on xmit interrupts
 */
ttyostart(iptr)
register	struct tty	*iptr;
{
	STATWORD			ps;

	disable(ps);

	if (fttyoint[iptr->dev] == FALSE)
	{
		fttyoint[iptr->dev] = TRUE;
		ttyoin(iptr->dev);
	}
	restore(ps);
}
