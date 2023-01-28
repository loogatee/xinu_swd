/* evec.c -- initevec, doevec */

#include <conf.h>
#include <kernel.h>
#include <ivect.h>
#include <mioaddrs.h>

typedef	unsigned long	ulong;
typedef	unsigned char	uchar;

#define		NUMEVEC		48

int		doevec();

static	uchar	(*ivect)();
static	ulong	gbavecs;
static	ulong	evec_org[NUMEVEC];
static	ulong	evec_xinu[NUMEVEC];

save_vectors()
{
	register	ulong	bavecs;
	register	int		i;
	register	ulong	*lptr;

	for (bavecs=gbavecs, i=0; i < NUMEVEC; ++i)
	{
		lptr         = (ulong *)(bavecs + JMPOFF);
		evec_xinu[i] = *lptr;
		bavecs      += NXTVEC;
	}
}

restore_vectors()
{
	register	ulong	bavecs;
	register	int		i;
	register	ulong	*lptr;

	for (bavecs=gbavecs, i=0; i < NUMEVEC; ++i)
	{
		lptr         = (ulong *)(bavecs + JMPOFF);
		*lptr        = evec_xinu[i];
		bavecs      += NXTVEC;
	}
}

restore_old_vectors()
{
	register	ulong	bavecs;
	register	int		i;
	register	ulong	*lptr;

	for (bavecs=gbavecs, i=0; i < NUMEVEC; ++i)
	{
		lptr         = (ulong *)(bavecs + JMPOFF);
		*lptr        = evec_org[i];
		bavecs      += NXTVEC;
	}
}

/*
 *		Each vector points to a code stub that appears in ram as follows:
 *
 *			movb	#VECNUM,i_vect		// don't ever change this
 *			jmp		handler:l			// handler address may be changed
 *
 *		There are 48 vectors that point to this "ram-based" stub:
 *			2..11    (10)
 *			24..45   (22)
 *			64..79   (16)
 *                 -------
 *                    48 = NUMEVEC
 *
 *		See init.s of the monitor code for further details.
 */
initevec()
{
	ulong	*sys_ptr;
	ulong	(*fptr)();
	ulong	bavecs;
	ulong	*lptr;
	int		i;

	/*
	 *	Assign gbavecs to the value of the base address of the interrupt
	 *	vector stubs
	 */
	sys_ptr = (ulong *)(ADDR_SAT + OFF_GV);
	fptr    = (ulong(*)())*sys_ptr;
	gbavecs = (*fptr)();

	/*
	 *	Assign ivect to the value of the monitor-controlled variable, i_vect
	 */
	sys_ptr = (ulong *)(ADDR_SAT + OFF_GIV);
	ivect   = (uchar(*)())*sys_ptr;


	bavecs  = gbavecs;

	/*
	 *	1st, save the original vectors in an array
	 */
	for (i=0; i < NUMEVEC; ++i)
	{
		lptr         = (ulong *)(bavecs + JMPOFF);
		evec_org[i]  = *lptr;
		bavecs      += NXTVEC;
	}

	bavecs = gbavecs;

	/*
	 *	Reprogram the vectors so they all route through doevec.
	 */
	for (i=0; i < NUMEVEC; ++i)
	{
		lptr    = (ulong *)(bavecs + JMPOFF);
		*lptr   = (long)doevec;
		bavecs += NXTVEC;
	}
}


doevec(arg)
int		arg;
{
    unsigned short	sr;
    unsigned long	pc;
    unsigned char	vector;
    int				evnum;
    char			*parg;
	short			fcodes;
	long			acc_addr;
	short			ins_reg;

	parg = (char *)&arg;
	parg = parg - 4;

	vector = (*ivect)();

	if (vector == 2 || vector == 3)
	{
		fcodes   = *((short *) parg);
		acc_addr = *((long  *) (parg+2));
		ins_reg  = *((short *) (parg+6));
		sr       = *((short *) (parg+8));
		pc       = *((long  *) (parg+10));
	}
	else
	{
	    sr = *((short *) parg);
		pc = *((long  *) (parg+2));
	}
    
    kprintf("\nXinu Trap!  ");
    kprintf("sr: 0x%x  ", sr);
    kprintf("exception: 0x%x  ", vector);
    kprintf("pc: 0x%lx\n",pc);

	if (vector == 2 || vector == 3)
	{
		kprintf("fcodes: 0x%x ",fcodes);
		kprintf("acc_addr: 0x%lx ",acc_addr);
		kprintf("ins_reg: 0x%x\n",ins_reg);
	}

    kprintf("Trap definition: ");

    evnum = vector;
    if (evnum<32)
	switch(evnum) {
	    case  0: kprintf("RESET0, SHOULDN'T SEE THIS\n"); break;
	    case  1: kprintf("RESET1, SHOULDN'T SEE THIS\n"); break;
	    case  2: kprintf("Bus Error\n"); break;
	    case  3: kprintf("Address Error\n"); break;
	    case  4: kprintf("Illegal instruction\n"); break;
	    case  5: kprintf("Divide by 0\n"); break;
	    case  6: kprintf("CHK,CHK2 software trap\n"); break;
	    case  7: kprintf("cpTRAPcc, TRAPcc, TRAPV software trap\n"); break;
	    case  8: kprintf("Privilege Violation\n"); break;
	    case  9: kprintf("Trace\n"); break;
	    case 10: kprintf("Line 1010 Emulator, illegal instruction\n"); break;
	    case 11: kprintf("Line 1111 Emulator, illegal instruction\n"); break;

	    case 15: kprintf("Uninitialized Interrupt\n"); break;

	    case 24: kprintf("Spurrious Interrupt\n"); break;
	    case 25: kprintf("Level 1 Auto Vector\n"); break;
	    case 26: kprintf("Level 2 Auto Vector\n"); break;
	    case 27: kprintf("Level 3 Auto Vector\n"); break;
	    case 28: kprintf("Level 4 Auto Vector\n"); break;
	    case 29: kprintf("Level 5 Auto Vector (clock)\n"); break;
	    case 30: kprintf("Level 6 Auto Vector (serial line)\n"); break;
	    case 31: kprintf("Level 7 Auto Vector (monitor clock)\n"); break;
	    default: kprintf("Unassigned (reserved)\n"); break;
	  }
    else if (evnum<48)
	kprintf("Trap #%d instruction trap\n",(evnum-32));
    else if (evnum<64)
	kprintf("Unassigned, reserved\n");
    else
	kprintf("User defined vector\n");

    kprintf("\n");
    evecnocont();
}

/* We want to keep people from continuing when an exception occurs */
evecnocont()
{
    while (TRUE) {
	ret_mon();
	kprintf("Can't continue from exception\n");
    }

}

