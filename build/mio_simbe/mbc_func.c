/*
   $Header: /home/swd.cvs/build/mio_simbe/mbc_func.c,v 1.1 1992/12/22 18:41:43 johnr Exp $
   $Log: mbc_func.c,v $
 * Revision 1.1  1992/12/22  18:41:43  johnr
 * JR:  All these files are for the "simulated MIO back-end code".  This
 *      is Ted's original version of a simulated MIO back-end that runs
 *      on Xinu.
 *
*/

/***********************************************************************

   Title:   MBC_FUNC.C

   Purpose: This file contains functions used to communicate back and
            forth between the printer and the MIO board.

   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

/* #include <string.h> */
#include "pdtypes.h"
#include "mbc_def.h"
#include "mbc_func.h"
#include "mbc_text.h"

               /* Global variables */

BYTE     NvRam[4];               /* Config settings found in NVRAM    */
BYTE     Language;               /* Indicates current native language */
BYTE     DisplayType;            /* Specifies printer LCD config      */
IO_STATUS      IOStatus;         /* Contains the lastest MIO info     */
PRINTER_STATUS PrinterStatus;    /* Contains the lastest printer info */

extern BYTE    SharedMemory[];   /* Points to shared memory           */
extern MAILBOX *Mb;              /* Mailbox for printer & MIO board   */
extern BYTE    MIO_Mail;         /* If set = mail from the printer    */
extern BYTE    PER_Mail;         /* If set = mail from the MIO        */
extern int     IntrMIO;          /* Zinu semaphore to interrupt MIO   */
extern int     IntrPrinter;      /* Zinu semaphore to interrupt Ptr   */

               /* Local static variables */

static int  config_item_indx;       /* Current item selected          */
static int  config_parm_indx;       /* Current parameter selected     */
static BYTE BufConfig[3];           /* Config parms for buffer negot. */
static BYTE PtrID[8];               /* Printers IDENTIFY response     */
static BYTE *CurrentPtr;            /* Used in WritePrn, tracks data  */
static UINT16 BytesRemaining;       /* Used in WritePrn, data left    */
static UINT16 config_array[CONFIG_ITEM_SIZE][2]; /* 1=active;2=#parms */
static PAC_HEAD *HeadReadPool, *TailReadPool;
static PAC_HEAD *HeadWritePool, *TailWritePool;
static PAC_HEAD *HeadIOGPPool, *TailIOGPPool;
static PAC_HEAD *HeadPRGPPool, *TailPRGPPool;


/***********************************************************************

   ReadMail:   This function read the mail, clears the new mail flag
               and performs the necessary actions required to satify
               the printer.

   input:   None

   output:  rtn_parm - Depending on the function, this variable is 
               used.

   returns: Indicates what type of mail was sent by the printer.

***********************************************************************/

BYTE  ReadMail( BYTE *rtn_parm )

