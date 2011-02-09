/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1989-2011 AT&T Intellectual Property          *
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
*                   Eduardo Krell <ekrell@adexus.cl>                   *
*                                                                      *
***********************************************************************/
#pragma prototyped

#include "3d.h"

#ifdef	getdents3d

ssize_t
getdents3d(int fd, void* buf, size_t n)
{
	ssize_t		r;

#if FS
	register Mount_t*	mp;

	if (!fscall(NiL, MSG_getdents, 0, fd, buf, n))
		return(state.ret);
	mp = monitored();
#endif
	if ((r = GETDENTS(fd, buf, n)) >= 0)
	{
#if FS
		if (mp)
			fscall(mp, MSG_getdents, r, fd, buf, n);
		for (mp = state.global; mp; mp = mp->global)
			if (fssys(mp, MSG_getdents))
				fscall(mp, MSG_getdents, r, fd, buf, n);
#endif
	}
	return(r);
}

#else

NoN(getdents)

#endif
