/*
   $Header$
   $Log$
*/

/***********************************************************************

   Title:   MIO_TEXT.H

   Purpose: This file declares text used to comm. with the printer's
            LCD display.  The mate to this file is MIO_TEXT.C, which
            contains the actual strings.

   Created: 22 June 1992

   Author:  Ted Ohnstad

***********************************************************************/

#include "pdtypes.h"

               /* The NUM_LANG indicates the number of languages
                  we are presently supporting. */

#define  NUM_LANG          2
#define  NUM_PORTS         2
#define  IO_DESC_SIZE      5
#define  CONFIG_ITEM_SIZE  5
#define  CONFIG_PARM_SIZE  1

               /* The list below defines all the language codes
                  supported by the MIO v5.0 design spec. */

#define  ENGLISH     0x00
#define  FRENCH      0x01
#define  GERMAN      0x02
#define  SPANISH     0x03
#define  ITALIAN     0x04
#define  SWEDISH     0x05
#define  DANISH      0x06
#define  NORWEGIAN   0x07
#define  DUTCH       0x08
#define  FINNISH     0x09
#define  KATAKANA    0x0A
#define  NUMERIC     0xFF

extern const char *io_description[NUM_LANG][IO_DESC_SIZE];
extern const char *port_select[NUM_LANG][NUM_PORTS];
extern const char *config[NUM_LANG];
extern const char *config_item[NUM_LANG][CONFIG_ITEM_SIZE];
extern const char *config_parm[NUM_LANG][CONFIG_ITEM_SIZE][CONFIG_PARM_SIZE];


