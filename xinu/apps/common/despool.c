#ifndef lint
static char rcsid[] = "$Id: despool.c,v 1.3 1993/01/30 02:55:41 johnr Exp $";
#endif

#include <conf.h>
#include <kernel.h>
#include <systypes.h>
#include <spooler.h>


extern	SPL_QJOBS	*spl_qjactive_start;	/* start of Qjob active list */
extern	u_long		ticks_100ms;			/* tick count, 100ms increments */
extern	int			spl_qjact_sem;			/* semaphore of QJOB active list */


PROCESS
despool_d()
{
	SPL_QJOBS	*qjob_ptr;
	SPL_QJOBS	*qtmp;
	SPL_DNODE	*dnode_ptr;
	SPL_DNODE	*dtmp;
	u_long		t100ms;
	int			eof_flag;
	int			len;

	for (;;)
	{
top_loop:
		/*
		 *	Wakeup every half-second and check the active Queue.
		 */
		while (1)
		{
			wait(spl_qjact_sem);

			if ((qjob_ptr = spl_qjactive_start) != 0)
			{
				signal(spl_qjact_sem);
				break;
			}

			signal(spl_qjact_sem);

			sleep10(5);
		}

		/*
		 *	Get a pointer to the 1st data node.  If it's 0, then loop
		 *	while waiting for it to become non-zero.  If, after a
		 *	period of waiting it's still zero, then delete this qjob
		 *	and go to the top of the loop and work on another job.
		 */
		if ((dnode_ptr = qjob_ptr->qj_iptr) == 0)
		{
			t100ms = ticks_100ms;

			while ((dnode_ptr = qjob_ptr->qj_iptr) == 0)
			{
				if ((ticks_100ms - t100ms) > 300)
				{
					spool_retqjob(qjob_ptr,0);
					goto top_loop;			/* go work on another job */
				}
				sleep10(5);
			}
		}

		for (;;)
		{
			/*
			 *	If the data node isn't full, and isn't the EOF record,
			 *	then record the time of this event, then loop while waiting
			 *	for the status of the dnode to change.
			 */
			if ((dnode_ptr->in_status & (SPL_STEND | SPL_STFULL)) == 0)
			{
				t100ms = ticks_100ms;

				while ((dnode_ptr->in_status & (SPL_STEND | SPL_STFULL)) == 0)
				{
					if ((ticks_100ms - t100ms) > 300)
					{
						/*
						 *	backend has waited long enough for the node to
						 *	fill, but it hasn't yet.  Get rid of this Q-job.
						 *	1st, return the dnode to the free list.
						 */
						spool_retdnode(dnode_ptr);

						/*
						 *	Delete the qjob from the active list,
						 *	and put it on the free list
						 */
						spool_retqjob(qjob_ptr,0);
						goto top_loop;	/* go work on another job */
					}
					sleep10(5);
				}
			}

			/*
			 *	Set eof_flag to 1 if this dnode is EOF.
			 *	If not, then set dtmp to point to the next dnode
			 */
			if ((dnode_ptr->in_status & SPL_STEND) != 0)
				eof_flag = 1;
			else
			{
				eof_flag = 0;
				dtmp     = dnode_ptr->in_link;
			}

			/*
			 *	Write the data to the device
			 */
			backend_write(dnode_ptr->in_buf,dnode_ptr->in_len,eof_flag);


			/*
			 *	Done with the dnode.  Return it to the free list
			 */
			spool_retdnode(dnode_ptr);

			/*
			 *	If this dnode was EOF, then the file is done, and this
			 *	qjob can go back on the free list.  It must be deleted
			 *	from the active list, and placed onto the free list
			 */
			if (eof_flag == 1)
			{
				spool_retqjob(qjob_ptr,0);
				break;
			}
			else
			{
				qjob_ptr->qj_iptr = dtmp;
				dnode_ptr         = dtmp;
			}
		}
	}
}

/*
 * $Log: despool.c,v $
 * Revision 1.3  1993/01/30  02:55:41  johnr
 * JR:  Added eof_flag as a parameter to backend_write.  This meant that the
 *      code which determined the state of eof_flag needed to be moved before
 *      the backend_write statement.
 *
 * Revision 1.2  1992/12/22  18:56:44  johnr
 * JR:  Took out both network.h and tty.h from the includes.  Renamed
 *      backend_d to despool_d.  Took out the tty specific stuff and made
 *      a generic interface to the backend-driver code by calling function
 *      backend_write.
 *
 * Revision 1.1  1992/11/25  22:54:28  johnr
 * JR:  1st entry for these files, in the new directory layout.
 *
 *
 */
