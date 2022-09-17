/* $Id: i_handle.s,v 1.1 1992/11/10 21:39:59 johnr Exp $ */
/****************************************************************/
/* i_handle.s - exception handlers for the 960					*/
/****************************************************************/
/*																*/
/* This routine contains all of the exception handler assembly	*/
/* stubs for the 960.											*/
/*																*/
/****************************************************************/


		.cuthere

		.globl		_xa_intr1
		.globl		_xa_intr2
		.globl		_xa_intr3
		.globl		_xa_intr4
		.globl		_xa_intr5
		.globl		_xa_intr6
		.globl		_xa_intr7
		.globl		_xa_intr8
		.globl		_xa_intr9
		.globl		_xa_intr10
		.globl		_xa_intr11
		.globl		_xa_intr12
		.globl		_xa_intr13
		.globl		_xa_intr14
		.globl		_xa_intr15
		.globl		_xa_intr16
		.globl		_xa_intr17
		.globl		_xa_intr18
		.globl		_xa_intr19
		.globl		_xa_intr20
		.globl		_xa_intr21
		.globl		_xa_intr22
		.globl		_xa_intr23
		.globl		_xa_intr24
		.globl		_xa_intr25
		.globl		_xa_intr26
		.globl		_xa_intr27
		.globl		_xa_intr28
		.globl		_xa_intr29
		.globl		_xa_intr30
		.globl		_xa_intr31
		.globl		_xa_nmi

		.text

		.ascii		"$Id: i_handle.s,v 1.1 1992/11/10 21:39:59 johnr Exp $"

		.align		4

_xa_intr1:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		1,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr2:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		2,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr3:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		3,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr4:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		4,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr5:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		5,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr6:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		6,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr7:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		7,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr8:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		8,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr9:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		9,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr10:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		10,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr11:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		11,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr12:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		12,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr13:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		13,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr14:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		14,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr15:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		15,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr16:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		16,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr17:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		17,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr18:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		18,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr19:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		19,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr20:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		20,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr21:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		21,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr22:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		22,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr23:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		23,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr24:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		24,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr25:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		25,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr26:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		26,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr27:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		27,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr28:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		28,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr29:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		29,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr30:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		30,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_intr31:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		ldconst		31,g0			/* parameter for xf_printinterupt() */
		call		_xf_printinterupt	/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

		.align		4

_xa_nmi:
		ldconst		64,r4
		addo		sp,r4,sp		/* create some room on the stack */
		stq			g0,-64(sp)		/* save global registers on stack... */
		stq			g4,-48(sp)
		stq			g8,-32(sp)
		stt			g12,-16(sp)
		call		_xf_printnmi		/* call C routine to handle interupt */
		ldq			-64(sp),g0		/* restore globals... */
		ldq			-48(sp),g4
		ldq			-32(sp),g8
		ldt			-16(sp),g12
		ret

/*
 * MODIFICATION HISTORY:
 *
 * $Log: i_handle.s,v $
# Revision 1.1  1992/11/10  21:39:59  johnr
# JR: 1st entry for all these files.
#
# Revision 1.1.1.1  1991/11/12  02:20:32  johnr
# JRs original Xinu distribution
#
# Revision 1.1  1991/11/12  02:20:31  johnr
# Initial revision
#
 *
 */
