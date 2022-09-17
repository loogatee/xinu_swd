/* ttywrite.c - ttywrite, writcopy */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>


/*------------------------------------------------------------------------
 *  ttywrite - write one or more characters to a tty device
 *------------------------------------------------------------------------
 */
ttywrite(devptr, buff, count)
struct	devsw	*devptr;
char	*buff;
int	count;
{
	STATWORD ps;    
	register struct tty *ttyp;
	int ncopied;

	if (count < 0)
		return(SYSERR);
	if (count == 0)
		return(OK);
	disable(ps);
	ttyp = &tty[devptr->dvminor];
	for ( ; count>0 ; count--) {
		ttyputc(devptr, *buff++);
	}
	restore(ps);
	return(OK);
}
