/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1990-2011 AT&T Intellectual Property          *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
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
*                                                                      *
***********************************************************************/
/*
 * File: ifs_cserrno.h
 */
#define E_NIL		0	/* nil */
#define E_COMMAND	301	/* unimplement command */
#define E_ARGUMENT	302	/* invalid arguments */
#define E_MOUNT		401	/* unmounted path */
#define E_PROTOCOL	402	/* unknown protocol */
#define E_OPENDEST	403	/* dest-file open error */
#define E_GETHOST	404	/* unknown hostname */
#define E_SOCKET	405	/* can't open stream socket */
#define E_CONNECT	406	/* can't connect to server */
#define E_USERAUTH	407	/* user authentication error */
#define E_DATAXFER	408	/* data transfer error */

