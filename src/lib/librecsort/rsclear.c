/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1996-2003 AT&T Corp.                *
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
*                 Phong Vo <kpv@research.att.com>                  *
*               Glenn Fowler <gsf@research.att.com>                *
*                                                                  *
*******************************************************************/
#include	"rshdr.h"

/*	Clear space in a region
**
**	Written by Kiem-Phong Vo (07/18/96)
*/

#if __STD_C
int rsclear(Rs_t* rs)
#else
int rsclear(rs)
Rs_t*	rs;
#endif
{
	reg uchar	*m, *endm;

	for(m = (uchar*)rs->methdata, endm = m+rs->meth->size; m < endm; ++m)
		*m = 0;

	if(rs->vm)
		vmclear(rs->vm);
	rs->c_size = 0;
	rs->type &= RS_TYPES;
	rs->free = rs->sorted = NIL(Rsobj_t*);

	return 0;
}
