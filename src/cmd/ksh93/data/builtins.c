/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1982-2003 AT&T Corp.                *
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
*                David Korn <dgk@research.att.com>                 *
*                                                                  *
*******************************************************************/
#pragma prototyped

#include	<shell.h>
#include	<signal.h>
#include	"shtable.h"
#include	"ulimit.h"
#include	"name.h"
#if KSHELL
#   include	"builtins.h"
#   include	"jobs.h"
#   include	"FEATURE/cmds"
#   define	bltin(x)	(b_##x)
    /* The following is for builtins that do not accept -- options */
#   define	Bltin(x)	(B_##x)
#else
#   define bltin(x)	0
#endif

#if SHOPT_CMDLIB_DIR
#   ifdef SH_CMDLIB_DIR
#       define BDIR		SH_CMDLIB_DIR
#   else
#       define BDIR		"/opt/ast/bin/"
#   endif
#   undef  SHOPT_CMDLIB_BLTIN
#   define SHOPT_CMDLIB_BLTIN	1
#else
#   define BDIR
#endif

/*
 * The order up through "[" is significant
 */
const struct shtable3 shtab_builtins[] =
{
	"login",	NV_BLTIN|BLT_ENV|BLT_SPC,	Bltin(login),
	"exec",		NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(exec),
	"set",		NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(set),	
	":",		NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(true),
	"true",		NV_BLTIN|BLT_ENV,		bltin(true),
	"command",	NV_BLTIN|BLT_ENV|BLT_EXIT,	bltin(command),
	"cd",		NV_BLTIN|BLT_ENV,		bltin(cd),
	"break",	NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(break),
	"continue",	NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(break),
	"typeset",	NV_BLTIN|BLT_ENV|BLT_SPC|BLT_DCL,bltin(typeset),
	"test",		NV_BLTIN|BLT_ENV|NV_NOFREE,	bltin(test),
	"[",		NV_BLTIN|BLT_ENV,		bltin(test),
	"let",		NV_BLTIN|BLT_ENV,		bltin(let),
#if SHOPT_BASH
	"local",	NV_BLTIN|BLT_ENV|BLT_SPC|BLT_DCL,bltin(typeset),
#endif
#ifdef _bin_newgrp
	"newgrp",	NV_BLTIN|BLT_ENV|BLT_SPC,	Bltin(login),
#endif	/* _bin_newgrp */
	".",		NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(dot_cmd),
	"alias",	NV_BLTIN|BLT_SPC|BLT_DCL,	bltin(alias),
	"hash",		NV_BLTIN|BLT_SPC|BLT_DCL,	bltin(alias),
	"exit",		NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(return),
	"export",	NV_BLTIN|BLT_SPC|BLT_DCL,	bltin(readonly),
	"eval",		NV_BLTIN|BLT_ENV|BLT_SPC|BLT_EXIT,bltin(eval),
	"fc",		NV_BLTIN|BLT_ENV|BLT_EXIT,	bltin(hist),
	"hist",		NV_BLTIN|BLT_ENV|BLT_EXIT,	bltin(hist),
	"readonly",	NV_BLTIN|BLT_ENV|BLT_SPC|BLT_DCL,bltin(readonly),
	"return",	NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(return),
	"shift",	NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(shift),
	"trap",		NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(trap),
	"unalias",	NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(unalias),
	"unset",	NV_BLTIN|BLT_ENV|BLT_SPC,	bltin(unset),
	"builtin",	NV_BLTIN,			bltin(builtin),
#if SHOPT_ECHOPRINT
	"echo",		NV_BLTIN|BLT_ENV,		bltin(print),
#else
	"echo",		NV_BLTIN|BLT_ENV,		Bltin(echo),
#endif /* SHOPT_ECHOPRINT */
#ifdef JOBS
#   ifdef SIGTSTP
	"bg",		NV_BLTIN|BLT_ENV,		bltin(bg),
	"fg",		NV_BLTIN|BLT_ENV|BLT_EXIT,	bltin(bg),
	"disown",	NV_BLTIN|BLT_ENV,		bltin(bg),
	"kill",		NV_BLTIN|BLT_ENV|NV_NOFREE,	bltin(kill),
#   else
	"/bin/kill",	NV_BLTIN|BLT_ENV|NV_NOFREE,	bltin(kill),
#   endif	/* SIGTSTP */
	"jobs",		NV_BLTIN|BLT_ENV,		bltin(jobs),
#endif	/* JOBS */
	"false",	NV_BLTIN|BLT_ENV,		bltin(false),
	"getconf",	NV_BLTIN|BLT_ENV,		bltin(getconf),
	"getopts",	NV_BLTIN|BLT_ENV,		bltin(getopts),
	"print",	NV_BLTIN|BLT_ENV,		bltin(print),
	"printf",	NV_BLTIN|NV_NOFREE,		bltin(printf),
	"pwd",		NV_BLTIN|NV_NOFREE,		bltin(pwd),
	"read",		NV_BLTIN,			bltin(read),
	"sleep",	NV_BLTIN|NV_NOFREE,		bltin(sleep),
	"alarm",	NV_BLTIN,			bltin(alarm),
	"ulimit",	NV_BLTIN|BLT_ENV,		bltin(ulimit),
	"umask",	NV_BLTIN|BLT_ENV,		bltin(umask),
#ifdef _cmd_universe
	"universe",	NV_BLTIN|BLT_ENV,		bltin(universe),
#endif /* _cmd_universe */
#if SHOPT_FS_3D
	"vpath",	NV_BLTIN|BLT_ENV,		bltin(vpath),
	"vmap",		NV_BLTIN|BLT_ENV,		bltin(vpath),
#endif /* SHOPT_FS_3D */
	"wait",		NV_BLTIN|BLT_ENV|BLT_EXIT,	bltin(wait),
	"type",		NV_BLTIN|BLT_ENV,		bltin(whence),
	"whence",	NV_BLTIN|BLT_ENV,		bltin(whence),
#if SHOPT_CMDLIB_BLTIN
	BDIR "basename",NV_BLTIN|NV_NOFREE,	 	bltin(basename),
	BDIR "cat",	NV_BLTIN|NV_NOFREE,	 	bltin(cat),
	BDIR "chgrp",	NV_BLTIN|NV_NOFREE,	 	bltin(chgrp),
	BDIR "chmod",	NV_BLTIN|NV_NOFREE,	 	bltin(chmod),
	BDIR "chown",	NV_BLTIN|NV_NOFREE,	 	bltin(chown),
	BDIR "cmp",	NV_BLTIN|NV_NOFREE,	 	bltin(cmp),
	BDIR "comm",	NV_BLTIN|NV_NOFREE,	 	bltin(comm),
	BDIR "cp",	NV_BLTIN|NV_NOFREE,	 	bltin(cp),
	BDIR "cut",	NV_BLTIN|NV_NOFREE,	 	bltin(cut),
	BDIR "date",	NV_BLTIN|NV_NOFREE,	 	bltin(date),
	BDIR "dirname",	NV_BLTIN|NV_NOFREE,	 	bltin(dirname),
	BDIR "expr",	NV_BLTIN|NV_NOFREE,	 	bltin(expr),
	BDIR "fmt",	NV_BLTIN|NV_NOFREE,	 	bltin(fmt),
	BDIR "fold",	NV_BLTIN|NV_NOFREE,	 	bltin(fold),
	BDIR "getconf",	NV_BLTIN|NV_NOFREE,	 	bltin(getconf),
	BDIR "head",	NV_BLTIN|NV_NOFREE,	 	bltin(head),
	BDIR "id",	NV_BLTIN|NV_NOFREE,	 	bltin(id),
	BDIR "join",	NV_BLTIN|NV_NOFREE,	 	bltin(join),
	BDIR "ln",	NV_BLTIN|NV_NOFREE,	 	bltin(ln),
	BDIR "logname",	NV_BLTIN|NV_NOFREE,	 	bltin(logname),
	BDIR "mkdir",	NV_BLTIN|NV_NOFREE,	 	bltin(mkdir),
	BDIR "mkfifo",	NV_BLTIN|NV_NOFREE,	 	bltin(mkfifo),
	BDIR "mv",	NV_BLTIN|NV_NOFREE,	 	bltin(mv),
	BDIR "paste",	NV_BLTIN|NV_NOFREE,	 	bltin(paste),
	BDIR "pathchk",	NV_BLTIN|NV_NOFREE,	 	bltin(pathchk),
	BDIR "rev",	NV_BLTIN|NV_NOFREE,	 	bltin(rev),
	BDIR "rm",	NV_BLTIN|NV_NOFREE,	 	bltin(rm),
	BDIR "rmdir",	NV_BLTIN|NV_NOFREE,	 	bltin(rmdir),
	BDIR "stty",	NV_BLTIN|NV_NOFREE,	 	bltin(stty),
	BDIR "tail",	NV_BLTIN|NV_NOFREE,	 	bltin(tail),
	BDIR "tee",	NV_BLTIN|NV_NOFREE,	 	bltin(tee),
	BDIR "tty",	NV_BLTIN|NV_NOFREE,	 	bltin(tty),
	BDIR "uname",	NV_BLTIN|NV_NOFREE,	 	bltin(uname),
	BDIR "uniq",	NV_BLTIN|NV_NOFREE,	 	bltin(uniq),
	BDIR "wc",	NV_BLTIN|NV_NOFREE,	 	bltin(wc),
#else
	"/bin/basename",NV_BLTIN|NV_NOFREE,		bltin(basename),
	"/bin/chmod",	NV_BLTIN|NV_NOFREE,		bltin(chmod),
	"/bin/dirname",	NV_BLTIN|NV_NOFREE,		bltin(dirname),
	"/bin/head",	NV_BLTIN|NV_NOFREE,		bltin(head),
	"/bin/mkdir",	NV_BLTIN|NV_NOFREE,		bltin(mkdir),
#   if defined(_usr_bin_logname)  && !defined(_bin_logname)
	"/usr/bin/logname",	NV_BLTIN|NV_NOFREE,	bltin(logname),
#   else
	"/bin/logname",	NV_BLTIN|NV_NOFREE,		bltin(logname),
#   endif
	"/bin/cat",	NV_BLTIN|NV_NOFREE,		bltin(cat),
	"/bin/cmp",	NV_BLTIN|NV_NOFREE,		bltin(cmp),
#   if defined(_usr_bin_cut)  && !defined(_bin_cut)
	"/usr/bin/cut",	NV_BLTIN|NV_NOFREE,		bltin(cut),
#   else
	"/bin/cut",	NV_BLTIN|NV_NOFREE,		bltin(cut),
#   endif
	"/bin/uname",	NV_BLTIN|NV_NOFREE,		bltin(uname),
#   if defined(_usr_bin_wc)  && !defined(_bin_wc)
	"/usr/bin/wc",	NV_BLTIN|NV_NOFREE,		bltin(wc),
#   else
#	if defined(_usr_ucb_wc)  && !defined(_bin_wc)
	   "/usr/ucb/wc", NV_BLTIN|NV_NOFREE,		bltin(wc),
#	else
	   "/bin/wc",	NV_BLTIN|NV_NOFREE,		bltin(wc),
#	endif
#   endif
#endif
	"",		0, 0 
};


