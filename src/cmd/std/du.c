/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1989-2009 AT&T Intellectual Property          *
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
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * du -- report number of blocks used by . | file ...
 */

static const char usage[] =
"[-?\n@(#)$Id: du (AT&T Research) 2009-12-09 $\n]"
USAGE_LICENSE
"[+NAME?du - summarize disk usage]"
"[+DESCRIPTION?\bdu\b reports the number of blocks contained in all files"
"	and recursively all directories named by the \apath\a arguments."
"	The current directory is used if no \apath\a is given. Usage for"
"	all files and directories is counted, even when the listing is"
"	omitted.]"
"[+?The default block size is 512. The block count includes only the actual"
"	data blocks used by each file and directory, and may not include"
"	other filesystem data required to represent the file. Blocks are"
"	counted only for the first link to a file; subsequent links are"
"	ignored. Partial blocks are rounded up for each file.]"

"[a:all?List usage for each file. If neither \b--all\b nor \b--summary\b"
"	is specified then only usage for the \apath\a arguments and"
"	directories is listed.]"
"[b:blocksize?Set the block size to \asize\a. If omitted, \asize\a defaults"
"	to 512.]#?[size:=512]"
"[f:silent?Do not report file and directory access errors.]"
"[h:binary-scale|human-readable?Scale disk usage to powers of 1024.]"
"[K:decimal-scale?Scale disk usage to powers of 1000.]"
"[k:kilobytes?List usage in units of 1024 bytes.]"
"[m:megabytes?List usage in units of 1024K bytes.]"
"[s:summary|summarize?Only display the total for each \apath\a argument.]"
"[t|c:total?Display a grand total for all files and directories.]"
"[v|r:verbose?Report all file and directory access errors. This is the"
"	default.]"
"[x|X|l:xdev|local|mount|one-file-system?Do not descend into directories in"
"	different filesystems than their parents.]"
"[L:logical|follow?Follow symbolic links. The default is \b--physical\b.]"
"[H:metaphysical?Follow command argument symbolic links, otherwise don't"
"	follow. The default is \b--physical\b.]"
"[P:physical?Don't follow symbolic links. The default is \b--physical\b.]"

"\n"
"\n[ path ... ]\n"
"\n"

"[+SEE ALSO?\bfind\b(1), \bls\b(1), \btw\b(1)]"
;

#include <ast.h>
#include <ls.h>
#include <hash.h>
#include <ftwalk.h>
#include <error.h>

#define BLOCKS(n)	(Count_t)((state.blocksize==LS_BLOCKSIZE)?(n):(((n)*LS_BLOCKSIZE+state.blocksize-1)/state.blocksize))

typedef Sfulong_t Count_t;

static struct State_s			/* program state		*/
{
	int		all;		/* list non-directories too	*/
	Hash_table_t*	links;		/* hard link hash table		*/
	int		silent;		/* be silent about errors	*/
	int		summary;	/* list summary only		*/
	int		scale;		/* numeric output scale		*/
	int		total;		/* list complete total only	*/
	unsigned long	blocksize;	/* blocksize			*/
	Count_t		count;		/* total block count		*/
} state;

typedef struct Fileid_s			/* unique file id		*/
{
	int		dev;
	int		ino;
} Fileid_t;

/*
 * list info on a single file
 */

