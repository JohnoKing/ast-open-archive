/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1999-2003 AT&T Corp.                *
*        and it may only be used by you under license from         *
*                       AT&T Corp. ("AT&T")                        *
*         A copy of the Source Code Agreement is available         *
*                at the AT&T Internet web site URL                 *
*                                                                  *
*       http://www.research.att.com/sw/license/ast-open.html       *
*                                                                  *
*    If you have copied or used this software without agreeing     *
*        to the terms of the license you are infringing on         *
*           the license and copyright and are violating            *
*               AT&T's intellectual property rights.               *
*                                                                  *
*            Information and Software Systems Research             *
*                        AT&T Labs Research                        *
*                         Florham Park NJ                          *
*                                                                  *
*               Glenn Fowler <gsf@research.att.com>                *
*                                                                  *
*******************************************************************/
#include	"sftest.h"


MAIN()
{
	int	i;
	char	wbuf[1023];
	char	rbuf[1023];
	Sfio_t	*fp;

	for(i = 0; i < sizeof(wbuf); ++i)
		wbuf[i] = (i%26)+'a';
	wbuf[sizeof(wbuf)-1] = '\0';

	if(!(fp = sftmp(0)))
		terror("Opening temp file\n");

	for(i = 0; i < 256; ++i)
		if(sfwrite(fp,wbuf,sizeof(wbuf)) != sizeof(wbuf))
			terror("Writing\n");

	sfseek(fp,(Sfoff_t)0,0);
	sfset(fp,SF_WRITE,0);
	sfsetbuf(fp,NIL(char*),0);
	sfsetbuf(fp,NIL(char*),(size_t)SF_UNBOUND);

	for(i = 0; i < 256; ++i)
	{	if(sfread(fp,rbuf,sizeof(rbuf)) != sizeof(rbuf))
			terror("Reading\n");

		if(strcmp(rbuf,wbuf) != 0)
			terror("Unmatched record\n");
	}

	TSTEXIT(0);
}
