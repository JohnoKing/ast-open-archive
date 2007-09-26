/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*           Copyright (c) 2000-2007 AT&T Knowledge Ventures            *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                      by AT&T Knowledge Ventures                      *
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
#pragma prototyped
/*
 * dss ip type library
 *
 * Glenn Fowler
 * AT&T Research
 */

static const char id[] = "\n@(#)$Id: dss ip type library (AT&T Research) 2007-09-05 $\0\n";

#include <dsslib.h>
#include <bgp.h>
#include <ire.h>
#include <itl.h>
#include <pt.h>

#define PREFIX(a,b)	((Cxnumber_t)(a)*64+(b))

#if _typ_int64_t

#define PREFIX_ADDR(p)	((Ptaddr_t)(((Cxinteger_t)(p))>>6))
#define PREFIX_BITS(p)	((int)(((Cxinteger_t)(p)) & 0x3f))

#else

#define PREFIX_ADDR(p)	((Ptaddr_t)((p)/64))
#define PREFIX_BITS(p)	prefix_bits(p)

static int
prefix_bits(Cxnumber_t p)
{
	Cxnumber_t	a;
	unsigned long	u;

	u = p / 64;
	a = u;
	a *= 64;
	return (int)(p - a) & 0x3f;
}

#endif

static Iredisc_t	iredisc;

static ssize_t
aspath_external(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, char* buf, size_t size, Cxdisc_t* disc)
{
	return itl2external(cx, type, 1, 1, details, &format, value, buf, size, disc);
}

static ssize_t
aspath_internal(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, const char* buf, size_t size, Vmalloc_t* vm, Cxdisc_t* disc)
{
	return itl2internal(cx, value, 1, 1, buf, size, vm, disc);
}

static void*
aspath_match_comp(Cx_t* cx, Cxtype_t* sub, Cxtype_t* pat, Cxvalue_t* val, Cxdisc_t* disc)
{
	if (!cxisstring(pat))
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, 2, "%s: match requires %s pattern", sub->name, cx->state->type_string->name, sub->name);
		return 0;
	}
	iredisc.version = IRE_VERSION;
	iredisc.errorf = disc->errorf;
	return irecomp(val->string.data, 2, 1, 1, &iredisc);
}

static ssize_t
cluster_external(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, char* buf, size_t size, Cxdisc_t* disc)
{
	return itl4external(cx, type, 1, 0, details, &format, value, buf, size, disc);
}

static ssize_t
cluster_internal(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, const char* buf, size_t size, Vmalloc_t* vm, Cxdisc_t* disc)
{
	return itl4internal(cx, value, 1, 0, buf, size, vm, disc);
}

static void*
cluster_match_comp(Cx_t* cx, Cxtype_t* sub, Cxtype_t* pat, Cxvalue_t* val, Cxdisc_t* disc)
{
	if (!cxisstring(pat))
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, 2, "%s: match requires %s pattern", sub->name, cx->state->type_string->name, sub->name);
		return 0;
	}
	iredisc.version = IRE_VERSION;
	iredisc.errorf = disc->errorf;
	return irecomp(val->string.data, 4, 2, 0, &iredisc);
}

static ssize_t
community_external(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, char* buf, size_t size, Cxdisc_t* disc)
{
	Cxformat_t*	formats[2];

	formats[0] = 0;
	formats[1] = format;
	return itl2external(cx, type, 2, 0, details, formats, value, buf, size, disc);
}

static ssize_t
community_internal(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, const char* buf, size_t size, Vmalloc_t* vm, Cxdisc_t* disc)
{
	return itl2internal(cx, value, 2, 0, buf, size, vm, disc);
}

static void*
community_match_comp(Cx_t* cx, Cxtype_t* sub, Cxtype_t* pat, Cxvalue_t* val, Cxdisc_t* disc)
{
	if (!cxisstring(pat))
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, 2, "%s: match requires %s pattern", sub->name, cx->state->type_string->name, sub->name);
		return 0;
	}
	iredisc.version = IRE_VERSION;
	iredisc.errorf = disc->errorf;
	return irecomp(val->string.data, 2, 2, 0, &iredisc);
}

