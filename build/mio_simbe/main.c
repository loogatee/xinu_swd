/*
   $Header: /home/swd.cvs/build/mio_simbe/main.c,v 1.1 1992/12/22 18:41:41 johnr Exp $
   $Log: main.c,v $
 * Revision 1.1  1992/12/22  18:41:41  johnr
 * JR:  All these files are for the "simulated MIO back-end code".  This
 *      is Ted's original version of a simulated MIO back-end that runs
 *      on Xinu.
 *
*/

/***********************************************************************

   Title:   MAIN.C

   Purpose: This file starts up the MIO simulation.

   Created: 15 July 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "conf.h"
#include "kernel.h"
#include "network.h"
#include "tty.h"

#include "pdtypes.h"
#include "mbc_def.h"
#include "mbc_func.h"
#include "mbc_text.h"

PROCESS		lp_d();
PROCESS		rlogin_d();
PROCESS		tftp_d();
PROCESS     despool_d();
PROCESS		MBC_Main();
PROCESS		PTR_Test();

int			shell_main = 0;

BYTE  MIO_Mail;            /* Might want to move these                */
BYTE  PER_Mail;            /* Might want to move these                */
BYTE  OnLine;              /* Might want to move these                */
BYTE  MIO_Error;           /* Might want to move these                */
BYTE  DataReady;           /* Despooler sets when data ready for MBC  */
int   IntrMIO;             /* Zinu semaphore to interrupt MIO         */
int   IntrPrinter;         /* Zinu semaphore to interrupt Ptr         */
int   IntrDespooler;       /* Zinu semaphore to interrupt despooler   */


               /* Global Routines */ 

extern void  DeliverMIOPacket( PAC_HEAD *packet_ptr );

               /* Shared Memory */

BYTE    SharedMemory[0x10000];                    /* 64K */
MAILBOX *Mb;


/***********************************************************************

   main:    This function kicks off the processes needed to simulate
            the MIO to printer communications.  Also, starts UDP echo
            server and the Xinu pseudo-shell.

   input:   None

   output:  None

   returns: None

***********************************************************************/

void
main( )
{

	/*
	 *	Need to test & zero the mailboxes before we continue
	 *	with startup.
	 */
	RestartMIO();

	/*
	 *	Have the Mailbox start at the beginning of
	 *	shared memory.
	 */ 
	Mb = (MAILBOX *)(&SharedMemory);

	/*
	 *	Initialize data structures associated with the spooler
	 */
	spool_init();

	/*
	 *	start up the de-spooler, the tftpd server, the lpd server,
	 *	and the rlogin server
	 */
	resume( create(despool_d, 4096, 35, "despool", 0) );
	resume( create(tftp_d,      8192, 35, "tftpd",   0) );
	resume( create(lp_d,        1024, 35, "lpd",     0) );
	resume( create(rlogin_d,    1024, 20, "rlogind", 0) );

	/*
	 *	Start up the MBC code
	 */ 
	IntrDespooler = screate(0);
	IntrPrinter   = screate(0);
	IntrMIO       = screate(1);

	resume( create(PTR_Test, 8000L, 20, "PRT", 0) );
	resume( create(MBC_Main, 8000L, 20, "MBC", 0) );

	if (shell_main == 0)
	{
		shell_main = 1;
 
		while (1)
		{
			login(CONSOLE,0);
			shell(CONSOLE,0);
		}
	}
}


/***********************************************************************

   PTR_Test:   This function should simulate the printer's responses and
            requests for MIO support.
   
   input:   None
   
            None

   output:  None

   returns: None

***********************************************************************/

static PAC_HEAD *HeadReadPool, *TailReadPool;
static PAC_HEAD *HeadPRGPPool, *TailPRGPPool;

PROCESS  PTR_Test( )

