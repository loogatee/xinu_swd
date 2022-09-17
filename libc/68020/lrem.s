|signed long remainder: a = a % b

	.globl	lrem
	.text

lrem:	link	a6,#0
	moveml	#0x3800,sp@-	|need d2,d3,d4 registers
	movl	#1,d4		|sign of result
	movl	a6@(8),d0	|dividend
	bge	bb1
	negl	d0
	negl	d4
bb1:	movl	d0,d2		|save positive dividend
	movl	a6@(12),d1	|divisor
	bge	bb2
	negl	d1

bb2:	cmpl	#0x10000,d1	|divisor < 2 ** 16?
	bge	bb3		|no, divisor must be < 2 ** 16
	clrw	d0		|d0 =
	swap	d0		|   dividend high
	divu	d1,d0		|yes, divide
	movw	d2,d0		|d0 = remainder high + quotient low
	divu	d1,d0		|divide
	clrw	d0		|d0 = 
	swap	d0		|   remainder
	bra	bb6		|return

bb3:	movl	d1,d3		|save divisor
bb4:	asrl	#1,d0		|shift dividend
	asrl	#1,d1		|shift divisor
	andl	#0x7FFFFFFF,d0	|assure positive
	andl	#0x7FFFFFFF,d1	|  sign bit
	cmpl	#0x10000,d1	|divisor < 2 ** 16?
	bge	bb4		|no, continue shift
	divu	d1,d0		|yes, divide
	andl	#0xFFFF,d0	|erase remainder
	movl	d0,sp@-		|call ulmul with quotient
	movl	d3,sp@-		|  and saved divisor on stack
	jsr	ulmul		|  as arguments
	addql	#8,sp		|restore sp
	cmpl	d0,d2		|original dividend >= lmul result?
	jge	bb5		|yes, quotient should be correct
	subl	d3,d0		|no, fixup 
bb5:	subl	d2,d0		|calculate
	negl	d0		|  remainder

bb6:	tstl	d4		|sign of result
	bge	bb7
	negl	d0
bb7:	moveml	sp@+,#0x1C	|restore registers
	unlk	a6
	rts

