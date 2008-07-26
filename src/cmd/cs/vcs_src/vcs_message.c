/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*           Copyright (c) 1990-2006 AT&T Knowledge Ventures            *
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
*                 Glenn Fowler <gsf@research.att.com>                  *
*                                                                      *
***********************************************************************/
#pragma prototyped

#include "vcs_rscs.h"

extern int	debug;

static void tracev(int level, va_list ap)
{
	char*	format;

	if (!level || debug && level <= debug)
	{
		if (level)
			sfprintf(sfstdout, "[%d] ", level);
		format = va_arg(ap, char*);
		sfvprintf(sfstdout, format, ap);
		sfprintf(sfstdout, "\n");
	}
}

void trace(int level, ...)
{
	va_list		ap;
	
	va_start(ap, level);
	tracev(level, ap);
	va_end(ap);
}
