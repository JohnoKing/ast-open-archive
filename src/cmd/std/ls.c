/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1989-2004 AT&T Corp.                *
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
 * Glenn Fowler
 * AT&T Research
 *
 * ls -- list file status
 */

static const char usage[] =
"[-?\n@(#)$Id: ls (AT&T Labs Research) 2004-03-28 $\n]"
USAGE_LICENSE
"[+NAME?ls - list files and/or directories]"
"[+DESCRIPTION?For each directory argument \bls\b lists the contents; for each"
"	file argument the name and requested information are listed."
"	The current directory is listed if no file arguments appear."
"	The listing is sorted by file name by default, except that file"
"	arguments are listed before directories.]"
"[+?\bgetconf PATH_RESOLVE\b determines how symbolic links are handled. This"
"	can be explicitly overridden by the \b--logical\b, \b--metaphysical\b,"
"	and \b--physical\b options below. \bPATH_RESOLVE\b can be one of:]{"
"		[+logical?Follow all symbolic links.]"
"		[+metaphysical?Follow command argument symbolic links,"
"			otherwise don't follow.]"
"		[+physical?Don't follow symbolic links.]"
"}"

"[a:all?List entries starting with \b.\b; turns off \b--almost-all\b.]"
"[A:almost-all?List all entries but \b.\b and \b..\b; turns off \b--all\b.]"
"[b:escape?Print escapes for nongraphic characters.]"
"[B:ignore-backups?Do not list entries ending with ~.]"
"[c:ctime?Sort by change time; list ctime with \b--long\b.]"
"[C:multi-column?List entries by columns.]"
"[d:directory?List directory entries instead of contents.]"
"[D:define?Define \akey\a with optional \avalue\a. \avalue\a will be expanded"
"	when \b%(\b\akey\a\b)\b is specified in \b--format\b. \akey\a may"
"	override internal \b--format\b identifiers.]:[key[=value]]]"
"[e:decimal-scale|thousands?Scale sizes to powers of 1000 { b K M G T }.]"
"[E:block-size?Use \ablocksize\a blocks.]#[blocksize]"
"[f:format?Append to the listing format string. \aformat\a follows"
"	\bprintf\b(3) conventions, except that \bsfio\b(3) inline ids"
"	are used instead of arguments:"
"	%[-+]][\awidth\a[.\aprecis\a[.\abase\a]]]]]](\aid\a[:\asubformat\a]])\achar\a."
"	If \achar\a is \bs\b then the string form of the item is listed,"
"	otherwise the corresponding numeric form is listed. \asubformat\a"
"	overrides the default formatting for \aid\a. Supported \aid\as"
"	and \asubformat\as are:]:[format]{"
"		[+atime?access time]"
"		[+blocks?size in blocks]"
"		[+ctime?change time]"
"		[+device?device number]"
"		[+devmajor?major device number]"
"		[+devminor?minor device number]"
"		[+dir.blocks?directory blocks]"
"		[+dir.bytes?directory size in bytes]"
"		[+dir.count?directory entry count]"
"		[+dir.files?directory file count]"
"		[+dir.octets?directory size in octets]"
"		[+flags?command line flags in effect]"
"		[+gid?group id]"
"		[+header?listing header]"
"		[+ino?serial number]"
"		[+linkop?link operation: => for symbolic, == for hard]"
"		[+linkname?symbolic link text]"
"		[+linkpath?symbolic link text]"
"		[+mark?file or directory mark character]"
"		[+markdir?directory mark character]"
"		[+mode?access mode]"
"		[+mtime?modification time]"
"		[+name?entry name]"
"		[+nlink?hard link count]"
"		[+path?file path from original root dir]"
"		[+perm?access permissions]"
"		[+size?file size in bytes]"
"		[+summary?listing summary info]"
"		[+total.blocks?running total block count]"
"		[+total.bytes?running total size in bytes]"
"		[+total.files?running total file count]"
"		[+total.octets?running total size in octets]"
"		[+trailer?listing trailer]"
"		[+uid?owner id]"
"		[+----?subformats ----]"
"		[+case\b::\bp\b\a1\a::\bs\b\a1\a::...::\bp\b\an\a::\bs\b\an\a?Expands"
"			to \bs\b\ai\a if the value of \aid\a matches the shell"
"			pattern \bp\b\ai\a, or the empty string if there is no"
"			match.]"
"		[+mode?The integral value as a \bfmtmode\b(3) string.]"
"		[+perm?The integral value as a \bfmtperm\b(3) string.]"
"		[+time[=\aformat\a]]?The integral value as a \bstrftime\b(3)"
"			string. For example,"
"			\b--format=\"%8(mtime)u %(ctime:time=%H:%M:%S)s\"\b"
"			lists the mtime in seconds since the epoch and the"
"			ctime as hours:minutes:seconds.]"
"	}"
"[F:classify?Append a character for typing each entry.]"
"[g?\b--long\b with no owner info.]"
"[G:group?\b--long\b with no group info.]"
"[h:scale|binary-scale|human-readable?Scale sizes to powers of 1024 { b K M G T }.]"
"[i:inode?List the file serial number.]"
"[I:ignore?Do not list implied entries matching shell \apattern\a.]:[pattern]"
"[k:kilobytes?Use 1024 blocks instead of 512.]"
"[K:shell-quote?Enclose entry names in shell $'...' if necessary.]"
"[l:long|verbose?Use a long listing format.]"
"[m:commas|comma-list?List names as comma separated list.]"
"[n:numeric-uid-gid?List numeric user and group ids instead of names.]"
"[N:literal?Print raw entry names (don't treat e.g. control characters specially).]"
"[o?\b--long\b with no group info.]"
"[O:owner?\b--long\b with no owner info.]"
"[p:markdir?Append / to each directory name.]"
"[q:hide-control-chars?Print ? instead of non graphic characters.]"
"[Q:quote-name?Enclose all entry names in \"...\".]"
"[J:quote-style|quoting-style?Quote entry names according to \astyle\a:]:[style:=question]{"
"	[c:C?C \"...\" quote.]"
"	[e:escape?\b\\\b escape if necessary.]"
"	[l:literal?No quoting.]"
"	[q:question?Replace unprintable characters with \b?\b.]"
"	[s:shell?Shell $'...' quote if necessary.]"
"	[S:shell-always?Shell $'...' every name.]"
"}"
"[r:reverse?Reverse order while sorting.]"
"[R:recursive?List subdirectories recursively.]"
"[s:size?Print size of each file, in blocks.]"
"[S:bysize?Sort by file size.]"
"[t:?Sort by modification time; list mtime with \b--long\b.]"
"[T:tabsize?Ignored by this implementation.]#[columns]"
"[u:access?Sort by last access time; list atime with \b--long\b.]"
"[U?Equivalent to \b--sort=none\b.]"
"[V:colors|colours?\akey\a determines when color is used to distinguish"
"	types:]:?[key:=never]{"
"		[n:never?Never use color.]"
"		[a:always?Always use color.]"
"		[t:tty|auto?Use color when output is a tty.]"
"}"
"[w:width?\ascreen-width\a is the current screen width.]#[screen-width]"
"[W:time?Display \akey\a time instead of the modification time:]:[key]{"
"	[a:atime|access|use?access time]"
"	[c:ctime|status?status change time]"
"	[m:mtime|time?modify time]"
"}"
"[x:across?List entries by lines instead of by columns.]"
"[X:extension?Sort alphabetically by entry extension.]"
"[y:sort?Sort by \akey\a:]:?[key]{"
"	[a:atime|access|use?Access time.]"
"	[c:ctime|status?Status change time.]"
"	[x:extension?File name extension.]"
"	[m:mtime|time?Modify time.]"
"	[f:name?File name.]"
"	[n:none?Don't sort.]"
"	[s:size|blocks?File size.]"
"}"
"[Y:layout?Listing layout \akey\a:]:[key]{"
"	[a:across|horizontal?Multi-column across the page.]"
"	[c:comma?Comma separated names across the page.]"
"	[l:long|verbose?Long listing.]"
"	[v:multi-column|vertical?Multi-column by column.]"
"	[1:single-column?One column down the page.]"
"}"
"[z:time-style?List the time according to \astyle\a:]:[style]{"
"	[i:iso?Equivalent to \b+%K\b.]"
"	[10:posix-iso?No change for the C or posix locales, \biso\b otherwise.]"
"	[f:full-iso?Equivalent to \b+%Y-%m-%d+%H:%M:%S.000000%z\b.]"
"	[11:posix-full-iso?No change for the C or posix locales, \bfull-iso\b"
"		otherwise.]"
"	[l:locale?Equivalent to \b+%c\b.]"
"	[12:+\aformat\a?A \bdate\b(1) +\aformat\a.]"
"}"
"[1:one-column?List one file per line.]"
"[L:logical|follow?Follow symbolic links. The default is determined by"
"	\bgetconf PATH_RESOLVE\b.]"
"[H:metaphysical?Follow command argument symbolic links, otherwise don't"
"	follow. The default is determined by \bgetconf PATH_RESOLVE\b.]"
"[P:physical?Don't follow symbolic links. The default is determined by"
"	\bgetconf PATH_RESOLVE\b.]"
"[101:dump?Print the generated \b--format\b string on the standard output"
"	and exit.]"
"[102:full-time?Equivalent to \b--time-style=full-iso\b.]"
"[103:testdate?\b--format\b time values newer than \adate\a will be printed"
"	as \adate\a. Used for regression testing.]:[date]"

