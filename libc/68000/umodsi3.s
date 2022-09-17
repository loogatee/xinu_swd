
/*
 *	$Id: umodsi3.s,v 1.1 1992/11/10 16:24:46 johnr Exp $
 */

	.globl		___umodsi3

	.text

	.align 4


/*
 *	68000 unsigned long remainder  (mod)
 *
 *	D0 is unsigned dividend,  D1 is unsigned divisor
 *	compute:D1 is unsigned remainder, D0 is unsigned quotient
 */
___umodsi3:
	movl	sp@(4),d0		/* copy dividend from stack to register */
	movl	sp@(8),d1		/* copy divisor from stack to register */
	cmpl	#0x10000,d1		/* can word divide instruction be used? */
	bges	dov1			/* do loop if not */
	divu	d1,d0
	bvss	dov1			/* do loop if DIVU overflowed */
	movl	d0,d1			/* word divide instruction successful */
	clrw	d1
	swap	d1				/* remainder */
	andl	#0xffff,d0		/* quotient */
	bras	dout1
dov1:
	movb	#32,sp@-		/* 32 loops	0(A7) */
	clrl	d1				/* d1:d0 is 8 byte dividend. */
dulp1:
	lsll	#1,d0			/* assume 0 quotient bit by shifting in a 0 */
	roxll	#1,d1
	cmpl	sp@(10),d1
	blts	dunx1
	subl	sp@(10),d1		/* divisor fits */
	addl	#1,d0			/* set quotient bit */
dunx1:
	subqb	#1,sp@
	bnes	dulp1
	addql	#2,sp
dout1:
	movl	d1,d0
	rts
/*
 *	$Log: umodsi3.s,v $
 * Revision 1.1  1992/11/10  16:24:46  johnr
 * JR: 1st entry for these files.
 *
 */
