/* $Id: tty_oin-ssp.c,v 1.2 1992/11/25 00:57:50 johnr Exp $ */

/* ttyoin_ssp.c ttyoin_ssp0, ttyoin_ssp1 */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <systypes.h>
#include <mioaddrs.h>

/*
 *	system-specific part of ttyoin, case 0.
 */
ttyoin_ssp0(iptr)
register	struct tty	*iptr;
{
	(volatile)*ACC_TXD = iptr->ebuff[iptr->etail++];
}


/*
 *	system-specific part of ttyoin, case 1.
 */
ttyoin_ssp1(iptr)
register	struct tty	*iptr;
{
	(volatile)*ACC_TXD = iptr->obuff[iptr->otail++];
}