"\n"
"\n[ file ... ]\n"
"\n"
"[+SEE ALSO?\bchmod\b(1), \bfind\b(1), \bgetconf\b(1), \btw\b(1)]"
"[+BUGS?Can we add options to something else now?]"
;

#include <ast.h>
#include <ls.h>
#include <ctype.h>
#include <error.h>
#include <ftwalk.h>
#include <sfdisc.h>
#include <sfstr.h>
#include <hash.h>
#include <tm.h>

#define LS_ACROSS	(LS_USER<<0)	/* multi-column row order	*/
#define LS_ALL		(LS_USER<<1)	/* list all			*/
#define LS_ALWAYS	(LS_USER<<2)	/* always quote			*/
#define LS_COLUMNS	(LS_USER<<3)	/* multi-column column order	*/
#define LS_COMMAS	(LS_USER<<4)	/* comma separated name list	*/
#define LS_DIRECTORY	(LS_USER<<5)	/* list directories as files	*/
#define LS_ESCAPE	(LS_USER<<6)	/* C escape unprintable chars	*/
#define LS_EXTENSION	(LS_USER<<7)	/* sort by name extension	*/
#define LS_LABEL	(LS_USER<<8)	/* label for all dirs		*/
#define LS_MARKDIR	(LS_USER<<9)	/* marks dirs with /		*/
#define LS_MOST		(LS_USER<<10)	/* list all but . and ..	*/
#define LS_NOBACKUP	(LS_USER<<11)	/* omit *~ names		*/
#define LS_NOSORT	(LS_USER<<12)	/* don't sort			*/
#define LS_NOSTAT	(LS_USER<<13)	/* leaf FTW_NS ok		*/
#define LS_PRINTABLE	(LS_USER<<14)	/* ? for non-printable chars	*/
#define LS_QUOTE	(LS_USER<<15)	/* "..." file names		*/
#define LS_RECURSIVE	(LS_USER<<16)	/* recursive directory descent	*/
#define LS_SEPARATE	(LS_USER<<17)	/* dir header needs separator	*/
#define LS_SHELL	(LS_USER<<18)	/* $'...' file names		*/
#define LS_TIME		(LS_USER<<19)	/* sort by time			*/

#define LS_STAT		LS_NOSTAT

#define VISIBLE(f)	((f)->level<=0||(!state.ignore||!strmatch((f)->name,state.ignore))&&(!(state.lsflags&LS_NOBACKUP)||(f)->name[(f)->namelen-1]!='~')&&((state.lsflags&LS_ALL)||(f)->name[0]!='.'||(state.lsflags&LS_MOST)&&((f)->name[1]&&(f)->name[1]!='.'||(f)->name[2])))

#define BETWEEN		2		/* space between columns	*/
#define AFTER		1		/* space after last column	*/

#define INVISIBLE	(-1)
#define LISTED		(-2)

#define KEY_environ		(-1)

#define KEY_atime		1
#define KEY_blocks		2
#define KEY_ctime		3
#define KEY_device		4
#define KEY_devmajor		5
#define KEY_devminor		6
#define KEY_dir_blocks		7
#define KEY_dir_bytes		8
#define KEY_dir_count		9
#define KEY_dir_files		10
#define KEY_flags		11
#define KEY_gid			12
#define KEY_header		13
#define KEY_ino			14
#define KEY_linkop		15
#define KEY_linkpath		16
#define KEY_mark		17
#define KEY_markdir		18
#define KEY_mode		19
#define KEY_mtime		20
#define KEY_name		21
#define KEY_nlink		22
#define KEY_path		23
#define KEY_perm		24
#define KEY_size		25
#define KEY_summary		26
#define KEY_total_blocks	27
#define KEY_total_bytes		28
#define KEY_total_files		29
#define KEY_trailer		30
#define KEY_uid			31

