#ifndef lint
static char rcsid[] = "$Id: spooler.c,v 1.4 1993/01/30 02:57:38 johnr Exp $";
#endif

#include <conf.h>
#include <kernel.h>
#include <systypes.h>
#include <spooler.h>
#include <lpd.h>
#include <ctype.h>

/*
 *	EXTERNALLY REFERENCED FUNCTIONS
 */
extern	WORD		*getmem();
extern	u_char		*index();

/*
 *	LOCAL FUNCTIONS - GLOBAL
 */
void		spool_init(), spool_retqjob(), spool_downcode();
SPL_QJOBS	*spool_connect();
int			spool_write();
void		spool_retdnode(), spool_disconnect(), spool_username();
void		spool_filename(), spool_hostname(), spool_hostip();
void		spool_stats(), spool_lpq(), spool_lprm();

/*
 *	LOCAL FUNCTIONS - STATIC
 */
static	SPL_DNODE	*spool_getdnode();
static	void		spool_lprm_case1(), spool_lprm_case2(), spool_lprm_case3();

/*
 *	LOCAL VARIABLES - GLOBAL
 */
SPL_QJOBS	*spl_qjactive_start;			/* start of qjob active list */
int			spl_qjact_sem;					/* semaphore for qjob active list */

/*
 *	LOCAL VARIABLES - STATIC
 */
static	SPL_DNODE	*spl_dnode_start;		/* start of dnode free list */
static	SPL_DNODE	*spl_dnode_end;			/* last node of dnode free list */
static	SPL_QJOBS	*spl_qjfree_start;		/* start of queue-job free list */
static	SPL_QJOBS	*spl_qjfree_end;		/* last node of qjob free list */
static	SPL_QJOBS	*spl_qjactive_end;		/* last node of qjob active list */
static	u_long		spl_bytesusd;			/* bytes used */
static	u_long		spl_tbytesav;			/* total bytes available */
static	int			spl_numdnodes;			/* total number of dnodes */
static	int			spl_numqjobs;			/* total number of Q jobs */
static	u_long		spl_sequence;			/* key sequencer */
static	int			spl_dnode_sem;			/* semaphore for dnode list */
static	int			spl_qjfree_sem;			/* semaphore for qjob free list */
static	int			spl_busd_sem;			/* semaphore for spl_bytesusd */
static	SPL_DNODE	*spl_dnode_mem;			/* holds start of dnode memory */


/*
 *	FUNCTION spool_init - Initialize spooler structures
 *
 *	DESCRIPTION
 *		This function initializes all the data structures and variables
 *		associated with the spooler.  All of the dnodes are placed on
 *		a list pointed to by spl_dnode_start.  All of the QJOBS are placed
 *		on the "free" list pointed to by spl_qjfree_start.  The "active"
 *		QJOB list is initially empty.  Several semaphores are initialized
 *		also.
 *
 *		This function is called by "main", and needs to execute before
 *		any processes are started that make reference to the spooler.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
spool_init()
{
	int			i;
	SPL_DNODE	*dnode_ptr;
	SPL_DNODE	*tmp;
	SPL_QJOBS	*qjob_ptr;
	SPL_QJOBS	*qtmp;
	SPL_QJOBS	*qtmp1;

	/*
	 *	Temporary Kludge:
	 *		The value of both these variables needs to be determined
	 *		at run time based on the amount of available memory.
	 */
	spl_numdnodes   = SPL_NUMDNODES;
	spl_numqjobs    = SPL_NUMQJOBS;

	/*
	 *	Use getmem to grab some available memory from the xinu heap.
	 */
	spl_dnode_start  = (SPL_DNODE *)getmem(spl_numdnodes * SPL_SZDNODE);
	spl_dnode_mem    = spl_dnode_start;
	spl_qjfree_start = (SPL_QJOBS *)getmem(spl_numqjobs * SPL_SZQJOBS);

	/*
	 *	Initialize the dnode list.
	 */
	for (dnode_ptr=spl_dnode_start, i=0; i < spl_numdnodes; ++i, ++dnode_ptr)
	{
		tmp                  = dnode_ptr;
		dnode_ptr->in_status = 0;
		dnode_ptr->in_len    = 0;
		dnode_ptr->in_jobid  = 0;			/* jobid == 0 when on free list */
		dnode_ptr->in_link   = ++tmp;
	}
	spl_dnode_end          = --dnode_ptr;
	spl_dnode_end->in_link = 0;

	/*
	 *	Initialize the Queue-job array structure
	 */
	for (qjob_ptr=spl_qjfree_start, i=0; i < spl_numqjobs; ++i, ++qjob_ptr)
	{
		qtmp = qtmp1       = qjob_ptr;
		qjob_ptr->qj_jobid = 0;			/* jobid == 0 when on free list */
		qjob_ptr->qj_len   = 0;
		qjob_ptr->qj_iptr  = 0;
		qjob_ptr->qj_fillp = 0;
		qjob_ptr->qj_flink = ++qtmp;
		qjob_ptr->qj_blink = --qtmp1;
	}
	spl_qjfree_end             = --qjob_ptr;
	spl_qjfree_start->qj_blink = 0;
	spl_qjfree_end->qj_flink   = 0;
	spl_qjactive_start         = 0;
	spl_qjactive_end           = 0;

	/*
	 *	Initialize some other global variables
	 */
	spl_tbytesav = SPL_SZDATA * spl_numdnodes;
	spl_bytesusd = 0;
	spl_sequence = 1;

	spl_dnode_sem  = screate(1);
	spl_qjfree_sem = screate(1);
	spl_qjact_sem  = screate(1);
	spl_busd_sem   = screate(1);
}

