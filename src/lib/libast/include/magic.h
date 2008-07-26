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

typedef struct Magicdisc_s
{
	unsigned long	version;	/* interface version		*/
	unsigned long	flags;		/* MAGIC_* flags		*/
	Error_f		errorf;		/* error function		*/
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
