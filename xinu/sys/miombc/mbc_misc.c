/* $Id: mbc_misc.c,v 1.2 1993/01/27 03:01:30 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_def.h>
#include <mbc_func.h>
#include <mbc_text.h>


/*
 *	EXTERNS
 */
extern UINT16	ReadBlockPool;		/* The num of read packets we hold */
extern UINT16	WriteBlockPool;		/* The num of write packets we hold */
extern BYTE		MIO_Mail;			/* If set = mail from the printer */

/*
 *	GLOBALS
 */
BYTE			NvRam[4];			/* Config settings found in NVRAM */
BYTE			Language;			/* Indicates current native language */
BYTE			DisplayType;		/* Specifies printer LCD config */
BYTE			UpdateNVRAM;		/* If set, NVRAM needs updating */
BYTE			HeadMessIdx;		/* Index of the oldest mail message */
BYTE			TailMessIdx;		/* Index of the newest mail message */
PRINTER_STATUS	PrinterStatus;		/* Contains the lastest printer info */
IO_STATUS		IOStatus;			/* Contains the lastest MIO info */
BYTE			Deliver;
PAC_HEAD		*per_mess_ptr;
BYTE			Mess_4_Per;
UINT16			Sent_size;
PAC_HEAD		*HeadIOGPPool;
PAC_HEAD		*TailIOGPPool;
BYTE			BufConfig[3];		/* Config parms for buffer negot. */
UINT16			BytesRemaining;		/* Used in WritePrn, data left */
int				config_item_indx;	/* Current item selected */
int				config_parm_indx;	/* Current parameter selected */
SINT16			InQueue;			/* For debugging */
BYTE			FirstAsciiIOConf;	/* 0=ASCII_IO_CONF not called yet */
PAC_HEAD		*HeadReadPool;
PAC_HEAD		*TailReadPool;
PAC_HEAD		*HeadWritePool;
PAC_HEAD		*TailWritePool;
BYTE			PtrID[9];			/* Printers IDENTIFY response */
BYTE			*CurrentPtr;		/* Used in WritePrn, tracks data */

UINT16			config_array[CONFIG_ITEM_SIZE][2];	/* 1=active; 2=#parms */
MESS_LIST		MessList[MESS_LIST_SIZE];			/* The mail message list */

/*
 *	GLOBALS - Pre-initialized
 */
BYTE	TCPIP_set = 1;			/* Temp until a better method is figured out */
BYTE	Sent_L    = 69;
BYTE	Sent_DT   = 69;
BYTE	Sent_SPD  = 0;


/*
 *	FUNCTION	InterruptPrinter - Interrupt the printer
 *
 *	DESCRIPTION
 *		This function tells the MIO hardware to interrupt
 *		the printer.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
InterruptPrinter()
{
	*INTERRUPT_PRINTER = 1;		/* Just put anything out there */
}


/*
 *	FUNCTION	DeliverPacket - Deliver a packet to the printer
 *
 *	DESCRIPTION
 *		This function waits until the printer is ready to
 *		receive a packet. When the printer is ready, the packet
 *		gets attached to the printer's mailbox and then sent.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
DeliverPacket(pptr)
PAC_HEAD	*pptr;
{
	UINT16		retry;

	Deliver = TRUE;
	while (Mb->PR_status & 0x0100)		/* Test mb_vld for printer ready */
	{
		for (retry=0; retry < 1024; retry++)
			;
		InterruptPrinter();
	}

	/*
	 *	Fill the mailbox and ship it off
	 */
	unpack(Mb->PR_pointer,(UINT16)((UINT32)pptr - (UINT32)SharedMemory) / 2);
	Mb->PR_status |= 0x0100;					/* Set mb_vld bit */
	InterruptPrinter();
	Deliver = FALSE;
}