const char sh_set[] =
"[a?Set the export attribute for each variable whose name does not "
	"contain a \b.\b that you assign a value in the current shell "
	"environment.]"
"[b?The shell writes a message to standard error as soon it detects that "
	"a background job completes rather than waiting until the next prompt.]"
"[e?A simple command that has an non-zero exit status will cause the shell "
	"to exit unless the simple command is:]{"
	"[++?contained in an \b&&\b or \b||\b list.]"
	"[++?the command immediately following \bif\b, \bwhile\b, or \buntil\b.]"
	"[++?contained in the pipeline following \b!\b.]"
"}"
"[f?Pathname expansion is disabled.]"
"[h?Obsolete.  Causes each command whose name has the syntax of an "
	"alias to become a tracked aliase when it is first encountered.]"
"[k?This is obsolete.  All arguments of the form \aname\a\b=\b\avalue\a "
	"are removed and placed in the variable assignment list for "
	"the command.  Ordinarily, variable assignments must precede "
	"command arguments.]"
"[m?When enabled, the shell runs background jobs in a separate process "
	"group and displays a line upon completion.  This mode is enabled "
	"by default for interactive shells on systems that support job "
	"control.]"
"[n?The shell reads commands and checks for syntax errors, but does "
	"not execute the command.  Usually specified on command invocation.]"
"[o]:?[option?If \aoption\a is not specified, the list of options and "
	"their current settings will be written to standard output.  When "
	"invoked with a \b+\b the options will be written in a format "
	"that can be reinput to the shell to restore the settings. "
	"This option can be repeated to enable/disable multiple options. "
	"The value of \aoption\a must be one of the following:]{"
		"[+allexport?Equivalent to \b-a\b.]"
#if SHOPT_BASH
		"[+braceexpand?Equivalent to \b-B\b. Available in bash "
		"compatibility mode only.]"
#endif
		"[+bgnice?Runs background jobs at lower priorities.]"
		"[+emacs?Enables/disables \bemacs\b editing mode.]"
		"[+errexit?Equivalent to \b-e\b.]"
		"[+globstar?Equivalent to \b-G\b.]"
		"[+gmacs?Enables/disables \bgmacs\b editing mode.  \bgmacs\b "
			"editing mode is the same as \bemacs\b editing mode "
			"except for the handling of \b^T\b.]"
#if SHOPT_BASH
		"[+hashall?Equivalent to \b-h\b and \b-o trackall\b. Available"
		" in bash compatibility mode only.]"
		"[+history?Enable command history. Available in bash "
		"compatibility mode only. On by default in interactive "
		"shells.]"
#endif
#if SHOPT_HISTEXPAND
		"[+histexpand?Equivalent to \b-H\b.]"
#endif
		"[+ignoreeof?Prevents an interactive shell from exiting on "
			"reading an end-of-file.]"
		"[+keyword?Equivalent to \b-k\b.]"
		"[+markdirs?A trailing \b/\b is appended to directories "
			"resulting from pathname expansion.]"
		"[+monitor?Equivalent to \b-m\b.]"
		"[+noclobber?Equivalent to \b-C\b.]"
		"[+noexec?Equivalent to \b-n\b.]"
		"[+noglob?Equivalent to \b-f\b.]"
		"[+nolog?This has no effect.  It is provided for backward "
			"compatibility.]"
		"[+notify?Equivalent to \b-b\b.]"
		"[+nounset?Equivalent to \b-u\b.]"
#if SHOPT_BASH
		"[+onecmd?Equivalent to \b-t\b. Available in bash compatibility "
		"mode only.]"
		"[+physical?Equivalent to \b-P\b. Available in bash "
		"compatibility mode only.]"
		"[+posix?Turn on POSIX compatibility. Available in bash "
		"compatibility mode only. Bash in POSIX mode is not the "
		"same as ksh.]"
#endif
		"[+pipefail?A pipeline will not complete until all components "
			"of the pipeline have completed, and the exit status "
			"of the pipeline will be the value of the last "
			"command to exit with non-zero exit status, or will "
			"be zero if all commands return zero exit status.]"
		"[+privileged?Equivalent to \b-p\b.]"
		"[+trackall?Equivalent to \b-h\b.]"
		"[+verbose?Equivalent to \b-v\b.]"
		"[+vi?Enables/disables \bvi\b editing mode.]"
		"[+viraw?Does not use canonical input mode when using \bvi\b "
			"edit mode.]"
		"[+xtrace?Equivalent to \b-x\b.]"
"}"
"[p?Privileged mode.  Disabling \b-p\b sets the effective user id to the "
	"real user id, and the effective group id to the real group id.  "
	"Enabling \b-p\b restores the effective user and group ids to their "
	"values when the shell was invoked.  The \b-p\b option is on "
	"whenever the real and effective user id is not equal or the "
	"real and effective group id is not equal.  User profiles are "
	"not processed when \b-p\b is enabled.]"
"[t?Obsolete.  The shell reads one command and then exits.]"
"[u?If enabled, the shell displays an error message when it tries to expand "
	"a variable that is unset.]"
"[v?Verbose.  The shell displays its input onto standard error as it "
	"reads it.]"
"[x?Execution trace.  The shell will display each command after all "
	"expansion and before execution preceded by the expanded value "
	"of the \bPS4\b parameter.]"
#if SHOPT_BASH
	"\fbash1\f"
#endif
"[C?Prevents existing regular files from being overwritten using the \b>\b "
	"redirection operator.  The \b>|\b redirection overrides this "
	"\bnoclobber\b option.]"
"[G?Causes \b**\b by itself to also match all sub-directories during pathname "
	"expansion.]"
#if SHOPT_HISTEXPAND
   "[H?Enable \b!\b-style history expansion similar to \bcsh\b.]"
#endif
;

const char sh_optbreak[] =
"[-1c?\n@(#)$Id: break (AT&T Labs Research) 1999-04-07 $\n]"
USAGE_LICENSE
"[+NAME?break - break out of loop ]"
"[+DESCRIPTION?\bbreak\b is a shell special built-in that exits the "
	"smallest enclosing \bfor\b, \bselect\b, \bwhile\b, or \buntil\b loop, "
	"or the \an\a-th enclosing loop if \an\a is specified.  "
	"Execution continues at the command following the loop(s).]"
"[+?If \an\a is given, it must be a positive integer >= 1. If \an\a "
	"is larger than the number of enclosing loops, the last enclosing "
	"loop will be exited.]"
"\n"
"\n[n]\n"
"\n"
"[+EXIT STATUS?0]"
"[+SEE ALSO?\bcontinue\b(1), \breturn\b(1)]"
;

const char sh_optcont[] =
"[-1c?\n@(#)$Id: continue (AT&T Labs Research) 1999-04-07 $\n]"
USAGE_LICENSE
"[+NAME?continue - continue execution at top of the loop]"
"[+DESCRIPTION?\bcontinue\b is a shell special built-in that continues " 
	"execution at the top of smallest enclosing enclosing \bfor\b, "
	"\bselect\b, \bwhile\b, or \buntil\b loop, if any; or the top of "
	"the \an\a-th enclosing loop if \an\a is specified.]"
"[+?If \an\a is given, it must be a positive integer >= 1. If \an\a "
	"is larger than the number of enclosing loops, the last enclosing "
	" loop will be used.]"

"\n"
"\n[n]\n"
"\n"
"[+SEE ALSO?\bbreak\b(1)]"
;

const char sh_optalarm[]	= "r [varname seconds]";
const char sh_optalias[] =
"[-1c?\n@(#)$Id: alias (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?alias - define or display aliases]"
"[+DESCRIPTION?\balias\b creates or redefines alias definitions "
	"or writes the existing alias definitions to standard output.  "
	"An alias definitions provides a string value that will replace "
	"a command name when the command is read.  Alias names can "
	"contain any printable character which is not special to the shell.  "
	"If an alias value ends in a space or tab, then the word "
	"following the command name the alias replaces is also checked "
	"to see whether it is an alias.]"
"[+?If no \aname\as are specified then the names and values of all "
	"aliases are written to standard output.  Otherwise, for "
	"each \aname\a that is specified, and \b=\b\avalue\a  is not "
	"specified, the current value of the alias corresponding to "
	"\aname\a is written to standard output.  If \b=\b\avalue\a is "
	"specified, the alias \aname\a will be created or redefined.]" 
"[+?\balias\b is built-in to the shell as a declaration command so that "
	"field splitting and pathname expansion are not performed on "
	"the arguments.  Tilde expansion occurs on \avalue\a.  An alias "
	"definition only affects scripts read by the current shell "
	"environment.  It does not effect scripts run by this shell.]"
"[p?Causes the output to be in the form of alias commands that can be used "
	"as input to the shell to recreate the current aliases.]"
"[t?Used for tracked aliases.  These are aliases that connect a "
	"command name to the pathname of the command and are reset "
	"when the \bPATH\b variable is unset.  The tracked aliases feature is "
	"now obsolete.]"
"[x?Ignored, this option is obsolete.]"
"\n"
"\n[name[=value]...]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?Successful completion.]"
        "[+>0?One or more \aname\a operands did not have an alias "
		"definition, or an error occurred.]"
"}"

"[+SEE ALSO?\bsh\b(1), \bunalias\b(1)]"
;

const char sh_optbuiltin[] =
"[-1c?\n@(#)$Id: builtin (AT&T Labs Research) 1999-07-10 $\n]"
USAGE_LICENSE
"[+NAME?builtin - add, delete, or display shell built-ins]"
"[+DESCRIPTION?\bbuiltin\b can be used to add, delete, or display "
	"built-in commands in the current shell environment.  A "
	"built-in command executes in the current shell process "
	"and can have side effects in the current shell.  On most "
	"systems, the invocation time for built-in commands is one "
	"or two orders of magnitude less than commands that create "
	"a separate process.]" 
"[+?For each \apathname\a specified, the basename of the pathname "
	"determines the name of the built-in.  For each basename, "
	"the shell looks for a C level function in the current shell "
	"whose name is determined by prepending \bb_\b to the built-in "
	"name.  If \apathname\a contains a \b/\b, then the built-in is "
	"bound to this pathname.  A built-in bound to a pathname will "
	"only be executed if \apathname\a is the first executable "
	"found during a path search.  Otherwise, built-ins are found "
	"prior to performing the path search.]"
