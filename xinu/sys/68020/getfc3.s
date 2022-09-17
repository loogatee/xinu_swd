| getfc3.s - getfc3()

	.text

	.globl	_getfc3
	.globl	_setfc3


_getfc3:
		movl	#3,d0		| Not sure why this statement is needed
		movc	d0,sfc		| Not sure why this statement is needed
#		.long	0x4e7b0000

		movl	sp@(4),d0	| size in d0
		movl	sp@(8),a0	| addr in a0

		cmpl	#1,d0		| size == sizeof(u_char) ??
		bnes	bb1			| if not branch ahead
		movsb	a0@,d0		| yes, set the return value
#		.long	0x0e100000
		bras	bb_ret		| and branch ahead to return
bb1:
		cmpl	#2,d0		| size == sizeof(u_short) ??
		bnes	bb2			| if not branch ahead
		movsw	a0@,d0		| yes, set the return value
#		.long	0x0e500000
		bras	bb_ret		| and branch ahead to return
bb2:
		cmpl	#4,d0		| size == sizeof(u_long) ??
		bnes	bb_ret		| if not branch ahead
		movsl	a0@,d0		| yes, set the return value
#		.long	0x0e900000
bb_ret:
		rts


_setfc3:
		movl	#3,d0		| Not sure why this statement is needed
		movc	d0,dfc		| Not sure why this statement is needed
#		.long	0x4e7b0001

		movl	sp@(4),d0	| size in d0
		movl	sp@(8),a0	| addr in a0
		movl	sp@(12),d1	| entry in d1

		cmpl	#1,d0		| size == sizeof(u_char) ??
		bnes	cc1			| if not branch ahead
		movsb	d1,a0@		| yes, set the value
#		.long	0x0e101800
		bras	cc_ret		| and branch ahead to return
cc1:
		cmpl	#2,d0		| size == sizeof(u_short) ??
		bnes	cc2			| if not branch ahead
		movsw	d1,a0@		| yes, set the value
#		.long	0x0e501800
		bras	cc_ret		| and branch ahead to return
cc2:
		cmpl	#4,d0		| size == sizeof(u_long) ??
		bnes	cc_ret		| if not branch ahead
		movsl	d1,a0@		| yes, set the return value
#		.long	0x0e901800
cc_ret:
		rts