#define BLOCKS(st)	((state.blocksize==LS_BLOCKSIZE)?iblocks(st):(iblocks(st)*LS_BLOCKSIZE+state.blocksize-1)/state.blocksize)
#define PRINTABLE(s)	((state.lsflags&LS_PRINTABLE)?printable(s):(s))

typedef struct				/* dir/total counts		*/
{
	unsigned long	blocks;		/* number of blocks		*/
	unsigned long	bytes;		/* number of bytes		*/
	unsigned long	files;		/* number of files		*/
} Count_t;

typedef struct				/* sfkeyprintf() keys		*/
{
	char*		name;		/* key name			*/
	short		index;		/* index			*/
	short		disable;	/* macro being expanded		*/
	char*		macro;		/* macro definition		*/
} Key_t;

typedef struct				/* list state			*/
{
	Count_t		count;		/* directory counts		*/
	Ftw_t*		ftw;		/* ftw info			*/
	char*		dirnam;		/* pr() dirnam			*/
	int		dirlen;		/* pr() dirlen			*/
} List_t;

typedef struct				/* program state		*/
{
	char		flags[64];	/* command line option flags	*/
	long		ftwflags;	/* FTW_* flags			*/
	long		lsflags;	/* LS_* flags			*/
	long		sortflags;	/* sort LS_* flags		*/
	long		timeflags;	/* time LS_* flags		*/
	long		blocksize;	/* file block size		*/
	unsigned long	directories;	/* directory count		*/
	unsigned long	testdate;	/* --format test date		*/
	Count_t		total;		/* total counts			*/
	int		adjust;		/* key() print with adjustment	*/
	int		comma;		/* LS_COMMAS ftw.level crossing	*/
	int		reverse;	/* reverse the sort		*/
	int		scale;		/* metric scale power		*/
	int		width;		/* output width in chars	*/
	char*		endflags;	/* trailing 0 in flags		*/
	char*		format;		/* sfkeyprintf() format		*/
	char*		ignore;		/* ignore files matching this	*/
	char*		timefmt;	/* time list format		*/
	Hash_table_t*	keys;		/* sfkeyprintf() keys		*/
	Sfio_t*		tmp;		/* tmp string stream		*/
	Ftw_t*		top;		/* top directory -- no label	*/
} State_t;

static char	DEF_header[] =
"%(dir.count:case;0;;1;%(path)s:\n;*;\n%(path)s:\n)s"
"%(flags:case;*d*;;*[ls]*;total %(dir.blocks)u\n)s"
;

static Key_t	keys[] =
{
	{ 0 },
	{ "atime",		KEY_atime		},
	{ "blocks",		KEY_blocks		},
	{ "ctime",		KEY_ctime		},
	{ "device",		KEY_device		},
	{ "devmajor",		KEY_devmajor		},
	{ "devminor",		KEY_devminor		},
	{ "dir.blocks",		KEY_dir_blocks		},
	{ "dir.bytes",		KEY_dir_bytes		},
	{ "dir.count",		KEY_dir_count		},
	{ "dir.files",		KEY_dir_files		},
	{ "flags",		KEY_flags		},
	{ "gid",		KEY_gid			},
	{ "header",		KEY_header, 0, DEF_header },
	{ "ino",		KEY_ino			},
	{ "linkop",		KEY_linkop		},
	{ "linkpath",		KEY_linkpath		},
	{ "mark",		KEY_mark		},
	{ "markdir",		KEY_markdir		},
	{ "mode",		KEY_mode		},
	{ "mtime",		KEY_mtime		},
	{ "name",		KEY_name		},
	{ "nlink",		KEY_nlink		},
	{ "path",		KEY_path		},
	{ "perm",		KEY_perm		},
	{ "size",		KEY_size		},
	{ "summary",		KEY_summary		},
	{ "total.blocks",	KEY_total_blocks	},
	{ "total.bytes",	KEY_total_bytes		},
	{ "total.files",	KEY_total_files		},
	{ "trailer",		KEY_trailer		},
	{ "uid",		KEY_uid			},

	/* aliases */

	{ "dir.octets",		KEY_dir_bytes		},
	{ "linkname",		KEY_linkpath		},
	{ "total.octets",	KEY_total_bytes		},
};

static State_t		state;

/*
 * return a copy of s with unprintable chars replaced by ?
 */

static char*
printable(register char* s)
{
	register char*	t;
	register char*	p;
	register int	c;

	static char*	prdata;
	static int	prsize;

	if (state.lsflags & LS_ESCAPE)
	{
		if (!(state.lsflags & LS_QUOTE))
			return fmtesc(s);
		if (state.lsflags & LS_SHELL)
			return fmtquote(s, "$'", "'", strlen(s), (state.lsflags & LS_ALWAYS) ? FMT_ALWAYS : 0);
		return fmtquote(s, "\"", "\"", strlen(s), FMT_ALWAYS);
	}
	c = strlen(s) + 4;
	if (c > prsize)
	{
		prsize = roundof(c, 512);
		if (!(prdata = newof(prdata, char, prsize, 0)))
			error(3, "out of space");
	}
	t = prdata;
	if (state.lsflags & LS_QUOTE)
		*t++ = '"';
	if (!mbwide())
		while (c = *s++)
			*t++ = (iscntrl(c) || !isprint(c)) ? '?' : c;
	else
		for (p = s; c = mbchar(s);)
			if (c < 0)
			{
				s++;
				*t++ = '?';
			}
			else if (mbwidth(c) <= 0)
				*t++ = '?';
			else
				while (p < s)
					*t++ = *p++;
	if (state.lsflags & LS_QUOTE)
		*t++ = '"';
	*t = 0;
	return prdata;
}

/*
 * sfkeyprintf() lookup
 */

