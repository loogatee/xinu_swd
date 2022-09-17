



bcopy(src, dst, length)
	register char *src, *dst;
	register int length;
{
	if (length && src != dst)
		if ((unsigned)dst < (unsigned)src)
			if (((int)src | (int)dst | length) & 3)
				do	/* copy by bytes */
					*dst++ = *src++;
				while (--length);
			else {
				length >>= 2;
				do	/* copy by longs */
					*((long *)dst)++ = *((long *)src)++;
				while (--length);
			}
		else {			/* copy backwards */
			src += length;
			dst += length;
			if (((int)src | (int)dst | length) & 3)
				do	/* copy by bytes */
					*--dst = *--src;
				while (--length);
			else {
				length >>= 2;
				do	/* copy by shorts */
					*--((long *)dst) = *--((long *)src);
				while (--length);
			}
		}
	return(0);
}
