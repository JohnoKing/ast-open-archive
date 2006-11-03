/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*           Copyright (c) 1982-2006 AT&T Knowledge Ventures            *
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
*                  David Korn <dgk@research.att.com>                   *
*                                                                      *
***********************************************************************/
#pragma prototyped
/*
 * Shell macro expander
 * expands ~
 * expands ${...}
 * expands $(...)
 * expands $((...))
 * expands `...`
 *
 *   David Korn
 *   AT&T Labs
 *
 */

#include	"defs.h"
#include	<fcin.h>
#include	<pwd.h>
#include	"name.h"
#include	"variables.h"
#include	"shlex.h"
#include	"io.h"
#include	"shnodes.h"
#include	"path.h"
#include	"national.h"
#include	"streval.h"

#undef STR_GROUP
#ifndef STR_GROUP
#   define STR_GROUP	0
#endif

static int	_c_;
typedef struct  _mac_
{
	Shell_t		*shp;		/* pointer to shell interpreter */
	Sfio_t		*sp;		/* stream pointer for here-document */
	struct argnod	**arghead;	/* address of head of argument list */
	char		*ifsp;		/* pointer to IFS value */
	int		fields;		/* number of fields */
	short		quoted;		/* set when word has quotes */
	unsigned char	ifs;		/* first char of IFS */
	char		quote;		/* set within double quoted contexts */
	char		lit;		/* set within single quotes */
	char		split;		/* set when word splittin is possible */
	char		pattern;	/* set when file expansion follows */
	char		patfound;	/* set if pattern character found */
	char		assign;		/* set for assignments */
	char		arith;		/* set for ((...)) */
	char		let;		/* set when expanding let arguments */
	char		zeros;		/* strip leading zeros when set */
	void		*nvwalk;	/* for name space walking*/
} Mac_t;

#define mac	(*((Mac_t*)(sh.mac_context)))

#undef ESCAPE
#define ESCAPE		'\\'
#define isescchar(s)	((s)>S_QUOTE)
#define isqescchar(s)	((s)>=S_QUOTE)
#define isbracechar(c)	((c)==RBRACE || (_c_=sh_lexstates[ST_BRACE][c])==S_MOD1 ||_c_==S_MOD2)
#define ltos(x)		fmtbase((long)(x),0,0)

/* type of macro expansions */
#define M_BRACE		1	/* ${var}	*/
#define M_TREE		2	/* ${var.}	*/
#define M_SIZE		3	/* ${#var}	*/
#define M_VNAME		4	/* ${!var}	*/
#define M_SUBNAME	5	/* ${!var[sub]}	*/
#define M_NAMESCAN	6	/* ${!var*}	*/
#define M_NAMECOUNT	7	/* ${#var*}	*/
#define M_TYPE		8	/* ${@var}	*/

static int	substring(const char*, const char*, int[], int);
static void	copyto(Mac_t*, int, int);
static void	comsubst(Mac_t*,int);
static int	varsub(Mac_t*);
static void	mac_copy(Mac_t*,const char*, int);
static void	tilde_expand2(int);
static char 	*sh_tilde(const char*);
static char	*special(int);
static void	endfield(Mac_t*,int);
static void	mac_error(Namval_t*);
static char	*mac_getstring(char*);
static int	charlen(const char*,int);
#if SHOPT_MULTIBYTE
    static char	*lastchar(const char*,const char*);
#endif /* SHOPT_MULTIBYTE */

void *sh_macopen(Shell_t *shp)
{
	void *addr = newof(0,Mac_t,1,0);
	Mac_t *mp = (Mac_t*)addr;
	mp->shp = shp;
	return(addr);
}

/*
 * perform only parameter substitution and catch failures
 */
char *sh_mactry(register char *string)
{
	if(string)
	{
		int		jmp_val;
		int		savexit = sh.savexit;
		struct checkpt	buff;
		sh_pushcontext(&buff,SH_JMPSUB);
		jmp_val = sigsetjmp(buff.buff,0);
		if(jmp_val == 0)
			string = sh_mactrim(string,0);
		sh_popcontext(&buff);
		sh.savexit = savexit;
		return(string);
	}
	return("");
}

/*
 * Perform parameter expansion, command substitution, and arithmetic
 * expansion on <str>. 
 * If <mode> greater than 1 file expansion is performed if the result 
 * yields a single pathname.
 * If <mode> negative, than expansion rules for assignment are applied.
 */
char *sh_mactrim(char *str, register int mode)
{
	register Mac_t *mp = (Mac_t*)sh.mac_context;
	Mac_t	savemac;
	savemac = *mp;
	stakseek(0);
	mp->arith = (mode==3);
	mp->let = 0;
	sh.argaddr = 0;
	mp->pattern = (mode==1||mode==2);
	mp->patfound = 0;
	mp->assign = (mode<0);
	mp->quoted = mp->lit = mp->split = mp->quote = 0;
	mp->sp = 0;
	if(mp->ifsp=nv_getval(nv_scoped(IFSNOD)))
		mp->ifs = *mp->ifsp;
	else
		mp->ifs = ' ';
	stakseek(0);
	fcsopen(str);
	copyto(mp,0,mp->arith);
	str = stakfreeze(1);
	if(mode==2)
	{
		/* expand only if unique */
		struct argnod *arglist=0;
		if((mode=path_expand(str,&arglist))==1)
			str = arglist->argval;
		else if(mode>1)
			errormsg(SH_DICT,ERROR_exit(1),e_ambiguous,str);
		sh_trim(str);
	}
	*mp = savemac;
	return(str);
}

/*
 * Perform all the expansions on the argument <argp>
 */
int sh_macexpand(register struct argnod *argp, struct argnod **arghead,int flag)
{
	register int flags = argp->argflag;
	register char *str = argp->argval;
	register Mac_t  *mp = (Mac_t*)sh.mac_context;
	char **saveargaddr = sh.argaddr;
	Mac_t savemac;
	savemac = *mp;
	mp->sp = 0;
	if(mp->ifsp=nv_getval(nv_scoped(IFSNOD)))
		mp->ifs = *mp->ifsp;
	else
		mp->ifs = ' ';
	if(flag&ARG_OPTIMIZE)
		sh.argaddr = (char**)&argp->argchn.ap;
	else
		sh.argaddr = 0;
	mp->arghead = arghead;
	mp->quoted = mp->lit = mp->quote = 0;
	mp->arith = ((flag&ARG_ARITH)!=0);
	mp->let = ((flag&ARG_LET)!=0);
	mp->split = !(flag&ARG_ASSIGN);
	mp->assign = !mp->split;
	mp->pattern = mp->split && !(flag&ARG_NOGLOB) && !sh_isoption(SH_NOGLOB);
	str = argp->argval;
	fcsopen(str);
	mp->fields = 0;
	if(!arghead)
	{
		mp->split = 0;
		mp->pattern = ((flag&ARG_EXP)!=0);
		stakseek(0);
	}
	else
	{
		stakseek(ARGVAL);
		*stakptr(ARGVAL-1) = 0;
	}
	mp->patfound = 0;
	copyto(mp,0,mp->arith);
	if(!arghead)
	{
		argp->argchn.cp = stakfreeze(1);
		if(sh.argaddr)
			argp->argflag |= ARG_MAKE;
	}
	else
	{
		endfield(mp,mp->quoted);
		flags = mp->fields;
		if(flags==1 && sh.argaddr)
			argp->argchn.ap = *arghead; 
	}
	sh.argaddr = saveargaddr;
	*mp = savemac;
	return(flags);
}

/*
 * Expand here document which is stored in <infile> or <string>
 * The result is written to <outfile>
 */
