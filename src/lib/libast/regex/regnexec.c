/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1985-2001 AT&T Corp.                *
*        and it may only be used by you under license from         *
*                       AT&T Corp. ("AT&T")                        *
*         A copy of the Source Code Agreement is available         *
*                at the AT&T Internet web site URL                 *
*                                                                  *
*       http://www.research.att.com/sw/license/ast-open.html       *
*                                                                  *
*        If you have copied this software without agreeing         *
*        to the terms of the license you are infringing on         *
*           the license and copyright and are violating            *
*               AT&T's intellectual property rights.               *
*                                                                  *
*                 This software was created by the                 *
*                 Network Services Research Center                 *
*                        AT&T Labs Research                        *
*                         Florham Park NJ                          *
*                                                                  *
*               Glenn Fowler <gsf@research.att.com>                *
*                David Korn <dgk@research.att.com>                 *
*                 Phong Vo <kpv@research.att.com>                  *
*******************************************************************/
#pragma prototyped

/*
 * posix regex executor
 * single sized-record interface
 */

#include "reglib.h"

#define BEG_ALT		1	/* beginning of an alt			*/
#define BEG_ONE		2	/* beginning of one iteration of a rep	*/
#define BEG_REP		3	/* beginning of a repetition		*/
#define BEG_SUB		4	/* beginning of a subexpression		*/
#define END_ANY		5	/* end of any of above			*/

/*
 * returns from parse()
 */

#define NONE		0	/* no parse found			*/
#define GOOD		1	/* some parse was found			*/
#define BEST		2	/* an unbeatable parse was found	*/
#define BAD		3	/* error ocurred			*/

/*
 * REG_SHELL_DOT test
 */

#define LEADING(e,r,s)	((e)->leading&&*(s)=='.'&&((s)==(e)->beg||*((s)-1)==(r)->explicit))

/*
 * Pos_t is for comparing parses. An entry is made in the
 * array at the beginning and at the end of each Group_t,
 * each iteration in a Group_t, and each Binary_t.
 */

typedef struct
{
	unsigned char*	p;		/* where in string		*/
	short		serial;		/* preorder subpattern number	*/
	short		be;		/* which end of pair		*/
} Pos_t;

/* ===== begin library support ===== */

#define vector(t,v,i)	(((i)<(v)->max)?(t*)((v)->vec+(i)*(v)->siz):(t*)vecseek(&(v),i))

static Vector_t*
vecopen(int inc, int siz)
{
	Vector_t*	v;
	Stk_t*		sp;

	if (inc <= 0)
		inc = 16;
	if (!(sp = stkopen(STK_SMALL|STK_NULL)))
		return 0;
	if (!(v = (Vector_t*)stkseek(sp, sizeof(Vector_t) + inc * siz)))
	{
		stkclose(sp);
		return 0;
	}
	v->stk = sp;
	v->vec = (char*)v + sizeof(Vector_t);
	v->max = v->inc = inc;
	v->siz = siz;
	v->cur = 0;
	return v;
}

static void*
vecseek(Vector_t** p, int index)
{
	Vector_t*	v = *p;

	if (index >= v->max)
	{
		while ((v->max += v->inc) <= index);
		if (!(v = (Vector_t*)stkseek(v->stk, sizeof(Vector_t) + v->max * v->siz)))
			return 0;
		*p = v;
		v->vec = (char*)v + sizeof(Vector_t);
	}
	return v->vec + index * v->siz;
}

static void
vecclose(Vector_t* v)
{
	if (v)
		stkclose(v->stk);
}

typedef struct
{
	Stk_pos_t	pos;
	char		data[1];
} Stk_frame_t;

#define stknew(s,p)	((p)->offset=stktell(s),(p)->base=stkfreeze(s,0))
#define stkold(s,p)	stkset(s,(p)->base,(p)->offset)

#define stkframe(s)	(*((Stk_frame_t**)(s)->_next-1))
#define stkdata(s,t)	((t*)stkframe(s)->data)
#define stkpop(s)	stkold(s,&(stkframe(s)->pos))

static void*
stkpush(Stk_t* sp, size_t size)
{
	Stk_frame_t*	f;
	Stk_pos_t	p;

	stknew(sp, &p);
	size = sizeof(Stk_frame_t) + sizeof(size_t) + size - 1;
	if (!(f = (Stk_frame_t*)stkalloc(sp, sizeof(Stk_frame_t) + sizeof(Stk_frame_t*) + size - 1)))
		return 0;
	f->pos = p;
	stkframe(sp) = f;
	return f->data;
}

/* ===== end library support ===== */

/*
 * Match_frame_t is for saving and restoring match records
 * around alternate attempts, so that fossils will not be
 * left in the match array.  These are the only entries in
 * the match array that are not otherwise guaranteed to
 * have current data in them when they get used.
 */

typedef struct
{
	size_t			size;
	regmatch_t*		match;
	regmatch_t		save[1];
} Match_frame_t;

#define matchframe	stkdata(stkstd,Match_frame_t)
#define matchpush(e,x)	((x)->re.group.number?_matchpush(e,x):0)
#define matchcopy(e,x)	((x)->re.group.number?memcpy(matchframe->match,matchframe->save,matchframe->size):(void*)0)
#define matchpop(e,x)	((x)->re.group.number?memcpy(matchframe->match,matchframe->save,matchframe->size),stkpop(stkstd):(void*)0)

