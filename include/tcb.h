/* tcb.h - RUDK, SUDK, RUHK, SUHK */

/* TCP endpoint types */

#define	TCPT_SERVER		1
#define	TCPT_CONNECTION		2
#define	TCPT_MASTER		3

/* TCP process info */
extern	PROCESS		tcpinp();
#define	TCPISTK		4096		/* stack size for TCP input	*/
#define	TCPIPRI		100		/* TCP runs at high priority	*/
#define	TCPINAM		"tcpinp"	/* name of TCP input process	*/
#define	TCPIARGC	0		/* count of args to tcpin 	*/

extern	PROCESS		tcpout();
#define	TCPOSTK		4096		/* stack size for TCP output	*/
#define	TCPOPRI		100		/* TCP runs at high priority	*/
#define	TCPONAM		"tcpout"	/* name of TCP output process	*/
#define	TCPOARGC	0		/* count of args to tcpout 	*/

#define	TCPQLEN		20	/* TCP process port queue length	*/

#define	TCPMAXURG  MAXNETBUF	/* maximum urgent data buffer size	*/
#define	TCPUQLEN	5	/* TCP urgent queue lengths		*/

/* TCP exceptional conditions */

#define	TCPE_RESET		-1
#define	TCPE_REFUSED		-2
#define	TCPE_TOOBIG		-3
#define	TCPE_TIMEDOUT		-4
#define	TCPE_URGENTMODE		-5
#define	TCPE_NORMALMODE		-6

/* string equivalents of TCPE_*, in "tcpswitch.c" */
extern	char	*tcperror[];

#define	READERS		1
#define	WRITERS		2

/* tcb_flags */

#define	TCBF_NEEDOUT	0x01	/* we need output			*/
#define	TCBF_FIRSTSEND	0x02	/* no data to ACK			*/
#define	TCBF_GOTFIN	0x04	/* no more to receive			*/
#define	TCBF_RDONE	0x08	/* no more receive data to process	*/
#define	TCBF_SDONE	0x10	/* no more send data allowed		*/
#define	TCBF_DELACK	0x20	/* do delayed ACK's			*/
#define	TCBF_BUFFER	0x40	/* do TCP buffering (default no)	*/
#define	TCBF_PUSH	0x80	/* got a push; deliver what we have	*/
#define	TCBF_SNDFIN	0x100	/* user process has closed; send a FIN	*/

/* aliases, for user programs */

#define	TCP_BUFFER	TCBF_BUFFER
#define	TCP_DELACK	TCBF_DELACK

/* receive segment reassembly data */

#define	NTCPFRAG	10

/* URG queue element data */

#define	UQTSIZE	(2*Ntcp)	/* (total) max # pending urgent segs	*/

#define	UQS_FREE	0
#define	UQS_ALLOC	1

struct	uqe {
	int	uq_state;	/* UQS_* above				*/
	tcpseq	uq_seq;		/* start sequence of this buffer	*/
	int	uq_len;		/* length of this buffer		*/
	char	*uq_data;	/* data (0 if on urgent hole queue)	*/
};

/* compute urgent data send and receive queue keys			*/
#define	SUDK(ptcb, seq)	(ptcb->tcb_sudseq - (seq))
#define	SUHK(ptcb, seq)	(ptcb->tcb_suhseq - (seq))
#define	RUDK(ptcb, seq)	(ptcb->tcb_rudseq - (seq))
#define	RUHK(ptcb, seq)	(ptcb->tcb_ruhseq - (seq))

extern	Bool	uqidone;
extern	struct	uqe uqtab[];
extern	int	uqmutex;

struct tcb {
	short	tcb_state;	/* TCP state				*/
	short	tcb_ostate;	/* output state				*/
	short	tcb_type;	/* TCP type (SERVER, CLIENT)		*/
	int	tcb_mutex;	/* tcb mutual exclusion			*/
	short	tcb_code;	/* TCP code for next packet		*/
	short	tcb_flags;	/* various TCB state flags		*/
	short	tcb_error;	/* return error for user side		*/

	IPaddr	tcb_rip;	/* remote IP address			*/
	short	tcb_rport;	/* remote TCP port			*/
	IPaddr	tcb_lip;	/* local IP address			*/
	short	tcb_lport;	/* local TCP port			*/
	struct	netif	*tcb_pni; /* pointer to our interface		*/

