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
#include	<signal.h>

static int	Write_error = 0;

#if __STD_C
static int except(Sfio_t* f, int type, Void_t* obj, Sfdisc_t* disc)
#else
static int except(f, type, obj, disc)
Sfio_t*	f;
int	type;
Void_t* obj;
Sfdisc_t* disc;
#endif
{
	if(type == SF_WRITE)
		Write_error = 1;
	return 0;
}

static Sfdisc_t Wdisc = {NIL(Sfread_f), NIL(Sfwrite_f), NIL(Sfseek_f), except};

main()
{
	int	fd[2];
	Sfio_t	*r, *w;
	char*	s;
	void(*	handler)_ARG_((int));

	signal(SIGPIPE,SIG_IGN);

	if(pipe(fd) < 0)
		terror("Opening pipes\n");

	if(!(w = sfnew(NIL(Sfio_t*),NIL(Void_t*),(size_t)SF_UNBOUND,
			fd[1],SF_WRITE)) )
		terror("Opening write stream\n");
	if(!(r = sfnew(NIL(Sfio_t*),NIL(Void_t*),(size_t)SF_UNBOUND,
			fd[0],SF_READ)) )
		terror("Opening read stream\n");

	sfdisc(w,&Wdisc);

	if(sfputr(w,"abc",'\n') != 4)
		terror("sfputr failed\n");
	if(!(s = sfgetr(r,'\n',1)) || strcmp(s,"abc") != 0)
		terror("sfgetr failed\n");

	if(sfclose(r) < 0)
		terror("sfclose failed closing read stream\n");

	if(sfputr(w,"def",'\n') != 4)
		terror("sfputr failed2\n");

	if(Write_error)
		terror("Write exception should not have been raised\n");

	if(sfclose(w) >= 0)
		terror("sfclose should have failed closing write stream\n");

	if(!Write_error)
		terror("Write exception did not get raised\n");

	signal(SIGPIPE,SIG_DFL);
	if((w = sfpopen(NIL(Sfio_t*), "head -10", "w+")) )
	{	int		i;

		if((handler = signal(SIGPIPE,SIG_IGN)) == SIG_DFL ||
		   handler == SIG_IGN)
			terror("Bad signal handler for SIGPIPE\n");
		signal(SIGPIPE,handler);

		Write_error = 0;
		sfdisc(w,&Wdisc);

		for(i = 0; i < 100; ++i)
			if(sfputr(w, "abc",'\n') != 4)
				terror("Writing to coprocess1\n");

		sfsync(w);
		sfset(w,SF_READ,1);
		i = sffileno(w);
		sfset(w,SF_WRITE,1);
		if (i != sffileno(w))
			close(sffileno(w));

		for(i = 0; i < 10; ++i)
			if(!(s = sfgetr(w,'\n',1)) || strcmp(s,"abc") != 0)
				terror("Reading coprocess [%s]\n", s);
		if((s = sfgetr(w,'\n',1)) )
			terror("sfgetr should have failed\n");

		if(sfputr(w, "abc",'\n') != 4)
			terror("Writing to coprocess2\n");

		if(Write_error)
			terror("Write exception should not have been raised yet\n");

		if(sfclose(w) < 0)
			terror("sfclose should have returned an exit status\n");

		if(!Write_error)
			terror("Write exception should have been raised\n");
	}

	if(signal(SIGPIPE,SIG_DFL) != SIG_DFL)
		terror("SIGPIPE handler should have been SIG_DFL\n");

	/* test for stdio signal handling behavior */
	if((w = sfpopen((Sfio_t*)(-1), "head -10", "w+")) )
		if((handler = signal(SIGPIPE,SIG_DFL)) != SIG_DFL)
			terror("SIGPIPE handler should have been SIG_DFL\n");

	return 0;
}
