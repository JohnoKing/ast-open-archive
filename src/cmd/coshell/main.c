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
 * AT&T Bell Laboratories
 *
 * remote coshell server main
 * except for history this program would
 * have been called shmux (with a TeX x)
 */

#include "service.h"

#include <namval.h>

#ifndef PATHCHECK
#define PATHCHECK	CO_ID
#endif

#define CO_OPT_COMMAND	"command"
#define CO_OPT_DUP	"dup"
#define CO_OPT_HOME	"home"

static char*		coshell[] =	/* shell to run on other side	*/
{
	"3d", "ksh", "sh"
};

State_t		state;			/* program state		*/

/*
 * initialize the server state
 */

static void*
init(void* handle, int fdmax)
{
	register int	n;
	register char*	s;

	NoP(handle);
	message((-1, "init pid=%d", getpid()));
	state.clock = state.start = state.toss = cs.time;
	for (n = 0; n < 10; n++) TOSS;
	state.fdtotal = fdmax;
	if (!(state.con = newof(0, Connection_t, state.fdtotal, 0)))
		error(3, "out of space [con]");
	state.con[0].type = POLL;
	if ((n = getgroups(0, NiL)) <= 0)
		n = NGROUPS_MAX;
	if (!(state.gids = newof(0, gid_t, n + 2, 0)))
		error(3, "out of space [gids]");
	if ((n = getgroups(n, state.gids + 1)) < 0)
		n = 0;
	state.gids[n + 1] = -1;
	state.gids[0] = getegid();
	state.uid = geteuid();
	n = state.fdtotal / 2;
	if (!(state.job = state.jobnext = newof(0, Cojob_t, n, 0)))
		error(3, "out of space [job]");
	state.jobmax = state.jobnext += n - 1;

	/*
	 * initialze the shell table
	 */

	state.busy = BUSY;
	state.grace = GRACE;
	state.pool = ((s = getenv(CO_ENV_PROC)) && *s) ? (int)strtol(s, NiL, 0) : POOL;
	state.profile = strdup("{ . ./.profile; eval test -f \\$ENV \\&\\& . \\$ENV; } >/dev/null 2>&1 </dev/null");
	if (!(state.home = search(DEF|NEW, csname(0), NiL, NiL)))
		error(3, "cannot get local host address");
	state.shell = state.shellnext = state.home;
	message((-1, "local name is %s", state.home->name));

	/*
	 * load the local net configuration
	 */

	do info(DEF|NEW, *state.argv); while (*state.argv && *++state.argv);

	/*
	 * load the access controls if any
	 */

	info(SET, NiL);

	/*
	 * set the job limits
	 */

	if (state.perserver <= 0 || state.perserver > n)
		state.perserver = n;
	if (state.peruser <= 0 && (state.peruser = (3 * (state.pool - 1)) / 2) <= 0)
		state.peruser = 1;
	if (state.percpu <= 0 && (state.percpu = state.peruser / 4) <= 2)
		state.percpu = 2;

	/*
	 * bias the local host so it can generate more work
	 */

	if (!state.indirect.con && state.home->idle)
	{
		state.home->idle = 0;
		if (!(state.home->flags & SETBIAS)) state.home->bias *= 4;
	}

	/*
	 * get the shell path
	 */

	if (!(s = state.sh))
	{
		s = state.buf;
		for (n = 0;;)
		{
			if (pathpath(s, coshell[n], NiL, PATH_ABSOLUTE|PATH_REGULAR|PATH_EXECUTE))
				break;
			if (++n >= elementsof(coshell))
				error(3, "shell not found");
		}
	}

	/*
	 * parameterize the shell path name on state.home->type
	 */

	pathrepl(s, state.home->type, "%s");
	if (!(state.sh = strdup(s)))
		error(3, "out of space [shell path]");
	message((-1, "parameterized shell path is %s", state.sh));

	/*
	 * set up the mesg and pump connect streams
	 */

	state.mesg = stream(MESG, "mesg");
	state.pump = stream(PUMP, "pump");

	/*
	 * open the local shell
	 */

	shellopen(state.home, 2);
	return((void*)&state);
}

