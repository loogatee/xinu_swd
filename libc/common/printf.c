/* $Id: printf.c,v 1.1 1992/11/10 18:04:07 johnr Exp $ */
/* printf.c - printf */
#include "varargs.h"

#define	XC_CONSOLE	0


int		putc();

/*------------------------------------------------------------------------
 *  printf  --  write formatted output on CONSOLE 
 *------------------------------------------------------------------------
 */
printf( va_alist )
va_dcl
{
	va_list		arg;
	int			v;
	const char	*fmt;

	va_start(arg);
	fmt = va_arg(arg, const char *);
	v = doprnt(putc,fmt,arg,XC_CONSOLE);
	va_end(arg);
	return(v);
}
