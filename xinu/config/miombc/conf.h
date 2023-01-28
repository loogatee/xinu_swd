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
#define	MIO         1			/* type mio      */

/* Control block sizes */

#define	Ntty	1
#define	Nmio	1

#define	NDEVS	2

/* Declarations of I/O routines referenced */

extern	int	ttyinit();
extern	int	ionull();
extern	int	ttyread();
extern	int	ttywrite();
extern	int	ioerr();
extern	int	ttycntl();
extern	int	ttygetc();
extern	int	ttyputc();
extern	int	mioinit();


/* Configuration and Size Constants */

#define	NPROC	    50			/* number of user processes	*/
#define	NSEM	    512			/* number of semaphores		*/
#define	MEMMARK				/* define if memory marking used*/
#define	RTCLOCK				/* now have RTC support		*/
#define	STKCHK				/* resched checks stack overflow*/
/* #define	NETDAEMON */		/* Ethernet network daemon runs	*/

/* #define	TSERVER	    "128.10.2.3:37" */	/* Time server address	*/
/* #define	RSERVER	    "128.10.3.20:2001" */ /* Rmt file server address */
/* #define	NSERVER	    "128.10.3.20:53" */	/* Domain Name server address */

/* #define	BINGID	9 */			/* my BING */

#define NBPOOLS	5
#define	BPMAXB	4096
#define BPMAXN	512

#define MAXMARK	20		/* max number of marked locations */
 
#define _NFILE	20

/* #define	TCPSBS	4096 */		/* TCP send buffer sizes	*/
/* #define	TCPRBS	16384 */	/* TCP receive buffer sizes	*/

#define	NPORTS	200

#define	TIMERGRAN	1		/* timer granularity, secs/10	*/

#define	Noth	0
