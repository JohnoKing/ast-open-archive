/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1985-2000 AT&T Corp.              *
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
*              David Korn <dgk@research.att.com>               *
*               Phong Vo <kpv@research.att.com>                *
*                                                              *
***************************************************************/
#pragma prototyped

#include <ast.h>

#if _lib_memmove

NoN(memmove)

#else

void*
memmove(void* to, const void* from, register size_t n)
{
	register char*	out = (char*)to;
	register char*	in = (char*)from;

	if (n <= 0)	/* works if size_t is signed or not */
		;
	else if (in + n <= out || out + n <= in)
		return(memcpy(to, from, n));	/* hope it's fast*/
	else if (out < in)
		do *out++ = *in++; while (--n > 0);
	else
	{
		out += n;
		in += n;
		do *--out = *--in; while(--n > 0);
	}
	return(to);
}

#endif
