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
#ifndef _UWIN

int _STUB_err;

#else

#pragma prototyped

/*
 * bsd 4.4 compatibility
 *
 * NOTE: errorv(ERROR_NOID) => the first arg is the printf format
 */

#include <ast.h>
#include <error.h>

#include <windows.h>

#ifdef __EXPORT__
#define extern	__EXPORT__
#endif

static void
errmsg(int level, int code, const char* fmt, va_list ap)
{
	if (!error_info.id)
	{
		struct _astdll*	dp = _ast_getdll();
		char*		s;
		char*		t;

		if (s = dp->_ast__argv[0])
		{
			if (t = strrchr(s, '/'))
				s = t + 1;
			error_info.id = s;
		}
	}
	errorv(fmt, level|ERROR_NOID, ap);
	if ((level & ERROR_LEVEL) >= ERROR_ERROR)
		exit(code);
}

extern void
verr(int code, const char* fmt, va_list ap)
{
	errmsg(ERROR_ERROR|ERROR_SYSTEM, code, fmt, ap);
}

extern void
err(int code, const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	errmsg(ERROR_ERROR|ERROR_SYSTEM, code, fmt, ap);
	va_end(ap);
}

extern void
verrx(int code, const char* fmt, va_list ap)
{
	errmsg(ERROR_ERROR, code, fmt, ap);
}

extern void
errx(int code, const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	errmsg(ERROR_ERROR, code, fmt, ap);
	va_end(ap);
}

extern void
vwarn(const char* fmt, va_list ap)
{
	errmsg(ERROR_WARNING|ERROR_SYSTEM, 0, fmt, ap);
}

extern void
warn(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	errmsg(ERROR_WARNING|ERROR_SYSTEM, 0, fmt, ap);
	va_end(ap);
}

extern void
vwarnx(const char* fmt, va_list ap)
{
	errmsg(ERROR_WARNING, 0, fmt, ap);
}

extern void
warnx(const char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	errmsg(ERROR_WARNING, 0, fmt, ap);
	va_end(ap);
}

#endif
