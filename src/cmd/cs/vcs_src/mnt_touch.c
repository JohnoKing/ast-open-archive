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
#include "mnt_imount.h"

#define USAGE	"rm file ... # remove cached files"

int im_touch_help(s)
	char*	s;
{
	printf("\t%s\n", USAGE);
	return (0);
}

int im_touch(argc, argv)
	int		argc;
	register char**	argv;
{	
	register int	n;
	register char*	s;
	int		fd;
	
	error_info.id = argv[0];
	opt_info.index = 1;
	while (n = optget(argv, "s:[server] "))
	 switch (n)
	{
	case 's':
		s = opt_info.arg;
		if ((fd = csopen(s, CS_OPEN_READ)) < 0)
		{
			printf("cannot connect cs server %s\n", s);
			return (-1);
		}
		istate.cs_svc = strdup(s);
		istate.fd = fd;
		break;
	case '?':
	case ':':
		printf(USAGE);
		return (1);
	}
	
	argc -= opt_info.index;
	argv += opt_info.index;
	
	while(argc > 0)
	{
		s = *argv;
		unlink(s);
		argc--;
		argv++;
	}
	return (0);
}