/*
 * accept a new user connection
 * no id checks since we can trust the connect stream path access
 */

static int
user(void* handle, int fd, Cs_id_t* id, int clone, char** argv)
{
	NoP(handle);
	NoP(id);
	NoP(clone);
	NoP(argv);
	if (state.indirect.con)
	{
		state.con[fd].type = INIT;
		state.con[fd].info.user.fds[0] = fd;
		state.con[fd].info.user.fds[1] = -1;
		state.con[fd].info.user.fds[2] = -1;
		if (cswrite(fd, "#00000\n", 7) != 7) return(-1);
	}
	else state.con[fd].type = IDENT;
	state.con[fd].info.user.flags = USER_IDENT;
	state.con[fd].info.user.home = 0;
	state.con[fd].info.user.pump = 0;
	state.con[fd].info.user.expr = 0;
	return(0);
}

/*
 * service a read event
 */

static int
service(void* handle, register int fd)
{
	register int		n;
	register int		i;
	char*			s;
	char*			t;
	char*			x;
	char*			e;
	int			n1;
	int			n2;
	Coshell_t*		sp;
	Cojob_t*		jp;
	Cs_id_t			id;
	struct stat		st;
	struct stat		ts;
	int			fds[5];
	char			cmd[256];

	NoP(handle);
	switch (state.con[fd].type)
	{
	case ANON:
	case SHELL:
#ifdef O_NONBLOCK
		if (state.con[fd].flags >= 0 && fcntl(fd, F_SETFL, state.con[fd].flags|O_NONBLOCK) < 0)
			state.con[fd].flags = -1;
#endif
		n = csread(fd, s = state.buf, state.buflen, CS_LINE);
#ifdef O_NONBLOCK
		if (state.con[fd].flags >= 0 && fcntl(fd, F_SETFL, state.con[fd].flags) < 0)
			state.con[fd].flags = -1;
#endif
		if (n <= 0)
		{
#ifdef O_NONBLOCK
			if (n < 0 && errno == EAGAIN)
				state.con[fd].error++;
			else
#endif
			drop(fd);
			break;
		}
		s[n - 1] = 0;

		/*
		 * parse the message(s)
		 */

		sp = state.con[fd].info.shell;
		do
		{
			if (x = strchr(s, '\n')) *x++ = 0;
			while (isspace(*s)) s++;
			if (!(i = *s++)) continue;
			message((-3, "%s-message: %s", sp ? sp->name : "", s - 1));
			while (isspace(*s)) s++;
			if ((jp = state.job + (int)strtol(s, NiL, 0)) > state.jobmax) continue;
			while (*s && !isspace(*s)) s++;
			while (isspace(*s)) s++;

			/*
			 * now interpret the message
			 */

			switch (i)
			{
			case 'j':
				/*
				 * <s> is the job pid
				 */

				if (!sp) break;
				i = jp->pid;
				jp->pid = (int)strtol(s, NiL, 0);
				if (i == WARP) goto nuke;
				if (jp->cmd)
				{
					free(jp->cmd);
					jp->cmd = 0;
					state.jobwait--;
				}
				break;
			case 'n':
				/*
				 * <s> is the name of the anonymous shell and its pid
				 */

				if (!sp && (sp = search(GET, s, NiL, NiL)))
				{
					if (sp->fd < 0)
					{
						/*
						 * nuke the zombie that kicked this shell
						 */

						if (sp->fd != -1) waitpid(-sp->fd, NiL, 0);
						state.con[fd].type = SHELL;
						state.con[fd].info.shell = sp;
						while (*s && !isspace(*s)) s++;
						while (isspace(*s)) s++;
						sp->pid = (int)strtol(s, NiL, 0);
						sp->fd = fd;
						sp->open++;
						state.shellwait--;
						state.shells++;
					}
					else
					{
						sp = 0;
						drop(fd);
					}
				}
				break;
			case 'r':
				/*
				 * <s> is the shell rating
				 */

				if (!sp) break;
				sfsprintf(cmd, sizeof(cmd), "%s,%s", s, sp->name);
				search(DEF, cmd, NiL, NiL);
				break;
			case 'x':
				/*
				 * <s> is the job exit code and user,sys times
				 */

				if (!sp) break;
				jp->status = strtol(s, &t, 10);
				jp->sig = 0;
				for (;;)
				{
					if (t <= s) break;
					for (s = t; isalpha(*s) || isspace(*s); s++);
					jp->user += strelapsed(s, &t, CO_QUANT);
					if (t <= s) break;
					for (s = t; isalpha(*s) || isspace(*s); s++);
					jp->sys += strelapsed(s, &t, CO_QUANT);
				}
				if (jp->pid == START)
				{
					if (jp->cmd)
					{
						free(jp->cmd);
						jp->cmd = 0;
						state.jobwait--;
					}
					jp->pid = WARP;
				}
				else
				{
				nuke:
					/*
					 * nuke the zombies
					 */

					sfsprintf(cmd, sizeof(cmd), "wait %d\n", jp->pid);
					cswrite(fd, cmd, strlen(cmd));
					if (--jp->ref <= 0) jobdone(jp);
				}
				break;
			}
		} while (s = x);
		if (sp && sp->running)
			jobcheck(sp);
		break;
	case DEST:
		if (csread(fd, s = state.buf, 13, CS_EXACT) != 13 || s[0] != '#' || (jp = state.job + (int)strtol(s + 1, NiL, 10)) > state.jobmax || fstat(state.con[fd].info.pass.fd = (int)strtol(s + 7, NiL, 10), &st))
			drop(fd);
		else
		{
			state.con[fd].type = PASS;
			state.con[fd].info.pass.job = jp->pid ? jp : 0;
		}
		break;
	case IDENT:
		if ((i = csrecv(fd, &id, fds, elementsof(fds))) < 0) /* error */;
		else if (i < 3) errno = EBADF;
		else if (cswrite(fd, "#00000\n", 7) != 7) /* error */;
		else
		{
			if (i == 3)
			{
				close(fds[0]);
				fds[0] = n = fd;
				state.con[n].info.user.flags = 0;
			}
			else
			{
				drop(fd);
				csfd(n = fds[0], CS_POLL_READ);
				fds[0] = fds[3];
				state.con[n].info.user.flags = USER_IDENT;
				state.con[n].info.user.home = 0;
				state.con[n].info.user.pump = 0;
			}
			state.con[fds[0]].type = UCMD;
			state.con[fds[1]].type = UOUT;
			state.con[fds[2]].type = UERR;
			state.con[n].type = INIT;
			state.con[n].info.user.pid = id.pid;
			for (i = 0; i < elementsof(state.con[n].info.user.fds); i++)
			{
				fcntl(fds[i], F_SETFD, FD_CLOEXEC);
				state.con[n].info.user.fds[i] = fds[i];
			}
			break;
		}
		sfsprintf(state.buf, state.buflen, "#%05d\n", errno);
		cswrite(fd, state.buf, 7);
		drop(fd);
		while (--i >= 0)
			close(fds[i]);
		break;
	case INIT:
		if (csread(fd, s = cmd, 7, CS_EXACT) != 7 || s[0] != '#' || (i = (int)strtol(s + 1, NiL, 10)) && (i < 0 || i > state.buflen || csread(fd, state.buf, i, CS_EXACT) != i))
		{
			drop(fd);
			break;
		}
		state.con[fd].info.user.attr.label[0] = 0;
		x = 0;
		if (i)
		{
			s = state.buf;
			s[i] = 0;
			while (e = strchr(s, '\n'))
			{
				*e++ = 0;
				if (strneq(s, CO_ENV_ATTRIBUTES, sizeof(CO_ENV_ATTRIBUTES) - 1) && s[sizeof(CO_ENV_ATTRIBUTES) - 1] == '=')
				{
					x = s + sizeof(CO_ENV_ATTRIBUTES);
					if (*x == '\'')
					{
						i = 1;
						s = t = ++x;
						while (*s = *t++)
						{
							if (*s == '\'') i = !i;
							else if (i || *s != '\\') s++;
							else if (!(*s++ = *t++)) break;
						}
					}
				}
				else if (state.indirect.con)
				{
					if (streq(s, CO_OPT_COMMAND))
						state.con[fd].info.user.flags &= ~USER_IDENT;
					else if (streq(s, CO_OPT_DUP))
						state.con[fd].info.user.flags |= USER_DUP;
					else if (strneq(s, CO_OPT_HOME, sizeof(CO_OPT_HOME) - 1) && s[sizeof(CO_OPT_HOME) - 1] == '=' && (sp = search(GET, s + sizeof(CO_OPT_HOME), NiL, NiL)))
						(state.con[fd].info.user.home = sp)->home++;
					else if (strneq(s, CO_OPT_INDIRECT, sizeof(CO_OPT_INDIRECT) - 1) && s[sizeof(CO_OPT_INDIRECT) - 1] == '=')
						state.con[fd].info.user.pump = strdup(s + sizeof(CO_OPT_INDIRECT));
				}
				s = e;
			}
		}
		if (state.indirect.con && (state.con[fd].info.user.flags & (USER_IDENT|USER_INIT)) == USER_IDENT)
		{
			state.con[fd].info.user.flags |= USER_INIT;
			break;
		}
		if (x) state.con[fd].info.user.expr = strdup(x);
		attributes(x, &state.con[fd].info.user.attr, NiL);
		state.con[fd].info.user.attr.set &= ~SETLABEL;
		if (state.indirect.con)
		{
			if (!state.con[fd].info.user.pump ||
				(state.con[fd].info.user.fds[1] = csopen(state.con[fd].info.user.pump, 0)) < 0 ||
				cswrite(state.con[fd].info.user.fds[1], "#00001\n", 7) != 7)
			{
				drop(fd);
				break;
			}
			else state.con[state.con[fd].info.user.fds[1]].type = UOUT;
			if (state.con[fd].info.user.flags & USER_DUP) state.con[fd].info.user.fds[2] = state.con[fd].info.user.fds[1];
			else if ((state.con[fd].info.user.fds[2] = csopen(state.con[fd].info.user.pump, 0)) < 0 ||
				cswrite(state.con[fd].info.user.fds[2], "#00002\n", 7) != 7)
			{
				drop(fd);
				break;
			}
			else state.con[state.con[fd].info.user.fds[2]].type = UERR;
		}
		else if (!fstat(state.con[fd].info.user.fds[1], &st) && !fstat(state.con[fd].info.user.fds[2], &ts) && st.st_ino == ts.st_ino && st.st_dev == ts.st_dev)
		{
			drop(state.con[fd].info.user.fds[2]);
			state.con[fd].info.user.fds[2] = state.con[fd].info.user.fds[1];
		}
		if (state.con[fd].info.user.flags & USER_IDENT)
		{
			s = state.buf;
			s += sfsprintf(s, state.buflen - (s - state.buf), "%s,%s", CO_OPT_SERVER, CO_OPT_ACK);
			if (state.indirect.con) s += sfsprintf(s, state.buflen - (s - state.buf), ",%s", CO_OPT_INDIRECT);
			s += sfsprintf(s, state.buflen - (s - state.buf), "\n");
			i = s - state.buf;
			if (cswrite(state.con[fd].info.user.fds[0], state.buf, i) != i)
			{
				drop(fd);
				break;
			}
		}
		state.con[fd].info.user.running = 0;
		state.con[fd].info.user.total = 0;
		state.con[fd].type = USER;
		state.users++;
		break;
	case MESG:
		if (csrecv(fd, &id, fds, 1) == 1)
		{
			i = fds[0];
			n = strlen(corinit);
			if (cswrite(i, corinit, n) == n)
			{
				state.con[i].type = ANON;
				state.con[i].info.shell = 0;
#ifdef O_NONBLOCK
				state.con[i].flags = fcntl(fd, F_GETFL, 0);
#endif
				csfd(i, CS_POLL_READ);
			}
			else close(i);
		}
		break;
	case PASS:
		if ((i = read(fd, state.buf, state.buflen)) <= 0)
			drop(fd);
		else
		{	
			if (state.identify)
			{
				n = state.con[fd].info.pass.fd;
				jp = state.con[fd].info.pass.job;
				if (state.con[n].info.ident.shell != jp->shell || state.con[n].info.ident.pid != jp->pid)
				{
					state.con[n].info.ident.shell = jp->shell;
					state.con[n].info.ident.pid = jp->pid;
					sfprintf(state.string, "%s", jp->shell->name);
					if (*(s = state.con[jp->fd].info.user.attr.label))
						sfprintf(state.string, " %s", s);
					if (*(s = state.con[fd].info.pass.job->label))
						sfprintf(state.string, " %s", s);
					else sfprintf(state.string, " %d", jp->pid);
					n = sfsprintf(cmd, sizeof(cmd) - 1, state.identify, sfstruse(state.string));
					cswrite(state.con[fd].info.pass.fd, cmd, n);
				}
			}
			if (cswrite(state.con[fd].info.pass.fd, state.buf, i) != i)
				drop(fd);
		}
		break;
	case PUMP:
		if (csrecv(fd, &id, fds, 1) == 1)
		{
			i = fds[0];
			state.con[i].type = DEST;
			csfd(i, CS_POLL_READ);
		}
		break;
	case SCHED:
		if ((i = read(fd, state.buf, state.buflen)) <= 0)
			drop(fd);
		/*HERE*/
		break;
	case USER:
		if (csread(fd, cmd, 7, CS_EXACT) != 7 || cmd[0] != '#' || (n = (int)strtol(cmd + 1, NiL, 10)) <= 0)
		{
			drop(fd);
			break;
		}
		if (n > state.buflen)
		{
			state.buflen = roundof(n, CHUNK);
			if (!(state.buf = newof(state.buf, char, state.buflen, 0)))
				error(3, "out of space [buf]");
		}
		if (csread(fd, state.buf, n, CS_EXACT) != n)
		{
			drop(fd);
			break;
		}
		state.buf[n - 1] = 0;
		state.cmds++;
		n = error_info.errors;
		if (!(state.home = state.con[fd].info.user.home))
			state.home = state.shell;
		switch (i = *state.buf)
		{
		case 'e':
		case 'E':
			shellexec(NiL, state.buf, fd);
			n = error_info.errors;
			break;
		case 'k':
		case 'K':
			if (tokscan(state.buf, NiL, "%s %d %d ", NiL, &n1, &n2) != 3)
				error_info.errors++;
			else
			{
				message((-1, "kill state.con=%d rid=%d sig=%d", fd, n1, n2));
				for (jp = state.job; jp <= state.jobmax; jp++)
					if (jp->fd == fd && jp->pid && (!n1 || jp->rid == n1))
					{
						jobkill(jp, n2);
						if (n1) break;
					}
				n = error_info.errors;
			}
			break;
		case 's':
		case 'S':
			if (tokscan(state.buf, NiL, "%s %s %s %d %s", NiL, &s, &t, &n1, &x) != 5)
				error_info.errors++;
			else server(fd, *s, *t, n1, x);
			break;
		default:
			error(ERROR_OUTPUT|2, state.con[fd].info.user.fds[2], "%c: unknown op", i);
			break;
		}
		state.home = state.shell;
		if (isupper(i))
		{
			n = sfsprintf(state.buf, state.buflen, "a 1 %d\n", error_info.errors != n);
			cswrite(state.con[fd].info.user.fds[0], state.buf, n);
		}
		break;
	}
	return(0);
}

