/* getutim.c - getutim */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <date.h>

/*------------------------------------------------------------------------
 * getutim  --  obtain time in seconds past Jan 1, 1970, ut (gmt)
 *------------------------------------------------------------------------
 */
SYSCALL	getutim(timvar)
long	*timvar;
{
	*timvar = clktime;
	return(OK);
}
