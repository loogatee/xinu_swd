#include <conf.h>
#include <kernel.h>
#include <systypes.h>
#include <mioaddrs.h>
#include <pdtypes.h>
#include <mbc_def.h>
#include <mbc_func.h>

typedef unsigned char	uchar;

extern	int			fttyoint[];
extern	int			tpg_sem;
extern	PAC_HEAD	*per_mess_ptr;
extern	BYTE		Mess_4_Per;
extern	UINT32		clktime;

static	UINT32		save_time;


c_mioisr()
{
	if (Mb->IO_status & (IOS_CLRREQ|IOS_IMBVLD))
		ReadMail();
}

cmn_isr(mport)
register	uchar	mport;
{
	volatile	register	uchar		*lsr_ptr;
				register	int			i;

	/*
	 *	Assign pointer to serial port register
	 */
	lsr_ptr = ACC_LSR;

	/*
	 *	xxx1xxxx   Check the MP_CLOCK bit.  Call the clock
	 *		handler if the bit is set
	 */
	if (mport & MP_CLOCK)
		c_clockisr();

	/*
	 *	x1xxxxxx   Check the MP_MIO bit.  Call the mio handler
	 *		if the bit is set.
	 */
	if (mport & MP_MIO)
		c_mioisr();

	/*
	 *	If the mail box is available and there is a message
	 *	for the printer, send it now
	 */
	if (Mess_4_Per && (Mb->PR_status == 0))
	{
		Mess_4_Per = FALSE;
		DeliverPacket(per_mess_ptr);
	}

	/*
	 *	xxxxxxx1  Check the Test-page button.  If the bit is
	 *		set, signal the semaphore which causes the test-page
	 *		process to run.
	 */
	if ((mport & MP_TP) == 0)
	{
		if ((clktime - save_time) >= 4)
		{
			save_time = clktime;
			signal(tpg_sem);
		}
	}

	/*
	 *	Check to see if there are characters to be read from
	 *	the serial device.  If so, call ttyiin() to process
	 *	the charcter(s).
	 */
	for (i=0; i < 4; ++i)
	{
		if ((*lsr_ptr & 1) != 0)
			ttyiin(0);
		else
			break;
	}

	/*
	 *	Check to see if there are characters available for
	 *	output to the serial device.  If so, and if the
	 *	serial fifo is available, then call ttyoin() in order to
	 *	transmit a character
	 */
	for (i=0; i < 6; ++i)
	{
		if (fttyoint[0] == TRUE)
		{
			if ((*lsr_ptr & 0x20) != 0)
				ttyoin(0);
			else
				break;
		}
		else
			break;
	}
}
