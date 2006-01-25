/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*                  Copyright (c) 1985-2006 AT&T Corp.                  *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                            by AT&T Corp.                             *
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

#include <ast.h>

#if !defined(getgroups) && defined(_lib_getgroups)

NoN(getgroups)

#else

#include <error.h>

#if defined(getgroups)
#undef	getgroups
#define	ast_getgroups	_ast_getgroups
#define botched		1
extern int		getgroups(int, int*);
#else
#define ast_getgroups	getgroups
#endif

#if defined(__EXPORT__)
#define extern	__EXPORT__
#endif

extern int
ast_getgroups(int len, gid_t* set)
{
#if botched
#if NGROUPS_MAX < 1
#undef	NGROUPS_MAX
#define NGROUPS_MAX	1
#endif
	register int	i;
	int		big[NGROUPS_MAX];
#else
#undef	NGROUPS_MAX
#define NGROUPS_MAX	1
#endif
	if (!len) return(NGROUPS_MAX);
	if (len < 0 || !set)
	{
		errno = EINVAL;
		return(-1);
	}
#if botched
	len = getgroups(len > NGROUPS_MAX ? NGROUPS_MAX : len, big);
	for (i = 0; i < len; i++)
		set[i] = big[i];
	return(len);
#else
	*set = getgid();
	return(1);
#endif
}

#endif
