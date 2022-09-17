	.data
	.even

CACR_CLEAR	= 0x8
CACR_ENABLE	= 0x1
HIGH		= 0x2700
KB_VT100	= 0x1
KB_MS_103SD32	= 0x0
INITGETKEY	= 0xfef0034
V_KEYBID	= 0xfef003c
VEC5		= 0x74
VEC6		= 0x78


| This variable is used in the DELAY macro.  5 is the right value for
| 68010's running 10MHz.  3 is the right value for 68020's running 16MHz
| with cache on.  (4x as fast at doing the delay loop.)  Other values
| should be empirically determined as needed.  Srt0.s sets this value
| based on the actual runtime environment encountered.
|
| It's critical that the value be no SMALLER than required, e.g. the
| DELAY macro guarantees a MINIMUM delay, not a maximum.
	.globl	_cpudelay
	.globl _origSP


_cpudelay:
	.long	3			| Multiplier for DELAY macro.

|
|  save some things for when we return to the monitor
|
_origSP:
	.long	0			| SP when we start

	.text


|
| Startup code for Sun3 Xinu version 6

	.globl	__exit
	.globl	_start



_start:
	bra	_start1
	nop

	.long	_rom
	.long	_etext
	.long	size_ddata

_start1:
	movw	#HIGH,sr		| disable interrupts
leax:	lea	pc@(_start1-(leax-2)),a0	| True current location of "_start"
	lea	_start1:l,a1		| Desired      location of "_start"
	cmpl	a0,a1
	jeq	gobegin			| If the same, just go do it.
	movl	#_end,d0		| Desired end of program
	subl	a1,d0			| Calculate length, round up.
	lsrl	#2,d0
movc:	movl	a0@+,a1@+		| Move it where it belongs.
	dbra	d0,movc

gobegin:

	/*
	 * ------- Copy initialized data region into ram location -------
	 *
	 * This routine is meant to initialize the .data area.
	 * When the code is burned into eprom, the .data area is
	 * put into rom next to the code (.text), (starting at address
	 * "_etext".) We will need to move the .data section to it's proper
	 * home, (starting at "_ram", which is defined at build time).
	 * this assumes that -Tdata <addr> is the same as _ram.
	 *
	 * WARNING:	Possible Porting Problem
	 *
	 */
	lea		_ram,a1		/* a1 = address of RAM area */
	movl		a1,d1		/* same address into d1 */
	lea		_etext,a0	/* base of .data into a0 */
	lea		_edata,a2	/* end address of .data */
	movl		a2,d0		/* same address into d0 */
	subl		d1,d0		/* size of .data in d0 */
	lsrl		#2,d0		/* size in longs ( roundsize oks ) */
	bras		xcount		/* start loop count */
xmore:	movl		a0@+,a1@+	/* copy long */
xcount:	dbra		d0,xmore	/* assumes sizeof(.data) < 256K bytes */


|	See if we're supposed to auto-start
|
#	jsr	_autostart
#	tstl	_fautostart
#	bne	autostart
#	rts				| Return to caller (PROM probably)

autostart:


	/*
	 *	Zero out the .bss area
	 */
	lea		_edata,a1	/* a1 = start address of .bss area */
	movl		a1,d1		/* same address into d1 */
	lea		_end,a0		/* a0 = end address of .bss area */
	movl		a0,d0		/* same address into d0 */
	subl		d1,d0		/* size of .bss in d0 */
	movl		#0,d1		/* put a 0 into d1 */
	lsrl		#2,d0		/* size in longs ( roundsize oks ) */
	bras		xc1		/* start loop count */
xm1:	movl		d1,a1@+		/* copy long */
xc1:	dbra		d0,xm1		/* assumes sizeof(.data) < 256K bytes */



| save the current SP
	movl	sp,_origSP

|	moveq	#(is020-is010),d0		| Set up jump offset
| gas doesn't assemble this correctly.  Here is the proper instruction.
	.word	0x70f2


	.word	0x4EFB

|J:	.word	0x0200+(is010-J)-(is020-is010)
| gas doesn't assemble this correctly.  Here is the proper code.
J:	.word	0x021e

is020:
| Turn on the cache...
	moveq	#CACR_CLEAR+CACR_ENABLE,d0
	.long	0x4e7b0002		| movc	d0,cacr
	moveq	#3,d0			| Set the delay factor for 68020
	movl	d0,_cpudelay
is010:
	jmp	begin:l			| Force non-PCrel jump

begin:
| We assume we have a reasonable stack already and just borrow it...
	movl	#_edata,a0
clr:
	clrl	a0@+
	cmpl	#_end,a0
	ble	clr

| If we are using a VT100 keyboard we can not call initgetkey
	movl	V_KEYBID,a0
	movb	a0@,d0
	andb	0xf,d0
	cmpb	#KB_VT100, d0
	beq	1f
	cmpb	#KB_MS_103SD32, d0
	beq	1f
	movl	INITGETKEY,a0		| address of initgetkey
	jsr	a0@			| reinit keyboard in monitor
|
| Now, find out how much memory we have to use
|
1:
#	movl	0xFEF00B8@,_maxaddr
# gas doesn't assemble this correctly.  The next 3 longs make up
# the proper instruction.

	.long	0x23f001f1
	.long	0x0fef00b8
	.long	_maxaddr


	subql	#4,_maxaddr

| now, set the stack pointer to the top of memory
	movl	_maxaddr,sp

	
| start Xinu
	movl	#0,a6			| fix pm trace (FP)

	clrl	sp@-			| argv = 0 for now.
	clrl	sp@-			| argc = 0 for now.

	jsr	_nulluser
	addqw	#8,sp

__exit:
	rts				| Return to caller (PROM probably)