void sh_machere(Sfio_t *infile, Sfio_t *outfile, char *string)
{
	register int	c,n;
	register const char	*state = sh_lexstates[ST_QUOTE];
	register char	*cp;
	register Mac_t	*mp = (Mac_t*)sh.mac_context;
	Fcin_t		save;
	Mac_t		savemac;
	savemac = *mp;
	stakseek(0);
	sh.argaddr = 0;
	mp->sp = outfile;
	mp->split = mp->assign = mp->pattern = mp->patfound = mp->lit = mp->arith = mp->let = 0;
	mp->quote = 1;
	mp->ifsp = nv_getval(nv_scoped(IFSNOD));
	mp->ifs = ' ';
	fcsave(&save);
	if(infile)
		fcfopen(infile);
	else
		fcsopen(string);
	fcnotify(0);
	cp = fcseek(0);
	while(1)
	{
#if SHOPT_MULTIBYTE
		if(mbwide())
		{
			do
			{
				ssize_t len;
				switch(len = mbsize(cp))
				{
				    case -1:	/* illegal multi-byte char */
				    case 0:
				    case 1:
					n=state[*(unsigned char*)cp++];
					break;
				    default:
					/* use state of alpah character */
					n=state['a'];
					cp += len;
				}
			}
			while(n == 0);
		}
		else
#endif /* SHOPT_MULTIBYTE */
		while((n=state[*(unsigned char*)cp++])==0);
		if(n==S_NL || n==S_QUOTE || n==S_RBRA)
			continue;
		if(c=(cp-1)-fcseek(0))
			sfwrite(outfile,fcseek(0),c);
		cp = fcseek(c+1);
		switch(n)
		{
		    case S_EOF:
			if((n=fcfill()) <=0)
			{
				/* ignore 0 byte when reading from file */
				if(n==0 && fcfile())
					continue;
				fcrestore(&save);
				*mp = savemac;
				return;
			}
			cp = fcseek(-1);
			continue;
		    case S_ESC:
			fcgetc(c);
			cp=fcseek(-1);
			if(c>0)
				cp++;
			if(!isescchar(state[c]))
				sfputc(outfile,ESCAPE);
			continue;
		    case S_GRAVE:
			comsubst(mp,0);
			break;
		    case S_DOL:
			c = fcget();
			if(c=='.')
				goto regular;
		    again:
			switch(n=sh_lexstates[ST_DOL][c])
			{
			    case S_ALP: case S_SPC1: case S_SPC2:
			    case S_DIG: case S_LBRA:
			    {
				Fcin_t	save2;
				int	offset = staktell();
				int	offset2;
				stakputc(c);
				if(n==S_LBRA)
					sh_lexskip(RBRACE,1,ST_BRACE);
				else if(n==S_ALP)
				{
					while(fcgetc(c),isaname(c))
						stakputc(c);
					fcseek(-1);
				}
				stakputc(0);
				offset2 = staktell();
				fcsave(&save2);
				fcsopen(stakptr(offset));
				varsub(mp);
				if(c=staktell()-offset2)
					sfwrite(outfile,(char*)stakptr(offset2),c);
				fcrestore(&save2);
				stakseek(offset);
				break;
			    }
			    case S_PAR:
				comsubst(mp,1);
				break;
			    case S_EOF:
				if((c=fcfill()) > 0)
					goto again;
				/* FALL THRU */
			    default:
			    regular:
				sfputc(outfile,'$');
				fcseek(-1);
				break;
			}
		}
		cp = fcseek(0);
	}
}

/*
 * expand argument but do not trim pattern characters
 */
char *sh_macpat(register struct argnod *arg, int flags)
{
	register char *sp = arg->argval;
	if((arg->argflag&ARG_RAW))
		return(sp);
	if(flags&ARG_OPTIMIZE)
		arg->argchn.ap=0;
	if(!(sp=arg->argchn.cp))
	{
		sh_macexpand(arg,NIL(struct argnod**),flags);
		sp = arg->argchn.cp;
		if(!(flags&ARG_OPTIMIZE) || !(arg->argflag&ARG_MAKE))
			arg->argchn.cp = 0;
		arg->argflag &= ~ARG_MAKE;
	}
	else
		sh.optcount++;
	return(sp);
}

/*
 * Process the characters up to <endch> or end of input string 
 */
