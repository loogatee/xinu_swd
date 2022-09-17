/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <conf.h>

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*  Format of entries is:
device-number, device-name,
init, open, close,
read, write, seek,
getc, putc, cntl,
device-csr-address, input-vector, output-vector,
iint-handler, oint-handler, control-block, minor-device,
*/

/*  CONSOLE  is tty  */

0, "CONSOLE",
ttyinit, ionull, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
01770020004, 0170, 0170,
ioerr, ioerr, NULLPTR, 0,

/*  TTYB  is tty  */

1, "TTYB",
ttyinit, ionull, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
01770020000, 0170, 0170,
ioerr, ioerr, NULLPTR, 1,

/*  ETHER  is eth  */

2, "ETHER",
ethinit, ioerr, ioerr,
ioerr, ethwrite, ioerr,
ioerr, ioerr, ethcntl,
01770200000, 0154, 0154,
ethinter, ethinter, NULLPTR, 0,

/*  UDP  is dgm  */

3, "UDP",
ionull, dgmopen, ioerr,
ioerr, ioerr, ioerr,
ioerr, ioerr, dgmcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  DGRAM0  is dg  */

4, "DGRAM0",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  DGRAM1  is dg  */

5, "DGRAM1",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 1,

/*  DGRAM2  is dg  */

6, "DGRAM2",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 2,

/*  DGRAM3  is dg  */

7, "DGRAM3",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 3,

/*  DGRAM4  is dg  */

8, "DGRAM4",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 4,

/*  DGRAM5  is dg  */

9, "DGRAM5",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 5,

/*  DGRAM6  is dg  */

10, "DGRAM6",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 6,

/*  DGRAM7  is dg  */

11, "DGRAM7",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 7,

/*  DGRAM8  is dg  */

12, "DGRAM8",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 8,

/*  DGRAM9  is dg  */

13, "DGRAM9",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 9,

/*  DGRAMA  is dg  */

14, "DGRAMA",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 10,

/*  DGRAMB  is dg  */

15, "DGRAMB",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 11,

/*  DGRAMC  is dg  */

16, "DGRAMC",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 12,

/*  DGRAMD  is dg  */

17, "DGRAMD",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 13,

/*  DGRAME  is dg  */

18, "DGRAME",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 14,

/*  DGRAMF  is dg  */

19, "DGRAMF",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 15,

/*  TCP  is tcpm  */

20, "TCP",
ionull, tcpmopen, ioerr,
ioerr, ioerr, ioerr,
ioerr, ioerr, tcpmcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  TCP0  is tcp  */

21, "TCP0",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  TCP1  is tcp  */

22, "TCP1",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 1,

/*  TCP2  is tcp  */

23, "TCP2",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 2,

/*  TCP3  is tcp  */

24, "TCP3",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 3,

/*  TCP4  is tcp  */

25, "TCP4",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 4,

/*  TCP5  is tcp  */

26, "TCP5",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 5,

/*  TCP6  is tcp  */

27, "TCP6",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 6,

/*  TCP7  is tcp  */

28, "TCP7",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 7,

/*  TCP8  is tcp  */

29, "TCP8",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 8,

/*  TCP9  is tcp  */

30, "TCP9",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 9,

/*  TCPA  is tcp  */

31, "TCPA",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 10,

/*  TCPB  is tcp  */

32, "TCPB",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 11,

/*  TCPC  is tcp  */

33, "TCPC",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 12,

/*  TCPD  is tcp  */

34, "TCPD",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 13,

/*  TCPE  is tcp  */

35, "TCPE",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 14,

/*  TCPF  is tcp  */

36, "TCPF",
tcpinit, ioerr, tcpclose,
tcpread, tcpwrite, ioerr,
tcpgetc, tcpputc, tcpcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 15
	};
