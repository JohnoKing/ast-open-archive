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

#ifdef pathconf3d

long
pathconf3d(const char* path, int op)
{
	register char*	sp;
	long		r;
	int		oerrno;
#if FS
	Mount_t*	mp;

	if (!fscall(NiL, MSG_pathconf, 0, path, op))
		return(state.ret);
	mp = monitored();
#endif
	if (!(sp = pathreal(path, 0, NiL)))
		return(-1);
	oerrno = errno;
	errno = 0;
	r = PATHCONF(sp, op);
	if (!errno)
	{
		errno = oerrno;
#if FS
		if (mp)
			fscall(mp, MSG_pathconf, 0, sp, op);
		for (mp = state.global; mp; mp = mp->global)
			if (fssys(mp, MSG_pathconf))
				fscall(mp, MSG_pathconf, 0, sp, op);
#endif
	}
	return(r);
}

#else

NoN(pathconf)

#endif
