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
	Sfio_t*	f1;
	Sfio_t* f2;
	char*	s;

	if(!(f1 = sfopen(NIL(Sfio_t*), sftfile(0),"w+")) )
		terror("Can't open file\n");
	if(sfwrite(f1,"0123456789\n",11) != 11)
		terror("Can't write to file\n");

	sfclose(sfstdin);
	if(sfswap(f1,sfstdin) != sfstdin)
		terror("Can't swap with sfstdin\n");
	sfseek(sfstdin,(Sfoff_t)0,0);
	if(!(s = sfgetr(sfstdin,'\n',1)) )
		terror("sfgetr failed\n");
	if(strcmp(s,"0123456789") != 0)
		terror("Get wrong data\n");

	if(!(f1 = sfswap(sfstdin,NIL(Sfio_t*))) )
		terror("Failed swapping to NULL\n");
	if(!sfstack(sfstdout,f1) )
		terror("Failed stacking f1\n");

	if(!(f2 = sfopen(NIL(Sfio_t*), sftfile(0), "r")) )
		terror("Can't open for read\n");

	if(sfswap(f1,f2) != NIL(Sfio_t*) )
		terror("sfswap should have failed\n");

	sftcleanup();
	return 0;
}
