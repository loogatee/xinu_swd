/* bufpool.h */

#define	BPMINB	2			/* Minimum buffer length	*/


struct	bpool	{			/* Description of a single pool	*/
	int	bpsize;			/* size of buffers in this pool	*/
	char	*bpnext;		/* pointer to next free buffer	*/
	int	bpsem;			/* semaphore that counts buffers*/
	};				/*  currently in THIS pool	*/

extern	struct	bpool bptab[];		/* Buffer pool table		*/
extern	int	nbpools;		/* current number of pools	*/
extern	MARKER	bpmark;
