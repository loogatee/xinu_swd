/* flash.c - x_flash */

#include <conf.h>
#include <kernel.h>
#include <mioaddrs.h>

typedef unsigned short	ushort;
typedef unsigned char	uchar;

static ushort	mfg_code;
static ushort	dev_code;

static uchar	burn_buf[50];

static
pflash()
{
	volatile	ushort		*cr_ptr;

	cr_ptr   = (ushort *)0;
	*cr_ptr  = 0x9090;
	mfg_code = *cr_ptr;
	dev_code = *(cr_ptr+1);
	*cr_ptr  = 0xffff;
}

/*------------------------------------------------------------------------
 *  x_flash  -  testing routine for flash stuff
 *------------------------------------------------------------------------
 */
COMMAND	x_flash(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	uchar	*sptr;
	uchar	*eptr;
	ushort	*shptr;
	int		(*fptr)();
	int		i;

	shptr = (ushort *)pflash;
	while (*shptr++ != 0x4e75)
		;

	sptr = (uchar *)pflash;
	eptr = (uchar *)shptr;
	fptr = (int(*)())burn_buf;

	for (i=0; sptr != eptr; ++i)
		burn_buf[i] = *sptr++;

	(*fptr)();

	fprintf(stdout,"mfg's code   = 0x%x\n",mfg_code);
	fprintf(stdout,"device code  = 0x%x\n",dev_code);
}