static ssize_t
ipaddr_external(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, char* buf, size_t size, Cxdisc_t* disc)
{
	char*	s;
	ssize_t	n;

	s = fmtip4((Ptaddr_t)value->number, -1);
	n = strlen(s);
	if ((n + 1) > size)
		return n + 1;
	memcpy(buf, s, n + 1);
	return n;
}

static ssize_t
ipaddr_internal(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, const char* buf, size_t size, Vmalloc_t* vm, Cxdisc_t* disc)
{
	char*		e;
	Ptaddr_t	addr;

	if (strtoip4(buf, &e, &addr, NiL))
	{
		if (disc->errorf && !(cx->flags & CX_QUIET))
			(*disc->errorf)(cx, disc, 1, "%-.*s: invalid ip address", size, buf);
		return -1;
	}
	value->number = addr;
	return e - (char*)buf;
}

static ssize_t
ipprefix_external(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, char* buf, size_t size, Cxdisc_t* disc)
{
	char*	s;
	ssize_t	n;

	if (s = (char*)CXDETAILS(details, format, type, 0))
		s = sfprints(s, PREFIX_ADDR(value->number), PREFIX_BITS(value->number));
	else
		s = fmtip4(PREFIX_ADDR(value->number), PREFIX_BITS(value->number));
	n = strlen(s);
	if ((n + 1) > size)
		return n + 1;
	memcpy(buf, s, n + 1);
	return n;
}

static ssize_t
ipprefix_internal(Cx_t* cx, Cxtype_t* type, const char* details, Cxformat_t* format, Cxvalue_t* value, const char* buf, size_t size, Vmalloc_t* vm, Cxdisc_t* disc)
{
	char*		e;
	Ptaddr_t	addr;
	unsigned char	bits;

	if (strtoip4(buf, &e, &addr, &bits))
	{
		if (disc->errorf && !(cx->flags & CX_QUIET))
			(*disc->errorf)(cx, disc, 1, "%-.*s: invalid ip prefix", size, buf);
		return -1;
	}
	value->number = PREFIX(addr, bits);
	return e - (char*)buf;
}

static int
match_list_exec(Cx_t* cx, void* data, Cxvalue_t* val, Cxdisc_t* disc)
{
	return ireexec((Ire_t*)data, val->buffer.data, val->buffer.size) != 0;
}

static int
match_list_free(Cx_t* cx, void* data, Cxdisc_t* disc)
{
	return irefree((Ire_t*)data);
}

static Cxmatch_t	match_aspath =
{
	"aspath-re",
	"Matches on this type treat a string pattern as an ire(3) integer list regular expression. Each number in the list is a distinct token. ^ $ * + . {n,m} [N1 .. Nn] are supported, and - is equivalent to .*. Adjacent numbers may be separated by space, comma, / or _; multiple adjacent separators are ignored in the match. For example, '[!1 100]' matches all lists that contain neither 1 nor 100, and '^[!1 100]-701$' matches all lists that don't start with 1 or 100 and end with 701.",
	CXH,
	aspath_match_comp,
	match_list_exec,
	match_list_free
};

static Cxmatch_t	match_cluster =
{
	"cluster-re",
	"Matches on this type treat a string pattern as an ire(3) integer list regular expression. Each number in the list is a distinct token. ^ $ * + . {n,m} [N1 .. Nn] are supported, and - is equivalent to .*. Adjacent numbers may be separated by space, comma, / or _; multiple adjacent separators are ignored in the match. For example, '[!1 100]' matches all lists that contain neither 1 nor 100, and '^[!1 100]-701$' matches all lists that don't start with 1 or 100 and end with 701.",
	CXH,
	cluster_match_comp,
	match_list_exec,
	match_list_free
};