static void copyto(register Mac_t *mp,int endch, int newquote)
{
	register int	c,n;
	register const char	*state = sh_lexstates[ST_MACRO];
	register char	*cp,*first;
	int		tilde = -1;
	int		oldquote = mp->quote;
	int		ansi_c = 0;
	int		paren = 0;
	int		brace = 0;
	Sfio_t		*sp = mp->sp;
	mp->sp = NIL(Sfio_t*);
	mp->quote = newquote;
	first = cp = fcseek(0);
	if(!mp->quote && *cp=='~')
		tilde = staktell();
	/* handle // operator specially */
	if(mp->pattern==2 && *cp=='/')
		cp++;
	while(1)
	{
#if SHOPT_MULTIBYTE
		if(mbwide())
		{
			ssize_t len;
			do
			{
				switch(len = mbsize(cp))
				{
				    case -1:	/* illegal multi-byte char */
				    case 0:
					len = 1;
				    case 1:
					n = state[*(unsigned char*)cp++];
					break;
				    default:
					/* treat as if alpha */
					cp += len;
					n=state['a'];
				}
			}
			while(n == 0);
			c = (cp-len) - first;
		}
		else
#endif /* SHOPT_MULTIBYTE */
		{
			while((n=state[*(unsigned char*)cp++])==0);
			c = (cp-1) - first;
		}
		switch(n)
		{
		    case S_ESC:
			if(ansi_c)
			{
				/* process ANSI-C escape character */
				char *addr= --cp;
				if(c)
					stakwrite(first,c);
				c = chresc(cp,&addr);
				cp = addr;
				first = fcseek(cp-first);
#if SHOPT_MULTIBYTE
				if(c > UCHAR_MAX && mbwide())
				{
					int		i;
					unsigned char	mb[8];

					n = wctomb((char*)mb, c);
					for(i=0;i<n;i++)
						stakputc(mb[i]);
				}
				else
#endif /* SHOPT_MULTIBYTE */
				stakputc(c);
				if(c==ESCAPE && mp->pattern)
					stakputc(ESCAPE);
				break;
			}
			else if(sh_isoption(SH_BRACEEXPAND) && mp->pattern==4 && (*cp==',' || *cp==LBRACE || *cp==RBRACE || *cp=='.'))
				break;
			else if(mp->split && endch && !mp->quote && !mp->lit)
			{
				if(c)
					mac_copy(mp,first,c);
				cp = fcseek(c+2);
				if(c= cp[-1])
				{
					stakputc(c);
					if(c==ESCAPE)
						stakputc(ESCAPE);
				}
				else
					cp--;
				first = cp;
				break;
			}
			n = state[*(unsigned char*)cp];
			if(n==S_ENDCH && *cp!=endch)
				n = S_PAT;
			if(mp->pattern)
			{
				/* preserve \digit for pattern matching */
				/* also \alpha for extended patterns */
				if(!mp->lit && !mp->quote && (n==S_DIG || (paren && sh_lexstates[ST_DOL][*(unsigned char*)cp]==S_ALP)))
					break;
				/* followed by file expansion */
				if(!mp->lit && (n==S_ESC || (!mp->quote && 
					(n==S_PAT||n==S_ENDCH||n==S_SLASH||n==S_BRACT||*cp=='-'))))
				{
					cp += (n!=S_EOF);
					break;
				}
				if(mp->lit || (mp->quote && !isqescchar(n) && n!=S_ENDCH))
				{
					/* add \ for file expansion */
					stakwrite(first,c+1);
					first = fcseek(c);
					break;
				}
			}
			if(mp->lit)
				break;
			if(!mp->quote || isqescchar(n) || n==S_ENDCH)
			{
				/* eliminate \ */
				if(c)
					stakwrite(first,c);
				/* check new-line joining */
				first = fcseek(c+1);
			}
			cp += (n!=S_EOF);
			break;
		    case S_GRAVE: case S_DOL:
			if(mp->lit)
				break;
			if(c)
			{
				if(mp->split && !mp->quote && endch)
					mac_copy(mp,first,c);
				else
					stakwrite(first,c);
			}
			first = fcseek(c+1);
			c = mp->pattern;
			if(n==S_GRAVE)
				comsubst(mp,0);
			else if((n= *cp)==0 || !varsub(mp))
			{
				if(n=='\'' && !mp->quote)
					ansi_c = 1;
				else if(mp->quote || n!='"')
					stakputc('$');
			}
			cp = first = fcseek(0);
			if(*cp)
				mp->pattern = c;
			break;
		    case S_ENDCH:
			if((mp->lit || cp[-1]!=endch || mp->quote!=newquote))
				goto pattern;
			if(endch==RBRACE && *cp==LPAREN && mp->pattern && brace)
				goto pattern;
		    case S_EOF:
			if(c)
			{
				if(mp->split && !mp->quote && !mp->lit && endch)
					mac_copy(mp,first,c);
				else
					stakwrite(first,c);
			}
			c += (n!=S_EOF);
			first = fcseek(c);
			if(tilde>=0)
				tilde_expand2(tilde);
			goto done;
		    case S_QUOTE:
			if(mp->lit || mp->arith)
				break;
		    case S_LIT:
			if(mp->arith)
			{
				if((*cp=='`' || *cp=='[') && cp[1]=='\'')
					cp +=2;
				break;
			}
			if(n==S_LIT && mp->quote)
				break;
			if(c)
			{
				if(mp->split && endch && !mp->quote && !mp->lit)
					mac_copy(mp,first,c);
				else
					stakwrite(first,c);
			}
			first = fcseek(c+1);
			if(n==S_LIT)
			{
				if(mp->quote)
					continue;
				if(mp->lit)
					mp->lit = ansi_c = 0;
				else
					mp->lit = 1;
			}
			else
				mp->quote = !mp->quote;
			mp->quoted++;
			break;
		    case S_BRACT:
			if(mp->arith || ((mp->assign==1 || endch==RBRACT) &&
				!(mp->quote || mp->lit)))
			{
				int offset=0,oldpat = mp->pattern;
				int oldarith = mp->arith;
				stakwrite(first,++c);
				if(mp->assign==1 && first[c-2]=='.')
					offset = staktell();
				first = fcseek(c);
				mp->pattern = 4;
				mp->arith = 0;
				copyto(mp,RBRACT,0);
				mp->arith = oldarith;
				mp->pattern = oldpat;
				stakputc(RBRACT);
				if(offset)
				{
					cp = stakptr(staktell());
					if(sh_checkid(stakptr(offset),cp)!=cp)
						stakseek(staktell()-2);
				}
				cp = first = fcseek(0);
				break;
			}
		    case S_PAT:
			if(mp->pattern && !(mp->quote || mp->lit))
			{
				mp->patfound = mp->pattern;
				if((n=cp[-1])==LPAREN)
					paren++;
				else if(n==RPAREN)
					--paren;
			}
			goto pattern;
		    case S_BRACE:
			if(!(mp->quote || mp->lit))
			{
				mp->patfound = mp->split && sh_isoption(SH_BRACEEXPAND);
				brace = 1;
			}
		    pattern:
			if(!mp->pattern || !(mp->quote || mp->lit))
			{
				/* mark beginning of {a,b} */
				if(n==S_BRACE && endch==0 && mp->pattern)
					mp->pattern=4;
				if(n==S_SLASH && mp->pattern==2)
					mp->pattern=3;
				break;
			}
			if(mp->pattern==3)
				break;
			if(c)
				stakwrite(first,c);
			first = fcseek(c);
			stakputc(ESCAPE);
			break;
		    case S_EQ:
			if(mp->assign==1)
			{
				if(*cp=='~' && !endch && !mp->quote && !mp->lit)
					tilde = staktell()+(c+1);
				mp->assign = 2;
			}
			break;
		    case S_SLASH:
		    case S_COLON:
			if(tilde >=0)
			{
				if(c)
					stakwrite(first,c);
				first = fcseek(c);
				tilde_expand2(tilde);
				tilde = -1;
				c=0;
			}
			if(n==S_COLON && mp->assign==2 && *cp=='~' && endch==0 && !mp->quote &&!mp->lit)
				tilde = staktell()+(c+1);
			else if(n==S_SLASH && mp->pattern==2)
#if 0
				goto pattern;
#else
			{
				if(mp->quote || mp->lit)
					goto pattern;
				stakwrite(first,c+1);
				first = fcseek(c+1);
				c = staktell();
				sh_lexskip(RBRACE,0,ST_NESTED);
				stakseek(c);
				cp = fcseek(-1);
				stakwrite(first,cp-first);
				first=cp;
			}
#endif
			break;
		}
	}
done:
	mp->sp = sp;
	mp->quote = oldquote;
}

/*
 * copy <str> to stack performing sub-expression substitutions
 */
static void mac_substitute(Mac_t *mp, register char *cp,char *str,register int subexp[],int subsize)
{
	register int c,n;
#if 0
	register char *first=cp;
#else
	register char *first=fcseek(0);
	char *ptr;
	Mac_t savemac;
	n = staktell();
	savemac = *mp;
	mp->pattern = 3;
	mp->split = 0;
	fcsopen(cp);
	copyto(mp,0,0);
	stakputc(0);
	ptr = cp = strdup(stakptr(n));
	stakseek(n);
	*mp = savemac;
	fcsopen(first);
	first = cp;
#endif
	while(1)
	{
		while((c= *cp++) && c!=ESCAPE);
		if(c==0)
			break;
		if((n= *cp++)=='\\' || n==RBRACE || (n>='0' && n<='9' && (n-='0')<subsize))
		{
			c = cp-first-2;
			if(c)
				mac_copy(mp,first,c);
			first=cp;
			if(n=='\\' || n==RBRACE)
			{
				first--;
				continue;
			}
			if((c=subexp[2*n])>=0)
			{
				if((n=subexp[2*n+1]-c)>0)
					mac_copy(mp,str+c,n);
			}
		}
		else if(n==0)
			break;
	}
	if(n=cp-first-1)
		mac_copy(mp,first,n);
#if 1
	free(ptr);
#endif
}

#if  SHOPT_FILESCAN
#define	MAX_OFFSETS	 (sizeof(shp->offsets)/sizeof(shp->offsets[0]))
#define MAX_ARGN	(32*1024)

/*
 * compute the arguments $1 ... $n and $# from the current line as needed
 * save line offsets in the offsets array.
 */
static char *getdolarg(Shell_t *shp, int n, int *size)
{
	register int c=S_DELIM, d=shp->ifstable['\\'];
	register unsigned char *first,*last,*cp = (unsigned char*)shp->cur_line;
	register int m=shp->offsets[0],delim=0;
	if(m==0)
		return(0);
	if(m<0)
		m = 0;
	else if(n<=m)
		m = n-1;
	else
		m--;
	if(m >= MAX_OFFSETS-1)
		m =  MAX_OFFSETS-2;
	cp += shp->offsets[m+1];
	n -= m;
	shp->ifstable['\\'] = 0;
	shp->ifstable[0] = S_EOF;
	while(1)
	{
		if(c==S_DELIM)
			while(shp->ifstable[*cp++]==S_SPACE);
		first = --cp;
		if(++m < MAX_OFFSETS)
			shp->offsets[m] = (first-(unsigned char*)shp->cur_line);
		while((c=shp->ifstable[*cp++])==0);
		last = cp-1;
		if(c==S_SPACE)
			while((c=shp->ifstable[*cp++])==S_SPACE);
		if(--n==0 || c==S_EOF)
		{
			if(last==first && c==S_EOF && (!delim || (m>1)))
			{
				n++;
				m--;
			}
			break;
		}
		delim = (c==S_DELIM);
	}
	shp->ifstable['\\'] = d;
	if(m > shp->offsets[0])
		shp->offsets[0] = m;
	if(n)
		first = last = 0;
	if(size)
		*size = last-first;
	return((char*)first);
}
#endif /* SHOPT_FILESCAN */

