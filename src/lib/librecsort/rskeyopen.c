/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1996-2009 AT&T Intellectual Property          *
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
*                   Phong Vo <kpv@research.att.com>                    *
*                 Glenn Fowler <gsf@research.att.com>                  *
*                                                                      *
***********************************************************************/
#pragma prototyped

/*
 * keyed data support for recsort
 */

#include "rskeyhdr.h"

static const char id[] = "\n@(#)$Id: rskey library (AT&T Research) 2009-12-09 $\0\n";

static const char lib[] = "librecsort:rskey";

static State_t	state;

/*
 * initialize the global readonly tables
 */

static void
#if __STD_C
initialize(void)
#else
initialize()
#endif
{
	register int	i;

	setlocale(LC_ALL, "");
	for (i = 0; i <= UCHAR_MAX; i++)
	{
		state.all[i] = 1;
		state.ident[i] = i;
		state.fold[i] = islower(i) ? toupper(i) : i;
		if (blank(i))
			state.dict[i] = 1;
		if (isalnum(i))
		{
			state.dict[i] = 1;
			state.print[i] = 1;
		}
		else if (isprint(i))
			state.print[i] = 1;
	}
}

/*
 * open a recsort key discipline handle
 */

Rskey_t*
#if __STD_C
rskeyopen(Rskeydisc_t* disc)
#else
rskeyopen(disc)
Rskeydisc_t*	disc;
#endif
{
	register Rskey_t*	kp;

	if (!state.dict[' '])
		initialize();
	if (!(kp = vmnewof(Vmheap, 0, Rskey_t, 1, sizeof(Rsdisc_t))))
		return 0;
	kp->id = lib;
	kp->disc = (Rsdisc_t*)(kp + 1);
	kp->disc->version = RS_VERSION;
	kp->disc->keylen = -1;
	kp->disc->data = REC_D_TYPE('\n');
	kp->keydisc = disc;
	kp->state = &state;
	kp->insize = INSIZE;
	kp->outsize = OUTSIZE;
	kp->procsize = PROCSIZE;
	kp->field.head = kp->field.tail = &kp->field.global;
	kp->field.global.end.field = MAXFIELD;
	kp->meth = Rsrasp;
	if (mbcoll())
	{
		kp->xfrmsiz = 256;
		if (!(kp->xfrmbuf = vmnewof(Vmheap, 0, unsigned char, kp->xfrmsiz, 0)))
		{
			vmfree(Vmheap, kp);
			kp = 0;
		}
	}
	return kp;
}

/*
 * close an rskeyopen() handle
 */

int
#if __STD_C
rskeyclose(Rskey_t* kp)
#else
rskeyclose(kp)
Rskey_t*	kp;
#endif
{
	register Field_t*	fp;
	register Field_t*	np;

	if (!kp)
		return -1;
	np = kp->field.global.next;
	while (fp = np)
	{
		np = fp->next;
		if (fp->freetrans)
			vmfree(Vmheap, fp->trans);
		vmfree(Vmheap, fp);
	}
	np = kp->accumulate.head;
	while (fp = np)
	{
		np = fp->next;
		vmfree(Vmheap, fp);
	}
	if (kp->field.positions)
		vmfree(Vmheap, kp->field.positions);
	vmfree(Vmheap, kp);
	return 0;
}