{              /* Local Variables */

   BYTE     aui_tpi = 0;    /* WANT TO REMOVE WHEN HARDWARE AVAILABLE */
   BYTE     *ptr, *tmp_ptr, i, loop, retval, buf_type, cmd, cmd_mod;
   PAC_HEAD *packet_ptr;

               /* See if the printer requested a destructive 
                  self test. */

   if( Mb->IO_status & 2 )                            /* Test clr_req */
   {  *rtn_parm = (BYTE)(Mb->IO_pointer[0] & 0x00FF);  /*want LSB*/

      Mb->IO_status = 0;
      Mb->IO_error = 0;
      unpack(Mb->IO_pointer, 0);

      Mb->PR_status = 0;
      Mb->PR_error = 0;
      unpack(Mb->PR_pointer, 0);

               /* Since I don't know exactly how long the destructive
                  self test will take, I will tell the printer that
                  we'll need ten seconds.  The default is one second
                  and then it times out. */

      unpack(Mb->PR_pointer, 10);
      Mb->PR_status |= 0x04;                        /* Set tm_out bit */

      retval = DEST_SELFTEST;
   }

               /* Determine what type of mail we just received. */

   else
   {  packet_ptr = (PAC_HEAD *)(pack(Mb->IO_pointer) + SharedMemory);
      buf_type = packet_ptr->buf_type;
      cmd = packet_ptr->command;
      cmd_mod = packet_ptr->command_mod;

      if( buf_type == 0 && cmd == 0 && cmd_mod == 0 )
         retval = READBLOCK;

      else if( buf_type == 1 && cmd == 0 && cmd_mod <= 3 )
         retval = WRITEBLOCK;

               /* Check for printer requestes. */

      else if( buf_type == 2 )
      {  if( cmd == 0 && cmd_mod == 0 )
            retval = SET_PERIPHERAL_DISPLAY;

         else if( cmd == 1 && (cmd_mod == 1 || cmd_mod == 2) )
            retval = SELFTEST_DESC;

         else if( cmd == 2 && (cmd_mod == 1 || cmd_mod == 2) )
            retval = SELFTEST;

         else if( cmd == 3 && cmd_mod < IO_DESC_SIZE )
            retval = IO_DESC;

         else if( cmd == 4 && cmd_mod == 0 )
            retval = ASYNC_PER_STATUS;

         else if( cmd == 5 && (cmd_mod == 0 || cmd_mod == 1) )
            retval = INQUIRE_IO_STATUS;

         else if( cmd == 6 && cmd_mod < 6 )
            retval = ASCII_IO_CONF;

         else if( cmd == 11 && cmd_mod < 2 )
            retval = ASCII_STATUS;

         else
            retval = UNKNOWN_CMD;
      }

               /* Check if it's a response from a earlier request. */

      else if( buf_type == 3 )
      {  if( cmd == 0 && cmd_mod == 0 )
            retval = BUFF_CONFIG;

         else if( cmd == 1 && cmd_mod == 0 )
            retval = SAVE_IO_CONFIG;

         else if( cmd == 2 && cmd_mod == 0 )
            retval = RETRIEVE_IO_CONFIG;

         else if( cmd == 3 && cmd_mod == 0 )
            retval = IDENTIFY;

         else if( cmd == 4 && cmd_mod == 0 )
            retval = ASYNC_IO_STATUS;

         else
            retval = UNKNOWN_CMD;

               /* Place the IO_GP buffer back into the pool.  The
                  pointer will still be valid for data extraction
                  further down in this routine. */

         if( HeadIOGPPool == NULL )                         /* No     */
            HeadIOGPPool = TailIOGPPool = packet_ptr;       /* blocks */
         else
         {  unpack2(TailIOGPPool->dev_spec, (UINT32)packet_ptr);
            TailIOGPPool = packet_ptr;
         }
         unpack2(packet_ptr->dev_spec, 0L);
      }
      else
         retval = UNKNOWN_CMD;
   }

               /* We need to clear the "new" mail bit.  Before
                  that, we need to reset the MIO_Mail flag. */

   MIO_Mail = FALSE;
   Mb->IO_status &= 0xFE;                         /* Clear mb_vld bit */
   InterruptPrinter();

               /* Now that we have figured out what we received,
                  start processing it. */

   switch( retval )
   {  
               /* For the read data block, the head points to the
                  oldest block and the tail will point to the block
                  just read. */
   
      case READBLOCK:
         xp_tracev(80,"READBLOCK\n");
         if( HeadReadPool == NULL )                         /* No     */
            HeadReadPool = TailReadPool = packet_ptr;       /* blocks */
         else
         {  unpack2(TailReadPool->dev_spec, (UINT32)packet_ptr);
            TailReadPool = packet_ptr;
         }
         unpack2(packet_ptr->dev_spec, 0L);
         break;
         
               /* Although the ASCII_STATUS request isn't supported 
                  by the IIIsi, I believe that future products will.
                  This request will be treated like a WRITEBLOCK.  The
                  mOS will have to do a ReadPrn to retrieve the data. */

      case WRITEBLOCK:
      case ASCII_STATUS:
         xp_tracev(80,"WRITEBLOCK/ASCII_STATUS\n");
         if( HeadWritePool == NULL )                        /* No     */
            HeadWritePool = TailWritePool = packet_ptr;     /* blocks */
         else
         {  unpack2(TailWritePool->dev_spec, (UINT32)packet_ptr);
            TailWritePool = packet_ptr;
         }
         unpack2(packet_ptr->dev_spec, 0L);
         break;

      case SET_PERIPHERAL_DISPLAY:
         xp_tracev(80,"SET_PER_DISPLAY\n");
         DisplayType = 0;              /* We support one display type */
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         packet_ptr->rtn_status = UNDERSTOOD;

               /* Since we only support one display type, this is
                  checking to see that no other type is sent. */
            
         if( *ptr != 0 )
         {  *ptr = 0;
            packet_ptr->rtn_status = ERROR_ENCOUNTERED;
         }

               /* At present, the numeric language will not be
                  supported.  If it ever does, this logic will
                  need to be modified to handle it. */

         ++ptr;
         if( (Language = *ptr) > (NUM_LANG - 1) )
         {  *ptr = Language = 0;
            packet_ptr->rtn_status = ERROR_ENCOUNTERED;
         }
         xp_tracev(80,"Language=%d\n", Language);
         
               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

               /* For now, since we only support two self tests,
                  test 1 will be a non-destructive self test and
                  test 2 will be a destructive self test with each
                  set for a timeout of 10 seconds.  The timeout is
                  long but it's not a problem since we tell the 
                  printer when we're done anyway. */

      case SELFTEST_DESC:
         xp_tracev(80,"SELFTEST_DESC\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         packet_ptr->rtn_status = UNDERSTOOD;
         unpack(packet_ptr->data_len, 2);
         *ptr++ = (cmd_mod == 1) ? 1 : 2;
         *ptr = 10;
         
               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

               /* Until I know what sort of non-destructive self test
                  I can perform we'll not worry about it. */

      case SELFTEST:
         xp_tracev(80,"SELFTEST\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         if( cmd_mod == 1 )
         {  packet_ptr->rtn_status = UNDERSTOOD;
            *ptr++ = 0;                                /* no error    */
            *ptr++ = 0;                                /* null string */
         }
         else if( cmd_mod == 2 )           /* Done only thru Dest_S.T.*/
         {  packet_ptr->rtn_status = NOT_UNDERSTOOD;
            *ptr++ = 1;                                /* error 1     */
            *ptr++ = 0;                                /* null string */
            Mb->PR_error = 0xC0;                       /* Protcol err */ 
         }
         unpack(packet_ptr->data_len, 2);

               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

      case IO_DESC:
         xp_tracev(80,"IO_DESC\n");
         packet_ptr->rtn_status = UNDERSTOOD;
         strcpy((char *)(pack(packet_ptr->start_data) + SharedMemory),
                (char *)(io_description[Language][cmd_mod]));

               /* The port select needs to have appended which
                  port is currently selected.  Should port select
                  be moved, the "if" statement below needs to be
                  updated. */
                
         if( cmd_mod == 3 )
         {
            i = aui_tpi ? 1 : 0;
            strcat((char *)(pack(packet_ptr->start_data) + SharedMemory),
                   (char *)(port_select[Language][i]));
         }
         unpack(packet_ptr->data_len, 
            strlen((char *)(io_description[Language][cmd_mod])));

               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

      case ASYNC_PER_STATUS:
         xp_tracev(80,"ASYNC_PER_STATUS\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         packet_ptr->rtn_status = UNDERSTOOD;

               /* Extract the printer status. */

         tmp_ptr = (BYTE *)&PrinterStatus;
         for(i = 0; i < pack(packet_ptr->data_len); i++)
            *tmp_ptr++ = *ptr++;

               /* Return the printer's current state (online/offline) */

         IOStatus.io_idle_mode = PrinterStatus.idle_mode;
         *rtn_parm = PrinterStatus.idle_mode ? IDLE_MODE : OPERATE_MODE;

               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

      case INQUIRE_IO_STATUS:
         xp_tracev(80,"INQUIRE_IO_STATUS\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         unpack(packet_ptr->data_len, 7);
         packet_ptr->rtn_status = UNDERSTOOD;

               /* Report back the current status of MIO board. */

         tmp_ptr = (BYTE *)&IOStatus;
         for(i = 0; i < pack(packet_ptr->data_len); i++)
            *ptr++ = *tmp_ptr++;

               /* Since the IIIsi doesn't support command
                  modifier 01H, we'll respond as if the 
                  command modifier was 00H. */

               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

      case ASCII_IO_CONF:
         xp_tracev(80,"ASCII_IO_CONF\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         packet_ptr->rtn_status = UNDERSTOOD;

               /* This request causes us to go into idle mode. */

         IOStatus.io_idle_mode = 1;
         *rtn_parm = IDLE_MODE;

               /* Command modifiers 0 - 4 select which item and 
                  parameter to send back to the display.  Modifiers
                  0 & 1 select the item and 2 & 3 select parameters
                  within a currently selected item. */

         if( cmd_mod != 5 )
         {  if( cmd_mod == 0 )
            {  config_item_indx = 0;
               config_parm_indx = config_array[config_item_indx][0];
            }

            else if( cmd_mod == 1 )
            {  config_item_indx += 1;
               if( config_item_indx >= CONFIG_ITEM_SIZE )
                  config_item_indx = 0;
               config_parm_indx = config_array[config_item_indx][0];
            }

            else if( cmd_mod == 2 )
            {  config_item_indx -= 1;
               if( config_item_indx < 0 )
                  config_item_indx = CONFIG_ITEM_SIZE - 1;
               config_parm_indx = config_array[config_item_indx][0];
            }

            else if( cmd_mod == 3 )
            {  config_parm_indx += 1;
               if( config_parm_indx > config_array[config_item_indx][1] )
                  config_parm_indx = 0;
            }

            else
            {  config_parm_indx -= 1;
               if( config_parm_indx < 0 )
                  config_parm_indx = config_array[config_item_indx][1];
            }
         }

               /* The command modifier 5 is the "Enter" key.  It
                  means that the current parameter is desired.
                  When NVRAM is needed, this portion should save
                  the setting. */

         else
         {  config_array[config_item_indx][0] = config_parm_indx;
         }

               /* Now let's fill the response buffer.  First,
                  if the current parameter is the active one,
                  set bit 0 to 1.  Also, if the current item
                  is the last one, set bit 1 to 1. */

         *ptr++ = 0;

         *ptr = 0;
         if( config_array[config_item_indx][0] == config_parm_indx )
            *ptr = 1;
         if( config_item_indx == CONFIG_ITEM_SIZE - 1 )
            *ptr = *ptr | 2;
         ++ptr;

               /* The parameters for Config_item's 0 thru 4 use numbers,
                  therefore convert the numbers into ASCII strings.  All
                  the other parameters come from text strings. */

         strcpy((char *)ptr,
            (char *)(config_item[Language][config_item_indx]));
         ptr += strlen((char *)(config_item[Language][config_item_indx]))+1;

         if( config_item_indx < 5 )
            sprintf(ptr,"%d",config_parm_indx);
         else
         {  strcpy((char *)ptr,
             (char *)(config_parm[Language][config_item_indx][config_parm_indx]));
            ptr += strlen((char *)
             (config_parm[Language][config_item_indx][config_parm_indx]))+1;
         }

         unpack(packet_ptr->data_len, (UINT16)(ptr - 
                   (BYTE *)(pack(packet_ptr->start_data) + SharedMemory)));

               /* Return the packet to the printer. */

         DeliverPacket( packet_ptr );
         break;

               /* Only pull off 4 bytes from this response.  Only 4
                  bytes are guarantied to be saved by all printers. */

      case RETRIEVE_IO_CONFIG:
         xp_tracev(80,"RETRIEVE_IO_CONFIG\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);

               /* Must be reset to factory defaults, if zero.  The
                  calling routine does the setting. */ 
                        
         if( pack(packet_ptr->data_len) == 0 )
            for(i = 0; i < 4; i++)
               NvRam[i] = 0;
         else
            for(i = 0; i < 4; i++)
               NvRam[i] = *ptr++;
         break;

      case BUFF_CONFIG:
         xp_tracev(80,"BUFF_CONFIG\n");
         loop = (pack(packet_ptr->data_len) > 3) ?
                    3 : pack(packet_ptr->data_len);
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
                        
         for(i = 0; i < 3; i++)
            BufConfig[i] = *ptr++;
         break;

      case IDENTIFY:
         xp_tracev(80,"IDENTIFY\n");
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
                        
         loop = (pack(packet_ptr->data_len) > 8) ? 
                    8: pack(packet_ptr->data_len);
         for(i = 0; i < loop; i++)
            PtrID[i] = *ptr++;
         break;

          /* Since we didn't understand the message, just
                  return the packet indicating as much. */

      case UNKNOWN_CMD:
         xp_tracev(80,"UNKNOWN_CMD\n");
         packet_ptr->rtn_status = NOT_UNDERSTOOD;
         DeliverPacket( packet_ptr );
         break;

      default:
         break;
   }

   return( retval );
}


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

***********************************************************************/

BYTE  SendMail( BYTE cmd, BYTE cmd_mod, BYTE input_parm )

{              /* Local Variables */

   BYTE     *ptr, i, retval = TRUE;
   PAC_HEAD *packet_ptr;

               /* Get a I/O general purpose packet off the pool. */

   if( HeadIOGPPool != NULL )
   {  packet_ptr = HeadIOGPPool;
      HeadIOGPPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));  /*Next*/

               /* Build the general part of the request packet. */

      packet_ptr->buf_type = 3;     /* Always constant for MIO to PRN */
      packet_ptr->command_mod = cmd_mod;

               /* Based on the request, build the request specific
                  part of the packet. */

      ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
      if( cmd == BUFF_CONFIG )
      {  packet_ptr->command = 0;
         unpack(packet_ptr->data_len, 0);
      }

      else if( cmd == SAVE_IO_CONFIG )
      {  packet_ptr->command = 1;
         unpack(packet_ptr->data_len, 4);
         for(i = 0; i < 4; i++)
            *ptr++ = NvRam[i];
      }

      else if( cmd == RETRIEVE_IO_CONFIG )
      {  packet_ptr->command = 2;
         unpack(packet_ptr->data_len, 0);
      }

      else if( cmd == IDENTIFY )
      {  packet_ptr->command = 3;
         unpack(packet_ptr->data_len, 8);
         *ptr++ = 3;                      /* Bi-directional transfers */ 
         *ptr++ = 0;                      /* Undefined I/O card class */
         *ptr++ = 0;                      /* Test Harness Support     */
         *ptr++ = 0;                      /* Test Harness Support     */
         *ptr++ = 5;                      /* MIO protocol revision 5.0*/
         *ptr++ = 0;                      /* MIO protocol revision 5.0*/
         *ptr++ = 0;                      /* If set, AppleTalk Support*/
         *ptr++ = 0;                      /* If set, Messaging Support*/
      }

               /* This gets called if the MIO board has a problem or
                  if the FLASH ROM is being updated.  We're using the
                  "I/O card not ready" byte to convey our status. */

      else if( cmd == ASYNC_IO_STATUS )
      {  packet_ptr->command = 4;
         IOStatus.io_not_ready = input_parm;     /* Change our state */

         unpack(packet_ptr->data_len, 7);
         *ptr++ = IOStatus.holding_data;          /* Get the current */
         *ptr++ = IOStatus.io_error;              /* I/O status of   */
         *ptr++ = IOStatus.link;                  /* the board.      */
         *ptr++ = IOStatus.io_idle_mode;
         *ptr++ = IOStatus.obsolete;
         *ptr++ = IOStatus.io_not_ready;
         *ptr++ = IOStatus.connection_state;
      }
               
      DeliverPacket( packet_ptr );     /* Send the packet to printer. */
   }
   else
   {  xp_tracev(80,"SendMail - FALSE\n");
      retval = FALSE;
   }

   return( retval );
}


/***********************************************************************

   InitMIO: This function gets called after the mOS has perfomed a 
            destructive self test.  This then performs initialization 
            and buffer negotiation.

   input:   None

   output:  None

   returns: None

***********************************************************************/

void  InitMIO( )

{              /* Local Variables */

   BYTE     retval, reply, *data_start;
   UINT16   i, j, pr, io;
   UINT32   mem_pr, mem_io, mem_avail, mem_read, mem_write, num_read,
            num_write, mix;
   PAC_HEAD *packet_ptr;

   Language = 0;
   DisplayType = 0;
   HeadReadPool = TailReadPool = NULL;
   HeadWritePool = TailWritePool = NULL;
   config_item_indx = CONFIG_ITEM_SIZE;         /* Point at last item */
   config_parm_indx = 0;
   CurrentPtr = NULL;
   BytesRemaining = 0;

               /* Clear the our copy of NVRAM */

   for(i = 0; i < 4; i++)
      NvRam[i] = 0;

               /* Look for the number selections for each.  
                  Configuration items 0 (IP address) & 1 (timeout)
                  use numbers and are handled differently.  All
                  other items will run thru the param string to
                  determine the number of parameters. */

   for(i = 0; i < 5; i++)
   {  config_array[i][0] = 0;
      if( i != 4 )
         config_array[i][1] = 255;
      else
         config_array[i][1] = 32767;
   }

   for(i = 5; i < CONFIG_ITEM_SIZE; i++)
   {  j = 0;
      config_array[i][0] = 0;
      while( strlen((char *)(config_parm[0][i][j])) > 1 &&
             j < CONFIG_PARM_SIZE)
         ++j;
      config_array[i][1] = --j;
   }

               /* Set the printer status bytes to assumed settings. */

   PrinterStatus.off_line = 1;               /*  Minimum requirements */
   PrinterStatus.paper_out = 0;              /*     "         "       */
   PrinterStatus.intervention = 0;           /*     "         "       */
   PrinterStatus.idle_mode = 1;              /*     "         "       */
   PrinterStatus.connect_ack = 0;            /*     "         "       */
   PrinterStatus.printer_err = 0;            /*     "         "       */
   PrinterStatus.reset_io_channel = 1;
   PrinterStatus.printer_req_paper = 0;
   PrinterStatus.paper_jam = 0;
   PrinterStatus.toner_problem = 0;
   PrinterStatus.page_punt = 0;
   PrinterStatus.memory_out = 0;
   PrinterStatus.io_channel_inactive = 1;
   PrinterStatus.printer_busy = 0;
   PrinterStatus.printer_wait_data = 1;
   PrinterStatus.printer_init = 1;
   PrinterStatus.cover_open = 0;
   PrinterStatus.printer_printing = 0;
   PrinterStatus.paper_output_prob = 0;

               /* Set the IO status bytes to assumed settings. */

   IOStatus.holding_data = 0;
   IOStatus.io_error = 0;
   IOStatus.link = 0;
   IOStatus.io_idle_mode = 1;
   IOStatus.obsolete = 0;
   IOStatus.io_not_ready = 0xF0;
   IOStatus.connection_state = 0;

               /* Let the printer know that we're done with our
                  initialization process and that we're ready
                  to continue. */

   unpack(Mb->PR_pointer, 0);   /* Set the timeout value to zero      */
   Mb->PR_status |= 0x02;       /* Setting clr_rsp indicate we're done*/
                                /* with self test.                    */

               /* We must now wait for the printer to acknowledge our
                  willingness to communicate (i.e., wait for the clr_rsp
                  bit to be cleared). */

/********   while( Mb->PR_status & 0x02 );  ************/

               /* I now need to send an IO_GP packet to the printer
                  within one second.  The IO_GP packets will start
                  right after the mailboxes.  Following the IO_GP's
                  will be the PR_GP's, READ's and finally the WRITES's. 
                  All buffers will be built based on the starting
                  IO_GP address. */

                  /* Build a temporary IO_GP packet.  This will get
                     reconfigured during buffer negotiation. */

   packet_ptr = (PAC_HEAD *)(START_IOGP_OFFSET + SharedMemory);
   unpack2(packet_ptr->dev_spec, 0L);
   unpack(packet_ptr->start_data, ((UINT16)((UINT32)(packet_ptr + 1)) - 
                                            (UINT32)SharedMemory));
   unpack(packet_ptr->max_data_len, MIN_IOGP_SIZE);
   HeadIOGPPool = packet_ptr;
   
   xp_tracev(80,"Send RETRIEVE_IO_CONFIG\n");
   SendMail(RETRIEVE_IO_CONFIG, 0, 0);

               /* Now wait for the desired reply from the printer. */

   retval = FALSE;
   while( retval != RETRIEVE_IO_CONFIG )
   {  wait( IntrMIO );                         /* while( !MIO_Mail ); */
      retval = ReadMail( &reply );
   }
   
   if( NvRam[0] == 0 && NvRam[1] == 0 && 
       NvRam[2] == 0 && NvRam[3] == 0 )
   {  
      /****************** RESET TO FACTORY DEFAULTS ****************/
      /* NvRam[0] = xx;
         NvRam[1] = xx;
         NvRam[2] = xx;
         NvRam[3] = xx;
      */

      xp_tracev(80,"Send SAVE_IO_CONFIG\n");
      SendMail(SAVE_IO_CONFIG, 0, 0);

               /* Wait again for the correct response. */

      while( retval != SAVE_IO_CONFIG )
      {  wait( IntrMIO );                      /* while( !MIO_Mail ); */
         retval = ReadMail( &reply );
      }
   }

               /* Now let the printer know what we have to offer. */

   xp_tracev(80,"Send IDENTIFY\n");
   SendMail(IDENTIFY, 0, 0);

   while( retval != IDENTIFY )
   {  wait( IntrMIO );                         /* while( !MIO_Mail ); */
      retval = ReadMail( &reply );
   }

               /* Send the buffer configuration request. */

   xp_tracev(80,"Send BUFF_CONFIG\n");
   SendMail(BUFF_CONFIG, 0, 0);

   while( retval != BUFF_CONFIG )
   {  wait( IntrMIO );                         /* while( !MIO_Mail ); */
      retval = ReadMail( &reply );
   }

               /* Now it's time to build all the packet buffers and
                  ship the appropriate packets to the printer. */

   pr = (BufConfig[0]>MIN_PRGP_SIZE) ? BufConfig[0] : MIN_PRGP_SIZE;
   io = (BufConfig[1]>MIN_IOGP_SIZE) ? BufConfig[1] : MIN_IOGP_SIZE;
   mix = (BufConfig[2]>PERCENT_READ) ? BufConfig[2] : PERCENT_READ;

   mem_pr = (pr + 18) * 2;           /* 18=packet size; 2=num packets */
   mem_io = (io + 18) * 2;           /* 18=packet size; 2=num packets */

               /* Figure out how much shared memory we have to work
                  with. */

   mem_avail = MAX_SHARE_MEM - (mem_pr + mem_io) - 8; /*8=mailbox size*/
   mem_read = SIZE_READ_PACK + 18;
   mem_write = SIZE_WRITE_PACK + 18;

   num_read = (mem_avail * mix) / (mem_read * 100);
   num_write = (mem_avail * (100 - mix)) / (mem_write * 100);

               /* Calculate the remaining memory.  See if we can
                  allocate another read packet.  If we can't, NO
                  WORRIES... */

   mem_avail -= ((mem_read * num_read) + (mem_write * num_write));
   if( mix > 50 && mem_avail > mem_read )
      ++num_read;

               /* All the packets occupy the beginning of shared
                  memory - barring the mailboxes.  Determine where
                  that data portion starts. */

   data_start = (BYTE *)(8 + (4 + num_read + num_write)*18
                         + SharedMemory);

               /* Build the IO_GP packets and place them on the pool. */

   for(i = 0; i < 2; i++)      
   {  if( i == 0 )
      {  packet_ptr = (PAC_HEAD *)(START_IOGP_OFFSET + SharedMemory);
         HeadIOGPPool = packet_ptr;
         unpack2(packet_ptr->dev_spec, (UINT32)(packet_ptr + 1));
      }
      else
      {  ++packet_ptr;
         TailIOGPPool = packet_ptr;
         unpack2(packet_ptr->dev_spec, 0L);
      }

      unpack(packet_ptr->start_data, (UINT16)((UINT32)data_start -
                                              (UINT32)SharedMemory));
      unpack(packet_ptr->max_data_len, io);
      data_start += io;
   }

               /* Build the PR_GP packets and send them to the
                  printer. */

   xp_tracev(80,"Build/Send PRGP's\n");
   for(i = 0; i < 2; i++)      
   {  ++packet_ptr;
      packet_ptr->rtn_status = NEW_PACKET;
      packet_ptr->buf_type = 2;
      unpack(packet_ptr->start_data, (UINT16)((UINT32)data_start -
                                              (UINT32)SharedMemory));
      unpack(packet_ptr->max_data_len, pr);
      data_start += pr;

               /* Return the packet to the printer. */

      DeliverPacket( packet_ptr );
      wait( IntrMIO );
   }

               /* Build the read packets and send them to the
                  printer. */

   xp_tracev(80,"Build/Send Read packets\n");
   for(i = 0; i < num_read; i++)      
   {  ++packet_ptr;
      packet_ptr->rtn_status = NEW_PACKET;
      packet_ptr->buf_type = 0;
      unpack(packet_ptr->start_data, (UINT16)((UINT32)data_start -
                                              (UINT32)SharedMemory));
      unpack(packet_ptr->max_data_len, SIZE_READ_PACK);
      data_start += SIZE_READ_PACK;

               /* Return the packet to the printer. */

      DeliverPacket( packet_ptr );
      wait( IntrMIO );
   }

               /* Build the write packets and send them to the
                  printer. */

   xp_tracev(80,"Build/Send Write packets\n");
   for(i = 0; i < num_write; i++)      
   {  ++packet_ptr;
      packet_ptr->rtn_status = NEW_PACKET;
      packet_ptr->buf_type = 1;
      unpack(packet_ptr->start_data, (UINT16)((UINT32)data_start -
                                              (UINT32)SharedMemory));
      unpack(packet_ptr->max_data_len, SIZE_WRITE_PACK);
      data_start += SIZE_WRITE_PACK;

               /* Return the packet to the printer. */

      DeliverPacket( packet_ptr );
      wait( IntrMIO );
   }
}


/***********************************************************************

   ReadPrn: This function transfers data received from the printer and
            passes the data on to the mOS for possible shipment back to
            a host computer.  This function gets called if the available
            write pool count is greater than zero.

   input:   None

   output:  ptr - pointer to where data will be stored.

   returns: The amount of data transfered.

***********************************************************************/

UINT16   ReadPrn( BYTE *ptr )

{              /* Local Variables */

   UINT16   *from_ptr, cnt, len = 0;
   PAC_HEAD *packet_ptr;

               /* Get the next packet to return and update the 
                  head pointer. */

   if( HeadWritePool != NULL )
   {  packet_ptr = HeadWritePool;
      HeadWritePool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));

               /* Copy the data from the packet into the mOS memory.
                  NOTE: The from_ptr will increment by two while the
                  ptr increments by 1. */

      len = pack(packet_ptr->data_len);
      from_ptr = (UINT16 *)(pack(packet_ptr->start_data) + SharedMemory);
      for(cnt = 0; cnt < len; cnt++)
         *ptr++ = *from_ptr++ & 0x00FF;

               /* Release the packet back to the printer. */

      packet_ptr->rtn_status = UNDERSTOOD;
      DeliverPacket( packet_ptr );
   }

   return( len );
}


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

   returns: TRUE means the transfer HAS NOT completed; FALSE means the
            transfer has completed and another job many be sent.

***********************************************************************/

BOOL  WritePrn( BYTE *ptr, UINT16 len, UINT16 *AvailReadBlocks,
                BYTE EndOfJob )

{              /* Local Variables */

/****   UINT16   *to_ptr, i, cnt;   Uncomment for BOARD use   *****/
   BYTE     *to_ptr;
   UINT16   i, cnt;
   PAC_HEAD *packet_ptr;

               /* If we're starting a new job, set the data pointer and
                  counter. */

   if( !BytesRemaining )
   {  CurrentPtr = ptr;
      BytesRemaining = len;
   }

               /* Build and send as many packets as needed to complete
                  the job, stop when we're out of packet. */

   while( *AvailReadBlocks && BytesRemaining )
   {  packet_ptr = HeadReadPool;
      HeadReadPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));
      --*AvailReadBlocks;

               /* NOTE: The to_ptr increments by two while CurrentPtr
                  increments by one. */

/*****      to_ptr = (UINT16 *)(pack(packet_ptr->start_data) + SharedMemory);  ****/
      to_ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
      cnt = (BytesRemaining > pack(packet_ptr->max_data_len)) ?
               pack(packet_ptr->max_data_len) : BytesRemaining;

      for(i = 0; i < cnt; i++, BytesRemaining--)
         *to_ptr++ = *CurrentPtr++;

               /* Release the packet back to the printer. */

      unpack(packet_ptr->data_len, cnt);
      packet_ptr->rtn_status = EndOfJob ? LOGICAL_BREAK : UNDERSTOOD;
      DeliverPacket( packet_ptr );
      if( packet_ptr->rtn_status == LOGICAL_BREAK )
         xp_tracev(80,"Logical Break\n");
   }

               /* If we still have data to send, don't reset the
                  static variables (BytesRemaining & CurrentPtr).
                  Also, return TRUE if data remains, FALSE otherwise. */

   if( BytesRemaining )
      return( TRUE );         /* Require more READBLOCK's to complete */
   else
      CurrentPtr = NULL;

   return( FALSE );                         /* Transfer has completed */
}


