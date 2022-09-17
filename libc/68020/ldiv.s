|J. Test	1/81
|signed long division: quotient = dividend / divisor

	.globl	ldiv
	.text

ldiv:	link	a6,#0
	moveml	#0x3C00,sp@-	|need d2,d3,d4,d5 registers
	movl	#1,d5		|sign of result
	movl	a6@(8),d0	|dividend
	jge	bb1
	negl	d0
	negl	d5
bb1:	movl	d0,d3		|save positive dividend
	movl	a6@(12),d1	|divisor
	jge	bb2
	negl	d1
	negl	d5
bb2:	movl	d1,d4		|save positive divisor

	cmpl	#0x10000,d1	|divisor >= 2 ** 16?
	jge	bb3		|yes, divisor must be < 2 ** 16
	clrw	d0		|divide dividend
	swap	d0		|  by 2 ** 16
	divu	d1,d0		|get the high order bits of quotient
	movw	d0,d2		|save quotient high
	movw	d3,d0		|dividend low + remainder * (2**16)
	divu	d1,d0		|get quotient low
	swap	d0		|temporarily save quotient low in high
	movw	d2,d0		|restore quotient high to low part of register
	swap	d0		|put things right
	jra	bb5		|return

bb3:	asrl	#1,d0		|shift dividend
	asrl	#1,d1		|shift divisor
	andl	#0x7FFFFFFF,d0	|insure positive
	andl	#0x7FFFFFFF,d0	|  sign bit
	cmpl	#0x10000,d1	|divisor < 2 ** 16?
	jge	bb3		|no, continue shift
	divu	d1,d0		|yes, divide, remainder is garbage
	andl	#0xFFFF,d0	|get rid of remainder
	movl	d0,d2		|save quotient
	movl	d0,sp@-		|call ulmul with quotient
	movl	d4,sp@-		|  and saved divisor on stack
	jsr	ulmul		|  as arguments
	addql	#8,sp		|restore sp
	cmpl	d0,d3		|original dividend >= lmul result?
	jge	bb4		|yes, quotient should be correct
	subql	#1,d2		|no, fix up quotient

bb4:	movl	d2,d0		|move quotient to d0
bb5:	tstl	d5		|sign of result
	jge	bb6
	negl	d0
bb6:	moveml	sp@+,#0x3C	|restore registers
	unlk	a6
	rts

