/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1989-2000 AT&T Corp.              *
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
 * xargs/tw command arg list interface definitions
 */

#ifndef _CMDARG_H
#define _CMDARG_H

#define CMD_EMPTY	(1<<0)		/* run once, even if no args	*/
#define CMD_IGNORE	(1<<1)		/* ignore EXIT_QUIT exit	*/
#define CMD_INSERT	(1<<2)		/* argpat for insertion		*/
#define CMD_MINIMUM	(1<<3)		/* argmax is a minimum		*/
#define CMD_NEWLINE	(1<<4)		/* echo separator is newline	*/
#define CMD_POST	(1<<5)		/* argpat is post arg position	*/
#define CMD_QUERY	(1<<6)		/* trace and query each command	*/
#define CMD_TRACE	(1<<7)		/* trace each command		*/

#define CMD_USER	(1<<12)

typedef struct				/* cmd + args info		*/
{
	struct
	{
	size_t		args;		/* total args			*/
	size_t		commands;	/* total commands		*/
	}		total;

	int		argcount;	/* current arg count		*/
	int		argmax;		/* max # args			*/
	int		echo;		/* just an echo			*/
	int		flags;		/* CMD_* flags			*/
	int		insertlen;	/* strlen(insert)		*/
	int		offset;		/* post arg offset		*/

	char**		argv;		/* exec argv			*/
	char**		firstarg;	/* first argv file arg		*/
	char**		insertarg;	/* argv before insert		*/
	char**		postarg;	/* start of post arg list	*/
	char**		nextarg;	/* next argv file arg		*/
	char*		nextstr;	/* next string ends before here	*/
	char*		laststr;	/* last string ends before here	*/
	char*		insert;		/* replace with current arg	*/
	char		buf[1];		/* argv and arg buffer		*/
} Cmdarg_t;

extern Cmdarg_t*	cmdopen(char**, int, int, const char*, int);
extern int		cmdflush(Cmdarg_t*);
extern int		cmdarg(Cmdarg_t*, const char*, int);
extern int		cmdclose(Cmdarg_t*);

#endif