/*
 * wake up to check for hung shells and jobs on busy hosts
 */

static int
wakeup(void* handle)
{
	NoP(handle);
	shellcheck();
	jobcheck(NiL);
	return(0);
}

/*
 * indirect coshell initialization
 */

static void*
indirect(void* handle, int fdmax)
{
	int*	pass;

	NoP(handle);
	if (!(pass = newof(0, int, fdmax, 0)))
		error(3, "out of space [pass]");
	csfd(state.indirect.con, CS_POLL_READ);
	pass[state.indirect.con] = state.indirect.msg;
	csfd(state.indirect.cmd, CS_POLL_READ);
	pass[state.indirect.cmd] = state.indirect.con;
	return((void*)pass);
}

/*
 * indirect coshell data pump
 */

static int
pump(void* handle, register int fd)
{
	register int	n;
	register int	pd;
	register int*	pass = (int*)handle + fd;
	register char*	s = state.buf;

	if ((n = read(fd, s, state.buflen)) <= 0) goto drop;
	if (!(pd = *pass))
	{
		if ((n -= 7) < 0 || s[0] != '#' || (pd = (int)strtol(s + 1, NiL, 10)) < 1 || pd > 2) goto drop;
		*pass = pd == 1 ? state.indirect.out : state.indirect.err;
		if (!n) return(0);
	}
	if (cswrite(pd, s, n) != n) goto drop;
	return(0);
 drop:
	if (fd == state.indirect.cmd) exit(0);
	if (fd = *pass)
	{
		close(fd);
		*pass = 0;
	}
	return(-1);
}