/*
 *	FUNCTION spool_retqjob - return a QJOB to the free list
 *
 *	DESCRIPTION
 *		This function deletes a given QJOB from the active list, and places
 *		it onto the free list.  The removal of the QJOB from the active
 *		list is handled such that the node can be anywhere on the list.
 *		Addition of the QJOB to the free list is done by placing the node
 *		at the end of the list.
 *
 *	PARAMETERS
 *		qjptr		-	pointer to the relevent QJOB
 *		wait_flag	-	A boolean flag value which controls whether or
 *						not the wait/signal pair is executed on spl_qjact_sem.
 *							wait_flag = 0, do the wait/signal
 *							wait_flag = 1, do NOT perform the wait/signal
 *
 *	RETURN VALUES
 *		none
 */
void
spool_retqjob(qjptr,wait_flag)
SPL_QJOBS	*qjptr;
int			wait_flag;
{
	SPL_QJOBS	*tptr;

	/*
	 *	Protect the QJOB active list
	 */
	if (wait_flag == 0)
		wait(spl_qjact_sem);

	/*
	 *	Delete the QJOB from the active list
	 */
	if (spl_qjactive_start == qjptr)
	{
		if ((spl_qjactive_start = qjptr->qj_flink) != 0)
			spl_qjactive_start->qj_blink = 0;
	}
	else
	{
		if (spl_qjactive_end == qjptr)
		{
			tptr             = qjptr->qj_blink;
			tptr->qj_flink   = 0;
			spl_qjactive_end = tptr;
		}
		else
		{
			qjptr->qj_blink->qj_flink = qjptr->qj_flink;
			qjptr->qj_flink->qj_blink = qjptr->qj_blink;
		}
	}

	/*
	 *	Un-protect the QJOB active list
	 */
	if (wait_flag == 0)
		signal(spl_qjact_sem);

	/*
	 *	Zero out several fields in the QJOB
	 */
	qjptr->qj_jobid = 0;
	qjptr->qj_len   = 0;
	qjptr->qj_iptr  = 0;
	qjptr->qj_fillp = 0;
	qjptr->qj_flink = 0;

	/*
	 *	Protect the QJOB free list
	 */
	wait(spl_qjfree_sem);

	/*
	 *	Add the QJOB to the free list
	 */
	if (spl_qjfree_start == 0)
	{
		spl_qjfree_start   = qjptr;
		qjptr->qj_blink    = 0;
	}
	else
	{
		spl_qjfree_end->qj_flink = qjptr;
		qjptr->qj_blink          = spl_qjfree_end;
	}
	spl_qjfree_end = qjptr;

	/*
	 *	Un-protect the QJOB free list
	 */
	signal(spl_qjfree_sem);
}

/*
 *	FUNCTION spool_downcode - Flag this QJOB as download code data
 *
 *	DESCRIPTION
 *		This function flags this particular QJOB as containing download
 *		code data.  The data is ultimately destined for flash memory,
 *		but must first be spooled into memory just like a regular old
 *		print job.  All this function does is set a flag within the QJOB,
 *		so that the de-spooler can tell that this particular QJOB needs
 *		to have its data sent to flash, instead of the printer.
 *
 *	PARAMETERS
 *		qptr	-	pointer to the relevent QJOB
 *		key		-	the job id of this QJOB as given by the connect call
 *		proto	-	Value which indicates the protocol being used to get
 *					the data on-board.
 *
 *	RETURN VALUES
 *		none
 */
void
spool_downcode(qptr,key,proto)
SPL_QJOBS	*qptr;
u_long		key;
int			proto;
{
	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_downcode: jobid != key\n",0);
		return;
	}
}

/*
 *	FUNCTION spool_connect - Connect with the spooler
 *
 *	DESCRIPTION
 *		This function is called in order to obtain a connection to the
 *		spooler.  Obtaining a connection requires finding an available
 *		QJOB on the free list, deleting it from the free list, and putting
 *		it on the active list.  A pointer to the found QJOB, as well as
 *		the job id of the QJOB is returned to the caller.  The job-id which
 *		is returned to the caller is used in subsequent calls into the
 *		spooler (like spool_write).  
 *
 *	PARAMETERS
 *		key		-	Address of variable in which to store the QJOB job id
 *		proto	-	Value which represents the connecting protocol type
 *
 *	RETURN VALUES
 *		0		- Indicates error - No free Queue-job entries
 *		!= 0	- Address of QJOB for this connection
 *		*key	- Job ID for this QJOB. Only valid when (return value != 0)
 */
SPL_QJOBS	*
spool_connect(key,proto)
u_long		*key;			/* address where jobid is written to */
u_long		proto;			/* connecting protocol type */
{
	register	SPL_QJOBS	*qptr;

	wait(spl_qjfree_sem);		/* protect the QJOB free list */

	/*
	 *	Take a node off of the free list.  If there is none
	 *	available, then return 0
	 */
	if ((qptr = spl_qjfree_start) == 0)
	{
		signal(spl_qjfree_sem);
		return 0;
	}
	if ((spl_qjfree_start = qptr->qj_flink) != 0)
		spl_qjfree_start->qj_blink = 0;

	signal(spl_qjfree_sem);		/* Un-protect the QJOB free list */

	wait(spl_qjact_sem);	/* Protect the QJOB active list */

	/*
	 *	A free node has been obtained.  Now put it on the
	 *	active list.
	 */
	if (spl_qjactive_start == 0)
	{
		spl_qjactive_start = qptr;
		qptr->qj_blink     = 0;
	}
	else
	{
		spl_qjactive_end->qj_flink = qptr;
		qptr->qj_blink             = spl_qjactive_end;
	}
	spl_qjactive_end = qptr;

	signal(spl_qjact_sem);		/* Un-protect the QJOB active list */

	/*
	 *	Initialize all the fields of this qjob
	 */
	qptr->qj_jobid     = spl_sequence++;
	qptr->qj_len       = 0;
	qptr->qj_uname[0]  = 0;
	qptr->qj_fname[0]  = 0;
	qptr->qj_hname[0]  = 0;
	qptr->qj_hostip[0] = 0;
	qptr->qj_hostip[1] = 0;
	qptr->qj_hostip[2] = 0;
	qptr->qj_hostip[3] = 0;
	qptr->qj_proto     = proto;
	qptr->qj_iptr      = 0;
	qptr->qj_fillp     = 0;
	qptr->qj_flink     = 0;

	/*
	 *	limit Job ID's to 4 digits
	 */
	if (spl_sequence == 10000)
		spl_sequence = 1;

	/*
	 *	Set *key to be the job-id, and return the address of this qjob
	 */
	*key = qptr->qj_jobid;
	return(qptr);
}