static int
key(void* handle, register Sffmt_t* fp, const char* arg, char** ps, Sflong_t* pn)
{
	register Ftw_t*		ftw;
	register struct stat*	st;
	register char*		s = 0;
	register Sflong_t	n = 0;
	register Key_t*		kp;
	List_t*			lp;

	static Sfio_t*		mp;
	static const char	fmt_mode[] = "mode";
	static const char	fmt_perm[] = "perm";
	static const char	fmt_time[] = "time";

	if (!fp->t_str)
		return 0;
	if (lp = (List_t*)handle)
	{
		ftw = lp->ftw;
		st = &ftw->statb;
	}
	else
	{
		ftw = 0;
		st = 0;
	}
	if (!(kp = (Key_t*)hashget(state.keys, fp->t_str)))
	{
		if (*fp->t_str != '$')
		{
			error(3, "%s: unknown format key", fp->t_str);
			return 0;
		}
		if (!(kp = newof(0, Key_t, 1, 0)))
			error(3, "out of space");
		kp->name = hashput(state.keys, 0, kp);
		kp->macro = getenv(fp->t_str + 1);
		kp->index = KEY_environ;
		kp->disable = 1;
	}
	if (kp->macro && !kp->disable)
	{
		kp->disable = 1;
		if (!mp && !(mp = sfstropen()))
			error(3, "out of space");
		sfkeyprintf(mp, handle, kp->macro, key, NiL);
		s = sfstruse(mp);
		kp->disable = 0;
	}
	else switch (kp->index)
	{
	case KEY_atime:
		if (st)
			n = st->st_atime;
		if (!arg)
			arg = state.timefmt;
		break;
	case KEY_blocks:
		if (st)
			n = BLOCKS(st);
		break;
	case KEY_ctime:
		if (st)
			n = st->st_ctime;
		if (!arg)
			arg = state.timefmt;
		break;
	case KEY_device:
		if (st && (S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode)))
			s = fmtdev(st);
		else
			return 0;
		break;
	case KEY_devmajor:
		if (st)
			n = (S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode)) ? major(idevice(st)) : major(st->st_dev);
		break;
	case KEY_devminor:
		if (st)
			n = (S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode)) ? minor(idevice(st)) : minor(st->st_dev);
		break;
	case KEY_dir_blocks:
		if (lp)
			n = lp->count.blocks;
		break;
	case KEY_dir_bytes:
		if (lp)
			n = lp->count.bytes;
		break;
	case KEY_dir_count:
		if (ftw != state.top)
		{
			if (state.lsflags & LS_SEPARATE)
				n = state.directories;
			else if (state.lsflags & LS_LABEL)
				n = 1;
		}
		break;
	case KEY_dir_files:
		if (lp)
			n = lp->count.files;
		break;
	case KEY_environ:
		if (!(s = kp->macro))
			return 0;
		break;
	case KEY_flags:
		s = state.flags;
		break;
	case KEY_gid:
		if (st)
		{
			if (fp->fmt == 's')
				s = fmtgid(st->st_gid);
			else
				n = st->st_gid;
		}
		break;
	case KEY_ino:
		if (st)
			n = st->st_ino;
		break;
	case KEY_linkpath:
		if (ftw && ftw->info == FTW_SL)
		{
			char*		dirnam;
			int		c;

			static char*	txtdata;
			static int	txtsize;

			if ((st->st_size + 1) > txtsize)
			{
				txtsize = roundof(st->st_size + 1, 512);
				if (!(txtdata = newof(txtdata, char, txtsize, 0)))
					error(3, "out of space");
			}
			if (*ftw->name == '/' || !lp->dirnam)
				dirnam = ftw->name;
			else
			{
				sfprintf(state.tmp, "%s/%s", lp->dirnam + streq(lp->dirnam, "/"), ftw->name);
				dirnam = sfstruse(state.tmp);
			}
			c = pathgetlink(dirnam, txtdata, txtsize);
			if (c > 0)
				s = PRINTABLE(txtdata);
		}
		else
			return 0;
		break;
	case KEY_linkop:
		if (ftw && ftw->info == FTW_SL)
			s = "->";
		else
			return 0;
		break;
	case KEY_mark:
		if (!st)
			return 0;
		else if (S_ISLNK(st->st_mode))
			s = "@";
		else if (S_ISDIR(st->st_mode))
			s = "/";
#ifdef S_ISDOOR
		else if (S_ISDOOR(st->st_mode))
			s = ">";
#endif
		else if (S_ISFIFO(st->st_mode))
			s = "|";
#ifdef S_ISSOCK
		else if (S_ISSOCK(st->st_mode))
			s = "=";
#endif
		else if (S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode))
			s = "$";
		else if (st->st_mode & (S_IXUSR|S_IXGRP|S_IXOTH))
			s = "*";
		else
			return 0;
		break;
	case KEY_markdir:
		if (!st || !S_ISDIR(st->st_mode))
			return 0;
		s = "/";
		break;
	case KEY_mode:
		if (st)
			n = st->st_mode;
		if (!arg)
			arg = fmt_mode;
		break;
	case KEY_mtime:
		if (st)
			n = st->st_mtime;
		if (!arg)
			arg = state.timefmt;
		break;
	case KEY_name:
		if (ftw)
			s = PRINTABLE(ftw->name);
		break;
	case KEY_nlink:
		if (st)
			n = st->st_nlink;
		break;
	case KEY_path:
		if (ftw)
			s = ftw->path ? PRINTABLE(ftw->path) : PRINTABLE(ftw->name);
		break;
	case KEY_perm:
		if (st)
			n = st->st_mode & S_IPERM;
		if (!arg)
			arg = fmt_perm;
		break;
	case KEY_size:
		if (st)
		{
			n = st->st_size;
			if (state.scale)
			{
				s = fmtscale(n, state.scale);
				fp->fmt = 's';
			}
		}
		break;
	case KEY_total_blocks:
		n = state.total.blocks;
		break;
	case KEY_total_bytes:
		n = state.total.bytes;
		break;
	case KEY_total_files:
		n = state.total.files;
		break;
	case KEY_uid:
		if (st)
		{
			if (fp->fmt == 's')
				s = fmtuid(st->st_uid);
			else
				n = st->st_uid;
		}
		break;
	default:
		return 0;
	}
	if (s)
	{
		*ps = s;
		if (mbwide())
		{
			register char*	p;
			int		w;
			int		i;

			for (p = s; w = mbchar(s); p = s)
				if (w < 0)
					s++;
				else if ((i = mbwidth(w)) >= 0)
					state.adjust -= (s - p) + i - 2;
		}
	}
	else if (fp->fmt == 's' && arg)
	{
		if (strneq(arg, fmt_mode, sizeof(fmt_mode) - 1))
			*ps = fmtmode(n, 0);
		else if (strneq(arg, fmt_perm, sizeof(fmt_perm) - 1))
			*ps = fmtperm(n & S_IPERM);
		else
		{
			if (strneq(arg, fmt_time, sizeof(fmt_time) - 1))
			{
				arg += sizeof(fmt_time) - 1;
				if (*arg == '=')
					arg++;
			}
			if ((unsigned long)n >= state.testdate)
				n = state.testdate;
			*ps = fmttime(*arg ? arg : state.timefmt, (time_t)n);
		}
	}
	else
		*pn = n;
	return 1;
}

/*
 * print info on a single file
 * parent directory name is dirnam of dirlen chars
 */

