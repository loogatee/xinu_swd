/* network.h */

/* All includes needed for the network */

#include <ip.h>
#include <ether.h>
#include <ipreass.h>
#include <icmp.h>
#include <udp.h>
#include <tcp.h>
#include <tcpfsm.h>
#include <tcpstat.h>
#include <tcb.h>
#include <net.h>
#include <dgram.h>
#include <arp.h>
#include <domain.h>
#include <netif.h>
#include <route.h>
#include <rip.h>
#include <daemon.h>
#include <snmpvars.h>

/* Declarations data conversion and checksum routines */

extern	short		cksum();	/* 1s comp of 16-bit 2s comp sum*/

/* for talking to VAXen */
#define h2vax(x) (unsigned) ((unsigned) ((x)>>8) + (unsigned)((x)<<8))
#define vax2h(x) (unsigned) ((unsigned) ((x)>>8) + (unsigned)((x)<<8))

/* The Sun uses network byte order already!! */
#define hs2net(x) (x)
#define net2hs(x) (x)
#define hl2net(x) (x)
#define net2hl(x) (x)

/* network macros */
#define hi8(x)   (unsigned char)  (((long) (x) >> 16) & 0x00ff)
#define low16(x) (unsigned short) ((long) (x) & 0xffff)

#define	BYTE(x, y)	(x[y]&0xff)
