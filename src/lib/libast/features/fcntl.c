/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1985-2003 AT&T Corp.                *
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
*                David Korn <dgk@research.att.com>                 *
*                 Phong Vo <kpv@research.att.com>                  *
*                                                                  *
*******************************************************************/
#pragma prototyped
/*
 * Glenn Fowler
 * AT&T Research
 *
 * generate POSIX fcntl.h
 */

#include <sys/types.h>
#include <sys/stat.h>

#include "FEATURE/lib"

#define getdtablesize	______getdtablesize
#define getpagesize	______getpagesize
#define ioctl		______ioctl
#define printf		______printf

#if _typ_off64_t
#undef	off_t
#ifdef __STDC__
#define	off_t		off_t
#endif
#endif

#include "FEATURE/fcntl.lcl"
#include "FEATURE/unistd.lcl"
#include "FEATURE/fs"

#undef	getdtablesize   
#undef	getpagesize
#undef	ioctl
#undef	printf

#include "FEATURE/tty"

#if _typ_off64_t
#undef	off_t
#define	off_t	off64_t
#endif

extern int		printf(const char*, ...);

main()
{
	int		f_local = 0;
	int		f_lck = 0;
	int		f_lk = 0;
	int		o_local = 2;

	printf("#pragma prototyped\n");
	printf("\n");
	printf("#if _typ_off64_t\n");
	printf("#undef	off_t\n");
	printf("#ifdef __STDC__\n");
	printf("#define	off_t	off_t\n");
	printf("#endif\n");
	printf("#endif\n");
	printf("\n");
#if _lcl_fcntl || _hdr_lcl_fcntl /* _hdr_lcl_ workaround iffe pre 2002-09-11 */
	printf("#if defined(__STDPP__directive) && defined(__STDPP__hide)\n");
	printf("__STDPP__directive pragma pp:hide chmod creat fcntl mkdir mkfifo mmap mmap64 munmap");
#if !_lib__xmknod && !defined(mknod)
	printf(" mknod");
#endif
	printf(" open umask\n");
	printf("#else\n");
	printf("#define chmod	______chmod\n");
#ifndef creat
	printf("#undef	creat\n");
	printf("#define creat	______creat\n");
#endif
	printf("#define fcntl	______fcntl\n");
	printf("#define mkdir	______mkdir\n");
	printf("#define mkfifo	______mkfifo\n");
#if !_lib__xmknod && !defined(mknod)
	printf("#define mknod	______mknod\n");
#endif
	printf("#undef	mmap\n");
	printf("#define mmap	______mmap\n");
	printf("#undef	mmap64\n");
	printf("#define mmap64	______mmap64\n");
	printf("#undef	munmap\n");
	printf("#define munmap	______munmap\n");
	printf("#undef	open\n");
	printf("#define open	______open\n");
	printf("#define umask	______umask\n");
	printf("#endif \n");
	printf("\n");
#if defined(S_IRUSR)
	printf("#include <ast_fs.h>	/* <fcntl.h> includes <sys/stat.h>! part I*/\n");
	printf("#if _typ_off64_t\n");
	printf("#undef	off_t\n");
	printf("#ifdef __STDC__\n");
	printf("#define	off_t	off_t\n");
	printf("#endif\n");
	printf("#endif\n");
#endif
	printf("#include <fcntl.h>\n");
#if _hdr_mman
	printf("#include <mman.h>\n");
#else
#if _sys_mman
	printf("#include <sys/mman.h>\n");
#endif
#endif
	printf("\n");
	printf("#if defined(_AST_STD_H) || defined(_POSIX_SOURCE)\n");
	printf("#define _AST_mode_t	mode_t\n");
	printf("#else\n");
	printf("#define _AST_mode_t	int\n");
	printf("#endif\n");
	printf("#if defined(__STDPP__directive) && defined(__STDPP__hide)\n");
	printf("__STDPP__directive pragma pp:nohide chmod creat fcntl mkdir mkfifo mmap mmap64 munmap");
#if !_lib__xmknod && !defined(mknod)
	printf(" mknod");
#endif
	printf(" open umask\n");
	printf("extern int	creat(const char*, _AST_mode_t);\n");
	printf("extern int	fcntl(int, int, ...);\n");
	printf("extern int	open(const char*, int, ...);\n");
	printf("extern void*	mmap(void*, size_t, int, int, int, off_t);\n");
	printf("extern int	munmap(void*, size_t);\n");
	printf("#else\n");
	printf("#ifdef	creat\n");
	printf("#undef	creat\n");
	printf("extern int	creat(const char*, _AST_mode_t);\n");
	printf("#endif \n");
	printf("#ifdef	fcntl\n");
	printf("#undef	fcntl\n");
	printf("extern int	fcntl(int, int, ...);\n");
	printf("#endif \n");
	printf("#ifdef	mmap\n");
	printf("#undef	mmap\n");
	printf("extern void*	mmap(void*, size_t, int, int, int, off_t);\n");
	printf("#endif \n");
	printf("#undef	mmap64\n");
	printf("#ifdef	munmap\n");
	printf("#undef	munmap\n");
	printf("extern int	munmap(void*, size_t);\n");
	printf("#endif \n");
	printf("#ifdef	open\n");
	printf("#undef	open\n");
	printf("extern int	open(const char*, int, ...);\n");
	printf("#endif \n");
	printf("\n");
	printf("#undef	chmod\n");
	printf("#undef	mkdir\n");
	printf("#undef	mkfifo\n");
#if !_lib__xmknod && !defined(mknod)
	printf("#undef	mknod\n");
#endif
	printf("#undef	umask\n");
	printf("#endif \n");
	printf("\n");
	printf("#undef	_AST_mode_t\n");
	printf("\n");
#endif

#ifndef	FD_CLOEXEC
	printf("#define FD_CLOEXEC	1\n");
	printf("\n");
#endif

#ifndef	F_DUPFD
#define NEED_F	1
#else
	if (F_DUPFD > f_local) f_local = F_DUPFD;
#endif
#ifndef	F_GETFD
#define NEED_F	1
#else
	if (F_GETFD > f_local) f_local = F_GETFD;
#endif
#ifndef	F_GETFL
#define NEED_F	1
#else
	if (F_GETFL > f_local) f_local = F_GETFL;
#endif
#ifndef	F_GETLK
#define NEED_F	1
	f_lk++;
#else
	if (F_GETLK > f_local) f_local = F_GETLK;
#endif
#ifndef	F_RDLCK
#define NEED_F	1
#define NEED_LCK	1
#else
	if (F_RDLCK > f_lck) f_lck = F_RDLCK;
#endif
#ifndef	F_SETFD
#define NEED_F	1
#else
	if (F_SETFD > f_local) f_local = F_SETFD;
#endif
#ifndef	F_SETFL
#define NEED_F	1
#else
	if (F_SETFL > f_local) f_local = F_SETFL;
#endif
#ifndef	F_SETLK
#define NEED_F	1
	f_lk++;
#else
	if (F_SETLK > f_local) f_local = F_SETLK;
#endif
#ifndef	F_SETLKW
#define NEED_F	1
	f_lk++;
#else
	if (F_SETLKW > f_local) f_local = F_SETLKW;
#endif
#ifndef	F_UNLCK
#define NEED_F	1
#define NEED_LCK	1
#else
	if (F_UNLCK > f_lck) f_lck = F_UNLCK;
#endif
#ifndef	F_WRLCK
#define NEED_F	1
#define NEED_LCK	1
#else
	if (F_WRLCK > f_lck) f_lck = F_WRLCK;
#endif

#if	NEED_F
	printf("#define fcntl		_ast_fcntl\n");
#if	_lib_fcntl
	printf("#define _lib_fcntl	1\n");
#endif
	printf("#define _ast_F_LOCAL	%d\n", f_local + 1);
#ifndef	F_DUPFD
	printf("#define F_DUPFD		%d\n", ++f_local);
#endif
#ifndef	F_GETFD
	printf("#define F_GETFD		%d\n", ++f_local);
#endif
#ifndef	F_GETFL
	printf("#define F_GETFL		%d\n", ++f_local);
#endif
#ifndef	F_GETLK
	printf("#define F_GETLK		%d\n", ++f_local);
#endif
#ifndef	F_SETFD
	printf("#define F_SETFD		%d\n", ++f_local);
#endif
#ifndef	F_SETFL
	printf("#define F_SETFL		%d\n", ++f_local);
#endif
#ifndef	F_SETLK
	printf("#define F_SETLK		%d\n", ++f_local);
#endif
#ifndef	F_SETLKW
	printf("#define F_SETLKW	%d\n", ++f_local);
#endif
#if	NEED_LCK
	printf("\n");
#ifndef	F_RDLCK
	printf("#define F_RDLCK		%d\n", f_lck++);
#endif
#ifndef	F_WRLCK
	printf("#define F_WRLCK		%d\n", f_lck++);
#endif
#ifndef	F_UNLCK
	printf("#define F_UNLCK		%d\n", f_lck++);
#endif
#endif
	printf("\n");
	if (f_lck == 3)
	{
		printf("struct flock\n");
		printf("{\n");
		printf("	short	l_type;\n");
		printf("	short	l_whence;\n");
		printf("	off_t	l_start;\n");
		printf("	off_t	l_len;\n");
		printf("	short	l_pid;\n");
		printf("};\n");
		printf("\n");
	}
	printf("\n");
#endif

#ifndef	O_APPEND
#define NEED_O	1
#else
	if (O_APPEND > o_local) o_local = O_APPEND;
#endif
#ifndef	O_CREAT
#define NEED_O	1
#else
	if (O_CREAT > o_local) o_local = O_CREAT;
#endif
#ifndef	O_EXCL
#define NEED_O	1
#else
	if (O_EXCL > o_local) o_local = O_EXCL;
#endif
#ifndef	O_NOCTTY
#ifdef	TIOCNOTTY
#define NEED_O	1
#endif
#else
	if (O_NOCTTY > o_local) o_local = O_NOCTTY;
#endif
#ifndef	O_NONBLOCK
#ifndef	O_NDELAY
#define NEED_O	1
#endif
#else
	if (O_NONBLOCK > o_local) o_local = O_NONBLOCK;
#endif
#ifndef	O_RDONLY
#define NEED_O	1
#endif
#ifndef	O_RDWR
#define NEED_O	1
#endif
#ifndef	O_TRUNC
#define NEED_O	1
#else
	if (O_TRUNC > o_local) o_local = O_TRUNC;
#endif
#ifndef	O_WRONLY
#define NEED_O	1
#endif

#if	NEED_O
	printf("#define open		_ast_open\n");
	printf("#define _ast_O_LOCAL	0%o\n", o_local<<1);
#ifndef	O_RDONLY
	printf("#define O_RDONLY	0\n");
#endif
#ifndef	O_WRONLY
	printf("#define O_WRONLY	1\n");
#endif
#ifndef	O_RDWR
	printf("#define O_RDWR		2\n");
#endif
#ifndef	O_APPEND
	printf("#define O_APPEND	0%o\n", o_local <<= 1);
#endif
#ifndef	O_CREAT
	printf("#define O_CREAT		0%o\n", o_local <<= 1);
#endif
#ifndef	O_EXCL
	printf("#define O_EXCL		0%o\n", o_local <<= 1);
#endif
#ifndef	O_NOCTTY
#ifdef	TIOCNOTTY
	printf("#define O_NOCTTY	0%o\n", o_local <<= 1);
#endif
#endif
#ifndef	O_NONBLOCK
#ifndef	O_NDELAY
	printf("#define O_NONBLOCK	0%o\n", o_local <<= 1);
#endif
#endif
#ifndef	O_TRUNC
	printf("#define O_TRUNC		0%o\n", o_local <<= 1);
#endif
#endif
#ifndef	O_ACCMODE
	printf("#define O_ACCMODE	(O_RDONLY|O_WRONLY|O_RDWR)\n");
#endif
#ifndef	O_NOCTTY
#ifndef	TIOCNOTTY
	printf("#define O_NOCTTY	0\n");
#endif
#endif
#ifndef	O_NONBLOCK
#ifdef	O_NDELAY
	printf("#define O_NONBLOCK	O_NDELAY\n");
#endif
#endif
#ifndef	O_BINARY
	printf("#define O_BINARY	0\n");
#endif
#ifndef	O_TEMPORARY
	printf("#define O_TEMPORARY	0\n");
#endif
#ifndef	O_TEXT
	printf("#define O_TEXT		0\n");
#endif
#if defined(S_IRUSR)
	printf("\n");
	printf("#include <ls.h>	/* <fcntl.h> includes <sys/stat.h> part II! */\n");
	printf("#if _typ_off64_t\n");
	printf("#undef	off_t\n");
	printf("#ifdef __STDC__\n");
	printf("#define	off_t	off_t\n");
	printf("#endif\n");
	printf("#endif\n");
#endif
#if	NEED_F || NEED_O
	printf("\n");
#if	NEED_F
	printf("extern int	fcntl(int, int, ...);\n");
#endif
#if	NEED_O
	printf("extern int	open(const char*, int, ...);\n");
#endif
#endif
	printf("#if _typ_off64_t\n");
	printf("#undef	off_t\n");
	printf("#define	off_t	off64_t\n");
	printf("#endif\n");
	printf("#if _lib_creat64\n");
	printf("#define creat	creat64\n");
	printf("#if !defined(__USE_LARGEFILE64)\n");
	printf("extern int	creat64(const char*, mode_t);\n");
	printf("#endif\n");
	printf("#endif\n");
	printf("#if _lib_mmap64\n");
	printf("#define mmap	mmap64\n");
	printf("extern void*	mmap64(void*, size_t, int, int, int, off64_t);\n");
	printf("#endif\n");
	printf("#if _lib_open64\n");
	printf("#define open	open64\n");
	printf("#if !defined(__USE_LARGEFILE64)\n");
	printf("extern int	open64(const char*, int, ...);\n");
	printf("#endif\n");
	printf("#endif\n");

	return(0);
}
