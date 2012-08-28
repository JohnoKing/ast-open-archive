/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1987-2010 AT&T Intellectual Property          *
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
#pragma prototyped

#include "format.h"

/*
 * pax delta_88 format
 */

static Delta_format_t	data = { "88" };

Format_t	pax_delta_88_format =
{
	"delta88",
	"odelta",
	"delta88 difference/compression",
	DELTA_88,
	DELTA|IN,
	0,
	0,
	0,
	PAXNEXT(delta_88),
};

PAXLIB(delta_88)
