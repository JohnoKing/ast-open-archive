/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1989-2008 AT&T Intellectual Property          *
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

#ifdef	read3d

ssize_t
read3d(int fd, void* buf, size_t n)
{
	register ssize_t 	r;
#if FS
	Mount_t*		mp;
	off_t			off;
	int			pos = 0;
	
	if (!fscall(NiL, MSG_read, 0, fd, buf, n))
		return(state.ret);
	mp = monitored();
#endif
	r = READ(fd, buf, n);
#if FS
	if (r >= 0)
	{
		if (mp)
		{
			if (!pos)
			{
				pos = 1;
				off = LSEEK(fd, 0, SEEK_CUR) - r;
			}
			fscall(mp, r, MSG_read3d, fd, buf, n, off);
		}
		for (mp = state.global; mp; mp = mp->global)
			if (fssys(mp, MSG_read))
			{
				if (!pos)
				{
					pos = 1;
					off = LSEEK(fd, 0, SEEK_CUR) - r;
				}
				fscall(mp, MSG_read3d, r, fd, buf, n, off);
			}
	}
#endif
	return(r);
}

#else

NoN(read)

#endif
