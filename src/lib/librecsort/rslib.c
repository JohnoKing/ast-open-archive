/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*           Copyright (c) 1996-2007 AT&T Knowledge Ventures            *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                      by AT&T Knowledge Ventures                      *
*                                                                      *
*                A copy of the License is available at                 *
*            http://www.opensource.org/licenses/cpl1.0.txt             *
*         (with md5 checksum 059e8cd6165cb4c31e351f2b69388fd9)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                   Phong Vo <kpv@research.att.com>                    *
*                 Glenn Fowler <gsf@research.att.com>                  *
*                                                                      *
***********************************************************************/
#pragma prototyped

#include "rskeyhdr.h"

#include <dlldefs.h>
#include <option.h>

typedef Rsdisc_t* (*Rslib_f)(Rskey_t*, const char*);

int
#if __STD_C
rslib(Rs_t* rs, Rskey_t* kp, const char* lib)
#else
rslib(rs, kp, lib)
Rs_t*		rs;
Rskey_t*	kp;
const char*	lib;
#endif
{
	register char*		s;
	void*			dll;
	Rsdisc_t*		disc;
	Rslib_f			fun;
	char			path[PATH_MAX];
	Opt_t			opt;

	static const char	symbol[] = "rs_disc";

	for (s = (char*)lib; *s && *s != ',' && *s != '\t' && *s != '\r' && *s != '\n'; s++);
	sfsprintf(path, sizeof(path), "%-.*s", s - (char*)lib, lib);
	if (!(dll = dllplug("sort", path, NiL, RTLD_LAZY, path, sizeof(path))))
	{
		if (kp->keydisc->errorf)
			(*kp->keydisc->errorf)(kp, kp->keydisc, 2, "%s: library not found", path);
		return -1;
	}
	if (!(fun = (Rslib_f)dlllook(dll, symbol)))
	{
		if (kp->keydisc->errorf)
			(*kp->keydisc->errorf)(kp, kp->keydisc, 2, "%s: %s: initialization function not found in library", path, symbol);
		return -1;
	}
	if (*s)
		s++;
	opt = opt_info;
	disc = (*fun)(kp, s);
	opt_info = opt;
	if (!disc)
		return -1;
	if (!kp->tail)
		kp->tail = kp->disc;
	kp->tail = kp->tail->disc = disc;
	rs->events |= disc->events;
	return 0;
}