/*
 *	FUNCTION spool_write - write data to the spooler
 *
 *	DESCRIPTION
 *		This function is called in order to write data into the
 *		spooler.  Data is written into dnodes, which is the key spooler
 *		data structure.  All of the dnodes for a particular QJOB are linked
 *		together.  Using this function is the only way to get data into
 *		spooler memory.
 *
 *	PARAMETERS
 *		qptr	-	Pointer to the QJOB for this connection
 *		key		-	Job ID that the caller continues to hold throughout the
 *					lifetime of the connection.  This key value needs to
 *					match the job-id stored in the QJOB in order for this
 *					write to be allowed.
 *		buf		-	Pointer to character buffer that holds the data to
 *					be spooled
 *		count	-	count of characters in "buf"
 *
 *	RETURN VALUES
 *		0		-	spool_write successfully wrote the data into the buffer
 *		1		-	Error, the data was not written into spooler memory
 */
int
spool_write(qptr,key,buf,count)
register	SPL_QJOBS	*qptr;
			u_long		key;
register	char		*buf;
register	int			count;
{
	register	u_char		*dptr;
	register	SPL_DNODE	*dnode_ptr;
	register	SPL_DNODE	*tmp_dnptr;
	register	int			bytes_remain;
	register	int			bytes_tocopy;

	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_write: jobid != key\n",0);
		return 1;
	}

	/*
	 *	If qj_iptr == 0, then there are NO data nodes connected
	 *	to the qjob yet
	 */
	if (qptr->qj_iptr == 0)
	{
		/*
		 *	Get an available dnode, link it with the qjob, set the
		 *	"filling" pointer, and set the "BEG" bit in the status word
		 */
		dnode_ptr = spool_getdnode(key);

		if (key != qptr->qj_jobid)
		{
			xp_errlog("spool_write: qjob not valid, returning dnode\n",0);
			spool_retdnode(dnode_ptr);
			return 1;
		}

		qptr->qj_iptr         = dnode_ptr;
		qptr->qj_fillp        = dnode_ptr;
		dnode_ptr->in_status |= SPL_STBEG;
	}
	else
	{
		/*
		 *	Current filling data node is always pointed
		 *	to by the qj_fillp field of the qjob
		 */
		dnode_ptr = qptr->qj_fillp;

		/*
		 *	Make sure this node is marked as "used".
		 *	If not, log the error and return
		 */
		if ((dnode_ptr->in_status & SPL_STUSED) == 0)
		{
			xp_errlog("spool_write: dnode status is NOT SPL_STUSED \n",0);
			return 1;
		}

		/*
		 *	Make sure the key values match
		 */
		if (dnode_ptr->in_jobid != key)
		{
			xp_errlog("spool_write: dnode jobid != key\n",0);
			return 1;
		}

		/*
		 *	This particular data record should be the last on the
		 *	list.  If not, log the error and return
		 */
		if (dnode_ptr->in_link != 0)
		{
			xp_errlog("spool_write: dnode pointer is NOT end-of-list\n",0);
			return 1;
		}

		/*
		 *	If the status indicates this data block is full, then
		 *	get a new dnode, link it into the rest of the list,
		 *	and set the new "filling" pointer.
		 */
		if ((dnode_ptr->in_status & SPL_STFULL) != 0)
		{
			tmp_dnptr = dnode_ptr;
			dnode_ptr = spool_getdnode(key);

			if (key != qptr->qj_jobid)
			{
				xp_errlog("spool_write: 0,qjob not valid, returning dnode\n",0);
				spool_retdnode(dnode_ptr);
				return 1;
			}

			tmp_dnptr->in_link = dnode_ptr;
			qptr->qj_fillp     = dnode_ptr;
		}
	}

	dptr = dnode_ptr->in_buf;		/* points to beginning of data buffer */

	for (;;)
	{
		/*
		 *	Calculate the number of bytes remaining in the buffer
		 */
		bytes_remain = SPL_SZDATA - dnode_ptr->in_len;

		/*
		 *	Set the number of bytes to copy into the buffer
		 */
		if (count > bytes_remain)
			bytes_tocopy = bytes_remain;
		else
			bytes_tocopy = count;

		/*
		 *	Copy the data, then:
		 *		- decrement the count (bytes that still need to be copied)
		 *		- increment the source data pointer
		 *		- increment length of data currently in the block
		 *		- increment total length of file
		 */
		blkcopy(dptr+dnode_ptr->in_len,buf,bytes_tocopy);
		count             = count - bytes_tocopy;
		buf               = buf + bytes_tocopy;
		dnode_ptr->in_len = dnode_ptr->in_len + bytes_tocopy;
		qptr->qj_len      = qptr->qj_len + bytes_tocopy;

		/*
		 *	If the data block filled up, then set STFULL
		 *	in the status field
		 */
		if (dnode_ptr->in_len == SPL_SZDATA)
			dnode_ptr->in_status |= SPL_STFULL;

		/*
		 *	if bytes remaining to be copied == 0, then all
		 *	the data has been copied, and the loop may be exited.
		 */
		if (count == 0)
			break;

		/*
		 *	Else, there are still bytes remaining to be copied.
		 *	count is non-zero, which means that the buffer is
		 *	full. Grab a new dnode, put it into the list, and
		 *	set the new "filling" pointer.
		 */
		tmp_dnptr = dnode_ptr;
		dnode_ptr = spool_getdnode(key);

		if (key != qptr->qj_jobid)
		{
			xp_errlog("spool_write: 1, qjob not valid, returning dnode\n",0);
			spool_retdnode(dnode_ptr);
			return 1;
		}

		dptr               = dnode_ptr->in_buf;
		tmp_dnptr->in_link = dnode_ptr;
		qptr->qj_fillp     = dnode_ptr;
	}

	return 0;	/* return OK */
}

