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
*                                                              *
***************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * write n bytes to fd, using multiple write(2) if necessary
 */

#include "cslib.h"

ssize_t
cswrite(register Cs_t* state, int fd, const void* buf, register size_t n)
{
	register char*		p = (char*)buf;
	register ssize_t	i;

	while (n > 0)
	{
		messagef((state->id, NiL, -9, "write(%d,%d) `%-.*s'", fd, n, n - 1, (n > 0 && *((char*)buf + n - 1) == '\n') ? (char*)buf : "..."));
		if ((i = write(fd, p, n)) <= 0)
		{
			messagef((state->id, NiL, -9, "write(%d,%d) [%d]", fd, n, i));
			if (i && p == (char*)buf)
				return i;
			break;
		}
		n -= i;
		p += i;
	}
	return p - (char*)buf;
}

ssize_t
_cs_write(int fd, const void* buf, size_t n)
{
	return cswrite(&cs, fd, buf, n);
}
