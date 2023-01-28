#ifndef lint
static char rcsid[] = "$Id: tftpd.c,v 1.1 1992/11/25 22:54:34 johnr Exp $";
#endif

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <systypes.h>
#include <spooler.h>

#undef SEGSIZE
#include "tftp.h"


          PROCESS        tftp_d();
static    int            tftp_wait_for_wrq();
static    int            tftp_write_behind();
static    int            tftp_synchnet();
static    int            tftp_writeit();
extern    u_long         spool_connect();


#define TFTP_SZBUF       SEGSIZE+XGHLEN
#define TFTP_BF_ALLOC   -3
#define TFTP_BF_FREE    -2
#define TFTP_SZACKBUF    4
#define TFTP_FSIZE       80

#define    TFTP_MAGIC_FNAME    "/tmp/JRjrJR"


typedef struct
{
    int     counter;
    char    buf[TFTP_SZBUF];
} TFTP_BF;


static    char           tftp_wrqbuf[TFTP_SZBUF];
static    char           tftp_ackbuf[TFTP_SZBUF];
static    TFTP_BF        tftp_bfs[2];
static    int            tftp_current;
static    int            tftp_nextone;
static    int            tftp_block;
static    int            tftp_crflag;


PROCESS
tftp_d()
{
    struct xgram      *wrq_xgptr;        /* xinugram pointer for wrq buffer */
    struct xgram      *ack_xgptr;        /* xinugram pointer for ack buffer */
    struct tftphdr    *dp;               /* pointer to tftp data */
    struct tftphdr    *ap;               /* pointer to data portion of ack mesg */
    int                dev;              /* device index returned by open */
    int                len;
    int                size;
    int                mode;
    int                num_timeouts;
    int                num_loop_reads;
    u_long             spool_dev;
    char               chbuf[1];
    u_long             splkey;
    u_long             proto;
    u_char             fname[TFTP_FSIZE];

    chbuf[0] = 13;

    if ((dev=open(UDP, ANYFPORT, UP_TFTP)) == SYSERR)
        panic("tftp_d: cannot open udp (UTFTP) port");

    /*
     *    The following assignments are all permanent.  Their values do
     *    not change within this function.
     */
    wrq_xgptr     = (struct xgram *)tftp_wrqbuf;
    ack_xgptr     = (struct xgram *)tftp_ackbuf;
    ap            = (struct tftphdr *)ack_xgptr->xg_data;
    ap->th_opcode = hs2net((u_short)TFTP_ACK);

    while (1)
    {

tftpd_loop:

        mode = tftp_wait_for_wrq(dev,fname);

        proto = (mode == 0) ? SPL_TFTPD_O : SPL_TFTPD_N;

        /*
         *    Keep trying until spool_connect gives us a valid return
         */
        if ((spool_dev = spool_connect(&splkey,proto)) == 0)
        {
            sleep10(5);
            goto tftpd_loop;
        }

        /*
         *    set the filename within the QJOB
         */
        spool_filename(spool_dev,splkey,fname);

        /*
         *    If the filename matches up with the tftp magic-filename,
         *    then flag the spooler
         */
        if (strcmp(fname,TFTP_MAGIC_FNAME) == 0)
            spool_downcode(spool_dev,splkey,proto);

        /*
         *    The following assignment statements are from rw_init(0)
         */
        tftp_crflag         = 0;
        tftp_block          = 0;
        tftp_current        = 0;
        tftp_nextone        = 0;                /* ahead or behind? */
        tftp_bfs[0].counter = TFTP_BF_ALLOC;    /* pass out the 1st buffer */
        tftp_bfs[1].counter = TFTP_BF_FREE;
        dp                  = (struct tftphdr *)tftp_bfs[0].buf;

        /*
         *    The following assignment statements initialize the "acknowledge"
         *    xinugram.  tftp_ackbuf is the entire xinugram.  ap points to
         *    the data section within the xinugram, which is a tftphdr.
         */
        ack_xgptr->xg_fip[0] = wrq_xgptr->xg_fip[0];
        ack_xgptr->xg_fip[1] = wrq_xgptr->xg_fip[1];
        ack_xgptr->xg_fip[2] = wrq_xgptr->xg_fip[2];
        ack_xgptr->xg_fip[3] = wrq_xgptr->xg_fip[3];
        ack_xgptr->xg_fport  = wrq_xgptr->xg_fport;
        ack_xgptr->xg_lport  = wrq_xgptr->xg_lport;

        /*
         *    Set the hostname within the QJOB
         */
        fname[0] = 0;
        ip2name(wrq_xgptr->xg_fip,fname);
        spool_hostname(spool_dev,splkey,fname);
        spool_hostip(spool_dev,splkey,ack_xgptr->xg_fip);

        do {

            /*
             *    Initialize the th_block field of the ACK message,
             *    bump up the block count, and initialize the
             *    timeout count.
             */
            ap->th_block   = hs2net((u_short)tftp_block);
            num_timeouts   = 0;
            num_loop_reads = 0;
            tftp_block++;

send_ack:

            /*
             *    Put the interface into "normal" mode, and send the
             *    ack buffer.
             */
            control(dev, DG_SETMODE, DG_NMODE);
            if (write(dev, tftp_ackbuf, TFTP_SZACKBUF) == SYSERR)
            {
                spool_disconnect(spool_dev,splkey);
                xp_errlog("tftp_d: write ACK returned SYSERR\n",0);
                goto tftpd_loop;
            }

            if (tftp_write_behind(spool_dev,splkey,mode) == 1)
                goto tftpd_loop;

            /*
             *    The ACK has been sent.  A UDP "DATA" message should be
             *    coming in shortly.  Set up the interface in "data only"
             *    mode, and also for timeouts.
             */
            control(dev,DG_SETMODE,DG_DMODE|DG_TMODE);
r1:
            if ((len = read(dev,dp,TFTP_SZBUF)) < 0)
            {
                if (len == TIMEOUT)
                {
                    xp_tracev(80,"tftp_d: read returned TIMEOUT\n",0);
                    if (++num_timeouts <= 5)
                        goto send_ack;
                    else
                    {
                        spool_disconnect(spool_dev,splkey);
                        xp_errlog("tftp_d: read has timed out\n",0);
                        goto tftpd_loop;
                    }
                }
                else
                {
                    spool_disconnect(spool_dev,splkey);
                    xp_errlog("tftp_d: read returned SYSERR\n",0);
                    goto tftpd_loop;
                }
            }

            dp->th_opcode = net2hs((u_short)dp->th_opcode);
            dp->th_block  = net2hs((u_short)dp->th_block);

            if (dp->th_opcode == TFTP_ERROR)
            {
                xp_errlog("tftp_d: th_opcode == ERROR\n",0);
                goto tftpd_loop;
            }

            /*
             *    The only type of packets we're willing to accept at this
             *    point are "DATA" packets.  If this is not a DATA packet,
             *    then (effectively drop this one) go read another packet.
             */
            if (dp->th_opcode == TFTP_DATA)
            {
                if (dp->th_block != tftp_block)
                {
                    /*
                     *    Re-synchronize with the other side
                     */
                    (void)tftp_synchnet(dev);

                    if (dp->th_block == (tftp_block-1))
                        goto send_ack;                /* rexmit */
                }
            }
            else
            {
                /*
                 *    num_loop_reads is used here to prevent what could
                 *    become an endless loop.  If the packet isn't
                 *    a data packet, then just read again.  Well, what if
                 *    the data packet never shows up (sending machine crashed),
                 *    and all that is received now are wrq packets.
                 *    if num_loop_reads gets greater than 40, then 40 packets
                 *    have been read in a row that are NOT data packets.
                 *    When this is the case, just start everything over.
                 *
                 *    Enhancement:  This should probably be some sort of a 
                 *    "timed" loop.
                 */
                if (num_loop_reads++ > 40)
                {
                    spool_disconnect(spool_dev,splkey);
                    xp_errlog("tftp_d: Too many reads and no Data Message\n",0);
                    goto tftpd_loop;
                }
                else
                    goto r1;
            }

            if ((size = tftp_writeit(spool_dev,splkey,&dp,len-4,mode)) < 0)
                goto tftpd_loop;

        } while (size == SEGSIZE);

        if (tftp_write_behind(spool_dev,splkey,mode) == 1)
            goto tftpd_loop;

        /*
         *    All the data packets have arrived, and all the data has been
         *    sent to the output device.  There may be a CR character that
         *    did not yet get sent.  If so, send it now.  Ignore errors
         *    since the disconnect will be done immediately following
         *    this statement.
         */
        if (tftp_crflag == 1)
            spool_write(spool_dev,splkey,chbuf,1);

        /*
         *    Disconnect from the spool device
         */
        spool_disconnect(spool_dev,splkey);
        xp_tracev(80,"tftp_d: done with spool_disconnect\n",0);

        /*
         *    Send the Final ack.  Assign the block field in the ACK header
         */
        ap->th_block = hs2net((u_short)(tftp_block));

        /*
         *    Put the interface into "normal" mode, and send the "final"
         *    ack buffer.
         */
        control(dev, DG_SETMODE, DG_NMODE);
        if (write(dev, tftp_ackbuf, TFTP_SZACKBUF) == SYSERR)
        {
            xp_errlog("tftp_d: ack-1, write ACK returned SYSERR\n",0);
            goto tftpd_loop;
        }

        /*
         *    Now put the interface into "Timed" mode, and attempt to read
         *    data.  Normally, this read will just time out, and the
         *    top of the loop is re-executed.
         */
        control(dev,DG_SETMODE,DG_DMODE|DG_TMODE);
        if ((len = read(dev,dp,TFTP_SZBUF)) < 0)
        {
            if (len != TIMEOUT)
                xp_errlog("tftp_d: last read returned SYSERR\n",0);
            else
                xp_tracev(80,"tftp_d: last read timed out\n",0);

            goto tftpd_loop;
        }

        xp_tracev(80,"tftp_d: last read has returned\n",0);

        /*
         *    Well, something was read.  If this is a data block, then the
         *    last ack could have been lost, in which case another ack
         *    is sent.  If its not data, then just ignore it.
         */
        if (len>=4 && dp->th_opcode == TFTP_DATA && tftp_block == dp->th_block)
        {
            control(dev, DG_SETMODE, DG_NMODE);
            write(dev, tftp_ackbuf, TFTP_SZACKBUF);            /* ignore errors */
        }

    }        /* while */
}


