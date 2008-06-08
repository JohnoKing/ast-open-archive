/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 2003-2008 AT&T Intellectual Property          *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
*                                                                      *
*                A copy of the License is available at                 *
*            http://www.opensource.org/licenses/cpl1.0.txt             *
*         (with md5 checksum 059e8cd6165cb4c31e351f2b69388fd9)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                   Phong Vo <kpv@research.att.com>                    *
*                                                                      *
***********************************************************************/
#include	"vctest.h"


main()
{
	char		*tar, *del, *t;
	Vcodex_t	*vc;
	ssize_t		k, n;

#if 1
	tar = "00000011111122222abc012000000111111222222012abc";
#else
	/* for debugging, here are the various data for "panama":
	** BW transform: \005npmaaaa
	** base[]: 'a' 0, 'm' 3, 'n' 4, 'p' 5
	** offset[]: 0 0 0 0 1 0 2
	*/
	tar = "panama";
#endif

	if(!(vc = vcopen(0, Vcbwt, 0, 0, VC_ENCODE)) )
		terror("Cannot open Vcbwt handle");

	if((n = vcapply(vc, tar, strlen(tar), &del)) <= 0)
		terror("Vcbwt failed");

	if(!(vc = vcopen(0, Vcbwt, 0, 0, VC_DECODE)) )
		terror("Cannot open Vcbwt handle");
	if((n = vcapply(vc, del, n, &t)) != strlen(tar) )
		terror("Vcunbwt returns wrong size");
	for(k = 0; k < n; ++k)
		if(t[k] != tar[k])
			terror("Vcunbwt computed bad byte");
	exit(0);
}