/*
 *	FUNCTION spool_retdnode - return a dnode to the free list
 *
 *	DESCRIPTION
 *		This function places a given dnode back on the dnode free
 *		list.  The dnode is placed at the end of the free list queue.
 *		spl_bytesusd is updated to indicate the newly available memory.
 *
 *	PARAMETERS
 *		dnptr	-	pointer to the relevant dnode
 *
 *	RETURN VALUES
 *		none
 */
void
spool_retdnode(dnptr)
register	SPL_DNODE	*dnptr;
{
	/*
	 *	zero out some key fields of the dnode
	 */
	dnptr->in_status = 0;
	dnptr->in_len    = 0;
	dnptr->in_jobid  = 0;
	dnptr->in_link   = 0;

	wait(spl_dnode_sem);		/* protect the dnode free list */

	/*
	 *	Put the dnode back onto the free list
	 */
	if (spl_dnode_start == 0)
		spl_dnode_start = dnptr;
	else
		spl_dnode_end->in_link = dnptr;
	spl_dnode_end = dnptr;

	signal(spl_dnode_sem);		/* un-protect the dnode free list */

	/*
	 *	Update the spl_bytesusd variable.  Protect it by using
	 *	the spl_busd_sem semaphore.
	 */
	wait(spl_busd_sem);
	spl_bytesusd  = spl_bytesusd - SPL_SZDATA;
	signal(spl_busd_sem);
}

/*
 *	FUNCTION spool_disconnect - disconnect from the spooler
 *
 *	DESCRIPTION
 *		This function is called to disconnect from the spooler.  In
 *		effect what it does is to mark the end of the data within
 *		the QJOB.  This is done by setting the SPL_STEND in the
 *		status flag of the last filling dnode.
 *
 *	PARAMETERS
 *		qptr	-	pointer to the relevant QJOB
 *		key		-	job id of the QJOB as kept by the caller
 *
 *	RETURN VALUES
 *		none
 */
void
spool_disconnect(qptr,key)
SPL_QJOBS	*qptr;
u_long		key;
{
	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_disconnect: jobid != key\n",0);
		return;
	}

	/*
	 *	Set the status in the last data record to indicate
	 *	this node is the end-of-the-file.
	 */
	qptr->qj_fillp->in_status |= SPL_STEND;
}

/*
 *	FUNCTION spool_username - Store user name for this QJOB
 *
 *	DESCRIPTION
 *		This function is called in order to store the user name
 *		of this particular job into the QJOB data structure.  The
 *		QJOB user name is used by the lpq and lprm code.
 *
 *	PARAMETERS
 *		qptr	-	pointer to the relevant QJOB
 *		key		-	job id of the QJOB as kept by the caller
 *		bptr	-	pointer to the user name string
 *
 *	RETURN VALUES
 *		none
 */
void
spool_username(qptr,key,bptr)
SPL_QJOBS	*qptr;
u_long		key;
u_char		*bptr;
{
	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_username: jobid != key\n",0);
		return;
	}

	/*
	 *	Copy the user name to qj_uname.  If the user name is too
	 *	long, then truncate it.
	 */
	if (strlen(bptr) > (QJNS - 1))
	{
		strncpy(qptr->qj_uname,bptr,QJNS-1);
		qptr->qj_uname[QJNS-1] = 0;
	}
	else
		strcpy(qptr->qj_uname,bptr);
}

/*
 *	FUNCTION spool_filename - Store file name for this QJOB
 *
 *	DESCRIPTION
 *		This function is called in order to store the file name
 *		of this particular job into the QJOB data structure. The
 *		file name is used by the lpq code.
 *
 *	PARAMETERS
 *		qptr	-	pointer to the relevant QJOB
 *		key		-	job id of the QJOB as kept by the caller
 *		bptr	-	pointer to the file name string
 *
 *	RETURN VALUES
 *		none
 */
void
spool_filename(qptr,key,bptr)
SPL_QJOBS	*qptr;
u_long		key;
u_char		*bptr;
{
	register	u_char		*tptr;

	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_filename: jobid != key\n",0);
		return;
	}

	/*
	 *	Skip over any slashes in order to get to the real file name
	 */
	while ((tptr = index(bptr,'/')) != 0)
		bptr = ++tptr;

	/*
	 *	Copy the file name to qj_fname.  If the file name is too
	 *	long, then truncate it.
	 */
	if (strlen(bptr) > (QJNS - 1))
	{
		strncpy(qptr->qj_fname,bptr,QJNS-1);
		qptr->qj_fname[QJNS-1] = 0;
	}
	else
		strcpy(qptr->qj_fname,bptr);
}

/*
 *	FUNCTION spool_hostname - Store host name for this QJOB
 *
 *	DESCRIPTION
 *		This function is called in order to store the host name
 *		of this particular job into the QJOB data structure.  The
 *		host name is used by the lpq code.
 *
 *	PARAMETERS
 *		qptr	-	pointer to the relevant QJOB
 *		key		-	job id of the QJOB as kept by the caller
 *		bptr	-	pointer to the host name string
 *
 *	RETURN VALUES
 *		none
 */
