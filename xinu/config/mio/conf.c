/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <conf.h>

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*  Format of entries is:
device-number, device-name,
init, open, close,
read, write, seek,
getc, putc, cntl,
device-csr-address, input-vector, output-vector,
iint-handler, oint-handler, control-block, minor-device,
*/

/*  CONSOLE  is tty  */

0, "CONSOLE",
ttyinit, ionull, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
01770020004, 0170, 0170,
ioerr, ioerr, NULLPTR, 0
	};
