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
 * Glenn Fowler
 * AT&T Bell Laboratories
 *
 * return strperm() expression for perm
 */

#include <ast.h>
#include <ls.h>

char*
fmtperm(register int perm)
{
	register char*	s;

	static char	buf[32];

	s = buf;

	/*
	 * u
	 */

	*s++ = 'u';
	*s++ = '=';
	if (perm & S_ISVTX) *s++ = 't';
	if (perm & S_ISUID) *s++ = 's';
	if (perm & S_IRUSR) *s++ = 'r';
	if (perm & S_IWUSR) *s++ = 'w';
	if (perm & S_IXUSR) *s++ = 'x';
	if ((perm & (S_ISGID|S_IXGRP)) == S_ISGID) *s++ = 'l';

	/*
	 * g
	 */

	*s++ = ',';
	*s++ = 'g';
	*s++ = '=';
	if ((perm & (S_ISGID|S_IXGRP)) == (S_ISGID|S_IXGRP)) *s++ = 's';
	if (perm & S_IRGRP) *s++ = 'r';
	if (perm & S_IWGRP) *s++ = 'w';
	if (perm & S_IXGRP) *s++ = 'x';

	/*
	 * o
	 */

	*s++ = ',';
	*s++ = 'o';
	*s++ = '=';
	if (perm & S_IROTH) *s++ = 'r';
	if (perm & S_IWOTH) *s++ = 'w';
	if (perm & S_IXOTH) *s++ = 'x';
	*s = 0;
	return(buf);
}
