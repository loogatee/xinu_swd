/* $Id: fprintf.c,v 1.1 1992/11/10 18:04:05 johnr Exp $ */
/* fprintf.c - fprintf */
#include "varargs.h"

int	putc();

#define	OK	1

fprintf( va_alist )
va_dcl
{
	va_list     arg;
	int         v;
	int			dev;
	const char  *fmt;

	va_start(arg);

	dev = va_arg(arg, int);
	fmt = va_arg(arg, const char *);

	v = doprnt(putc,fmt,arg,dev);
	va_end(arg);
	return(v);
}
