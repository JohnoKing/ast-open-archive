/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1985-2010 AT&T Intellectual Property          *
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
*                 Glenn Fowler <gsf@research.att.com>                  *
*                  David Korn <dgk@research.att.com>                   *
*                   Phong Vo <kpv@research.att.com>                    *
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Bell Laboratories
 *
 * return base b representation for n
 * if p!=0 then base prefix is included
 * otherwise if n==0 or b==0 then output is signed base 10
 */

#include <ast.h>

char*
fmtbase(intmax_t n, int b, int p)
{
	char*	buf;
	int	z;

	if (!p)
	{
		if (!n)
			return "0";
		if (!b)
			return fmtint(n, 0);
		if (b == 10)
			return fmtint(n, 1);
	}
	buf = fmtbuf(z = 72);
	sfsprintf(buf, z, p ? "%#..*I*u" : "%..*I*u", b, sizeof(n), n);
	return buf;
}

#if __OBSOLETE__ < 20140101

#undef	fmtbasell

char*
fmtbasell(intmax_t n, int b, int p)
{
	return fmtbase(n, b, p);
}

#endif
