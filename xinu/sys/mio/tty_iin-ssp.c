/* $Id: tty_iin-ssp.c,v 1.2 1992/11/25 00:57:18 johnr Exp $ */

/* ttyiin_ssp.c ttyiin_ssp */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <systypes.h>
#include <mioaddrs.h>


extern int	ttyi_err;
extern int	ttyi_cerr;



ttyiin_ssp(iptr)
struct tty		*iptr;
{
	volatile	register	u_char	*lsr_ptr;
				register	int		ch;

	lsr_ptr = ACC_LSR;
	ch      = (volatile)*ACC_RXD & 0x7f;

	ttyi_err  = 0;
	ttyi_cerr = 0;

	return(ch);
}
