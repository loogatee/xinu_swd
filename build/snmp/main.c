/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>





/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
main()
{
    resume(create(snmpd, SNMPSTK, SNMPPRI, SNMPDNAM, 0));
}



#define	extern
#include <snmpvars.h>