/*
 * get the prefix after name reference resolution
 */
static char *prefix(char *id)
{
	Namval_t *np;
	register char *cp = strchr(id,'.');
	if(cp)
	{
		*cp = 0;
		np = nv_search(id, sh.var_tree,0);
		*cp = '.';
		if(isastchar(cp[1]))
			cp[1] = 0;
		if(np && nv_isref(np))
		{
			int n;
			char *sp;
			sh.argaddr = 0;
			while(nv_isref(np))
				np = nv_refnode(np);
			id = (char*)malloc(strlen(cp)+1+(n=strlen(sp=nv_name(np)))+1);
			strcpy(&id[n],cp);
			memcpy(id,sp,n);
			return(id);
		}
	}
	return(strdup(id));
}

/*
 * copy to ']' onto the stack and return offset to it
 */
static int subcopy(Mac_t *mp, int flag)
{
	int split = mp->split;
	int xpattern = mp->pattern;
	int loc = staktell();
	int xarith = mp->arith;
	mp->split = 0;
	mp->arith = 0;
	mp->pattern = flag?4:0;
	copyto(mp,RBRACT,0);
	mp->pattern = xpattern;
	mp->split = split;
	mp->arith = xarith;
	return(loc);
}

static int namecount(Mac_t *mp,const char *prefix)
{
	int count = 0;
	mp->nvwalk = nv_diropen(prefix);
	while(nv_dirnext(mp->nvwalk))
		count++;
	nv_dirclose(mp->nvwalk);
	return(count);
}

static char *nextname(Mac_t *mp,const char *prefix, int len)
{
	char *cp;
	if(len==0)
	{
		mp->nvwalk = nv_diropen(prefix);
		return((char*)mp->nvwalk);
	}
	if(!(cp=nv_dirnext(mp->nvwalk)))
		nv_dirclose(mp->nvwalk);
	return(cp);
}

/*
 * This routine handles $param,  ${parm}, and ${param op word}
 * The input stream is assumed to be a string
 */
