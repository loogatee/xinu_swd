/* blkcopy.c - blkcopy */

/*------------------------------------------------------------------------
 *  blkcopy  -  copy a block of memory form one location to another
 *------------------------------------------------------------------------
 */
blkcopy(to, from, nbytes)
register char	*to;
register char	*from;
register int	nbytes;
{
	while (--nbytes >= 0)
		*to++ = *from++;
	return(1);
}
