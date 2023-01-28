/* $Id: mbc_func.h,v 1.2 1993/01/27 16:33:31 johnr Exp $ */


#define		IDLE_MODE			1
#define		OPERATE_MODE		0

#define		MIN_PRGP_SIZE		36
#define		MIN_IOGP_SIZE		36
#define		PERCENT_READ		90
#define		SIZE_READ_PACK		1540			/* 1.5K */
#define		SIZE_WRITE_PACK		48
	
#define		START_IOGP_OFFSET	0x10			/* 1st packet address offset */
#define		SHARE_MEM_OFFSET	0x200000		/* Start of shared memory */
#define		MAX_SHARE_MEM		0x10000			/* 64K of shared memory */

#define		MB_TEST_ERROR		0x0100			/* MIO error */
#define		PRINTER_HUNG		0x0200			/*  "    "   */
#define		MESS_LIST_FULL		0x0300			/*  "    "   */

#define		INTERRUPT_PRINTER	((BYTE *)0xE00001)
#define		Mb					((MAILBOX *)0x380000)
#define		SharedMemory		((BYTE *)0x380000)

enum	RequestMessageTypes
{
	DEST_SELFTEST,
	SELFTEST,
	SELFTEST_DESC,
	READBLOCK,
	WRITEBLOCK,
	SET_PERIPHERAL_DISPLAY,
	IO_DESC,
	ASYNC_PER_STATUS,
	INQUIRE_IO_STATUS,
	ASCII_IO_CONF,
	ASCII_STATUS,
	PROTOCOL_ERROR,
	BUFF_CONFIG,
	SAVE_IO_CONFIG,
	RETRIEVE_IO_CONFIG,
	IDENTIFY,
	ASYNC_IO_STATUS,
	UNKNOWN_CMD
};

#define		NEW_PACKET			0x0000
#define		UNDERSTOOD			0x0100
#define		NOT_UNDERSTOOD		0x0200
#define		ERROR_ENCOUNTERED	0x0300
#define		ABNORMAL_BREAK		0x0400
#define		OVERRUN				0x0500
#define		DATA_ERROR			0x0600
#define		NORMAL_BREAK		0x0700
#define		LOGICAL_BREAK		0x0800
#define		LOGICAL_CONN		0x0900
#define		NOT_SENT_HOST		0x0A00
#define		PARTIAL_MESS		0x0B00
#define		END_JOB				0x0C00

/*
 *	The following defines all the routines found in
 *	the file MBC_FUNC.C
 */
extern	void	ReadMail();
extern	BYTE	ProcessMail();
extern	BYTE	SendMail();
extern	void	InitMIO();
extern	UINT16	ReadPrn();
extern	BOOL	WritePrn();
extern	void	InterruptPrinter();
extern	void	DeliverPacket();
extern	void	RestartMIO();
extern	void	UpdateFlash();
extern	void	HangMIO();
extern	UINT16	pack();
extern	void	unpack();
extern	UINT32	pack2();
extern	void	unpack2();

/*
 * $Log: mbc_func.h,v $
 * Revision 1.2  1993/01/27  16:33:31  johnr
 * JR:  Changed INTERRUPT_PRINTER from e00000 to e00001.
 *
 * Revision 1.1  1993/01/21  18:58:24  johnr
 * JR:  .h files for the MIO physical interface.
 *
 *
 */
