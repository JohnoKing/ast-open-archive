/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1985-2000 AT&T Corp.              *
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
*              David Korn <dgk@research.att.com>               *
*               Phong Vo <kpv@research.att.com>                *
*                                                              *
***************************************************************/
#pragma prototyped
/*
 *   Routines to implement a stack-like storage library
 *   
 *   A stack consists of a link list of variable size frames
 *   The beginning of each frame is initialized with a frame structure
 *   that contains a pointer to the previous frame and a pointer to the
 *   end of the current frame.
 *
 *   This is a rewrite of the stk library that uses sfio
 *
 *   David Korn
 *   AT&T Research
 *   Room 2B-102
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *   dgk@research.att.com
 *
 */

#include	<sfio_t.h>
#include	<ast.h>
#include	<align.h>
#include	<stk.h>

/*
 *  A stack is a header and a linked list of frames
 *  The first frame has structure
 *	Sfio_t
 *	Sfdisc_t
 *	struct stk
 * Frames have structure
 *	struct frame
 *	data
 */
#define STK_ALIGN	ALIGN_BOUND
#define STK_FSIZE	(1024*sizeof(int))
#define STK_HDRSIZE	(sizeof(Sfio_t)+sizeof(Sfdisc_t))

typedef char* (*_stk_overflow_)(int);

static int stkexcept(Sfio_t*,int,void*,Sfdisc_t*);
static Sfdisc_t stkdisc = { 0, 0, 0, stkexcept };

Sfio_t _Stak_data = SFNEW((char*)0,0,-1,SF_STATIC|SF_WRITE|SF_STRING,&stkdisc);

__EXTERN__(Sfio_t, _Stak_data);

struct frame
{
	char	*prev;	/* address of previous frame */
	char	*end;	/* address of end this frame */
};

struct stk
{
	_stk_overflow_	stkoverflow;	/* called when malloc fails */
	short		stkref;	/* reference count; */
	short		stkflags;	/* stack attributes */
	char		*stkbase;	/* beginning of current stack frame */
	char		*stkend;	/* end of current stack frame */
};

static int		init;		/* 1 when initialized */
static struct stk	*stkcur;	/* pointer to current stk */
static char		*stkgrow(Sfio_t*, unsigned);

#define stream2stk(stream)	((stream)==stkstd? stkcur:\
				 ((struct stk*)(((char*)(stream))+STK_HDRSIZE)))
#define stk2stream(sp)		((Sfio_t*)(((char*)(sp))-STK_HDRSIZE))
#define stkleft(stream)		((stream)->endb-(stream)->data)
	

#ifdef STKSTATS
    static struct
    {
	int	create;
	int	delete;
	int	install;
	int	alloc;
	int	copy;
	int	puts;
	int	seek;
	int	set;
	int	grow;
	int	addsize;
	int	delsize;
	int	movsize;
    } _stkstats;
#   define increment(x)	(_stkstats.x++)
#   define count(x,n)	(_stkstats.x += (n))
#else
#   define increment(x)
#   define count(x,n)
#endif /* STKSTATS */

static const char Omsg[] = "malloc failed while growing stack\n";

/*
 * default overflow exception
 */
static char *overflow(int n)
{
	NoP(n);
	write(2,Omsg, sizeof(Omsg)-1);
	exit(2);
	/* NOTREACHED */
	return(0);
}

/*
 * initialize stkstd, sfio operations may have already occcured
 */
static void stkinit(int size)
{
	register Sfio_t *sp;
	init = size;
	sp = stkopen(0);
	init = 1;
	stkinstall(sp,overflow);
}

static int stkexcept(register Sfio_t *stream, int type, void* val, Sfdisc_t* dp)
{
	NoP(dp);
	NoP(val);
	switch(type)
	{
	    case SF_CLOSE:
		{
			register struct stk *sp = stream2stk(stream); 
			register char *cp = sp->stkbase;
			register struct frame *fp;
			if(--sp->stkref<=0)
			{
				increment(delete);
				if(stream==stkstd)
					stkset(stream,(char*)0,0);
				else
				{
					while(1)
					{
						fp = (struct frame*)cp;
						if(fp->prev)
						{
							cp = fp->prev;
							free(fp);
						}
						else
						{
							free(fp);
							break;
						}
					}
				}
			}
		}
		return(0);
	    case SF_FINAL:
		free(stream);
		return(1);
	    case SF_DPOP:
		return(-1);
	    case SF_WRITE:
	    case SF_SEEK:
		{
			long size = sfvalue(stream);
			if(init)
			{
				Sfio_t *old = 0;
				if(stream!=stkstd)
					old = stkinstall(stream,NiL);
				if(!stkgrow(stkstd,size-(stkstd->endb-stkstd->data)))
					return(-1);
				if(old)
					stkinstall(old,NiL);
			}
			else
				stkinit(size);
		}
		return(1);
	    case SF_NEW:
		return(-1);
	}
	return(0);
}

