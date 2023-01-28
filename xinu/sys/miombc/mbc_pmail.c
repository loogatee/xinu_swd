/* $Id: mbc_pmail.c,v 1.4 1993/01/30 03:07:05 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>
#include <mbc_text.h>



extern BYTE			HeadMessIdx;
extern MESS_LIST	MessList[];
extern SINT16		InQueue;
extern BYTE			MIO_Mail;
extern UINT16		ReadBlockPool;
extern UINT16		WriteBlockPool;
extern PAC_HEAD		*HeadReadPool;
extern PAC_HEAD		*TailReadPool;
extern PAC_HEAD		*HeadWritePool;
extern PAC_HEAD		*TailWritePool;
extern BYTE			Sent_SPD;
extern UINT16		Sent_size;
extern BYTE			Sent_L;
extern BYTE			Sent_DT;
extern BYTE			TCPIP_set;
extern BYTE			FirstAsciiIOConf;
extern UINT16		config_array[CONFIG_ITEM_SIZE][2];
extern int			config_item_indx;
extern int			config_parm_indx;
extern BYTE			UpdateNVRAM;
extern BYTE			NvRam[];
extern BYTE			BufConfig[];
extern BYTE			PtrID[];
extern PAC_HEAD		*HeadIOGPPool;
extern PAC_HEAD		*TailIOGPPool;

extern char		*Version;
extern BYTE		LEDval;
extern UINT32	Meminstalled;
extern BYTE		Etherdat[];
extern UINT32	RPacket;
extern UINT32	TPacket;
extern UINT32	ETncr;
extern UINT32	ETjam;
extern UINT32	ETfae;
extern UINT32	ETcrc;
extern UINT32	ETmiss;
extern BYTE		Ipcaddr[];



/*
 *	FUNCTION	ProcessMail - process the next mail message
 *
 *	DESCRIPTION
 *		This function processes the next mail message on the
 *		mail message list. Based on the command and command
 *		modifier, the necessary actions required to satify
 *		the printer are performed.
 *
 *	PARAMETERS
 *		rtn_parm -	Address of location to return information in. This
 *					parameter is used only for case ASCII_IO_CONF.
 *
 *	RETURN VALUES
 *		The return value indicates what type of mail was sent
 *		by the printer.
 */
