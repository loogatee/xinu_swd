/* $Id: mbc_text.h,v 1.1 1993/01/21 18:58:25 johnr Exp $ */


#define		NUM_LANG			2	/* num of languages currently supported */
#define		NUM_PORTS			2
#define		IO_DESC_SIZE		40
#define		CONFIG_ITEM_SIZE	5
#define		CONFIG_PARM_SIZE	1

/*
 *	The list below defines all the language codes
 *	supported by the MIO v5.0 design spec.
 */
#define		ENGLISH		0x00
#define		FRENCH		0x01
#define		GERMAN		0x02
#define		SPANISH		0x03
#define		ITALIAN		0x04
#define		SWEDISH		0x05
#define		DANISH		0x06
#define		NORWEGIAN	0x07
#define		DUTCH		0x08
#define		FINNISH		0x09
#define		KATAKANA	0x0A
#define		NUMERIC		0xFF

extern char		*io_description[NUM_LANG][IO_DESC_SIZE];
extern char		*port_select[NUM_LANG][NUM_PORTS];
extern char		*config[NUM_LANG];
extern char		*config_item[NUM_LANG][CONFIG_ITEM_SIZE];
extern char		*config_parm[NUM_LANG][CONFIG_ITEM_SIZE][CONFIG_PARM_SIZE];
extern char		*card_title[NUM_LANG];
extern char		*our_addr[NUM_LANG];

/*
 * $Log: mbc_text.h,v $
 * Revision 1.1  1993/01/21  18:58:25  johnr
 * JR:  .h files for the MIO physical interface.
 *
 *
 */
