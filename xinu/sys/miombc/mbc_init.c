/* $Id: mbc_init.c,v 1.5 1993/01/28 01:43:47 johnr Exp $ */

#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_func.h>
#include <mbc_def.h>
#include <ivect.h>
#include <mioaddrs.h>

extern	void	miointr();

extern	BYTE	MIO_Mail;


/*
 *	FUNCTION	mioinit - begin to initialize MIO interface
 *
 *	DESCRIPTION
 *		This function must be called at power up and after hard-
 *		ware resets. This function will test the mailboxes and zero
 *		them out when finished. NOTE: The printer expects this to
 *		be completed within two seconds after MIO nRESET line is
 *		de-asserted.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
int
mioinit()
{
	register	UINT32	i;
	register	UINT16	*ptr;
				UINT32	*sys_ptr;	/* pointer to system address table */
				UINT32	(*fptr)();	/* pointer to a function */
				UINT32	bavecs;		/* base address - interrupt vectors */
				UINT32	l2iv_addr;	/* address of level 2 interrupt vector */

	sys_ptr   = (UINT32 *)ADDR_SAT;
	fptr      = (UINT32(*)())*sys_ptr;
	bavecs    = (*fptr)();
	l2iv_addr = bavecs + L2IV + JMPOFF;

	*((UINT32 *)l2iv_addr) = (UINT32)miointr;

	/*
	 *	Fill mailboxes with a test pattern.
	 */
	for (ptr = (UINT16 *)Mb, i = 0; i < 8; i++)
		*ptr++ = 0xA500;

	/*
	 *	Verify the pattern just written is correct
	 *	and try another pattern.
	 */
	for (ptr = (UINT16 *)Mb, i = 0; i < 8; i++)
	{
		if (*ptr != 0xA500)						/* We've got problems */
			HangMIO(MB_TEST_ERROR);
		else
			*ptr++ = 0x5A00;
	}

	/*
	 *	Again, test the pattern and then zero out the mailboxes.
	 */
	for (ptr = (UINT16 *)Mb, i = 0; i < 8; i++)
	{
		if (*ptr != 0x5A00)						/* We've got problems */
			HangMIO(MB_TEST_ERROR);
		else
			*ptr++ = 0;
	}

	/*
	 *	Finally, make sure all the mailbox bytes are zero.
	 */
	for (ptr = (UINT16 *)Mb, i = 0; i < 8; ptr++, i++)
	{
		if (*ptr != 0)							/* We've got problems */
			HangMIO(MB_TEST_ERROR);
	}

	/*
	 *	Inform the printer we're about ready to perform
	 *	selftest. We'll assume a maximum of 10 seconds
	 *	for the MIO board to perform its selftest.
	 */
	unpack(Mb->PR_pointer, 10);
	Mb->PR_status |= PRS_TMOUT;					/* Set tm_out bit */

	/*
	 *	Verify that the shared memory works.
	 *	Fill shared memory with a test pattern.
	 */
	ptr = (UINT16 *)Mb + 8;
	for (i = 8; i < MAX_SHARE_MEM; i++)			/* Skip every other byte */
		*ptr++ = 0xA500;

	/*
	 *	Verify the pattern just written is correct and then
	 *	zero out shared memory.
	 */
	ptr = (UINT16 *)Mb + 8;
	for (i = 8; i < MAX_SHARE_MEM; i++)			/* Skip every other byte */
	{
		if (*ptr != 0xA500)						/* We've got problems */
			HangMIO(MB_TEST_ERROR);
		else
			*ptr++ = 0;
	}

	/*
	 *	Finally, make sure all the shared memory is zero.
	 */
	ptr = (UINT16 *)Mb + 8;
	for (i = 8; i < MAX_SHARE_MEM; i++)			/* Skip every other byte */
	{
		if (*ptr != 0)							/* We've got problems */
			HangMIO(MB_TEST_ERROR);
	}

	return(0);
}




/*
 * $Log: mbc_init.c,v $
 * Revision 1.5  1993/01/28  01:43:47  johnr
 * JR:  Changed all instances of "Mb + 16"  to  "Mb + 8"
 *
 * Revision 1.4  1993/01/27  18:38:21  johnr
 * JR:  Took out the c_mioisr() routine.
 *
 * Revision 1.3  1993/01/27  17:24:41  johnr
 * JR:  Took out the disable() and restore() function calls surrounding
 *      the call to ReadMail in c_mioisr.
 *
 * Revision 1.2  1993/01/27  03:01:27  johnr
 * JR:  Developmental checkin.  Status is that miostart completes successfully,
 *      but I get a service error 09 displayed on the front panel.  Board does
 *      not hang.
 *
 * Revision 1.1  1993/01/26  16:55:09  johnr
 * JR:  Initial entry for all these files.  These files are all from Ted's
 *      original MIO driver code, and I have already modified them slightly.
 *
 *
 */
