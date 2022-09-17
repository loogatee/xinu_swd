/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <tty.h>



PROCESS		tftp_d();
PROCESS		lp_d();
PROCESS		despool_d();
PROCESS		rlogin_d();


int		shell_main = 0;


aux_shell()
{
	sleep(30);

	if (shell_main == 0)
	{
		shell_main = 1;
		while (TRUE)
		{
			login(CONSOLE,0);
			shell(CONSOLE,0);
		}
	}
}


/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
main()
{
	/*
	 * Initialize data structures associated with the spooler
	 */
	spool_init();

	/*
	 * start up the despooler (for a serial port)
	 */
	resume( create(despool_d, 4096, 35, "despool", 0) );

	/*
	 * start up the tftpd server
	 */
	resume( create(tftp_d, 8192, 35, "tftpd", 0) );

	/*
	 * start up the lpd server
	 */
	resume( create(lp_d, 1024, 35, "lpd", 0) );

	/*
	 * start of the rlogind server
	 */
	resume( create(rlogin_d, 1024, 20, "rlogind", 0) );

	rtadd("\201\347\101\0", "\377\377\377\0", "\201\347\75\1", 1, 1, RT_INF);
	rtadd("\201\347\105\0", "\377\377\377\0", "\201\347\75\40", 1, 1, RT_INF);

	if (shell_main == 0)
	{
		shell_main = 1;

		while (1)
		{
			login(CONSOLE,0);
			shell(CONSOLE,0);
		}
	}
}


/*
 *	Write the back-end data out to TTYB
 */
backend_write(buf,len)
char	*buf;
int		len;
{
	char	rbuf[2];

	/*
	 *	Effectively flush the input buffer
	 */
	while (control(TTYB,TCICHARS) != 0)
		read(TTYB,rbuf,1);

	/*
	 *	Write the data to the device
	 */
	write(TTYB,buf,len);
}







#define	extern
#include <snmpvars.h>
