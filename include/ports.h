/* ports.h - isbadport */

#define	MAXMSGS		100		/* maximum messages on all ports*/
#define	PTFREE		'\01'		/* port is Free			*/
#define	PTLIMBO		'\02'		/* port is being deleted/reset	*/
#define	PTALLOC		'\03'		/* port is allocated		*/
#define	PTEMPTY		-1		/* initial semaphore entries	*/

#define	PTNODISP	0		/* ptclear() null disposal func */

struct	ptnode	{			/* node on list of message ptrs	*/
	int	ptmsg;			/* a one-word message		*/
	struct	ptnode	*ptnext;	/* address of next node on list	*/
};

struct	pt	{			/* entry in the port table	*/
	char	ptstate;		/* port state (FREE/LIMBO/ALLOC)*/
	int	ptssem;			/* sender semaphore		*/
	int	ptrsem;			/* receiver semaphore		*/
	int	ptmaxcnt;		/* max messages to be queued	*/
	int	ptseq;			/* sequence changed at creation	*/
	struct	ptnode	*pthead;	/* list of message pointers	*/
	struct	ptnode	*pttail;	/* tail of message list		*/
};

extern	struct	ptnode	*ptfree;	/* list of free nodes		*/
extern	struct	pt	ports[];	/* port table			*/
extern	int	ptnextp;		/* next port to examine when	*/
					/*   looking for a free one	*/
extern	MARKER	ptmark;

#define	isbadport(portid)	( (portid)<0 || (portid)>=NPORTS )
