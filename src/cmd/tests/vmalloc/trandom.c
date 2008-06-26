/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1999-2008 AT&T Intellectual Property          *
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
#include	"vmtest.h"
#include	<stdlib.h>

#define TWARN		0		/* enable debug warnings	*/

static unsigned long	RAND_state = 1;

#define RAND()		(RAND_state = RAND_state * 0x63c63cd9L + 0x9c39c33dL)

#define N_OBJ		500000		/* number of allocated objects	*/

#define COMPACT		(N_OBJ/5)	/* period for compacting	*/

#define HUGESZ		(4*1024)	/* increase huge block by this	*/
#define H_FREQ		(N_OBJ/100)	/* 1/H_FREQ: resizing probab.	*/

/* size of an allocation or resize */
#define L_FREQ		100		/* one out of L_FREQ is LARGE	*/		
#define LARGE		10000		/* size of large objects	*/
#define SMALL		100		/* size of small objects	*/
#define MICRO		10
#define L_ALLOC()	(RAND()%LARGE + LARGE)
#define S_ALLOC()	(RAND()%SMALL + SMALL)
#define M_ALLOC()	(RAND()%MICRO + 1)
#define ALLOCSZ()	(RAND()%L_FREQ == 0 ? L_ALLOC() : \
			 RAND()%2 == 0 ? S_ALLOC() : M_ALLOC() )

/* when to resize or free */
#define L_RANGE		10000		/* when to resize/free large 	*/
#define S_RANGE		10		/* when to resize/free small	*/
#define M_RANGE		5		/* when to resize/free micro	*/
#define RANGE(sz)	(sz >= LARGE ? L_RANGE : sz >= SMALL ? S_RANGE : M_RANGE)
#define WHEN(sz)	((RAND() % RANGE(sz)) + 1)

typedef struct _obj_s	Obj_t;
struct _obj_s
{	Void_t*	obj;
	size_t	size;
	Obj_t*	head;
	Obj_t*	next;
};
static Obj_t		Obj[N_OBJ+1];

main()
{
	Obj_t		*o, *next, *last, *n, *f;
	Void_t		*huge;
	size_t		sz, hugesz;
	int		counter;
#if TWARN
	Vmstat_t	sb;
#endif

	hugesz = HUGESZ;
	if(!(huge = vmalloc(Vmregion, hugesz)) )
		terror("Can't allocate block");

	for(counter = 1, o = Obj, last = Obj+N_OBJ; o < last; ++o, ++counter)
	{	
		/* free all stuff that should be freed */
		for(f = o->head; f; f = next)
		{	next = f->next;

			if(f->obj)
			{	if((RAND()%2) == 0 ) /* resize */
				{	sz = ALLOCSZ();
					f->obj = vmresize(Vmregion,f->obj,sz,VM_RSMOVE);
					if(!f->obj)
						terror("Vmresize failed");
					f->size = sz;

					if((n = o + WHEN(sz)) > last)
						n = last;
					f->next = n->head;
					n->head = f;
				}
				else	vmfree(Vmregion,f->obj);
			}
		}

		if(counter%COMPACT == 0)
		{	
#if TWARN
			if(vmstat(Vmregion, &sb) < 0)
				terror("Vmstat failed");
			twarn("Arena: busy=(%u,%u) free=(%u,%u) extent=%u #segs=%d",
				sb.n_busy,sb.s_busy, sb.n_free,sb.s_free,
				sb.extent, sb.n_seg);
#endif
			if(vmcompact(Vmregion) < 0 )
				terror("Vmcompact failed");
#if TWARN
			if(vmstat(Vmregion, &sb) < 0)
				terror("Vmstat failed");
			twarn("Compact: busy=(%u,%u) free=(%u,%u) extent=%u #segs=%d",
				sb.n_busy,sb.s_busy, sb.n_free,sb.s_free,
				sb.extent, sb.n_seg);
#endif
		}

		if(((o-Obj)%H_FREQ) == 0 )
		{	hugesz += HUGESZ;
			if(!(huge = vmresize(Vmregion, huge, hugesz, VM_RSMOVE)) )
				terror("Bad resize of huge block");
		}

		sz = ALLOCSZ();
		o->obj = vmalloc(Vmregion, sz);
		if(!o->obj)
			terror("Vmalloc failed");
		o->size = sz;

		if((n = o + WHEN(sz)) > last)
			n = last;
		o->next = n->head;
		n->head = o;
	}

	if(vmdbcheck(Vmregion) < 0)
		terror("Corrupted region");

#if TWARN
	if(vmstat(Vmregion, &sb) < 0)
		terror("Vmstat failed");
	twarn("Full: Busy=(%u,%u) Free=(%u,%u) Extent=%u #segs=%d\n",
		sb.n_busy, sb.s_busy, sb.n_free, sb.s_free, sb.extent, sb.n_seg);
#endif
	if(vmcompact(Vmregion) < 0 )
		terror("Vmcompact failed");
#if TWARN
	if(vmstat(Vmregion, &sb) < 0)
		terror("Vmstat failed");
	twarn("Compact: Busy=(%u,%u) Free=(%u,%u) Extent=%u #segs=%d\n",
		sb.n_busy, sb.s_busy, sb.n_free, sb.s_free, sb.extent, sb.n_seg);
#endif

	for(f = last->head; f; f = next)
	{	next = f->next;
		vmfree(Vmregion,f->obj);
	}
	vmfree(Vmregion,huge);

#if TWARN
	if(vmstat(Vmregion, &sb) < 0)
		terror("Vmstat failed");
	twarn("Free: Busy=(%u,%u) Free=(%u,%u) Extent=%u #segs=%d\n",
		sb.n_busy, sb.s_busy, sb.n_free, sb.s_free, sb.extent, sb.n_seg);
#endif
	if(vmcompact(Vmregion) < 0 )
		terror("Vmcompact failed2");
#if TWARN
	if(vmstat(Vmregion, &sb) < 0)
		terror("Vmstat failed");
	twarn("Compact: Busy=(%u,%u) Free=(%u,%u) Extent=%u #segs=%d\n",
		sb.n_busy, sb.s_busy, sb.n_free, sb.s_free, sb.extent, sb.n_seg);
#endif

	if(!(huge = vmalloc(Vmregion, SMALL)))
		terror("Vmalloc failed");
#if TWARN
	if(vmstat(Vmregion, &sb) < 0)
		terror("Vmstat failed");
	twarn("Small: Busy=(%u,%u) Free=(%u,%u) Extent=%u #segs=%d\n",
		sb.n_busy, sb.s_busy, sb.n_free, sb.s_free, sb.extent, sb.n_seg);
#endif

	exit(0);
}
