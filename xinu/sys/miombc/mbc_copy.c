




CopyToEO(dstp, srcp, cnt)
register	char	*dstp;
register	char	*srcp;
register	int		cnt;
{
	register	int		i;

	for (i=0; i < cnt; ++i)
	{
		*dstp++ = *srcp++;
		dstp++;
	}
}


CopyFromEO(dstp, srcp, cnt)
register	char	*dstp;
register	char	*srcp;
register	int		cnt;
{
	register	int		i;

	for (i=0; i < cnt; ++i)
	{
		*dstp++ = *srcp++;
		srcp++;
	}
}
