/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1986-2011 AT&T Intellectual Property          *
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
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * preprocessor error handler
 */

#include "pplib.h"

void
pperror(int level, ...)
{
	va_list	ap;

	va_start(ap, level);
	errorv(pp.pass, level, ap);
	va_end(ap);
}
