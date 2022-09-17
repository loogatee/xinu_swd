/* x_net.c - x_net */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <asn1.h>

static	int	noarg(), iarg(), parp();
static	char	*tcpfnames();

/*------------------------------------------------------------------------
 *  x_net  -  (command netstat) print network status information
 *------------------------------------------------------------------------
 */
COMMAND x_net(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{

	if (nargs == 1)
		return noarg(stdout, stderr);
	if (nargs > 2 || *args[1] != '-') {
		fprintf(stdout, "usage: netstat -[i]\n");
		return 1;
	}
	switch (args[1][1]) {
	case	'i':	return iarg(stdout, stderr);
	default:
		fprintf(stdout, "usage: netstat -[i]\n");
		return 1;
	}
}

char	*tcpsnames[] = { "FREE", "CLOSED", "LISTEN", "SYNSENT", "SYNRCVD",
			"ESTABLISHED", "FINWAIT1", "FINWAIT2", "CLOSEWAIT",
			"LASTACK", "CLOSING", "TIMEWAIT" };

#define	NH1 \
"Proto  RQ   SQ  L. Port    Remote IP    R. Port    State    flags   dev\n"
#define NH2 \
"----- ---- ---- ------- --------------- ------- ----------- -----  -----\n"

static	int
noarg(stdout, stderr)
int	stdout, stderr;
{
	char	str[120];
	char	ipa[16];	/* "XXX.XXX.XXX.XXX\0" */
	int	i;

	fprintf(stdout, "%s", NH1);
	fprintf(stdout, "%s", NH2);
	for (i=0; i<Ntcp; ++i) {
		struct tcb	*ptcb = &tcbtab[i];

		if (ptcb->tcb_state == TCPS_FREE)
			continue;
		ip2dot(ipa, ptcb->tcb_rip);
		fprintf(stdout, " %3s  %4d %4d  %5d  %15s  %5d %11s %6s %5d\n",
			"tcp", ptcb->tcb_rbcount, ptcb->tcb_sbcount,
			ptcb->tcb_lport, ipa, ptcb->tcb_rport,
			tcpsnames[ptcb->tcb_state], tcpfnames(ptcb->tcb_flags),
			ptcb->tcb_dvnum);
	}
	return(OK);
}

static	int
iarg(stdout, stderr)
int	stdout, stderr;
{
	struct	upq	*pup;
	char	str[80];
	int	i,j;

	fprintf(stdout,
		"netpool=%d, lrgpool=%d\n", Net.netpool, Net.lrgpool);

	fprintf(stdout, "Number of interfaces configured: %d\n", Net.nif);

	for (i=0; i<Net.nif; ++i) {
		struct	netif	*pni = &nif[i];

		if (pni->ni_state == NIS_DOWN &&
		    pni->ni_admstate == NIS_DOWN)
				continue;
		fprintf(stdout,
			"\nif%d\nIn: %d packets (%d broadcast), %d bytes\n", i,
			pni->ni_iucast+pni->ni_inucast, pni->ni_inucast,
			pni->ni_ioctets);

		fprintf(stdout,
			"Out: %d packets (%d broadcast), %d bytes\n",
			pni->ni_oucast+pni->ni_onucast, pni->ni_onucast,
			pni->ni_ooctets);

		fprintf(stdout,
		"Errors: %d input, %d output\tDiscards %d input, %d output\n",
			pni->ni_ierrors, pni->ni_oerrors,
			pni->ni_idiscard, pni->ni_odiscard);

		fprintf(stdout, "%d input packets with unknown protocols\n",
			pni->ni_iunkproto);
	}
	fprintf(stdout,"\n");

	for (i=0; i<UPPS; i++) {
		pup = &upqs[i];
		if (!pup->up_valid)
		  continue;
		fprintf(stdout,
			"%2d. port=%4d, pid=%3d, xport=%2d, size=%2d\n",
			i, pup->up_port, pup->up_pid, pup->up_xport,
			pcount(pup->up_xport) );
	}

	fprintf(stdout, "\nfragment queues:\n");

	wait(ipfmutex);
	for (i=0; i<IP_FQSIZE; ++i) {
		char *state;
		struct ep *p;

		if (ipfqt[i].ipf_state == IPFF_FREE)
			continue;
		state = (ipfqt[i].ipf_state == IPFF_VALID) ? "VALID" : "ORPHAN";
		fprintf(stdout, "%d.\tstate %s src %X id %x ttl %d\toffsets: ",
			i, state,
			*((int *)ipfqt[i].ipf_src), ipfqt[i].ipf_id,
			ipfqt[i].ipf_ttl);

		while ((p = (struct ep *)seeq(ipfqt[i].ipf_q)) != NULL) {
			struct ip *pip;

			pip = (struct ip *)p->ep_data;
			fprintf(stdout, " %d", pip->ip_fragoff & IP_FRAGOFF);
		}
		fprintf(stdout,"\n");
	}
	signal(ipfmutex);
	return(OK);
}

#define	addflag(f, name) \
	if (flag & f) { \
		*p++ = name; \
		*p = '\0'; \
	}

static
char *
tcpfnames(flag)
{
	static char string[5];
	char	*p;

	p = &string[0];
	*p = '\0';

	addflag(TCBF_RDONE, 'R');
	addflag(TCBF_SDONE, 'S');
	addflag(TCBF_DELACK, 'D');
	addflag(TCBF_BUFFER, 'B');
	return string;
}


struct {
	char		*name;
	unsigned	*value;
} tcpstuff[] =  {
	{ "MaxConn",		&TcpMaxConn },
	{ "ActiveOpens",	&TcpActiveOpens },
	{ "PassiveOpens",	&TcpPassiveOpens },
	{ "AttemptFails",	&TcpAttemptFails },
	{ "EstabResets",	&TcpEstabResets },
	{ "CurrEstab",		&TcpCurrEstab },
	{ "InSegs",		&TcpInSegs },
	{ "OutSegs",		&TcpOutSegs },
	{ "RetransSegs",	&TcpRetransSegs },
	{ 0, 0}
};
