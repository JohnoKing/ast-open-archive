/***************************************************************
*                                                              *
*           This software is part of the ast package           *
*              Copyright (c) 1993-2000 AT&T Corp.              *
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
*              David Korn <dgk@research.att.com>               *
*                                                              *
***************************************************************/
#pragma prototyped
/*
 * huffman coding routine to read a pack format header
 *
 *   David Korn
 *   AT&T Bell Laboratories
 *   Room 3C-526B
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *   ulysses!dgk
 */

#include	"huffman.h"
#include	<error.h>

#define END	(1<<CHAR_BIT)

Huff_t *huffgethdr(register Sfio_t *infile)
{
	register Huff_t*	hp;
	register int		i, j, c;
	/* allocate space for huffman tree */
	if(!(hp=newof(0, Huff_t, 1, 0)))
	{
		errno = ENOMEM;
		return((Huff_t*)0);
	}
	/* check two-byte header */
	if(sfgetc(infile)!=HUFFMAG1 || sfgetc(infile)!=HUFFMAG2)
		goto error;
	/* get size of original file, */
	for (i=0; i<4; i++)
		hp->insize = (hp->insize<<CHAR_BIT)+ sfgetc(infile);
	/* get number of levels in maxlev, */
	hp->maxlev = sfgetc(infile);
	if(hp->maxlev < 0 || hp->maxlev > HUFFLEV)
		goto error;
	/* get number of leaves on level i  */
	for (i=1; i<=hp->maxlev; i++)
	{
		if((c=sfgetc(infile)) < 0)
			goto error;
		hp->levcount[i] = c;
	}
	/* read in the characters and compute number of bits for each */
	for(i=0; i <= END; i++)
		hp->length[i] = 0;
	hp->nchars = 0;
	for (i=1; i<=hp->maxlev; i++)
	{
		j = hp->levcount[i];
		if((hp->nchars += j) >=END)
			goto error;
		while (j-- > 0)
		{
			if((c=sfgetc(infile)) < 0)
				goto error;
			hp->length[c] = i;
		}
	}
	if((c=sfgetc(infile)) < 0)
		goto error;
	hp->length[c] = i-1;
	return(hp);

error:
	huffend(hp);
	return ((Huff_t*)0);
}