static int varsub(Mac_t *mp)
{
	register int	c;
	register int	type=0; /* M_xxx */
	register char	*v,*argp=0;
	register Namval_t	*np = NIL(Namval_t*);
	register int 	dolg=0, mode=0;
	Namarr_t	*ap=0;
	int		dolmax=0, vsize= -1, offset= -1, nulflg, replen=0, bysub=0;
	char		idbuff[3], *id = idbuff, *pattern=0, *repstr;
	int		oldpat=mp->pattern,idnum=0,flag=0,d;
retry1:
	mp->zeros = 0;
	idbuff[0] = 0;
	idbuff[1] = 0;
	switch(sh_lexstates[ST_DOL][c=fcget()])
	{
	    case S_RBRA:
		if(type<M_SIZE)
			goto nosub;
		/* This code handles ${#} */
		c = mode;
		mode = type = 0;
		/* FALL THRU */
	    case S_SPC1:
		if(type==M_BRACE)
		{
			if(isaletter(mode=fcpeek(0)) || mode=='.')
			{
				if(c=='#')
					type = M_SIZE;
#ifdef SHOPT_TYPEDEF
				else if(c=='@')
				{
					type = M_TYPE;
					goto retry1;
				}
#endif /* SHOPT_TYPEDEF */
				else
					type = M_VNAME;
				mode = c;
				goto retry1;
			}
			else if(c=='#' && (isadigit(mode)||fcpeek(1)==RBRACE))
			{
				type = M_SIZE;
				mode = c;
				goto retry1;
			}
		}
		/* FALL THRU */
	    case S_SPC2:
		*id = c;
		v = special(c);
		if(isastchar(c))
		{
			mode = c;
#if  SHOPT_FILESCAN
			if(sh.cur_line)
			{
				v = getdolarg(&sh,1,(int*)0);
				dolmax = MAX_ARGN;
			}
			else
#endif  /* SHOPT_FILESCAN */
			dolmax = sh.st.dolc+1;
			dolg = (v!=0);
		}
		break;
	    case S_LBRA:
		if(type)
			goto nosub;
		type = M_BRACE;
		goto retry1;
	    case S_PAR:
		if(type)
			goto nosub;
		comsubst(mp,1);
		return(1);
	    case S_DIG:
		c -= '0';
		sh.argaddr = 0;
		if(type)
		{
			register int d;
			while((d=fcget()),isadigit(d))
				c = 10*c + (d-'0');
			fcseek(-1);
		}
		idnum = c;
		if(c==0)
			v = special(c);
#if  SHOPT_FILESCAN
		else if(sh.cur_line)
		{
			sh.used_pos = 1;
			v = getdolarg(&sh,c,&vsize);
		}
#endif  /* SHOPT_FILESCAN */
		else if(c <= sh.st.dolc)
		{
			sh.used_pos = 1;
			v = sh.st.dolv[c];
		}
		else
			v = 0;
		break;
	    case S_ALP:
		if(c=='.' && type==0)
			goto nosub;
		offset = staktell();
		do
		{
			np = 0;
			do
				stakputc(c);
			while(((c=fcget()),isaname(c))||type && c=='.');
			while(c==LBRACT && type)
			{
				sh.argaddr=0;
				if((c=fcget(),isastchar(c)) && fcpeek(0)==RBRACT)
				{
					if(type==M_VNAME)
						type = M_SUBNAME;
					idbuff[0] = mode = c;
					fcget();
					c = fcget();
					if(c=='.' || c==LBRACT)
					{
						stakputc(LBRACT);
						stakputc(mode);
						stakputc(RBRACT);
					}
					else
						flag = NV_ARRAY;
					break;
				}
				else
				{
					fcseek(-1);
					if(type==M_VNAME)
						type = M_SUBNAME;
					stakputc(LBRACT);
					v = stakptr(subcopy(mp,1));
					stakputc(RBRACT);
					c = fcget();
				}
			}
		}
		while(type && c=='.');
		if(c==RBRACE && type &&  fcpeek(-2)=='.')
		{
			stakseek(staktell()-1);
			type = M_TREE;
		}
		stakputc(0);
		id=stakptr(offset);
		if(isastchar(c) && type)
		{
			if(type==M_VNAME || type==M_SIZE)
			{
				idbuff[0] = mode = c;
				if((d=fcpeek(0))==c)
					idbuff[1] = fcget();
				if(type==M_VNAME)
					type = M_NAMESCAN;
				else
					type = M_NAMECOUNT;
				break;
			}
			goto nosub;
		}
		flag |= NV_NOASSIGN|NV_VARNAME|NV_NOADD;
		if(c=='=' || c=='?' || (c==':' && ((d=fcpeek(0))=='=' || d=='?')))
			flag &= ~NV_NOADD;
#if  SHOPT_FILESCAN
		if(sh.cur_line && *id=='R' && strcmp(id,"REPLY")==0)
		{
			sh.argaddr=0;
			np = REPLYNOD;
		}
		else
#endif  /* SHOPT_FILESCAN */
		np = nv_open(id,sh.var_tree,flag);
		ap = np?nv_arrayptr(np):0;
		if(type)
		{
			if(ap && isastchar(mode) && !(ap->nelem&ARRAY_SCAN))
				nv_putsub(np,NIL(char*),ARRAY_SCAN);
			if(!isbracechar(c))
				goto nosub;
			else
				fcseek(-1);
		}
		else
			fcseek(-1);
		if((type==M_VNAME||type==M_SUBNAME)  && sh.argaddr && strcmp(nv_name(np),id))
			sh.argaddr = 0;
		c = (type>M_BRACE && isastchar(mode));
		if(np && (!c || !ap))
		{
			if(type==M_VNAME)
			{
				type = M_BRACE;
				v = nv_name(np);
			}
#ifdef SHOPT_TYPEDEF
			else if(type==M_TYPE)
			{
#if 0
				Namval_t *nq = nv_type(np);
#else
				Namval_t *nq = 0;
#endif
				type = M_BRACE;
				if(nq)
					v = nv_name(nq);
				else
				{
					nv_attribute(np,sh.strbuf,"typeset",1);
					v = sfstruse(sh.strbuf);
				}
			}
#endif /* SHOPT_TYPEDEF */
#if  SHOPT_FILESCAN
			else if(sh.cur_line && np==REPLYNOD)
				v = sh.cur_line;
#endif  /* SHOPT_FILESCAN */
			else if(type==M_TREE)
				v = nv_getvtree(np,(Namfun_t*)0);
			else
			{
				v = nv_getval(np);
				/* special case --- ignore leading zeros */  
				if( (mp->arith||mp->let) && (np->nvfun || nv_isattr(np,(NV_LJUST|NV_RJUST|NV_ZFILL))) && (offset==0 || !isalnum(*((unsigned char*)stakptr(offset-1)))))
					mp->zeros = 1;
			}
		}
		else
			v = 0;
		stakseek(offset);
		if(ap)
		{
#if SHOPT_OPTIMIZE
			if(sh.argaddr)
				nv_optimize(np);
#endif
			if(isastchar(mode) && array_elem(ap)> !c)
				dolg = -1;
			else
				dolg = 0;
		}
		break;
	    case S_EOF:
		fcseek(-1);
	    default:
		goto nosub;
	}
	c = fcget();
	if(type>M_TREE)
	{
		if(c!=RBRACE)
			mac_error(np);
		if(type==M_NAMESCAN || type==M_NAMECOUNT)
		{
			id = prefix(id);
			stakseek(offset);
			if(type==M_NAMECOUNT)
			{
				c = namecount(mp,id);
				v = ltos(c);
			}
			else
			{
				dolmax = strlen(id);
				dolg = -1;
				nextname(mp,id,0);
				v = nextname(mp,id,dolmax);
			}
		}
		else if(type==M_SUBNAME)
		{
			if(dolg<0)
			{
				v = nv_getsub(np);
				bysub=1;
			}
			else if(v)
			{
				if(!ap || isastchar(mode))
					v = "0";
				else
					v = nv_getsub(np);
			}
		}
		else
		{
			if(!isastchar(mode))
				c = charlen(v,vsize);
			else if(dolg>0)
			{
#if  SHOPT_FILESCAN
				if(sh.cur_line)
				{
					getdolarg(&sh,MAX_ARGN,(int*)0);
					c = sh.offsets[0];
				}
				else
#endif  /* SHOPT_FILESCAN */
				c = sh.st.dolc;
			}
			else if(dolg<0)
				c = array_elem(ap);
			else
				c = (v!=0);
			dolg = dolmax = 0;
			v = ltos(c);
		}
		c = RBRACE;
	}
	nulflg = 0;
	if(type && c==':')
	{
		c = fcget();
		if(sh_lexstates[ST_BRACE][c]==S_MOD1 && c!='*' && c!= ':')
			nulflg=1;
		else if(c!='%' && c!='#')
		{
			fcseek(-1);
			c = ':';
		}
	}
	/* check for quoted @ */
	if(mode=='@' && mp->quote && !v && c!='-')
		mp->quoted-=2;
	if(type)
	{
		if(!isbracechar(c))
		{
			if(!nulflg)
				mac_error(np);
			fcseek(-1);
			c = ':';
		}
		if(c!=RBRACE)
		{
			int newops = (c=='#' || c == '%' || c=='/');
			offset = staktell();
			if(c=='/' ||c==':' || ((!v || (nulflg && *v==0)) ^ (c=='+'||c=='#'||c=='%')))
			{
				int newquote = mp->quote;
				int split = mp->split;
				int quoted = mp->quoted;
				int arith = mp->arith;
				int zeros = mp->zeros;
				if(newops)
				{
					type = fcget();
					if(type=='%' || type=='#')
					{
						int d = fcget();
						fcseek(-1);
						if(d=='(')
							type = 0;
					}
					fcseek(-1);
					mp->pattern = 1+(c=='/');
					mp->split = 0;
					mp->quoted = 0;
					mp->arith = mp->zeros = 0;
					newquote = 0;
				}
				else if(c=='?' || c=='=')
					mp->split = mp->pattern = 0;
				copyto(mp,RBRACE,newquote);
				if(!oldpat)
					mp->patfound = 0;
				mp->pattern = oldpat;
				mp->split = split;
				mp->quoted = quoted;
				mp->arith = arith;
				mp->zeros = zeros;
				/* add null byte */
				stakputc(0);
				stakseek(staktell()-1);
			}
			else
			{
				sh_lexskip(RBRACE,0,(!newops&&mp->quote)?ST_QUOTE:ST_NESTED);
				stakseek(offset);
			}
			argp=stakptr(offset);
		}
	}
	else
	{
		fcseek(-1);
		c=0;
	}
	if(c==':')  /* ${name:expr1[:expr2]} */
	{
		char *ptr;
		type = (int)sh_strnum(argp,&ptr,1);
		if(isastchar(mode))
		{
			if(id==idbuff)  /* ${@} or ${*} */
			{
				if(type<0 && (type+= dolmax)<0)
					type = 0;
				if(type==0)
					v = special(dolg=0);
#if  SHOPT_FILESCAN
				else if(sh.cur_line)
				{
					v = getdolarg(&sh,dolg=type,&vsize);
					if(!v)
						dolmax = type;
				}
#endif  /* SHOPT_FILESCAN */
				else if(type < dolmax)
					v = sh.st.dolv[dolg=type];
				else
					v =  0;
			}
			else if(ap)
			{
				if(type<0)
				{
					if(array_assoc(ap))
						type = -type;
					else
						type += array_maxindex(np);
				}
				if(array_assoc(ap))
				{
					while(type-- >0 && (v=0,nv_nextsub(np)))
						v = nv_getval(np);
				}
				else if(type > 0)
				{
					if(nv_putsub(np,NIL(char*),type|ARRAY_SCAN))
						v = nv_getval(np);
					else
						v = 0;
				}
			}
			else if(type>0)
				v = 0;
		}
		else if(v)
		{
			vsize = charlen(v,vsize);
			if(type<0 && (type += vsize)<0)
				type = 0;
			if(vsize < type)
				v = 0;
#if SHOPT_MULTIBYTE
			else if(mbwide())
			{
				mbinit();
				while(type-->0)
				{
					if((c=mbsize(v))<1)
						c = 1;
					v += c;
					vsize -= c;
				}
				c = ':';
			}
#endif /* SHOPT_MULTIBYTE */
			else
			{
				v += type;
				vsize -= type;
			}
		}
		if(*ptr==':')
		{
			if((type = (int)sh_strnum(ptr+1,&ptr,1)) <=0)
				v = 0;
			else if(isastchar(mode))
			{
				if(dolg>=0)
				{
					if(dolg+type < dolmax)
						dolmax = dolg+type;
				}
				else
					dolmax = type;
			}
			else if(type < vsize)
			{
#if SHOPT_MULTIBYTE
				if(mbwide())
				{
					char *vp = v;
					mbinit();
					while(type-->0)
					{
						if((c=mbsize(vp))<1)
							c = 1;
						vp += c;
					}
					type = vp-v;
					c = ':';
				}
#endif /* SHOPT_MULTIBYTE */
				vsize = type;
			}
		}
		if(*ptr)
			mac_error(np);
		stakseek(offset);
		argp = 0;
	}
	/* check for substring operations */
	else if(c == '#' || c == '%' || c=='/')
	{
		if(c=='/')
		{
			if(type=='/' || type=='#' || type=='%')
			{
				c = type;
				type = '/';
				argp++;
			}
			else
				type = 0;
		}
		else
		{
			if(type==c) /* ## or %% */
				argp++;
			else
				type = 0;
		}
		pattern = strdup(argp);
		if((type=='/' || c=='/') && (repstr = mac_getstring(pattern)))
			replen = strlen(repstr);
		if(v || c=='/' && offset>=0)
			stakseek(offset);
	}
retry2:
	if(v && (!nulflg || *v ) && c!='+')
	{
		register int d = (mode=='@'?' ':mp->ifs);
		int match[2*(MATCH_MAX+1)], nmatch, vsize_last;
		char *vlast;
		while(1)
		{
			if(!v)
				v= "";
			if(c=='/' || c=='#' || c== '%')
			{
				flag = (type || c=='/')?STR_GROUP|STR_MAXIMAL:STR_GROUP;
				if(c!='/')
					flag |= STR_LEFT;
				while(1)
				{
					vsize = strlen(v);
					if(c=='%')
						nmatch=substring(v,pattern,match,flag&STR_MAXIMAL);
					else
						nmatch=strgrpmatch(v,pattern,match,elementsof(match)/2,flag);
					if(replen>0)
						sh_setmatch(v,vsize,nmatch,match);
					if(nmatch)
					{
						vlast = v;
						vsize_last = vsize;
						vsize = match[0];
					}
					else if(c=='#')
						vsize = 0;
					if(vsize)
						mac_copy(mp,v,vsize);
					if(nmatch && replen>0)
						mac_substitute(mp,repstr,v,match,nmatch);
					if(nmatch==0)
						v += vsize;
					else
						v += match[1];
					if(*v &&  c=='/' && type)
					{
						/* avoid infinite loop */
						if(nmatch && match[1]==0)
							v++;
						continue;
					}
					vsize = -1;
					break;
				}
				if(replen==0)
					sh_setmatch(vlast,vsize_last,nmatch,match);
			}
			if(vsize)
				mac_copy(mp,v,vsize>0?vsize:strlen(v));
			if(dolg==0 && dolmax==0)
				 break;
			if(dolg>=0)
			{
				if(++dolg >= dolmax)
					break;
#if  SHOPT_FILESCAN
				if(sh.cur_line)
				{
					if(dolmax==MAX_ARGN && isastchar(mode))
						break;
					if(!(v=getdolarg(&sh,dolg,&vsize)))
					{
						dolmax = dolg;
						break;
					}
				}
				else
#endif  /* SHOPT_FILESCAN */
				v = sh.st.dolv[dolg];
			}
			else if(!np)
			{
				if(!(v = nextname(mp,id,dolmax)))
					break;
			}
			else
			{
				if(dolmax &&  --dolmax <=0)
				{
					nv_putsub(np,NIL(char*),ARRAY_UNDEF);
					break;
				}
				if(nv_nextsub(np) == 0)
					break;
				if(bysub)
					v = nv_getsub(np);
				else
					v = nv_getval(np);
			}
			if(mp->split && (!mp->quote || mode=='@'))
			{
				if(!np)
					mp->pattern = 0;
				endfield(mp,mp->quoted);
				mp->pattern = oldpat;
			}
			else if(d)
			{
				if(mp->sp)
					sfputc(mp->sp,d);
				else
					stakputc(d);
			}
		}
		if(pattern)
			free((void*)pattern);
	}
	else if(argp)
	{
		if(c=='/' && replen>0 && pattern && strmatch("",pattern))
			mac_substitute(mp,repstr,v,0,0);
		if(c=='?')
		{
			if(np)
				id = nv_name(np);
			else if(idnum)
				id = ltos(idnum);
			if(*argp)
			{
				stakputc(0);
				errormsg(SH_DICT,ERROR_exit(1),"%s: %s",id,argp);
			}
			else if(v)
				errormsg(SH_DICT,ERROR_exit(1),e_nullset,id);
			else
				errormsg(SH_DICT,ERROR_exit(1),e_notset,id);
		}
		else if(c=='=')
		{
			if(np)
			{
				if(sh.subshell)
					np = sh_assignok(np,1);
				nv_putval(np,argp,0);
				v = nv_getval(np);
				nulflg = 0;
				stakseek(offset);
				goto retry2;
			}
		else
			mac_error(np);
		}
	}
	else if(sh_isoption(SH_NOUNSET) && (!np  || nv_isnull(np)))
	{
		if(np)
			nv_close(np);
		errormsg(SH_DICT,ERROR_exit(1),e_notset,id);
	}
	if(np)
		nv_close(np);
	return(1);
nosub:
	if(type)
		mac_error(np);
	fcseek(-1);
	nv_close(np);
	return(0);
}

