

/* $Id: ivect.h,v 1.1 1992/11/24 18:09:36 johnr Exp $ */

#define		JMPOFF		10
#define		NXTVEC		14


#define		BERR		0
#define		AERR		BERR  + 14
#define		IINST		AERR  + 14
#define		ZDIV		IINST + 14
#define		CHK			ZDIV  + 14
#define		TRAPV		CHK   + 14
#define		PRIV		TRAPV + 14
#define		TRAC		PRIV  + 14
#define		L1010		TRAC  + 14
#define		L1111		L1010 + 14
#define		SINTR		L1111 + 14
#define		L1IV		SINTR + 14
#define		L2IV		L1IV  + 14
#define		L3IV		L2IV  + 14
#define		L4IV		L3IV  + 14
#define		L5IV		L4IV  + 14
#define		L6IV		L5IV  + 14
#define		L7IV		L6IV  + 14
#define		TRAP0		L7IV  + 14
#define		TRAP1		TRAP0 + 14
#define		TRAP2		TRAP1 + 14
#define		TRAP3		TRAP2 + 14
#define		TRAP4		TRAP3 + 14
#define		TRAP5		TRAP4 + 14
#define		TRAP6		TRAP5 + 14
#define		TRAP7		TRAP6 + 14
#define		TRAP8		TRAP7 + 14
#define		TRAP9		TRAP8 + 14
#define		TRAPA		TRAP9 + 14
#define		TRAPB		TRAPA + 14
#define		TRAPC		TRAPB + 14
#define		TRAPD		TRAPC + 14
#define		UIV0		TRAPD + 14
#define		UIV1		UIV0  + 14
#define		UIV2		UIV1  + 14
#define		UIV3		UIV2  + 14
#define		UIV4		UIV3  + 14
#define		UIV5		UIV4  + 14
#define		UIV6		UIV5  + 14
#define		UIV7		UIV6  + 14
#define		UIV8		UIV7  + 14
#define		UIV9		UIV8  + 14
#define		UIV10		UIV9  + 14
#define		UIV11		UIV10 + 14
#define		UIV12		UIV11 + 14
#define		UIV13		UIV12 + 14
#define		UIV14		UIV13 + 14
#define		UIV15		UIV14 + 14
