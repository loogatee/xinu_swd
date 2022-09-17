/* conf.h (GENERATED FILE; DO NOT EDIT) */

#define	NULLPTR	(char *)0

/* Device table declarations */
struct	devsw	{			/* device table entry */
	int	dvnum;
	char	*dvname;
	int	(*dvinit)();
	int	(*dvopen)();
	int	(*dvclose)();
	int	(*dvread)();
	int	(*dvwrite)();
	int	(*dvseek)();
	int	(*dvgetc)();
	int	(*dvputc)();
	int	(*dvcntl)();
	int	dvcsr;
	int	dvivec;
	int	dvovec;
	int	(*dviint)();
	int	(*dvoint)();
	char	*dvioblk;
	int	dvminor;
	};

extern	struct	devsw devtab[];		/* one entry per device */


/* Device name definitions */

#define	CONSOLE     0			/* type tty      */
#define	TTYB        1			/* type tty      */
#define	ETHER       2			/* type eth      */
#define	UDP         3			/* type dgm      */
#define	DGRAM0      4			/* type dg       */
#define	DGRAM1      5			/* type dg       */
#define	DGRAM2      6			/* type dg       */
#define	DGRAM3      7			/* type dg       */
#define	DGRAM4      8			/* type dg       */
#define	DGRAM5      9			/* type dg       */
#define	DGRAM6      10			/* type dg       */
#define	DGRAM7      11			/* type dg       */
#define	DGRAM8      12			/* type dg       */
#define	DGRAM9      13			/* type dg       */
#define	DGRAMA      14			/* type dg       */
#define	DGRAMB      15			/* type dg       */
#define	DGRAMC      16			/* type dg       */
#define	DGRAMD      17			/* type dg       */
#define	DGRAME      18			/* type dg       */
#define	DGRAMF      19			/* type dg       */
#define	TCP         20			/* type tcpm     */
#define	TCP0        21			/* type tcp      */
#define	TCP1        22			/* type tcp      */
#define	TCP2        23			/* type tcp      */
#define	TCP3        24			/* type tcp      */
#define	TCP4        25			/* type tcp      */
#define	TCP5        26			/* type tcp      */
#define	TCP6        27			/* type tcp      */
#define	TCP7        28			/* type tcp      */
#define	TCP8        29			/* type tcp      */
#define	TCP9        30			/* type tcp      */
#define	TCPA        31			/* type tcp      */
#define	TCPB        32			/* type tcp      */
#define	TCPC        33			/* type tcp      */
#define	TCPD        34			/* type tcp      */
#define	TCPE        35			/* type tcp      */
#define	TCPF        36			/* type tcp      */

/* Control block sizes */

#define	Ntty	2
#define	Neth	1
#define	Ndgm	1
#define	Ndg	16
#define	Ntcpm	1
#define	Ntcp	16

#define	NDEVS	37

/* Declarations of I/O routines referenced */

extern	int	ttyinit();
extern	int	ionull();
extern	int	ttyread();
extern	int	ttywrite();
extern	int	ioerr();
extern	int	ttycntl();
extern	int	ttygetc();
extern	int	ttyputc();
extern	int	ethinit();
extern	int	ethwrite();
extern	int	ethcntl();
extern	int	ethinter();
extern	int	dgmopen();
extern	int	dgmcntl();
extern	int	dginit();
extern	int	dgclose();
extern	int	dgread();
extern	int	dgwrite();
extern	int	dgcntl();
extern	int	tcpmopen();
extern	int	tcpmcntl();
extern	int	tcpinit();
extern	int	tcpclose();
extern	int	tcpread();
extern	int	tcpwrite();
extern	int	tcpcntl();
extern	int	tcpgetc();
extern	int	tcpputc();


/* Configuration and Size Constants */

#define	NPROC	    50			/* number of user processes	*/
#define	NSEM	    512			/* number of semaphores		*/
#define	MEMMARK				/* define if memory marking used*/
#define	RTCLOCK				/* now have RTC support		*/
#define	STKCHK				/* resched checks stack overflow*/
#define	NETDAEMON			/* Ethernet network daemon runs	*/

#define	TSERVER	    "129.231.61.27:37"	/* Time server address		*/
#define	RSERVER	    "129.231.61.27:2001"	/* Remote file server address */
#define	NSERVER	    "129.231.61.27:53"	/* Domain Name server address	*/

#define	BINGID	9			/* my BING */

#define	SMALLMTU	400		/* for OTHER2 */

#define	NBPOOLS	5
#define	BPMAXB	4096
#define BPMAXN	512

#define	MAXMARK	20				/* max number of marked locations */

#define _NFILE	20

#define	TCPSBS	4096			/* TCP send buffer sizes	*/
#define	TCPRBS	16384			/* TCP receive buffer sizes	*/

#define	NPORTS	200

#define	TIMERGRAN	1		/* timer granularity, secs/10	*/

#define	Noth	0
