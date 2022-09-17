/* ether_bsp.h */

#define EP_NUMRCV 16		/* number LANCE recv buffers (power 2)	*/
#define EP_NUMRCVL2 4		/* log2 of the number of buffers	*/


/*
 *	structure for handling a buffer ring entry for the LANCE
 */
struct bre
{
    struct le_md	*pmd;		/* pointer to the message descriptor */
    char			*buf;		/* pointer to the buffer for this desc */
    int				flags;		/* status of the buffer */
};

/*
 *	Ethernet control block descriptions,
 *			Board specific stuff
 */
struct etblk_bsp
{
	struct le_init_block	*etib;			/* initialization block */
	struct bre				etbrt[1];		/* ring descriptor, transmits */
	struct bre				etbrr[EP_NUMRCV];	/* ring descriptor, receives */
};

extern	struct	etblk_bsp	eth_bsp[];
