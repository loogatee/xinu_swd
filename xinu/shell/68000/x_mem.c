/* x_mem.c - x_mem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>

extern char *etext, *edata, *rom, *ram;


/*------------------------------------------------------------------------
 *  x_mem  -  (command mem) print memory use and free list information
 *------------------------------------------------------------------------
 */
COMMAND	x_mem(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i;
	struct	mblock	*mptr;
	unsigned free;
	unsigned avail;
	unsigned stkmem;

	/* calculate current size of free memory and stack memory */

	for( free=0,mptr=memlist.mnext ; mptr!=(struct mblock *)NULL ;
	     mptr=mptr->mnext)
		free += mptr->mlen;
	for (stkmem=0,i=0 ; i<NPROC ; i++) {
		if (proctab[i].pstate != PRFREE)
			stkmem += (unsigned)proctab[i].pstklen;
	}
	fprintf(stdout,
		"Memory: %u bytes real memory, %u text, %u data, %u bss\n",
		4 + (unsigned)maxaddr-(unsigned)&rom,(unsigned)&etext-(unsigned) &rom,
		(unsigned) &edata - (unsigned) &ram,
		(unsigned) &end - (unsigned) &edata);

	avail = (unsigned)maxaddr+4 - (unsigned) &end;
	fprintf(stdout," initially: %5u avail\n", avail);

	fprintf(stdout," presently: %5u avail, %5u stack, %5u heap\n",
		free, stkmem, avail - stkmem - free);

	fprintf(stdout," free list:\n");

	for( mptr=memlist.mnext ; mptr!=(struct mblock *)NULL ;
	     mptr=mptr->mnext) {
		fprintf(stdout,"   block at 0x%x, length %5u (0x%x)\n",
			mptr, mptr->mlen, mptr->mlen);
	}
	return(OK);
}