static void
pr(register List_t* lp, Ftw_t* ftw, register int fill)
{
#ifdef S_ISLNK
	/*
	 * -H == --hairbrained
	 * no way around it - this is really ugly
	 * symlinks should be no more visible than mount points
	 * but I wear my user hat more than my administrator hat
	 */

	if (ftw->level == 0 && (state.ftwflags & (FTW_META|FTW_PHYSICAL)) == (FTW_META|FTW_PHYSICAL) && !(ftw->info & FTW_D) && !lstat(ftw->path ? ftw->path : ftw->name, &ftw->statb) && S_ISLNK(ftw->statb.st_mode))
		ftw->info = FTW_SL;
#endif
	lp->ftw = ftw;
	state.adjust = 0;
	fill -= sfkeyprintf(sfstdout, lp, state.format, key, NiL) + state.adjust;
	if (!(state.lsflags & LS_COMMAS))
	{
		if (fill > 0)
			while (fill-- > 0)
				sfputc(sfstdout, ' ');
		else
			sfputc(sfstdout, '\n');
	}
}

/*
 * pr() ftw directory child list in column order
 * directory name is dirnam of dirlen chars
 * count is the number of VISIBLE children
 * length is the length of the longest VISIBLE child
 */

static void
col(register List_t* lp, register Ftw_t* ftw, int length)
{
	register Ftw_t*	p;
	register int	i;
	register int	n;
	register int	files;
	register char*	s;
	int		w;

	lp->ftw = ftw;
	if (keys[KEY_header].macro && ftw->level >= 0)
		sfkeyprintf(sfstdout, lp, keys[KEY_header].macro, key, NiL);
	if ((files = lp->count.files) > 0)
	{
		if (!(state.lsflags & LS_COLUMNS) || length <= 0)
			n = w = 1;
		else
		{
			i = ftw->name[1];
			ftw->name[1] = 0;
			state.adjust = 2;
			w = sfkeyprintf(state.tmp, lp, state.format, key, NiL) + state.adjust;
			length += w;
			sfstrset(state.tmp, 0);
			ftw->name[1] = i;
			n = ((state.width - (length + BETWEEN + 2)) < 0) ? 1 : 2;
				
		}
		if (state.lsflags & LS_COMMAS)
		{
			length = w - 1;
			i = 0;
			n = state.width;
			for (p = ftw->link; p; p = p->link)
				if (p->local.number != INVISIBLE)
				{
					if (!mbwide())
						w = p->namelen;
					else
						for (s = p->name, w = 0; i = mbchar(s);)
							if (i < 0)
							{
								s++;
								w++;
							}
							else if ((n = mbwidth(i)) > 0)
								w += n;
					if ((n -= length + w) < 0)
					{
						n = state.width - (length + p->namelen);
						if (i)
							sfputr(sfstdout, ",\n", -1);
					}
					else if (i)
						sfputr(sfstdout, ", ", -1);
					pr(lp, p, 0);
					i = 1;
				}
			if (i)
				sfputc(sfstdout, '\n');
		}
		else if (n <= 1)
		{
			for (p = ftw->link; p; p = p->link)
				if (p->local.number != INVISIBLE)
					pr(lp, p, 0);
		}
		else
		{
			register Ftw_t**	x;
			int			c;
			int			j;
			int			k;
			int			l;
			int			o;
			int			r;
			int			w;
			int			z;

			static unsigned short*	siz;
			static int		sizsiz;

			static Ftw_t**		vec;
			static int		vecsiz;

			if (files > sizsiz)
			{
				sizsiz = roundof(files, 64);
				if (!(siz = newof(siz, unsigned short, sizsiz, 0)))
					error(3, "out of space");
			}
			if (files > (vecsiz - 1))
			{
				vecsiz = roundof(files + 1, 64);
				if (!(vec = newof(vec, Ftw_t*, vecsiz, 0)))
					error(3, "out of space");
			}
			x = vec;
			i = 0;
			for (p = ftw->link; p; p = p->link)
				if (p->local.number != INVISIBLE)
					x[i++] = p;
			n = i / (state.width / (length + BETWEEN)) + 1;
			o = 0;
			if ((state.lsflags & LS_ACROSS) && n > 1)
			{
				for (;;)
				{
					c = (i - 1) / n + 1;
					w = -AFTER;
					for (j = 0; j < c; j++)
					{
						z = 0;
						for (l = 0, r = j; l < n && r < i; r += c, l++)
							if (z < x[r]->namelen)
								z = x[r]->namelen;
						w += z + BETWEEN;
					}
					if (w <= state.width)
						o = n;
					if (n == 1)
						break;
					n--;
				}
				n = o ? o : 1;
				c = (i - 1) / n + 1;
				k = 0;
				for (j = 0; j < c; j++)
				{
					siz[k] = 0;
					for (l = 0, r = j; l < n && r < i; r += c, l++)
						if (siz[k] < x[r]->namelen)
							siz[k] = x[r]->namelen;
					siz[k] += BETWEEN;
					k++;
				}
				for (j = 0; j <= i; j += c)
					for (l = 0, w = j; l < k && w < i; l++, w++)
						pr(lp, x[w], l < (k - 1) && w < (i - 1) ? siz[l] : 0);
			}
			else
			{
				o = 0;
				if (n > 1)
					for (;;)
					{
						w = -AFTER;
						j = 0;
						while (j < i)
						{
							z = 0;
							for (l = 0; l < n && j < i; j++, l++)
								if (z < x[j]->namelen)
									z = x[j]->namelen;
							w += z + BETWEEN;
						}
						if (w <= state.width)
							o = n;
						if (n == 1)
							break;
						n--;
					}
				n = o ? o : 1;
				j = k = 0;
				while (j < i)
				{
					siz[k] = 0;
					for (l = 0; l < n && j < i; j++, l++)
						if (siz[k] < x[j]->namelen)
							siz[k] = x[j]->namelen;
					siz[k] += BETWEEN;
					k++;
				}
				for (j = 0; j < n; j++)
					for (l = 0, w = j; l < k && w < i; l++, w += n)
						pr(lp, x[w], l < (k - 1) && w < (i - n) ? siz[l] : 0);
			}
		}
	}
	if (keys[KEY_trailer].macro && ftw->level >= 0)
		sfkeyprintf(sfstdout, lp, keys[KEY_trailer].macro, key, NiL);
}

/*
 * order child entries
 */

