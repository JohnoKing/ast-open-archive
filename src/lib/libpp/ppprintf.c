/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1986-2000 AT&T Corp.              *
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
*                                                              *
***************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * preprocessor printf using ppputchar() buffering
 */

#include "pplib.h"

int
ppprintf(char* format, ...)
{
	va_list	ap;
	Sfio_t*	sp;

	if (!(sp = sfnew(NiL, pp.outp, MAXTOKEN, -1, SF_WRITE|SF_STRING)))
		error(3, "temporary buffer allocation error");
	va_start(ap, format);
	sfvprintf(sp, format, ap);
	va_end(ap);
	pp.outp += sfseek(sp, 0L, SEEK_CUR);
	ppcheckout();
	sfclose(sp);
	return(0);
}
