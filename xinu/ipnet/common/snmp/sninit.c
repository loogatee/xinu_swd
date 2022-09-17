/* sninit.c - sninit */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <ctype.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <snmpvars.h>

extern char vers[];

char		*SysDescr;
struct oid	SysObjectID = { {0}, 1};

/*
 *	System & Interface MIB
 */
unsigned	SysUpTime, IfNumber;

/*
 *	IP MIB
 */
unsigned	IpForwarding,   IpDefaultTTL,    IpInReceives,      IpInHdrErrors,
			IpInAddrErrors, IpForwDatagrams, IpInUnknownProtos, IpInDiscards,
			IpInDelivers,   IpOutRequests,   IpOutDiscards,     IpOutNoRoutes,
			IpReasmTimeout, IpReasmReqds,    IpReasmOKs,        IpReasmFails,
			IpFragOKs,      IpFragFails,     IpFragCreates;

/*
 *	ICMP MIB
 */
unsigned	IcmpInMsgs,          IcmpInErrors,        IcmpInDestUnreachs,
			IcmpInTimeExcds,     IcmpInParmProbs,     IcmpInSrcQuenchs,
			IcmpInRedirects,     IcmpInEchos,         IcmpInEchoReps,
			IcmpInTimestamps,    IcmpInTimestampReps, IcmpInAddrMasks,
			IcmpInAddrMaskReps,  IcmpOutMsgs,         IcmpOutErrors,
			IcmpOutDestUnreachs, IcmpOutTimeExcds,    IcmpOutParmProbs,
			IcmpOutSrcQuenchs,   IcmpOutRedirects,    IcmpOutEchos,
			IcmpOutEchoReps,     IcmpOutTimestamps,   IcmpOutTimestampReps,
			IcmpOutAddrMasks,    IcmpOutAddrMaskReps;

/*
 *	UDP MIB
 */
unsigned	UdpInDatagrams, UdpNoPorts, UdpInErrors, UdpOutDatagrams;

/*
 *	TCP MIB
 */
unsigned	TcpRtoAlgorithm, TcpRtoMin,       TcpRtoMax,       TcpMaxConn,
			TcpActiveOpens,  TcpPassiveOpens, TcpAttemptFails, TcpEstabResets,
			TcpCurrEstab,    TcpInSegs,       TcpOutSegs,
			TcpRetransSegs;

int snmpinitialized = FALSE;

/*------------------------------------------------------------------------
 * sninit - initialize the data structures for the SNMP server and client
 *------------------------------------------------------------------------
 */
sninit()
{
	int	i;

	if (snmpinitialized)
		return;

	snmpinitialized = TRUE;

	hashinit();

	SysUpTime = 0;
	SysDescr  = vers;
	IfNumber  = Net.nif - 1;
	
	IpDefaultTTL        = IP_TTL;			IpInReceives         = 0;
	IpInHdrErrors       = 0;				IpInAddrErrors       = 0;
	IpForwDatagrams     = 0;				IpInUnknownProtos    = 0;
	IpInDiscards        = 0;				IpInDelivers         = 0;
	IpOutRequests       = 0;				IpOutDiscards        = 0;
	IpOutNoRoutes       = 0;				IpReasmTimeout       = IP_FTTL;
	IpReasmReqds        = 0;				IpReasmOKs           = 0;
	IpReasmFails        = 0;				IpFragOKs            = 0;
	IpFragFails         = 0;				IpFragCreates        = 0;
	IcmpInMsgs          = 0;				IcmpInErrors         = 0;
	IcmpInDestUnreachs  = 0;				IcmpInTimeExcds      = 0;
	IcmpInParmProbs     = 0;				IcmpInSrcQuenchs     = 0;
	IcmpInRedirects     = 0;				IcmpInEchos          = 0;
	IcmpInEchoReps      = 0;				IcmpInTimestamps     = 0;
	IcmpInTimestampReps = 0;				IcmpInAddrMasks      = 0;
	IcmpInAddrMaskReps  = 0;				IcmpOutMsgs          = 0;
	IcmpOutErrors       = 0;				IcmpOutDestUnreachs  = 0;
	IcmpOutTimeExcds    = 0;				IcmpOutParmProbs     = 0;
	IcmpOutSrcQuenchs   = 0;				IcmpOutRedirects     = 0;
	IcmpOutEchos        = 0;				IcmpOutEchoReps      = 0;
	IcmpOutTimestamps   = 0;				IcmpOutTimestampReps = 0;
	IcmpOutAddrMasks    = 0;				IcmpOutAddrMaskReps  = 0;
	TcpRtoAlgorithm     = 4;				TcpActiveOpens       = 0;
	TcpRtoMax           = TCP_MAXRXT*10;	TcpMaxConn           = Ntcp;
	TcpRtoMin           = TCP_MINRXT*10;	TcpPassiveOpens      = 0;
	TcpAttemptFails     = 0;				TcpEstabResets       = 0;
	TcpCurrEstab        = 0;				TcpInSegs            = 0;
	TcpOutSegs          = 0;				TcpRetransSegs       = 0;
	UdpInDatagrams      = 0;				UdpNoPorts           = 0;
	UdpInErrors         = 0;				UdpOutDatagrams      = 0;

	for (i=0; i < Net.nif; ++i)
		nif[i].ni_lastchange = 0;
}
