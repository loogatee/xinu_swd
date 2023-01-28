#include <conf.h>
#include <kernel.h>
#include <systypes.h>
#include <mioaddrs.h>

typedef unsigned char	uchar;

extern	int				fttyoint[];
extern	int				tpg_sem;
extern	unsigned long	clktime;


static	unsigned long	save_time;


cmn_isr(mport)
uchar	mport;
{
	volatile	register	uchar	*lsr_ptr;
				register	int		i;

	lsr_ptr = ACC_LSR;

	if (mport & MP_CLOCK)
		c_clockisr();

	if ((mport & MP_TP) == 0)
	{
		if ((clktime - save_time) >= 4)
		{
			save_time = clktime;
			signal(tpg_sem);
		}
	}

	for (i=0; i<4; ++i)
	{
		if ((*lsr_ptr & 1) != 0)
			ttyiin(0);
		else
			break;
	}

	for (i=0; i<6; ++i)
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