#define pospop(e)	(--(e)->pos->cur)

/*
 * allocate a frame and push a match onto the stack
 */

static int
_matchpush(Env_t* env, Rex_t* rex)
{
	Match_frame_t*	f;
	regmatch_t*	m;
	regmatch_t*	e;
	regmatch_t*	s;
	int		num;

	if (rex->re.group.number <= 0 || (num = rex->re.group.last - rex->re.group.number + 1) <= 0)
		num = 0;
	if (!(f = (Match_frame_t*)stkpush(stkstd, sizeof(Match_frame_t) + (num - 1) * sizeof(regmatch_t))))
	{
		env->error = REG_ESPACE;
		return 1;
	}
	f->size = num * sizeof(regmatch_t);
	f->match = m = env->match + rex->re.group.number;
	e = m + num;
	s = f->save;
	while (m < e)
	{
		*s++ = *m;
		*m++ = state.nomatch;
	}
	return 0;
}

/*
 * allocate a frame and push a pos onto the stack
 */

static int
pospush(Env_t* env, Rex_t* rex, unsigned char* p, int be)
{
	Pos_t*	pos;

	if (!(pos = vector(Pos_t, env->pos, env->pos->cur)))
	{
		env->error = REG_ESPACE;
		return 1;
	}
	pos->serial = rex->serial;
	pos->p = p;
	pos->be = be;
	env->pos->cur++;
	return 0;
}

/*
 * two matches are known to have the same length
 * os is start of old pos array, ns is start of new,
 * oend and nend are end+1 pointers to ends of arrays.
 * oe and ne are ends (not end+1) of subarrays.
 * returns 1 if new is better, -1 if old, else 0.
 */

static int
better(Env_t* env, Pos_t* os, Pos_t* ns, Pos_t* oend, Pos_t* nend)
{
	Pos_t*	oe;
	Pos_t*	ne;
	int	k;
	int	n;

	if (env->error)
		return -1;
	for (;;)
	{
		if (ns >= nend)
			return os < oend;
		if (os >= oend)
		{
			if (os->be != BEG_ALT)
				/* can control get here? */
				env->error = REG_PANIC;
			return -1;
		}
		if (ns->serial > os->serial)
			return -1;
		if (os->serial > ns->serial)
		{
			env->error = REG_PANIC;
			return -1;
		}
		if (ns->p > os->p)
			/* can control get here? */
			return 1;
		if (os->p > ns->p)
			return -1;
		oe = os;
		n = oe->serial;
		k = 0;
		for (;;)
			if ((++oe)->serial == n)
			{
				if (oe->be != END_ANY)
					k++;
				else if (k-- <= 0)
					break;
			}
		ne = ns;
		n = ne->serial;
		k = 0;
		for (;;)
			if ((++ne)->serial == n)
			{
				if (ne->be != END_ANY)
					k++;
				else if (k-- <= 0)
					break;
			}
		if (ne->p > oe->p)
			return 1;
		if (oe->p > ne->p)
			return -1;
		if (k = better(env, os + 1, ns + 1, oe, ne))
			return k;
		os = oe + 1;
		ns = ne + 1;
	}
}

#define follow(e,r,c,s)	((r)->next?parse(e,(r)->next,c,s):(c)?parse(e,c,0,s):BEST)

static int		parse(Env_t*, Rex_t*, Rex_t*, unsigned char*);

static int
parserep(Env_t* env, Rex_t* rex, Rex_t* cont, unsigned char* s, int n)
{
	int	i;
	int	r = NONE;
	Rex_t	catcher;

	if ((rex->flags & REG_MINIMAL) && n >= rex->lo && n < rex->hi)
	{
		if (env->stack && pospush(env, rex, s, END_ANY))
			return BAD;
		i = follow(env, rex, cont, s);
		if (env->stack)
			pospop(env);
		switch (i)
		{
		case BAD:
			return BAD;
		case BEST:
		case GOOD:
			return BEST;
		}
	}
	if (n < rex->hi)
	{
		catcher.type = REX_REP_CATCH;
		catcher.serial = rex->serial;
		catcher.re.rep_catch.ref = rex;
		catcher.re.rep_catch.cont = cont;
		catcher.re.rep_catch.beg = s;
		catcher.re.rep_catch.n = n + 1;
		catcher.next = rex->next;
		if (env->stack)
		{
			if (matchpush(env, rex))
				return BAD;
			if (pospush(env, rex, s, BEG_ONE))	
				return BAD;
		}
		r = parse(env, rex->re.group.expr.rex, &catcher, s);
		if (env->stack)
		{
			pospop(env);
			matchpop(env, rex);
		}
		switch (r)
		{
		case BAD:
			return BAD;
		case BEST:
			return BEST;
		case GOOD:
			if (rex->flags & REG_MINIMAL)
				return BEST;
			r = GOOD;
			break;
		}
	}
	if (n < rex->lo)
		return r;
	if (!(rex->flags & REG_MINIMAL) || n >= rex->hi)
	{
		if (env->stack && pospush(env, rex, s, END_ANY))
			return BAD;
		i = follow(env, rex, cont, s);
		if (env->stack)
			pospop(env);
		switch (i)
		{
		case BAD:
			r = BAD;
			break;
		case BEST:
			r = BEST;
			break;
		case GOOD:
			r = (rex->flags & REG_MINIMAL) ? BEST : GOOD;
			break;
		}
	}
	return r;
}