/*
 * coshell main
 */

int
main(int argc, char** argv)
{
	register int		n;
	register int		fd;
	register int		i;
	int			pfd;
	int			d;
	char*			s;
	char*			t;
	int			fds[4];

	NoP(argc);
	setlocale(LC_ALL, "");
	opt_info.argv = argv;
	error_info.id = CO_ID;
	if (pathcheck(PATHCHECK, error_info.id, &state.check))
		exit(1);
	error(-1, "%s", version);
	if (!(state.string = sfstropen()))
		error(3, "out of space [string]");
	state.buflen = 8 * CHUNK;
	if (!(state.buf = newof(0, char, state.buflen, 0)))
		error(3, "out of space [buf]");

	/*
	 * check for alternate connect stream
	 */

	if (*argv && (s = *++argv) && strmatch(s, "/dev/(fdp|tcp)/*")) argv++;
	else if (!(t = getenv(CO_ENV_SHELL)) || tokscan(t, NiL, " %s %s ", NiL, &s) != 2)
	{
		if ((fd = csopen(t = "/dev/fdp", 0)) >= 0) close(fd);
		else t = "/dev/tcp";
		sfsprintf(s = state.buf, state.buflen, "%s/local/%s/user", t, CO_ID);
	}
	if (!strmatch(s, "/dev/fdp/*")) state.indirect.con = 1;
	error(-1, "connect stream is %s", s);
	if (!(state.service = strdup(s)))
		error(3, "out of space [service]");

	/*
	 * check for alternate stdin
	 */

	if ((s = *argv) && strneq(s, "/dev/fd/", 8))
	{
		argv++;
		if (i = (int)strtol(s + 8, NiL, 0))
		{
			close(0);
			if (dup(i))
				error(ERROR_SYSTEM|3, "%s: cannot open", s);
		}
	}

	/*
	 * COMMAND	interactive or argv command processor to server (-*)
	 * DEFER	pass fds to the server and exit (no args or /dev/fd/n)
	 * SERVER	the server (+*)
	 */

	if ((s = strrchr(*opt_info.argv, '.')) && streq(s + 1, CS_SVC_SUFFIX))
	{
		n = SERVER;
		argv--;
	}
	else if (!(s = *argv)) n = DEFER;
	else if (s[0] == '+' && !s[1]) n = SERVER;
	else if (s[0] == '-' && s[1] != '?') n = COMMAND;
	else error(ERROR_USAGE|4, "[connect-stream] [-hjqQs[aelpst]] [-r host [cmd]] | + [info]");
	if (n != SERVER)
	{
		if ((fd = csopen(state.service, CS_OPEN_TEST)) < 0)
		{
			if (errno == ENOENT) error(3, "%s: server not running", state.service);
			else error(ERROR_SYSTEM|3, "%s: cannot open connect stream", state.service);
		}
		if (!state.indirect.con)
		{
			for (i = 0; i < elementsof(fds); i++) fds[i] = i;
			i = elementsof(fds) - (n == COMMAND);
		}
		errno = EINVAL;
		s = state.buf;
		if ((state.indirect.con || !cssend(fd, fds, i)) && csread(fd, s, 7, CS_EXACT) == 7 && s[0] == '#' && !(errno = (int)strtol(s + 1, NiL, 10))) do
		{
			if (state.indirect.con)
			{
				if ((pfd = csopen(t = "/dev/tcp/local/normal/slave", CS_OPEN_CREATE)) < 0)
					error(ERROR_SYSTEM|3, "%s: cannot create pump connect stream", t);
			}
			else if (n != COMMAND) exit(0);
			s += 7;
			s += sfsprintf(s, state.buflen - (s - state.buf), "%s=label=%s", CO_ENV_ATTRIBUTES, CO_ID);
			if ((t = getenv(CO_ENV_ATTRIBUTES)) && *t) s += sfsprintf(s, state.buflen - (s - state.buf), ",%s", t);
			if (state.indirect.con)
			{
				struct stat	st;
				struct stat	ts;

				s += sfsprintf(s, state.buflen - (s - state.buf), "\n%s=%s", CO_OPT_INDIRECT, cspath(pfd, 0));
				s += sfsprintf(s, state.buflen - (s - state.buf), "\n%s=%s", CO_OPT_HOME, csname(0));
				if (!fstat(1, &st) && !fstat(2, &ts) && st.st_ino == ts.st_ino && st.st_dev == ts.st_dev)
				{
					s += sfsprintf(s, state.buflen - (s - state.buf), "\n%s", CO_OPT_DUP);
					d = 1;
				}
				else d = 2;
				if (n == COMMAND) s += sfsprintf(s, state.buflen - (s - state.buf), "\n%s", CO_OPT_COMMAND);
			}
			s += sfsprintf(s, state.buflen - (s - state.buf), "\n");
			i = s - state.buf;
			s = state.buf;
			sfsprintf(s, 7, "#%05d\n", i - 7);
			if (cswrite(fd, s, i) != i) break;
			if (n == COMMAND)
			{
				if (state.indirect.con)
				{
					for (; d > 0 && csrecv(pfd, NiL, fds, 1) == 1 && csread(fds[0], s, 7, CS_EXACT) == 7 && s[0] == '#'; d--)
						if ((n = (int)strtol(s + 1, NiL, 10)) == 1) state.indirect.out = fds[0];
						else if (n == 2) state.indirect.err = fds[0];
						else break;
					if (d) break;
					state.indirect.con = pfd;
				}
				exit(command(fd, (*argv)[1] ? argv : (char**)0));
			}
			if ((state.indirect.cmd = dup(0)) < 0) break;
			close(0);
			if (dup(pfd)) break;
			close(pfd);
			csfd(state.indirect.cmd, 0);
			csfd(state.indirect.con = fd, 0);
			csfd(state.indirect.out = 1, 0);
			csfd(state.indirect.err = 2, 0);
			csfd(state.indirect.msg = 3, 0);
			csserve(NiL, NiL, indirect, NiL, NiL, pump, NiL, NiL);
		} while (0);
		error(ERROR_SYSTEM|3, "%s: cannot connect to server", state.service);
	}
	state.argv = argv + 1;

	/*
	 * we are the server
	 */

	csserve(NiL, state.service, init, NiL, user, service, NiL, wakeup);
	/*NOTREACHED*/
	return 1;
}
