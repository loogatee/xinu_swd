/* $Id: mbc_rmail.c,v 1.4 1993/01/30 03:12:28 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>
#include <mbc_text.h>

extern BYTE			Mess_4_Per;
extern PAC_HEAD		*per_mess_ptr;
extern BYTE			TailMessIdx;
extern MESS_LIST	MessList[];
extern BYTE			HeadMessIdx;
extern BYTE			MIO_Mail;
extern SINT16		InQueue;
extern int			IntrMIO;


/*
 *	FUNCTION	ReadMail - read the MIO mailbox
 *
 *	DESCRIPTION
 *		This function reads the mail, clears the new mail flag
 *		and loads the mail message list. Two mail requests are
 *		processed and responded to immediatly, ASYNC_PER_STATUS
 *		& INQUIRE_IO_STATUS. This is done to satisfy printer
 *		requirements during heavy network activity.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
ReadMail()
{
	register	MESS_LIST	*MessListPtr;
	register	PAC_HEAD	*pptr;
	register	BYTE		*ptr;
	register	BYTE		retval;
	register	BYTE		cmd_mod;
				BYTE		buf_type;
				BYTE		cmd;

	/*
	 *	See if the printer requested a destructive self test.
	 */
	if (Mb->IO_status & IOS_CLRREQ)
		reboot();
	else
	{
		/*
		 *	Determine what type of mail we just received.
		 */
		pptr     = (PAC_HEAD *)(pack(Mb->IO_pointer)*2 + SharedMemory);
		ptr      = (BYTE *)((UINT32)pack(pptr->start_data)*2L + SharedMemory);
		buf_type = pptr->buf_type >> 8;
		cmd      = pptr->command >> 8;
		cmd_mod  = pptr->command_mod >> 8;

/*		xp_tracev(82,"rmail: buf_type = %d\n",buf_type);
		xp_tracev(82,"rmail: cmd      = %d\n",cmd);
		xp_tracev(82,"rmail: cmd_mod  = %d\n",cmd_mod); */
/*		xp_tracev(82,"rmail: InQueue  = %d\n",InQueue); */

		/*
		 *	We need to clear the "new" mail bit.
		 */
		Mb->IO_status &= (~IOS_IMBVLD & 0xff00);	/* Clear mb_vld bit */
		InterruptPrinter();

		retval = UNKNOWN_CMD;		/* Let's assume we don't understand */
		switch (buf_type)
		{
			case 0:
				if (cmd == 0 && cmd_mod == 0)
					retval = READBLOCK;
				break;

			case 1:
				if (cmd == 0 && cmd_mod <= 3)
					retval = WRITEBLOCK;
				break;

			case 2:
				switch (cmd)
				{
					case 0:
						if (cmd_mod == 0)
							retval = SET_PERIPHERAL_DISPLAY;
						break;

					case 1:
						if (cmd_mod == 1 || cmd_mod == 2)
							retval = SELFTEST_DESC;
						break;

					case 2:
						if (cmd_mod == 1 || cmd_mod == 2)
							retval = SELFTEST;
						break;

					case 3:
						if (cmd_mod < IO_DESC_SIZE)
							retval = IO_DESC;
						break;

					case 4:
						if (cmd_mod == 0)
						{
							Mess_4_Per   = TRUE;
							per_mess_ptr = pptr;
							retval       = ASYNC_PER_STATUS;
						}
						break;

					case 5:
						if (cmd_mod == 0 || cmd_mod == 1)
						{
							Mess_4_Per   = TRUE;
							per_mess_ptr = pptr;
							retval       = INQUIRE_IO_STATUS;
						}
						break;

					case 6:
						if (cmd_mod < 6)
							retval = ASCII_IO_CONF;
						break;

					case 11:
						if (cmd_mod < 2)
							retval = ASCII_STATUS;
						break;

					default:
						retval = UNKNOWN_CMD;
						break;
				}
				break;

			case 3:
				switch (cmd)
				{
					case 0:
						if (cmd_mod == 0)
							retval = BUFF_CONFIG;
						break;

					case 1:
						if (cmd_mod == 0)
							retval = SAVE_IO_CONFIG;
						break;

					case 2:
						if (cmd_mod == 0)
							retval = RETRIEVE_IO_CONFIG;
						break;

					case 3:
						if (cmd_mod == 0)
							retval = IDENTIFY;
						break;

					case 4:
						if (cmd_mod == 0)
							retval = ASYNC_IO_STATUS;
						break;

					default:
						retval = UNKNOWN_CMD;
						break;
				}
				break;

			default:
				retval = UNKNOWN_CMD;
		}
	}

	/*
	 *	For ASYNC_PER_STATUS & INQUIRE_IO_STATUS, process
	 *	the packet and place into the watch dog queue.
	 */
	if (retval == ASYNC_PER_STATUS)
	{
		pptr->rtn_status = UNDERSTOOD;
		CopyFromEO((BYTE *)&PrinterStatus, ptr, pack(pptr->data_len));

		/*
		 *	Update the MIO's status based on
		 *	the printer's current state.
		 */
		IOStatus.io_idle_mode = PrinterStatus.idle_mode;
	}
	else if (retval == INQUIRE_IO_STATUS)
	{
		/*
		 *	Report back the current status of MIO board.
		 *	Also, since the IIIsi doesn't support command
		 *	modifier 01H, we'll respond as if the command
		 *	modifier was 00H. NOTE: If modified, update
		 *	ReadMail.
		 */
		pptr->rtn_status = UNDERSTOOD;
		unpack(pptr->data_len, 7);
		CopyToEO(ptr, (BYTE *)&IOStatus, 7);		/* It's always seven */
	}
	else
	{
		/*
		 *	Place packet onto the queue for later processing.
		 */
		TailMessIdx = MessList[TailMessIdx].next_idx;
		if (HeadMessIdx == TailMessIdx)
			HangMIO(MESS_LIST_FULL);

		MIO_Mail                = TRUE;
		MessListPtr             = &MessList[TailMessIdx];
		MessListPtr->packet_ptr = (UINT32 *)pptr;
		MessListPtr->cmd        = retval;
		MessListPtr->cmd_mod    = cmd_mod;
		++InQueue;
		signal(IntrMIO);
	}
}
