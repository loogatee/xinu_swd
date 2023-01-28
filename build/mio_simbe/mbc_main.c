/*
   $Header: /home/swd.cvs/build/mio_simbe/mbc_main.c,v 1.1 1992/12/22 18:41:44 johnr Exp $
   $Log: mbc_main.c,v $
 * Revision 1.1  1992/12/22  18:41:44  johnr
 * JR:  All these files are for the "simulated MIO back-end code".  This
 *      is Ted's original version of a simulated MIO back-end that runs
 *      on Xinu.
 *
*/

/***********************************************************************

   Title:   MBC_MAIN.C

   Purpose: This file glues the printer to the MIO board.
   
   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "conf.h"
#include "kernel.h"
#include "systypes.h"
#include "network.h"
#include "spooler.h"

#include "pdtypes.h"
#include "mbc_def.h"
#include "mbc_func.h"
#include "mbc_text.h"

extern BYTE  MIO_Mail; 
extern BYTE  OnLine;   
extern BYTE  MIO_Error;
extern BYTE  DataReady;
extern int   IntrMIO;        /* Zinu semaphore to interrupt MIO       */
extern int   IntrPrinter;    /* Zinu semaphore to interrupt Ptr       */
extern int   IntrDespooler;  /* Zinu semaphore to interrupt despooler */
extern SPL_QJOBS *spl_qjactive_start;  /* Start of Qjob active list   */


backend_write(buf,len)
char	*buf;
int		len;
{
	DataReady = TRUE;
	signal(IntrMIO);
	wait(IntrDespooler);
}

/***********************************************************************

   MBC_Main:   This procedure should glue the printer with my MBC code.

   input:   None
   
            None

   output:  None

   returns: None

***********************************************************************/

PROCESS  MBC_Main( )

{              /* Local Variables */

   UINT16    dude[2];
   UINT16    numbytes, ReadBlockPool = 0, WriteBlockPool = 0;
   BYTE      ErrorFlagged, EndOfJob, rtn_val, *dataptr;
   SPL_DNODE *dnode_ptr;


               /* Initialize MIO board variables and build transport 
                  packets to communicate with the printer. */

   InitMIO();

               /* Let's plan on looping for awhile. */

   while( TRUE )
   {
               /* Wait for the printer to talk to us */
   
      wait( IntrMIO );

               /* If any errors are detected and they affect the 
                  printer, let the printer know we're having 
                  problems. */

      if( MIO_Error && !ErrorFlagged )
      {
         SendMail( ASYNC_IO_STATUS, 0, 1 );                /* Error 1 */
         ErrorFlagged = TRUE;
      }

               /* If we were in an error state and the error has been
                  cleared, inform the printer that we can go back
                  online. */

      if( !MIO_Error && ErrorFlagged )
      {
         SendMail( ASYNC_IO_STATUS, 0, 0 );
         ErrorFlagged = FALSE;
      }

               /* Test to see if the printer has any data to send
                  back to the host. */

      if( WriteBlockPool )
      {
         --WriteBlockPool;
         numbytes = ReadPrn( dataptr );
/* ///         mOS_OutGoing( dataptr, numbytes );    \\\ */
      }


               /* Test to see if the printer sent us any mail.  If
                  it has, perform the appropriate services. */

      if( MIO_Mail )
      {
         MIO_Mail = FALSE;
         switch( ReadMail( &rtn_val ) )
         {  
            case READBLOCK:
               ++ReadBlockPool;
               break;

            case WRITEBLOCK:
               ++WriteBlockPool;
               break;

            case SELFTEST:
  /*             mOS_Reset();  */
               printf("Call mOS_Reset()");
               xp_tracev(80,"Call mOS_Reset  &MBC_Main = 0x%x\n",&MBC_Main);
               break;

            case DEST_SELFTEST:
  /*             mOS_ReBoot();  */
               printf("Call mOS_ReBoot()");
               xp_tracev(80,"Call mOS_ReBoot  &MBC_Main = 0x%x\n",&MBC_Main);
               InitMIO();
               break;

            case ASYNC_PER_STATUS:
               OnLine = rtn_val;
               break;

            default:
               break;
         }
      }
      
               /* Do we have any data to send to the printer?
                  If so, send it.  When first signaled, get the 
                  pointer looking at the active job.  DataReady
                  will stay TRUE until the entire data block
                  gets transfered. */

      if( DataReady )
      {
         if( !IOStatus.holding_data )
         {  
            IOStatus.holding_data = TRUE;
            dnode_ptr = spl_qjactive_start->qj_iptr;
         }
         
         if( ReadBlockPool )
         {
            EndOfJob = dnode_ptr->in_status & SPL_STEND;
            DataReady = WritePrn( dnode_ptr->in_buf,
                                  dnode_ptr->in_len,
                                  &ReadBlockPool,
                                  EndOfJob );
                                  
               /* Once the data gets transfer to the printer, signal
                  the despooler to continue with clean-up. */
                  
             if( !DataReady )
             {  
                signal( IntrDespooler );
                IOStatus.holding_data = FALSE;
             }
         }                                  
         else
            SendMail( ASYNC_IO_STATUS, 0, 0 ); /* No error, just data */
      }                                        /* waiting to be sent. */
      
      signal( IntrPrinter );     /* Let the printer do some work */
   }
}