static int
du(register Ftw_t* ftw)
{
	register Count_t	n = 0;
	register Count_t	b;
	register int		list = !state.summary;

	if (ftw->info == FTW_NS)
	{
		if (!state.silent)
			error(ERROR_SYSTEM|2, "%s: not found", ftw->path);
		return 0;
	}
	switch (ftw->info)
	{
	case FTW_D:
		if (!(ftw->local.pointer = newof(0, Count_t, 1, 0)))
			error(ERROR_SYSTEM|3, "out of space");
		return 0;
	case FTW_DC:
		if (!state.silent)
			error(2, "%s: directory causes cycle", ftw->path);
		return 0;
	case FTW_DNR:
		if (!state.silent)
			error(ERROR_SYSTEM|2, "%s: cannot read directory", ftw->path);
		break;
	case FTW_DNX:
		if (!state.silent)
			error(ERROR_SYSTEM|2, "%s: cannot search directory", ftw->path);
		ftw->status = FTW_SKIP;
		break;
	case FTW_DP:
		if (ftw->local.pointer)
		{
			n = *(Count_t*)ftw->local.pointer;
			free(ftw->local.pointer);
		}
		break;
	default:
		if (ftw->statb.st_nlink > 1)
		{
			Fileid_t	id;
			Hash_bucket_t*	b;

			id.dev = ftw->statb.st_dev;
			id.ino = ftw->statb.st_ino;
			if (!(b = (Hash_bucket_t*)hashlook(state.links, (char*)&id, HASH_CREATE|HASH_FIXED, (char*)sizeof(Hash_bucket_t))))
			{
				static int	warned;

				if (!warned)
				{
					warned = 1;
					error(1, "%s: hard link table out of space", ftw->path);
				}
			}
			else if (b->value)
				return 0;
			b->value = (char*)b;
		}
		if (!state.all)
			list = 0;
		break;
	}
	b = iblocks(&ftw->statb);
	state.count += b;
	n += b;
	if (ftw->parent->local.pointer)
		*(Count_t*)ftw->parent->local.pointer += n;
	if (!state.total && (list || ftw->level <= 0))
	{
		if (state.scale)
			sfprintf(sfstdout, "%s\t%s\n", fmtscale((Sfulong_t)n * state.blocksize, state.scale), ftw->path);
		else
			sfprintf(sfstdout, "%I*u\t%s\n", sizeof(Count_t), BLOCKS(n), ftw->path);
	}
	return 0;
}

int
main(int argc, register char** argv)
{
	int	flags;

	NoP(argc);
	error_info.id = "du";
	state.blocksize = LS_BLOCKSIZE;
	flags = FTW_MULTIPLE|FTW_PHYSICAL|FTW_TWICE;
	for (;;)
	{
		switch (optget(argv, usage))
		{
		case 'a':
			state.all = 1;
			continue;
		case 'b':
			state.blocksize = (opt_info.num <= 0) ? 512 : opt_info.num;
			continue;
		case 'f':
			state.silent = 1;
			continue;
		case 'h':
			state.scale = 1024;
			continue;
		case 'K':
			state.scale = 1000;
			continue;
		case 'k':
			state.blocksize = 1024;
			continue;
		case 'm':
			state.blocksize = 1024 * 1024;
			continue;
		case 's':
			state.summary = 1;
			continue;
		case 't':
			state.total = 1;
			continue;
		case 'x':
			flags |= FTW_MOUNT;
			continue;
		case 'v':
			state.silent = 0;
			continue;
		case 'H':
			flags |= FTW_META|FTW_PHYSICAL;
			continue;
		case 'L':
			flags &= ~(FTW_META|FTW_PHYSICAL);
			continue;
		case 'P':
			flags &= ~FTW_META;
			flags |= FTW_PHYSICAL;
			continue;
		case '?':
			error(ERROR_USAGE|4, "%s", opt_info.arg);
			break;
		case ':':
			error(2, "%s", opt_info.arg);
			break;
		}
		break;
	}
	argv += opt_info.index;
	if (error_info.errors)
		error(ERROR_USAGE|4, "%s", optusage(NiL));
	if (!(state.links = hashalloc(NiL, HASH_set, HASH_ALLOCATE, HASH_namesize, sizeof(Fileid_t), HASH_name, "hard-links", 0)))
		error(3, "not enough space for hard link table");

	/*
	 * do it
	 */

	ftwalk(argv[0] ? (char*)argv : NiL, du, flags, NiL);
	if (state.total)
	{
		if (state.scale)
			sfprintf(sfstdout, "%s\n", fmtscale(state.count * state.blocksize, state.scale));
		else
			sfprintf(sfstdout, "%I*u\n", sizeof(Count_t), BLOCKS(state.count));
	}
	return error_info.errors != 0;
}
