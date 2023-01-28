/* $Id: mbc_write.c,v 1.2 1993/01/30 03:09:42 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>


extern UINT16	BytesRemaining;
extern BYTE		*CurrentPtr;
extern PAC_HEAD	*HeadReadPool;


/*
 *	FUNCTION	WritePrn - write data to the printer
 *
 *	DESCRIPTION
 *		This function sends data received from a host computer
 *		to the printer. This function will use as many READBLOCK's
 *		as necessary to complete the transfer. Should the transfer
 *		require more READBLOCK's, the current state of the transfer
 *		is saved and the function returns a value of TRUE. Only
 *		when the transfer is complete will a value of FALSE be
 *		returned.
 *
 *		NOTE: The caller should NOT attempt to send another job
 *		until the current job has completed. ptr & len only have
 *		meaning when a job is FIRST sent.
 *
 *	PARAMETERS
 *		ptr				- points to where to get the data to send.
 *		len				- number of bytes to send.
 *		AvailReadBlocks	- number of READBLOCK's available to send.
 *		EndOfJob		- If TRUE, means last packet in job.
 *
 *	RETURN VALUES
 *		TRUE	- means the transfer HAS NOT completed
 *		FALSE	- means the transfer has completed and another
 *				  job many be sent.
 */
BOOL
WritePrn(ptr, len, AvailReadBlocks, EndOfJob)
register	BYTE	*ptr;
			UINT32	len;
			UINT16	*AvailReadBlocks;
			UINT32	EndOfJob;
{
	register	PAC_HEAD	*pptr;
	register	BYTE		*to_ptr;
				UINT32		cnt;

	/*
	 *	If we're starting a new job, set the data pointer and counter
	 */
	if (!BytesRemaining)
	{
		CurrentPtr = ptr;
		BytesRemaining = len;
	}

	/*
	 *	Build and send as many packets as needed to complete
	 *	the job, stop when we're out of packet
	 */
	while (*AvailReadBlocks && BytesRemaining)
	{
		pptr   = HeadReadPool;
		HeadReadPool = (PAC_HEAD *)(pack2(pptr->dev_spec));
		--*AvailReadBlocks;

		/*
		 *	NOTE: The to_ptr increments by two while CurrentPtr
		 *	increments by one
		 */
		to_ptr = (BYTE *)((UINT32)pack(pptr->start_data)*2L + SharedMemory);
		cnt = (BytesRemaining > pack(pptr->max_data_len)) ?
						pack(pptr->max_data_len) : BytesRemaining;

		CopyToEO(to_ptr, CurrentPtr, cnt);
		BytesRemaining -= cnt;

		/*
		 *	Release the packet back to the printer
		 */
		unpack(pptr->data_len, cnt);
		pptr->rtn_status = EndOfJob ? LOGICAL_BREAK : UNDERSTOOD;
		DeliverPacket(pptr);
	}

	/*
	 *	If we still have data to send, don't reset the
	 *	static variables (BytesRemaining & CurrentPtr).
	 *	Also, return TRUE if data remains, FALSE otherwise
	 */
	if (BytesRemaining)
		return(TRUE);			/* Require more READBLOCK's to complete */
	else
		CurrentPtr = NULL;

	return(FALSE);				/* Transfer has completed */
}
