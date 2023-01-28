/* $Id: mbc_start.c,v 1.3 1993/01/27 16:30:03 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>
#include <mbc_text.h>

extern BYTE			UpdateNVRAM;
extern BYTE			HeadMessIdx;
extern BYTE			TailMessIdx;
extern UINT16		ReadBlockPool;
extern UINT16		WriteBlockPool;
extern BYTE			FirstAsciiIOConf;
extern PAC_HEAD		*HeadReadPool;
extern PAC_HEAD		*TailReadPool;
extern PAC_HEAD		*HeadWritePool;
extern PAC_HEAD		*TailWritePool;
extern int			config_item_indx;
extern int			config_parm_indx;
extern BYTE			*CurrentPtr;
extern UINT16		BytesRemaining;
extern MESS_LIST	MessList[];
extern SINT16		InQueue;
extern BYTE			NvRam[];
extern UINT16		config_array[CONFIG_ITEM_SIZE][2];
extern BYTE			MIO_Mail;
extern PAC_HEAD		*HeadIOGPPool;
extern PAC_HEAD		*TailIOGPPool;
extern int			IntrMIO;
extern BYTE			BufConfig[];
extern int			ticks_10ms;



/*
 *	FUNCTION	miostart - start up the MIO interface
 *
 *	DESCRIPTION
 *		This function gets called after the mOS has perfomed a
 *		destructive self test. This then performs initialization
 *		and buffer negotiation.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
miostart()
{
	register	PAC_HEAD	*pptr;
	register	BYTE		*data_start;
				BYTE		retval;
				BYTE		reply;
				UINT16		i;
				UINT16		j;
				UINT16		pr;
				UINT16		io;
				UINT16		prev_isr;
				UINT32		mem_pr;
				UINT32		mem_io;
				UINT32		mem_avail;
				UINT32		mem_read;
				UINT32		mem_write;
				UINT32		num_read;
				UINT32		num_write;
				UINT32		mix;

	Language         = 0;
	DisplayType      = 0;
	UpdateNVRAM      = 0;
	HeadMessIdx      = 0;
	TailMessIdx      = 0;
	ReadBlockPool    = 0;
	WriteBlockPool   = 0;
	FirstAsciiIOConf = 0;
	HeadReadPool     = TailReadPool = NULL;
	HeadWritePool    = TailWritePool = NULL;
	config_item_indx = CONFIG_ITEM_SIZE;		/* Point at last item */
	config_parm_indx = 0;
	CurrentPtr       = NULL;
	BytesRemaining   = 0;

	/*
	 *	Initialize the mail message list
	 */
	for (i=0; i < MESS_LIST_SIZE; i++)
	{
		MessList[i].packet_ptr = 0;
		MessList[i].cmd        = 0;
		MessList[i].cmd_mod    = 0;
		MessList[i].next_idx   = i + 1;
	}
	MessList[MESS_LIST_SIZE - 1].next_idx = 0;	/* Pt back to beginning */
	InQueue = 0;

	/*
	 *	Clear out our copy of NVRAM
	 */
	for (i=0; i < 4; i++)
		NvRam[i] = 0;

	/*
	 *	Look for the number selections for each.
	 *	Configuration items 0 (IP address) & 1 (timeout)
	 *	use numbers and are handled differently. All
	 *	other items will run thru the param string to
	 *	determine the number of parameters.
	 */
	for (i=0; i < CONFIG_ITEM_SIZE; i++)
	{
		config_array[i][0] = 0;
		if (i != 4)							/* Init the IP address */
			config_array[i][1] = 255;
		else								/* Init the timeout value */
			config_array[i][1] = 32767;
	}

	for (i=5; i < CONFIG_ITEM_SIZE; i++)
	{
		j = 0;
		config_array[i][0] = 0;
		while (strlen(config_parm[0][i][j]) > 1 && j < CONFIG_PARM_SIZE)
			++j;
		config_array[i][1] = --j;
	}

	/*
	 *	Set the printer status bytes to assumed settings.
	 */
	PrinterStatus.off_line            = 1;		/*  Minimum requirements */
	PrinterStatus.paper_out           = 0;		/*     "         "       */
	PrinterStatus.intervention        = 0;		/*     "         "       */
	PrinterStatus.idle_mode           = 1;		/*     "         "       */
	PrinterStatus.connect_ack         = 0;		/*     "         "       */
	PrinterStatus.printer_err         = 0;		/*     "         "       */
	PrinterStatus.reset_io_channel    = 1;
	PrinterStatus.printer_req_paper   = 0;
	PrinterStatus.paper_jam           = 0;
	PrinterStatus.toner_problem       = 0;
	PrinterStatus.page_punt           = 0;
	PrinterStatus.memory_out          = 0;
	PrinterStatus.io_channel_inactive = 1;
	PrinterStatus.printer_busy        = 0;
	PrinterStatus.printer_wait_data   = 1;
	PrinterStatus.printer_init        = 1;
	PrinterStatus.cover_open          = 0;
	PrinterStatus.printer_printing    = 0;
	PrinterStatus.paper_output_prob   = 0;

	/*
	 *	Set the IO status bytes to assumed settings.
	 */
	IOStatus.holding_data     = 0;
	IOStatus.io_error         = 0;
	IOStatus.link             = 0;
	IOStatus.io_idle_mode     = 1;
	IOStatus.obsolete         = 0;
	IOStatus.io_not_ready     = 0xF0;
	IOStatus.connection_state = 0;

	/*
	 *	Let the printer know that we're done with our
	 *	initialization process and that we're ready to continue.
	 */
	MIO_Mail = 1;					/* 1 => no isr has occured; special case */
	unpack(Mb->PR_pointer, 0);		/* Set the timeout value to zero */
	Mb->PR_status |= PRS_CLRRSP;	/* indicate that self test is done */

	/*
	 *	We must now wait for the printer to acknowledge our
	 *	willingness to communicate (i.e., wait for the clr_rsp
	 *	bit to be cleared & isr to happen).
	 */
	while ((Mb->PR_status & PRS_CLRRSP) != 0)
		sleep10(2);

	/*
	 *	I now need to send an IO_GP packet to the printer
	 *	within one second. The IO_GP packets will start
	 *	right after the mailboxes. Following the IO_GP's
	 *	will be the PR_GP's, READ's and finally the WRITES's. 
	 *	All buffers will be built based on the starting
	 *	IO_GP address.
	 *	Build a temporary IO_GP packet. This will get
	 *	reconfigured during buffer negotiation.
	 */
	pptr = (PAC_HEAD *)(START_IOGP_OFFSET + SharedMemory);
	unpack2(pptr->dev_spec, 0L);
	unpack(pptr->start_data,(((UINT32)(pptr+1)) - (UINT32)SharedMemory) / 2);
	unpack(pptr->max_data_len, MIN_IOGP_SIZE);
	HeadIOGPPool = pptr;
	SendMail(RETRIEVE_IO_CONFIG, 0, 0);

	/*
	 *	Now wait for the desired reply from the printer.
	 */
	retval = 0;
	while (retval != RETRIEVE_IO_CONFIG)
	{
		wait(IntrMIO);
		retval = ProcessMail(&reply);
	}

	/*
	 *	Now let the printer know what we have to offer.
	 */
	SendMail(IDENTIFY, 0, 0);
	while (retval != IDENTIFY)
	{
		wait(IntrMIO);
		retval = ProcessMail(&reply);
	}

	/*
	 *	Send the buffer configuration request.
	 */
	SendMail(BUFF_CONFIG, 0, 0);
	while (retval != BUFF_CONFIG)
	{
		wait(IntrMIO);
		retval = ProcessMail(&reply);
	}

	/*
	 *	Now it's time to build all the packet buffers and
	 *	ship the appropriate packets to the printer.
	 */
	pr  = (BufConfig[0] > MIN_PRGP_SIZE) ? BufConfig[0] : MIN_PRGP_SIZE;
	io  = (BufConfig[1] > MIN_IOGP_SIZE) ? BufConfig[1] : MIN_IOGP_SIZE;
	mix = (BufConfig[2] > PERCENT_READ)  ? BufConfig[2] : PERCENT_READ;

	mem_pr = (pr + 18) * 2;				/* 18=packet size; 2=num packets */
	mem_io = (io + 18) * 2;				/* 18=packet size; 2=num packets */

	/*
	 *	Figure out how much shared memory we have to work with.
	 */
	mem_avail = MAX_SHARE_MEM - (mem_pr + mem_io) - 8;	/* 8 = mailbox size */
	mem_read  = SIZE_READ_PACK + 18;
	mem_write = SIZE_WRITE_PACK + 18;
	num_read  = (mem_avail * mix) / (mem_read * 100);
	num_write = (mem_avail * (100 - mix)) / (mem_write * 100);

	/*
	 *	Calculate the remaining memory. See if we can
	 *	allocate another read packet. If we can't, NO
	 *	WORRIES...
	 */
	mem_avail -= ((mem_read * num_read) + (mem_write * num_write));
	if (mix > 50 && mem_avail > mem_read)
		++num_read;

	/*
	 *	All the packets occupy the beginning of shared
	 *	memory - barring the mailboxes. Determine where
	 *	that data portion starts.
	 *	
	 */
	data_start = (BYTE *)((8 + (4 + num_read + num_write)*18)*2 + SharedMemory);

	/*
	 *	Build the read packets and send them to the printer.
	 */
	for (i=0; i < num_read; i++)
	{
		if (i == 0)
			pptr = (PAC_HEAD *)(START_IOGP_OFFSET + SharedMemory);
		else
			++pptr;

		pptr->rtn_status  = NEW_PACKET;
		pptr->buf_type    = 0;
		pptr->command     = 0;
		pptr->command_mod = 0;

		unpack(pptr->start_data,(((UINT32)data_start-(UINT32)SharedMemory)/2));
		unpack(pptr->data_len, 0);
		unpack(pptr->max_data_len, SIZE_READ_PACK);

		data_start += (SIZE_READ_PACK*2);

		/*
		 *	Return the packet to the printer.
		 */
		DeliverPacket(pptr);
		wait(IntrMIO);
		retval = ProcessMail(&reply);
	}

	/*
	 *	Build the write packets and send them to the printer.
	 */
	for (i=0; i < num_write; i++)
	{
		++pptr;
		pptr->rtn_status  = NEW_PACKET;
		pptr->buf_type    = 0x0100;
		pptr->command     = 0;
		pptr->command_mod = 0;

		unpack(pptr->start_data,(((UINT32)data_start-(UINT32)SharedMemory)/2));
		unpack(pptr->data_len, 0);
		unpack(pptr->max_data_len, SIZE_WRITE_PACK);

		data_start += (SIZE_WRITE_PACK*2);

		/*
		 *	Return the packet to the printer.
		 */
		DeliverPacket(pptr);
/*
		wait(IntrMIO);
		retval = ProcessMail(&reply); */

		while (Mb->PR_status & 0x0100)
		{
			if (MIO_Mail)
				retval = ProcessMail(&reply);
		}
	}