void
spool_hostname(qptr,key,bptr)
SPL_QJOBS	*qptr;
u_long		key;
u_char		*bptr;
{
	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_hostname: jobid != key\n",0);
		return;
	}

	/*
	 *	Copy the host name to qj_hname.  If the host name is too
	 *	long, then truncate it.
	 */
	if (strlen(bptr) > (QJNS - 1))
	{
		strncpy(qptr->qj_hname,bptr,QJNS-1);
		qptr->qj_hname[QJNS-1] = 0;
	}
	else
		strcpy(qptr->qj_hname,bptr);
}

/*
 *	FUNCTION spool_hostip - Store IP address of sending host
 *
 *	DESCRIPTION
 *		This function is called in order to store the IP address
 *		of the sending host into the QJOB data structure.  This
 *		information is needed by the lprm code.
 *
 *	PARAMETERS
 *		qptr	-	pointer to the relevant QJOB
 *		key		-	job id of the QJOB as kept by the caller
 *		ipval	-	IP address of the sending host
 *
 *	RETURN VALUES
 *		none
 */
void
spool_hostip(qptr,key,ipval)
SPL_QJOBS	*qptr;
u_long		key;
IPADDR		ipval;
{
	/*
	 *	The jobid in the qjob should match the key sent by the calling
	 *	function.  If they don't match, then perhaps this qjob has
	 *	gone on the free list.
	 */
	if (qptr->qj_jobid != key)
	{
		xp_errlog("spool_hostip: jobid != key\n",0);
		return;
	}

	qptr->qj_hostip[0] = ipval[0];
	qptr->qj_hostip[1] = ipval[1];
	qptr->qj_hostip[2] = ipval[2];
	qptr->qj_hostip[3] = ipval[3];
}

/*
 *	FUNCTION spool_stats - Show the spool queue
 *
 *	DESCRIPTION
 *		This function is called by a shell command.  All it
 *		really does is display out the spool queue on the Xinu
 *		console.  It does this by calling spool_lpq.
 *
 *	PARAMETERS
 *		none
 *
 *	RETURN VALUES
 *		none
 */
void
spool_stats(fd)
int		fd;
{
	char		sbuf[1024];
	SPL_DNODE	*dptr;
	int			i;

	/*
	 *	Just call spool_lpq to do all the real work.
	 */
	spool_lpq(sbuf,1024,fd,LPC_SQSS);

	/*
	 *	The following code segment helps to find "floating" dnodes.
	 *	It really only helps when the queue shows up as empty, but
	 *	there is still a positive count in spl_bytesusd.
	 *	Un-comment this code to use it.
	 */
/*
	for (dptr=spl_dnode_mem, i=0; i < spl_numdnodes; ++i, ++dptr)
	{
		if (dptr->in_jobid != 0)
		{
			printf("got one: jobid   = %d\n",dptr->in_jobid);
			printf("         status  = 0x%x\n",dptr->in_status);
			printf("         len     = %d\n",dptr->in_len);
			printf("         link    = 0x%x\n",dptr->in_link);
		}
	}
*/

}

/*
 *	FUNCTION spool_lpq - Show the spool queue
 *
 *	DESCRIPTION
 *		This function is called in order to show information about
 *		the spool queue.  The output shown for the spool queue is
 *		printed in very nearly the same fashion as that in BSD Unix.
 *		Both the short form and the long form for displaying the queue
 *		are handled.
 *
 *	PARAMETERS
 *		bp		-	buffer pointer, this is where the data goes
 *		size	-	available size in bytes of bp
 *		fd		-	file descriptor to use for write
 *		cmd		-	denotes long or short
 *
 *	RETURN VALUES
 *		none
 */
void
spool_lpq(bp,size,fd,cmd)
u_char		*bp;
int			size;
int			fd;
int			cmd;
{
	SPL_QJOBS	*qptr;			/* pointer to QJOB, indexes through all jobs */
	SPL_DNODE	*dnptr;			/* pointer to current filling dnode of job */
	int			rank;			/* holds rank within queue for the jobs */
	char		*x;				/* pointer to "", or "-F" */
	char		obuf[14];		/* storage for user name, and host name */
	char		fbuf[40];		/* storage for the file name of job */
	char		*tptr;			/* temp character pointer */
	char		*tfptr;			/* another temp character pointer */
	int			tmp;			/* temp variable */
	int			tmp1;			/* another temp variable */

#define		BPX		bp+strlen(bp)

	rank = 0;
	qptr = spl_qjactive_start;
	*bp  = 0;

	sprintf(bp,"\nMemory available to spooler = %d\n",spl_tbytesav);
	sprintf(BPX,"Total bytes used            = %d\n",spl_bytesusd);
	sprintf(BPX,
		"Total bytes available       = %d\n\n",spl_tbytesav-spl_bytesusd);

	sprintf(BPX,"Spool Queue:\n");

	if (qptr == 0)
	{
		sprintf(BPX,"    Empty\n\n");
		fprintf(fd,"%s", bp);
		return;
	}
	else
		sprintf(BPX,
	" Rank    Owner        Job    Files                       Total Size\n");

	if (cmd == LPC_SQSL)
		sprintf(BPX,"\n");

	while (qptr != 0)
	{
		tmp1 = (cmd == LPC_SQSL) ? 120 : 75;

		if (((tmp = strlen(bp)) + tmp1) >= size)
		{
			fprintf(fd,"%s",bp);
			*bp = 0;
		}

		dnptr = qptr->qj_fillp;

		if (dnptr->in_status & SPL_STEND)
			x = "";
		else
			x = "-F";

		if (strlen(qptr->qj_uname) > 11)
		{
			strncpy(obuf,qptr->qj_uname,11);
			obuf[12] = 0;
		}
		else
			strcpy(obuf,qptr->qj_uname);

		if (strlen(qptr->qj_fname) > 23)
		{
			tptr  = qptr->qj_fname;
			while ((tfptr = index(tptr,'/')) != 0)
					tptr = ++tfptr;

			if (strlen(tptr) > 23)
			{
				strncpy(fbuf,tptr,23);
				fbuf[24] = 0;
			}
			else
				strcpy(fbuf,tptr);
		}
		else
			strcpy(fbuf,qptr->qj_fname);

		sprintf(BPX,"%-3d%-2s    %-11s  %-4d   %-25s   %-7d bytes\n",
				rank,x,obuf,qptr->qj_jobid,fbuf,qptr->qj_len);

		if (cmd == LPC_SQSL)
		{
			if (strlen(qptr->qj_hname) > 15)
			{
				strncpy(obuf,qptr->qj_hname,15);
				obuf[16] = 0;
			}
			else
				strcpy(obuf,qptr->qj_hname);

			switch (qptr->qj_proto)
			{
				case SPL_LPD:		tptr = "lpd";      break;
				case SPL_TFTPD_N:	tptr = "tftpd-n";  break;
				case SPL_TFTPD_O:	tptr = "tftpd-o";  break;
				case SPL_TESTPG:	tptr = "testpg";   break;
				default:            tptr = "";
			}

			sprintf(BPX,"   host=%-15s   protocol=%-8s\n\n",obuf,tptr);
		}

		rank++;
		qptr = qptr->qj_flink;
	}

	fprintf(fd,"%s",bp);

	return;
}

