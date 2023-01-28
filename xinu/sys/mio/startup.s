

BASE_RAM  = 0x380000
MEG_1RAM  = 0x400000
MEG_4RAM  = 0x500000
MEG_4RAME = 0x800000



#		.cuthere

		.globl		_maxaddr
		.globl		_start

		.comm		_maxaddr,4

		.text

_start:	bra			st1
		nop

		.long		_rom
		.long		_etext
		.long		size_ddata
		.long		0

st1:	movl		#0x11223344,d0		/* some value into d0 */
		movl		d0,MEG_1RAM			/* write it to start of 1Meg exp */
		movl		MEG_1RAM,d1			/* read same location into d1 */
		cmpl		d0,d1				/* compare original with read value */
		bnes		st2					/* branch ahead if different */

		movl		#0x44332211,d2		/* new value into d2 */
		movl		d2,MEG_4RAM			/* write to area of 4Meg expansion */
		movl		MEG_4RAM,d1			/* read from the same spot */
		cmpl		d2,d1				/* compare original with read value */
		bnes		st3					/* branch ahead if different ... */
										/* ... else check "mirror" condition */
		movl		MEG_1RAM,d1			/* Read from MEG_1RAM again */
		cmpl		d2,d1				/* compare value written to MEG_4RAM */
		beqs		st3					/* if equal, then mirror condition */

		movl		#MEG_4RAME,d0		/* memory = 4Meg + 512k */
		bras		st4

st2:	movl		#MEG_1RAM,d0		/* memory = 512k */
		bras		st4

st3:	movl		#MEG_4RAM,d0		/* memory = 1Meg + 512k */

st4:
		movl		d0,d4				/* save for later use */
		subql		#4,d4				/* decrement maxaddr by 1 */
		subl		#16,d0				/* start sp on valid 16-byte bound */
		movl		d0,sp				/* set starting sp */

	/*
	 * ------- Copy initialized data region into ram location -------
	 *
	 * This routine is meant to initialize the .data area.
	 * When the code is burned into eprom, the .data area is
	 * put into rom next to the code (.text), (starting at address
	 * "_etext".) We will need to move the .data section to it's proper
	 * home, (starting at "ram_", which is defined at build time).
	 * this assumes that -Tdata <addr> is the same as ram_.
	 *
	 * WARNING:	Possible Porting Problem
	 *
	 */
		lea		_ram,a1		/* a1 = address of RAM area */
		movl	a1,d1		/* same address into d1 */
		lea		_etext,a0	/* base of .data into a0 */
		lea		_edata,a2	/* end address of .data */
		movl	a2,d0		/* same address into d0 */
		subl	d1,d0		/* size of .data in d0 */
		lsrl	#2,d0		/* size in longs ( roundsize oks ) */
		bras	xcount		/* start loop count */
xmore:	movl	a0@+,a1@+	/* copy long */
xcount:	dbra	d0,xmore	/* assumes sizeof(.data) < 256K bytes */

		/*
		 *	Zero out the .bss area
		 */
		lea		_edata,a1	/* a1 = start address of .bss area */
		movl	a1,d1		/* same address into d1 */
		lea		_end,a0		/* a0 = end address of .bss area */
		movl	a0,d0		/* same address into d0 */
		subl	d1,d0		/* size of .bss in d0 */
		movl	#0,d1		/* put a 0 into d1 */
		lsrl	#2,d0		/* size in longs ( roundsize oks ) */
		bras	xc1			/* start loop count */
xm1:	movl	d1,a1@+		/* copy long */
xc1:	dbra	d0,xm1		/* assumes sizeof(.data) < 256K bytes */

		movl		d4,_maxaddr
		movl		#0,a6

		jsr			_nulluser

		.short		0x4e4e
