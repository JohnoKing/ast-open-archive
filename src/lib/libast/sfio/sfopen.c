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
#include	"sfhdr.h"

/*	Open a file/string for IO.
**	If f is not nil, it is taken as an existing stream that should be
**	closed and its structure reused for the new stream.
**
**	Written by Kiem-Phong Vo (06/27/90)
*/

#if __STD_C
Sfio_t* sfopen(reg Sfio_t* f, const char* file, const char* mode)
#else
Sfio_t* sfopen(f,file,mode)
reg Sfio_t*	f;		/* old stream structure */
char*		file;		/* file/string to be opened */
reg char*	mode;		/* mode of the stream */
#endif
{
	int	fd, oldfd, oflags, sflags;

	if((sflags = _sftype(mode,&oflags)) == 0)
		return NIL(Sfio_t*);

	if(f && !file && (f->mode&SF_INIT) )
	{	/* change appropriate file control flags */
		if(f->file >= 0 && !(f->flags&SF_STRING) )
		{	if((oflags &= (O_TEXT|O_BINARY|O_APPEND)) != 0)
			{	int ctl = fcntl(f->file, F_GETFL, 0);
				ctl = (ctl & ~(O_TEXT|O_BINARY|O_APPEND)) | oflags;
				fcntl(f->file, F_SETFL, ctl);
			}
			f->flags |= (sflags&SF_APPENDWR);
		}

		/* reset read/write modes */
		if((sflags &= SF_RDWR) != 0)
		{	f->flags = (f->flags & ~SF_RDWR) | sflags;
			if((f->flags&SF_RDWR) == SF_RDWR)
				f->bits |= SF_BOTH;
			else
			{	f->bits &= ~SF_BOTH;
				if(f->flags&SF_READ)
					f->mode = (f->mode & ~SF_WRITE)|SF_READ;
				else	f->mode = (f->mode & ~SF_READ)|SF_WRITE;
			}
		}

		return f;
	}

	if(sflags&SF_STRING)
		fd = -1;
	else
	{	if(!file)
			return NIL(Sfio_t*);

#if _has_oflags /* open the file */
		while((fd = open((char*)file,oflags,SF_CREATMODE)) < 0 && errno == EINTR)
			errno = 0;
#else
		while((fd = open(file,oflags&(O_WRONLY|O_RDWR))) < 0 && errno == EINTR)
			errno = 0;
		if(fd >= 0)
		{	if((oflags&(O_CREAT|O_EXCL)) == (O_CREAT|O_EXCL) )
			{	CLOSE(fd);	/* error: file already exists */
				return NIL(Sfio_t*);
			}
			if(oflags&O_TRUNC )	/* truncate file */
			{	reg int	tf;
				while((tf = creat(file,SF_CREATMODE)) < 0 &&
				      errno == EINTR)
					errno = 0;
				CLOSE(tf);
			}
		}
		else if(oflags&O_CREAT)
		{	while((fd = creat(file,SF_CREATMODE)) < 0 && errno == EINTR)
				errno = 0;
			if(!(oflags&O_WRONLY))
			{	/* the file now exists, reopen it for read/write */
				CLOSE(fd);
				while((fd = open(file,oflags&(O_WRONLY|O_RDWR))) < 0 &&
				      errno == EINTR)
					errno = 0;
			}
		}
#endif
		if(fd < 0)
			return NIL(Sfio_t*);
	}

	oldfd = (f && !(f->flags&SF_STRING)) ? f->file : -1;

	if(sflags&SF_STRING)
		f = sfnew(f,(char*)file,
			  file ? (size_t)strlen((char*)file) : (size_t)SF_UNBOUND,
			  fd,sflags);
	else if((f = sfnew(f,NIL(char*),(size_t)SF_UNBOUND,
			   fd,sflags|SF_OPEN)) && oldfd >= 0)
		(void)sfsetfd(f,oldfd);

	return f;
}

#if __STD_C
int _sftype(reg const char* mode, int* oflagsp)
#else
int _sftype(mode, oflagsp)
reg char*	mode;
int*		oflagsp;
#endif
{
	reg int	sflags, oflags;

	if(!mode)
		return 0;

	/* construct the open flags */
	sflags = oflags = 0;
	while(1) switch(*mode++)
	{
	case 'w' :
		sflags |= SF_WRITE;
		oflags |= O_WRONLY | O_CREAT;
		if(!(sflags&SF_READ))
			oflags |= O_TRUNC;
		continue;
	case 'a' :
		sflags |= SF_WRITE | SF_APPENDWR;
		oflags |= O_WRONLY | O_APPEND | O_CREAT;
		continue;
	case 'r' :
		sflags |= SF_READ;
		oflags |= O_RDONLY;
		continue;
	case 's' :
		sflags |= SF_STRING;
		continue;
	case 'b' :
		oflags |= O_BINARY;
		continue;
	case 't' :
		oflags |= O_TEXT;
		continue;
	case 'x' :
		oflags |= O_EXCL;
		continue;
	case '+' :
		if(sflags)
			sflags |= SF_READ|SF_WRITE;
		continue;
	default :
		if(!(oflags&O_CREAT) )
			oflags &= ~O_EXCL;
		if((sflags&SF_RDWR) == SF_RDWR)
			oflags = (oflags&~(O_RDONLY|O_WRONLY))|O_RDWR;
		if(oflagsp)
			*oflagsp = oflags;
		if((sflags&(SF_STRING|SF_RDWR)) == SF_STRING)
			sflags |= SF_READ;
		return sflags;
	}
}
