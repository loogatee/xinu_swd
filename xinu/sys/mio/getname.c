/* getname.c - getname */

#include <conf.h>
#include <kernel.h>

static char mach_name[] = "mio";

/*------------------------------------------------------------------------
 *  getname  -  get name of this host and place it where specified
 *------------------------------------------------------------------------
 */
SYSCALL	getname(nam)
char	*nam;
{
#ifdef NETSTUFF
	IPaddr	myaddr[4];
	char	*p;

	*nam = NULLCH;
	if (!Net.mnvalid) {
		getaddr(myaddr);
		if (ip2name(myaddr, Net.myname) == SYSERR)
			return(SYSERR);
		Net.mnvalid = TRUE;
	}
	for (p=Net.myname ; *p != NULLCH && *p != '.' ; )
		*nam++ = *p++;
	*nam = NULLCH;
#endif

	register	char	*p;

	for (p=mach_name; *p != NULLCH && *p != '.'; )
		*nam++ = *p++;
	*nam = NULLCH;

	return(OK);
}