{              /* Local Variables */

   char     rbuf[2];
   BYTE     toggle = 0, i, cntr = 0, retval, buf_type, cmd, cmd_mod, *ptr;
   UINT16   countup = 0, countup1 = 16384, countup2 = 8192;
   PAC_HEAD *packet_ptr;

               /* Init some local stuff. */
               
   HeadReadPool = NULL;
   TailReadPool = NULL;

               /* For now, just acknowledge stuff sent from the MIO */

   while( TRUE )               
   {
               /* Wait for the MIO to talk to us */
   
      Mb->PR_status &= 0xFE;
      wait( IntrPrinter );

      if( PER_Mail )
      {  packet_ptr = (PAC_HEAD *)(pack(Mb->PR_pointer) + SharedMemory);
         buf_type = packet_ptr->buf_type;
         cmd = packet_ptr->command;
         cmd_mod = packet_ptr->command_mod;

                /* See if this is a READBLOCK */
   
         if( buf_type == 0 && cmd == 0 && cmd_mod == 0 )
         {  if( packet_ptr->rtn_status == NEW_PACKET )
            {  if( HeadReadPool == NULL )
                  HeadReadPool = TailReadPool = packet_ptr;      
               else
               {  unpack2(TailReadPool->dev_spec, (UINT32)packet_ptr);
                  TailReadPool = packet_ptr;
               }
               unpack2(packet_ptr->dev_spec, 0L);
               ++cntr;
            }

               /* We just got some data to print. */
               
            else
            {  ptr = (BYTE *)(pack(packet_ptr->start_data) + 
                              (UINT32)&SharedMemory);

               /* Flush the input buffer - serial junk gets in. */
               
               while( control(1,TCICHARS) != 0 )
                  read(1, rbuf, 1);
               
               /* Write the data to the device. */ 
               
               write(1, ptr, pack(packet_ptr->data_len));
   
               /* Append this "spent" packet to the end the list. */
               
               if( HeadReadPool == NULL ) 
                  HeadReadPool = TailReadPool = packet_ptr;       
               else
               {  unpack2(TailReadPool->dev_spec, (UINT32)packet_ptr);
                  TailReadPool = packet_ptr;
               }
               unpack2(packet_ptr->dev_spec, 0L);
               ++cntr;
            }
            
            signal( IntrMIO );
         }
            
               /* If reply required, return junk. */
               
         else if( buf_type == 3 )
         {  packet_ptr->rtn_status = UNDERSTOOD;
            unpack(packet_ptr->data_len, 0);
            DeliverMIOPacket( packet_ptr );
         }
         
               /* See if we have a PR_GP buffer */ 
         
         else if( buf_type == 2 )
         {
            if( packet_ptr->rtn_status == NEW_PACKET )
            {
               if( HeadPRGPPool == NULL )
                  HeadPRGPPool = TailPRGPPool = packet_ptr;
               else
               {
                  unpack2(TailPRGPPool->dev_spec, (UINT32)packet_ptr);
                  TailPRGPPool = packet_ptr;
               }
               unpack2(packet_ptr->dev_spec, 0L);
            }  
            
               /* Append this "spent" packet to the end the list. */
               
            else
            {
               if( HeadPRGPPool == NULL ) 
                  HeadPRGPPool = TailPRGPPool = packet_ptr;       
               else
               {  unpack2(TailPRGPPool->dev_spec, (UINT32)packet_ptr); 
                  TailPRGPPool = packet_ptr;
               }
               unpack2(packet_ptr->dev_spec, 0L);
            }
            
               /* Is this a Set Peripheral Display response? */ 
            
            if( cmd == 0 && cmd_mod == 0 && 
                packet_ptr->rtn_status != NEW_PACKET)
            {
                if( packet_ptr->rtn_status == UNDERSTOOD )
                   xp_tracev(80,"Understood on SET PERIPHERAL DISPLAY\n");
                else
                   xp_tracev(80,"Error on SET PERIPHERAL DISPLAY: %x\n",
                             packet_ptr->rtn_status);
            }
            
               /* Is this an I/O Description response? */ 
            
            else if( cmd == 3 && cmd_mod == 3 && 
                packet_ptr->rtn_status != NEW_PACKET)
            {
                if( packet_ptr->rtn_status == UNDERSTOOD )
                {
                   ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
                   xp_tracev(80,"I/O DESCRIPTION = %s\n", ptr);
                }
                else
                   xp_tracev(80,"Error on I/O DESCRIPTION: %x\n",
                             packet_ptr->rtn_status);
            }
            
               /* Is this an ASCII I/O Configuration response? */ 
            
            else if( cmd == 6 && cmd_mod == 1 && 
                packet_ptr->rtn_status != NEW_PACKET)
            {
                if( packet_ptr->rtn_status == UNDERSTOOD )
                {
                   ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory + 2);
                   xp_tracev(80,"ASCII I/O CONFIG = %s\n", ptr);
                }
                else
                   xp_tracev(80,"Error on ASCII I/O CONFIG: %x\n",
                             packet_ptr->rtn_status);
            }

            signal( IntrMIO );
         }          
         else
            signal( IntrMIO );
         
         PER_Mail = FALSE;                       
      }
          
               /* Since there was no mail, see if there's any 
                  READBLOCK to send to the MIO */
                  
      else
      {  if( HeadReadPool )
         {  packet_ptr = HeadReadPool;
            HeadReadPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));
            
            packet_ptr->rtn_status = 99;
            unpack(packet_ptr->data_len, 0);
            DeliverMIOPacket( packet_ptr );
         }
         else
            signal( IntrMIO );
      }
      
               /* Let's send a Language packet */
               
      if( ++countup > 32767 && HeadPRGPPool )
      {
         countup = 0;
         toggle = (toggle == 1) ? 0 : 1;     /* Picks the language 0=US 1=Fr */
         packet_ptr = HeadPRGPPool;
         HeadPRGPPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));
         
         packet_ptr->command = 0;
         packet_ptr->command_mod = 0;
         ptr = (BYTE *)(pack(packet_ptr->start_data) + SharedMemory);
         *ptr++ = 0;
         *ptr++ = toggle;
         unpack(packet_ptr->data_len, 2);
         packet_ptr->rtn_status = 99;
         
         xp_tracev(80,"Sending SET PERIPHERAL DISPLAY %d\n", toggle);
         DeliverMIOPacket( packet_ptr );
      }
      
               /* Let's request an I/O description */
               
      if( ++countup1 > 32767 && HeadPRGPPool )
      {
         countup1 = 0;
         packet_ptr = HeadPRGPPool;
         HeadPRGPPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));
         
         packet_ptr->command = 3;
         packet_ptr->command_mod = 3;
         unpack(packet_ptr->data_len, 0);
         packet_ptr->rtn_status = 99;
         
         xp_tracev(80,"Sending IO DESCRIPTION\n");
         DeliverMIOPacket( packet_ptr );
      }
               /* Let's request an ASCII I/O configuration */
               
      if( ++countup2 > 32767 && HeadPRGPPool )
      {
         countup2 = 0;
         packet_ptr = HeadPRGPPool;
         HeadPRGPPool = (PAC_HEAD *)(pack2(packet_ptr->dev_spec));
         
         packet_ptr->command = 6;
         packet_ptr->command_mod = 1;
         unpack(packet_ptr->data_len, 0);
         packet_ptr->rtn_status = 99;
         
         xp_tracev(80,"Sending ASCII IO CONFIGURATION\n");
         DeliverMIOPacket( packet_ptr );
      }
      
   }
}


/***********************************************************************

   InterruptMIO: This function tells the printer hardware to interrupt
            the MIO.

   input:   None

   output:  None

   returns: None

***********************************************************************/

void  InterruptMIO( )

{              /* Local Variables */

   MIO_Mail = TRUE;
   signal( IntrMIO );    /* Let the MIO do some work */
}


/***********************************************************************

   DeliverMIOPacket: This function waits until the MIO is ready to
            receive a packet.  When the MIO is ready, the packet
            gets attached to the MIO's mailbox and then sent.

   input:   packet_ptr - pointer to the packet to be sent.

   output:  None

   returns: None

***********************************************************************/

void  DeliverMIOPacket( PAC_HEAD *packet_ptr )

{
   while( Mb->IO_status & 0x01 )        /* Test imb_vld for MIO ready */
      sleep10(1);                                    /* Wait 10 msecs */
      
   unpack(Mb->IO_pointer, (UINT16)((UINT32)packet_ptr -
                                   (UINT32)SharedMemory));
   Mb->IO_status |= 0x01;                           /* Set mb_vld bit */
   InterruptMIO();
}


#define extern
#include <snmpvars.h>
