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
 * slip into the background
 * (1<<fd) bits not in fds will be modified:
 *	0-2 dup'd to /dev/null
 *	3-sysconf(_SC_OPEN_MAX) closed
 *
 * daemon details thanks to
 *	"UNIX Network Programming, W. Richard Stevens, Prentice Hall, 1990"
 */

#include "cslib.h"

#include <sig.h>

int
csdaemon(register Cs_t* state, int fds)
{
	register int	fd;
	register int	i;
	int		oerrno;

	messagef((state->id, NiL, -8, "daemon(%o) call", fds));
	oerrno = errno;

	/*
	 * unconditionally ignore some signals
	 */

	signal(SIGHUP, SIG_IGN);
#ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif

	/*
	 * generate some children if not scheduled from init
	 */

	if (getppid() > 1)
	{
#if _lib_fork
		if (!(state->flags & CS_DAEMON_SLAVE)) switch (fork())
		{
		case -1:
			return -1;
		case 0:
			systrace(NiL);
			break;
		default:
			/*
			 * allow some time for service setup
			 */

			sleep(2);
			exit(0);
		}
#endif

		/*
		 * become process group leader and drop control tty
		 */

		setsid();
	}

	/*
	 * redirect {0,1,2} to /dev/null if not in fds
	 */

	if ((fds & ((1<<0)|(1<<1)|(1<<2))) != ((1<<0)|(1<<1)|(1<<2)) && (fd = open("/dev/null", O_RDWR)) >= 0)
	{
		fds |= (1<<fd);
		for (i = 0; i <= 2; i++)
			if (!(fds & (1<<i)))
			{
				close(i);
				dup(fd);
			}
		close(fd);
	}

	/*
	 * close any other garbage fds
	 */

	for (i = 3; i < 20; i++)
		if (!(fds & (1<<i)) && !fcntl(i, F_GETFD, 0))
			close(i);

	/*
	 * avoid unexpected permission problems
	 */

	umask(0);

	/*
	 * no command-relative relative root searching
	 */

	pathpath(NiL, NiL, "", 0);

	/*
	 * we ignored a lot of errors to get here
	 */

	errno = oerrno;
	messagef((state->id, NiL, -8, "daemon(%o) = 0", fds));
	return 0;
}

int
_cs_daemon(int fds)
{
	return csdaemon(&cs, fds);
}
