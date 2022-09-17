/* snmpd.c - snmpd */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <snmp.h>
#include <asn1.h>

/*------------------------------------------------------------------------
 * snmpd - open the SNMP port and handle incoming queries
 *------------------------------------------------------------------------
 */
int
snmpd()
{
	int					snmpdev;
	int					len;
	struct xgram		*query;
	struct req_desc		rqd;

	sninit();

	printf("sizeof(struct snbentry) = %d\n",sizeof(struct snbentry));

	query = (struct xgram *) getmem(sizeof (struct xgram));

	/*
	 *	open the SNMP server port
	 */
	if ((snmpdev = open(UDP, ANYFPORT, SNMPPORT)) == SYSERR)
		return SYSERR;

	while (TRUE)
	{
		/*
		 *	In this mode, give read the size of xgram, it returns
		 *	number of bytes of *data* in xgram.
		 */
		len = read(snmpdev, query, sizeof(struct xgram));

		xp_tracev(80,"snmpd: starting\n",0);

/*
		xp_tracev(80,"snmpd: len        = 0x%x\n",len);
		xp_tracev(80,"snmpd: xg_data[0] = 0x%x\n",query->xg_data[0]);
		xp_tracev(80,"snmpd: xg_data[1] = 0x%x\n",query->xg_data[1]);
		xp_tracev(80,"snmpd: xg_data[2] = 0x%x\n",query->xg_data[2]);
		xp_tracev(80,"snmpd: xg_data[3] = 0x%x\n",query->xg_data[3]);
		xp_tracev(80,"snmpd: xg_data[4] = 0x%x\n",query->xg_data[4]);
		xp_tracev(80,"snmpd: xg_data[5] = 0x%x\n",query->xg_data[5]);
		xp_tracev(80,"snmpd: xg_data[6] = 0x%x\n",query->xg_data[6]);
		xp_tracev(80,"snmpd: xg_data[7] = 0x%x\n",query->xg_data[7]);
*/

		/*
		 *	parse the packet into the request desc. structure
		 */
		if (snparse(&rqd, query->xg_data, len) == SYSERR)
		{
			snfreebl(rqd.bindlf);
			continue;
		}

		xp_tracev(80,"snmpd: snparse returned OK\n",0);

		/*
		 *	convert ASN.1 representations to internal forms
		 */
		if (sna2b(&rqd) == SYSERR)
		{
			snfreebl(rqd.bindlf);
			continue;
		}

		dump_rqd(&rqd);

		/*
		 *	Resolve the query.  This consists of performing the specified
		 *	get, set, or get-next operation for each identifier in the
		 *	request list.
		 */
		if (snrslv(&rqd) == SYSERR)
		{
			query->xg_data[rqd.pdutype_pos]  = PDU_RESP;
			query->xg_data[rqd.err_stat_pos] = rqd.err_stat;
			query->xg_data[rqd.err_idx_pos]  = rqd.err_idx;

			if (write(snmpdev, query, len) == SYSERR)
				return SYSERR;

			snfreebl(rqd.bindlf);
			continue;
		}

		/*
		 *	Form a reply message
		 */
		len = mksnmp(&rqd, query->xg_data, PDU_RESP);

		if (len == SYSERR)
		{
			query->xg_data[rqd.pdutype_pos]  = PDU_RESP;
			query->xg_data[rqd.err_stat_pos] = rqd.err_stat;
			query->xg_data[rqd.err_idx_pos]  = rqd.err_idx;

			if (write(snmpdev, query, len) == SYSERR)
				return SYSERR;

			snfreebl(rqd.bindlf);
			continue;	
		}

		/*
		 *	Send the finished reply to the client
		 */
		if (write(snmpdev, query, len) == SYSERR) 
			return SYSERR;

		/*
		 *	Free up the linked list of names
		 */
		snfreebl(rqd.bindlf);
	}
}


dump_rqd(rqd)
struct req_desc		*rqd;
{
	struct snbentry		*snptr1;
	struct snbentry		*snptr2;
	int					l;
	int					i;

	xp_tracev(80,"    reqtype      = %d\n",rqd->reqtype);
	xp_tracev(80,"    reqid[0]     = %d\n",rqd->reqid[0]);
	xp_tracev(80,"    reqid[1]     = %d\n",rqd->reqid[1]);
	xp_tracev(80,"    reqid[2]     = %d\n",rqd->reqid[2]);
	xp_tracev(80,"    reqid[3]     = %d\n",rqd->reqid[3]);
	xp_tracev(80,"    reqid[4]     = %d\n",rqd->reqid[4]);
	xp_tracev(80,"    reqid[5]     = %d\n",rqd->reqid[5]);
	xp_tracev(80,"    reqid[6]     = %d\n",rqd->reqid[6]);
	xp_tracev(80,"    reqid[7]     = %d\n",rqd->reqid[7]);
	xp_tracev(80,"    reqid[8]     = %d\n",rqd->reqid[8]);
	xp_tracev(80,"    reqid[9]     = %d\n",rqd->reqid[9]);
	xp_tracev(80,"    reqidlen     = %d\n",rqd->reqidlen);
	xp_tracev(80,"    err_stat     = %d\n",rqd->err_stat);
	xp_tracev(80,"    err_idx      = %d\n",rqd->err_idx);
	xp_tracev(80,"    err_stat_pos = %d\n",rqd->err_stat_pos);
	xp_tracev(80,"    err_idx_pos  = %d\n",rqd->err_idx_pos);
	xp_tracev(80,"    pdutype_pos  = %d\n",rqd->pdutype_pos);
	xp_tracev(80,"    bindlf       = 0x%x\n",rqd->bindlf);
	xp_tracev(80,"    bindle       = 0x%x\n",rqd->bindle);

	snptr1 = rqd->bindlf;
	snptr2 = rqd->bindle;

	if (snptr1 != 0)
	{
		while (1)
		{
			if (snptr1->sb_a1slen == 0)
				xp_tracev(80,"        sb_a1str  = 0!\n",0);
			else
				xp_tracev(80,"        sb_a1str  = %s\n",snptr1->sb_a1str);
			xp_tracev(80,"        sb_a1slen = %d\n",snptr1->sb_a1slen);

			xp_tracev(80,"        sb_oid.len = %d\n",snptr1->sb_oid.len);

			for (i=0; i < snptr1->sb_oid.len; ++i)
				xp_tracev(80,"id[x] = %d\n",snptr1->sb_oid.id[i]);

			snptr1 = snptr1->sb_next;

			if (snptr1 == 0)
				break;
		}
	}
}
