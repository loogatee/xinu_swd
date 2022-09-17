/* $Id: ctxsw.s,v 1.1 1992/11/10 21:39:54 johnr Exp $ */
/****************************************************************/
/* ctxsw.s - perform a context switch							*/
/****************************************************************/
/*																*/
/*	This file contains the code that performs a Xinu			*/
/*	context switch.  When this routine exits, a new process		*/
/*	is executing.												*/
/*																*/
/****************************************************************/

/*
 *	INCLUDES
 */
#include <i960.h>
#include <proc.h>


			.cuthere

			.globl		_xa_ctxsw

			.text

			.ascii		"$Id: ctxsw.s,v 1.1 1992/11/10 21:39:54 johnr Exp $"

			.align 4

/*
 *	FUNCTION	xa_ctxsw	- Perform a Context switch
 *
 *	DESCRIPTION
 *		This routine performs a context switch.  That is, it allows the
 *		processor to begin executing code on behalf of a different task.
 *
 *	C CALLING CONVENTION
 *
 *		xa_ctxsw(oldregs,newregs);
 *
 *		where:
 *				oldregs is a pointer to the register save area for the
 *					currently executing task
 *
 *				newregs is a pointer to the register save area for the
 *					task which will be started
 *
 *	REGISTER USAGE
 *		g0	-	upon entry contains the address of oldregs
 *		g1	-	upon entry contains the address of newregs
 *		r5	-	keeps copy of g1
 *		r6	-	scratch
 *		r7	-	scratch
 *
 *	PARAMETERS
 *		g0	-	as mentioned, it contains the address of oldregs
 *		g1	-	as mentioned, it contains the address of newregs
 *
 *	RETURN VALUE
 *		none
 */
_xa_ctxsw:
			stq			g0,(g0)			/* 4 regs @ g0 to old save area */
			stq			g4,16(g0)		/* 4 regs @ g4 to old save area */
			stq			g8,32(g0)		/* 4 regs @ g8 to old save area */
			stq			g12,48(g0)		/* 4 regs @ g12 to old save area */
			stq			pfp,64(g0)		/* 4 regs @ r0 to old save area */

			mov			0,r6			/* Zero out r6 */
			modpc		r6,r6,r6		/* Read pc register, result in r6 */
			st			r6,80(g0)		/* store pc to old save area */

			flushreg					/* write cached regs to memory */

			mov			g1,r5			/* store ptr to new save area */

			ldq			(r5),g0			/* 4 regs @ g0 from new save area */
			ldq			16(r5),g4		/* 4 regs @ g4 from new save area */
			ldq			32(r5),g8		/* 4 regs @ g8 from new save area */
			ldq			48(r5),g12		/* 4 regs @ g12 from new save area */
			ldq			64(r5),pfp		/* 4 regs @ r0 from new save area */

			ld			80(r5),r6		/* saved pc register into r6 */
			ldconst		PC_PRIMASK,r7	/* pc priority mask into r7 */
			modpc		r7,r7,r6		/* load new pc */

			ret							/* done, return */

/*
 * MODIFICATION HISTORY:
 *
 * $Log: ctxsw.s,v $
# Revision 1.1  1992/11/10  21:39:54  johnr
# JR: 1st entry for all these files.
#
# Revision 1.1.1.1  1991/11/12  02:20:25  johnr
# JRs original Xinu distribution
#
# Revision 1.1  1991/11/12  02:20:24  johnr
# Initial revision
#
 *
 */
