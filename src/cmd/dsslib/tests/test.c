/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*           Copyright (c) 2003-2007 AT&T Knowledge Ventures            *
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

#include <dsslib.h>

typedef struct State_s
{
	Cxunsigned_t	selected;
} State_t;

static const char even_usage[] =
"[-1?\n@(#)$Id: dss even test query (AT&T Research) 2003-09-22 $\n]"
USAGE_LICENSE
"[+LIBRARY?\findex\f]"
"[+DESCRIPTION?The \beven\b test query selects even ordinal records.]"
"\n"
"\n[ file ... ]\n"
"\n"
;

static int
even_beg(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	State_t*	state;
	int		errors;

	errors = error_info.errors;
	if (!(state = vmnewof(cx->vm, 0, State_t, 1, 0)))
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, ERROR_SYSTEM|2, "out of space");
		return -1;
	}
	expr->data = state;
	for (;;)
	{
		switch (optget((char**)data, even_usage))
		{
		case '?':
			if (disc->errorf)
				(*disc->errorf)(NiL, disc, ERROR_USAGE|4, "%s", opt_info.arg);
			else
				return -1;
			continue;
		case ':':
			if (disc->errorf)
				(*disc->errorf)(NiL, disc, 2, "%s", opt_info.arg);
			else
				return -1;
			continue;
		}
		break;
	}
	if (error_info.errors > errors)
		return -1;
	sfprintf(sfstdout, "even_beg\n");
	return 0;
}

static int
even_sel(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	State_t*	state = (State_t*)expr->data;

	if (!(expr->queried & 1))
	{
		state->selected++;
		sfprintf(sfstdout, "even_sel %I*u\n", sizeof(expr->queried), expr->queried);
		return 1;
	}
	return 0;
}

static int
even_act(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	sfprintf(sfstdout, "even_act %I*u\n", sizeof(expr->queried), expr->queried);
	return 0;
}

static int
even_end(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	State_t*	state = (State_t*)expr->data;

	sfprintf(sfstdout, "even_end %I*u %I*u %I*u%s\n", sizeof(expr->queried), expr->queried, sizeof(expr->selected), expr->selected, sizeof(state->selected), state->selected, expr->selected == state->selected ? "" : " FAILED");
	return 0;
}

static const char odd_usage[] =
"[-1?\n@(#)$Id: dss odd test query (AT&T Research) 2003-09-22 $\n]"
USAGE_LICENSE
"[+LIBRARY?\findex\f]"
"[+DESCRIPTION?The \bodd\b test query selects odd ordinal records.]"
"\n"
"\n[ file ... ]\n"
"\n"
;

static int
odd_beg(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	State_t*	state;
	int		errors;

	errors = error_info.errors;
	if (!(state = vmnewof(cx->vm, 0, State_t, 1, 0)))
	{
		if (disc->errorf)
			(*disc->errorf)(NiL, disc, ERROR_SYSTEM|2, "out of space");
		return -1;
	}
	expr->data = state;
	for (;;)
	{
		switch (optget((char**)data, odd_usage))
		{
		case '?':
			if (disc->errorf)
				(*disc->errorf)(NiL, disc, ERROR_USAGE|4, "%s", opt_info.arg);
			else
				return -1;
			continue;
		case ':':
			if (disc->errorf)
				(*disc->errorf)(NiL, disc, 2, "%s", opt_info.arg);
			else
				return -1;
			continue;
		}
		break;
	}
	if (error_info.errors > errors)
		return -1;
	sfprintf(sfstdout, "odd_beg\n");
	return 0;
}

static int
odd_sel(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	State_t*	state = (State_t*)expr->data;

	if (expr->queried & 1)
	{
		state->selected++;
		sfprintf(sfstdout, "odd_sel %I*u\n", sizeof(expr->queried), expr->queried);
		return 1;
	}
	return 0;
}

static int
odd_act(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	sfprintf(sfstdout, "odd_act %I*u\n", sizeof(expr->queried), expr->queried);
	return 0;
}

static int
odd_end(Cx_t* cx, Cxexpr_t* expr, void* data, Cxdisc_t* disc)
{
	State_t*	state = (State_t*)expr->data;

	sfprintf(sfstdout, "odd_end %I*u %I*u %I*u%s\n", sizeof(expr->queried), expr->queried, sizeof(expr->selected), expr->selected, sizeof(state->selected), state->selected, expr->selected == state->selected ? "" : " FAILED");
	return 0;
}

static Cxquery_t	queries[] =
{
	{
		"even",
		"Select even ordinal records.",
		CXH,
		even_beg,
		even_sel,
		even_act,
		even_end,
	},
	{
		"odd",
		"Select odd ordinal records.",
		CXH,
		odd_beg,
		odd_sel,
		odd_act,
		odd_end,
	},
	{0}
};

static Dsslib_t		lib =
{
	"test",
	"test queries",
	CXH,
	0,
	0,
	0,
	0,
	0,
	0,
	&queries[0]
};

Dsslib_t*
dss_lib(const char* name, Dssdisc_t* disc)
{
	return &lib;
}