/*	printf("XX\n"); */

	/*
	 *	Build the IO_GP packets and place them on the pool.
	 */
	for (i=0; i < 2; i++)
	{
		++pptr;
		if(i == 0)
		{
			HeadIOGPPool = pptr;
			unpack2(pptr->dev_spec, (UINT32)(pptr + 1));
		}
		else
		{
			TailIOGPPool = pptr;
			unpack2(pptr->dev_spec, 0L);
		}

		pptr->command     = 0;
		pptr->command_mod = 0;

		unpack(pptr->start_data,(((UINT32)data_start-(UINT32)SharedMemory)/2));
		unpack(pptr->data_len, 0);
		unpack(pptr->max_data_len, io);

		data_start += (io*2);
	}

	/*
	 *	Build the PR_GP packets and send them to the printer.
	 */
/*	for (i=0; i < 2; i++) */	/* Needs to be Commented out for Series IV */
	{
		++pptr;
		pptr->rtn_status  = NEW_PACKET;
		pptr->buf_type    = 0x0200;
		pptr->command     = 0;
		pptr->command_mod = 0;

		unpack(pptr->start_data,(((UINT32)data_start-(UINT32)SharedMemory)/2));
		unpack(pptr->data_len, 0);
		unpack(pptr->max_data_len, pr);
		data_start += (pr*2);

		/*
		 *	Return the packet to the printer.
		 */
		DeliverPacket(pptr);

		while (Mb->PR_status & 0x0100)
		{
			if (MIO_Mail)
				retval = ProcessMail(&reply);
		}

/*		wait(IntrMIO);
		retval = ProcessMail( &reply ); */
	}
}
