/* ttyutils.c */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <slu.h>
#include <zsreg.h>

int fttyoint[2] = { FALSE, FALSE };


/*
 * ttyostop -- turn off tty xmit interrupts
 */
ttyostop(iptr)
register	struct tty	*iptr;
{
				STATWORD			ps;
	register	struct zscc_device	*zptr;

	zptr = (struct zscc_device *)iptr->ioaddr;

	disable(ps);

	DELAY(2);
	zptr->zscc_control = 1;
	DELAY(2);
	zptr->zscc_control = ZSWR1_RIE;

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
	register	struct zscc_device	*zptr;

	zptr = (struct zscc_device *)iptr->ioaddr;

	disable(ps);

	if (fttyoint[iptr->dev] == FALSE)
	{
		DELAY(2);
		zptr->zscc_control = 1;
		DELAY(2);
		zptr->zscc_control = ZSWR1_RIE | ZSWR1_TIE;

		fttyoint[iptr->dev] = TRUE;
		ttyoin(iptr->dev);
	}
	restore(ps);
}
