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
 * syscall message implementation interface
 */

#ifndef _MSGLIB_H
#define _MSGLIB_H

#define CS_INTERFACE	2

#include <ast.h>

#include "cs_lib.h"

#include <msg.h>
#include <ast_dir.h>
#include <errno.h>
#include <ls.h>

#ifndef D_FILENO
#define D_FILENO(d)	(1)
#endif

#ifndef errno
extern int	errno;
#endif

#endif