/***********************************************************************

   InterruptPrinter: This function tells the MIO hardware to interrupt
            the printer.

   input:   None

   output:  None

   returns: None

***********************************************************************/

void  InterruptPrinter( )

{              /* Local Variables */

   signal( IntrPrinter );
}


/***********************************************************************

   DeliverPacket: This function waits until the printer is ready to
            receive a packet.  When the printer is ready, the packet
            gets attached to the printer's mailbox and then sent.

   input:   packet_ptr - pointer to the packet to be sent.

   output:  None

   returns: None

***********************************************************************/

void  DeliverPacket( PAC_HEAD *packet_ptr )

{              /* Local Variables */

   UINT16  retry = 0;

   while( Mb->PR_status & 0x01 )     /* Test mb_vld for printer ready */
   {
      sleep10( 5 );                                  /* Wait 50 msecs */         
      if( ++retry > 40 )   /* After 2 secs, give up, printer problems */
         HangMIO( PRINTER_HUNG );
   }

               /* Fill the mailbox and ship it off. */ 
      
   unpack(Mb->PR_pointer, (UINT16)((UINT32)packet_ptr - 
                                   (UINT32)SharedMemory));
   Mb->PR_status |= 0x01;                           /* Set mb_vld bit */
   PER_Mail = TRUE;
   InterruptPrinter();
}


