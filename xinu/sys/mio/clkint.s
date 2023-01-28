

#		.cuthere

		.globl		_rtcintr

		.text

_rtcintr:
		movw		#0x2700,sr
		moveml		d0/d1/a0/a1,sp@-	/* save registers */
		eorl		d0,d0				/* zero out d0 */
		movb		0xf00001,d0			/* read the Misc. port */
		movl		d0,sp@-				/* push parameter on the stack */
		jsr			_cmn_isr			/* call common ISR routine */
		addql		#4,sp				/* adjust the sp */
		moveml		sp@+,d0/d1/a0/a1	/* restore registers */
		rte								/* get out of here */