	tcpseq	tcb_suna;	/* send unacked				*/
	tcpseq	tcb_snext;	/* send next				*/
	tcpseq	tcb_slast;	/* sequence of FIN, if TCBF_SNDFIN	*/
	long	tcb_swindow;	/* send window size (octets)		*/
	tcpseq	tcb_lwseq;	/* sequence of last window update	*/
	tcpseq	tcb_lwack;	/* ack seq of last window update	*/
	int	tcb_cwnd;	/* congestion window size (octets)	*/
	int	tcb_ssthresh;	/* slow start threshold (octets)	*/
	int	tcb_smss;	/* send max segment size (octets)	*/
	tcpseq	tcb_iss;	/* initial send sequence		*/

	int	tcb_srt;	/* smoothed Round Trip Time		*/
	int	tcb_rtde;	/* Round Trip deviation estimator	*/
	int	tcb_persist;	/* persist timeout value		*/
	int	tcb_keep;	/* keepalive timeout value		*/
	int	tcb_rexmt;	/* retransmit timeout value		*/
	int	tcb_rexmtcount;	/* number of rexmts sent		*/

	tcpseq	tcb_rnext;	/* receive next				*/

	tcpseq	tcb_rudseq;	/* base sequence for rudq entries	*/
	int	tcb_rudq;	/* receive urgent data queue		*/
	tcpseq	tcb_ruhseq;	/* base sequence for ruhq entries	*/
	int	tcb_ruhq;	/* receive urgent hole queue		*/
	int	tcb_sudseq;	/* base sequence for sudq entries	*/
	int	tcb_sudq;	/* send urgent data queue		*/
	int	tcb_suhseq;	/* base sequence for suhq entries	*/
	int	tcb_suhq;	/* send urgent hole queue		*/

	int	tcb_lqsize;	/* listen queue size (SERVERs)		*/
	int	tcb_listenq;	/* listen queue port (SERVERs)		*/
	struct tcb *tcb_pptcb;	/* pointer to parent TCB (for ACCEPT)	*/
	int	tcb_ocsem;	/* open/close semaphore 		*/
	int	tcb_dvnum;	/* TCP slave pseudo device number	*/

	int	tcb_ssema;	/* send semaphore			*/
	char	*tcb_sndbuf;	/* send buffer				*/
	int	tcb_sbstart;	/* start of valid data			*/
	int	tcb_sbcount;	/* data character count			*/
	int	tcb_sbsize;	/* send buffer size (bytes)		*/

	int	tcb_rsema;	/* receive semaphore			*/
	char	*tcb_rcvbuf;	/* receive buffer (circular)		*/
	int	tcb_rbstart;	/* start of valid data			*/
	int	tcb_rbcount;	/* data character count			*/
	int	tcb_rbsize;	/* receive buffer size (bytes)		*/
	int	tcb_rmss;	/* receive max segment size		*/
	tcpseq	tcb_cwin;	/* seq of currently advertised window	*/
	int	tcb_rsegq;	/* segment fragment queue		*/
	tcpseq	tcb_finseq;	/* FIN sequence number, or 0		*/
	tcpseq	tcb_pushseq;	/* PUSH sequence number, or 0		*/
};
/* TCP fragment structure */

struct tcpfrag {
	tcpseq	tf_seq;
	int	tf_len;
};
/* TCP control() functions */

#define	TCPC_LISTENQ	0x01	/* set the listen queue length		*/
#define	TCPC_ACCEPT	0x02	/* wait for connect after passive open	*/
#define	TCPC_STATUS	0x03	/* return status info (all, for master)	*/
#define	TCPC_SOPT	0x04	/* set user-selectable options		*/
#define	TCPC_COPT	0x05	/* clear user-selectable options	*/
#define	TCPC_SENDURG	0x06	/* write urgent data			*/

/* global state information */

extern int tcps_oport;	/* Xinu port to start TCP output		*/
extern int tcps_iport;	/* Xinu port to send TCP input packets		*/
extern int tcps_lqsize;	/* default SERVER queue size			*/
extern int tcps_tmutex;	/* tcb table mutex				*/

extern int	(*tcpswitch[])(), (*tcposwitch[])();

extern struct	tcb	tcbtab[];
