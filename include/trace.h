/* $Id: trace.h,v 1.1 1992/11/11 16:51:41 johnr Exp $ */

typedef struct
{
	long	time;
	char	*str;
	u_long	val;
	long	qnext;
	long	qprev;
} trent;

typedef struct
{
	long	ln;
	trent	*trptr;
} scrlim;


#define TR_ENTRYS	1000
