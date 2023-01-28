

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_def.h>
#include <mbc_func.h>
#include <mbc_text.h>

/*
 *	EXTERNS
 */
extern BYTE			Mess_4_Per;		/* When set, watchdog must send message */
extern BYTE			UpdateNVRAM;
extern IO_STATUS	IOStatus;
extern int			IntrMIO;
extern int			IntrDespooler;
extern BYTE			DataReady;
extern	int			xv_tracepri;

extern	SINT16		InQueue;

/*
 *	GLOBALS
 */
BYTE	Junk[64];			/* Series IV does an ASCII_STATUS, need storage */
BYTE	MIO_Error;
BYTE	MIO_Mail; 
UINT16	ReadBlockPool;
UINT16	WriteBlockPool;
UINT16	in_sec, out_sec, in_10, out_10;

/*
 *	LOCALS
 */
static	BYTE	ErrorFlagged = 1;		/* 1st time, tell printer online */
static	UINT16	Counter = 0;


backend_write(buf,len,eof_flag)
char	*buf;
int		len;
int		eof_flag;
{
	STATWORD	ps;
	int			send_flag;

	disable(ps);

	while (1)
	{
		IOStatus.holding_data = TRUE;

		if (PrinterStatus.idle_mode == IDLE_MODE)
			send_flag = 0;
		else
			send_flag = 1;

		if (send_flag && !Mess_4_Per)
		{
			if (ReadBlockPool >= 2)
			{
				WritePrn(buf,len,&ReadBlockPool,eof_flag);
				IOStatus.holding_data = FALSE;
				restore(ps);
				return;
			}
			else
				sleep10(2);
		}
		else
			sleep10(2);
	}
}


/***********************************************************************

   MIO:     This function should glue the printer to my MBC code which
            in turn glues the front code.

   input:   None

   output:  None

   returns: None

***********************************************************************/

PROCESS
MBC_Main()
{
	UINT16			numbytes;
	BYTE			i;
	BYTE			retval;
	BYTE			reply;
	STATWORD		ps;

	/*
	 *	Initialize MIO board variables and build transport packets
	 *	to communicate with the printer
	 */
	miostart();

	IOStatus.holding_data = FALSE;

	while (1)
	{
		/*
		 *	Wait for the printer to interrupt
		 */
		wait(IntrMIO);

		/*
		 *	Test to see if the printer sent us any mail.  If
		 *	it has, perform the appropriate services.
		 */
		++Counter;
		while (MIO_Mail)
			ProcessMail(&retval);

		/*
		 *	If any errors are detected and they affect the 
		 *	printer, let the printer know we're having problems.
		 */
		if (MIO_Error && !ErrorFlagged)
		{
			SendMail(ASYNC_IO_STATUS, 0, 1);			/* Error 1 */
			ErrorFlagged = TRUE;
		}

		/*
		 *	If we were in an error state and the error has been
		 *	cleared, inform the printer that we can go back online.
		 */
		if (!MIO_Error && ErrorFlagged)
		{
			SendMail(ASYNC_IO_STATUS, 0, 0);
			ErrorFlagged = FALSE;
		}

		/*
		 *	Test to see if the printer has any data to send
		 *	back to the host.
		 */
		if (WriteBlockPool)
		{
			--WriteBlockPool;				/* Someday we'll have do */
			numbytes = ReadPrn(Junk);		/* something with this. */
		}

#ifdef NOTYET
		/*
		 *	If UpdateNVRAM is set and we need to update the FLASH RAM.
		 */
		if( UpdateNVRAM && OnLine )
		{
			UpdateFlash();
			UpdateNVRAM = FALSE;
		}
#endif
	}				/* end of while (1) */
}

/*
 *	$Log: mbc_main.c,v $
 * Revision 1.4  1993/01/30  03:17:53  johnr
 * JR:  Got rid of the globals OnLine and SendIt.  Got rid of the volatile
 *      descriptor for MIO_Mail.  Added eof_flag as a parameter to
 *      backend_write.  Greatly modified backend_write so that it is
 *      the function which calls WritePrn in order to send data to the MIO
 *      interface.  Got rid of some variables which were no longer needed
 *      in MBC_Main.  Took out "xx=0" debug stuff in MBC_Main.  Also got
 *      rid of the assignments to OnLine and SendIt in MBC_Main.
 *
 *      The main change was that backend_write now sends the data directly,
 *      instead of signalling MBC_Main to do it.
 *
 * Revision 1.3  1993/01/27  16:26:19  johnr
 * JR:  Added xv_tracepri = 82 in here.  This replaces the same statement in
 *      miostart.
 *
 * Revision 1.2  1993/01/27  03:01:29  johnr
 * JR:  Developmental checkin.  Status is that miostart completes successfully,
 *      but I get a service error 09 displayed on the front panel.  Board does
 *      not hang.
 *
 * Revision 1.1  1993/01/26  16:55:12  johnr
 * JR:  Initial entry for all these files.  These files are all from Ted's
 *      original MIO driver code, and I have already modified them slightly.
 *
 * Revision 1.1  1992/12/22  18:39:54  johnr
 * JR:  Initial build files for mio_mbc.  This code has no networking, but
 *      does have an MIO interface.
 *
 *
 */