/***********************************************************************

   RestartMIO: This function must be called at power up and after hard-
            ware resets.  This function will test the mailboxes and zero
            them out when finished.  NOTE: The printer expects this to
            be completed within two seconds after MIO nRESET line is
            de-asserted.

   input:   None

   output:  None

   returns: None

***********************************************************************/

void  RestartMIO( )

{              /* Local Variables */

   BYTE     *ptr, i;

               /* Fill mailboxes with a test pattern. */
   
   for(ptr = (BYTE *)Mb, i = 0; i < 8; i++)
      *ptr++ = 0xA5;
    
               /* Verify the pattern just written is correct and try
                  another pattern. */
      
   for(ptr = (BYTE *)Mb, i = 0; i < 8; i++)
   {  if( *ptr != 0xA5 )                        /* We've got problems */
         HangMIO( MB_TEST_ERROR );
      else
         *ptr++ = 0x5A;
   }

               /* Again, test the pattern and then zero out the 
                  mailboxes. */

   for(ptr = (BYTE *)Mb, i = 0; i < 8; i++)
   {  if( *ptr != 0x5A )                        /* We've got problems */
         HangMIO( MB_TEST_ERROR );
      else
         *ptr++ = 0;
   }
 
               /* Finally, make sure all the mailbox bytes are zero. */

   for(ptr = (BYTE *)Mb, i = 0; i < 8; ptr++, i++)
   {  if( *ptr != 0 )                           /* We've got problems */
         HangMIO( MB_TEST_ERROR );
   }
   
               /* Inform the printer we're about ready to perform
                  selftest.  We'll assume a maximum of 10 seconds
                  for the MIO board to perform its selftest. */

      unpack(Mb->PR_pointer, 10);
      Mb->PR_status |= 0x04;                        /* Set tm_out bit */
}


