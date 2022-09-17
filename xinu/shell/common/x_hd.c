/* x_hd.c - x_hd */

#include <conf.h>
#include <kernel.h>

typedef unsigned long	ulong;
typedef unsigned char	uchar;

#define BYTES_ONE_LINE  16
#define INDEX_ASC_STR   65


/*------------------------------------------------------------------------
 *  x_hd  -  hex dump
 *------------------------------------------------------------------------
 */
COMMAND	x_hd(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char	str[80];
	ulong	addr;
	ulong	count;
	int	i;

	if (nargs == 1)
	{
		fprintf(stdout,"Usage: x_hd <address (hex)> [count (decimal)]\n");
		return(OK);
	}
	else if (nargs == 2)
	{
		if (hex_to_bin(args[1],&addr))
		{
			fprintf(stdout,"Error: bad hex address entered\n");
			return(OK);
		}
		count = 256;
	}
	else if (nargs == 3)
	{
		if (hex_to_bin(args[1],&addr))
		{
			fprintf(stdout,"Error: bad hex address entered\n");
			return(OK);
		}
		count = atoi(args[2]);
	}
	else
	{
		fprintf(stdout,"Usage: x_hd <address (hex)> [count (decimal)]\n");
		return(OK);
	}

	hex_dump(stdout,addr,count);

	return(OK);
}




hex_to_bin(str,bin)
uchar	*str;
long	*bin;
{
	int		i;
	int		x;
	uchar	ch;
	long	ret;

	ret  = 0;
	*bin = 0;

	for (i=0; (ch = *(str+i)); ++i)
	{
		if (ch >= '0' && ch <= '9')
			x = 48;
		else if (ch >= 'a' && ch <= 'f')
			x = 87;
		else if (ch >= 'A' && ch <= 'F')
			x = 55;
		else
		{
			ret = 1;
			break;
		}

		*bin = (*bin << 4) + (ch - x);
	}

	return(ret);
}




static
concat(str,ch)
char	*str;
char	ch;
{
	while(*str++)
		;
	*str-- = 0;
	*str   = ch;
}



hex_dump(dev,addr1,count)
int		dev;
uchar	*addr1;
long	count;
{
	char	asc_str[18];
	char	hstr[12];
	char	tbuf[40];
	long	i;
	long	bc;
	uchar	ch;
	char	disp[88];

	do
	{
		asc_str[0] = 0;
		disp[0]    = 13;
		disp[1]    = 10;
		disp[2]    = 0;

		sprintf(hstr,"%08x",addr1);
		concat(hstr,32);
		strcat(disp,hstr);

		concat(disp,32);

		for (bc = 0, i = 0; bc < BYTES_ONE_LINE && count; ++bc, --count)
		{
			ch = *addr1;

			if (ch >= 32 && ch <= 126)
				concat(asc_str,ch);
			else
				concat(asc_str,'.');

			sprintf(hstr,"%02x",ch);
			strcat(disp,hstr);

			if (i == 3)
			{
				concat(disp,32);
				i = 0;
			}
			else
				++i;

			++addr1;
		}

		while (strlen(disp) != INDEX_ASC_STR)
			concat(disp,32);

		strcat(disp,asc_str);
		fprintf(dev,"%s",disp);

	} while (count);

	fprintf(dev,"\n\r\n");
}
