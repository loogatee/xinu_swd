/* $Id: mbc_smail.c,v 1.1 1993/01/26 16:55:19 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>
#include <mbc_text.h>


extern PAC_HEAD		*HeadIOGPPool;
extern BYTE			NvRam[];

/*
 *	FUNCTION	SendMail - send mail to the printer
 *
 *	DESCRIPTION
 *		This function sends mail to the printer. The caller
 *		provides the command and command modifier and this
 *		function will build a packet and send if off.
 *
 *	PARAMETERS
 *		cmd			-	An input parameter which specifies what type of
 *						request is being made of the printer.
 *		cmd_mod		-	If the command supports modifications, this value
 *						is an input paramater which indicates the modification
 *						that is desired.
 *		input_parm	-	Depending on the request, this value may be used
 *						to further specify the input.
 *
 *	RETURN VALUES
 *		TRUE - if able to send letter
 */
BYTE
SendMail(cmd, cmd_mod, input_parm)
BYTE	cmd;
BYTE	cmd_mod;
BYTE	input_parm;
{
	register	PAC_HEAD	*packet_ptr;
	register	BYTE		*ptr;
	register	BYTE		i;
				BYTE		retval = TRUE;

	/*
	 *	Get a I/O general purpose packet off the pool.
	 */
	if (HeadIOGPPool != NULL)
	{
		packet_ptr   = HeadIOGPPool;
		HeadIOGPPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));	/* Next */

		/*
		 *	Build the general part of the request packet.
		 */
		packet_ptr->buf_type    = 0x0300;		/* Constant for MIO to PRN */
		packet_ptr->command_mod = cmd_mod;

		/*
		 *	Based on the request, build the request specific
		 *	part of the packet.
		 */
		ptr = (BYTE *)((UINT32)pack(packet_ptr->start_data)*2L + SharedMemory);

		if (cmd == BUFF_CONFIG)
		{
			packet_ptr->command = 0;
			unpack(packet_ptr->data_len, 0);
		}

		else if (cmd == SAVE_IO_CONFIG)
		{
			packet_ptr->command = 0x0100;
			unpack(packet_ptr->data_len, 4);
			for (i=0; i < 4; i++)
			{
				*ptr++ = NvRam[i];
				ptr++;
			}
		}

		else if (cmd == RETRIEVE_IO_CONFIG)
		{
			packet_ptr->command = 0x0200;
			unpack(packet_ptr->data_len, 0);
		}

		else if (cmd == IDENTIFY)
		{
			packet_ptr->command = 0x0300;
			unpack(packet_ptr->data_len, 8);
			*ptr++ = 3; ptr++;					/* Bi-directional transfers */
			*ptr++ = 3; ptr++;					/* CSMA/CD network */
			*ptr++ = 0; ptr++;					/* Test Harness Support */
			*ptr++ = 0; ptr++;					/* Test Harness Support */
			*ptr++ = 5; ptr++;					/* MIO protocol revision 5.0 */
			*ptr++ = 0; ptr++;					/* MIO protocol revision 5.0 */
			*ptr++ = 0; ptr++;					/* If set, AppleTalk Support */
			*ptr++ = 0; ptr++;					/* If set, Messaging Support */
		}

		else if (cmd == ASYNC_IO_STATUS)
		{
			/* This gets called if the MIO board has a problem or
			 *	if the FLASH ROM is being updated. We're using the
			 *	"I/O card not ready" byte to convey our status.
			 */
			packet_ptr->command   = 0x0400;
			IOStatus.io_not_ready = input_parm;		/* Change our state */

			/*
			 *	Get the current I/O status of the board.
			 */
			unpack(packet_ptr->data_len, 7);
			CopyToEO(ptr, (BYTE *)&IOStatus, 7);
		}

		DeliverPacket(packet_ptr);			/* Send the packet to printer. */
	}
	else
		retval = FALSE;

	return(retval);
}

