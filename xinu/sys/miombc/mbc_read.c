/* $Id: mbc_read.c,v 1.1 1993/01/26 16:55:16 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>


extern PAC_HEAD		*HeadWritePool;

/*
 *	FUNCTION	ReadPrn - read data from the printer
 *
 *	DESCRIPTION
 *		This function transfers data received from the printer and
 *		passes the data on to the mOS for possible shipment back to
 *		a host computer. This function gets called if the available
 *		write pool count is greater than zero.
 *
 *	PARAMETERS
 *		ptr	-	A pointer to where the data will be stored
 *
 *	RETURN VALUES
 *		none
 */
UINT16
ReadPrn(ptr)
register	BYTE	*ptr;
{
	register	PAC_HEAD	*pptr;
	register	BYTE		*from_ptr;
				UINT16		len;

	/*
	 *	Get the next packet to return and
	 *	update the head pointer
	 */
	if (HeadWritePool != NULL)
	{
		pptr          = HeadWritePool;
		HeadWritePool = (PAC_HEAD *)(pack2(pptr->dev_spec));

		/*
		 *	Copy the data from the packet into the mOS memory.
		 *	NOTE: The from_ptr will increment by two while the
		 *	ptr increments by 1.
		 */
		len      = pack(pptr->data_len);
		from_ptr = (BYTE *)((UINT32)pack(pptr->start_data)*2 + SharedMemory);

		CopyFromEO(ptr, from_ptr, len);

		/*
		 *	Release the packet back to the printer.
		 */
		pptr->rtn_status = UNDERSTOOD;
		DeliverPacket(pptr);
	}

	return(len);
}