/*
 * This routine handles command substitution
 * <type> is 0 for older `...` version
 */
static void comsubst(Mac_t *mp,int type)
{
	Sfdouble_t		num;
	register int		c;
	register char		*str;
	Sfio_t			*sp;
	Fcin_t			save;
	struct slnod            *saveslp = sh.st.staklist;
	struct _mac_		savemac;
	int			savtop = staktell();
	char			lastc, *savptr = stakfreeze(0);
	int			was_history = sh_isstate(SH_HISTORY);
	int			was_verbose = sh_isstate(SH_VERBOSE);
	int			newlines,bufsize;
	register Shnode_t	*t;
	Namval_t		*np;
	sh.argaddr = 0;
	savemac = *mp;
	sh.st.staklist=0;
	if(type)
	{
		sp = 0;
		fcseek(-1);
		t = sh_dolparen();
		if(t && t->tre.tretyp==TARITH)
		{
			str =  t->ar.arexpr->argval;
			fcsave(&save);
			if(!(t->ar.arexpr->argflag&ARG_RAW))
				str = sh_mactrim(str,3);
			num = sh_arith(str);
		out_offset:
			stakset(savptr,savtop);
			*mp = savemac;
			if((Sflong_t)num==num)
				sfprintf(sh.strbuf,"%lld",(Sflong_t)num);
			else
				sfprintf(sh.strbuf,"%.*Lg",LDBL_DIG,num);
			str = sfstruse(sh.strbuf);
			mac_copy(mp,str,strlen(str));
			sh.st.staklist = saveslp;
			fcrestore(&save);
			return;
		}
	}
	else
	{
		while(fcgetc(c)!='`' && c)
		{
			if(c==ESCAPE)
			{
				fcgetc(c);
				if(!(isescchar(sh_lexstates[ST_QUOTE][c]) ||
				  (c=='"' && mp->quote)) || (c=='$' && fcpeek(0)=='\''))
					stakputc(ESCAPE);
			}
			stakputc(c);
		}
		c = staktell();
		str=stakfreeze(1);
		/* disable verbose and don't save in history file */
		sh_offstate(SH_HISTORY);
		sh_offstate(SH_VERBOSE);
		if(mp->sp)
			sfsync(mp->sp);	/* flush before executing command */
		sp = sfnew(NIL(Sfio_t*),str,c,-1,SF_STRING|SF_READ);
		c = sh.inlineno;
		sh.inlineno = error_info.line+sh.st.firstline;
		t = (Shnode_t*)sh_parse(mp->shp, sp,SH_EOF|SH_NL);
		sh.inlineno = c;
	}
#if KSHELL
	if(t)
	{
		fcsave(&save);
		sfclose(sp);
		if(t->tre.tretyp==0 && !t->com.comarg)
		{
			/* special case $(<file) and $(<#file) */
			register int fd;
			int r;
			struct checkpt buff;
			struct ionod *ip=0;
			sh_pushcontext(&buff,SH_JMPIO);
			if((ip=t->tre.treio) && 
				((ip->iofile&IOLSEEK) || !(ip->iofile&IOUFD)) &&
				(r=sigsetjmp(buff.buff,0))==0)
				fd = sh_redirect(ip,3);
			else
				fd = sh_chkopen(e_devnull);
			sh_popcontext(&buff);
			if(r==0 && ip && (ip->iofile&IOLSEEK))
			{
				if(sp=sh.sftable[fd])
					num = sftell(sp);
				else
					num = lseek(fd, (off_t)0, SEEK_CUR);
				goto out_offset;
			}
			sp = sfnew(NIL(Sfio_t*),(char*)malloc(IOBSIZE+1),IOBSIZE,fd,SF_READ|SF_MALLOC);
		}
		else
			sp = sh_subshell(t,sh_isstate(SH_ERREXIT),1);
		fcrestore(&save);
	}
	else
		sp = sfopen(NIL(Sfio_t*),"","sr");
	sh_freeup();
	sh.st.staklist = saveslp;
	if(was_history)
		sh_onstate(SH_HISTORY);
	if(was_verbose)
		sh_onstate(SH_VERBOSE);
#else
	sp = sfpopen(NIL(Sfio_t*),str,"r");
#endif
	*mp = savemac;
	np = nv_scoped(IFSNOD);
	nv_putval(np,mp->ifsp,0);
	mp->ifsp = nv_getval(np);
	stakset(savptr,savtop);
	newlines = 0;
	lastc = 0;
	sfsetbuf(sp,(void*)sp,0);
	bufsize = sfvalue(sp);
	/* read command substitution output and put on stack or here-doc */
	sfpool(sp, NIL(Sfio_t*), SF_WRITE);
	while((str=(char*)sfreserve(sp,SF_UNBOUND,0)) && (c = sfvalue(sp))>0)
	{
#if SHOPT_CRNL
		/* eliminate <cr> */
		register char *dp;
		char *buff = str;
		while(c>1 && (*str !='\r'|| str[1]!='\n'))
		{
			c--;
			str++;
		}
		dp = str;
		while(c>1)
		{
			str++;
			c--;
			while(c>1 && (*str!='\r' || str[1]!='\n'))
			{
				c--;
				*dp++ = *str++;
			}
		}
		if(c)
			*dp++ = *str++;
		*dp = 0;
		str = buff;
		c = dp-str;
#endif /* SHOPT_CRNL */
		if(newlines >0)
		{
			if(mp->sp)
				sfnputc(mp->sp,'\n',newlines);
			else if(!mp->quote && mp->split && sh.ifstable['\n'])
				endfield(mp,0);
			else	while(newlines--)
					stakputc('\n');
			newlines = 0;
		}
		else if(lastc)
		{
			mac_copy(mp,&lastc,1);
			lastc = 0;
		}
		/* delay appending trailing new-lines */
		while(str[--c]=='\n')
			newlines++;
		if(++c < bufsize)
			str[c] = 0;
		else
		{
			/* can't write past buffer so save last character */
			lastc = str[--c];
			str[c] = 0;
		}
		mac_copy(mp,str,c);
	}
	if(--newlines>0 && sh.ifstable['\n']==S_DELIM)
	{
		if(mp->sp)
			sfnputc(mp->sp,'\n',newlines);
		else if(!mp->quote && mp->split && sh.ifstable['\n'])
			endfield(mp,0);
		else	while(newlines--)
				stakputc('\n');
	}
	if(lastc)
		mac_copy(mp,&lastc,1);
	sfclose(sp);
	return;
}