"[+?If no \apathname\a operands are specified, then \bbuiltin\b displays "
	"the current list of built-ins, or just the special built-ins if "
	"\b-s\b is specified, on standard output.  The full pathname for "
	"built-ins that are bound to pathnames are displayed.]"
"[+?Libraries containing built-ins can be specified with the \b-f\b "
	"option.  If the library contains a function named \blib_init\b(), "
	"this function will be invoked with argument \b0\b when the "
	"library is loaded.  The \blib_init\b() function can load "
	"built-ins by invoking an appropriate C level function.  In "
	"this case there is no restriction on the C level function name.]"
"[+?The C level function will be invoked with three arguments.  The first "
	"two are the same as \bmain\b() and the third one is a pointer.]"
"[+?\bbuiltin\b cannot be invoked from a restricted shell.]"
"[d?Deletes each of the specified built-ins.  Special built-ins cannot "
	"be deleted.]"
"[f]:[lib?On systems with dynamic linking, \alib\a names a shared library "
	"to load and search for built-ins.  The shared library suffix, which "
	"depends on the system, can be omitted. Once a library is loaded, "
	"its symbols become available for the current and subsequent "
	"invocations of \bbuiltin\b.  Multiple libraries can be specified "
	"with separate invocations of \bbuiltin\b.  Libraries are searched in "
	"the reverse order in which they are specified.]"
"[s?Display only the special built-ins.]"
"\n"
"\n[pathname ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?All \apathname\a operands and \b-f\b options processed "
	"successfully.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bwhence\b(1)]"
;

const char sh_optcd[] =
"[-1c?\n@(#)$Id: cd (AT&T Labs Research) 1999-06-05 $\n]"
USAGE_LICENSE
"[+NAME?cd - change working directory ]"
"[+DESCRIPTION?\bcd\b changes the current working directory of the "
	"current shell environment.]"
"[+?In the first form with one operand, if \adirectory\a begins with "
	"\b/\b, or if the first component is \b.\b or \b..\b, the "
	"directory will be changed to this directory.  If directory is \b-\b, "
	"the directory will be changed to the last directory visited.  " 
	"Otherwise, if the \bCDPATH\b environment variable is set, \bcd\b "
	"searches for \adirectory\a relative to each directory named in "
	"the colon separated list of directories defined by \bCDPATH\b.  "
	"If \bCDPATH\b not set, \bcd\b changes to the directory specified "
	"by \adirectory\a.]"
"[+?In the second form, the first occurrence of the string \aold\a "
	"contained in the pathname of the present working directory "
	"is replaced by the string \anew\a and the resulting string "
	"is used as the directory to which to change.]"
"[+?When invoked without operands and when the \bHOME\b environment "
	"variable is set to a nonempty value,  the directory named by "
	"the \bHOME\b environment variable will be used.  If \bHOME\b "
	"is empty or unset, \bcd\b will fail.]"
"[+?When \bcd\b is successful, the \bPWD\b environment variable will be set "
	"to the name of an absolute pathname that does not contain any "
	"\b..\b components corresponding to the new directory.  The "
	"environment variable \bOLDPWD\b will be set to the previous "
	"value of \bPWD\b.  If the new directory is found by searching "
	"the directories named by \bCDPATH\b, or if \adirectory\a is \b-\b, "
	"or if the two operand form is used, the new value of \bPWD\b will be "
	"written to standard output.]"
"[+?If both \b-L\b and \b-P\b are specified, the last one specified will "
	"be used.  If neither \b-P\b or \b-L\b is specified then the "
	"behavior will be determined by the \bgetconf\b parameter "
	"\bPATH_RESOLVE\b.  If \bPATH_RESOLVE\b is \bphysical\b, "
	"then the behavior will be as if \b-P\b were specified.  Otherwise, "
	"the behavior will be as if  \b-L\b were specified.]"
"[L?Handle each pathname component \b..\b in a logical fashion by moving "
	"up one level by name in the present working directory.]"
"[P?The present working directory is first converted to an absolute pathname "
	"that does not contain symbolic link components and symbolic name "
	"components are expanded in the resulting directory name.]"
"\n"
"\n[directory]\n"
"old new\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?Directory successfully changed.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bpwd\b(1), \bgetconf\b(1)]"
;

const char sh_optcommand[] =
"[-1c?\n@(#)$Id: command (AT&T Labs Research) 2003-06-21 $\n]"
USAGE_LICENSE
"[+NAME?command - execute a simple command]"
"[+DESCRIPTION?Without \b-v\b or \b-V\b,  \bcommand\b executes \acommand\a "
	"with arguments given by \aarg\a, suppressing the shell function lookup "
	"that normally occurs.  In addition, if \acommand\a is a special "
	"built-in command, then the special properties are removed so that "
	"failures will not cause the script that executes it to terminate.]"
"[+?With the \b-v\b or \b-V\b options, \bcommand\b is equivalent to the "
	"\bwhence\b(1) command.]"
"[p?Causes a default path to be searched rather than the one defined by the "
	"value of \bPATH\b.]"
"[v?Equivalent to \bwhence\b \acommand\a [\aarg\a ...]].]"
"[x?If \acommand\a fails because there are too many \aarg\as, it will be "
	"invoked multiple times with a subset of the arguments on each "
	"invocation.  Arguments that occur prior to the first word that "
	"expands to multiple arguments will be passed on each invocation. "
	"The exit status will be the maximum invocation exit status.]"
"[V?Equivalent to \bwhence \b-v\b \acommand\a [\aarg\a ...]].]"
"\n"
"\n[command [arg ...]]\n"
"\n"
"[+EXIT STATUS?If \acommand\a is invoked, the exit status of \bcommand\b "
	"will be that of \acommand\a.  Otherwise, it will be one of "
	"the following:]{"
	"[+0?\bcommand\b completed successfully.]"
	"[+>0?\b-v\b or \b-V\b has been specified and an error occurred.]"
	"[+126?\acommand\a was found but could not be invoked.]"
	"[+127?\acommand\a could not be found.]"
"}"

"[+SEE ALSO?\bwhence\b(1), \bgetconf\b(1)]"
;

const char sh_optdot[]	 =
"[-1c?@(#)$Id: \b.\b (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?\b.\b - execute commands in the current environment]"
"[+DESCRIPTION?\b.\b is a special built-in command that executes commands "
	"from a function or a file in the current environment.]"
"[+?If \aname\a refers to a function defined with the \bfunction\b \aname\a "
	"syntax, the function executes in the current environment as "
	"if it had been defined with the \aname\a\b()\b syntax so that "
	"there is no scoping.  Otherwise, commands from the file defined "
	"by \aname\a are executed in the current environment.  Note that "
	"the complete script is read before it begins to execute so that "
	"any aliases defined in this script will not take effect until "
	"the script completes execution.]"
"[+?When \aname\a refers to a file, the \bPATH\b variable is searched "
	"for the file containing commands.  In this case execute permission "
	"is not required for \aname\a.]" 
"[+?If any \aarg\as are specified, these become the positional parameters "
	"for the duration of the function or script and are restored "
	"upon completion.]"
"\n"
"\n name [arg ...]\n"
"\n"
"[+EXIT STATUS?If \aname\a is found, then the exit status is that "
	"of the last command executed.  Otherwise, since this is a special "
	"built-in, an error will cause a non-interactive shell to exit with "
	"a non-zero exit status.  A interactive shell returns a non-zero exit"
	"status to indicate an error.]"

"[+SEE ALSO?\bcommand\b(1), \bksh\b(1)]"
;

#ifndef ECHOPRINT
    const char sh_optecho[]	= " [-n] [arg...]";
#endif /* !ECHOPRINT */

const char sh_opteval[] =
"[-1c?\n@(#)$Id: eval (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?eval - create a shell command and process it]"
"[+DESCRIPTION?\beval\b is a shell special built-in command that constructs "
	"a command by concatenating the \aarg\as together, separating each "
	"with a space.  The resulting string is then taken as input to "
	"the shell and evaluated in the current environment.  Note that "
	"command words are expanded twice; once to construct \aarg\a, and "
	"again when the shell executes the constructed command.]"
"[+?It is not an error if \aarg\a is not given.]"
"\n"
"\n[arg...]\n"
"\n"
"[+EXIT STATUS?If \aarg\a is not specified, the exit status is \b0\b.  "
	"Otherwise, it is the exit status of the command defined by the "
	"\aarg\a operands.]"
"[+SEE ALSO?\bexec\b(1), \btrap\b(1), \b.\b(1)]"
;

const char sh_optexec[] =
"[-1c?\n@(#)$Id: exec (AT&T Labs Research) 1999-07-10 $\n]"
USAGE_LICENSE
"[+NAME?exec - execute command, open/close and duplicate file descriptors]"
"[+DESCRIPTION?\bexec\b is a special built-in command that can be used to "
	"manipulate file descriptors or to replace the current shell "
	"with a new command.]"
"[+?If \acommand\a is specified, then the current shell process will be "
	"replaced by \acommand\a rather than running \acommand\a and waiting "
	"for it to complete.  Note that there is no need to use "
	"\bexec\b to enhance performance since the shell implicitly "
	"uses the exec mechanism internally whenever possible.]"
"[+?If no operands are specified, \bexec\b can be used to open or "
	"close files, or to manipulate file descriptors from \b0\b to "
	"\b9\b in the current shell environment using the standard "
	"redirection mechanism available with all commands.  The " 
	"close-on-exec flags will be set on file descriptor numbers "
	"greater than \b2\b that are opened this way so that they "
	"will be closed when another program is invoked.]"
"[+?Because \bexec\b is a special command, any failure will cause the "
	"script that invokes it to exit.  This can be prevented by "
	"invoking \bexec\b from the \bcommand\b utility.]"
"[+?\bexec\b cannot be invoked from a restricted shell to create "
	"files or to open a file for writing or appending.]"
"[c?Clear all environment variables before executions except variable "
	"assignments that are part of the current \bexec\b command.]"
"[a]:[name?\bargv[0]]\b will be set to \aname\a for \acommand\a]"
"\n"
"\n[command [arg ...]]\n"
"\n"
"[+EXIT STATUS?If \acommand\a is specified, \bexec\b does not return.  "
	"Otherwise, the exit status is one of the following:]{"
	"[+0?All I/O redirections were successful.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bcommand\b(1), \beval\b(1)]"
;


const char sh_optexit[] =
"[-1c?\n@(#)$Id: exit (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?exit - exit the current shell]"
"[+DESCRIPTION?\bexit\b is shell special built-in that causes the "
	"shell that invokes it to exit.  Before exiting the shell, if the "
	"\bEXIT\b trap is set it will be invoked.]"