/***********************************************************************

   HangMIO: This function writes an error number in the printers mail-
            box and loops until a reset occurs.
            
   input:   errornum - value to store into the printers mailbox.

   output:  None

   returns: None

***********************************************************************/

void  HangMIO( BYTE errornum )

{
   xp_tracev(80,"HangMIO - Error num = %d\n", errornum);
   Mb->PR_error = errornum;
   InterruptPrinter();
   while( TRUE );
}  

 
/***********************************************************************

   pack:    This function takes unpacked data, data in every other byte,
            and moves the bytes together.  
            NOTE:  This function takes two bytes in two adjacent words
                   and builds a single word from them. 
            
   input:   src - points to the low order byte.

   output:  None

   returns: value of the two combined bytes.

***********************************************************************/

UINT16  pack( UINT16 *src )

{              /* Local variables */

   UINT16  retval = 0;
    
   retval = *src++;
   retval = (*src << 8) | (retval & 0x00FF);
   return( retval );
}


/***********************************************************************

   unpack:  This function takes data in one word and moves the two bytes
            into two adjacent words (i.e., B1B2 -> xxB1.xxB2 ).
            
   input:   src - contains the value to split.

   output:  dst - points to where the bytes will go.

   returns: None

***********************************************************************/

void unpack( UINT16 *dst, UINT16 src )

