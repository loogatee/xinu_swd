/*
   $Header$
   $Log$
*/

/***********************************************************************

   Title:   MBC_DEF.H

   Purpose: This file defines the difference structures used to comm.
            between the printer and the MIO board.

   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "pdtypes.h"

               /* The following structure defines the mailboxes found
                  in location zero of the MIO's data space. */

typedef struct MAILBOX_tag
{
   UINT16   IO_status;
   UINT16   IO_error;
   UINT16   IO_pointer[2];

   UINT16   PR_status;
   UINT16   PR_error;
   UINT16   PR_pointer[2];
} MAILBOX;

               /* This next structure defines the packet header
                  used by both the printer & MIO board. */

typedef struct PAC_HEAD_tag
{
   UINT16   dev_spec[8];
   UINT16   command;
   UINT16   command_mod;
   UINT16   rtn_status;
   UINT16   buf_type;
   UINT16   start_data[2];
   UINT16   data_len[2];
   UINT16   max_data_len[2];
} PAC_HEAD;

typedef struct PRINTER_STATUS_tag
{
   BYTE     off_line;                 /*  Minimum requirements */
   BYTE     paper_out;                /*     "         "       */
   BYTE     intervention;             /*     "         "       */
   BYTE     idle_mode;                /*     "         "       */
   BYTE     connect_ack;              /*     "         "       */
   BYTE     printer_err;              /*     "         "       */
   BYTE     reset_io_channel;
   BYTE     printer_req_paper;
   BYTE     paper_jam;
   BYTE     toner_problem;
   BYTE     page_punt;
   BYTE     memory_out;
   BYTE     io_channel_inactive;
   BYTE     printer_busy;
   BYTE     printer_wait_data;
   BYTE     printer_init;
   BYTE     cover_open;
   BYTE     printer_printing;
   BYTE     paper_output_prob;
} PRINTER_STATUS;

typedef struct IO_STATUS_tag
{
   BYTE     holding_data;
   BYTE     io_error;
   BYTE     link;
   BYTE     io_idle_mode;
   BYTE     obsolete;
   BYTE     io_not_ready;
   BYTE     connection_state;
} IO_STATUS;

               /* This next section extern's the globally defined
                  variables. */

extern MAILBOX  *Mb;              /* Mailbox for printer & MIO board  */
extern BYTE     Language;         /* Specifies current lang selection */
extern BYTE     DisplayType;      /* Specifies printer LCD config     */
extern IO_STATUS      IOStatus;   /* Contains the lastest MIO info    */
extern PRINTER_STATUS PrinterStatus; /* Has the lastest printer info  */