"[+?If \an\a is given, it will be used to set the exit status.]"
"\n"
"\n[n]\n"
"\n"
"[+EXIT STATUS?If \an\a is specified, the exit status is the least significant "
	"eight bits of the value of \an\a.  Otherwise, the exit status is the "
	"exit status of preceding command.  When invoked inside a trap, the "
	"preceding command means the command that invoked the trap.]"
"[+SEE ALSO?\bbreak\b(1), \breturn\b(1)]"
;

const char sh_optexport[] =
"[-1c?\n@(#)$Id: export (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?export - set export attribute on variables]"
"[+DESCRIPTION?\bexport\b sets the export attribute on each of "
	"the variables specified by \aname\a which causes them "
	"to be in the environment of subsequently executed commands.  "
	"If \b=\b\avalue\a is specified, the variable \aname\a is "
	"set to \avalue\a.]"
"[+?If no \aname\as are specified then the names and values of all "
	"exported variables are written to standard output.]" 
"[+?\bexport\b is built-in to the shell as a declaration command so that "
	"field splitting and pathname expansion are not performed on "
	"the arguments.  Tilde expansion occurs on \avalue\a.]"
"[p?Causes the output to be in the form of \bexport\b commands that can be "
	"used as input to the shell to recreate the current exports.]"
"\n"
"\n[name[=value]...]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?Successful completion.]"
        "[+>0?An error occurred.]"
"}"

"[+SEE ALSO?\bsh\b(1), \btypeset\b(1)]"
;

const char sh_optgetopts[] =
":[-1c?\n@(#)$Id: getopts (AT&T Labs Research) 1999-07-20 $\n]"
"[-author?Glenn Fowler <gsf@research.att.com>]"
USAGE_LICENSE
"[+NAME?\f?\f - parse utility options]"
"[+DESCRIPTION?The \bgetopts\b utility can be used to retrieve options and "
  "arguments from a list of arguments given by \aargs\a or the positional "
  "parameters if \aargs\a is omitted.  It can also generate usage messages "
  "and a man page for the command based on the information in \aoptstring\a.]"
"[+?Each time it is invoked, the \bgetopts\b utility places the value "
  "of the next option in the shell variable specified by the \aname\a "
  "operand and the index of the next argument to be processed in the "
  "shell variable \bOPTIND\b.  When the shell is invoked \bOPTIND\b "
  "is initialized to \b1\b.  When an option requires or permits an option "
  "argument, \bgetopts\b places the option argument in the shell "
  "variable \bOPTARG\b.]"
"[+?The \aoptstring\a string consists of alpha-numeric characters, "
  "the special characters +, -, ?, :, and <space>, or character groups "
  "enclosed in [...]].  Character groups may be nested in {...}. "
  "Outside of a [...]] group, a single new-line followed by zero or "
  "more blanks is ignored.  One or more blank lines separate the "
  "options from the command argument synopsis.]"
"[+?Each [...]] group consists of an optional label, "
  "optional attributes separated by :, and an "
  "optional description string following ?.  The characters from the ? "
  "to the end of the next ]] are ignored for option parsing and short "
  "usage messages.  They are used for generating verbose help or man pages. "
  "The : character may not appear in the label.  "
  "The ? character must be specified as ?? in the label and the ]] character "
  "must be specified as ]]]] in the description string. "
  "Text between two \\b (backspace) characters indicates "
  "that the text should be emboldened when displayed. "
  "Text between two \\a (bell) characters indicates that the text should "
  "be emphasised or italicized when displayed."
  "Text between two \\v (vertical tab) characters indicates "
  "that the text should displayed in a fixed width font. "
  "Text between two \\f (formfeed) characters will be replaced by the "
    "output from the shell function whose name is that of the enclosed text.]"
"[+?There are five types of groups:]{"
  "[+1.?An option specification of the form \aoption\a:\alongname\a. "
    "In this case the first field is the option character.  If there "
    "is no option character, then a two digit number should be specified "
    "that corresponds to the long option.  This negative of this number "
    "will be returned as the value of \aname\a by \bgetopts\b if the long "
    "option is matched. A longname is matched with \b--\b\alongname\a.  A "
    "* in the \alongname\a field indicates that only characters up to that "
    "point need to match, provided any additional characters match the option. "
    "The [ and ]] can be omitted for an option that does not have a longname "
    "or descriptive text.]"
  "[+2.?A string option argument specification. "
    "Options that take arguments can be followed by : or # and an option "
    "group specification.  An option group specification consists "
    "of a name for the option argument as field 1.   The remaining "
    "fields are a type name and zero or more of the special attribute words "
    "\blistof\b, \boneof\b, and \bignorecase\b. "
    "The option specification can be followed "
    "by a list of option value descriptions enclosed in parenthesis.]"
  "[+3.?A option value description.]"
  "[+4.?A argument specification. A list of valid option argument values "
    "can be specified by enclosing them inside a {...} following "
    "the option argument specification.  Each of the permitted "
    "values can be specified with a [...]] containing the "
    "value followed by a description.]"
  "[+5.?A group of the form [+\\n...]] will display the characters "
    "representing ... in fixed with font wihtout adding line breaks.]"
"}"
"[+?If the leading character of \aoptstring\a is +, then arguments "
  "beginning with + will also be considered options.]"
"[+?A leading : character or a : following a leading + in \aoptstring\a "
  "affects the way errors are handled.  If an option character or longname "
  "argument not specified in \aoptstring\a is encountered when processing "
  "options, the shell variable whose name is \aname\a will be set to the ? "
  "character.  The shell variable \bOPTARG\b will be set to "
  "the character found.  If an option argument is missing or has an invalid "
  "value, then \aname\a will be set to the : character and the shell variable "
  "\bOPTARG\b will be set to the option character found. "
  "Without the leading :, \aname\a will be set to the ? character, \bOPTARG\b "
  "will be unset, and an error message will be written to standard error "
  "when errors are encountered.]"
"[+?The end of options occurs when:]{"
	"[+1.?The special argument \b--\b is encountered.]"
	"[+2.?An argument that does not begin with a \b-\b is encountered.]"
	"[+3.?A help argument is specified.]"
	"[+4.?An error is encountered.]"
"}"
"[+?If \bOPTARG\b is set to the value \b1\b, a new set of arguments "
  "can be used.]"
"[+?\bgetopts\b can also be used to generate help messages containing command "
  "usage and detailed descriptions.  Specify \aargs\a as:]"
"{ "
	"[+-???To generate a usage synopsis.]"
	"[+--?????To generate a verbose usage message.]"
	"[+--????man?To generate a formatted man page.]"
	"[+--????api?To generate an easy to parse usage message.]"
	"[+--????html?To generate a man page in \bhtml\b format.]"
	"[+--????nroff?To generate a man page in \bnroff\b format.]"
"}"
"[+?When the end of options is encountered, \bgetopts\b exits with a "
  "non-zero return value and the variable \bOPTIND\b is set to the "
  "index of the first non-option argument.]"
"a:[name?Use \aname\a instead of the command name in usage messages.]"
"\n"
"\nopstring name [args...]\n"
"\n"
"[+EXIT STATUS]{"
	"[+0?An option specified was found.]"
	"[+1?An end of options was encountered.]"
	"[+2?A usage or information message was generated.]"
"}"
;

const char sh_optbg[] =
"[-1c?@(#)$Id: bg (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?bg - resume jobs in the background]"
"[+DESCRIPTION?\bbg\b places the given \ajob\as into the background "
	"and sends them a \bCONT\b signal to start them running.]"
"[+?If \ajob\a is omitted, the most recently started or stopped "
	"background job is resumed or continued in the background.]"
"[+?Each \ajob\a can be specified as one of the following:]{"
	"[+\anumber\a?\anumber\a refers to a process id.]"
	"[+-\anumber\a?\anumber\a refers to a process group id.]"
	"[+%\anumber\a?\anumber\a refer to a job number.]"
	"[+%\astring\a?Refers to a job whose name begins with \astring\a.]"
	"[+%??\astring\a?Refers to a job whose name contains \astring\a.]"
	"[+%+ \bor\b %%?Refers to the current job.]"
	"[+%-?Refers to the previous job.]"
"}"
"\n"
"\n[job ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?If all background jobs are started.]"
	"[+>0?If one more jobs does not exist or there are no background "
		"jobs.]" 
"}"

"[+SEE ALSO?\bwait\b(1), \bfg\b(1), \bdisown\b(1), \bjobs\b(1)]"
;

const char sh_optfg[] =
"[-1c?@(#)$Id: fg (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?fg - move jobs to the foreground]"
"[+DESCRIPTION?\bfg\b places the given \ajob\as into the foreground "
	"in sequence and sends them a \bCONT\b signal to start each running.]"
"[+?If \ajob\a is omitted, the most recently started or stopped "
	"background job is moved to the foreground.]"
"[+?Each \ajob\a can be specified as one of the following:]{"
	"[+\anumber\a?\anumber\a refers to a process id.]"
	"[+-\anumber\a?\anumber\a refers to a process group id.]"
	"[+%\anumber\a?\anumber\a refer to a job number.]"
	"[+%\astring\a?Refers to a job whose name begins with \astring\a.]"
	"[+%??\astring\a?Refers to a job whose name contains \astring\a.]"
	"[+%+ \bor\b %%?Refers to the current job.]"
	"[+%-?Refers to the previous job.]"
"}"
"\n"
"\n[job ...]\n"
"\n"
"[+EXIT STATUS?If \bfg\b brings one or more jobs into the foreground, "
	"the exit status of \bfg\b  will be that of the last \ajob\a.  "
	"If one or more jobs does not exist or has completed, \bfg\b will "
	"return a non-zero exit status.]"
"}"

"[+SEE ALSO?\bwait\b(1), \bbg\b(1), \bjobs\b(1)]"
;

const char sh_optdisown[] =
"[-1c?@(#)$Id: disown (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?disown - disassociate a job with the current shell]"
"[+DESCRIPTION?\bdisown\b prevents the current shell from sending "
	"a \bHUP\b signal to each of the given \ajob\as when "
	"the current shell terminates a login session.]"
"[+?If \ajob\a is omitted, the most recently started or stopped "
	"background job is used.]"
"[+?Each \ajob\a can be specified as one of the following:]{"
	"[+\anumber\a?\anumber\a refers to a process id.]"
	"[+-\anumber\a?\anumber\a refers to a process group id.]"
	"[+%\anumber\a?\anumber\a refer to a job number.]"
	"[+%\astring\a?Refers to a job whose name begins with \astring\a.]"
	"[+%??\astring\a?Refers to a job whose name contains \astring\a.]"
	"[+%+ \bor\b %%?Refers to the current job.]"
	"[+%-?Refers to the previous job.]"
"}"
"\n"
"\n[job ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?If all jobs are sucessfully disowned.]"
	"[+>0?If one more \ajob\as does not exist.]"
"}"

