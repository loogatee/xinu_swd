|* intr.s - disable(), restore(), enable(), pause(), halt()


	.globl	_enable
	.globl	_disable
	.globl	_restore
	.globl	_pause
	.globl	_halt
	.text

|---------------------------------------------------------
| pause: halt the processor until an interrupt occurs
|---------------------------------------------------------
_pause:
	stop #0x2000
	rts


|---------------------------------------------------------
| halt: do nothing forever
|---------------------------------------------------------
_halt:
	jmp _halt
	rts


|---------------------------------------------------------
| enable all interrupts
|---------------------------------------------------------
_enable:
	movw #0x2000, sr
	rts


|---------------------------------------------------------
| disable(ps)    - disable interrupts, save old state in ps
| STATWORD ps  (short *ps)
|---------------------------------------------------------
_disable:
	movl	sp@(0x4), a0
	movw	sr,a0@
	movw	#0x2700, sr
	rts


|---------------------------------------------------------
| restore(ps)    - restore interrupts to value in ps
| STATWORD ps    (short *ps)
|---------------------------------------------------------
_restore:
	movl	sp@(0x4),a0
	movw	a0@, sr
	rts
