/* lexan.c - lexan */

#include <conf.h>
#include <kernel.h>
#include <shell.h>

/*------------------------------------------------------------------------
 *  lexan  -  ad hoc lexical analyzer to divide command line into tokens
 *------------------------------------------------------------------------
 */
lexan(shptr)
struct	shvars	*shptr;
{
	register	char	*line;
	register	char	ch;
	register	int		ntok;
	register	char	*to;
				char	**tokptr;
				char	*p;
				char	quote;

	line = shptr->shbuf;
	to = shptr->shargst;		/* area to place token strings */
	tokptr = &shptr->shtok[ntok = 0];	/* array of ptrs to tokens */
	for  (p = line ; *p!='\0' && *p!='\n' && ntok < SHMAXTOK ;) {
		while ( (ch = *p) == ' ')	/* skip leading blanks	*/
			p++;
		if (ch == '\0' || ch == '\n')	/* end of line or string*/
			return(ntok);
		*tokptr++ = to;			/* save start of token	*/
		shptr->shtktyp[ntok++] = ch;
		if (ch == '"' || ch == '\'') {	/* check for quoted str.*/
			quote = ch;
			for (p++ ; (ch = *p++) != quote && ch != '\n'
				&& ch != '\0' ; )
				*to++ = ch;
			if (ch != quote)
				return(SYSERR);
		} else {		/* other possible tokens	*/
			*to++ = *p++;
			if (ch!='>' && ch!='<' && ch!='&')
				while ((ch = *p)!='\n' && ch !='\0' &&
					ch!='<' && ch!='>' && ch!=' ' &&
					ch!='"' && ch!='\'' && ch !='&')
					*to++= *p++; /* copy alphamerics*/
		}
		*to++ = NULLCH;		/* terminate token string	*/
	}
	return(ntok);
}