"[+SEE ALSO?\bwait\b(1), \bbg\b(1), \bjobs\b(1)]"
;

const char sh_optjobs[] =
"[-1c?@(#)$Id: jobs (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?jobs - display status of jobs]"
"[+DESCRIPTION?\bjobs\b displays information about specified \ajob\as "
	"that were started by the current shell environment on standard "
	"output.  The information contains the job number enclosed in "
	"[...]], the status, and the command line that started the job.]"
"[+?If \ajob\a is omitted, \bjobs\b displays the status of all stopped jobs, "
	"background jobs, and all jobs whose status has changed since last "
	"reported by the shell.]"
"[+?When \bjobs\b reports the termination status of a job, the "
	"shell removes the jobs from the list of known jobs in "
	"the current shell environment.]"
"[+?Each \ajob\a can be specified as one of the following:]{"
	"[+\anumber\a?\anumber\a refers to a process id.]"
	"[+-\anumber\a?\anumber\a refers to a process group id.]"
	"[+%\anumber\a?\anumber\a refer to a job number.]"
	"[+%\astring\a?Refers to a job whose name begins with \astring\a.]"
	"[+%??\astring\a?Refers to a job whose name contains \astring\a.]"
	"[+%+ \bor\b %%?Refers to the current job.]"
	"[+%-?Refers to the previous job.]"
"}"
"[l?\bjobs\b displays process id's after the job number in addition "
	"to the usual information]"
"[n?Only the jobs whose status has changed since the last prompt "
	"is displayed.]"
"[p?The process group leader id's for the specified jobs are displayed.]"
"\n"
"\n[job ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?The information for each job is written to standard output.]"
	"[+>0?One or more jobs does not exist.]"
"}"

"[+SEE ALSO?\bwait\b(1), \bps\b(1), \bfg\b(1), \bbg\b(1)]"
;

const char sh_opthist[]	= 
"[-1c?@(#)$Id: hist (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?\f?\f - process command history list]"
"[+DESCRIPTION?\b\f?\f\b lists, edits, or re-executes, commands  "
	"previously entered into the current shell environment.]"
"[+?The command history list references commands by number. The first number "
	"in the list is selected arbitrarily.  The relationship of a number "
	"to its command does not change during a login session.  When the "
	"number reaches 32767 the number wraps around to 1 but "
	"maintains the ordering.]"
"[+?When commands are edited (when the \b-l\b option is not specified), the "
	"resulting lines will be entered at the end of the history list and "
	"then reexecuted by the current shell.  The \b\f?\f\b command that "
	"caused the editing will not be entered into the history list.  If the "
	"editor returns a non-zero exit status, this will suppress the "
	"entry into the history list and the command reexecution.  Command "
	"line variable assignments and redirections affect both the \f?\f "
	"command and the commands that are reexecuted.]"
"[+?\afirst\a and \alast\a define the range of commands. \afirst\a and "
		"\alast\a can be one of the following:]{"
		"[+\anumber\a?A positive number representing a command "
			"number.  A \b+\b sign can precede \anumber\a.]"
		"[+-\anumber\a?A negative number representing a command "
			"that was executed \anumber\a commands previously. "
			"For example, \b-1\b is the previous command.]"
		"[+\astring\a?\astring\a indicates the most recently "
			"entered command that begins with \astring\a. "
			"\astring\a should not contain an \b=\b.]"
	"}"
"[+?If \afirst\a is omitted, the previous command is used, unless \b-l\b "
	"is specified, in which case it will default to \b-16\b and \alast\a "
	"will default to \b-1\b.]"
"[+?If \afirst\a is specified and \alast\a is omitted, then \alast\a will "
	"default to \afirst\a unless \b-l\b is specified in which case "
	"it will default to \b-1\b.]"
"[+?If no editor is specified, then the editor specfied by the \bHISTEDIT\b "
	"variable will be used if set, or the \bFCEDIT\b variable will be "
	"used if set, otherwise, \bed\b will be used.]"
"[e]:[editor?\aeditor\a specifies the editor to use to edit the history "
	"command.   A value of \b-\b for \aeditor\a is equivalent to "
	"specifiying the \b-s\b option.]"
"[l?List the commands rather than editing and reexecuting them.]"
"[N]#[num?Start at \anum\a commands back.]" 
"[n?Suppress the command numbers when the commands are listed.]"
#if SHOPT_HISTEXPAND
"[p?Writes the result of history expansion for each operand to standard "
	"output.  All other options are ignored.]"
#endif
"[r?Reverse the order of the commands.]"
"[s?Reexecute the command without invoking an editor.  In this case "
	"an operand of the form \aold\a\b-\b\anew\a can be specified "
	"to change the first occurrence of the string \aold\a in the "
	"command to \anew\a before reexecuting the command.]"

"\n"
"\n[first [last] ]\n"
"\n"
"[+EXIT STATUS?If a command is reexecuted, the exit status is that of "
	"the command that gets reexecuted.  Otherwise, it is one of the "
	"following:]{"
	"[+0?Successfully completion of the listing.]"
	"[+>0?An error occurred.]" 
"}"

"[+SEE ALSO?\bksh\b(1), \bsh\b(1), \bed\b(1)]"
;

const char sh_optkill[]	 = 
"[-1c?\n@(#)$Id: kill (AT&T Labs Research) 1999-06-17 $\n]"
USAGE_LICENSE
"[+NAME?kill - terminate or signal process]"
"[+DESCRIPTION?With the first form in which \b-l\b is not specified, "
	"\bkill\b sends a signal to one or more processes specified by "
	"\ajob\a.  This normally terminates the processes unless the signal "
	"is being caught or ignored.]"
"[+?A \ajob\a can be specified as one of the following:]{"
	"[+\anumber\a?\anumber\a refers to a process id.]"
	"[+-\anumber\a?\anumber\a refers to a process group id.]"
	"[+%\anumber\a?\anumber\a refer to a job number.]"
	"[+%\astring\a?Refers to a job whose name begins with \astring\a.]"
	"[+%??\astring\a?Refers to a job whose name contains \astring\a.]"
	"[+%+ \bor\b %%?Refers to the current job.]"
	"[+%-?Refers to the previous job.]"
"}"
"[+?If the signal is not specified with either the \b-n\b or the \b-s\b  "
	"option, the \bSIGTERM\b signal is used.]"
"[+?If \b-l\b is specified, and no \aarg\a is specified, then \bkill\b "
	"writes the list of signals to standard output.  Otherwise, \aarg\a "
	"can be either a signal name, or a number representing either a "
	"signal number or exit status for a process that was terminated "
	"due to a signal.  If a name is given the corresponding signal "
	"number will be written to standard output.  If a number is given "
	"the corresponding signal name will be written to standard output.]"
"[l?List signal names or signal numbers rather than sending signals as "
	"described above.  "
	"The \b-n\b and \b-s\b options cannot be specified.]"
"[n]#[signum?Specify a signal number to send.  Signal numbers are not "
	"portable across platforms, except for the following:]{"
		"[+0?No signal]"
		"[+1?\bHUP\b]"
		"[+2?\bINT\b]"
		"[+3?\bQUIT\b]"
		"[+6?\bABRT\b]"
		"[+9?\bKILL\b]"
		"[+14?\bALRM\b]"
		"[+15?\bTERM\b]"
	"}"
"[s]:[signame?Specify a signal name to send.  The signal names are derived "
	"from their names in \b<signal.h>\b without the \bSIG\b prefix and "
	"are case insensitive.  \bkill -l\b will generate the list of "
	"signals on the current platform.]"
"\n"
"\njob ...\n"
" -l [arg ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?At least one matching process was found for each \ajob\a "
	"operand, and the specified signal was successfully sent to at "
	"least one matching process.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bps\b(1), \bjobs\b(1), \bkill\b(2), \bsignal\b(2)]"
;

const char sh_optlet[]	=
"[-1c?@(#)$Id: let (AT&T Labs Research) 2000-04-02 $\n]"
USAGE_LICENSE
"[+NAME?let - evaluate arithmetic expressions]"
"[+DESCRIPTION?\blet\b evaluates each \aexpr\a in the current "
	"shell environment as an arithmetic expression using ANSI C "
	"syntax.  Variables names are shell variables and they "
	"are recursively evaluated as arithmetic expressions to "
	"get numerical values.]"
"[+?\blet\b has been made obsolete by the \b((\b...\b))\b syntax "
	"of \bksh\b(1) which does not require quoting of the operators "
	"to pass them as command arguments.]"
"\n"
"\n[expr ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?The last \aexpr\a evaluates to a non-zero value.]"
	"[+>0?The last \aexpr\a evaluates to \b0\b or an error occurred.]"
"}"

"[+SEE ALSO?\bexpr\b(1), \btest\b(1), \bksh\b(1)]"
;

const char sh_optprint[] =
"[-1c?\n@(#)$Id: print (AT&T Labs Research) 1999-04-07 $\n]"
USAGE_LICENSE
"[+NAME?print - write arguments to standard output]"
"[+DESCRIPTION?By default, \bprint\b writes each \astring\a operand to "
	"standard output and appends a newline character.]"  
"[+?Unless, the \b-r\b or \b-f\b option is specifed, each \b\\\b "
	"character in each \astring\a operand is processed specially as "
	"follows:]{"
		"[+\\a?Alert character.]"
		"[+\\b?Backspace character.]"
		"[+\\c?Terminate output without appending newline.  The "
			"remaining \astring\a operands are ignored.]"
		"[+\\f?Formfeed character.]"
		"[+\\n?Newline character.]"
		"[+\\t?Tab character.]"
		"[+\\v?Vertical tab character.]"
		"[+\\\\?Backslash character.]"
		"[+\\E?Escape character (ASCII octal 033).]"
		"[+\\0\ax\a?The 8-bit character whose ASCII code is the "
			"1-, 2-, or 3-digit octal number \ax\a.]"
	"}"
"[+?If both \b-e\b and \b-r\b are specified, the last one specified is "
	"the one that is used.]"
"[+?When the \b-f\b option is specified and there are more \astring\a "
	"operands than format specifiers, the format string is "
	"reprocessed from the beginning.  If there are fewer \astring\a "
	"operands than format specifiers, then outputting will end "
	"at the first unneeded format specifier.]" 
"[e?Unless \b-f\b is specified, process \b\\\b sequences in each \astring\a "
	"operand as described above. This is the default behavior.]"
"[n?Do not append a new-line character to the output.]"
"[f]:[format?Write the \astring\a arguments using the format string "
	"\aformat\a and do not append a new-line.  See \bprintf\b for "
	"details on how to specify \aformat\a.]"
"[p?Write to the current co-process instead of standard output.]"
"[r?Do not process \b\\\b sequences in each \astring\a operand as described "
	"above.]"
"[s?Write the output as an entry in the shell history file instead of "
	"standard output.]"