/*
 * create a stack
 */
Sfio_t *stkopen(int flags)
{
	register int bsize;
	register Sfio_t *stream;
	register struct stk *sp;
	register struct frame *fp;
	register Sfdisc_t *dp;
	register char *cp;
	if(!(stream=newof((char*)0,Sfio_t, 1, sizeof(*dp)+sizeof(*sp))))
		return(0);
	increment(create);
	count(addsize,sizeof(*stream)+sizeof(*dp)+sizeof(*sp));
	dp = (Sfdisc_t*)(stream+1);
	dp->exceptf = stkexcept;
	sp = (struct stk*)(dp+1);
	sp->stkref = 1;
	sp->stkflags = (flags&STK_SMALL);
	if(flags&STK_NULL) sp->stkoverflow = 0;
	else sp->stkoverflow = stkcur?stkcur->stkoverflow:overflow;
	bsize = init+sizeof(struct frame);
#ifndef USE_REALLOC
	if(flags&STK_SMALL)
		bsize = roundof(bsize,STK_FSIZE/16);
	else
#endif /* USE_REALLOC */
		bsize = roundof(bsize,STK_FSIZE);
	bsize -= sizeof(struct frame);
	if(!(fp=newof((char*)0,struct frame, 1,bsize)))
	{
		free(stream);
		return(0);
	}
	count(addsize,sizeof(*fp)+bsize);
	cp = (char*)(fp+1);
	sp->stkbase = (char*)fp;
	fp->prev = 0;
	fp->end = sp->stkend = cp+bsize;
	if(!sfnew(stream,cp,bsize,-1,SF_STRING|SF_WRITE|SF_STATIC|SF_EOF))
		return((Sfio_t*)0);
	sfdisc(stream,dp);
	return(stream);
}

/*
 * return a pointer to the current stack
 * if <stream> is not null, it becomes the new current stack
 * <oflow> becomes the new overflow function
 */
Sfio_t *stkinstall(Sfio_t *stream, _stk_overflow_ oflow)
{
	Sfio_t *old;
	register struct stk *sp;
	if(!init)
	{
		stkinit(1);
		if(oflow)
			stkcur->stkoverflow = oflow;
		return((Sfio_t*)0);
	}
	increment(install);
	old = stkcur?stk2stream(stkcur):0;
	if(stream)
	{
		sp = stream2stk(stream);
		while(sfstack(stkstd, SF_POPSTACK));
		if(stream!=stkstd)
			sfstack(stkstd,stream);
		stkcur = sp;
#ifdef USE_REALLOC
		/*** someday ***/
#endif /* USE_REALLOC */
	}
	else
		sp = stkcur;
	if(oflow)
		sp->stkoverflow = oflow;
	return(old);
}

/*
 * increase the reference count on the given <stack>
 */
int stklink(register Sfio_t* stream)
{
	register struct stk *sp = stream2stk(stream);
	return(sp->stkref++);
}

/*
 * terminate a stack and free up the space
 * >0 returned if reference decremented but still > 0
 *  0 returned on last close
 * <0 returned on error
 */
int stkclose(Sfio_t* stream)
{
	register struct stk *sp = stream2stk(stream); 
	if(sp->stkref>1)
	{
		sp->stkref--;
		return(1);
	}
	return(sfclose(stream));
}

/*
 * reset the bottom of the current stack back to <loc>
 * if <loc> is not in this stack, then the stack is reset to the beginning
 * otherwise, the top of the stack is set to stkbot+<offset>
 *
 */
