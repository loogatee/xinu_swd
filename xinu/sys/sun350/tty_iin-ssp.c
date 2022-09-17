/* $Id: tty_iin-ssp.c,v 1.2 1992/11/13 23:16:53 johnr Exp $ */

/* ttyiin_ssp.c ttyiin_ssp */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <slu.h>
#include <zsreg.h>


extern int	ttyi_err;
extern int	ttyi_cerr;



ttyiin_ssp(iptr)
struct tty		*iptr;
{
	register volatile	struct	zscc_device *zptr;
	register int	ch;

	zptr = (struct zscc_device *)iptr->ioaddr;
	ch   = zptr->zscc_data;

	DELAY(2);

	/* get the contents of RR1 */
	zptr->zscc_control = 1;
	DELAY(2);
	ttyi_err = zptr->zscc_control;

	/* check for Parity, Overrun, and Framing errors */
	ttyi_err &= (ZSRR1_PE | ZSRR1_DO | ZSRR1_FE);

	ttyi_cerr = 0;
	if (ttyi_err != 0) {
#ifdef PRINTERRORS	    
		kprintf("recv error, ch: 0x%x ", (unsigned int) ch);
		if (ttyi_err & ZSRR1_PE)
			kprintf(", Parity Error");
		if (ttyi_err & ZSRR1_DO)
			kprintf(", Data Overrun");
		if (ttyi_err & ZSRR1_PE)
			kprintf(", Framing Error");
		kprintf("\n");
#endif	    
		ttyi_cerr = IOCHERR;
		zptr->zscc_control = ZSWR0_RESET_ERRORS;	/* reset the error */
	}

	return(ch);
}
