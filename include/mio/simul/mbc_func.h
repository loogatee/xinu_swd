/*
   $Header$
   $Log$
*/

/***********************************************************************

   Title:   MBC_FUNC.H

   Purpose: This file protypes the functions used to communicate back
            and forth between the printer and the MIO board.  It also
            contains definitions & constants common to the functions.

   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "pdtypes.h"

#define  IDLE_MODE         0
#define  OPERATE_MODE      1

#define  MIN_PRGP_SIZE     36
#define  MIN_IOGP_SIZE     36
#define  PERCENT_READ      90
#define  SIZE_READ_PACK    1536                               /* 1.5K */
#define  SIZE_WRITE_PACK   48

#define  START_IOGP_OFFSET 0x08          /* 1st packet address offset */
#define  SHARE_MEM_OFFSET  0x200000         /* Start of shared memory */
#define  MAX_SHARE_MEM     0x8000             /* 32K of shared memory */

#define  MB_TEST_ERROR     0x01                          /* MIO error */
#define  PRINTER_HUNG      0x02                          /*  "    "   */

enum  RequestMessageTypes
{
   DEST_SELFTEST,
   SELFTEST,
   SELFTEST_DESC,
   READBLOCK,
   WRITEBLOCK,
   SET_PERIPHERAL_DISPLAY,
   IO_DESC,
   ASYNC_PER_STATUS,
   INQUIRE_IO_STATUS,
   ASCII_IO_CONF,
   ASCII_STATUS,
   PROTOCOL_ERROR,
   BUFF_CONFIG,
   SAVE_IO_CONFIG,
   RETRIEVE_IO_CONFIG,
   IDENTIFY,
   ASYNC_IO_STATUS,
   UNKNOWN_CMD
};

enum  ReturnTypes
{
   NEW_PACKET,
   UNDERSTOOD,
   NOT_UNDERSTOOD,
   ERROR_ENCOUNTERED,
   ABNORMAL_BREAK,
   OVERRUN,
   DATA_ERROR,
   NORMAL_BREAK,
   LOGICAL_BREAK,
   LOGICAL_CONN,
   NOT_SENT_HOST,
   PARTIAL_MESS,
   END_JOB
};

               /* The following defines all the routines found in
                  the file MBC_FUNC.C */

/***********************************************************************

   ReadMail:   This function read the mail, clears the new mail flag
               and performs the necessary actions required to satify
               the printer.

   input:   None

   output:  rtn_parm - Depending on the function, this variable is 
               used.

   returns: Indicates what type of mail was sent by the printer.
*/

extern BYTE  ReadMail( BYTE *rtn_parm );


/***********************************************************************

   SendMail:   This function sends mail to the printer.  The caller
               provides the command and command modifier and this
               function will build a packet and send if off.

   input:   cmd - Specifies what type of request is being made of the
               printer.

            cmd_mod - If the command supports modifications, this value
               will indicate which modification is desired.

            input_parm - Depending on the request, this value may be
               used.

   output:  None

   returns: TRUE if able to send letter.
*/

extern BYTE  SendMail( BYTE cmd, BYTE cmd_mod, BYTE input_parm );


/***********************************************************************

   InitMIO: This function gets called after the mOS has perfomed a 
            destructive self test.  This then performs initialization 
            and buffer negotiation.

   input:   None

   output:  None

   returns: None
*/

extern void  InitMIO( );


/***********************************************************************

   ReadPrn: This function transfers data received from the printer and
            passes the data on to the mOS for possible shipment back to
            a host computer.  This function gets called if the available
            write pool count is greater than zero.

   input:   None

   output:  ptr - pointer to where data will be stored.

   returns: The amount of data transfered.
*/

extern UINT16   ReadPrn( BYTE *ptr );


/***********************************************************************

   WritePrn:   This function sends data received from a host computer
            to the printer.  This function will use as many READBLOCK's
            as necessary to complete the transfer.  Should the transfer
            require more READBLOCK's, the current state of the transfer
            is saved and the function returns a value of TRUE.  Only
            when the transfer is complete will a value of FALSE be
            returned.  
            
            NOTE: The caller should NOT attempt to send another job
            until the current job has completed.  ptr & len only have
            meaning when a job is FIRST sent.

   input:   ptr - points to where to get the data to send.

            len - number of bytes to send.

            AvailReadBlocks - number of READBLOCK's available to send.
            
            EndOfJob - If TRUE, means last packet in job.

   output:  AvailReadBlocks - remaining READBLOCK's

   returns: TRUE means transfer HAS NOT completed; FALSE means transfer
            has completed and another job many be sent.
*/

extern BOOL  WritePrn( BYTE *ptr, UINT16 len, UINT16 *AvailReadBlocks,
                       BYTE EndOfJob );


/***********************************************************************

   InterruptPrinter: This function tells the MIO hardware to interrupt
            the printer.

   input:   None

   output:  None

   returns: None
*/

extern void  InterruptPrinter( );


/***********************************************************************

   DeliverPacket: This function waits until the printer is ready to
            receive a packet.  When the printer is ready, the packet
            gets attached to the printer's mailbox and then sent.

   input:   packet_ptr - pointer to the packet to be sent.

   output:  None

   returns: None
*/

extern void  DeliverPacket( PAC_HEAD *packet_ptr );


/***********************************************************************

   RestartMIO: This function must be called at power up and after hard-
            ware resets.  This function will test the mailboxes and zero
            them out when finished.  NOTE: The printer expects this to
            be completed within two seconds after MIO nRESET line is
            de-asserted.

   input:   None

   output:  None

   returns: None

*/

extern void  RestartMIO( );


/***********************************************************************

   HangMIO: This function writes an error number in the printers mail-
            box and loops until a reset occurs.
            
   input:   errornum - value to store into the printers mailbox.

   output:  None

   returns: None

*/

extern void  HangMIO( BYTE errornum );


/***********************************************************************

   pack:    This function take unpacked data, data in every other byte,
            and moves the bytes together.  
            NOTE:  This function takes two bytes in two adjacent words
                   and builds a single word from them. 
            
   input:   src - points to the low order byte.

   output:  None

   returns: value of the two combined bytes.

*/

extern UINT16  pack( UINT16 *src );


/***********************************************************************

   unpack:  This function take data in one word and moves the two bytes
            into two adjacent words (i.e., B1B2 -> xxB1.xxB2 ).
            
   input:   src - contains the value to split.

   output:  dst - points to where the bytes will go.

   returns: None

*/

extern void  unpack( UINT16 *dst, UINT16 src );


/***********************************************************************

   pack2:   This function takes unpacked data, data in every other byte,
            and moves the bytes together.  This function packs 4 bytes 
            from 4 words into 2 words.
            
   input:   src - points to the low order byte.

   output:  None

   returns: value of the four combined bytes.

*/

extern UINT32  pack2( UINT16 *src );


/***********************************************************************

   unpack2: This function takes data in 2 words and moves the 4 bytes
            into 4 adjacent words(ie, B1B2B3B4 -> xxB1.xxB2.xxB3.xxB4 ).
            
   input:   src - contains the value to split.

   output:  dst - points to where the bytes will go.

   returns: None

*/

extern void  unpack2( UINT16 *dst, UINT32 src );