static Cxmatch_t	match_community =
{
	"community-re",
	"Matches on this type treat a string pattern as an ire(3) integer list regular expression. Each number in the list is a distinct token. Pairs are separated by :. ^ $ * + . {n,m} [N1 .. Nn] are supported, and - is equivalent to .*. Adjacent numbers may be separated by space, comma, / or _; multiple adjacent separators are ignored in the match. If a : tuple separator is omitted then :.* is assumed. For example, '[!1 100]' matches all lists that contain neither 1 nor 100 as the first pair element, and '^[!1: :100]-701:999$' matches all lists that don't start with 1 as the first pair element or 100 as the second pair element and end with 701:999.",
	CXH,
	community_match_comp,
	match_list_exec,
	match_list_free
};

typedef struct Matchdisc_s
{
	Ptdisc_t	ptdisc;
	int		prefix;
} Matchdisc_t;

static void*
match_prefix_comp(Cx_t* cx, Cxtype_t* sub, Cxtype_t* pat, Cxvalue_t* val, Cxdisc_t* disc)
{
	Pt_t*		pt;
	Dssmeth_t*	meth;
	Dss_t*		dss;
	Dssfile_t*	ip;
	Bgproute_t*	rp;
	char*		s;
	char*		t;
	Ptaddr_t	addr;
	unsigned char	bits;
	Matchdisc_t*	matchdisc;

	if (!cxisstring(pat) && pat->externalf != ipprefix_external)
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, 2, "%s: match requires %s or ipprefix_t pattern", sub->name, cx->state->type_string->name);
		return 0;
	}
	if (!(matchdisc = newof(0, Matchdisc_t, 1, 0)))
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, ERROR_SYSTEM|2, "out of space");
		return 0;
	}
	ptinit(&matchdisc->ptdisc);
	matchdisc->ptdisc.errorf = disc->errorf;
	matchdisc->prefix = sub->externalf == ipprefix_external;
	if (!(pt = ptopen(&matchdisc->ptdisc)))
		return 0;
	if (!cxisstring(pat))
	{
		addr = PREFIX_ADDR(val->number);
		bits = PREFIX_BITS(val->number);
		if (ptinsert(pt, PTMIN(addr, bits), PTMAX(addr, bits)))
		{
			ptclose(pt);
			return 0;
		}
	}
	else if (*(s = val->string.data) != '<')
	{
		while (!strtoip4(s, &t, &addr, &bits))
		{
			if (ptinsert(pt, PTMIN(addr, bits), PTMAX(addr, bits)))
			{
				ptclose(pt);
				return 0;
			}
			s = t;
		}
	}
	else if ((meth = dssmeth("bgp", disc)) && (dss = dssopen(0, 0, disc, meth)))
	{
		if (ip = dssfopen(dss, s + 1, NiL, DSS_FILE_READ, NiL))
		{
			while (rp = (Bgproute_t*)dssfread(ip))
				if (ptinsert(pt, PTMIN(rp->addr, rp->bits), PTMAX(rp->addr, rp->bits)))
				{
					dssfclose(ip);
					ptclose(pt);
					return 0;
				}
			dssfclose(ip);
		}
		dssclose(dss);
	}
	return pt;
}

static int
match_prefix_exec(Cx_t* cx, void* data, Cxvalue_t* val, Cxdisc_t* disc)
{
	Pt_t*		pt = (Pt_t*)data;
	Ptprefix_t*	prefix;
	Ptaddr_t	addr;
	int		bits;

	if (((Matchdisc_t*)pt->disc)->prefix)
	{
		addr = PREFIX_ADDR(val->number);
		bits = PREFIX_BITS(val->number);
		prefix = ptmatch(pt, addr);
		return (prefix != 0) && prefix->min <= PTMIN(addr, bits) && prefix->max >= PTMAX(addr, bits);
	}
	return ptmatch(pt, (Ptaddr_t)val->number) != 0;
}

static int
match_prefix_free(Cx_t* cx, void* data, Cxdisc_t* disc)
{
	Pt_t*		pt = (Pt_t*)data;
	Ptdisc_t*	ptdisc;
	int		i;

	if (data)
	{
		ptdisc = pt->disc;
		i = ptclose(pt);
		free(ptdisc);
		return i;
	}
	return 0;
}

