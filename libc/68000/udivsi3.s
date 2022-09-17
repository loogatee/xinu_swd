
/*
 *	$Id: udivsi3.s,v 1.1 1992/11/10 16:24:44 johnr Exp $
 */

	.globl		___udivsi3

	.text

	.align 4

/*
 *	68000 unsigned long divide
 *
 *	D0 is unsigned dividend,  D1 is unsigned divisor
 *	compute:D1 is unsigned remainder, D0 is unsigned quotient
 */
___udivsi3:
	movl	sp@(4),d0		/* copy dividend from stack to register */
	movl	sp@(8),d1		/* copy divisor from stack to register */
	cmpl	#0x10000,d1		/* can word divide instruction be used? */
	bges	dov0			/* do loop if not */
	divu	d1,d0
	bvss	dov0			/* do loop if DIVU overflowed */
	movl	d0,d1			/* word divide instruction successful */
	clrw	d1
	swap	d1				/* remainder */
	andl	#0xffff,d0		/* quotient */
	bras	dout0
dov0:
	movb	#32,sp@-		/* 32 loops	0(A7) */
	clrl	d1				/* d1:d0 is 8 byte dividend. */
dulp0:
	lsll	#1,d0			/* assume 0 quotient bit by shifting in a 0 */
	roxll	#1,d1
	cmpl	sp@(10),d1
	blts	dunx0
	subl	sp@(10),d1		/* divisor fits */
	addl	#1,d0			/* set quotient bit */
dunx0:
	subqb	#1,sp@
	bnes	dulp0
	addql	#2,sp
dout0:
	rts
