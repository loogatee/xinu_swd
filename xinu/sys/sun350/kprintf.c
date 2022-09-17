/* kprintf.c - kprintf, kputc, savestate, rststate */

#include <sunromvec.h>
#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <tty.h>

#include "varargs.h"

#define prputchar(x) (romp->v_putchar)((unsigned char) x)


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
	    prputchar('M');		/* do what "cat -v" does */
	    prputchar('-');		/* prom routine dies otherwise */
	    prputchar((char) (c & 0x007f) );
	    return;
	}

	if (c==NEWLINE)
	    prputchar(RETURN);

	prputchar(c);
}


#ifdef NOT_THIS_ONE
/*------------------------------------------------------------------------
 *  kprintf  --  kernel printf: formatted, unbuffered output to CONSOLE
 *------------------------------------------------------------------------
 */
kprintf(fmt, args)		/* Derived by Bob Brown, Purdue U.	*/
char *fmt;
int args;
{
	STATWORD ps;    
        int     kputc();

        disable(ps);
        _doprnt(fmt, &args, kputc, CONSOLE);
        restore(ps);
        return(OK);
}
#endif

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