/*
 *	FUNCTION	UpdateFlash - update the flash
 *
 *	DESCRIPTION
 *		This function updates the current FLASH RAM parameter
 *		structure with a new structure.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
UpdateFlash( )
{
	BYTE			*ptr;
	BYTE			retval;
	BYTE			reply;
	BYTE			i;
}


/*
 *	FUNCTION	HangMIO - cause printer to reset the MIO board
 *
 *	DESCRIPTION
 *		This function writes an error number in the printers mail-
 *		box and loops until a reset occurs.
 *
 *	PARAMETERS
 *		errornum - value to store into the printers mailbox.
 *
 *	RETURN VALUES
 *		none
 */
void
HangMIO(errornum)
UINT16	errornum;
{
	STATWORD	ps;

	ps[0] = 0x2000;

	restore(ps);					/* Enable interrupts */

	Mb->PR_error = errornum;
	InterruptPrinter();
	while (1)
		;
}


/*
 *	FUNCTION	pack - pack up some un-packed data
 *
 *	DESCRIPTION
 *		This function takes unpacked data, data in every other byte,
 *		and moves the bytes together.
 *		NOTE:	This function takes two bytes in two adjacent words
 *				and builds a single word from them.
 *
 *	PARAMETERS
 *		src - points to the hi order byte.
 *
 *	RETURN VALUES
 *		none
 */
UINT16
pack(src)
register	UINT16	*src;
{
	register	UINT16	retval = 0;

	retval = *src++;
	retval = (*src >> 8) | (retval & 0xFF00);
	return(retval);
}


/*
 *	FUNCTION	unpack - split up some packed data
 *
 *	DESCRIPTION
 *		This function takes data in one word and moves the two bytes
 *		into two adjacent words (i.e., B1B2 -> xxB1.xxB2 ).
 *
 *	PARAMETERS
 *		src - contains the value to split.
 *		dst - points to where the bytes will go.
 *
 *	RETURN VALUES
 *		none
 */
void
unpack(dst, src)
register	UINT16	*dst;
register	UINT16	src;
{
	*dst++ = src & 0xFF00;
	*dst   = (src << 8) & 0xFF00;
}


/*
 *	FUNCTION	pack2 - pack 4 bytes from 4 words into 2 words
 *
 *	DESCRIPTION
 *		This function takes unpacked data, data in every other byte,
 *		and moves the bytes together. This function packs 4 bytes
 *		from 4 words into 2 words.
 *
 *	PARAMETERS
 *		src - points to the hi order byte.
 *
 *	RETURN VALUES
 *		value of the four combined bytes.
 */
UINT32
pack2(src)
register	UINT16	*src;
{
	register	UINT32	retval = 0;

	retval = ((UINT32)*src++ << 16);
	retval = ((UINT32)*src++ << 8) | (retval & 0xFF00FFFF);
	retval = ((UINT32)*src++) | (retval & 0xFFFF00FF);
	retval = ((UINT32)*src >> 8) | (retval & 0xFFFFFF00);
	return(retval);
}


/*
 *	FUNCTION	unpack2 - packs up un-packed data from 2 words
 *
 *	DESCRIPTION
 *		This function takes data in 2 words and moves the 4 bytes
 *		into 4 adjacent words(ie, B1B2B3B4 -> xxB1.xxB2.xxB3.xxB4 ).
 *
 *	PARAMETERS
 *		src - contains the value to split.
 *		dst - points to where the bytes will go.
 *
 *	RETURN VALUES
 *		none
 */
void
unpack2(dst, src)
register	UINT16	*dst;
register	UINT32	src;
{
	*dst++ = (src >> 16) & 0xFF00;
	*dst++ = (src >> 8) & 0xFF00;
	*dst++ = src & 0xFF00;
	*dst++ = (src << 8) & 0xFF00;
}

/*
 * $Log: mbc_misc.c,v $
 * Revision 1.2  1993/01/27  03:01:30  johnr
 * JR:  Developmental checkin.  Status is that miostart completes successfully,
 *      but I get a service error 09 displayed on the front panel.  Board does
 *      not hang.
 *
 * Revision 1.1  1993/01/26  16:55:13  johnr
 * JR:  Initial entry for all these files.  These files are all from Ted's
 *      original MIO driver code, and I have already modified them slightly.
 *
 *
 */