{
   *dst++ = src & 0x00FF;
   *dst = (src & 0xFF00) >> 8;
}


/***********************************************************************

   pack2:   This function takes unpacked data, data in every other byte,
            and moves the bytes together.  This function packs 4 bytes 
            from 4 words into 2 words.
            
   input:   src - points to the low order byte.

   output:  None

   returns: value of the four combined bytes.

***********************************************************************/

UINT32  pack2( UINT16 *src )

{              /* Local variables */

   UINT32  retval = 0;
    
   retval = *src++;
   retval = (*src++ <<  8) | (retval & 0x000000FF);
   retval = (*src++ << 16) | (retval & 0x0000FFFF);
   retval = (*src   << 24) | (retval & 0x00FFFFFF);
   return( retval );
}


/***********************************************************************

   unpack2: This function takes data in 2 words and moves the 4 bytes
            into 4 adjacent words(ie, B1B2B3B4 -> xxB1.xxB2.xxB3.xxB4 ).
            
   input:   src - contains the value to split.

   output:  dst - points to where the bytes will go.

   returns: None

***********************************************************************/

void unpack2( UINT16 *dst, UINT32 src )

{
   *dst++ =  src & 0x000000FF;
   *dst++ = (src & 0x0000FF00) >> 8;
   *dst++ = (src & 0x00FF0000) >> 16;
   *dst   = (src & 0xFF000000) >> 24;
}
