/*******************************************************************
*                                                                  *
*             This software is part of the ast package             *
*                Copyright (c) 1989-2002 AT&T Corp.                *
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
*                                                                  *
*******************************************************************/
/* : : generated by a yacc that works -- solaris take note : : */
#ifndef _EXPARSE_H
#define _EXPARSE_H

typedef union
#ifdef __cplusplus
	EXSTYPE
#endif

{
	struct Exnode_s*expr;
	double		floating;
	struct Exref_s*	reference;
	struct Exid_s*	id;
	Sflong_t	integer;
	int		op;
	char*		string;
	struct Exbuf_s*	buffer;
} EXSTYPE;
extern EXSTYPE exlval;
# define MINTOKEN 257
# define CHAR 258
# define INT 259
# define INTEGER 260
# define UNSIGNED 261
# define FLOATING 262
# define STRING 263
# define VOID 264
# define BREAK 265
# define CALL 266
# define CASE 267
# define CONSTANT 268
# define CONTINUE 269
# define DECLARE 270
# define DEFAULT 271
# define DYNAMIC 272
# define ELSE 273
# define EXIT 274
# define FOR 275
# define FUNCTION 276
# define ITERATE 277
# define ID 278
# define IF 279
# define LABEL 280
# define MEMBER 281
# define NAME 282
# define POS 283
# define PRAGMA 284
# define PRE 285
# define PRINTF 286
# define PROCEDURE 287
# define QUERY 288
# define RETURN 289
# define SPRINTF 290
# define SWITCH 291
# define WHILE 292
# define F2I 293
# define F2S 294
# define I2F 295
# define I2S 296
# define S2B 297
# define S2F 298
# define S2I 299
# define F2X 300
# define I2X 301
# define S2X 302
# define X2F 303
# define X2I 304
# define X2S 305
# define OR 306
# define AND 307
# define EQ 308
# define NE 309
# define LE 310
# define GE 311
# define LS 312
# define RS 313
# define UNARY 314
# define INC 315
# define DEC 316
# define CAST 317
# define MAXTOKEN 318
#endif /* _EXPARSE_H */
