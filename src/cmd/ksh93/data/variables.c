/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1982-2000 AT&T Corp.              *
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
*              David Korn <dgk@research.att.com>               *
*                                                              *
***************************************************************/
#pragma prototyped

#include	<ast.h>
#include	"FEATURE/options"
#include	"FEATURE/dynamic"
#include	<shell.h>
#include	"shtable.h"
#include	"name.h"

/*
 * This is the list of built-in shell variables and default values
 * and default attributes.
 */

const struct shtable2 shtab_variables[] =
{
	"PATH",		0,				(char*)0,
	"PS1",		0,				(char*)0,
	"PS2",		NV_NOFREE, 			"> ",
	"IFS",		NV_NOFREE, 			" \t\n",
	"PWD",		0,				(char*)0,
	"HOME",		0,				(char*)0,
	"MAIL",		0,				(char*)0,
	"REPLY",	0,				(char*)0,
	"SHELL",	NV_NOFREE,			"/bin/sh",
	"EDITOR",	0,				(char*)0,
	"MAILCHECK",	NV_NOFREE|NV_INTEGER,		(char*)0,
	"RANDOM",	NV_NOFREE|NV_INTEGER,		(char*)0,
	"ENV",		0,				(char*)0,
	"HISTFILE",	0,				(char*)0,
	"HISTSIZE",	0,				(char*)0,
	"HISTEDIT",	NV_NOFREE,			(char*)0,
	"HISTCMD",	NV_NOFREE|NV_INTEGER,		(char*)0,
	"FCEDIT",	NV_NOFREE,			"/bin/ed",
	"CDPATH",	0,				(char*)0,
	"MAILPATH",	0,				(char*)0,
	"PS3",		NV_NOFREE, 			"#? ",
	"OLDPWD",	0,				(char*)0,
	"VISUAL",	0,				(char*)0,
	"COLUMNS",	0,				(char*)0,
	"LINES",	0,				(char*)0,
	"PPID",		NV_NOFREE|NV_INTEGER,		(char*)0,
	"_",		NV_EXPORT,			(char*)0,
	"TMOUT",	NV_NOFREE|NV_INTEGER,		(char*)0,
	"SECONDS",	NV_NOFREE|NV_INTEGER|NV_DOUBLE,	(char*)0,
	"LINENO",	NV_NOFREE|NV_INTEGER,		(char*)0,
	"OPTARG",	0,				(char*)0,
	"OPTIND",	NV_NOFREE|NV_INTEGER,		(char*)0,
	"PS4",		0,				(char*)0,
	"FPATH",	0,				(char*)0,
	"LANG",		0,				(char*)0,
	"LC_ALL",	0,				(char*)0,
	"LC_COLLATE",	0,				(char*)0,
	"LC_CTYPE",	0,				(char*)0,
	"LC_MESSAGES",	0,				(char*)0,
	"LC_NUMERIC",	0,				(char*)0,
	"FIGNORE",	0,				(char*)0,
	".sh",		NV_TABLE|NV_RDONLY|NV_NOFREE|NV_NOPRINT,(char*)0,
	".sh.edchar",	0,				(char*)0,
	".sh.edcol",	0,				(char*)0,
	".sh.edtext",	0,				(char*)0,
	".sh.edmode",	0,				(char*)0,
	".sh.name",	0,				(char*)0,
	".sh.subscript",0,				(char*)0,
	".sh.value",	0,				(char*)0,
	".sh.version",	NV_NOFREE,			(char*)(&e_version[5]),
	".sh.dollar",	0,				(char*)0,
#ifdef SHOPT_FS_3D
	"VPATH",	0,				(char*)0,
#endif /* SHOPT_FS_3D */
#ifdef SHOPT_MULTIBYTE
	"CSWIDTH",	0,				(char*)0,
#endif /* SHOPT_MULTIBYTE */
#ifdef apollo
	"SYSTYPE",	0,				(char*)0,
#endif /* apollo */
	"",	0,					(char*)0
};

