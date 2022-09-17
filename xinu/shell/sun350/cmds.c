/* cmd.h */

#include <conf.h>
#include <kernel.h>
#include <cmd.h>

extern	int		x_arp(), x_bpool(), x_cat(), x_close(), x_conf(), x_cp();
extern	int		x_creat(), x_date(), x_devs(), x_dg(), x_dumper(), x_echo();
extern	int		x_exit(), x_finger(), x_help(), x_ifstat(), x_kill(), x_mem();
extern	int		x_mount(), x_mv(), x_net(), x_ns(), x_ping(), x_ps();
extern	int		x_reboot(), x_rf(), x_rls(), x_rm(), x_route(), x_routes();
extern	int		x_sleep(), x_snmp(), x_timerq(), x_prelog(), x_unmou();
extern	int		x_uptime(), x_who(), x_ptrace(), x_trcb(), x_trkp(), x_trse();
extern	int		x_trsp(), x_spoolq(), x_enttyb(), x_hd();

/* Commands:	 name		Builtin?	procedure	*/

#define	CMDS 	"arp",		FALSE,		x_arp,		\
				"bpool",	FALSE,		x_bpool,	\
				"conf",		FALSE,		x_conf,		\
				"devs",		FALSE,		x_devs,		\
				"dg",		FALSE,		x_dg,		\
				"echo",		FALSE,		x_echo,		\
				"enttyb",	FALSE,		x_enttyb,	\
				"exit",		TRUE,		x_exit,		\
				"help",		FALSE,		x_help,		\
				"hd",		FALSE,		x_hd,		\
				"ifstat",	FALSE,		x_ifstat,	\
				"kill",		TRUE,		x_kill,		\
				"logout",	TRUE,		x_exit,		\
				"mem",		FALSE,		x_mem,		\
				"netstat",	FALSE,		x_net,		\
				"ns",		FALSE,		x_ns,		\
				"ping",		FALSE,		x_ping,		\
				"prelog",	FALSE,		x_prelog,	\
				"ps",		FALSE,		x_ps,		\
				"reboot",	TRUE,		x_reboot,	\
				"route",	FALSE,		x_route,	\
				"routes",	FALSE,		x_routes,	\
				"sleep",	FALSE,		x_sleep,	\
				"spoolq",	FALSE,		x_spoolq,	\
				"time",		FALSE,		x_date,		\
				"timerq",	FALSE,		x_timerq,	\
				"trgb",		FALSE,		x_ptrace,	\
				"trcb",		FALSE,		x_trcb,		\
				"trkp",		FALSE,		x_trkp,		\
				"trse",		FALSE,		x_trse,		\
				"trsp",		FALSE,		x_trsp,		\
				"who",		FALSE,		x_who,		\
				"?",		FALSE,		x_help

struct cmdent	cmds[] = {CMDS};

szof_cmds()
{
	return(sizeof(cmds));
}
