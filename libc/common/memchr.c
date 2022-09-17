

char *
memchr(s, c, n)
	register char *s;
	register c, n;
{
	while (--n >= 0)
		if (*s++ == c)
			return (--s);
	return (0);
}

