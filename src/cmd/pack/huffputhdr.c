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
 * huffman coding routine to write pack format file header
 *
 *   David Korn
 *   AT&T Bell Laboratories
 *   Room 3C-526B
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *   ulysses!dgk
 */

#include	"huffman.h"

/*
 * This routine outputs a pack format header to <outfile> and returns
 * the number of bytes in the header
 */

int huffputhdr(register Huff_t *hp,Sfio_t *outfile)
{
	register int		i, c;
	register Sfio_t	*fp = outfile;
	/* output magic number */
	sfputc(fp,HUFFMAG1);
	sfputc(fp,HUFFMAG2);
	/* output the length and the dictionary */
	sfputc(fp,hp->insize>>(3*CHAR_BIT));
	sfputc(fp,hp->insize>>(2*CHAR_BIT));
	sfputc(fp,hp->insize>>(CHAR_BIT));
	sfputc(fp,hp->insize);
	/* output number of levels and count for each level */
	sfputc(fp,hp->maxlev);
	for (i=1; i<hp->maxlev; i++)
		sfputc(fp,hp->levcount[i]);
	sfputc(fp,hp->levcount[hp->maxlev]-2);
	/* output the characters */
	for (i=1; i<=hp->maxlev; i++)
		for (c=0; c < (1<<CHAR_BIT); c++)
			if (hp->length[c] == i)
				sfputc(fp,c);
	return(huffhsize(hp));
}
