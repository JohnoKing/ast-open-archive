/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1982-2002 AT&T Corp.                *
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
#ifndef SH_SIGBITS
/*
 *	UNIX shell
 *	S. R. Bourne
 *	Rewritten by David Korn
 *
 */

#include	<sig.h>
#include	<setjmp.h>
#include	<error.h>
#include	<sfio.h>
#include	"FEATURE/setjmp"
#include	"FEATURE/sigfeatures"

typedef void (*SH_SIGTYPE)(int,void(*)(int));

#define SH_FORKLIM		16	/* fork timeout interval */

#define SH_TRAP			0200	/* bit for internal traps */
#define SH_ERRTRAP		0	/* trap for non-zero exit status */
#define SH_KEYTRAP		1	/* trap for keyboard event */
#define SH_DEBUGTRAP		4	/* must be last internal trap */

#define SH_SIGBITS		8
#define SH_SIGFAULT		1	/* signal handler is sh_fault */
#define SH_SIGOFF		2	/* signal handler is SIG_IGN */
#define SH_SIGSET		4	/* pending signal */
#define SH_SIGTRAP		010	/* pending trap */
#define SH_SIGDONE		020	/* default is exit */
#define SH_SIGIGNORE		040	/* default is ingore signal */
#define SH_SIGINTERACTIVE	0100	/* handle interactive specially */
#define SH_SIGTSTP		0200	/* tstp signal received */
#define SH_SIGALRM		0200	/* timer alarm received */
#define SH_SIGTERM		SH_SIGOFF /* term signal received */

/*
 * These are longjmp values
 */

#define SH_JMPDOT	2
#define SH_JMPEVAL	3
#define SH_JMPTRAP	4
#define SH_JMPIO	5
#define SH_JMPCMD	6
#define SH_JMPFUN	7
#define SH_JMPERRFN	8
#define SH_JMPSUB	9
#define SH_JMPERREXIT	10
#define SH_JMPEXIT	11
#define SH_JMPSCRIPT	12

struct openlist
{
	Sfio_t	*strm;
	struct openlist *next;
};

struct checkpt
{
	sigjmp_buf	buff;
	sigjmp_buf	*prev;
	int		topfd;
	int		mode;
	struct openlist	*olist;
	struct Error_context_s err;
};

#define sh_pushcontext(bp,n)	( (bp)->mode=(n) , (bp)->olist=0,  \
				  (bp)->topfd=sh.topfd, (bp)->prev=sh.jmplist, \
				  (bp)->err = *ERROR_CONTEXT_BASE, \
					sh.jmplist = (sigjmp_buf*)(bp) \
				)
#define sh_popcontext(bp)	(sh.jmplist=(bp)->prev, errorpop(&((bp)->err)))

extern void 	sh_fault(int);
extern void 	sh_done(int);
extern void 	sh_chktrap(void);
extern void 	sh_sigclear(int);
extern void 	sh_sigdone(void);
extern void	sh_siginit(void);
extern void 	sh_sigtrap(int);
extern void 	sh_sigreset(int);
extern void 	sh_timetraps(void);
extern void 	*sh_timeradd(unsigned long,int ,void (*)(void*),void*);
extern void	timerdel(void*);

extern const char e_alarm[];

#endif /* !SH_SIGBITS */
