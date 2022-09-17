/* netstart.c - netstart */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <tcptimer.h>
#include <q.h>

extern	int	bsdbrc;

extern int	aux_shell();

int udpecho();

/*------------------------------------------------------------------------
 *  netstart - initialize net
 *------------------------------------------------------------------------
 */
PROCESS netstart(userpid)
int	userpid;			/* user process to resume	*/
{
	char str[128];
	char *str1 = "sleeping 5 secs to get routes...";
	char *str2 = "\ndone.\n";
	struct	upq	*pup;
	long	now;
	int	i;

	if (clkruns == FALSE)
		panic("net: no clock");

	/* initialize ports */

	for (i=0 ; i<UPPS ; i++)
		upqs[i].up_valid = FALSE;
	udpmutex = screate(1);

	arpinit();
	ipfinit();	/* init the IP frag queue */

	/*
	 *	The statement to create-and-resume "aux-shell" can be used
	 *	if you are having problems with Xinu, and the shell will
	 *	not come up.  To use, just un-comment.    -JR-
	 */
/*	resume( create(aux_shell,0x4000,20,"aux_shell",1,0)); */

	/* these need a command to set/clear them. FIX ME!!! */
	gateway = 0;
	IpForwarding = 2;	/* == 2 if not a gateway */
	IfNumber = Net.nif - 1;

	bsdbrc = 1;

	inithost();

	rtadd(RT_LOOPBACK, ip_maskall, RT_LOOPBACK, 0, NI_LOCAL, RT_INF);

	for (i=0; i<Net.nif; ++i) {
		nif[i].ni_ipinq = newq(NI_INQSZ, QF_NOWAIT);
		nif[i].ni_hwtype = AR_HARDWARE;		/* for ARP */
		/* no OTHER's for now */
		if (i < 2)
			nif[i].ni_state = NIS_UP;
	}

	/*
	 * wait()'s synchronize to insure initialization is done
	 * before proceeding.
	 */

	resume(create(slowtimer, STSTK, STPRI, STNAM, STARGC));
	wait(Net.sema);
	resume(create(ipproc, IPSTK, IPPRI, IPNAM, IPARGC));
	wait(Net.sema);
	resume(create(tcptimer, TMSTK, TMPRI, TMNAM, TMARGC));
	wait(Net.sema);
	resume(create(tcpinp, TCPISTK, TCPIPRI, TCPINAM, TCPIARGC));
	wait(Net.sema);
	resume(create(tcpout, TCPOSTK, TCPOPRI, TCPONAM, TCPOARGC));
	wait(Net.sema);


	if (!gateway) {
		write(CONSOLE, str1, strlen(str1));
		sleep(5);
		write(CONSOLE, str2, strlen(str2));
	}

	/* get addrs & names */
	for (i=0; i<Net.nif; ++i) {
		IPaddr junk;
		char junk2[64];

		if (i == NI_LOCAL)
			continue;
		if (nif[i].ni_state != NIS_UP)
			continue;
		nif[i].ni_ivalid = 0;
		getiaddr(i, junk);
		sprintf(str,
		"if%d IP %u.%u.%u.%u ha %02x:%02x:%02x:%02x:%02x:%02x ",
			i,
			nif[i].ni_ip[0]&0xff, nif[i].ni_ip[1]&0xff,
			nif[i].ni_ip[2]&0xff, nif[i].ni_ip[3]&0xff,
			nif[i].ni_hwa.ha_addr[0]&0xff,
			nif[i].ni_hwa.ha_addr[1]&0xff,
			nif[i].ni_hwa.ha_addr[2]&0xff,
			nif[i].ni_hwa.ha_addr[3]&0xff,
			nif[i].ni_hwa.ha_addr[4]&0xff,
			nif[i].ni_hwa.ha_addr[5]&0xff);
		write(CONSOLE, str, strlen(str));
		sprintf(str, "br %02x:%02x:%02x:%02x:%02x:%02x\n",
			nif[i].ni_hwb.ha_addr[0]&0xff,
			nif[i].ni_hwb.ha_addr[1]&0xff,
			nif[i].ni_hwb.ha_addr[2]&0xff,
			nif[i].ni_hwb.ha_addr[3]&0xff,
			nif[i].ni_hwb.ha_addr[4]&0xff,
			nif[i].ni_hwb.ha_addr[5]&0xff);
		write(CONSOLE, str, strlen(str));

		icmp(ICT_MASKRQ, 0, nif[i].ni_brc);
		recvtim(30);	/* wait for an answer */
		getiname(i, junk2);
	}
	resume(create(rip, RIPSTK, RIPPRI, RIPNAM, RIPARGC));
	resume(create(snmpd, SNMPSTK, SNMPPRI, SNMPDNAM, 0));

	getutim(&now);
/*	rwho(); */
/*	resume(create(FINGERD, FNGDSTK, FNGDPRI, FNGDNAM, FNGDARGC)); */
/*	resume(create(ECHOD, ECHOSTK, ECHOPRI, ECHODNAM, 0)); */
/*	resume(create(udpecho, 1000, 50, "udpecho", 0)); */
	resume(userpid);
}

struct	netif		nif[NIF];
struct	netinfo		Net;
