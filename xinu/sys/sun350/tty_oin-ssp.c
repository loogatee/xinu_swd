/* $Id: tty_oin-ssp.c,v 1.1 1992/11/11 22:36:53 johnr Exp $ */

/* ttyoin_ssp.c ttyoin_ssp0, ttyoin_ssp1 */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <slu.h>
#include <zsreg.h>

/*
 *	system-specific part of ttyoin, case 0.
 */
ttyoin_ssp0(iptr)
register	struct tty	*iptr;
{
	register	struct zscc_device	*zptr;

	zptr = (struct zscc_device *)iptr->ioaddr;

	zptr->zscc_data = iptr->ebuff[iptr->etail++];
}


/*
 *	system-specific part of ttyoin, case 1.
 */
ttyoin_ssp1(iptr)
register	struct tty	*iptr;
{
	register	struct zscc_device	*zptr;

	zptr = (struct zscc_device *)iptr->ioaddr;

	zptr->zscc_data = iptr->obuff[iptr->otail++];
}
