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
 * AT&T Research
 *
 * magic interface definitions
 */

#ifndef _MAGIC_H
#define _MAGIC_H

#include <sfio.h>
#include <ls.h>

#define MAGIC_VERSION	19961031L

#ifndef MAGIC_FILE
#define MAGIC_FILE	"lib/file/magic"
#endif

#ifndef MAGIC_DIR
#define MAGIC_DIR	"lib/file"
#endif

#define MAGIC_FILE_ENV	"MAGICFILE"

#define MAGIC_MIME	(1<<0)		/* magictype returns MIME type	*/
#define MAGIC_VERBOSE	(1<<1)		/* verbose magic file errors	*/

#define MAGIC_USER	(1L<<16)	/* first user flag bit		*/

struct Magic_s;
struct Magicdisc_s;

typedef int (*Magicerror_f)(struct Magic_s*, struct Magicdisc_s*, int, const char*, ...);

typedef struct Magicdisc_s
{
	unsigned long	version;	/* interface version		*/
	unsigned long	flags;		/* MAGIC_* flags		*/
	Magicerror_f	errorf;		/* error function		*/
} Magicdisc_t;

typedef struct Magic_s
{
	const char*	id;		/* library id string		*/

#ifdef _MAGIC_PRIVATE_
	_MAGIC_PRIVATE_
#endif

} Magic_t;

#if _BLD_ast && defined(__EXPORT__)
#define extern		__EXPORT__
#endif

extern Magic_t*		magicopen(Magicdisc_t*);
extern int		magicload(Magic_t*, const char*, unsigned long);
extern int		magiclist(Magic_t*, Sfio_t*);
extern char*		magictype(Magic_t*, Sfio_t*, const char*, struct stat*);
extern int		magicclose(Magic_t*);

#undef	extern

#endif
