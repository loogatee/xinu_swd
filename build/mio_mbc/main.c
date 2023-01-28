#include <conf.h>
#include <kernel.h>
#include <pdtypes.h>
#include <mbc_def.h>
#include <mbc_func.h>
#include <mbc_text.h>

extern	PROCESS		MBC_Main(), testpage_d(), despool_d();


BYTE	MIO_Mail;		/* Might want to move these */
BYTE	PER_Mail;		/* Might want to move these */
BYTE	OnLine;			/* Might want to move these */
BYTE	MIO_Error;		/* Might want to move these */
int		IntrMIO;

int		shell_main = 0;


/*
 *	This comment added here just for the heck of it!
 */
void
main()
{
	/*
	 *	Initialize data structures associated with the spooler
	 */
	spool_init();

	/*
	 *	Initialize a couple of semaphores, then start up the MBC code
	 */ 
	IntrMIO = screate(0);
	resume( create(MBC_Main, 8000L, 40, "MBC", 0) );

	/*
	 *	start up the de-spooler, and the testpage process
	 */
	resume( create(despool_d,  4096, 35, "despool", 0) );
	resume( create(testpage_d, 4096, 35, "testpg", 0) );

	/*
	 *	Fire up the Xinu shell
	 */
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
