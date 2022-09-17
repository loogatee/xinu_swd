#ifndef lint
static char rcsid[] = "$Id: evec.c,v 1.1 1992/11/10 21:39:56 johnr Exp $";
#endif
/****************************************************************/
/* evec.c - set up the 960 exception vector table				*/
/****************************************************************/
/*																*/
/* This file contains the routines which initialize the			*/
/* exception vector tables on the 960.							*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include "kernel.h"
#include "st.std.h"


/*
 *	Imports
 */
extern	xa_intr1(),  xa_intr2(),  xa_intr3(),  xa_intr4(),  xa_intr5();
extern	xa_intr6(),  xa_intr7(),  xa_intr8(),  xa_intr9(),  xa_intr10();
extern	xa_intr11(), xa_intr12(), xa_intr13(), xa_intr14(), xa_intr15();
extern	xa_intr16(), xa_intr17(), xa_intr18(), xa_intr19(), xa_intr20();
extern	xa_intr21(), xa_intr22(), xa_intr23(), xa_intr24(), xa_intr25();
extern	xa_intr26(), xa_intr27(), xa_intr28(), xa_intr29(), xa_intr30();
extern	xa_intr31(), xa_nmi();

extern	bit32	*xv_itableba;			/* xinu's interrupt table base addr */

xf_initevec()
{
	register	bit32	*vecptr;
	register	int		i;

	vecptr = xv_itableba;
	vecptr = vecptr + 9;

	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr1;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr2;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr3;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr4;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr5;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr6;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr7;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr8;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr9;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr10;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr11;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr12;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr13;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr14;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr15;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr16;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr17;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr18;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr19;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr20;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr21;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr22;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr23;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr24;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr25;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr26;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr27;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr28;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr29;
	for (i=0; i < 8; ++i, ++vecptr)   *vecptr = (bit32)xa_intr30;

	/*
	 *	The vector immediately following the end of priority 30 is the
	 *		NMI vector.
	 */
	*vecptr++ = (bit32)xa_nmi;

	for (i=0; i < 7; ++i, ++vecptr)   *vecptr = (bit32)xa_intr31;
}


xf_printinterupt(vector)
int		vector;
{
	xp_kprintf("\nReceived interrupt, priority = 0x%x\n",vector);
}

xf_printnmi()
{
    xp_kprintf("\nReceived NMI\n");
}


xf_setevec(vecno,routine)
bit32		vecno;
bit32		routine;
{
	register	bit32	*vecptr;

	vecptr                =  xv_itableba;
	*(vecptr + vecno + 1) = routine;
}


/*
 * MODIFICATION HISTORY:
 *
 * $Log: evec.c,v $
 * Revision 1.1  1992/11/10  21:39:56  johnr
 * JR: 1st entry for all these files.
 *
 * Revision 1.1.1.1  1991/11/12  02:20:27  johnr
 * JRs original Xinu distribution
 *
 * Revision 1.1  1991/11/12  02:20:26  johnr
 * Initial revision
 *
 *
 */