static int
order(register Ftw_t* f1, register Ftw_t* f2)
{
	register int	n;
	char*		x1;
	char*		x2;

	if (state.sortflags & LS_NOSORT)
		return 0;
	if (!(state.lsflags & LS_DIRECTORY) && (state.ftwflags & FTW_MULTIPLE) && f1->level == 0)
	{
		if (f1->info == FTW_D)
		{
			if (f2->info != FTW_D)
				return 1;
		}
		else if (f2->info == FTW_D)
			return -1;
	}
	if (state.sortflags & LS_BLOCKS)
	{
		if (f1->statb.st_size < f2->statb.st_size)
			n = 1;
		else if (f1->statb.st_size > f2->statb.st_size)
			n = -1;
		else
			n = 0;
	}
	else if (state.sortflags & LS_TIME)
	{
		if (state.sortflags & LS_ATIME)
		{
			f1->statb.st_mtime = f1->statb.st_atime;
			f2->statb.st_mtime = f2->statb.st_atime;
		}
		else if (state.sortflags & LS_CTIME)
		{
			f1->statb.st_mtime = f1->statb.st_ctime;
			f2->statb.st_mtime = f2->statb.st_ctime;
		}
		if (f1->statb.st_mtime < f2->statb.st_mtime)
			n = 1;
		else if (f1->statb.st_mtime > f2->statb.st_mtime)
			n = -1;
		else
			n = 0;
	}
	else if (state.sortflags & LS_EXTENSION)
	{
		x1 = strrchr(f1->name, '.');
		x2 = strrchr(f2->name, '.');
		if (x1)
		{
			if (x2)
				n = strcoll(x1, x2);
			else
				n = 1;
		}
		else if (x2)
			n = -1;
		else
			n = 0;
		if (!n)
			n = strcoll(f1->name, f2->name);
	}
	else
		n = strcoll(f1->name, f2->name);
	if (state.reverse)
		n = -n;
	return n;
}

/*
 * list a directory and its children
 */

static void
dir(register Ftw_t* ftw)
{
	register Ftw_t*	p;
	register int	length;
	int		top = 0;
	List_t		list;

	if (ftw->status == FTW_NAME)
	{
		list.dirlen = ftw->namelen;
		list.dirnam = ftw->path + ftw->pathlen - list.dirlen;
	}
	else
	{
		list.dirlen = ftw->pathlen;
		list.dirnam = ftw->path;
	}
	if (ftw->level >= 0)
		state.directories++;
	else
		state.top = ftw;
	length = 0;
	list.count.blocks = 0;
	list.count.bytes = 0;
	list.count.files = 0;
	for (p = ftw->link; p; p = p->link)
	{
		if (p->level == 0 && p->info == FTW_D && !(state.lsflags & LS_DIRECTORY))
		{
			p->local.number = INVISIBLE;
			top++;
		}
		else if (VISIBLE(p))
		{
			if (p->info == FTW_NS)
			{
				if (ftw->level < 0 || !(state.lsflags & LS_NOSTAT))
				{
					if (ftw->path[0] == '.' && !ftw->path[1])
						error(2, "%s: not found", p->name);
					else
						error(2, "%s/%s: not found", ftw->path, p->name);
					goto invisible;
				}
			}
			else
			{
				list.count.blocks += BLOCKS(&p->statb);
				list.count.bytes += p->statb.st_size;
			}
			list.count.files++;
			if (p->namelen > length)
				length = p->namelen;
			if (!(state.lsflags & LS_RECURSIVE))
				p->status = FTW_SKIP;
		}
		else
		{
		invisible:
			p->local.number = INVISIBLE;
			p->status = FTW_SKIP;
		}
	}
	state.total.blocks += list.count.blocks;
	state.total.bytes += list.count.bytes;
	state.total.files += list.count.files;
	col(&list, ftw, length);
	state.lsflags |= LS_SEPARATE;
	if (top)
	{
		if (list.count.files)
		{
			state.directories++;
			state.top = 0;
		}
		else if (top > 1)
			state.top = 0;
		else
			state.top = ftw->link;
		for (p = ftw->link; p; p = p->link)
			if (p->level == 0 && p->info == FTW_D)
				p->local.number = 0;
	}
}

/*
 * list info on a single file
 */

static int
ls(register Ftw_t* ftw)
{
	if (!VISIBLE(ftw))
	{
		ftw->status = FTW_SKIP;
		return 0;
	}
	switch (ftw->info)
	{
	case FTW_NS:
		if (ftw->parent->info == FTW_DNX)
			break;
		error(2, "%s: not found", ftw->path);
		return 0;
	case FTW_DC:
		if (state.lsflags & LS_DIRECTORY)
			break;
		error(2, "%s: directory causes cycle", ftw->path);
		return 0;
	case FTW_DNR:
		if (state.lsflags & LS_DIRECTORY)
			break;
		error(2, "%s: cannot read directory", ftw->path);
		return 0;
	case FTW_D:
	case FTW_DNX:
		if ((state.lsflags & LS_DIRECTORY) && ftw->level >= 0)
			break;
		if (!(state.lsflags & LS_RECURSIVE))
			ftw->status = FTW_SKIP;
		else if (ftw->info == FTS_DNX)
		{
			error(2, "%s: cannot search directory", ftw->path, ftw->level);
			ftw->status = FTW_SKIP;
			if (ftw->level > 0 && !(state.lsflags & LS_NOSTAT))
				return 0;
		}
		dir(ftw);
		return 0;
	}
	ftw->status = FTW_SKIP;
	if (!ftw->level)
	{
		static List_t	list;

		list.ftw = ftw;
		pr(&list, ftw, 0);
	}
	return 0;
}

