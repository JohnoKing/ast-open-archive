/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 2000-2011 AT&T Intellectual Property          *
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
#ifndef _IV_H
#define _IV_H	1

/*	Library for handling intervals of arbitrary sized unsigned integer points.
**
**	ivset(iv, lo, hi, data): specifies an interval [lo,hi]
**		associated with the given "data".
**	ivget(iv, pt): find the data currently associated with "pt".
**	ivdel(iv, lo, hi): deletes the specified interval [lo,hi].
**
**	Written by Kiem-Phong Vo and Glenn Fowler.
*/

#include <fv.h>

#define IV_VERSION	20090717L

#define IV_OPEN		1
#define IV_CLOSE	2

/* types related to the interval handle */
typedef unsigned char*		Ivpoint_t;	/* interval point		*/
typedef struct Iv_s		Iv_t;		/* handle structure		*/
typedef struct Ivdisc_s		Ivdisc_t;	/* interval discipline		*/
typedef struct Ivmeth_s		Ivmeth_t;	/* interval method		*/
typedef struct Ivseg_s		Ivseg_t;	/* a segment of data		*/

typedef int	(*Ivevent_f)(Iv_t*, int, void*);

struct Ivseg_s
{	
	unsigned char*	lo;
	unsigned char*	hi;
	void*		data;
#ifdef _IV_SEG_PRIVATE_
	_IV_SEG_PRIVATE_
#endif
};

struct Ivdisc_s
{
	uint32_t	version;	/* api version				*/
	void*		unmatched;	/* data for unmatched addresses		*/ 
	Error_f		errorf;		/* error handling function		*/
};

struct Ivmeth_s
{
	const char*	name;
	const char*	description;
	const char*	options;
	unsigned char*	(*getf)(Iv_t*, unsigned char*);
	int		(*setf)(Iv_t*, unsigned char*, unsigned char*, void*);
	int		(*delf)(Iv_t*, unsigned char*, unsigned char*);
	Ivseg_t*	(*segf)(Iv_t*, unsigned char*);
	int		(*eventf)(Iv_t*, int, void*);
};

struct Iv_s
{
	Ivmeth_t*	meth;
	Ivdisc_t*	disc;
	void*		data;
	unsigned char*	unit;
	unsigned char*	r1;
	unsigned char*	r2;
	int		size;
};

#define ivinit(p)		(memset(p,0,sizeof(*(p))),(p)->version=IV_VERSION)

#define ivset(iv,lo,hi,dt)	(*(iv)->meth->setf)((iv),(lo),(hi),(dt))
#define ivget(iv,pt)		(*(iv)->meth->getf)((iv),(pt))
#define ivdel(iv,lo,hi)		(*(iv)->meth->delf)((iv),(lo),(hi))
#define ivseg(iv,pt)		(*(iv)->meth->segf)((iv),(pt))

_BEGIN_EXTERNS_

extern Ivmeth_t*	ivmeth(const char*);
extern Iv_t*		ivopen(Ivdisc_t*, Ivmeth_t*, int, const char*);
extern int		ivclose(Iv_t*);

_END_EXTERNS_

#if _BLD_iv || _BLD_ip_t || _PACKAGE_astsa

#define IVLIB(m)	Ivmeth_t* Iv##m = &_##Iv##m;

#else

#if defined(__EXPORT__)
#define IVLIB(m)	Ivmethod_t* Iv##m = &_##Iv##m; extern __EXPORT__ Ivmethod_t* vcodex_lib(const char* path) { return Iv##m; }
#else
#define IVLIB(m)	Ivmethod_t* Iv##m = &_##Iv##m; extern Ivmethod_t* iv_lib(const char* path) { return Iv##m; }
#endif

#endif

#endif /* _IV_H */