"[u]:[fd:=1?Write to file descriptor number \afd\a instead of standard output.]"
"\n"
"\n[string ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?Successful completion.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\becho\b(1), \bprintf\b(1), \bread\b(1)]"
;

const char sh_optprintf[] =
"[-1c?\n@(#)$Id: printf (AT&T Labs Research) 1999-06-21 $\n]"
USAGE_LICENSE
"[+NAME?printf - write formatted output]"
"[+DESCRIPTION?\bprintf\b writes each \astring\a operand to "
	"standard output using \aformat\a to control the output format.]"  
"[+?The \aformat\a operands supports the full range of ANSI C formatting "
	"specifiers plus the following additional specifiers:]{"
	"[+%b?Each character in the \astring\a operand is processed "
		"specially as follows:]{"
			"[+\\a?Alert character.]"
			"[+\\b?Backspace character.]"
			"[+\\c?Terminate output without appending newline. "
			    "The remaining \astring\a operands are ignored.]"
			"[+\\f?Formfeed character.]"
			"[+\\n?Newline character.]"
			"[+\\t?Tab character.]"
			"[+\\v?Vertical tab character.]"
			"[+\\\\?Backslash character.]"
			"[+\\E?Escape character (ASCII octal 033).]"
			"[+\\0\ax\a?The 8-bit character whose ASCII code is "
				"the 1-, 2-, or 3-digit octal number \ax\a.]"
		"}"
	"[+%q?Output \astring\a quoted in a manner that it can be read in "
		"by the shell to get back the same string.]"
	"[+%B?Treat the argument as a variable name and output the value "
		"without converting it to a string.  This is most useful for "
		"variables of type \b-b\b.]"
	"[+%H?Output \astring\a with characters \b<\b, \b&\b, \b>\b, "
		"\b\"\b, and non-printable characters properly escaped for "
		"use in HTML and XML documents.]"
	"[+%P?Treat \astring\a as an extended regular expression and  "
		"convert it to a shell pattern.]"
	"[+%R?Treat \astring\a as an shell pattern expression and  "
		"convert it to an extended regular expression.]"
	"[+%T?Treat \astring\a as a date/time string and format it.  The "
		"\bT\b can be preceded by \b(\b\adformat\a\b)\b, where "
		"\adformat\a is a date format as defined by the \bdate\b "
		"command.]"
	"[+%Z?Output a byte whose value is \b0\b.]"
"}"
"[+?When performing conversions of \astring\a to satisfy a numeric "
	"format specifier, if the first character of \astring\a "
	"is \b\"\b or \b'\b, then the value will be the numeric value "
	"in the underlying code set of the character following the "
	"\b\"\b or \b'\b.  Otherwise, \astring\a is treated like a shell "
	"arithmetic expression and evaluated.]"
"[+?If a \astring\a operand cannot be completed converted into a value "
	"appropriate for that format specifier, an error will occur, "
	"but remaining \astring\a operands will continue to be processed.]"
"[+?In addition to the format specifier extensions, the following "
	"extensions of ANSI-C are permitted in format specifiers:]{"
	"[+-?The escape sequences \b\\E\b and \b\\e\b expand to the escape "
		"character which is octal \b033\b in ASCII.]"
	"[+-?The escape sequence \b\\c\b\ax\a expands to Control-\ax\a.]"
	"[+-?The escape sequence \b\\C[.\b\aname\a\b.]]\b expands to "
		"the collating element \aname\a.]"
	"[+-?The escape sequence \b\\x{\b\ahex\a\b}\b expands to the "
		"character corresponding to the hexidecimal value \ahex\a.]"
	"[+-?Each of the integral format specifiers can have a third "
		"modifier after width and precision that specifies the "
		"base of the conversion from 2 to 64.]"
	"}"
"[+?If there are more \astring\a operands than format specifiers, the "
	"\aformat\a string is reprocessed from the beginning.  If there are "
	"fewer \astring\a operands than format specifiers, then string "
	"specifiers will be treated as if null strings were supplied, "
	"numeric conversions will be treated as if 0 were supplied, and "
	"time conversions will be treated as if \bnow\b were supplied.]"
"[+?\bprintf\b is equivalent to \bprint -f\b which allows additional "
	"options to be specified.]"
"\n"
"\nformat [string ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?Successful completion.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bdate\b(1), \bprint\b(1), \bread\b(1)]"
;

const char sh_optpwd[] =
"[-1c?\n@(#)$Id: pwd (AT&T Labs Research) 1999-06-07 $\n]"
USAGE_LICENSE
"[+NAME?pwd - write working directory name]"
"[+DESCRIPTION?\bpwd\b writes an absolute pathname of the current working "
	"directory to standard output.   An absolute pathname is a "
	"pathname that begins with \b/\b that does not contains any "
	"\b.\b  or \b..\b components.]"
"[+?If both \b-L\b and \b-P\b are specified, the last one specified will "
	"be used.  If neither \b-P\b or \b-L\b is specified then the "
	"behavior will be determined by the \bgetconf\b parameter "
	"\bPATH_RESOLVE\b.  If \bPATH_RESOLVE\b is \bphysical\b, "
	"then the behavior will be as if \b-P\b were specified.  Otherwise, "
	"the behavior will be as if  \b-L\b were specified.]"
"[L?The absolute pathname may contains symbolic link components.  This is "
	"the default.]"
"[P?The absolute pathname will not contain any symbolic link components.]"
"[+EXIT STATUS?]{"
	"[+0?Successful completion.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bcd\b(1), \bgetconf\b(1)]"
;

const char sh_optread[] =
"[-1c?\n@(#)$Id: read (AT&T Labs Research) 2003-05-19 $\n]"
USAGE_LICENSE
"[+NAME?read - read a line from standard input]"
"[+DESCRIPTION?\bread\b reads a line from standard input and breaks it "
	"into fields using the characters in value of the \bIFS\b variable "
	"as separators.  The escape character, \b\\\b, is used to remove "
	"any special meaning for the next character and for line continuation "
	"unless the \b-r\b option is specified.]"
"[+?If there are more variables than fields, the remaining variables are "
	"set to empty strings.  If there are fewer variables than fields, "
	"the leftover fields and their intervening separators are assigned "
	"to the last variable.  If no \avar\a is specifed then the variable "
	"\bREPLY\b is used.]"
"[+?If you specify \b?\b\aprompt\a after the first \avar\a, then \bread\b "
	"will display \aprompt\a on standard error when standard input "
	"is a terminal or pipe.]"
"[A?Unset \avar\a and then create an indexed array containing each field in "
	"the line starting at index 0.]"
"[d]:[delim?Read until delimiter \adelim\a instead of to the end of line.]"
"[p?Read from the current co-process instead of standard input.  An end of "
	"file causes \bread\b to disconnect the co-process so that another "
	"can be created.]"
"[r?Do not treat \b\\\b specially when processing the input line.]"
"[s?Save a copy of the input as an entry in the shell history file.]"
"[u]#[fd:=0?Read from file descriptor number \afd\a instead of standard input.]"
"[t]#[timeout?Specify a timeout \atimeout\a in seconds when reading from "
	"a terminal or pipe.]"
"[n]#[nbyte?Read at most \anbyte\a bytes.]"
"[N]#[nbyte?Read exactly \anbyte\a bytes.]"
"\n"
"\n[var?prompt] [var ...]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0? Successful completion.]"
	"[+>0?End of file was detected or an error occurred.]"
"}"
"[+SEE ALSO?\bprint\b(1), \bprintf\b(1), \bcat\b(1)]"
;

const char sh_optreadonly[] =
"[-1c?\n@(#)$Id: readonly (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?readonly - set readonly attribute on variables]"
"[+DESCRIPTION?\breadonly\b sets the readonly attribute on each of "
	"the variables specified by \aname\a which prevents their "
	"values from being changed.  If \b=\b\avalue\a is specified, "
	"the variable \aname\a is set to \avalue\a before the variable "
	"is made readonly.]"
"[+?If no \aname\as are specified then the names and values of all "
	"readonly variables are written to standard output.]" 
"[+?\breadonly\b is built-in to the shell as a declaration command so that "
	"field splitting and pathname expansion are not performed on "
	"the arguments.  Tilde expansion occurs on \avalue\a.]"
"[p?Causes the output to be in a form of \breadonly\b commands that can be "
	"used as input to the shell to recreate the current set of "
	"readonly variables.]"
"\n"
"\n[name[=value]...]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?Successful completion.]"
        "[+>0?An error occurred.]"
"}"

"[+SEE ALSO?\bsh\b(1), \btypeset\b(1)]"
;

const char sh_optreturn[] =
"[-1c?\n@(#)$Id: return (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?return - return from a function or dot script ]"
"[+DESCRIPTION?\breturn\b is a shell special built-in that causes the "
	"function or dot script that invokes it to exit.  "
	"If \breturn\b is invoked outside of a function or dot script "
	"it is equivalent to \bexit\b.]"
"[+?If \breturn\b is invoked inside a function defined with the \bfunction\b "
	"reserved word syntax, then any \bEXIT\b trap set within the "
	"then function will be invoked in the context of the caller "
	"before the function returns.]"
"[+?If \an\a is given, it will be used to set the exit status.]"
"\n"
"\n[n]\n"
"\n"
"[+EXIT STATUS?If \an\a is specified, the exit status is the least significant "
	"eight bits of the value of \an\a.  Otherwise, the exit status is the "
	"exit status of preceding command.]"
"[+SEE ALSO?\bbreak\b(1), \bexit\b(1)]"
;


const char sh_optksh[] =
"+[-1c?\n@(#)$Id: sh (AT&T Labs Research) "SH_RELEASE" $\n]"
USAGE_LICENSE
"[+NAME?\b\f?\f\b - Shell, the standard command language interpreter]"
"[+DESCRIPTION?\b\f?\f\b is a command language interpreter that "
	"executes commands read from a command line string, the "
	"standard input, or a specified file.]"
"[+?If the \b-i\b option is present, or there are no \aarg\as and "
	"the standard input and standard error are attached to a "
	"terminal, the shell is considered to be interactive.]"
"[+?The \b-s\b and \b-c\b options are mutually exclusive.  If the \b-c\b "
	"option is specified, the first \aarg\a is the command-line string "
	"and must be specified.  Any remaining \aarg\as will be used "
	"to initialize \b$0\b and positional parameters.]" 
"[+?If the neither \b-s\b nor \b-c\b is specified, then the first \barg\b "
	"will be the pathname of the file containing commands and \b$0\b "
	"will be set to this value.  If there is no file with this pathname, "
	"and this pathame does not contain a \b/\b, then the \bPATH\b "
	"will be searched for an executable with this name.  Any remaining "
	"\aarg\as will be used to initialize the positional parmaeters.]"
