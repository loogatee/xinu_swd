|
|  trap.s -- handle Xinu traps elegantly so we can return to the monitor
	.text
	.even

	.globl _trapX


_trapX:
	jbsr	_doevec

