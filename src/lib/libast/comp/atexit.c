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
 * ANSI C atexit()
 * arrange for func to be called LIFO on exit()
 */

#include <ast.h>

#if _lib_atexit

NoN(atexit)

#else

#if _lib_onexit || _lib_on_exit

#if !_lib_onexit
#define onexit		on_exit
#endif

extern int		onexit(void(*)(void));

int
atexit(void (*func)(void))
{
	return(onexit(func));
}

#else

struct list
{
	struct list*	next;
	void		(*func)(void);
};

static struct list*	funclist;

extern void		_exit(int);

int
atexit(void (*func)(void))
{
	register struct list*	p;

	if (!(p = newof(0, struct list, 1, 0))) return(-1);
	p->func = func;
	p->next = funclist;
	funclist = p;
	return(0);
}

void
_ast_atexit(void)
{
	register struct list*	p;

	while (p = funclist)
	{
		funclist = p->next;
		(*p->func)();
	}
}

#if _std_cleanup

#if _lib__cleanup
extern void		_cleanup(void);
#endif

void
exit(int code)
{
	_ast_atexit();
#if _lib__cleanup
	_cleanup();
#endif
	_exit(code);
}

#else

void
_cleanup(void)
{
	_ast_atexit();
}

#endif

#endif

#endif
