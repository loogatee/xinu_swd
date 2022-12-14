/* sif_get.c - sif_get */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <mib.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * sif_get - perform a get on a variable in the Interfaces Table
 *------------------------------------------------------------------------
 */
int sif_get(bindl, numifaces)
struct snbentry	*bindl;
int		numifaces;
{
	int 	iface, field, sl;
	
	if (sif_match(bindl, &iface, &field, numifaces) == SYSERR)
		return SERR_NO_SUCH;
	switch (field) {
	case 1:		/* ifIndex */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = iface;
		return SNMP_OK;
	case 2:		/* ifDescr */
		SVTYPE(bindl) = ASN1_OCTSTR;
		SVSTRLEN(bindl) = sl = strlen(nif[iface].ni_descr);
		xp_tracev(80,"sif_get: 1 getmem  %d\n",sl);
		SVSTR(bindl) = (char *) getmem(sl);
		blkcopy(SVSTR(bindl), nif[iface].ni_descr, sl);
		return SNMP_OK;
	case 3:		/* ifType */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = nif[iface].ni_mtype;
		return SNMP_OK;
	case 4:		/* ifMtu */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = nif[iface].ni_mtu;
		return SNMP_OK;
	case 5:		/* ifSpeed */
		SVTYPE(bindl) = ASN1_GAUGE;
		SVINT(bindl) = nif[iface].ni_speed;
		return SNMP_OK;
	case 6:		/* ifPhysAddress */
		SVTYPE(bindl) = ASN1_OCTSTR;
		xp_tracev(80,"sif_get: 2 getmem  %d\n",nif[iface].ni_hwa.ha_len);
		SVSTR(bindl) = (char *) getmem(nif[iface].ni_hwa.ha_len);
		blkcopy(SVSTR(bindl), nif[iface].ni_hwa.ha_addr,
			SVSTRLEN(bindl) = nif[iface].ni_hwa.ha_len);
		return SNMP_OK;
	case 7:		/* ifAdminStatus */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = nif[iface].ni_admstate;
		return SNMP_OK;
	case 8:		/* ifOperStatus */
		SVTYPE(bindl) = ASN1_INT;
		SVINT(bindl) = nif[iface].ni_state;
		return SNMP_OK;
	case 9:		/* ifLastChange */
		SVTYPE(bindl) = ASN1_TIMETICKS;
		SVINT(bindl) = nif[iface].ni_lastchange;
		return SNMP_OK;
	case 10:	/* ifInOctets */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_ioctets;
		return SNMP_OK;
	case 11:	/* ifInUcastPkts */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_iucast;
		return SNMP_OK;
	case 12:	/* ifInNUcastPkts */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_inucast;
		return SNMP_OK;
	case 13:	/* ifInDiscards */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_idiscard;
		return SNMP_OK;
	case 14:	/* ifInErrors */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_ierrors;
		return SNMP_OK;
	case 15:	/* ifInUnknownProtos */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_iunkproto;
		return SNMP_OK;
	case 16:	/* ifOutOctets */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_ooctets;
		return SNMP_OK;
	case 17:	/* ifOutUcastPkts */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_oucast;
		return SNMP_OK;
	case 18:	/* ifOutNUcastPkts */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_onucast;
		return SNMP_OK;
	case 19:	/* ifOutDiscards */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_odiscard;
		return SNMP_OK;
	case 20:	/* ifOutErrors */
		SVTYPE(bindl) = ASN1_COUNTER;
		SVINT(bindl) = nif[iface].ni_oerrors;
		return SNMP_OK;
	case 21:	/* ifOutQLen */
		SVTYPE(bindl) = ASN1_GAUGE;
		SVINT(bindl) = lenq(nif[iface].ni_outq);
		return SNMP_OK;
	default:
		break;
	}
	return SERR_NO_SUCH;
}
