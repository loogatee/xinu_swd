#ifndef lint
static char rcsid[] = "$Id: rlogind.c,v 1.2 1992/12/02 22:22:27 johnr Exp $";
#endif

#include <conf.h>
#include <kernel.h>
#include <network.h>

static	int		nsh_active;
static	int		nsh_pid;

PROCESS
net_shell()
{
	int				fd;
	struct devsw	*devptr;
	struct tcb		*ptcb;

	while (1)
	{
		fd = receive();

		/*
		 * Fire up the shell.
		 */
		if (login(fd,1) == OK)
			shell(fd,1);

		/*
		 *	Close the TCP slave device
		 */
		devptr          = &devtab[fd];
		ptcb            = (struct tcb *)devptr->dvioblk;
		ptcb->tcb_state = TCPS_CLOSEWAIT;
		close(fd);

		nsh_active = 0;
	}
}


PROCESS
rlogin_d()
{
	int				fd;
	int				fd1;
	char			buf[80];
	char			rbuf[2];
	struct devsw	*devptr;
	struct tcb		*ptcb;

	rbuf[1]    = 0;
	nsh_active = 0;

	nsh_pid = create(net_shell,INITSTK,INITPRIO,"net_shell",INITARGS);
	resume(nsh_pid);

	/*
	 *	Open up the TCP master device and set the listen queue length
	 */
	fd = open(TCP, ANYFPORT, 513);
	control(fd, TCPC_LISTENQ, 2);

	while (1)
	{
		/*
		 *	Wait for a client to initiate a connection
		 */
		fd1 = control(fd, TCPC_ACCEPT);

		/*
		 *	Read the "pre-connection" data, then respond with a 0
		 */
		read(fd1, buf, 80);
		read(fd1, buf, 80);

		if (nsh_active == 0)
			rbuf[0] = 0;
		else
			rbuf[0] = 1;

		write(fd1, rbuf, 1);

		if (nsh_active == 0)
		{
			nsh_active = 1;
			send(nsh_pid,fd1);
		}
		else
		{
			/*
			 *	Close the TCP slave device
			 */
			devptr          = &devtab[fd1];
			ptcb            = (struct tcb *)devptr->dvioblk;
			ptcb->tcb_state = TCPS_CLOSEWAIT;
			close(fd1);
		}
	}
}


/*
 *	$Log: rlogind.c,v $
 * Revision 1.2  1992/12/02  22:22:27  johnr
 * JR:  Changed the strategy for the rlogin_d process.  rlogin_d now serves
 *      only to accept/reject the TCP connections.  Currently only 1 is
 *      allowed.  rlogin_d creates a new process, net_shell, which does the
 *      actual creation of the shell.  If a network shell is currently active
 *      (controlled by the variable nsh_active), then rlogin_d disallows the
 *      TCP connection and answers the accept by returning 1.  If a network
 *      shell is not currently active (net_active = 0), then the connection may
 *      be allowed, and rlogin_d sends a message to net_shell to get it going.
 *      The messsage represents (much like in lpd) the file descriptor to use
 *      for reading and writing to/from the network.
 *
 * Revision 1.1  1992/12/02  00:35:37  johnr
 * JR:  1st entry for this file.  This is a Xinu server process which
 *      accepts rlogin connections, and fires off a shell to communicate
 *      over the open channel.
 *
 *
 */