/*
 * copy <str> onto the stack
 */
static void mac_copy(register Mac_t *mp,register const char *str, register int size)
{
	register char		*state;
	register const char	*cp=str;
	register int		c,n,nopat;
	nopat = (mp->quote||mp->assign==1||mp->arith);
	if(mp->zeros)
	{
		/* prevent leading 0's from becomming octal constants */
		while(size>1 && *str=='0')
			str++,size--;
		mp->zeros = 0;
		cp = str;
	}
	if(mp->sp)
		sfwrite(mp->sp,str,size);
	else if(mp->pattern>=2 || (mp->pattern && nopat))
	{
		state = sh_lexstates[ST_MACRO];
		/* insert \ before file expansion characters */
		while(size-->0)
		{
			c = state[n= *(unsigned char*)cp++];
			if(nopat&&(c==S_PAT||c==S_ESC||c==S_BRACT||c==S_ENDCH) && mp->pattern!=3)
				c=1;
			else if(mp->pattern==4 && (c==S_ESC||c==S_BRACT||c==S_ENDCH || isastchar(n)))
				c=1;
			else if(mp->pattern==2 && c==S_SLASH)
				c=1;
			else if(mp->pattern==3 && c==S_ESC && (state[*(unsigned char*)cp]==S_DIG||(*cp==ESCAPE)))
			{
				if(!(c=mp->quote))
					cp++;
			}
			else
				c=0;
			if(c)
			{
				if(c = (cp-1) - str)
					stakwrite(str,c);
				stakputc(ESCAPE);
				str = cp-1;
			}
		}
		if(c = cp-str)
			stakwrite(str,c);
	}
	else if(!mp->quote && mp->split && (mp->ifs||mp->pattern))
	{
		/* split words at ifs characters */
		state = sh.ifstable;
		if(mp->pattern)
		{
			char *sp = "&|()";
			while(c = *sp++)
			{
				if(state[c]==0)
					state[c] = S_EPAT;
			}
			sp = "*?[{";
			while(c = *sp++)
			{
				if(state[c]==0)
					state[c] = S_PAT;
			}
			if(state[ESCAPE]==0)
				state[ESCAPE] = S_ESC;
		}
		while(size-->0)
		{
			if((n=state[c= *(unsigned char*)cp++])==S_ESC || n==S_EPAT)
			{
				/* don't allow extended patterns in this case */
				mp->patfound = mp->pattern;
				stakputc(ESCAPE);
			}
			else if(n==S_PAT)
				mp->patfound = mp->pattern;
			else if(n && mp->ifs)
			{
#if SHOPT_MULTIBYTE
				if(n==S_MBYTE)
				{
					if(sh_strchr(mp->ifsp,cp-1)<0)
						continue;
					n = mbsize(cp-1) - 1;
					if(n==-2)
						n = 0;
					cp += n;
					size -= n;
					n= S_DELIM;
				}
#endif /* SHOPT_MULTIBYTE */
				if(n==S_SPACE || n==S_NL)
				{
					while(size>0 && ((n=state[c= *(unsigned char*)cp++])==S_SPACE||n==S_NL))
						size--;
#if SHOPT_MULTIBYTE
					if(n==S_MBYTE && sh_strchr(mp->ifsp,cp-1)>=0)
					{
						n = mbsize(cp-1) - 1;
						if(n==-2)
							n = 0;
						cp += n;
						size -= n;
						n=S_DELIM;
					}
					else
#endif /* SHOPT_MULTIBYTE */
					if(n==S_DELIM)
						size--;
				}
				endfield(mp,n==S_DELIM||mp->quoted);
				mp->patfound = 0;
				if(n==S_DELIM)
					while(size>0 && ((n=state[c= *(unsigned char*)cp++])==S_SPACE||n==S_NL))
						size--;
				if(size<=0)
					break;
				cp--;
				continue;

			}
			stakputc(c);
		}
		if(mp->pattern)
		{
			cp = "&|()";
			while(c = *cp++)
			{
				if(state[c]==S_EPAT)
					state[c] = 0;
			}
			cp = "*?[{";
			while(c = *cp++)
			{
				if(state[c]==S_PAT)
					state[c] = 0;
			}
			if(sh.ifstable[ESCAPE]==S_ESC)
				sh.ifstable[ESCAPE] = 0;
		}
	}
	else
		stakwrite(str,size);
}

/*
 * Terminate field.
 * If field is null count field if <split> is non-zero
 * Do filename expansion of required
 */
static void endfield(register Mac_t *mp,int split)
{
	register struct argnod *argp;
	register int count=0;
	if(staktell() > ARGVAL || split)
	{
		argp = (struct argnod*)stakfreeze(1);
		argp->argnxt.cp = 0;
		argp->argflag = 0;
		if(mp->patfound)
		{
			sh.argaddr = 0;
#if SHOPT_BRACEPAT
			count = path_generate(argp,mp->arghead);
#else
			count = path_expand(argp->argval,mp->arghead);
#endif /* SHOPT_BRACEPAT */
			if(count)
				mp->fields += count;
			else if(split)	/* pattern is null string */
				*argp->argval = 0;
			else	/* pattern expands to nothing */
				count = -1;
		}
		if(count==0)
		{
			argp->argchn.ap = *mp->arghead;
			*mp->arghead = argp;
			mp->fields++;
		}
		if(count>=0)
		{
			(*mp->arghead)->argflag |= ARG_MAKE;
			if(mp->assign || sh_isoption(SH_NOGLOB))
				argp->argflag |= ARG_RAW|ARG_EXP;
		}
		stakseek(ARGVAL);
	}
	mp->quoted = mp->quote;
}

