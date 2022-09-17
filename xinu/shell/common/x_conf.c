/* x_conf.c - x_conf */

#include <conf.h>
#include <kernel.h>
#ifdef NETDAEMON
#include <network.h>
#endif

extern	char	vers[];

/*------------------------------------------------------------------------
 *  x_conf  -  Print info about the system configuration
 *------------------------------------------------------------------------
 */
COMMAND	x_conf(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char	str[128];
	char	name[80];
	char	*FindName();
	int	i;

	fprintf(stdout,"Identification\n");

	fprintf(stdout,"    Xinu version:     %s\n",vers);

#ifdef NETDAEMON
	fprintf(stdout,"    Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		eth[0].etpaddr[0]&0xff,
		eth[0].etpaddr[1]&0xff,
		eth[0].etpaddr[2]&0xff,
		eth[0].etpaddr[3]&0xff,
		eth[0].etpaddr[4]&0xff,
		eth[0].etpaddr[5]&0xff);
	
	for (i=0; i<Net.nif; ++i) {
		if (nif[i].ni_state == NIS_DOWN)
			continue;
		if (i == NI_LOCAL)
			continue;
		fprintf(stdout,"    IP address:       %d.%d.%d.%d\n",
			(unsigned short) nif[i].ni_ip[0]&0xff,
			(unsigned short) nif[i].ni_ip[1]&0xff,
			(unsigned short) nif[i].ni_ip[2]&0xff,
			(unsigned short) nif[i].ni_ip[3]&0xff);
		fprintf(stdout,"    Domain name:      %s\n",
			ip2name(nif[i].ni_ip,name) == SYSERR ? "UNKNOWN" : name);
	}
#endif


	fprintf(stdout,"\nTable sizes\n");
	fprintf(stdout,"    Number of processes:   %d\n",NPROC);
	fprintf(stdout,"    Number of semaphores:  %d\n",NSEM);
	fprintf(stdout,"    Number of devices:     %d\n",NDEVS);

#ifdef NETDAEMON
	fprintf(stdout,"\nNetwork information\n");

#ifdef TSERVER
	fprintf(stdout,"    Time server:        %s	(%s)\n",
		TSERVER, FindName(TSERVER));
#endif	

#ifdef RSERVER
	fprintf(stdout,"    Remote file server: %s	(%s)\n",
		RSERVER, FindName(RSERVER));
#endif	

#ifdef NSERVER
	fprintf(stdout,"    Domain name server: %s	(%s)\n",
		NSERVER, FindName(NSERVER));
#endif	
#endif

	return(OK);
}

#ifdef NETDAEMON
char *FindName(strIP)
     char *strIP;
{
    static char name[80];
    IPaddr addr;

    dot2ip(addr, strIP);
    
    if (ip2name(addr, name) == SYSERR)
	return("UNKNOWN");
    else
	return(name);
}
#endif
