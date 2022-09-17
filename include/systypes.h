/*	@(#)types.h 2.4 85/02/06 SMI; from UCB 4.11 83/07/01	*/

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* System V compatibility */

typedef	struct	_quad { long val[2]; } quad;
typedef	long	daddr_t;
typedef	char *	caddr_t;
typedef	u_long	ino_t;
typedef	long	swblk_t;
typedef	int		size_t;
typedef	int		time_t;
typedef	short	dev_t;
typedef	int		off_t;

typedef	struct	fd_set { int fds_bits[1]; } fd_set;
