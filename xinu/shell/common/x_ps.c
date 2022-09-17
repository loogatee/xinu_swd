/* x_ps.c - x_ps */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

LOCAL	char	hd1[] =
	"pid   name   state prio  stack range  stack length   sem message\n";
LOCAL	char	hd2[] =
	"--- -------- ----- ---- ------------- ------------   --- -------\n";
LOCAL	char	*pstnams[] = {"curr ","free ","ready","recv ",
			    "sleep","susp ","wait ","rtim "};
LOCAL	int	psavsp;

/*------------------------------------------------------------------------
 *  x_ps  -  (command ps) format and print process table information
 *------------------------------------------------------------------------
 */
COMMAND	x_ps(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i;
	char	str[80];
	struct	pentry	*pptr;
	unsigned long currstk;

	asm("movl sp,_psavsp");	/* capture current stack pointer */
	proctab[currpid].pregs[SSP] = psavsp;
	fprintf(stdout, "%s", hd1);
	fprintf(stdout, "%s", hd2);
	for (i=0 ; i<NPROC ; i++) {
		if ((pptr = &proctab[i])->pstate == PRFREE)
			continue;
		fprintf(stdout, "%3d %8s %s ", i, pptr->pname,
			pstnams[pptr->pstate-1]);

		fprintf(stdout, "%4d %06x-%06x ", pptr->pprio,  pptr->plimit,
			(unsigned)pptr->pbase + 1);

		currstk = pptr->pregs[SSP];
		if (currstk < pptr->plimit || currstk > pptr->pbase)
			fprintf(stdout, " OVERFLOWED     ");
		else
			fprintf(stdout, "%5d /%5d    ", pptr->pbase - currstk,
				pptr->pbase - pptr->plimit + sizeof(int));

		if (pptr->pstate == PRWAIT)
			fprintf(stdout, "%2d", pptr->psem);
		else
			fprintf(stdout, "- ");

		if (pptr->phasmsg)
			fprintf(stdout, "%06x\n", pptr->pmsg);
		else
			fprintf(stdout, "   -\n");
	}
}




