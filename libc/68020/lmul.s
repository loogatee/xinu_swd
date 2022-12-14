|J. Test	1/81
|signed long multiply: c = a * b

	.globl	lmul
	.text

lmul:	link	a6,#0
	moveml	#0x3800,sp@-	|save d2,d3,d4
	movl	#1,d4		|sign of result
	movl	a6@(8),d2	|d2 = a
	bge	bb1
	negl	d2
	negl	d4
bb1:	movl	a6@(12),d3	|d3 = b
	bge	bb2
	negl	d3
	negl	d4

bb2:	clrl	d0
	movw	d2,d0		|d0 = alo, unsigned
	mulu	d3,d0		|d0 = blo*alo, unsigned
	movw	d2,d1		|d1 = alo
	swap	d2		|swap alo-ahi
	mulu	d3,d2		|d2 = blo*ahi, unsigned
	swap	d3		|swap blo-bhi
	mulu	d3,d1		|d1 = bhi*alo, unsigned
	addl	d2,d1		|d1 = (ahi*blo + alo*bhi)
	swap	d1		|d1 =
	clrw	d1		|   (ahi*blo + alo*bhi)*(2**16)
	addl	d1,d0		|d0 = alo*blo + (ahi*blo + alo*bhi)*(2**16)
	tstl	d4		|sign of result
	bge	bb3
	negl	d0

bb3:	moveml	sp@+,#0x1C	|restore d2,d3,d4
	unlk	a6
	rts