/*
 *    Returns:
 *        0 - octet
 *        1 - netascii
 */
static int
tftp_wait_for_wrq(dev,fptr)
int        dev;
u_char    *fptr;        /* filename pointer */
{
    int                len;
    struct xgram      *xgptr;
    struct tftphdr    *tfptr;
    u_char            *dptr;
    u_char            *tptr;
    int                ret;

    ret   = 0;
    xgptr = (struct xgram *)tftp_wrqbuf;
    tfptr = (struct tftphdr *)xgptr->xg_data;
    dptr  = tfptr->th_stuff;

    /*
     *    Clear all Q'ed packets, then set up the interface
     *    in normal mode.
     */
    control(dev, DG_CLEAR);
    control(dev, DG_SETMODE, DG_NMODE);

    /*
     *    Just keep reading until a WRQ packet arrives.  If SYSERR is
     *    returned from the read, then clear out all the packets, and
     *    read again.
     */
    do
    {
        xp_tracev(80,"tftp_wait_for_wrq: before read\n",0);
        if ((len = read(dev,tftp_wrqbuf,TFTP_SZBUF)) == SYSERR)
        {
            xp_errlog("tftp_wait_for_wrq: read returned SYSERR\n",0);
            control(dev, DG_CLEAR);
            tfptr->th_opcode = TFTP_ERROR;    /* make sure we don't exit loop */
        }
        xp_tracev(80,"tftp_wait_for_wrq: read has returned\n",0);
    } while (tfptr->th_opcode != TFTP_WRQ);

    /*
     *    Store the filename within the message into the given storage area.
     */
    if (strlen(dptr) < TFTP_FSIZE)
        strcpy(fptr,dptr);
    else
    {
        for (tptr=dptr, len=0; len < (TFTP_FSIZE - 1); ++len)
            *tptr++ = *dptr++;
    }

    /*
     *    index over the filename
     */
    while (*dptr != 0)
        ++dptr;
    ++dptr;

    /*
     *    Determine whether the mode is "netascii", or "octet".
     */
    if (strcmp(dptr,"netascii") == 0)
        ret = 1;

    return(ret);
}



