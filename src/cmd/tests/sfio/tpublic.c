/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1999-2000 AT&T Corp.              *
*      and it may only be used by you under license from       *
*                     AT&T Corp. ("AT&T")                      *
*       A copy of the Source Code Agreement is available       *
*              at the AT&T Internet web site URL               *
*                                                              *
*     http://www.research.att.com/sw/license/ast-open.html     *
*                                                              *
*     If you received this software without first entering     *
*       into a license with AT&T, you have an infringing       *
*           copy and cannot use it without violating           *
*             AT&T's intellectual property rights.             *
*                                                              *
*               This software was created by the               *
*               Network Services Research Center               *
*                      AT&T Labs Research                      *
*                       Florham Park NJ                        *
*                                                              *
*             Glenn Fowler <gsf@research.att.com>              *
*                                                              *
***************************************************************/
#include	"sftest.h"

main()
{
	Sfio_t*	f;
	Sfio_t*	g;
	char*	s;
	char	buf[1024];
	int	n, i;

	if(!(f = sfopen(NIL(Sfio_t*), sftfile(0), "w")) )
		terror("Can't open file to write\n");
	sfputr(f,"1111",'\n');
	sfputr(f,"2222",'\n');
	sfputr(f,"3333",'\n');
	sfputr(f,"4444",'\n');

	if(!(f = sfopen(f, sftfile(0), "r")) )
		terror("Can't open file to read1\n");
	if(!(g = sfnew(NIL(Sfio_t*),NIL(Void_t*),SF_UNBOUND,sffileno(f),SF_READ)) )
		terror("Can't open file to read2\n");

	sfset(f,SF_SHARE|SF_PUBLIC,1);
	sfset(g,SF_SHARE|SF_PUBLIC,1);

	if(!(s = sfgetr(f,'\n',1)) || strcmp(s,"1111") != 0)
		terror("Wrong data1\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"2222") != 0)
		terror("Wrong data2\n");
	sfsync(g);
	if(!(s = sfgetr(f,'\n',1)) || strcmp(s,"3333") != 0)
		terror("Wrong data3\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"4444") != 0)
		terror("Wrong data4\n");
	sfsync(g);

	sfclose(f);
	sfclose(g);
	if(!(f = sfopen(NIL(Sfio_t*), sftfile(0), "r+")) )
		terror("Can't open file to write2\n");
	if(!(g = sfnew(NIL(Sfio_t*),NIL(char*),-1,sffileno(f),SF_READ)) )
		terror("Can't open file to read3\n");

	sfset(f,SF_SHARE|SF_PUBLIC,1);
	sfset(g,SF_SHARE|SF_PUBLIC,1);

	if(sfputr(f,"1111",'\n') <= 0)
		terror("bad write1\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"2222") != 0)
		terror("Wrong data5\n");
	sfsync(g);
	if(sfputr(f,"3333",'\n') <= 0)
		terror("bad write2\n");
	sfsync(f);
	if(!(s = sfgetr(g,'\n',1)) || strcmp(s,"4444") != 0)
		terror("Wrong data6\n");
	sfsync(g);

	if(!(f = sfopen(f, sftfile(0), "w")) )
		terror("Can't open file to write3\n");

	for(i = 0; i < sizeof(buf); ++i)
		buf[i] = 0;
	for(i = 0; i < 256; ++i)
		if(sfwrite(f,buf,sizeof(buf)) != sizeof(buf))
			terror("Writing buffer0\n");

	for(i = 0; i < sizeof(buf); ++i)
		buf[i] = 1;
	for(i = 0; i < 256; ++i)
		if(sfwrite(f,buf,sizeof(buf)) != sizeof(buf))
			terror("Writing buffer1\n");

	if(!(f = sfopen(f, sftfile(0), "r")) )
		terror("Can't open file to read3\n");
	sfset(f,SF_SHARE|SF_PUBLIC,1);

	for(n = 0; n < 256; ++n)
	{	if(!(s = sfreserve(f,sizeof(buf),0)) )
			terror("Can't reserve buffer1\n");
		for(i = 0; i < sizeof(buf); ++i)
			if(s[i] != 0)
				terror("Bad data1\n");
	}

	for(n = 0; n < 256; ++n)
	{	if(!(s = sfreserve(f,sizeof(buf),0)) )
			terror("Can't reserve buffer2\n");
		for(i = 0; i < sizeof(buf); ++i)
			if(s[i] != 1)
				terror("Bad data2\n");
	}

	if((s = sfreserve(f,1,0)) )
		terror("Reading beyond eof\n");

	if(!(f = sfopen(f, sftfile(0), "w")) )
		terror("Can't open to write\n");
	if(sfwrite(f,"aaa\nbbb\nccc\n",12) != 12)
		terror("Can't write\n");
	sfclose(f);
	if(sfopen(sfstdin,sftfile(0),"r") != sfstdin)
		terror("Can't open file as sfstdin\n");
	if((n = (int)sfmove(sfstdin,NIL(Sfio_t*),(Sfoff_t)SF_UNBOUND,'\n')) != 3)
		terror("sfmove wrong number of lines %d\n",n);
	if(sfseek(sfstdin,(Sfoff_t)0,0) != 0)
		terror("Can't seek back to 0\n");
	if((n = (int)sfmove(sfstdin,NIL(Sfio_t*),(Sfoff_t)2,'\n')) != 2)
		terror("sfmove2 wrong number of lines %d\n",n);
	if((n = (int)sfmove(sfstdin,NIL(Sfio_t*),(Sfoff_t)SF_UNBOUND,'\n')) != 1)
		terror("sfmove3 wrong number of lines %d\n",n);

	sftcleanup();
	return 0;
}