/*
 *	FUNCTION spool_lprm - Remove a job from the queue
 *
 *	DESCRIPTION
 *		This function is called in order to remove 1 or more jobs
 *		from the spool queue.  This function is called by the lp_dd
 *		process in response to receiving an 05 (Remove Job) command
 *		from the lpd client.  This function parses the data found
 *		in the 05 message (see RFC1179 section 5.5), and calls one of
 *		several functions to perform the actual removal of the QJOB.
 *
 *	PARAMETERS
 *		bp		-	buffer pointer, this is where the data is
 *		size	-	available size in bytes of bp
 *		fd		-	file descriptor of network (TCP) device
 *
 *	RETURN VALUES
 *		none
 */
void
spool_lprm(bp,size,fd,localip)
u_char		*bp;
int			size;
int			fd;
IPADDR		localip;
{
	u_char			*ptr_n;			/* Points right at the \n */
	u_char			*uname;			/* user name */
	u_char			*tptr;			/* temp pointer */
	u_char			*jptr;			/* job pointer */

	/*
	 *	loop through the data buffer, looking for \n.  When it's found
	 *	ptr_n points right at the \n.
	 */
	for (ptr_n = bp; *ptr_n != 10; ptr_n++)
		;

	/*
	 *	loop through the data buffer, looking for a SPACE.  The 1st character
	 *	after the SP is the start of the user name.
	 */
	for (uname = bp; *uname != 32; uname++)
		;
	uname++;		/* uname now points at 1st character of username */

	/*
	 *	Continue looping, looking for the end of the user name.  When the
	 *	end of the user name is found, terminate it with a 0.
	 */
	for (tptr = uname; (*tptr != 32) && (*tptr != 10); ++tptr)
		;
	*tptr = 0;		/* terminate the string */


	if (tptr == ptr_n)
	{
		if (strcmp(uname,"-all") == 0)
			spool_lprm_case2(uname,localip,0,1);
		else
			spool_lprm_case1(uname,localip);
	}
	else
	{
		do
		{
			/*
			 *	Find the end of the current word and terminate it with
			 *	a 0.  jptr points at the 1st character of the current word.
			 */
			for (jptr=++tptr; (*tptr != 32) && (*tptr != 10); ++tptr)
				;
			*tptr = 0;			/* terminate the string */

			if (isdigit(*jptr) != 0)
				spool_lprm_case3(uname,localip,jptr);
			else
				spool_lprm_case2(uname,localip,jptr,0);

		} while (tptr != ptr_n);
	}
}

/*
 *	FUNCTION spool_getdnode - Get an available dnode
 *
 *	DESCRIPTION
 *		This function is called in order to obtain a data node
 *		from the free list.  If all of the dnodes are currently used
 *		then the calling processes is put to sleep for 1/2 second, then
 *		it tries again.  There is also a provision in the code which
 *		checks to see if the requested dnode is the last one available
 *		on the free list.  If so, and the currently de-spooling QJOB is
 *		still filling, then the dnode is reserved for it.  An available
 *		dnode is eventually found, and deleted from the start of the
 *		free list.  spl_bytesusd is bumped up to show the increase in
 *		used spooler memory.
 *
 *	PARAMETERS
 *		key		-	job id of the QJOB as kept by the caller
 *
 *	RETURN VALUES
 *		!= 0	-	pointer to the valid dnode
 */
static SPL_DNODE *
spool_getdnode(key)
u_long	key;
{
	register	SPL_DNODE	*dnode_ptr;
	register	SPL_DNODE	*dnptr;

dnode_top:

	/*
	 *	Get a dnode from the free list, and initialize it
	 */
	while (1)
	{
		wait(spl_dnode_sem);

		if ((dnode_ptr = spl_dnode_start) != 0)
			break;

		signal(spl_dnode_sem);

		sleep10(5);
	}

	/*
	 *	If in_link == 0, then this particular dnode is the
	 *	only one on the free list.  When this is the case,
	 *	this last dnode needs to be reserved for the currently
	 *	de-spooling process.  It only needs to be reserved
	 *	if the active qjob is currently filling.
	 */
	if (dnode_ptr->in_link == 0)
	{
		wait(spl_qjact_sem);

		dnptr = spl_qjactive_start->qj_fillp;

		/*
		 *	If the SPL_STEND bit is NOT set, then the active QJOB is
		 *	currently filling.  If it IS set, then the active QJOB is
		 *	already (completely) on-board and we don't care who gets
		 *	the available dnode.
		 */
		if ((dnptr->in_status & SPL_STEND) == 0)
		{
			if (spl_qjactive_start->qj_jobid != key)
			{
				signal(spl_qjact_sem);
				signal(spl_dnode_sem);
				sleep(2);
				goto dnode_top;
			}
		}
		signal(spl_qjact_sem);
	}

	spl_dnode_start = dnode_ptr->in_link;

	signal(spl_dnode_sem);

	/*
	 *	Initialize all the fields in the dnode
	 */
	dnode_ptr->in_status = SPL_STUSED;
	dnode_ptr->in_len    = 0;
	dnode_ptr->in_jobid  = key;
	dnode_ptr->in_link   = 0;

	/*
	 *	bump up the "bytes used" variable
	 */
	wait(spl_busd_sem);
	spl_bytesusd = spl_bytesusd + SPL_SZDATA;
	signal(spl_busd_sem);

	return(dnode_ptr);
}

