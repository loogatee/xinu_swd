

/*
 *	$Id: sprintf.c,v 1.1 1992/11/10 18:04:08 johnr Exp $
 */

#include "varargs.h"


/*------------------------------------------------------------------------
 *  sprntf  --  routine called by doprnt to handle each character
 *------------------------------------------------------------------------
 */

static	sprntf(cpp, c)
char    **cpp;
char    c;
{
        return(*(*cpp)++ = c);
}



/*------------------------------------------------------------------------
 *  sprintf  --  format arguments and place output in a string
 *------------------------------------------------------------------------
 */
sprintf( va_alist )
va_dcl
{
		char		*buf;
		const char	*fmt;
		va_list		arg;
        char    	*s;

		va_start(arg);

		buf = va_arg(arg, char *);
		fmt = va_arg(arg, const char *);
	
        s = buf;
        doprnt(sprntf, fmt, arg, &s);
		va_end(arg);
        *s++ = '\0';

        return((int)s);
}

/*
 *	$Log: sprintf.c,v $
 * Revision 1.1  1992/11/10  18:04:08  johnr
 * JR: Initial entry for these files.
 *
 *
 */