static int
parsetrie(Env_t* env, Trie_node_t* x, Rex_t* rex, Rex_t* cont, unsigned char* s)
{
	unsigned char*	p;
	int		r;

	if (rex->flags & REG_ICASE)
	{
		p = state.fold;
		for (;;)
		{
			if (s >= env->end)
				return NONE;
			while (x->c != p[*s])
				if (!(x = x->sib))
					return NONE;
			if (x->end)
				break;
			x = x->son;
			s++;
		}
	}
	else
	{
		for (;;)
		{
			if (s >= env->end)
				return NONE;
			while (x->c != *s)
				if (!(x = x->sib))
					return NONE;
			if (x->end)
				break;
			x = x->son;
			s++;
		}
	}
	s++;
	if (rex->flags & REG_MINIMAL)
		switch (follow(env, rex, cont, s))
		{
		case BAD:
			return BAD;
		case BEST:
		case GOOD:
			return BEST;
		}
	if (x->son)
		switch (parsetrie(env, x->son, rex, cont, s))
		{
		case BAD:
			return BAD;
		case BEST:
			return BEST;
		case GOOD:
			if (rex->flags & REG_MINIMAL)
				return BEST;
			r = GOOD;
			break;
		default:
			r = NONE;
			break;
		}
	else
		r = NONE;
	if (!(rex->flags & REG_MINIMAL))
		switch (follow(env, rex, cont, s))
		{
		case BAD:
			return BAD;
		case BEST:
			return BEST;
		case GOOD:
			return GOOD;
	}
	return r;
}

static int
collmatch(Rex_t* rex, unsigned char* s, unsigned char* e, unsigned char** p)
{
	register Celt_t*	ce;
	unsigned char*		t;
	wchar_t			c;
	int			w;
	int			r;
	int			x;
	Ckey_t			key;
	Ckey_t			elt;

	if ((w = mbsize(s)) > 1)
	{
		memcpy((char*)key, (char*)s, w);
		key[w] = 0;
		mbxfrm(elt, key, COLL_KEY_MAX);
		t = s;
		c = mbchar(t);
		x = 0;
	}
	else
	{
		key[0] = s[0];
		key[1] = 0;
		r = mbxfrm(elt, key, COLL_KEY_MAX);
		while (w < COLL_KEY_MAX && &s[w] < e)
		{
			key[w] = s[w];
			key[w + 1] = 0;
			if (mbxfrm(elt, key, COLL_KEY_MAX) != r)
				break;
			w++;
		}
		key[w] = 0;
		mbxfrm(elt, key, COLL_KEY_MAX);
		c = s[0];
		x = w - 1;
	}
	r = 1;
	for (ce = rex->re.collate.elements;; ce++)
	{
		switch (ce->typ)
		{
		case COLL_call:
			if (!x && (*ce->fun)(c))
				break;
			continue;
		case COLL_char:
			if (!strcmp((char*)ce->beg, (char*)elt))
				break;
			continue;
		case COLL_range:
			if (strcmp((char*)ce->beg, (char*)elt) <= ce->min && strcmp((char*)elt, (char*)ce->end) <= ce->max)
				break;
			continue;
		case COLL_range_lc:
			if (strcmp((char*)ce->beg, (char*)elt) <= ce->min && strcmp((char*)elt, (char*)ce->end) <= ce->max && (islower(c) || !isupper(c)))
				break;
			continue;
		case COLL_range_uc:
			if (strcmp((char*)ce->beg, (char*)elt) <= ce->min && strcmp((char*)elt, (char*)ce->end) <= ce->max && (isupper(c) || !islower(c)))
				break;
			continue;
		default:
			r = 0;
			break;
		}
		if (!x || r)
			break;
		r = 1;
		w = x--;
		key[w] = 0;
		mbxfrm(elt, key, COLL_KEY_MAX);
	}
	*p = s + w;
	return rex->re.collate.invert ? !r : r;
}

