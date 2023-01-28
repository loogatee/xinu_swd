/*
   $Header: /home/swd.cvs/xinu/sys/miombc/mbc_text.c,v 1.1 1993/01/26 16:55:22 johnr Exp $
*/

/***********************************************************************

   Title:   MBC_TEXT.C

   Purpose: This file contains all the ASCII text used to comm. with
            the printer's LCD display.  Depending on the language 
            selection, the appropriate string is sent.

   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "pdtypes.h"
#include "mbc_text.h"


char  *config[NUM_LANG] = { "MIO config.     " ,
                            "French MIO conf." };


char  *io_description[NUM_LANG][IO_DESC_SIZE] = 

                     /* English */

               { "JRs Printer Card  " ,
                 "JR Tech. Support (619) 549-7885" ,
                 "NOS Support: Novell " ,
                 "ROM rev: " ,
                 "Port Select: ",                   /* 10BaseT or BNC */
                 "Memory Avail: ",
                 "Ethernet Addr: ",
                 "Packets RX: ",
                 "Packets TX: ",
                 "Collisions: ",
                 "Unsendable: ",
                 "Bad Packets: ",
                 "Framing Errors: ",
                 "CRC Errors: ",
                 "Total Bytes RX: ",
                 "Total Bytes Printed: ",
                 " " ,
                 " " , " " , " " , " " , " " , " " , " " , " " , " " ,
                 " " , " " , " " , " " , " " , " " , " " , " " , " " ,
                 " " , " " , " " , " " , " " ,
                  
                     /* French */

                 "La JR Inc.        " ,
                 "JR Tech. Support (619) 549-7885" ,
                 "NOS Support: Novell " ,
                 "ROM rev: " ,
                 "Port Select: ",
                 "Memory Avail: ",
                 "Ethernet Addr: ",
                 "Packets RX: ",
                 "Packets TX: ",
                 "Collisions: ",
                 "Unsendable: ",
                 "Bad Packets: ",
                 "Framing Errors: ",
                 "CRC Errors: ",
                 "Total Bytes RX: ",
                 "Total Bytes Printed: " ,
                 " " ,
                 " " , " " , " " , " " , " " , " " , " " , " " , " " ,
                 " " , " " , " " , " " , " " , " " , " " , " " , " " ,
                 " " , " " , " " , " " , " " };


char  *port_select[NUM_LANG][NUM_PORTS] = 

                     /* English */

               { "10BaseT" ,
                 "BNC" ,
                  
                     /* French */

                 "10BaseT",
                 "BNC" };


char  *config_item[NUM_LANG][CONFIG_ITEM_SIZE] = 

                     /* English */

               { "IP ADDR-1" ,
                 "IP ADDR-2" ,
                 "IP ADDR-3" ,
                 "IP ADDR-4" ,
                 "TIMEOUT" ,

                     /* French */

                 "LE IP-1" ,
                 "LE IP-2" ,
                 "LE IP-3" ,
                 "LE IP-4" ,
                 "LA TIME" };

char  *config_parm[NUM_LANG][CONFIG_ITEM_SIZE][CONFIG_PARM_SIZE] = 

                     /* English */

               { " " ,                 /* Item IP-1 */

                 " " ,                 /* Item IP-2 */

                 " " ,                 /* Item IP-3 */

                 " " ,                 /* Item IP-4 */

                 " " ,                 /* Item Timeout */

                     /* French */

                 " " ,                 /* Item IP-1 */

                 " " ,                 /* Item IP-2 */

                 " " ,                 /* Item IP-3 */

                 " " ,                 /* Item IP-4 */

                 " " };                /* Item Timeout */

               /* Our test page strings. */

char  *card_title[NUM_LANG] = 
      { "JRNet Network Printer Interface Card\r\n" ,
        "La JRNet Network Printer Interface Card\r\n" };

char  *our_addr[NUM_LANG] = 
      { "JR Inc., San Diego, CA\r\n" ,
        "JR Inc, San Diego, CA - USA\r\n" };



/*
   $Log: mbc_text.c,v $
 * Revision 1.1  1993/01/26  16:55:22  johnr
 * JR:  Initial entry for all these files.  These files are all from Ted's
 *      original MIO driver code, and I have already modified them slightly.
 *
 * Revision 1.1  1992/12/22  18:39:55  johnr
 * JR:  Initial build files for mio_mbc.  This code has no networking, but
 *      does have an MIO interface.
 *
 * 
 *    Rev 1.2   02 Dec 1992 19:34:52   TED.OHN
 * Added some blank lines to move strings around.
 * 
 *    Rev 1.1   25 Nov 1992 09:25:20   TED.OHN
 * Changed and added information used in the printers test page.
 * 
 *    Rev 1.0   07 Oct 1992 17:23:14   TED.OHN
 * Initial revision.
*/

