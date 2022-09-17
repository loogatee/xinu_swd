/* ttyiin.c ttyiin, ttybreak, erase1, eputc, echoch */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>

LOCAL	erase1(), echoch(), eputc();

unsigned short	savePS;

int	ttyi_err;
int	ttyi_cerr;


/*------------------------------------------------------------------------
 *  ttyiin  --  lower-half tty device driver for input interrupts
 *------------------------------------------------------------------------
 */
INTPROC	ttyiin(devm)
long	devm;
{
	STATWORD ps;    
    
	register struct	tty	*iptr;
	register int	ch;
	int	ct;

	iptr = &tty[devm];

	/*
	 *	Call "system-specific" portion of ttyiin, in order to
	 *	read the character.  Error info is returned in err.
	 */
	ch = ttyiin_ssp(iptr);

	if (iptr->imode == IMRAW) {
		if (iptr->icnt >= IBUFLEN){
			return;
		}
		iptr->ibuff[iptr->ihead++] = ch | ttyi_cerr;
		++iptr->icnt;
		if (iptr->ihead	>= IBUFLEN)	/* wrap buffer pointer	*/
			iptr->ihead = 0;
	        signal(iptr->isem);
	} else {				/* cbreak | cooked mode	*/
		if ( ch	== RETURN && iptr->icrlf )
			ch = NEWLINE;
		if (iptr->iintr && ch == iptr->iintrc) {
			send(iptr->iintpid, INTRMSG);
			eputc(ch, iptr);
			return;
		}
		if (iptr->oflow) {
			if (ch == iptr->ostart)	{
				iptr->oheld = FALSE;
				ttyostart(iptr);
				return;
			}
			if (ch == iptr->ostop) {
				iptr->oheld = TRUE;
				return;
			}
		}
		iptr->oheld = FALSE;
		if (iptr->imode	== IMCBREAK) {		/* cbreak mode	*/
			if (iptr->icnt >= IBUFLEN) {
				eputc(iptr->ifullc,iptr);
				return;
			}
			iptr->ibuff[iptr->ihead++] = ch	| ttyi_cerr;
			if (iptr->ihead	>= IBUFLEN)
				iptr->ihead = 0;
			if (iptr->iecho)
				echoch(ch,iptr);
			if (iptr->icnt < IBUFLEN) {
				++iptr->icnt;
				signal(iptr->isem);
			}
		} else {				/* cooked mode	*/
			if (ch == iptr->ikillc && iptr->ikill) {
				iptr->ihead -= iptr->icursor;
				if (iptr->ihead	< 0)
					iptr->ihead += IBUFLEN;
				iptr->icursor =	0;
				eputc(RETURN,iptr);
				eputc(NEWLINE,iptr);
				return;
			}
			if (ch == iptr->ierasec	&& iptr->ierase) {
				if (iptr->icursor > 0) {
					iptr->icursor--;
					erase1(iptr);
				}
				return;
			}
			if (ch == NEWLINE || ch == RETURN ||
				(iptr->ieof && ch == iptr->ieofc)) {
				if (iptr->iecho) {
					echoch(ch,iptr);
					if (ch == iptr->ieofc)
						echoch(NEWLINE,iptr);
				}
				iptr->ibuff[iptr->ihead++] = ch | ttyi_cerr;
				if (iptr->ihead	>= IBUFLEN)
					iptr->ihead = 0;
				ct = iptr->icursor+1; /* +1 for \n or \r*/
				iptr->icursor =	0;
				iptr->icnt += ct;
				signaln(iptr->isem,ct);
				return;
			}
			ct = iptr->icnt;
			ct = ct	< 0 ? 0	: ct;
			if ((ct	+ iptr->icursor) >= IBUFLEN-1) {
				eputc(iptr->ifullc,iptr);
				return;
			}
			if (iptr->iecho)
				echoch(ch,iptr);
			iptr->icursor++;
			iptr->ibuff[iptr->ihead++] = ch	| ttyi_cerr;
			if (iptr->ihead	>= IBUFLEN)
				iptr->ihead = 0;
		}
	}
}

/*------------------------------------------------------------------------
 *  erase1  --  erase one character honoring erasing backspace
 *------------------------------------------------------------------------
 */
LOCAL erase1(iptr)
	struct	tty   *iptr;
{
	char	ch;

	if (--(iptr->ihead) < 0)
		iptr->ihead += IBUFLEN;
	ch = iptr->ibuff[iptr->ihead];
	if (iptr->iecho) {
		if (ch < BLANK || ch == 0177) {
			if (iptr->evis)	{
				eputc(BACKSP,iptr);
				if (iptr->ieback) {
					eputc(BLANK,iptr);
					eputc(BACKSP,iptr);
				}
			}
			eputc(BACKSP,iptr);
			if (iptr->ieback) {
				eputc(BLANK,iptr);
				eputc(BACKSP,iptr);
			}
		} else {
			eputc(BACKSP,iptr);
			if (iptr->ieback) {
				eputc(BLANK,iptr);
				eputc(BACKSP,iptr);
			}
		}
	} 
        else
            ttyostart(iptr);
}




/*------------------------------------------------------------------------
 *  echoch  --  echo a character with visual and ocrlf options
 *------------------------------------------------------------------------
 */
LOCAL echoch(ch, iptr)
	char	ch;		/* character to	echo			*/
	struct	tty   *iptr;	/* pointer to I/O block for this devptr	*/
{
	if ((ch==NEWLINE||ch==RETURN)&&iptr->ecrlf) {
		eputc(RETURN,iptr);
		eputc(NEWLINE,iptr);
	} else if ((ch<BLANK||ch==0177) && iptr->evis) {
		eputc(UPARROW,iptr);
		eputc(ch+0100,iptr);	/* make it printable	*/
	} else {
		eputc(ch,iptr);
	}
}

/*------------------------------------------------------------------------
 *  eputc - put one character in the echo queue
 *------------------------------------------------------------------------
 */
LOCAL eputc(ch,iptr)
	char	ch;
	struct	tty   *iptr;
{
	iptr->ebuff[iptr->ehead++] = ch;
	if (iptr->ehead	>= EBUFLEN)
		iptr->ehead = 0;
	ttyostart(iptr);
}


/*------------------------------------------------------------------------
 *  ttybreak -- handle a break received on the serial line
 *------------------------------------------------------------------------
 */
ttybreak(dev)
{
    STATWORD ps;
    
    disable(ps);
    kprintf("\n\nSerial line BREAK detected, dev = %d\n",dev);
	if (dev == 0)
		ret_mon();
    restore(ps);
    return;
}
