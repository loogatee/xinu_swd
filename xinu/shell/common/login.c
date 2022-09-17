/* login.c - login */

#include <conf.h>
#include <kernel.h>
#include <shell.h>
/* #include <network.h> */

/*------------------------------------------------------------------------
 *  login  -  log user onto system
 *------------------------------------------------------------------------
 */
login(dev,net_flag)
int	dev;
{
	register	struct shvars	*shptr;
				int				i, len;
				char			rbuf[2];
				char			ch;

	rbuf[1] = 0;
	shptr   = &Shl[net_flag];

	shptr->shused = FALSE;
	shptr->shuser[0] = NULLCH;
	getname(shptr->shmach);

l2:

	while (TRUE) {
		fprintf(dev,"\n\n%s - The magic of Xinu\n\nlogin: ", shptr->shmach);

		if (net_flag == 1)
		{
l1:
			for (len = 0; ; )
			{
				if (len < SHBUFLEN)
				{
					if (read(dev,rbuf,1) < 0)
						goto login_end;
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
						goto login_end;
					--len;
				}
				else if (ch >= 32 && ch <= 126)
				{
					if (write(dev,rbuf,1) < 0)
						goto login_end;
					shptr->shbuf[len++] = ch;
				}
				else if (ch == 4)
				{
					if (len == 0)
					{
						shptr->shused = FALSE;
						goto l2;
					}
				}
			}
			if (write(dev,"\n\r",2) < 0)
				goto login_end;
			if (len == 0 || len == 1)
			{
				if (write(dev,"login: ",7) < 0)
					goto login_end;
				goto l1;
			}
		}
		else
		{
			while ( (len=read(dev,shptr->shbuf,SHBUFLEN))==0 || len==1)
				fprintf(dev, "login: ");
			if (len == EOF) {
				read(dev, shptr->shbuf, 0);
				shptr->shused = FALSE;
				continue;
			}
		}

		shptr->shbuf[len-1] = NULLCH;
		strncpy(shptr->shuser, shptr->shbuf, SHNAMLEN-1);
		shptr->shused = TRUE;
		getutim(&shptr->shlogon);
		mark(shptr->shmark);
		fprintf(dev,"\n%s\n\n", "      Welcome to Xinu (type ? for help)" );
		getutim(&shptr->shlast);
		return(OK);

login_end:
		return(-1);
	}
}