BYTE
ProcessMail(rtn_parm)
BYTE	*rtn_parm;
{
	register	PAC_HEAD	*packet_ptr;
	register	BYTE		*ptr;
	register	BYTE		*tmp_ptr;
	register	MESS_LIST	*MessListPtr;
	register	BYTE		i;
	register	UINT16		cnt;
				BYTE		loop;
				BYTE		retval;
				BYTE		buf_type;
				BYTE		cmd;
				BYTE		cmd_mod;
				BYTE		buff[32];
				BYTE		reply;
				BYTE		*ad;
				UINT16		x;
				STATWORD	ps;
				UINT32		sum;
	static		UINT16		FS_indx = 0;
	static		UINT16		PS_indx = 0;

	/*
	 *	Move the head index forward, if it equals the tail
	 *	we're finishing the last mail message.
	 */
	disable(ps);
	HeadMessIdx = MessList[HeadMessIdx].next_idx;

	/*
	 *	Extract the oldest mail message's information.
	 */
	MessListPtr          = &MessList[HeadMessIdx];
	packet_ptr           = (PAC_HEAD *)MessListPtr->packet_ptr;
	retval               = MessListPtr->cmd;
	cmd_mod              = MessListPtr->cmd_mod;
	MessListPtr->cmd_mod = 0xbd;
	--InQueue;

/*	xp_tracev(82,"pmail: retval  = %d\n",retval);
	xp_tracev(82,"pmail: cmd_mod = %d\n",cmd_mod); */
/*	xp_tracev(82,"pmail: InQueue = %d\n",InQueue); */

	if (!InQueue)
	{
		MIO_Mail = FALSE;
	}

	restore(ps);			/* Restore interrupts to previous settings */

	ptr = (BYTE *)((UINT32)pack(packet_ptr->start_data)*2L + SharedMemory);

	/*
	 *	Now that we have figured out what we received,
	 *	start processing it.
	 */
	switch (retval)
	{
		case READBLOCK:
			/*	For the read data block, the head points to the
			 *	oldest block and the tail will point to the block
			 *	just read.
			 */
			++ReadBlockPool;
			if (HeadReadPool == NULL)							/* No blocks */
				HeadReadPool = TailReadPool = packet_ptr;
			else
			{
				unpack2(TailReadPool->dev_spec, (UINT32)packet_ptr);
				TailReadPool = packet_ptr;
			}
			unpack2(packet_ptr->dev_spec, 0);
			break;

		case WRITEBLOCK:
		case ASCII_STATUS:
			/*
			 *	Although the ASCII_STATUS request isn't supported
			 *	by the IIIsi, I believe that future products will.
			 *	This request will be treated like a WRITEBLOCK. The
			 *	mOS will have to do a ReadPrn to retrieve the data.
			 */
			++WriteBlockPool;
			if (HeadWritePool == NULL)						/* No blocks */
				HeadWritePool = TailWritePool = packet_ptr;
			else
			{
				unpack2(TailWritePool->dev_spec, (UINT32)packet_ptr);
				TailWritePool = packet_ptr;
			}
			unpack2(packet_ptr->dev_spec, 0L);
			break;

		case SET_PERIPHERAL_DISPLAY:
			DisplayType            = 0;		/* 1 display type is supported */
			packet_ptr->rtn_status = UNDERSTOOD;

			/*
			 *	Since we only support one display type, this is
			 *	checking to see that no other type is sent.
			 */
			Sent_SPD  = 1;
			Sent_size = pack(packet_ptr->data_len);
			Sent_DT   = *ptr;

			if (*ptr != 0)
			{
				*ptr = 0;
				packet_ptr->rtn_status = ERROR_ENCOUNTERED;
			}

			/*
			 *	At present, the numeric language will not be
			 *	supported. If it ever does, this logic will
			 *	need to be modified to handle it.
			 */
			++ptr; ++ptr;
			Sent_L = *ptr;

			if ((Language = *ptr) > (NUM_LANG - 1))
			{
				*ptr = Language = 0;
				packet_ptr->rtn_status = ERROR_ENCOUNTERED;
			}

			DeliverPacket(packet_ptr);		/* Return packet to the printer */
			break;

		case SELFTEST_DESC:
			/*
			 *	For now, since we only support two self tests,
			 *	test 1 will be a non-destructive self test and
			 *	test 2 will be a destructive self test with each
			 *	set for a timeout of 10 seconds. The timeout is
			 *	long but it's not a problem since we tell the
			 *	printer when we're done anyway.
			 */
			packet_ptr->rtn_status = UNDERSTOOD;
			unpack(packet_ptr->data_len, 2);
			*ptr++ = (cmd_mod == 1) ? 1 : 2;
			ptr++;
			*ptr = 10;

			DeliverPacket(packet_ptr);		/* Return packet to the printer */
			break;

		case SELFTEST:
			/*
			 *	Until I know what sort of non-destructive self test
			 *	I can perform we'll not worry about it.
			 */
			if (cmd_mod == 1)
			{
				packet_ptr->rtn_status = UNDERSTOOD;
				*ptr++ = 0;									/* no error */
				*ptr++ = 0;									/* null string */
			}
			else if (cmd_mod == 2)				/* Done only thru Dest_S.T.*/
			{
				packet_ptr->rtn_status = NOT_UNDERSTOOD;
				*ptr++ = 1;									/* error 1 */
				*ptr++ = 0;									/* null string */
				Mb->PR_error = 0xC000;						/* Protcol err */
			}
			unpack(packet_ptr->data_len, 2);

			DeliverPacket(packet_ptr);		/* Return packet to the printer. */
			break;

		case IO_DESC:
			packet_ptr->rtn_status = NOT_UNDERSTOOD;
			DeliverPacket(packet_ptr);	/* Return packet to the printer */
			break;

#ifdef NEEDS_FIXING

			packet_ptr->rtn_status = UNDERSTOOD;
			tmp_ptr  = (BYTE *)(io_description[Language][cmd_mod]);
			cnt      = 0;

			while (*tmp_ptr)
			{
				*ptr++ = *tmp_ptr++;
				ptr++;
				cnt++;
			}

			/*
			 *	The port select and memory available need to have
			 *	their current settings appended. Should either
			 *	selection be moved, the "if" statements below need
			 *	to be updated.
			 */
			if (cmd_mod >= 2)
			{
				tmp_ptr  = buff;
				*tmp_ptr = NULL;

				if (cmd_mod == 2)						/* NOS support */
				{
					if (TCPIP_set)
						sprintf(tmp_ptr, "& TCP/IP");
				}

				else if (cmd_mod == 3)					/* ROM version */
					sprintf(tmp_ptr, "%s", Version);

				else if (cmd_mod == 4)		/* Port select: BNC or 10Base10 */
				{
					i       = (LEDval & 0x04) ? 1 : 0;
					tmp_ptr = (BYTE *)(port_select[Language][i]);
				}

				else if (cmd_mod == 5)				/* Available memory */
					sprintf(tmp_ptr, "%ldK", Meminstalled/1024L);

				else if (cmd_mod == 6)				/* Ethernet address */
				{
					ad = Etherdat;		/* FIX */
					sprintf(tmp_ptr, "%02x:%02x:%02x:%02x:%02x:%02x",
								ad[0], ad[1], ad[2], ad[3], ad[4], ad[5]);
				}

				else if (cmd_mod == 7)				/* Packets Received */
					sprintf(tmp_ptr, "%-ld", RPacket);

				else if (cmd_mod == 8)				/* Packets Transmitted */
					sprintf(tmp_ptr, "%-ld", TPacket);

				else if (cmd_mod == 9)				/* Collisions */
					sprintf(tmp_ptr, "%-ld", ETncr);

				else if (cmd_mod == 10)				/* Unsendable */
					sprintf(tmp_ptr, "%-ld", ETjam);

				else if (cmd_mod == 11)				/* Bad Packets */
					sprintf(tmp_ptr, "%-ld", ETfae + ETcrc + ETmiss);

				else if (cmd_mod == 12)				/* Framing Errors */
					sprintf(tmp_ptr, "%-ld", ETfae);

				else if (cmd_mod == 13)				/* CRC Errors */
					sprintf(tmp_ptr, "%-ld", ETcrc);

				else if (cmd_mod == 14)				/* Tot Bytes RX */
				{
					/* FIX */
				}

				else if (cmd_mod == 15)				/* Tot Bytes Printed */
					sprintf(tmp_ptr, "%-ld", 123);	/* FIX */

				/*
				 *	Line 16 prints blank line.
				 */
				while (*tmp_ptr)
				{
					*ptr++ = *tmp_ptr++;
					ptr++;
					cnt++;
				}
			}

			*ptr = NULL;
			cnt++;

			unpack(packet_ptr->data_len, cnt);
			DeliverPacket(packet_ptr);		/* Return packet to the printer */
			break;
#endif

		case ASYNC_PER_STATUS:
			/*
			 *	Extract the printer status. NOTE: If modified,
			 *	update ReadMail
			 */
			packet_ptr->rtn_status = UNDERSTOOD;
			CopyFromEO((BYTE *)&PrinterStatus, ptr, pack(packet_ptr->data_len));

			/*
			 *	Update the MIO's status based on the
			 *	printer's current state
			 */
			IOStatus.io_idle_mode = PrinterStatus.idle_mode;
			DeliverPacket(packet_ptr);		/* Return packet to the printer */
			break;

		case INQUIRE_IO_STATUS:
			/*
			 *	Report back the current status of MIO board.
			 *	Also, since the IIIsi doesn't support command
			 *	modifier 01H, we'll respond as if the command
			 *	modifier was 00H. NOTE: If modified, update
			 *	ReadMail
			 */
			packet_ptr->rtn_status = UNDERSTOOD;
			unpack(packet_ptr->data_len, 7);
			CopyToEO(ptr, (BYTE *)&IOStatus, 7);	/* It's always seven */
			DeliverPacket(packet_ptr);		/* Return packet to the printer */
			break;

		case ASCII_IO_CONF:

			packet_ptr->rtn_status = NOT_UNDERSTOOD;
			DeliverPacket(packet_ptr);	/* Return packet to the printer */
			break;

#ifdef NEEDS_FIXING

			/*
			 *	If we're dealing with only Novell, we have no front
			 *	panel stuff to display. Therefore, return with a
			 *	request not understood
			 */
			if (!TCPIP_set)
			{
				packet_ptr->rtn_status = NOT_UNDERSTOOD;
				DeliverPacket(packet_ptr);	/* Return packet to the printer */
				break;
			}

			packet_ptr->rtn_status = UNDERSTOOD;

			/*
			 *	The very first time ASCII_IO_CONF gets called, the
			 *	IP address & RARP timeout needs to get loaded in the
			 *	config_array. These values weren't available at
			 *	xio_init & xio_setup times
			 */
			if (!FirstAsciiIOConf)
			{
				FirstAsciiIOConf = TRUE;
				for (i=0; i < 4; i++)
					config_array[i][0] = Ipcaddr[i];		/* Load IP addr */

				/* FIX */
/*				config_array[4][0] = EEconfig.Etherconfig.RARPtout * 10; */
			}

			/*
			 *	This request causes us to go into idle mode
			 */
			IOStatus.io_idle_mode = IDLE_MODE;
			*rtn_parm             = IDLE_MODE;

			/*
			 *	Command modifiers 0 - 4 select which item and
			 *	parameter to send back to the display. Modifiers
			 *	0 & 1 select the item and 2 & 3 select parameters
			 *	within a currently selected item
			 */
			switch (cmd_mod)
			{
				case 0:
					config_item_indx = 0;
					config_parm_indx = config_array[config_item_indx][0];
					break;

				case 1:
					config_item_indx += 1;
					if (config_item_indx >= CONFIG_ITEM_SIZE)
						config_item_indx = 0;
					config_parm_indx = config_array[config_item_indx][0];
					break;

				case 2:
					config_item_indx -= 1;
					if (config_item_indx < 0)
						config_item_indx = CONFIG_ITEM_SIZE - 1;
					config_parm_indx = config_array[config_item_indx][0];
					break;

				case 3:
					config_parm_indx += 1;
					if (config_parm_indx > config_array[config_item_indx][1])
						config_parm_indx = 0;
					break;

				case 4:
					config_parm_indx -= 1;
					if (config_parm_indx < 0)
						config_parm_indx = config_array[config_item_indx][1];
					break;

				case 5:
					/*
					 *	The command modifier 5 is the "Enter" key. It
					 *	means that the current parameter is desired.
					 *	When NVRAM is needed, this portion should save
					 *	the setting
					 */
					config_array[config_item_indx][0] = config_parm_indx;
					if (config_item_indx <= 4)
						UpdateNVRAM = TRUE;
					break;
			}

			/*
			 *	Now let's fill the response buffer. First,
			 *	if the current parameter is the active one,
			 *	set bit 0 to 1. Also, if the current item
			 *	is the last one, set bit 1 to 1
			 */
			*ptr++ = 0;
			ptr++;
			*ptr = 0;

			if (config_array[config_item_indx][0] == config_parm_indx)
				*ptr = 1;

			if (config_item_indx == CONFIG_ITEM_SIZE - 1)
				*ptr = *ptr | 2;

			ptr++; ptr++;

			/*
			 *	The parameters for Config_item's 0 thru 4 use numbers,
			 *	therefore convert the numbers into ASCII strings. All
			 *	the other parameters come from text strings
			 */
			tmp_ptr = (BYTE *)(config_item[Language][config_item_indx]);
			cnt     = 2;

			while (*tmp_ptr)
			{
				*ptr++ = *tmp_ptr++;
				ptr++;
				cnt++;
			}
			*ptr++ = NULL;
			ptr++;
			cnt++;

			tmp_ptr = buff;

			if (config_item_indx < 5)
				sprintf(tmp_ptr, "%d", config_parm_indx);
			else
				tmp_ptr = (BYTE *)(config_parm[Language][config_item_indx][config_parm_indx]);

			while (*tmp_ptr)
			{
				*ptr++ = *tmp_ptr++;
				ptr++;
				cnt++;
			}
			*ptr++ = NULL;
			cnt++;

			unpack(packet_ptr->data_len, cnt);
			DeliverPacket(packet_ptr);		/* Return packet to the printer. */
			break;
#endif

		case RETRIEVE_IO_CONFIG:
			/*
			 *	Only pull off 4 bytes from this response. Only 4
			 *	bytes are guarantied to be saved by all printers.
			 *	Must be reset to factory defaults, if zero. The
			 *	calling routine does the setting.
			 */
			if (pack(packet_ptr->data_len) == 0)
			{
				for (i=0; i < 4; i++)
					NvRam[i] = 0;
			}
			else
			{
				for (i=0; i < 4; i++)
					NvRam[i] = *ptr++;
			}
			break;

		case BUFF_CONFIG:
			loop = (pack(packet_ptr->data_len) > 3) ?
							3 : pack(packet_ptr->data_len);

			for (i=0; i < 3; i++)
			{
				BufConfig[i] = *ptr++;
				ptr++;
			}
			break;

		case IDENTIFY:
			loop = (pack(packet_ptr->data_len) > 9) ?
							9: pack(packet_ptr->data_len);

			for (i=0; i < loop; i++)
				PtrID[i] = *ptr++;
			break;

		case UNKNOWN_CMD:
			/*
			 *	Since we didn't understand the message, just
			 *	return the packet indicating as much.
			 */
			packet_ptr->rtn_status = NOT_UNDERSTOOD;
			DeliverPacket(packet_ptr);
			break;

		default:
			break;
	}

	/*
	 *	Place the IO_GP buffer back into the pool. The
	 *	pointer will still be valid for data extraction
	 *	further down in this routine.
	 */
	switch (retval)
	{
		case BUFF_CONFIG:
		case SAVE_IO_CONFIG:
		case RETRIEVE_IO_CONFIG:
		case IDENTIFY:
		case ASYNC_IO_STATUS:
			if (HeadIOGPPool == NULL)						/* No blocks */
				HeadIOGPPool = TailIOGPPool = packet_ptr;
			else
			{
				unpack2(TailIOGPPool->dev_spec, (UINT32)packet_ptr);
				TailIOGPPool = packet_ptr;
			}
			unpack2(packet_ptr->dev_spec, 0L);
			break;
	}

	return(retval);
}

