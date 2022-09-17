/* shell.c - shell */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <shell.h>
#include <cmd.h>
#include <tty.h>
/* #include <network.h> */

typedef unsigned long	ulong;

extern	char	*index();

struct	shvars	Shl[2];			/* globals used by Xinu shell	*/
LOCAL	char	errhd[] = "Syntax error\n";/* global error messages	*/
LOCAL	char	fmt[]   = "Cannot open %s\n";
LOCAL	char	fmt2[]  = "[%d]\n";

/*------------------------------------------------------------------------
 *  shell  -  Xinu shell with file redirection and background processing
 *------------------------------------------------------------------------
 */
shell(dev,net_flag)
int	dev;
int	net_flag;
{
	register	struct shvars	*shptr;
				int				ntokens;
				int				i, j, len;
				int				com;
				char			*outnam, *innam;
				int				stdin, stdout, stderr;
				Bool			backgnd;
				char			ch, mach[SHMLEN], *pch;
				int				child;
				char			rbuf[2];

	rbuf[1] = 0;
	shptr   = &Shl[net_flag];

	shptr->shncmds = (ulong)szof_cmds() / (ulong)sizeof(struct cmdent);

	getname(mach);
	if (pch = index(mach, '.'))
		*pch = NULLCH;
	
	while (TRUE) {
		fprintf(dev, "%s %% ", mach);
		getutim(&shptr->shlast);


		if (net_flag == 1)
		{
			for (len = 0; ; )
			{
				if (len < SHBUFLEN)
				{
					if (read(dev,rbuf,1) < 0)
						goto shell_end;
					if ((ch = rbuf[0]) == 13)
					{
						shptr->shbuf[len++] = ch;
						break;
					}
				}
				else
					break;
	
				if (len && (ch == 8 || ch == 127))
				{
					if (write(dev,"\010\040\010",3) < 0)	/* BS SP BS */
						goto shell_end;
					--len;
				}
				else if (ch >= 32 && ch <= 126)
				{
					if (write(dev,rbuf,1) < 0)
						goto shell_end;
					shptr->shbuf[len++] = ch;
				}
				else if (ch == 4)
				{
					if (len == 0)
						goto shell_end;
				}
			}
			if (write(dev,"\n\r",2) < 0)
				goto shell_end;
		}
		else
		{
			if ( (len = read(dev, shptr->shbuf, SHBUFLEN)) == 0)
				len = read(dev, shptr->shbuf, SHBUFLEN);
			if (len == EOF)
				break;
		}



		shptr->shbuf[len-1] = NULLCH;
		if ( (ntokens=lexan(shptr)) == SYSERR) {
			fprintf(dev, errhd);
			continue;
		} else if (ntokens == 0)
			continue;
		outnam = innam = NULL;
		backgnd = FALSE;

		/* handle '&' */

		if (shptr->shtktyp[ntokens-1] == '&') {
			ntokens-- ;
			backgnd = TRUE;
		}

		/* scan tokens, accumulating length;  handling redirect	*/

		for (len=0,i=0 ; i<ntokens ; ) {
			if ((ch = shptr->shtktyp[i]) == '&') {
				ntokens = -1;
				break;
			}
#ifdef REDIRECT
			else if (ch == '>') {
				if (outnam != NULL || i >= --ntokens) {
					ntokens = -1;
					break;
				}
				outnam = shptr->shtok[i+1];
				for (ntokens--,j=i ; j<ntokens ; j++) {
					shptr->shtktyp[j] = shptr->shtktyp[j+2];
					shptr->shtok  [j] = shptr->shtok  [j+2];
				}
				continue;
			} else if (ch == '<') {
				if (innam != NULL || i >= --ntokens) {
					ntokens = -1;
					break;
				}
				innam = shptr->shtok[i+1];
				for (ntokens--,j=i ; j < ntokens ; j++) {
					shptr->shtktyp[j] = shptr->shtktyp[j+2];
					shptr->shtok  [j] = shptr->shtok  [j+2];
				}
				continue;
			}
#endif
			else {
				 len += strlen(shptr->shtok[i++]);
			}
		}
		if (ntokens <= 0) {
			fprintf(dev, errhd);
			continue;
		}
		stdin = stdout = stderr = dev;

		/* Look up command in table */

		for (com=0 ; com<shptr->shncmds ; com++) {
			if (strcmp(cmds[com].cmdnam,shptr->shtok[0]) == 0)
				break;
		}
		if (com >= shptr->shncmds) {
			fprintf(dev, "%s: not found\n", shptr->shtok[0]);
			continue;
		}

		/* handle built-in commands with procedure call */

		if (cmds[com].cbuiltin) {
			if (innam != NULL || outnam != NULL || backgnd)
				fprintf(dev, errhd);
			else if ( (*cmds[com].cproc)(stdin, stdout,
				stderr, ntokens, shptr->shtok) == SHEXIT)
				break;
			continue;
		}

		/* Open files and redirect I/O if specified */
#ifdef REDIRECT
		if (innam != NULL && (stdin=open(NAMESPACE,innam,"ro"))
			== SYSERR) {
			fprintf(dev, fmt, innam);
			continue;
		}
		if (outnam != NULL && (stdout=open(NAMESPACE,outnam,"w"))
			== SYSERR) {
			fprintf(dev, fmt, outnam);
			continue;
		}
#endif

		/* compute space needed for string args. (in bytes) */

		/* add a null for the end of each string, plus a    */
		/*    pointer to the string (see xinu2, p300)       */
		len += ntokens * (sizeof(char *) + sizeof(char));

		/* plus a (char *) null for the end of the table    */
		len += sizeof(char *);

		/* plus a pointer to the head of the table          */
		len += sizeof(char *);

		
		len = (len+3) & ~(unsigned) 0x3;

		control(dev, TCINT, getpid());

		/* create process to execute conventional command */

		if ( (child = create(cmds[com].cproc, SHCMDSTK, SHCMDPRI,
				shptr->shtok[0],(len/sizeof(long)) + 4,
				stdin, stdout, stderr, ntokens))
				== SYSERR) {
			fprintf(dev, "Cannot create\n");
			close(stdout);
			close(stdin);
			continue;
		}
		addarg(child, ntokens, len, shptr);
		if (net_flag == 0)
			setdev(child, stdin, stdout);
		proctab[child].pshdev = net_flag;
		if (backgnd) {
			fprintf(dev, fmt2, child);
			resume(child);
		} else {
			setnok(getpid(), child);
			recvclr();
			resume(child);
			if (receive() == INTRMSG) {
				setnok(BADPID, child);
/*				fprintf(dev, fmt2, child); */
				kill(child);
			}
		}
	}
	shell_end:
	return(OK);
}
