/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1990-2003 AT&T Corp.                *
*        and it may only be used by you under license from         *
*                       AT&T Corp. ("AT&T")                        *
*         A copy of the Source Code Agreement is available         *
*                at the AT&T Internet web site URL                 *
*                                                                  *
*       http://www.research.att.com/sw/license/ast-open.html       *
*                                                                  *
*    If you have copied or used this software without agreeing     *
*        to the terms of the license you are infringing on         *
*           the license and copyright and are violating            *
*               AT&T's intellectual property rights.               *
*                                                                  *
*            Information and Software Systems Research             *
*                        AT&T Labs Research                        *
*                         Florham Park NJ                          *
*                                                                  *
*               Glenn Fowler <gsf@research.att.com>                *
*                                                                  *
*******************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * output control mark on fd
 */

#include "cslib.h"

int
cscontrol(register Cs_t* state, int fd)
{
#if CS_LIB_SOCKET
	return send(fd, "", 1, MSG_OOB) == 1 ? 0 : -1;
#else
#if CS_LIB_STREAM
	struct strbuf	buf;

	buf.maxlen = 0;
	return putmsg(fd, NiL, &buf, RS_HIPRI);
#else
	return write(fd, "", 1) == 1 ? 0 : -1;
#endif
#endif
}

int
_cs_control(int fd)
{
	return cscontrol(&cs, fd);
}
