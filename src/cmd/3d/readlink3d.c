/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1990-2000 AT&T Corp.              *
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
*               Eduardo Krell <ekrell@adexus.cl>               *
*                                                              *
***************************************************************/
#pragma prototyped

#include "3d.h"

ssize_t
readlink3d(const char* path, char* buf, register size_t size)
{
	size_t		r;
#if FS
	Mount_t*	mp;
#endif

	if (state.in_2d)
		return(READLINK(path, buf, size));
#if FS
	if (!fscall(NiL, MSG_readlink, 0, path, buf, size))
		return(state.ret);
	mp = monitored();
#endif
	if (!pathreal(path, P_READLINK, NiL))
		return(-1);

	/*
	 * see if link text is already in memory
	 */

	if (r = state.path.linksize)
	{
		if (r > state.path.linksize)
			r = state.path.linksize;
		memcpy(buf, state.path.linkname, r);
#if FS
		if (mp) fscall(mp, MSG_readlink, r, path, buf, size);
		for (mp = state.global; mp; mp = mp->global)
			if (fssys(mp, MSG_readlink))
				fscall(mp, MSG_readlink, r, path, buf, size);
#endif
		return(r);
	}

	/*
	 * exists but not a symbolic link
	 */

	errno = EINVAL;
	return(-1);
}