int
main(int argc, register char** argv)
{
	register int	n;
	register char*	s;
	char*		e;
	Key_t*		kp;
	Sfio_t*		fmt;
	long		lsflags;
	int		dump = 0;

	static char	fmt_color[] = "%(mode:case:d*:\\E[01;34m%(name)s\\E[0m:l*:\\E[01;36m%(name)s\\E[0m:*x*:\\E[01;32m%(name)s\\E[0m:*:%(name)s)s";

	NoP(argc);
	setlocale(LC_ALL, "");
	if (s = strrchr(argv[0], '/'))
		s++;
	else
		s = argv[0];
	error_info.id = s;
	state.ftwflags = ftwflags() | FTW_CHILDREN;
	if (!(fmt = sfstropen()) || !(state.tmp = sfstropen()))
		error(3, "out of space");
	if (!(state.keys = hashalloc(NiL, HASH_name, "keys", 0)))
		error(3, "out of space");
	for (n = 1; n < elementsof(keys); n++)
		hashput(state.keys, keys[n].name, &keys[keys[n].index]);
	hashset(state.keys, HASH_ALLOCATE);
	if (streq(s, "lc"))
		state.lsflags |= LS_COLUMNS;
	else if (streq(s, "lf") || streq(s, "lsf"))
		state.lsflags |= LS_MARK;
	else if (streq(s, "ll"))
		state.lsflags |= LS_LONG;
	else if (streq(s, "lsr"))
		state.lsflags |= LS_RECURSIVE;
	else if (streq(s, "lsx"))
		state.lsflags |= LS_ACROSS|LS_COLUMNS;
	else if (isatty(1))
		state.lsflags |= LS_COLUMNS|LS_PRINTABLE;
	state.endflags = state.flags;
	state.blocksize = 512;
	state.testdate = ~0;
	state.timefmt = "%?%l";
	lsflags = state.lsflags;
	while (n = optget(argv, usage))
	{
		switch (n)
		{
		case 'a':
			state.lsflags |= LS_ALL;
			break;
		case 'b':
			state.lsflags |= LS_PRINTABLE|LS_ESCAPE;
			break;
		case 'c':
			state.lsflags &= ~LS_ATIME;
			state.lsflags |= LS_CTIME;
			break;
		case 'd':
			state.lsflags |= LS_DIRECTORY;
			break;
		case 'e':
			state.scale = 1000;
			break;
		case 'f':
			if (!sfstrtell(fmt))
				state.lsflags &= ~LS_COLUMNS;
			sfputr(fmt, opt_info.arg, ' ');
			break;
		case 'g':
		case 'O':
			if (opt_info.num)
				state.lsflags |= LS_LONG|LS_NOUSER;
			else
				state.lsflags |= LS_LONG|LS_NOGROUP;
			break;
		case 'h':
			state.scale = 1024;
			break;
		case 'i':
			state.lsflags |= LS_INUMBER;
			break;
		case 'k':
			state.blocksize = 1024;
			break;
		case 'l':
			state.lsflags |= LS_LONG;
			break;
		case 'm':
			state.lsflags |= LS_COMMAS;
			break;
		case 'n':
			state.lsflags |= LS_NUMBER;
			break;
		case 'o':
		case 'G':
			if (opt_info.num)
				state.lsflags |= LS_LONG|LS_NOGROUP;
			else
				state.lsflags |= LS_LONG|LS_NOUSER;
			break;
		case 'p':
			state.lsflags |= LS_MARKDIR;
			break;
		case 'q':
			state.lsflags |= LS_PRINTABLE;
			break;
		case 'r':
			state.reverse = 1;
			break;
		case 's':
			state.lsflags |= LS_BLOCKS;
			break;
		case 't':
			state.lsflags |= LS_TIME;
			break;
		case 'u':
			state.lsflags &= ~LS_CTIME;
			state.lsflags |= LS_ATIME;
			break;
		case 'w':
			state.width = opt_info.num;
			break;
		case 'x':
			state.lsflags |= LS_ACROSS|LS_COLUMNS;
			break;
		case 'y':
			if (!opt_info.arg)
				state.sortflags = LS_NOSORT;
			else
				switch (opt_info.num)
				{
				case 'a':
					state.sortflags = LS_TIME|LS_ATIME;
					break;
				case 'c':
					state.sortflags = LS_TIME|LS_CTIME;
					break;
				case 'f':
					state.sortflags = 0;
					break;
				case 'm':
					state.sortflags = LS_TIME;
					break;
				case 'n':
					state.sortflags = LS_NOSORT;
					break;
				case 's':
					state.sortflags = LS_BLOCKS;
					break;
				case 't':
					state.sortflags = LS_TIME;
					break;
				case 'x':
					state.sortflags = LS_EXTENSION;
					break;
				}
			break;
		case 'z':
			switch (opt_info.num)
			{
			case -10:
				if (!strcmp(setlocale(LC_TIME, NiL), "C"))
					break;
				/*FALLTHROUGH*/
			case 'i':
				state.timefmt = "%K";
				break;
			case -11:
				if (!strcmp(setlocale(LC_TIME, NiL), "C"))
					break;
				/*FALLTHROUGH*/
			case 'f':
				state.timefmt = "%Y-%m-%d+%H:%M:%S.000000%z";
				break;
			case 'l':
				state.timefmt = "%c";
				break;
			case -12:
				s = opt_info.arg + 1;
				if (strchr(s, '\n'))
				{
					/*
					 * gnu compatibility
					 */

					s = sfprints("%%Q\n%s\n", s);
					if (!s || !(s = strdup(s)))
						error(ERROR_SYSTEM|3, "out of space");
				}
				state.timefmt = s;
				break;
			}
			break;
		case 'A':
			state.lsflags |= LS_MOST;
			state.lsflags &= ~LS_ALL;
			break;
		case 'B':
			state.lsflags |= LS_NOBACKUP;
			break;
		case 'C':
			state.lsflags |= LS_COLUMNS;
			break;
		case 'D':
			if (s = strchr(opt_info.arg, '='))
				*s++ = 0;
			if (*opt_info.arg == 'n' && *(opt_info.arg + 1) == 'o')
			{
				opt_info.arg += 2;
				s = 0;
			}
			if (!(kp = (Key_t*)hashget(state.keys, opt_info.arg)))
			{
				if (!s)
					break;
				if (!(kp = newof(0, Key_t, 1, 0)))
					error(3, "out of space");
				kp->name = hashput(state.keys, 0, kp);
			}
			if (kp->macro = s)
			{
				stresc(s);
				if (strmatch(s, "*:case:*"))
					state.lsflags |= LS_STAT;
			}
			break;
		case 'E':
			if (opt_info.num <= 0)
				error(3, "%ld: invalid block size", opt_info.num);
			state.blocksize = opt_info.num;
			break;
		case 'F':
			state.lsflags |= LS_MARK;
			break;
		case 'H':
			state.ftwflags |= FTW_META|FTW_PHYSICAL;
			break;
		case 'I':
			state.ignore = opt_info.arg;
			break;
		case 'J':
			state.lsflags &= ~(LS_ALWAYS|LS_ESCAPE|LS_PRINTABLE|LS_QUOTE|LS_SHELL);
			switch (opt_info.num)
			{
			case 'c':
				state.lsflags |= LS_ESCAPE|LS_PRINTABLE|LS_QUOTE;
				break;
			case 'e':
				state.lsflags |= LS_ESCAPE|LS_PRINTABLE;
				break;
			case 'l':
				state.lsflags |= LS_ESCAPE|LS_PRINTABLE|LS_QUOTE|LS_SHELL;
				break;
			case 'q':
				state.lsflags |= LS_PRINTABLE;
				break;
			case 's':
				state.lsflags |= LS_ESCAPE|LS_PRINTABLE|LS_QUOTE|LS_SHELL;
				break;
			case 'S':
				state.lsflags |= LS_ALWAYS|LS_ESCAPE|LS_PRINTABLE|LS_QUOTE|LS_SHELL;
				break;
			}
			break;
		case 'K':
			state.lsflags |= LS_PRINTABLE|LS_SHELL|LS_QUOTE|LS_ESCAPE;
			break;
		case 'L':
			state.ftwflags &= ~(FTW_META|FTW_PHYSICAL|FTW_SEEDOTDIR);
			break;
		case 'N':
			state.lsflags &= ~LS_PRINTABLE;
			break;
		case 'P':
			state.ftwflags &= ~FTW_META;
			state.ftwflags |= FTW_PHYSICAL;
			break;
		case 'Q':
			state.lsflags |= LS_PRINTABLE|LS_QUOTE;
			break;
		case 'R':
			state.lsflags |= LS_RECURSIVE;
			break;
		case 'S':
			state.sortflags |= LS_BLOCKS;
			break;
		case 'T':
			/* ignored */
			break;
		case 'U':
			state.sortflags = LS_NOSORT;
			break;
		case 'V':
			switch (opt_info.num)
			{
			case 't':
				if (!isatty(1))
					break;
				/*FALLTHROUGH*/
			case 'a':
				if (kp = (Key_t*)hashget(state.keys, "name"))
				{
					stresc(kp->macro = fmt_color);
					state.lsflags |= LS_STAT;
				}
				break;
			}
			break;
		case 'W':
			state.timeflags = 0;
			switch (opt_info.num)
			{
			case 'a':
				state.timeflags = LS_ATIME;
				break;
			case 'c':
				state.timeflags = LS_CTIME;
				break;
			}
			break;
		case 'X':
			state.lsflags |= LS_EXTENSION;
			break;
		case 'Y':
			switch (opt_info.num)
			{
			case 'a':
				state.lsflags |= LS_ACROSS|LS_COLUMNS;
				break;
			case 'c':
				state.lsflags |= LS_COMMAS;
				break;
			case 'l':
				state.lsflags |= LS_LONG;
				break;
			case 'v':
				state.lsflags &= ~LS_ACROSS;
				state.lsflags |= LS_COLUMNS;
				break;
			case '1':
				state.lsflags &= ~(LS_ACROSS|LS_COLUMNS);
				break;
			}
			break;
		case '1':
			state.lsflags &= ~(LS_COLUMNS|LS_PRINTABLE);
			break;
		case -101:
			dump = 1;
			break;
		case -102:
			state.timefmt = "%c";
			break;
		case -103:
			state.testdate = tmdate(opt_info.arg, &e, NiL);
			if (*e)
				error(3, "%s: invalid date string", opt_info.arg, opt_info.option);
			break;
		case '?':
			error(ERROR_USAGE|4, "%s", opt_info.arg);
			break;
		case ':':
			error(2, "%s", opt_info.arg);
			break;
		default:
			error(1, "%s: option not implemented", opt_info.name);
			continue;
		}
		if (!strchr(state.flags, n))
			*state.endflags++ = n;
	}
	argv += opt_info.index;
	if (error_info.errors)
		error(ERROR_USAGE|4, "%s", optusage(NiL));
	if (state.lsflags == (lsflags|LS_TIME))
		state.ftwflags |= FTW_SEEDOTDIR; /* keep configure happy */
	if (state.lsflags & LS_DIRECTORY)
		state.lsflags &= ~LS_RECURSIVE;
	if (!state.sortflags)
		state.sortflags = state.lsflags & ~LS_BLOCKS;
	if (!state.timeflags)
		state.timeflags = state.lsflags;
	if (state.lsflags & (LS_COLUMNS|LS_COMMAS))
	{
		if (state.lsflags & LS_LONG)
			state.lsflags &= ~(LS_COLUMNS|LS_COMMAS);
		else if (!state.width)
		{
			astwinsize(1, NiL, &state.width);
			if (state.width <= 20)
				state.width = 80;
		}
	}
	if (state.lsflags & LS_STAT)
		state.lsflags &= ~LS_NOSTAT;
	else if (!(state.lsflags & (LS_DIRECTORY|LS_BLOCKS|LS_LONG|LS_MARK|LS_MARKDIR|LS_TIME
#if !_mem_d_fileno_dirent && !_mem_d_ino_dirent
		|LS_INUMBER
#endif
		)) && !sfstrtell(fmt))
	{
		state.lsflags |= LS_NOSTAT;
		state.ftwflags |= FTW_DELAY|FTW_DOT;
	}
	if (!sfstrtell(fmt))
	{
		if (state.lsflags & LS_INUMBER)
			sfputr(fmt, "%6(ino)u ", -1);
		if (state.lsflags & LS_BLOCKS)
			sfputr(fmt, "%5(blocks)u ", -1);
		if (state.lsflags & LS_LONG)
		{
			sfputr(fmt, "%(mode)s%3(nlink)u", -1);
			if (!(state.lsflags & LS_NOUSER))
				sfprintf(fmt, " %%-8(uid)%c", (state.lsflags & LS_NUMBER) ? 'd' : 's');
			if (!(state.lsflags & LS_NOGROUP))
				sfprintf(fmt, " %%-8(gid)%c", (state.lsflags & LS_NUMBER) ? 'd' : 's');
			sfputr(fmt, "%8(device:case::%(size)u:*:%(device)s)s", -1);
			sfprintf(fmt, " %%(%s)s ", (state.timeflags & LS_ATIME) ? "atime" : (state.timeflags & LS_CTIME) ? "ctime" : "mtime");
		}
		sfputr(fmt, "%(name)s", -1);
		if (state.lsflags & LS_MARK)
			sfputr(fmt, "%(mark)s", -1);
		else if (state.lsflags & LS_MARKDIR)
			sfputr(fmt, "%(markdir)s", -1);
		if (state.lsflags & LS_LONG)
			sfputr(fmt, "%(linkop:case:?*: %(linkop)s %(linkpath)s)s", -1);
	}
	else
		sfstrrel(fmt, -1);
	state.format = sfstruse(fmt);
	if (dump)
	{
		sfprintf(sfstdout, "%s\n", state.format);
		exit(0);
	}
	stresc(state.format);

	/*
	 * do it
	 */

	if (argv[0])
	{
		if (argv[1])
			state.lsflags |= LS_LABEL;
		state.ftwflags |= FTW_MULTIPLE;
		ftwalk((char*)argv, ls, state.ftwflags, order);
	}
	else
		ftwalk(".", ls, state.ftwflags, order);
	if (keys[KEY_summary].macro)
		sfkeyprintf(sfstdout, NiL, keys[KEY_summary].macro, key, NiL);
	exit(error_info.errors != 0);
}
