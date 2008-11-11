/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*                  Copyright (c) 1985-2005 AT&T Corp.                  *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                            by AT&T Corp.                             *
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
*                   Phong Vo <kpv@research.att.com>                    *
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * AT&T Research
 *
 * external mode_t representation support
 */

#ifndef _MODEX_H
#define _MODEX_H

#include <ast_fs.h>
#include <modecanon.h>

#if _BLD_ast && defined(__EXPORT__)
#define extern		__EXPORT__
#endif

extern int		modei(int);
extern int		modex(int);

#undef	extern

#if _S_IDPERM
#define modei(m)	((m)&X_IPERM)
#if _S_IDTYPE
#define modex(m)	(m)
#endif
#endif

#endif
