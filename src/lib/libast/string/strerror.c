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

/*
 * Glenn Fowler
 * AT&T Research
 *
 * return error message string given errno
 */

#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:hide strerror
#else
#define strerror	______strerror
#endif

#include <ast.h>

#if defined(__STDPP__directive) && defined(__STDPP__hide)
__STDPP__directive pragma pp:nohide strerror
#else
#undef	strerror
#endif

#if _lib_strerror

NoN(strerror)

#else

extern int	sys_nerr;
extern char*	sys_errlist[];

#if defined(__EXPORT__)
#define extern		__EXPORT__
#endif

extern char*
strerror(int err)
{
	static char	msg[28];

	if (err > 0 && err <= sys_nerr)
		return sys_errlist[err];
	sfsprintf(msg, sizeof(msg), "Error %d", err);
	return msg;
}

#endif