char *stkset(register Sfio_t * stream, register char* loc, unsigned offset)
{
	register struct stk *sp = stream2stk(stream); 
	register char *cp;
	register struct frame *fp;
	register int frames = 0;
	if(!init)
		stkinit(offset+1);
	increment(set);
	while(1)
	{
		/* see whether <loc> is in current stack frame */
		if(loc>=(cp=sp->stkbase) && loc<=sp->stkend)
		{
			cp += sizeof(struct frame);
			if(frames)
				sfsetbuf(stream,cp,sp->stkend-cp);
			stream->data = (unsigned char*)(cp + roundof(loc-cp,STK_ALIGN));
			stream->next = (unsigned char*)loc+offset;
			goto found;
		}
		fp = (struct frame*)cp;
		if(fp->prev)
		{
			sp->stkbase = fp->prev;
			sp->stkend = ((struct frame*)(fp->prev))->end;
			free(cp);
		}
		else
			break;
		frames++;
	}
	/* set stack back to the beginning */
	cp = (char*)(fp+1);
	if(frames)
		sfsetbuf(stream,cp,sp->stkend-cp);
	else
		stream->data = stream->next = (unsigned char*)cp;
found:
	return((char*)stream->data);
}

/*
 * allocate <n> bytes on the current stack
 */
char *stkalloc(register Sfio_t *stream, register unsigned int n)
{
	register unsigned char *old;
	if(!init)
		stkinit(n);
	increment(alloc);
	n = roundof(n,STK_ALIGN);
	if(stkleft(stream) <= (int)n && !stkgrow(stream,n))
		return(0);
	old = stream->data;
	stream->data = stream->next = old+n;
	return((char*)old);
}

/*
 * begin a new stack word of at least <n> bytes
 */
char *_stkseek(register Sfio_t *stream, register unsigned n)
{
	if(!init)
		stkinit(n);
	increment(seek);
	if(stkleft(stream) <= (int)n && !stkgrow(stream,n))
		return(0);
	stream->next = stream->data+n;
	return((char*)stream->data);
}

/*
 * advance the stack to the current top
 * if extra is non-zero, first add a extra bytes and zero the first
 */
char	*stkfreeze(register Sfio_t *stream, register unsigned extra)
{
	register unsigned char *old, *top;
	if(!init)
		stkinit(extra);
	old = stream->data;
	top = stream->next;
	if(extra)
	{
		if(extra > (stream->endb-stream->next))
		{
			if (!(top = (unsigned char*)stkgrow(stream,extra)))
				return(0);
			old = stream->data;
		}
		*top = 0;
		top += extra;
	}
	stream->next = stream->data += roundof(top-old,STK_ALIGN);
	return((char*)old);
}

/*
 * copy string <str> onto the stack as a new stack word
 */
char	*stkcopy(Sfio_t *stream, const char* str)
{
	register unsigned char *cp = (unsigned char*)str;
	register int n;
	while(*cp++);
	n = roundof(cp-(unsigned char*)str,STK_ALIGN);
	if(!init)
		stkinit(n);
	increment(copy);
	if(stkleft(stream) <= n && !stkgrow(stream,n))
		return(0);
	strcpy((char*)(cp=stream->data),str);
	stream->data = stream->next = cp+n;
	return((char*)cp);
}

/*
 * add a new stack frame of size >= <n> to the current stack.
 * if <n> > 0, copy the bytes from stkbot to stktop to the new stack
 * if <n> is zero, then copy the remainder of the stack frame from stkbot
 * to the end is copied into the new stack frame
 */

static char *stkgrow(register Sfio_t *stream, unsigned size)
{
	register int n = size;
	register struct stk *sp = stream2stk(stream);
	register struct frame *fp;
	register char *cp;
	register unsigned m = stktell(stream);
	register int reused = 0;
	n += (m + sizeof(struct frame)+1);
	if(sp->stkflags&STK_SMALL)
#ifndef USE_REALLOC
		n = roundof(n,STK_FSIZE/16);
	else
#endif /* !USE_REALLOC */
		n = roundof(n,STK_FSIZE);
	/* see whether current frame can be extended */
	if((char*)(stream->data) == sp->stkbase+sizeof(struct frame))
	{
		cp = newof(sp->stkbase,char,n,0);
		reused++;
	}
	else
		cp = newof((char*)0, char, n, 0);
	if(!cp && (!sp->stkoverflow || !(cp = (*sp->stkoverflow)(n))))
		return(0);
	increment(grow);
	count(addsize,n);
	fp = (struct frame*)cp;
	if(!reused)
		fp->prev = sp->stkbase;
	sp->stkbase = cp;
	sp->stkend = fp->end = cp+n;
	cp = (char*)(fp+1);
	if(m && !reused)
		memcpy(cp,(char*)stream->data,m);
	count(movsize,m);
	sfsetbuf(stream,cp,sp->stkend-cp);
	return((char*)(stream->next = stream->data+m));
}