static int
parse(Env_t* env, Rex_t* rex, Rex_t* cont, unsigned char* s)
{
	int		c;
	int		d;
	int		i;
	int		m;
	int		n;
	int		r;
	int*		f;
	unsigned char*	p;
	unsigned char*	t;
	unsigned char*	b;
	unsigned char*	e;
	regmatch_t*	o;
	Trie_node_t*	x;
	Rex_t		catcher;
	Rex_t		next;

	for (;;)
	{
		switch (rex->type)
		{
		case REX_ALT:
			if (matchpush(env, rex))
				return BAD;
			if (pospush(env, rex, s, BEG_ALT))
				return BAD;
			catcher.type = REX_ALT_CATCH;
			catcher.serial = rex->serial;
			catcher.re.alt_catch.cont = cont;
			catcher.next = rex->next;
			r = parse(env, rex->re.group.expr.binary.left, &catcher, s);
			if (r < BEST || (rex->flags & REG_MINIMAL))
			{
				matchcopy(env, rex);
				((Pos_t*)env->pos->vec + env->pos->cur - 1)->serial = catcher.serial = rex->re.group.expr.binary.serial;
				n = parse(env, rex->re.group.expr.binary.right, &catcher, s);
				if (n != NONE)
					r = n;
			}
			pospop(env);
			matchpop(env, rex);
			return r;
		case REX_ALT_CATCH:
			if (pospush(env, rex, s, END_ANY))
				return BAD;
			r = follow(env, rex, rex->re.alt_catch.cont, s);
			pospop(env);
			return r;
		case REX_BACK:
			o = &env->match[rex->lo];
			if (o->rm_so < 0)
				return NONE;
			i = o->rm_eo - o->rm_so;
			e = s + i;
			if (e > env->end)
				return NONE;
			t = env->beg + o->rm_so;
			if (!(rex->flags & REG_ICASE))
			{
				while (s < e)
					if (*s++ != *t++)
						return NONE;
			}
			else if (!mbwide())
			{
				p = state.fold;
				while (s < e)
					if (p[*s++] != p[*t++])
						return NONE;
			}
			else
			{
				while (s < e)
				{
					c = mbchar(s);
					d = mbchar(t);
					if (toupper(c) != toupper(d))
						return NONE;
				}
			}
			break;
		case REX_BEG:
			if ((!(env->flags & REG_NEWLINE) || s <= env->beg || *(s - 1) != '\n') && ((env->flags & REG_NOTBOL) || s != env->beg))
				return NONE;
			break;
		case REX_CLASS:
			if (LEADING(env, rex, s))
				return NONE;
			n = rex->hi;
			if (n > env->end - s)
				n = env->end - s;
			m = rex->lo;
			if (m > n)
				return NONE;
			r = NONE;
			if (!(rex->flags & REG_MINIMAL))
			{
				for (i = 0; i < n; i++)
					if (!settst(rex->re.charclass, s[i]))
					{
						n = i;
						break;
					}
				for (s += n; n-- >= m; s--)
					switch (follow(env, rex, cont, s))
					{
					case BAD:
						return BAD;
					case BEST:
						return BEST;
					case GOOD:
						r = GOOD;
						break;
					}
			}
			else
			{
				for (e = s + m; s < e; s++)
					if (!settst(rex->re.charclass, *s))
						return r;
				e += n - m;
				for (;;)
				{
					switch (follow(env, rex, cont, s))
					{
					case BAD:
						return BAD;
					case BEST:
					case GOOD:
						return BEST;
					}
					if (s >= e || !settst(rex->re.charclass, *s))
						break;
					s++;
				}
			}
			return r;
		case REX_COLL_CLASS:
			if (LEADING(env, rex, s))
				return NONE;
			n = rex->hi;
			if (n > env->end - s)
				n = env->end - s;
			m = rex->lo;
			if (m > n)
				return NONE;
			r = NONE;
			e = env->end;
			if (!(rex->flags & REG_MINIMAL))
			{
				if (!(b = (unsigned char*)stkpush(stkstd, n)))
				{
					env->error = REG_ESPACE;
					return BAD;
				}
				for (i = 0; s < e && i < n && collmatch(rex, s, e, &t); i++)
				{
					b[i] = t - s;
					s = t;
				}
				for (; i-- >= rex->lo; s -= b[i])
					switch (follow(env, rex, cont, s))
					{
					case BAD:
						stkpop(stkstd);
						return BAD;
					case BEST:
						stkpop(stkstd);
						return BEST;
					case GOOD:
						r = GOOD;
						break;
					}
				stkpop(stkstd);
			}
			else
			{
				for (i = 0; s < e && i < m; i++, s = t)
					if (!collmatch(rex, s, e, &t))
						return r;
				while (i++ < n)
				{
					switch (follow(env, rex, cont, s))
					{
					case BAD:
						return BAD;
					case BEST:
					case GOOD:
						return BEST;
					}
					if (s >= e || !collmatch(rex, s, e, &t))
						break;
					s = t;
				}
			}
			return r;
		case REX_CONJ:
			next.type = REX_CONJ_RIGHT;
			next.re.conj_right.cont = cont;
			next.next = rex->next;
			catcher.type = REX_CONJ_LEFT;
			catcher.re.conj_left.right = rex->re.group.expr.binary.right;
			catcher.re.conj_left.cont = &next;
			catcher.re.conj_left.beg = s;
			catcher.next = 0;
			return parse(env, rex->re.group.expr.binary.left, &catcher, s);
		case REX_CONJ_LEFT:
			rex->re.conj_left.cont->re.conj_right.end = s;
			cont = rex->re.conj_left.cont;
			s = rex->re.conj_left.beg;
			rex = rex->re.conj_left.right;
			continue;
		case REX_CONJ_RIGHT:
			if (rex->re.conj_right.end != s)
				return NONE;
			cont = rex->re.conj_right.cont;
			break;
		case REX_DONE:
			if (!env->stack)
				return BEST;
			n = s - env->beg;
			r = env->nsub;
			if ((i = env->best[0].rm_eo) >= 0)
			{
				if (rex->flags & REG_MINIMAL)
				{
					if (n > i)
						return GOOD;
				}
				else
				{
					if (n < i)
						return GOOD;
				}
				if (n == i && better(env,
						     (Pos_t*)env->bestpos->vec,
				   		     (Pos_t*)env->pos->vec,
				   		     (Pos_t*)env->bestpos->vec+env->bestpos->cur,
				   		     (Pos_t*)env->pos->vec+env->pos->cur) <= 0)
					return GOOD;
			}
			env->best[0].rm_eo = n;
			memcpy(&env->best[1], &env->match[1], r * sizeof(regmatch_t));
			n = env->pos->cur;
			if (!vector(Pos_t, env->bestpos, n))
			{
				env->error = REG_ESPACE;
				return BAD;
			}
			env->bestpos->cur = n;
			memcpy(env->bestpos->vec, env->pos->vec, n * sizeof(Pos_t));
			return GOOD;
		case REX_DOT:
			if (LEADING(env, rex, s))
				return NONE;
			n = rex->hi;
			if (n > env->end - s)
				n = env->end - s;
			m = rex->lo;
			if (m > n)
				return NONE;
			if ((c = rex->explicit) >= 0 && !mbwide())
				for (i = 0; i < n; i++)
					if (s[i] == c)
					{
						n = i;
						break;
					}
			r = NONE;
			if (!(rex->flags & REG_MINIMAL))
			{
				if (!mbwide())
				{
					for (s += n; n-- >= m; s--)
						switch (follow(env, rex, cont, s))
						{
						case BAD:
							return BAD;
						case BEST:
							return BEST;
						case GOOD:
							r = GOOD;
							break;
						}
				}
				else
				{
					if (!(b = (unsigned char*)stkpush(stkstd, n)))
					{
						env->error = REG_ESPACE;
						return BAD;
					}
					e = env->end;
					for (i = 0; s < e && i < n && s[i] != c; i++)
						s += b[i] = mbsize(s);
					for (; i-- >= m; s -= b[i])
						switch (follow(env, rex, cont, s))
						{
						case BAD:
							stkpop(stkstd);
							return BAD;
						case BEST:
							stkpop(stkstd);
							return BEST;
						case GOOD:
							r = GOOD;
							break;
						}
					stkpop(stkstd);
				}
			}
			else
			{
				if (!mbwide())
				{
					e = s + n;
					for (s += m; s <= e; s++)
						switch (follow(env, rex, cont, s))
						{
						case BAD:
							return BAD;
						case BEST:
						case GOOD:
							return BEST;
						}
				}
				else
				{
					e = env->end;
					for (i = 0; s < e && i < m && s[i] != c; i++)
						s += mbsize(s);
					if (i >= m)
						for (; s <= e && i < n; s += mbsize(s), i++)
							switch (follow(env, rex, cont, s))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
				}
			}
			return r;
		case REX_END:
			if ((!(env->flags & REG_NEWLINE) || *s != '\n') && ((env->flags & REG_NOTEOL) || s < env->end))
				return NONE;
			break;
		case REX_GROUP:
			if (env->stack)
			{
				env->match[rex->re.group.number].rm_so = s - env->beg;
				if (pospush(env, rex, s, BEG_SUB))
					return BAD;
				catcher.re.group_catch.eo = &env->match[rex->re.group.number].rm_eo;
			}
			catcher.type = REX_GROUP_CATCH;
			catcher.serial = rex->serial;
			catcher.re.group_catch.cont = cont;
			catcher.next = rex->next;
			r = parse(env, rex->re.group.expr.rex, &catcher, s);
			if (env->stack)
			{
				pospop(env);
				env->match[rex->re.group.number].rm_so = -1;
			}
			return r;
		case REX_GROUP_CATCH:
			if (env->stack)
			{
				*rex->re.group_catch.eo = s - env->beg;
				if (pospush(env, rex, s, END_ANY))
					return BAD;
			}
			r = follow(env, rex, rex->re.group_catch.cont, s);
			if (env->stack)
			{
				pospop(env);
				*rex->re.group_catch.eo = -1;
			}
			return r;
		case REX_GROUP_AHEAD:
			catcher.type = REX_GROUP_DONE;
			catcher.flags = rex->flags;
			catcher.re.group.size = 0;
			catcher.next = 0;
			r = parse(env, rex->re.group.expr.rex, &catcher, s);
			if (r == GOOD || r == BEST)
			{
				r = follow(env, rex, cont, s);
				if (r == GOOD)
					r = BEST;
			}
			return r;
		case REX_GROUP_AHEAD_NOT:
			r = parse(env, rex->re.group.expr.rex, NiL, s);
			if (r == NONE)
			{
				r = follow(env, rex, cont, s);
				if (r == GOOD)
					r = BEST;
			}
			else if (r != BAD)
				r = NONE;
			return r;
		case REX_GROUP_BEHIND:
			if ((s - env->beg) < rex->re.group.size)
				return NONE;
			catcher.type = REX_GROUP_BEHIND_CATCH;
			catcher.re.neg_catch.beg = s;
			catcher.next = &next;
			next.type = REX_GROUP_DONE;
			catcher.flags = rex->flags;
			catcher.re.group.size = 0;
			next.next = 0;
			e = env->end;
			env->end = s;
			for (t = s - rex->re.group.size; t >= env->beg; t--)
			{
				r = parse(env, rex->re.group.expr.rex, &catcher, t);
				if (r == GOOD || r == BEST)
				{
					env->end = e;
					r = follow(env, rex, cont, s);
					if (r == GOOD)
						r = BEST;
					return r;
				}
			}
			env->end = e;
			return NONE;
		case REX_GROUP_BEHIND_NOT:
			if ((s - env->beg) < rex->re.group.size)
				r = NONE;
			else
			{
				catcher.type = REX_GROUP_BEHIND_CATCH;
				catcher.re.neg_catch.beg = s;
				catcher.next = 0;
				r = parse(env, rex->re.group.expr.rex, &catcher, s - rex->re.group.size);
				e = env->end;
				env->end = s;
				for (t = s - rex->re.group.size; t >= env->beg; t--)
				{
					r = parse(env, rex->re.group.expr.rex, &catcher, t);
					if (r != NONE)
						break;
				}
				env->end = e;
			}
			if (r == NONE)
			{
				r = follow(env, rex, cont, s);
				if (r == GOOD)
					r = BEST;
			}
			else if (r != BAD)
				r = NONE;
			return r;
		case REX_GROUP_BEHIND_CATCH:
			return s == rex->re.neg_catch.beg ? BEST : NONE;
		case REX_GROUP_COND:
			if (rex->re.group.expr.binary.left)
			{
				catcher.type = REX_GROUP_DONE;
				catcher.flags = rex->flags;
				catcher.re.group.size = 0;
				catcher.next = 0;
				r = parse(env, rex->re.group.expr.rex, &catcher, s);
			}
			else if (!rex->re.group.size || rex->re.group.size > 0 && env->match[rex->re.group.size].rm_so >= 0)
				r = GOOD;
			else
				r = NONE;
			if (r == BAD)
				return BAD;
			rex = rex->re.group.expr.binary.right;
			rex = r == NONE ? rex->re.group.expr.binary.right : rex->re.group.expr.binary.left;
			return parse(env, rex, cont, s);
		case REX_GROUP_CUT:
			if (env->stack && pospush(env, rex, s, BEG_SUB))
				return BAD;
			catcher.type = REX_GROUP_CUT_CATCH;
			catcher.serial = rex->serial;
			catcher.re.group_catch.cont = cont;
			catcher.next = rex->next;
			r = parse(env, rex->re.group.expr.rex, &catcher, s);
			if (env->stack)
				pospop(env);
			return r;
		case REX_GROUP_CUT_CATCH:
			if (env->stack && pospush(env, rex, s, END_ANY))
				return BAD;
			switch (r = follow(env, rex, rex->re.group_catch.cont, s))
			{
			case GOOD:
				r = BEST;
				break;
			case NONE:
				r = BAD;
				env->error = REG_NOMATCH;
				break;
			}
			if (env->stack)
				pospop(env);
			return r;
		case REX_GROUP_DONE:
			/*
			 * NOTE: this code records the first of the matches
			 *	 of the same length
			 */

			if (env->zero)
			{
				i = s - env->beg;
				if (rex->flags & REG_MINIMAL)
				{
					if (rex->re.group.size < i)
						return GOOD;
				}
				else
				{
					if (rex->re.group.size > i)
						return GOOD;
				}
				rex->re.group.size = i;
				memcpy(&env->zero[1], &env->match[1], env->nsub * sizeof(regmatch_t));
			}
			return GOOD;
		case REX_KMP:
			f = rex->re.string.fail;
			b = rex->re.string.base;
			n = rex->re.string.size;
			t = s;
			e = env->end;
			if (rex->flags & REG_ICASE)
			{
				p = state.fold;
				while (t + n <= e)
				{
					for (i = -1; t < e; t++)
					{
						while (i >= 0 && b[i+1] != p[*t])
							i = f[i];
						if (b[i+1] == p[*t])
							i++;
						if (i + 1 == n)
						{
							t++;
							if (env->stack)
								env->best[0].rm_so = t - s - n;
							switch (follow(env, rex, cont, t))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
							t -= n - 1;
							break;
						}
					}
				}
			}
			else
			{
				while (t + n <= e)
				{
					for (i = -1; t < e; t++)
					{
						while (i >= 0 && b[i+1] != *t)
							i = f[i];
						if (b[i+1] == *t)
							i++;
						if (i + 1 == n)
						{
							t++;
							if (env->stack)
								env->best[0].rm_so = t - s - n;
							switch (follow(env, rex, cont, t))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
							t -= n - 1;
							break;
						}
					}
				}
			}
			return NONE;
		case REX_NEG:
			if (LEADING(env, rex, s))
				return NONE;
			i = env->end - s;
			n = ((i + 7) >> 3) + 1;
			catcher.type = REX_NEG_CATCH;
			catcher.re.neg_catch.beg = s;
			if (!(p = (unsigned char*)stkpush(stkstd, n)))
				return BAD;
			memset(catcher.re.neg_catch.index = p, 0, n);
			catcher.next = rex->next;
			if (parse(env, rex->re.group.expr.rex, &catcher, s) == BAD)
				r = BAD;
			else
			{
				r = NONE;
				for (; i >= 0; i--)
					if (!bittst(p, i))
					{
						switch (follow(env, rex, cont, s + i))
						{
						case BAD:
							r = BAD;
							break;
						case BEST:
							r = BEST;
							break;
						case GOOD:
							r = GOOD;
							/*FALLTHROUGH*/
						default:
							continue;
						}
						break;
					}
			}
			stkpop(stkstd);
			return r;
		case REX_NEG_CATCH:
			bitset(rex->re.neg_catch.index, s - rex->re.neg_catch.beg);
			return NONE;
		case REX_NULL:
			break;
		case REX_ONECHAR:
			n = rex->hi;
			if (n > env->end - s)
				n = env->end - s;
			m = rex->lo;
			if (m > n)
				return NONE;
			r = NONE;
			c = rex->re.onechar;
			if (!(rex->flags & REG_MINIMAL))
			{
				if (!mbwide())
				{
					if (!(rex->flags & REG_ICASE))
					{
						for (i = 0; i < n; i++, s++)
							if (*s != c)
								break;
					}
					else
					{
						p = state.fold;
						for (i = 0; i < n; i++, s++)
							if (p[*s] != c)
								break;
					}
					for (; i-- >= m; s--)
						switch (follow(env, rex, cont, s))
						{
						case BAD:
							return BAD;
						case BEST:
							return BEST;
						case GOOD:
							r = GOOD;
							break;
						}
				}
				else
				{
					if (!(b = (unsigned char*)stkpush(stkstd, n)))
					{
						env->error = REG_ESPACE;
						return BAD;
					}
					e = env->end;
					if (!(rex->flags & REG_ICASE))
					{
						for (i = 0; s < e && i < n; i++, s = t)
						{
							t = s;
							if (mbchar(t) != c)
								break;
							b[i] = t - s;
						}
					}
					else
					{
						for (i = 0; s < e && i < n; i++, s = t)
						{
							t = s;
							if (toupper(mbchar(t)) != c)
								break;
							b[i] = t - s;
						}
					}
					for (; i-- >= m; s -= b[i])
						switch (follow(env, rex, cont, s))
						{
						case BAD:
							stkpop(stkstd);
							return BAD;
						case BEST:
							stkpop(stkstd);
							return BEST;
						case GOOD:
							r = GOOD;
							break;
						}
					stkpop(stkstd);
				}
			}
			else
			{
				if (!mbwide())
				{
					e = s + m;
					if (!(rex->flags & REG_ICASE))
					{
						for (; s < e; s++)
							if (*s != c)
								return r;
						e += n - m;
						for (;;)
						{
							switch (follow(env, rex, cont, s))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
							if (s >= e || *s++ != c)
								break;
						}
					}
					else
					{
						p = state.fold;
						for (; s < e; s++)
							if (p[*s] != c)
								return r;
						e += n - m;
						for (;;)
						{
							switch (follow(env, rex, cont, s))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
							if (s >= e || p[*s++] != c)
								break;
						}
					}
				}
				else
				{
					if (!(rex->flags & REG_ICASE))
					{
						for (i = 0; i < m && s < e; i++, s = t)
						{
							t = s;
							if (mbchar(t) != c)
								return r;
						}
						while (i++ <= n && s < e)
						{
							switch (follow(env, rex, cont, s))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
							if (mbchar(s) != c)
								break;
						}
					}
					else
					{
						for (i = 0; i < m && s < e; i++, s = t)
						{
							t = s;
							if (toupper(mbchar(t)) != c)
								return r;
						}
						while (i++ <= n && s < e)
						{
							switch (follow(env, rex, cont, s))
							{
							case BAD:
								return BAD;
							case BEST:
							case GOOD:
								return BEST;
							}
							if (toupper(mbchar(s)) != c)
								break;
						}
					}
				}
			}
			return r;
		case REX_REP:
			if (env->stack && pospush(env, rex, s, BEG_REP))
				return BAD;
			r = parserep(env, rex, cont, s, 0);
			if (env->stack)
				pospop(env);
			return r;
		case REX_REP_CATCH:
			if (env->stack && pospush(env, rex, s, END_ANY))
				return BAD;
			if (rex->re.rep_catch.beg == s && rex->re.rep_catch.n > rex->re.rep_catch.ref->lo)
			{
				/*
				 * optional empty iteration
				 */

				if (!env->stack || (env->flags & REG_EXTENDED))
					r = NONE;
				else if (pospush(env, rex, s, END_ANY))
					r = BAD;
				else
				{
					r = follow(env, rex, rex->re.rep_catch.cont, s);
					pospop(env);
				}
			}
			else
				r = parserep(env, rex->re.rep_catch.ref, rex->re.rep_catch.cont, s, rex->re.rep_catch.n);
			if (env->stack)
				pospop(env);
			return r;
		case REX_STRING:
			if (rex->re.string.size > (env->end - s))
				return NONE;
			t = rex->re.string.base;
			e = t + rex->re.string.size;
			if (!(rex->flags & REG_ICASE))
			{
				while (t < e)
					if (*s++ != *t++)
						return NONE;
			}
			else if (!mbwide())
			{
				p = state.fold;
				while (t < e)
					if (p[*s++] != *t++)
						return NONE;
			}
			else
			{
				while (t < e)
				{
					c = mbchar(s);
					d = mbchar(t);
					if (toupper(c) != d)
						return NONE;
				}
			}
			break;
		case REX_TRIE:
			if (((s + rex->re.trie.min) > env->end) || !(x = rex->re.trie.root[(rex->flags & REG_ICASE) ? toupper(*s) : *s]))
				return NONE;
			return parsetrie(env, x, rex, cont, s);
		case REX_EXEC:
			e = 0;
			r = (*env->disc->re_execf)(env->regex, rex->re.exec.data, rex->re.exec.text, rex->re.exec.size, (const char*)s, env->end - s, (char**)&e, env->disc);
			if (e >= s && e <= env->end)
				s = e;
			switch (r)
			{
			case 0:
				break;
			case REG_NOMATCH:
				return NONE;
			default:
				env->error = r;
				return BAD;
			}
			break;
		case REX_WBEG:
			if (!isword(*s) || s > env->beg && isword(*(s - 1)))
				return NONE;
			break;
		case REX_WEND:
			if (isword(*s) || s > env->beg && !isword(*(s - 1)))
				return NONE;
			break;
		case REX_WORD:
			if (s > env->beg && isword(*(s - 1)) == isword(*s))
				return NONE;
			break;
		case REX_WORD_NOT:
			if (s == env->beg || isword(*(s - 1)) != isword(*s))
				return NONE;
			break;
		case REX_BEG_STR:
			if (s != env->beg)
				return NONE;
			break;
		case REX_END_STR:
			for (t = s; t < env->end && *t == '\n'; t++);
			if (t < env->end)
				return NONE;
			break;
		case REX_FIN_STR:
			if (s < env->end)
				return NONE;
			break;
		}
		if (!(rex = rex->next))
		{
			if (!(rex = cont))
				break;
			cont = 0;
		}
	}
	return GOOD;
}