/*
 * Output a buffer to a device, converting from netascii if requested.
 * CR,NUL -> CR  and CR,LF => LF.
 * Note spec is undefined if we get CR as last byte of file or a
 * CR followed by anything else.  In this case we leave it alone.
 */
static int
tftp_write_behind(dev, key, convert)
u_long    dev;
u_long    key;
int       convert;
{
                char              *buf;
                int                count;
                TFTP_BF           *b;
                struct tftphdr    *dp;
    register    char              *p;
    register    int                ct;
    register    char               c;
                char               ch[1];


    b = &tftp_bfs[tftp_nextone];

    if (b->counter < -1)                        /* anything to flush? */
        return 0;                                /* just nop if nothing to do */

    count        = b->counter;                    /* remember byte count */
    b->counter   = TFTP_BF_FREE;                /* reset flag */
    dp           = (struct tftphdr *)b->buf;
    tftp_nextone = !tftp_nextone;                /* incr for next time */
    buf          = dp->th_data;

    if (count <= 0)                                /* nak logic? */
        return 0;

    if (convert == 0)
    {
        if (spool_write(dev, key, buf, count) == 1)
            return 1;
        return 0;
    }

    p  = buf;
    ct = count;

    if (tftp_crflag == 1)
    {
        tftp_crflag = 0;        /* reset the flag */
        ch[0] = 13;                /* last character in the buffer was CR */
        if (*p == 0)
        {
            p++; ct--;            /* just skip the character if its 0 */
        }
        else if (*p == 10)
        {
            p++; ct--; ch[0] = 10;    /* skip character, set output char to LF */
        }
        if (spool_write(dev,key,ch,1) == 1)
            return 1;
    }

    while (ct--)
    {
        c = *p++;

        if (c == 13)
        {
            /*
             *    If ct == 0, then the CR was the last character
             *    in the packet.  This is noted by setting crflag = 1.
             *
             */
            if (ct == 0)
                tftp_crflag = 1;
            else
            {
                if (*p == 0)
                {
                    ct--; p++;
                }
                else if (*p == 10)
                {
                    ct--; p++; c = 10;
                }
                ch[0] = c;
                if (spool_write(dev,key,ch,1) == 1)
                    return 1;
            }
        }
        else
        {
            ch[0] = c;
            if (spool_write(dev,key,ch,1) == 1)
                return 1;
        }
    }
}


/*
 *    Update count associated with the buffer, get new buffer
 *    from the queue.  Calls tftp_write_behind only if next buffer not
 *    available.
 */
static int
tftp_writeit(dev, key, dpp, ct, convert)
u_long            dev;
u_long            key;
struct tftphdr    **dpp;
int                convert;
{
    tftp_bfs[tftp_current].counter = ct;        /* set size of data to write */
    tftp_current = !tftp_current;                /* switch to other buffer */

    if (tftp_bfs[tftp_current].counter != TFTP_BF_FREE)        /* if not free */
    {
        if (tftp_write_behind(dev, key, convert) == 1)            /* flush it */
            return(-1);
    }

    tftp_bfs[tftp_current].counter = TFTP_BF_ALLOC;        /* mark as alloc'd */
    *dpp =  (struct tftphdr *)tftp_bfs[tftp_current].buf;

    return ct;                                    /* this is a lie of course */
}


static int
tftp_synchnet(dev)
int        dev;
{
    control(dev, DG_CLEAR);
}


/*
 *    $Log: tftpd.c,v $
 * Revision 1.1  1992/11/25  22:54:34  johnr
 * JR:  1st entry for these files, in the new directory layout.
 *
 *
 */
