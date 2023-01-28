



/*
 *	Address of the  system address table
 */
#define	ADDR_SAT		0x300

/*
 *	Offsets of various routines in the system address table
 */
#define	OFF_GV			0				/* get vectors */
#define	OFF_PUTCH		4				/* put character */
#define	OFF_ADDE		8				/* add command extension table */
#define OFF_GIV			12				/* get ivect */

/*
 *	The "magic" I/O address of the MIO board (Misc. port)
 */
#define	IOREG			0xf00001

/*
 *	Mask Defines for the bits in the Misc. port
 */
#define	MP_TP			0x1				/* Testpage switch */
#define	MP_GDLNK		0x2				/* GDLNK */
#define MP_AUI			0x4				/* AUITPI */
#define MP_PRQ			0x8				/* PRQ */
#define MP_CLOCK		0x10			/* Clock tick IPEND */
#define MP_ENET			0x20			/* Ethernet IPEND */
#define MP_MIO			0x40			/* MIO IPEND */


#define	ACC_TXD		((u_char *)0x800001)
#define	ACC_RXD		((u_char *)0x800001)
#define	ACC_BAL		((u_char *)0x800001)
#define	ACC_BAH		((u_char *)0x800003)
#define	ACC_IER		((u_char *)0x800003)
#define	ACC_IIR		((u_char *)0x900001)
#define	ACC_LCR		((u_char *)0x900003)
#define	ACC_MCR		((u_char *)0xa00001)
#define	ACC_LSR		((u_char *)0xa00003)
#define	ACC_MSR		((u_char *)0xb00001)
#define	ACC_SCR		((u_char *)0xb00003)


/*
 *	Length of reserved area in ram for the monitor
 */
#define	RAM_MON_RESRV	0x1000
