/* kprintf.c - kprintf, kputc, savestate, rststate */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <tty.h>
#include <mioaddrs.h>

#include "varargs.h"

typedef unsigned long	ulong;

static	int		(*kputch)();


init_kprintf()
{
	ulong	*sys_ptr;

	sys_ptr = (ulong *)(ADDR_SAT + OFF_PUTCH);
	kputch  = (int(*)())*sys_ptr;
}



/*------------------------------------------------------------------------
 *  kputc  --  write a character on the console using polled I/O
 *------------------------------------------------------------------------
 */
kputc(device ,c)
	int	device;
	register unsigned char c;	/* character to print from _doprnt	*/
{
	if ( c == 0 )
		return;
	if ( c > 127 ) {
	    kputch('M');		/* do what "cat -v" does */
	    kputch('-');		/* prom routine dies otherwise */
	    kputch((char) (c & 0x007f) );
	    return;
	}

	if (c==NEWLINE)
	    kputch(RETURN);

	kputch(c);
}


kprintf( va_alist )
va_dcl
{
	va_list     arg;
	int         v;
	const char  *fmt;
 
	va_start(arg);
	fmt = va_arg(arg, const char *);
	v = doprnt(kputc,fmt,arg,CONSOLE);
	va_end(arg);
	return(v);
}
