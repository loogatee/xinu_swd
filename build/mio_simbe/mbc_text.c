/*
   $Header: /home/swd.cvs/build/mio_simbe/mbc_text.c,v 1.1 1992/12/22 18:41:46 johnr Exp $
   $Log: mbc_text.c,v $
 * Revision 1.1  1992/12/22  18:41:46  johnr
 * JR:  All these files are for the "simulated MIO back-end code".  This
 *      is Ted's original version of a simulated MIO back-end that runs
 *      on Xinu.
 *
*/

/***********************************************************************

   Title:   MIO_TEXT.C

   Purpose: This file contains all the ASCII text used to comm. with
            the printer's LCD display.  Depending on the language 
            selection, the appropriate string is sent.

   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "pdtypes.h"
#include "mbc_text.h"


const char  *config[NUM_LANG] = { "MIO config.     " ,
                                  "French MIO conf." };


const char  *io_description[NUM_LANG][IO_DESC_SIZE] = 

                     /* English */

               { "JR Inc.           " ,
                 "Novell - TCP/IP Networking Board" ,
                 "ROM rev: 0.3 - MIO rev: 5.0     " ,
                 "Port Select: ",                   /* 10BaseT or BNC */
                 "(619) 549-7885 - Tech. Support  " ,
                  
                     /* French */

                 "La JR Inc.        " ,
                 "Novell - TCP/IP Networking Board" ,
                 "ROM rev: 0.3 - MIO rev: 5.0     " ,
                 "Port Select: ",
                 "(619) 549-7885 - Si le Support  " };


const char  *port_select[NUM_LANG][NUM_PORTS] = 

                     /* English */

               { "10BaseT" ,
                 "BNC" ,
                  
                     /* French */

                 "10BaseT",
                 "BNC" };


const char  *config_item[NUM_LANG][CONFIG_ITEM_SIZE] = 

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

const char  *config_parm[NUM_LANG][CONFIG_ITEM_SIZE][CONFIG_PARM_SIZE] = 

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


