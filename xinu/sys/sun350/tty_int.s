/* ttyint.s -  Asm_ttyint */


ZCONTROLA = 0xfe02004	/* A serial line */
ZCONTROLB = 0xfe02000	/* B serial line */
ZCONTROLM = 0xfe00000	/* Mouse serial line */

|*------------------------------------------------------------------------
|* ttyint  --  zilog chip interrupt service routine
|*------------------------------------------------------------------------
	.globl	_Asm_ttyint


	.text

_Asm_ttyint:
	movw	sr, _savePS		/* save current interrupts status */
	movw	#0x2700, sr		/* disable all interrupts */
	moveml	#0xc1c0, sp@-		/* save d0, d1, d7, a0, a1 */
#	jsr	_jrshow0		/* counter, plus tracev message */

	/*
	 * Read from register 2 in Channel B in order to
	 * determine which channel caused the interrupt.
	 */
	movl	#ZCONTROLB,a0		/* address of channel-B into a0 */
	movb	#2,a0@			/* need to read reg 2 */
	bsr	ddelay			/* need to delay before reading */
	movb	a0@,d7			/* d7 = RR3 */
	btst	#3,d7			/* xxxx 1xxx = interrupt pending - B */
	beq	chan_1			/* goto chan_1 if pending on B... */

chan_0:					/* ...else drop thru to do channel-A */
#	jsr	_jrshow1		/* counter, plus tracev message */

	/*
	 * read from RR0 of the chip - Channel A
	 */
	movl	#ZCONTROLA,a0		/* address of A channel */
	movb	#0,a0@			/* select register 0 */
	bsr	ddelay			/* need to delay before reading */
	movb	a0@,d7			/* d7 = RR0 */

	/*
	 * test for ttyline break - Channel A
	 */
	btst	#7,d7			/* 1xxx xxxx = break detected */
	beq	nobreak0		/* jump ahead if no break */
	pea	0			/* for channel-A */
	jsr	_ttybreak		/* call break routine */
	addql	#4,sp			/* adjust the stack */
nobreak0:

	/*
	 * read from RR3 of the chip - Channel A
	 */
	movl	#ZCONTROLA,a0		/* address of A channel */
	movb	#3,a0@			/* select register 3 */
	bsr	ddelay			/* need to delay before reading */
	movb	a0@,d7			/* d7 = RR3 */

	/*
	 * test for transmit interrupt - Channel A
	 */
	btst	#4,d7			/* xxx1 xxxx = xmit interupt pending */
	beq	noxmit			/* if no xmit, branch ahead */
#	jsr	_jrshow2		/* counter, plus tracev message */
	pea	0			/* for channel-A */
	jsr	_ttyoin			/* call output interrupt routine */
	addql	#4,sp			/* adjust stack */
noxmit:

	/*
	 * test for receive interrupt - Channel A
	 */
	btst	#5,d7			/* xx1x xxxx = recv int. pending */
	beq	norecv			/* if no receive, branch ahead */
#	jsr	_jrshow3		/* counter, plus tracev message */
	pea	0			/* for channel-A */
	jsr	_ttyiin			/* call input interrupt routine */
	addql	#4,sp			/* adjust stack */
norecv:

	/*
	 * reset the interrupt for channel A
	 */
	movl	#ZCONTROLA,a0		/* address of A channel */
	movb	#0x38,a0@		/* reset interrupt */
	bra	chan_x			/* done with chan-A, skip ahead */

chan_1:					/* start here for channel-B */
#	jsr	_jrshow4		/* counter, plus tracev message */

	/*
	 * read from RR0 of the chip - Channel B
	 */
	movl	#ZCONTROLB,a0		/* address of B channel */
	movb	#0,a0@			/* select register 0 */
	bsr	ddelay			/* need to delay before reading */
	movb	a0@,d7			/* d7 = RR0 */

	/*
	 * test for ttyline break - Channel B
	 */
	btst	#7,d7			/* 1xxx xxxx = break detected */
	beq	nobreak1		/* if no break, branch ahead */
	pea	1			/* for channel-B */
	jsr	_ttybreak		/* call break routine */
	addql	#4,sp			/* adjust stack */
nobreak1:

	/*
	 * read from RR3 of the chip - Channel A
	 *	Comment from zsreg.h:
	 *		bits in RR3 -- Interrupt Pending flags for
	 *		both channels (this reg can only be read in
	 *		Channel A, tho)
	 */
	movl	#ZCONTROLA,a0		/* address of A channel */
	movb	#3,a0@			/* select register 3 */
	bsr	ddelay			/* need to delay before reading */
	movb	a0@,d7			/* d7 = RR3 */

	/*
	 * test for transmit interrupt - Channel B
	 */
	btst	#1,d7			/* xxxx xx1x = xmit detected */
	beq	noxmitb			/* if no xmit, branch ahead */
#	jsr	_jrshow5		/* counter, plus tracev message */
	pea	1			/* for channel-B */
	jsr	_ttyoin			/* call output interrupt routine */
	addql	#4,sp			/* adjust stack */
noxmitb:

	/*
	 * test for receive interrupt - Channel B
	 */
	btst	#2,d7			/* xxxx x1xx = recv int. pending */
	beq	norecvb			/* if no receive, branch ahead */
#	jsr	_jrshow6		/* counter, plus tracev message */
	pea	1			/* for channel-B */
	jsr	_ttyiin			/* call input interrupt routine */
	addql	#4,sp			/* adjust stack */
norecvb:

	/*
	 * reset the interrupt for channel B
	 */
	movl	#ZCONTROLB,a0		/* address of B channel */
	movb	#0x38,a0@		/* reset interrupt */

chan_x:
#	jsr	_jrshow7		/* counter, plus tracev message */

	moveml	sp@+,#0x0383		/* restore d0, d1, d7, a0, a1 */
	movw	_savePS, sr		/* return original content of sr */
	rte				/* Return from interrupt */



ddelay:
	nop	/* 1 */
	nop	/* 2 */
	nop	/* 3 */
	nop	/* 4 */
	nop	/* 5 */
	nop	/* 6 */
	nop	/* 7 */
	nop	/* 8 */
	nop	/* 9 */
	nop	/* 10 */
	nop	/* 11 */
	nop	/* 12 */
	rts
