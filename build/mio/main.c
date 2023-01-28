/* user.c - main */

#include <conf.h>
#include <kernel.h>


extern	PROCESS		testpage_d();


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
	 *	Start of the testpage process
	 */
	resume( create(testpage_d, 4096, 10, "testpg", 0) );


	if (shell_main == 0)
	{
		shell_main = 1;
		/*
		 * Fire up the shell.
		 */
		while(TRUE) {
			login(CONSOLE,0);
			shell(CONSOLE,0);
		}
	}
}
