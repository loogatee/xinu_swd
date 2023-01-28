/*------------------------------------------------------------------------
 *	ret_to_monitor -- return to the Sun monitor
 *------------------------------------------------------------------------
 */

#include <kernel.h>

/*
 *   Return to the JR's monitor program.
 *
 *   Currently called by:
 *     ttyiin -- on BREAK
 *     doevec -- when exception occurs
 *     xdone  -- when all user processes have finished
 */


ret_mon()
{
	int			i;
	long		*new;
	long		*old;
	STATWORD	ps;

	/*
	 *	mask out all interupts
	 */
	disable(ps);

	/*
	 *	save the currently programmed vectors
	 */
	save_vectors();

	/*
	 *	Reprogram vectors to the original monitor-based vectors
	 */
	restore_old_vectors();

	/*
	 *	trap into the monitor dude!
	 */
    asm("		.short	0x4e4e	");

	/*
	 *	restore the previously saved vectors
	 */
	restore_vectors();

	/*
	 *	turn on interupts
	 */
	restore(ps);
}
