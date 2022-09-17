/* cmd.h */

/* Declarations for all commands known by the shell */

struct cmdent			/* entry in command table	*/
{
	char	*cmdnam;		/* name of command */
	Bool	cbuiltin;		/* Is this a builtin command? */
	int		(*cproc)();		/* procedure that implements cmd */
};


extern	struct	cmdent	cmds[];
