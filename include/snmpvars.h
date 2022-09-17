/* snmpvars.h */

/* System & Interface MIB */

extern	char		*SysDescr;
extern	unsigned	SysUpTime, IfNumber;

/*
 *	IP MIB
 */
extern unsigned
	IpForwarding,   IpDefaultTTL,    IpInReceives,      IpInHdrErrors,
	IpInAddrErrors, IpForwDatagrams, IpInUnknownProtos, IpInDiscards,
	IpInDelivers,   IpOutRequests,   IpOutDiscards,     IpOutNoRoutes,
	IpReasmTimeout, IpReasmReqds,    IpReasmOKs,        IpReasmFails,
	IpFragOKs,      IpFragFails,     IpFragCreates;

/*
 *	ICMP MIB
 */
extern unsigned
	IcmpInMsgs,          IcmpInErrors,         IcmpInDestUnreachs,
	IcmpInTimeExcds,     IcmpInParmProbs,      IcmpInSrcQuenchs,
	IcmpInRedirects,     IcmpInEchos,          IcmpInEchoReps,
	IcmpInTimestamps,    IcmpInTimestampReps,  IcmpInAddrMasks,
	IcmpInAddrMaskReps,  IcmpOutMsgs,          IcmpOutErrors,
	IcmpOutDestUnreachs, IcmpOutTimeExcds,     IcmpOutParmProbs,
	IcmpOutSrcQuenchs,   IcmpOutRedirects,     IcmpOutEchos,
	IcmpOutEchoReps,     IcmpOutTimestamps,    IcmpOutTimestampReps,
	IcmpOutAddrMasks,    IcmpOutAddrMaskReps;

/*
 *	UDP MIB
 */
extern unsigned
	UdpInDatagrams, UdpNoPorts, UdpInErrors, UdpOutDatagrams;

/*
 *	TCP MIB
 */
extern unsigned
	TcpRtoAlgorithm, TcpRtoMin,       TcpRtoMax,       TcpMaxConn,
	TcpActiveOpens,  TcpPassiveOpens, TcpAttemptFails, TcpEstabResets,
	TcpCurrEstab,    TcpInSegs,       TcpOutSegs,      TcpRetransSegs;
