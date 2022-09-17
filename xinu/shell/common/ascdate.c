/* ascdate.c - ascdate */

#include <conf.h>
#include <kernel.h>
#include <date.h>

/*------------------------------------------------------------------------
 *  ascdate  -  print a given date in ascii including hours:mins:secs
 *------------------------------------------------------------------------
 */
ascdate(time, str)
unsigned long	time;
unsigned char	*str;
{
	unsigned long	hour;
	unsigned long	minute;
	unsigned long	second;

	time %= SECPERDY;

	/* set hour (0-23) */
	hour = (int) ( time/SECPERHR );
	time %= SECPERHR;

	/* set minute (0-59) */
	minute = time / SECPERMN;
	time %= SECPERMN;

	/* set second (0-59) */
	second = (int) time;
	sprintf(str, "%2d:%02d:%02d", hour, minute, second);

	return(OK);
}
