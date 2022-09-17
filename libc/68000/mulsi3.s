
/*
 *	$Id: mulsi3.s,v 1.1 1992/12/22 18:51:57 johnr Exp $
 */

	.globl		___mulsi3

	.text

	.align 4


___mulsi3:
	movl	sp@(4),d0		/* fill d0 from value on stack */
	movl	sp@(8),d1		/* fill d1 from value on stack */
	mulu	d1,d0
	mulu	sp@(4),d1
	swap	d1
	clrw	d1
	addl	d1,d0
	movew	sp@(6),d1
	mulu	sp@(8),d1
	swap	d1
	clrw	d1
	addl	d1,d0
	rts