/*
 *	FUNCTION spool_lprm_case1 - Remove a QJOB, case 1
 *
 *	DESCRIPTION
 *		case1 only deletes the currently active job.  It executes due to
 *		an lprm command like the following:
 *
 *			lprm -Pxinu2p
 *
 *		Note there are no parameters (to lprm) other than the printer
 *		name.  For case1 to delete the currently active job, 2 conditions
 *		must be met:
 *
 *		1.  IP address of active QJOB must match IP address of lprm request
 *		2.	Username in QJOB must match username in lprm request.  This
 *			condition is not checked if the username in the lprm request
 *			is "root".
 *
 *	PARAMETERS
 *		uname	-	pointer to user name found in the 05-Remove Job message
 *		localip	-	IP address of host sending the lprm request
 *
 *	RETURN VALUES
 *		none
 */
static void
spool_lprm_case1(uname,localip)
u_char	*uname;
IPADDR	localip;
{
	SPL_QJOBS		*qjptr;
	SPL_DNODE		*dnptr;
	SPL_DNODE		*dtmp;

	wait(spl_qjact_sem);

	/*
	 *	If spl_qjactive_start == 0, then there is nothing on the QJOB active
	 *	list, and thus there is nothing to do.
	 */
	if ((qjptr = spl_qjactive_start) == 0)
		goto case1_ret;

	/*
	 *	Check to see if the IP addresses match.  If they do, then
	 *	continue on.  If not, then no action is performed, and case1
	 *	returns
	 */
	if (blkequ(localip,qjptr->qj_hostip,4) == TRUE)
	{
		/*
		 *	If the uname is not root, AND the uname does not match
		 *	the one in the QJOB, then disallow any action and return
		 */
		if (strcmp(uname,"root") && strcmp(uname,qjptr->qj_uname))
			goto case1_ret;

		/*
		 *	Set the SPL_STEND bit within the dnode.  This will cause
		 *	the de-spooler to quit processing this job, and return it
		 *	to the free list.
		 */
		dnptr             = qjptr->qj_iptr;
		dnptr->in_status |= SPL_STEND;

		/*
		 *	Go through the rest of the dnodes for this job, and
		 *	return them to the free list
		 */
		dnptr = dnptr->in_link;
		while (dnptr)
		{
			dtmp  = dnptr->in_link;
			spool_retdnode(dnptr);
			dnptr = dtmp;
		}
	}

case1_ret:
	signal(spl_qjact_sem);
	return;
}

/*
 *	FUNCTION spool_lprm_case2 - Remove a QJOB, case 2
 *
 *	DESCRIPTION
 *		case2 deletes all jobs based on the given username.  It executes
 *		due to an lprm command like the following:
 *
 *				lprm -Pxinu2p johnr
 *
 *		For case2 to delete the given job, 3 conditions must be met:
 *
 *		1.  A matching QJOB with the given username must be found
 *		2.	IP address of the QJOB must match IP address of lprm request
 *		3.	Username in QJOB must match username in lprm request.  This
 *			condition is not checked if the username in the lprm request
 *			is "root".
 *
 *	PARAMETERS
 *		uname	 -	pointer to user name found in the 05-Remove Job message
 *		localip	 -	IP address of host sending the lprm request
 *		wptr	 -	pointer to the user name given on the lprm command line
 *		all_flag -	A boolean flag value:
 *						1 - do NOT attempt to match user names.  This indicates
 *							that "root" is deleting all jobs from a particular
 *							host.
 *						0 - Matching user names must be found.
 *
 *	RETURN VALUES
 *		none
 */
void
spool_lprm_case2(uname,localip,wptr,all_flag)
u_char	*uname;
IPADDR	localip;
u_char	*wptr;
int		all_flag;
{
	SPL_QJOBS		*qjptr;
	SPL_QJOBS		*qj_nextptr;
	SPL_DNODE		*dnptr;
	SPL_DNODE		*dtmp;

	wait(spl_qjact_sem);

	/*
	 *	If spl_qjactive_start == 0, then there is nothing on the QJOB active
	 *	list, and thus there is nothing to do.
	 */
	if ((qjptr = spl_qjactive_start) == 0)
		goto case2_ret;

case2_again:

	if (all_flag == 0)
	{
		/*
		 *	Search through the qjobs, looking for a matching user name
		 */
		while (qjptr)
		{
			if (strcmp(qjptr->qj_uname,wptr) == 0)
				break;
			qjptr = qjptr->qj_flink;
		}
	}

	/*
	 *	qjptr == 0, when the list has been exhausted and a match
	 *	has not been found.
	 */
	if (qjptr == 0)
		goto case2_ret;

	qj_nextptr = qjptr->qj_flink;

	/*
	 *	Check to see if the IP addresses match.  If they do, then
	 *	continue on.  If not, then no action is performed, and the
	 *	next qjob is queried.
	 */
	if (blkequ(localip,qjptr->qj_hostip,4) == TRUE)
	{
		if (all_flag == 0)
		{
			/*
			 *	If the uname is not root, AND the uname does not match
			 *	the one in the QJOB, then disallow any action and return
			 */
			if (strcmp(uname,"root") && strcmp(uname,qjptr->qj_uname))
			{
				qjptr = qj_nextptr;
				goto case2_again;
			}
		}

		/*
		 *	Check to see if the found QJOB is currently de-spooling.
		 *	If it is, then set STEND in the current dnode, and delete
		 *	the rest of the dnode.  Deletion of the QJOB is left up
		 *	to the de-spooler.
		 */
		dnptr = qjptr->qj_iptr;
		if (qjptr == spl_qjactive_start)
		{
			/*
			 *	Set the SPL_STEND bit within the dnode.  This will cause
			 *	the de-spooler to quit processing this job, and return it
			 *	to the free list.
			 */
			dnptr->in_status |= SPL_STEND;
			dnptr             = dnptr->in_link;
		}
		else
			spool_retqjob(qjptr,1);		/* no wait */

		/*
		 *	Go through the rest of the dnodes for this job, and
		 *	return them to the free list
		 */
		while (dnptr)
		{
			dtmp  = dnptr->in_link;
			spool_retdnode(dnptr);
			dnptr = dtmp;
		}
	}

	qjptr = qj_nextptr;
	goto case2_again;

case2_ret:
	signal(spl_qjact_sem);
	return;

}