"[+?Any option can use a \b+\b instead of a \b-\b to disable the corresponding "
	"option.]"
"[c?Read the commands from the first \aarg\a.]"
"[i?Specifies that the shell is interactive.]"
"[r\f:restricted\f?Invoke the shell in a restricted mode.  A restricted "
	"shell does not permit any of the following:]{"
	"[+-?Changing the working directory.]"
	"[+-?Setting values or attributes of the variables \bSHELL\b, "
		"\bENV\b, or \bPATH\b.]"
	"[+-?Executing any command whose name as a \b/\b in it.]"
	"[+-?Redirecting output of a command with \b>\b, \b>|\b, "
		"\b<>\b, or \b>>\b.]"
	"[+-?Adding or deleting built-in commands or libraries with "
		"\bbuiltin\b.]"
	"}"
"[s?Read the commands from standard input.  The positional parameters will be "
	"initialized from \aarg\a.]"
"[D\f:dump-strings\f?Do not execute the script, but output the set of double "
	"quoted strings preceded by a \b$\b.  These strings are needed for "
	"localization of the script to different locales.]"
#if SHOPT_PFSH
"[P?Invoke the shell as a profile shell.  See \bpfexec\b(1).]"
#endif
#if SHOPT_KIA
"[R]:[file?Do not execute the script, but create a cross reference database "
	"in \afile\a that can be used a separate shell script browser.]"
#endif /* SHOPT_KIA */
#if SHOPT_BASH
   "\fbash2\f"
#endif
"\fabc\f"
"\n"
"\n[arg ...]\n"
"\n"
"[+EXIT STATUS?If \b\f?\f\b executes command, the exit status will be that "
        "of the last command executed.  Otherwise, it will be one of"
        "the following:]{"
        "[+0?The script or command line to be executed consists entirely "
		"of zero or more blank lines or comments.]"
        "[+>1-125?A noninteractive shell detected a syntax error, a variable "
		"assignment error, or an error in a special built-in.]"
	"[+126?\b-c\b and \b-s\b were not specified and the command script "
		"was found on \bPATH\b but was not executable.]"
	"[+127?\b-c\b and \b-s\b were not specified and the command script "
		"corresponding to \aarg\a could not be found.]"
"}"

"[+SEE ALSO?\bset\b(1), \bbuiltin\b(1)]"
;
const char sh_optset[] =
"+[-1c?\n@(#)$Id: set (AT&T Labs Research) 1999-09-28 $\n]"
USAGE_LICENSE
"[+NAME?set - set/unset options and positional parameters]"
"[+DESCRIPTION?\bset\b sets or unsets options and positional parameters.  "
	"Options that are specified with a \b-\b cause the options to "
	"be set.  Options that are specified with a \b+\b cause the "
	"option to be unset.]"
"[+?\bset\b without any options or arguments displays the names and "
	"values of all shell variables in the order of the collation "
	"sequence in the current locale.  The values are quoted so that "
	"they are suitable for reinput to the shell.]"
"[+?If no \aarg\as are specified, not even the end of options argument \b--\b, "
	"the positional parameters are unchanged.  Otherwise, unless "
	"the \b-A\b options has been specified, the positional parameters "
	"are replaced by the list of \aarg\as.  A first \aarg\a of "
	"\b--\b is ignored when setting positional parameters.]"
"[+?For backward compatibility, a \bset\b command without any options "
	"specified whose first \aarg\a is \b-\b will turn off "
	"the \b-v\b and \b-x\b options.  If any additional \aarg\as "
	"are specified, they will replace the positional parameters.]"
"[s?Sort the positional parameters.]"
"[A]:[name?Assign the arguments sequentially to the array named by \aname\a "
	"starting at subscript 0 rather than to the positional parameters.]"
"\fabc\f"
"\n"
"\n[arg ...]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?No errors occurred.]"
        "[+>0?An error occurred.]"
"}"

"[+SEE ALSO?\btypeset\b(1), \bshift\b(1)]"
;



const char sh_optshift[] =
"[-1c?\n@(#)$Id: shift (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?shift - shift positional parameters]"
"[+DESCRIPTION?\bshift\b is a shell special built-in that shifts the "
	"positional parameters to the left by the number of places "
	"defined by \an\a, or \b1\b if \an\a is omitted.  The number of "
	"positional parameters remaining will be reduced by the "
	"number of places that are shifted.]" 
"[+?If \an\a is given, it will be evaluated as an arithmetic expression "
	"to determinate the number of places to shift.  It is an error "
	"to shift more than the number of positional parameters or a "
	"negative number of places.]"
"\n"
"\n[n]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?The positional parameters were successfully shifted.]"
        "[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bset\b(1)]"
;

const char sh_optsleep[] =
"[-1c?\n@(#)$Id: sleep (AT&T Labs Research) 1999-04-07 $\n]"
USAGE_LICENSE
"[+NAME?sleep - suspend execution for an interval]"
"[+DESCRIPTION?\bsleep\b suspends execution for at least the time specified "
	"by \aseconds\a or until a \bSIGALRM\b signal is received.  "
	"\aseconds\a can be specifed as a floating point number but the "
	"actual granularity depends on the underlying system, normally "
	"around 1 millisecond.]"
"\n"
"\nseconds\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?The execution was successfully suspended for at least \atime\a "
	"seconds, or a \bSIGALRM\b signal was received.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\btime\b(1), \bwait\b(1)]"
;

const char sh_opttrap[] =
"[-1c?\n@(#)$Id: trap (AT&T Labs Research) 1999-07-17 $\n]"
USAGE_LICENSE
"[+NAME?trap - trap signals and conditions]"
"[+DESCRIPTION?\btrap\b is a special built-in that defines actions to be "
	"taken when conditions such as receiving a signal occur.  Also, "
	"\btrap\b can be used to display the current trap settings on "
	"standard output.]"
"[+?If \aaction\a is \b-\b, \btrap\b resets each \acondition\a "
	"to the default value.  If \aaction\a is an empty string, the "
	"shell ignores each of the \acondition\as if they arise. "
	"Otherwise, the argument \aaction\a will be read and executed "
	"by the shell as if it were processed by \beval\b(1) when one "
	"of the corresponding conditions arise.  The action of the trap "
	"will override any previous action associated with each specified "
	"\acondition\a.  The value of \b$?\b is not altered by the trap "
	"execution.]"
"[+?\acondition\a can be the name or number of a signal, or one of the "
	"following:]{"
	"[+EXIT?This trap is executed when the shell exits.  If defined "
		"within a function defined with the \bfunction\b reserved "
		"word, the trap is executed in the caller's environment "
		"when the function returns and the trap action is restored "
		"to the value it had when it called the function.]"
	"[+0?Same as EXIT.]"
	"[+DEBUG?Executed before each simple command is executed but after "
		"the arguments are expanded.]"
	"[+ERR?Executed whenever \bset -e\b would cause the shell to exit.]"
	"[+KEYBD?Executed when a key is entered from a terminal device.]"
"}"
"[+?Signal names are case insensitive and the \bsig\b prefix is optional.  "
	"Signals that were ignored on entry to a noninteractive shell cannot "
	"trapped or reset although doing so will not report an error.  The "
	"use of signal numbers other than \b1\b, \b2\b, \b3\b, \b6\b, "
	"\b9\b, \b14\b, and \b15\b is not portable.]"
"[+?Although \btrap\b is a special built-in, specifying a condition that "
	"the shell does not know about causes \btrap\b to exit with a "
	"non-zero exit status, but does not terminate the invoking shell.]"
"[+?If no \aaction\a or \acondition\as are specified then all the current "
	"trap settings are written to standard output.]" 
"[p?Causes the current traps to be output in a format that can be processed "
	"as input to the shell to recreate the current traps.]"
"\n"
"\n[action condition ...]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?Successful completion.]"
        "[+>0?An error occurred.]"
"}"

"[+SEE ALSO?\bkill\b(1), \beval\b(1), \bsignal\b(3)]"
;

const char sh_opttypeset[] =
"+[-1c?\n@(#)$Id: typeset (AT&T Labs Research) 2003-01-15 $\n]"
USAGE_LICENSE
"[+NAME?\f?\f - declare or display variables with attributes]"
"[+DESCRIPTION?Without the \b-f\b option, \b\f?\f\b sets, unsets, "
	"or displays attributes of variables as specified with the "
	"options.  If the first option is specified with a \b-\b "
	"then the attributes are set for each of the given \aname\as. "
	"If the first option is specified with a \b+\b, then the specified "
	"attributes are unset.  If \b=\b\avalue\a is specified value is "
	"assigned before the attributes are set.]"
"[+?When \b\f?\f\b is called inside a function defined with the "
	"\bfunction\b reserved word, and \aname\a does not contain a "
	"\b.\b, then a local variable statically scoped to  that function "
	"will be created.]"
"[+?Not all option combinations are possible.  For example, the numeric "
	"options \b-i\b, \b-E\b, and \b-F\b cannot be specified with "
	"the justification options \b-L\b, \b-R\b, and \b-Z\b.]"
"[+?Note that the following preset aliases are set by the shell:]{"
	"[+float?\b\f?\f -E\b.]"
	"[+functions?\b\f?\f -f\b.]"
	"[+integer?\b\f?\f -i\b.]"
	"[+nameref?\b\f?\f -n\b.]"
"}"
"[+?If no \aname\as are specified then variables that have the specified "
	"options are displayed.  If the first option is specified with "
	"a leading \b-\b then the name and value of each variable is "
	"written to standard output.  Otherwise, only the names are "
	"written.  If no options are specified or just \b-p\b is "
	"specified, then the names and attributes of all variables that have "
	"attributes are written to standard output.  When \b-f\b is specified, "
	"the names displayed will be function names.]"
"[+?If \b-f\b is specified, then each \aname\a refers to a function "
	"and the only valid options are \b-u\b and \b-t\b.  In this "
	"case no \b=\b\avalue\a can be specified.]"
"[+?\b\f?\f\b is built-in to the shell as a declaration command so that "
	"field splitting and pathname expansion are not performed on "
	"the arguments.  Tilde expansion occurs on \avalue\a.]"
#if SHOPT_BASH
"[a?Ignored, used for bash compatibility.]"
#endif
"[b?Each \aname\a may contain binary data.  Its value is the mime "
	"base64 encoding of the data. It can be used with \b-Z\b, "
	"to specify fixed sized fields.]"
"[f?Each of the options and \aname\as refers to a function.]"
"[i]#?[base:=10?An integer. \abase\a represents the arithmetic base "
	"from 2 to 64.]"
"[l?Convert uppercase character to lowercase.  Unsets \b-u\b attribute.]"
"[n?Name reference.  The value is the name of a variable that \aname\a "
	"references.  \aname\a cannot contain a \b.\b.]"
