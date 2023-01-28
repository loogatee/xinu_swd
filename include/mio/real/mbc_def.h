/* $Id: mbc_def.h,v 1.1 1993/01/21 18:58:22 johnr Exp $ */



/*
 *	The following structure defines the mailboxes found
 *	in location zero of the MIO's data space.
 */
typedef		struct MAILBOX_tag
{
	UINT16	IO_status;
	UINT16	IO_error;
	UINT16	IO_pointer[2];
	UINT16	PR_status;
	UINT16	PR_error;
	UINT16	PR_pointer[2];
} MAILBOX;


/*
 *	Defines for the IO_status field of the MAILBOX.
 *		See MIO spec, page 21.
 */
#define		IOS_CLRREQ		0x0200
#define		IOS_IMBVLD		0x0100

/*
 *	Defines for the PR_status field of the MAILBOX
 *		See MIO spec, page 22.
 */
#define		PRS_IORSV		0x8000
#define		PRS_RSV			0x4000
#define		PRS_TMOUT		0x0400
#define		PRS_CLRRSP		0x0200
#define		PRS_PMBVLD		0x0100


/*
 *	This structure defines the packet header
 *	used by both the printer & MIO board.
 */
typedef		struct PAC_HEAD_tag
{
	UINT16	dev_spec[8];
	UINT16	command;
	UINT16	command_mod;
	UINT16	rtn_status;
	UINT16	buf_type;
	UINT16	start_data[2];
	UINT16	data_len[2];
	UINT16	max_data_len[2];
} PAC_HEAD;


/*
 *	This next structure defines the mail message
 *	list. It tracks the mail sent by the printer.
 */
typedef		struct MESS_LIST_tag
{
	UINT32	*packet_ptr;
	BYTE	cmd;
	BYTE	cmd_mod;
	BYTE	next_idx;
} MESS_LIST;

#define		MESS_LIST_SIZE		100		/* size of the mail message list */


typedef 	struct PRINTER_STATUS_tag
{
	BYTE	off_line;				/*  Minimum requirements */
	BYTE	paper_out;				/*     "         "       */
	BYTE	intervention;			/*     "         "       */
	BYTE	idle_mode;				/*     "         "       */
	BYTE	connect_ack;			/*     "         "       */
	BYTE	printer_err;			/*     "         "       */
	BYTE	reset_io_channel;
	BYTE	printer_req_paper;
	BYTE	paper_jam;
	BYTE	toner_problem;
	BYTE	page_punt;
	BYTE	memory_out;
	BYTE	io_channel_inactive;
	BYTE	printer_busy;
	BYTE	printer_wait_data;
	BYTE	printer_init;
	BYTE	cover_open;
	BYTE	printer_printing;
	BYTE	paper_output_prob;
} PRINTER_STATUS;


typedef		struct IO_STATUS_tag
{
	BYTE	holding_data;
	BYTE	io_error;
	BYTE	link;
	BYTE	io_idle_mode;
	BYTE	obsolete;
	BYTE	io_not_ready;
	BYTE	connection_state;
} IO_STATUS;

/*
 *	This next section extern's the globally defined
 *	variables.
 */
extern BYTE				Language;		/* Specifies current lang selection */
extern BYTE				DisplayType;	/* Specifies printer LCD config */
extern IO_STATUS		IOStatus;		/* Contains the lastest MIO info */
extern PRINTER_STATUS	PrinterStatus;	/* Has the lastest printer info */

/*
 *	$Log: mbc_def.h,v $
 * Revision 1.1  1993/01/21  18:58:22  johnr
 * JR:  .h files for the MIO physical interface.
 *
 *
 */