/*
 * returning REG_BADPAT or REG_ESPACE is not explicitly
 * countenanced by the standard
 */

int
regnexec(const regex_t* p, const char* s, size_t len, size_t nmatch, regmatch_t* match, regflags_t flags)
{
	register int	n;
	register int	i;
	int		j;
	int		k;
	int		m;
	Env_t*		env;
	Rex_t*		e;

	if (!s || !p || !(env = p->env))
		return fatal(p->env->disc, REG_BADPAT, NiL);
	if (len < env->min)
		return REG_NOMATCH;
#if __OBSOLETE__ < 20030101L
	/*
	 * repeat 1000x: sharing bits is never worth it
	 */

	if (flags & REG_MULTIPLE)
	{
		flags &= ~REG_MULTIPLE;
		flags |= REG_INVERT;
	}
	if (flags & REG_DELIMITED)
	{
		flags &= ~REG_DELIMITED;
		flags |= REG_STARTEND;
	}
#endif
	env->regex = p;
	env->beg = (unsigned char*)s;
	env->end = env->beg + len;
	stknew(stkstd, &env->stk);
	env->flags &= ~REG_EXEC;
	env->flags |= (flags & REG_EXEC);
	if (env->stack = env->hard || !(env->flags & REG_NOSUB) && nmatch)
	{
		n = env->nsub;
		if (!(env->match = (regmatch_t*)stkpush(stkstd, (env->zeroes + 2) * (n + 1) * sizeof(regmatch_t))) ||
		    !env->pos && !(env->pos = vecopen(16, sizeof(Pos_t))) ||
		    !env->bestpos && !(env->bestpos = vecopen(16, sizeof(Pos_t))))
		{
			k = REG_ESPACE;
			goto done;
		}
		env->pos->cur = env->bestpos->cur = 0;
		env->best = &env->match[n + 1];
		env->best[0].rm_so = 0;
		env->best[0].rm_eo = -1;
		for (i = 0; i <= n; i++)
			env->match[i] = state.nomatch;
		if (env->zeroes)
		{
			env->zero = &env->best[n + 1];
			for (i = 0; i <= n; i++)
				env->zero[i] = state.nomatch;
		}
		else
			env->zero = 0;
	}
	else
		env->zero = 0;
	k = REG_NOMATCH;
	if ((e = env->rex)->type == REX_BM)
	{
		if (len < e->re.bm.right)
			goto done;
		else
		{
			register unsigned char*	buf = (unsigned char*)s;
			register size_t		index = e->re.bm.left + e->re.bm.size;
			register size_t		mid = len - e->re.bm.right;
			register size_t*	skip = e->re.bm.skip;
			register size_t*	fail = e->re.bm.fail;
			register Bm_mask_t**	mask = e->re.bm.mask;
			Bm_mask_t		m;

			for (;;)
			{
				while ((index += skip[buf[index]]) < mid);
				if (index < HIT)
					goto done;
				index -= HIT;
				m = mask[n = e->re.bm.size - 1][buf[index]];
				do
				{
					if (!n--)
						goto possible;
				} while (m &= mask[n][buf[--index]]);
				if ((index += fail[n + 1]) >= len)
					goto done;
			}
 possible:
			n = env->nsub;
			e = e->next;
		}
	}
	while (parse(env, e, &env->done, (unsigned char*)s) == NONE)
	{
		if (env->once)
			goto done;
		i = mbsize(s);
		s += i;
		if ((unsigned char*)s > env->end - env->min)
			goto done;
		if (env->stack)
			env->best[0].rm_so += i;
	}
	if (k = env->error)
		goto done;
	if (!(env->flags & REG_NOSUB))
	{
		k = (env->flags & (REG_SHELL|REG_AUGMENTED)) == (REG_SHELL|REG_AUGMENTED);
		for (i = j = m = 0; j < nmatch; i++)
			if (!i || !k || (i & 1))
			{
				if (i > n)
					match[j] = state.nomatch;
				else
					match[m = j] = (env->zero && env->best[i].rm_so < 0) ? env->zero[i] : env->best[i];
				j++;
			}
		if (k)
		{
			while (m > 0 && match[m].rm_so == -1 && match[m].rm_eo == -1)
				m--;
			((regex_t*)p)->re_nsub = m;
		}
	}
	k = 0;
 done:
	stkold(stkstd, &env->stk);
	env->stk.base = 0;
	if (k > REG_NOMATCH)
		fatal(p->env->disc, k, NiL);
	return k;
}

void
regfree(regex_t* p)
{
	Env_t*	env;

	if (p && (env = p->env))
	{
		p->env = 0;
		drop(env->disc, env->rex);
		if (env->pos)
			vecclose(env->pos);
		if (env->bestpos)
			vecclose(env->bestpos);
		if (env->stk.base)
			stkold(stkstd, &env->stk);
		alloc(env->disc, env, 0);
	}
}