"[p?Causes the output to be in a format that can be used as input to the "
	"shell to recreate the attributes for variables.]"
"[r?Enables readonly.  Once enabled it cannot be disabled.  See "
	"\breadonly\b(1).]"
"[t?When used with \b-f\b, enables tracing for each of the specified "
	"functions.  Otherwise, \b-t\b is a user defined attribute and "
	"has no meaning to the shell.]"
"[u?Without \b-f\b or \b-i\b, converts lowercase character to uppercase "
	"and unsets \b-l\b.  With \b-f\b specifies that \aname\a is a function "
	"that hasn't been loaded yet.  With \b-i\b specifies that the "
	"value will be displayed as an unsigned integer.]"
"[x?Puts each \aname\a on the export list.  See \bexport\b(1).  \aname\a "
	"cannot contain a \b.\b.]"
"[A?Associative array.  Each \aname\a will converted to an associate "
	"array.  If a variable already exists, the current value will "
	"become index \b0\b.]"
"[E]#?[n:=10?Floating point number represented in scientific notation. "
	"\an\a specifies the number of significant figures when the "
	"value is expanded.]"
"[F]#?[n:=10?Floating point.  \an\a is the number of places after the "
	"decimal point when the value is expanded.]"
"[H?Hostname mapping.  Each \aname\a holds a native pathname.  Assigning a "
	"UNIX format pathname will cause it to be converted to a pathname "
	"suitable for the current host.  This has no effect when the "
	"native system is UNIX.]"
"[L]#?[n?Left justify.  If \an\a is given it represents the field width.  If "
	"the \b-Z\b attribute is also specified, then leading zeros are "
	"stripped.]"
"[R]#?[n?Right justify.  If \an\a is given it represents the field width.  If "
	"the \b-Z\b attribute is also specified, then zeros will "
	"be used as the fill character.  Otherwise, spaces are used.]"
"[T]:[tname?\atname\a is the name of a type name given to each \aname\a.]"
"[Z]#?[n?Zero fill.  If \an\a is given it represents the field width.]"
"\n"
"\n[name[=value]...]\n"
" -f [name...]\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?No errors occurred.]"
        "[+>0?An error occurred.]"
"}"

"[+SEE ALSO?\breadonly\b(1), \bexport\b(1)]"
;

const char sh_optulimit[] =
"[-1c?@(#)$Id: ulimit (AT&T Labs Research) 2003-06-21 $\n]"
USAGE_LICENSE
"[+NAME?ulimit - set or display resource limits]"
"[+DESCRIPTION?\bulimit\b sets or displays resource limits.  These "
	"limits apply to the current process and to each child process "
	"created after the resource limit has been set.  If \alimit\a "
	"is specified, the resource limit is set, otherwise, its current value "
	"is displayed on standard output.]"
"[+?Increasing the limit for a resource usually requires special privileges.  "
	"Some systems allow you to lower resource limits and later increase "
	"them.  These are called soft limits.  Once a hard limit is "
	"set the resource can not be increased.]"
"[+?Different systems allow you to specify different resources and some "
	"restrict how much you can raise the limit of the resource.]"
"[+?The value of \alimit\a depends on the unit of the resource listed "
	"for each resource.  In addition, \alimit\a can be \bunlimited\b "
	"to indicate no limit for that resource.]"
"[+?If you do not specify \b-H\b or \b-S\b, then \b-S\b is used for "
	"listing and both \b-S\b and \b-H\b are used for setting resources.]"
"[+?If you do not specify any resource, the default is \b-f\b.]"
"[H?A hard limit is set or displayed.]"
"[S?A soft limit is set or displayed.]"
"[a?Displays all current resource limits]"
"\flimits\f"
"\n"
"\n[limit]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?Successful completion.]"
	"[+>0?A request for a higher limit was rejected or an error occurred.]"
"}"

"[+SEE ALSO?\bulimit\b(2), \bgetrlimit\b(2)]"
;

const char sh_optumask[] =
"[-1c?\n@(#)$Id: umask (AT&T Labs Research) 1999-04-07 $\n]"
USAGE_LICENSE
"[+NAME?umask - get or set the file creation mask]"
"[+DESCRIPTION?\bumask\b sets the file creation mask of the current "
	"shell execution environment to the value specified by the "
	"\amask\a operand.  This mask affects the file permission bits "
	"of subsequently created files.  \amask\a can either be an "
	"octal number or a symbolic value as described in \bchmod\b(1).  "
	"If a symbolic value is given, the new file creation mask is the "
	"complement of the result of applying \amask\a to the complement "
	"of the current file creation mask.]"
"[+?If \amask\a is not specified, \bumask\b writes the value of the "
	"file creation mask for the current process to standard output.]"
"[S?Causes the file creation mask to be written or treated as a symbolic value "
	"rather than an octal number.]"
"\n"
"\n[mask]\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?The file creation mask was successfully changed, or no "
		"\amask\a operand was supplied.]"
	"[+>0?An error occurred.]"
"}"
"[+SEE ALSO?\bchmod\b(1)]"
;
const char sh_optuniverse[]	= " [name]";
const char sh_optunset[] =
"[-1c?\n@(#)$Id: unset (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?unset - unset values and attributes of variables and functions]"
"[+DESCRIPTION?For each \aname\a specified, \bunset\b  unsets the variable, "
	"or function if \b-f\b is specified, from the current shell "
	"execution environment.  Readonly variables cannot be unset.]"
"[n?If \aname\a refers to variable that is a reference, the variable \aname\a "
	"will be unset rather than the variable it references.  Otherwise, "
	"is is equivalent to \b-v\b.]"
"[f?\aname\a refers to a function name and the shell will unset the "
	"function definition.]"
"[v?\aname\a refers to a variable name and the shell will unset it and "
	"remove it from the environment.  This is the default behavior.]"
"\n"
"\nname...\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?All \aname\as were successfully unset.]"
        "[+>0?One or more \aname\a operands could not be unset "
	"or an error occurred.]"
"}"

"[+SEE ALSO?\btypeset\b(1)]"
;

const char sh_optunalias[] =
"[-1c?\n@(#)$Id: unalias (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?unalias - remove alias definitions]"
"[+DESCRIPTION?\bunalias\b removes the definition of each named alias "
	"from the current shell execution environment, or all aliases if "
	"\b-a\b is specified.  It will not affect any commands that "
	"have already been read and subsequently executed.]"
"[a?Causes all alias definitions to be removed.  \aname\a operands "
	"are optional and ignored in this case.]"
"\n"
"\nname...\n"
"\n"
"[+EXIT STATUS?]{"
        "[+0?Successful completion.]"
        "[+>0?\b-a\b was not specified and one or more \aname\a operands "
	"did not have an alias definition, or an error occurred.]"
"}"

"[+SEE ALSO?\balias\b(1)]"
;

const char sh_optwait[]	=
"[-1c?\n@(#)$Id: wait (AT&T Labs Research) 1999-06-17 $\n]"
USAGE_LICENSE
"[+NAME?wait - wait for process or job completion]"
"[+DESCRIPTION?\bwait\b with no operands, waits until all jobs "
	"known to the invoking shell have terminated.  If one or more "
	"\ajob\a operands are specified, \bwait\b waits until all of them "
	"have completed.]"
"[+?Each \ajob\a can be specified as one of the following:]{"
	"[+\anumber\a?\anumber\a refers to a process id.]"
	"[+-\anumber\a?\anumber\a refers to a process group id.]"
	"[+%\anumber\a?\anumber\a refer to a job number.]"
	"[+%\astring\a?Refers to a job whose name begins with \astring\a.]"
	"[+%??\astring\a?Refers to a job whose name contains \astring\a.]"
	"[+%+ \bor\b %%?Refers to the current job.]"
	"[+%-?Refers to the previous job.]"
"}"
"[+?If one ore more \ajob\a operands is a process id or process group id "
	"not known by the current shell environment, \bwait\b treats each "
	"of them as if it were a process that exited with status 127.]"
"\n"
"\n[job ...]\n"
"\n"
"[+EXIT STATUS?If \await\a is invoked with one or more \ajob\as, and all of "
	"them have terminated or were not known by the invoking shell, "
	"the exit status of \bwait\b  will be that of the last \ajob\a.  "
	"Otherwise, it will be one of the following:]{"
	"[+0?\bwait\b utility was invoked with no operands and all "
		"processes known by the invoking process have terminated.]"
	"[+127?\ajob\a is a process id or process group id that is unknown "
		"to the current shell environment.]"
"}"

"[+SEE ALSO?\bjobs\b(1), \bps\b(1)]"
;

#if SHOPT_FS_3D
    const char sh_optvpath[]	= " [top] [base]";
    const char sh_optvmap[]	= " [dir] [list]";
#endif /* SHOPT_FS_3D */

const char sh_optwhence[] =
"[-1c?\n@(#)$Id: whence (AT&T Labs Research) 1999-07-07 $\n]"
USAGE_LICENSE
"[+NAME?whence - locate a command and describe its type]"
"[+DESCRIPTION?Without \b-v\b, \bwhence\b writes on standard output an "
	"absolute pathname, if any, corresponding to \aname\a based "
	"on the complete search order that the shell uses.  If \aname\a "  
	"is not found, then no output is produced.]"
"[+?If \b-v\b is specified, the output will also contain information "
	"that indicates how the given \aname\a would be interpretted by "
	"the shell in the current execution environment.]" 
"[a?Displays all uses for each \aname\a rather than the first.]"
"[f?Do not check for functions.]"
"[p?Do not check to see if \aname\a is a reserved word, a built-in, "
	"an alias, or a function.]"
"[v?For each name you specify, the shell displays a line that indicates "
	"if that name is one of the following:]{"
	"[+?Reserved word]"
	"[+?Alias]"
	"[+?Built-in]"
	"[+?Undefined function]"
	"[+?Function]"
	"[+?Tracked alias]"
	"[+?Program]"
	"[+?Not found]"
"}"
"\n"
"\nname  ...\n"
"\n"
"[+EXIT STATUS?]{"
	"[+0?Each \aname\a was found by the shell.]"
	"[+1?One or more \aname\as were not found by the shell.]"
	"[+>1?An error occurred.]"
"}"

"[+SEE ALSO?\bcommand\b(1)]"
;


const char e_alrm1[]		= "alarm -r %s +%.3g\n";
const char e_alrm2[]		= "alarm %s %.3f\n";
const char e_badfun[]		= "%s: illegal function name";
const char e_baddisc[]		= "%s: invalid discipline function";
const char e_nospace[]		= "out of memory";
const char e_nofork[]		= "cannot fork";
const char e_nosignal[]		= "%s: unknown signal name";
const char e_numeric[]		= "*([0-9])?(.)*([0-9])";
const char e_condition[]	= "condition(s) required";
const char e_cneedsarg[]	= "-c requires argument";
