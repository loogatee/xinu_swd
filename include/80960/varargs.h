/* $Id: varargs.h,v 1.1 1992/11/10 18:22:28 johnr Exp $ */
/* traditional varargs.h for 80960 for GCC  -  mcg 2/18/89 */
/* fix bug in adjusting pointer when a structure > 16 bytes is
 * passed by-value.  - atw 10/5/89 */
/* note that this is NOT stdarg.h, required for ANSI */

#ifndef _VARARGS_H
#define	_VARARGS_H

#define	_VA_NREGS	12	/* number of argument registers */
#define	_VA_NBYTES	(_VA_NREGS * sizeof(int))
#define _VA_2ALGN	(4 * sizeof(int) )

/* va_list[0] is base of argblk; va_list[1] is displacement of current arg */
typedef unsigned va_list[2];	

/* __builtin_va_alist as a formal tells GCC that this function is varargs */
#define	va_alist __builtin_va_alist
#define	va_dcl	 char *__builtin_va_alist;

#define	va_start(ap)	(ap)[1] = 0, (ap)[0] = (unsigned)&__builtin_va_alist

#define _va_mask(type) (					\
	(sizeof(type) <= sizeof(int))				\
	?							\
		0x03						\
	:							\
		((sizeof(type) <= sizeof(double))		\
		?						\
			0x07					\
		:						\
			0x0f)					\
	)

#define	va_arg(ap,type) (						\
	(  							        \
	    (((ap)[1] < _VA_NBYTES) && (sizeof(type) > _VA_2ALGN))	\
	    ?								\
		((ap)[1]=_VA_NBYTES + sizeof(type))      		\
	    :								\
		((ap)[1]=((((ap)[1]+_va_mask(type))&~_va_mask(type)) + sizeof(type))) \
	) ,								\
	*((type *) ((char *) (ap)[0] + (ap)[1] - sizeof(type)))      	\
    )

#define	va_end(ap)

#endif