static Cxmatch_t	match_prefix =
{
	"prefix-match",
	"Matches on this type treat a string pattern as a prefix table and test whether the subject is matched by the table. If the first character of the pattern is \b<\b then the remainder of the string is the path name of a file containing a prefix table. If the pattern is an \bipprefix_t\b then matches test if the subject is matched by the prefix.",
	CXH,
	match_prefix_comp,
	match_prefix_exec,
	match_prefix_free
};

static int
op_match_NP(Cx_t* cx, Cxinstruction_t* pc, Cxoperand_t* r, Cxoperand_t* a, Cxoperand_t* b, void* data, Cxdisc_t* disc)
{
	Ptaddr_t	aa;
	Ptaddr_t	ba;
	int		ab;
	int		bb;

	aa = PREFIX_ADDR(a->value.number);
	ab = PREFIX_BITS(a->value.number);
	ba = PREFIX_ADDR(b->value.number);
	bb = PREFIX_BITS(b->value.number);
	r->value.number = (PTMIN(aa,ab) >= PTMIN(ba,bb) && PTMAX(aa,ab) <= PTMAX(ba,bb)) == (pc->op == CX_MATCH);
	return 0;
}

static Cxcallout_t callouts[] =
{

CXC(CX_MATCH,	"number",	"ipprefix_t",	op_match_NP,	0)

{0}

};

static Cxtype_t types[] =
{
{ "aspath_t", "A sequence of as_t autonomous system numbers.", CXH, (Cxtype_t*)"buffer", 0, aspath_external, aspath_internal, 0, 0, { "The format details string is the format character (\b.\b: dotted quad, \bd\b: signed decimal, \bo\b: octal, \bx\b: hexadecimal, \bu\b: default unsigned decimal), followed by the separator string.", "u," }, &match_aspath },
{ "as_t", "An unsigned 16 bit autonomous system number.", CXH, (Cxtype_t*)"number", 0, 0, 0, 0, 0, { 0, 0, CX_UNSIGNED|CX_INTEGER, 2, 5 }, 0 },
{ "cluster_t", "A sequence of unsigned 32 bit integer cluster ids.", CXH, (Cxtype_t*)"buffer", 0, cluster_external, cluster_internal, 0, 0, { "The format details string is the format character (\b.\b: dotted quad, \bd\b: signed decimal, \bo\b: octal, \bx\b: hexadecimal, \bu\b: default unsigned decimal), followed by the separator string.", ".," }, &match_cluster },
{ "community_t", "A sequence of unsigned 16 bit integer pairs.", CXH, (Cxtype_t*)"buffer", 0, community_external, community_internal, 0, 0, { "The format details string is the format character (\b.\b: dotted quad, \bd\b: signed decimal, \bo\b: octal, \bx\b: hexadecimal, \bu\b: default unsigned decimal), followed by the separator string.", "u," }, &match_community },
{ "ipaddr_t",	"A dotted quad ipv4 address.", CXH, (Cxtype_t*)"number", 0, ipaddr_external, ipaddr_internal, 0, 0, { 0, 0, CX_UNSIGNED|CX_INTEGER, 4, 16 }, &match_prefix },
{ "ipprefix_t",	"/length appended to an ipaddr_t prefix.", CXH, (Cxtype_t*)"number", 0, ipprefix_external, ipprefix_internal, 0, 0, { "The format details string is a \bprintf\b(3) format specification for the integer arguments \aaddress,bits\a; e.g., \b%2$u|%1$08x\b prints the decimal bits followed by the hexadecimal prefix address.", 0, CX_UNSIGNED|CX_INTEGER, 8, 19 }, &match_prefix },
{0}
};

Dsslib_t dss_lib_ip_t =
{
	"ip_t",
	"IP type support",
	CXH,
	0,
	0,
	&types[0],
	&callouts[0],
};
