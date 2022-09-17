/* $Id: errlog.h,v 1.1 1992/11/11 16:50:44 johnr Exp $ */

typedef struct
{
	long	el_time;
	char	*el_str;
	u_long	el_val;
	long	el_qnext;
	long	el_qprev;
} elent;


#define EL_ENTRYS	300
