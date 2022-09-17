/* ether.h */

/* Ethernet definitions and constants */


#define	EP_MINLEN	60				/* minimum packet length */
#define	EP_DLEN		1500			/* length of data field ep */
#define	EP_HLEN		20				/* size of (extended) Ether header */
#define	EP_CRC		4				/* ether CRC (trailer) */
#define	EP_MAXLEN	EP_HLEN+EP_DLEN	/* maximum length of a packet */
#define	EP_ALEN		6				/* number of octets in physical address */
#define	EP_RETRY	3				/* number of times to retry xmit errors	*/
#define EP_RTO		300				/* time out in seconds for reads	*/

#define	EP_BRC		"\377\377\377\377\377\377"	/* Ethernet broadcast addr */


#define	EPT_LOOP	0x0060		/* type: Loopback */
#define	EPT_ECHO	0x0200		/* type: Echo */
#define	EPT_PUP		0x0400		/* type: Xerox PUP */
#define	EPT_IP		0x0800		/* type: Internet Protocol */
#define	EPT_ARP		0x0806		/* type: ARP */
#define	EPT_RARP	0x8035		/* type: Reverse ARP */

typedef	char		Eaddr[EP_ALEN];	/* length of physical address (48 bits)	*/

/*
 *	ethernet header
 */
struct eh
{
	Eaddr			eh_dst;		/* destination host address */
	Eaddr			eh_src;		/* source host address */
	unsigned short	eh_type;	/* Ethernet packet type (see below) */
};

/*
 *	Complete structure of Ethernet packet
 */
struct ep
{
	IPaddr		ep_nexthop;			/* niput() uses this */
	short		ep_len;				/* length of the packet */
	struct eh	ep_eh;				/* the ethernet header */
	char		ep_data[EP_DLEN];	/* data in the packet */
};


/*
 *	Ethernet control block descriptions
 */
struct etblk
{
	char			*etle;			/* address of device csr */
	struct devsw	*etdev;			/* Addr device switch table entry */
	Eaddr			etpaddr;		/* Ethernet physical device addr */
	Eaddr			etbcast;		/* Ethernet broadcast address */
	char			etfilter[8];	/* Ethernet logical addr filter */
	int				etoutq;			/* mutex for writing to ethernet */
	int				etxpending;		/* # of packets pending output */
	int				etrpending;		/* 1 => receive is pending */
	int				etwtry;			/* # times to retry xmit errors */
	char			*etwbuf;		/* ptr to current transmit buf */
	int				etintf;			/* our interface number */
	short			etnextbuf;		/* for checking bufs round robin */
	char			*etdescr;		/* text description of the device */
	char			*etbsp;			/* ptr to board specific part of etblk */
};

/*
 *	these allow us to pretend it's all one big happy structure
 */
#define	ep_dst		ep_eh.eh_dst
#define	ep_src		ep_eh.eh_src
#define	ep_type		ep_eh.eh_type


#define	ETOUTQSZ	10


/*
 *	ethernet function codes
 */
#define	EPC_PROMON		1		/* turn on promiscuous mode */
#define	EPC_PROMOFF		2		/* turn off promiscuous mode */
#define	EPC_MULTICAST	3		/* set multicast address */

extern	struct	etblk	eth[];