/*
 *	FUNCTION spool_lprm_case3 - Remove a QJOB, case 3
 *
 *	DESCRIPTION
 *		case3 deletes a job based on the given job-id.  It executes
 *		due to an lprm command like the following:
 *
 *				lprm -Pxinu2p 201
 *
 *		For case3 to delete the given job, 3 conditions must be met:
 *
 *		1.  A matching QJOB with the given jobid must be found
 *		2.	IP address of the QJOB must match IP address of lprm request
 *		3.	Username in QJOB must match username in lprm request.  This
 *			condition is not checked if the username in the lprm request
 *			is "root".
 *
 *	PARAMETERS
 *		uname	 -	pointer to user name found in the 05-Remove Job message
 *		localip	 -	IP address of host sending the lprm request
 *		wptr	 -	pointer to the job-id given on the lprm command line
 *
 *	RETURN VALUES
 *		none
 */
void
spool_lprm_case3(uname,localip,wptr)
u_char	*uname;
IPADDR	localip;
u_char	*wptr;
{
	SPL_QJOBS		*qjptr;
	SPL_DNODE		*dnptr;
	SPL_DNODE		*dtmp;
	u_long			jobid;

	wait(spl_qjact_sem);

	/*
	 *	If spl_qjactive_start == 0, then there is nothing on the QJOB active
	 *	list, and thus there is nothing to do.
	 */
	if ((qjptr = spl_qjactive_start) == 0)
		goto case3_ret;

	jobid = atoi(wptr);			/* convert to long */

	/*
	 *	Search through the qjobs, looking for a matching ID
	 */
	while (qjptr)
	{
		if (qjptr->qj_jobid == jobid)
			break;
		qjptr = qjptr->qj_flink;
	}

	/*
	 *	qjptr == 0, when the list has been exhausted and a match
	 *	has not been found.
	 */
	if (qjptr == 0)
		goto case3_ret;

	xp_tracev(80,"case3: qjptr->qj_jobid = %d\n",qjptr->qj_jobid);

	/*
	 *	Check to see if the IP addresses match.  If they do, then
	 *	continue on.  If not, then no action is performed, and case3
	 *	returns
	 */
	if (blkequ(localip,qjptr->qj_hostip,4) == TRUE)
	{
		/*
		 *	If the uname is not root, AND the uname does not match
		 *	the one in the QJOB, then disallow any action and return
		 */
		if (strcmp(uname,"root") && strcmp(uname,qjptr->qj_uname))
			goto case3_ret;

		/*
		 *	Check to see if the found QJOB is currently de-spooling.
		 *	If it is, then set STEND in the current dnode, and delete
		 *	the rest of the dnode.  Deletion of the QJOB is left up
		 *	to the de-spooler.
		 */
		dnptr = qjptr->qj_iptr;
		if (qjptr == spl_qjactive_start)
		{
			/*
			 *	Set the SPL_STEND bit within the dnode.  This will cause
			 *	the de-spooler to quit processing this job, and return it
			 *	to the free list.
			 */
			dnptr->in_status |= SPL_STEND;
			dnptr             = dnptr->in_link;
		}
		else
			spool_retqjob(qjptr,1);		/* no wait */

		/*
		 *	Go through the rest of the dnodes for this job, and
		 *	return them to the free list
		 */
		while (dnptr)
		{
			dtmp  = dnptr->in_link;
			spool_retdnode(dnptr);
			dnptr = dtmp;
		}
	}

case3_ret:
	signal(spl_qjact_sem);
	return;
}



/*
 * $Log: spooler.c,v $
 * Revision 1.4  1993/01/30  02:57:38  johnr
 * JR:  In spool_lpq(), I added "case SPL_TESTPG" to the piece of code which
 *      is filling in data concerning the protocol type.
 *
 * Revision 1.3  1992/12/22  19:00:10  johnr
 * JR:  Took out network.h from the includes.  Changed all instances of
 *      IPaddr to IPADDR.  Modified spool_lprm(), so that it takes an
 *      additional parameter, that of the IP address.  The code in spool_lprm()
 *      that used ptcb to assign localip was deleted because of the addition
 *      of the IP address parameter.
 *
 * Revision 1.2  1992/12/02  17:37:05  johnr
 * JR:  Modified spool_stats() so that it receives a parameter.  The parm
 *      is a file-descriptor, and is passed on to spool_lpq(), which uses
 *      it (the file-descriptor) for writing data.  Also, changed the write
 *      statements to fprintf statements.
 *
 * Revision 1.1  1992/11/25  22:54:31  johnr
 * JR:  1st entry for these files, in the new directory layout.
 *
 *
 */