/*
 * Finds the right substring of STRING using the expression PAT
 * the longest substring is found when FLAG is set.
 */
static int substring(register const char *string,const char *pat,int match[], int flag)
{
	register const char *sp=string;
	register int size,len,nmatch,n;
	int smatch[2*(MATCH_MAX+1)];
	if(flag)
	{
		if(n=strgrpmatch(sp,pat,smatch,elementsof(smatch)/2,STR_RIGHT|STR_MAXIMAL))
		{
			memcpy(match,smatch,n*2*sizeof(smatch[0]));
			return(n);
		}
		return(0);
	}
	size = len = strlen(sp);
	sp += size;
	while(sp>=string)
	{
#if SHOPT_MULTIBYTE
		if(mbwide())
			sp = lastchar(string,sp);
#endif /* SHOPT_MULTIBYTE */
		if(n=strgrpmatch(sp,pat,smatch,elementsof(smatch)/2,STR_RIGHT|STR_LEFT|STR_MAXIMAL))
		{
			nmatch = n;
			memcpy(match,smatch,n*2*sizeof(smatch[0]));
			size = sp-string;
			break;
		}
		sp--;
	}
	if(size==len)
		return(0);
	if(nmatch)
	{
		nmatch *=2;
		while(--nmatch>=0)
			match[nmatch] += size;
	}
	return(n);
}

#if SHOPT_MULTIBYTE
	static char	*lastchar(const char *string, const char *endstring)
	{
		register char *str = (char*)string;
		register int c;
		mbinit();
		while(*str)
		{
			if((c=mbsize(str))<0)
				c = 1;
			if(str+c > endstring)
				break;
			str += c;
		}
		return(str);
	}
#endif /* SHOPT_MULTIBYTE */
static int	charlen(const char *string,int len)
{
	if(!string)
		return(0);
#if SHOPT_MULTIBYTE
	if(mbwide())
	{
		register const char *str = string, *strmax=string+len;
		register int n=0;
		mbinit();
		if(len>0)
		{
			while(str<strmax && mbchar(str))
				n++;
		}
		else while(mbchar(str))
			n++;
		return(n);
	}
	else
#endif /* SHOPT_MULTIBYTE */
	{
		if(len<0)
			return(strlen(string));
		return(len);
	}
}

/*
 * This is the default tilde discipline function
 */
static int sh_btilde(int argc, char *argv[], void *context)
{
	char *cp = sh_tilde(argv[1]);
	NOT_USED(argc);
	NOT_USED(context);
	if(!cp)
		cp = argv[1];
	sfputr(sfstdout, cp, '\n');
	return(0);
}
 
/*
 * <offset> is byte offset for beginning of tilde string
 */
static void tilde_expand2(register int offset)
{
	char		shtilde[10], *av[3], *ptr=stakfreeze(1);
	Sfio_t		*iop, *save=sfstdout;
	Namval_t	*np;
	static int	beenhere=0;
	strcpy(shtilde,".sh.tilde");
	np = nv_open(shtilde,sh.fun_tree, NV_VARNAME|NV_NOARRAY|NV_NOASSIGN|NV_NOFAIL);
	if(np && !beenhere)
	{
		beenhere = 1;
		sh_addbuiltin(shtilde,sh_btilde,0);
	}
	av[0] = ".sh.tilde";
	av[1] = &ptr[offset];
	av[2] = 0;
	iop = sftmp(IOBSIZE+1);;
	sfset(iop,SF_READ,0);
	sfstdout = iop;
	if(np)
		sh_fun(np, (Namval_t*)0, av);
	else
		sh_btilde(2, av, &sh);
	sfstdout = save;
	stakset(ptr, offset);
	sfseek(iop,(Sfoff_t)0,SEEK_SET);
	sfset(iop,SF_READ,1);
	if(ptr = sfreserve(iop, SF_UNBOUND, -1))
	{
		Sfoff_t n = sfvalue(iop);
		while(ptr[n-1]=='\n')
			n--;
		if(n==1 && fcpeek(0)=='/' && ptr[n-1])
			n--;
		if(n)
			stakwrite(ptr,n);
	}
	else
		stakputs(av[1]);
	sfclose(iop);
}

/*
 * This routine is used to resolve ~ expansion.
 * A ~ by itself is replaced with the users login directory.
 * A ~- is replaced by the previous working directory in shell.
 * A ~+ is replaced by the present working directory in shell.
 * If ~name  is replaced with login directory of name.
 * If string doesn't start with ~ or ~... not found then 0 returned.
 */
                                                            
static char *sh_tilde(register const char *string)
{
	register char		*cp;
	register int		c;
	register struct passwd	*pw;
	register Namval_t *np=0;
	static Dt_t *logins_tree;
	if(*string++!='~')
		return(NIL(char*));
	if((c = *string)==0)
	{
		if(!(cp=nv_getval(nv_scoped(HOME))))
			cp = getlogin();
		return(cp);
	}
	if((c=='-' || c=='+') && string[1]==0)
	{
		if(c=='+')
			cp = nv_getval(nv_scoped(PWDNOD));
		else
			cp = nv_getval(nv_scoped(OLDPWDNOD));
		return(cp);
	}
	if(logins_tree && (np=nv_search(string,logins_tree,0)))
		return(nv_getval(np));
	if(!(pw = getpwnam(string)))
		return(NIL(char*));
	if(!logins_tree)
		logins_tree = dtopen(&_Nvdisc,Dtbag);
	if(np=nv_search(string,logins_tree,NV_ADD))
		nv_putval(np, pw->pw_dir,0);
	return(pw->pw_dir);
}

/*
 * return values for special macros
 */
static char *special(register int c)
{
	register Namval_t *np;
	if(c!='$')
		sh.argaddr = 0;
	switch(c)
	{
	    case '@':
	    case '*':
		return(sh.st.dolc>0?sh.st.dolv[1]:NIL(char*));
	    case '#':
#if  SHOPT_FILESCAN
		if(sh.cur_line)
		{
			getdolarg(&sh,MAX_ARGN,(int*)0);
			return(ltos(sh.offsets[0]));
		}
#endif  /* SHOPT_FILESCAN */
		return(ltos(sh.st.dolc));
	    case '!':
		if(sh.bckpid)
			return(ltos(sh.bckpid));
		break;
	    case '$':
		if(nv_isnull(SH_DOLLARNOD))
			return(ltos(sh.pid));
		return(nv_getval(SH_DOLLARNOD));
	    case '-':
		return(sh_argdolminus());
	    case '?':
		return(ltos(sh.savexit));
	    case 0:
		if(sh_isstate(SH_PROFILE) || !error_info.id || ((np=nv_search(error_info.id,sh.bltin_tree,0)) && nv_isattr(np,BLT_SPC)))
			return(sh.shname);
		else
			return(error_info.id);
	}
	return(NIL(char*));
}

/*
 * Handle macro expansion errors
 */
static void mac_error(Namval_t *np)
{
	if(np)
		nv_close(np);
	errormsg(SH_DICT,ERROR_exit(1),e_subst,fcfirst());
}

/*
 * Given pattern/string, replace / with 0 and return pointer to string
 * \ characters are stripped from string.
 */ 
static char *mac_getstring(char *pattern)
{
	register char *cp = pattern;
	register int c;
	while(c = *cp++)
	{
		if(c==ESCAPE)
			cp++;
		else if(c=='/')
		{
			cp[-1] = 0;
			return(cp);
		}
	}
	return(NIL(char*));
}
