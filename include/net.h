/* net.h */

/* High-level network definitions and constants */

#define	NETBUFS		512		/* number of network buffers	*/
#define	MAXNETBUF	EP_MAXLEN	/* max network buffer length	*/
#define	LRGBUFS		50		/* number of large net buffers	*/
#define	MAXLRGBUF	BPMAXB		/* size of large net bufs	*/
#define	NETFQ		3		/* size of input-to-output queue*/

/* Network input and output processes: procedure name and parameters */

extern	PROCESS		netstart(), slowtimer();
#define	NETSTK		1000		/* stack size for network setup	*/
#define	STSTK		1000		/* stack size for network timer	*/
#define	NETPRI		100		/* network startup priority	*/
#define	NETIPRI		100		/* min network input priority	*/
#define	STPRI		100		/* network timer priority	*/
#define	NETNAM		"netstart"	/* name of network setup process*/
#define	STNAM		"slowtimer"	/* name of network timer   "	*/
#define	NETARGC		1		/* count of args to net startup	*/
#define	STARGC		0		/* count of args to net timer	*/

struct	netinfo	{			/* info and parms. for network	*/
	int	netpool;		/* network packet buffer pool	*/
	int	lrgpool;		/* large packet buffer pool	*/
	int	nif;			/* # of configured interfaces	*/
	int	sema;			/* semaphore for global net	*/
};

extern	struct	netinfo	Net;		/* All network parameters	*/
