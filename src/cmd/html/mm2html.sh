########################################################################
#                                                                      #
#               This software is part of the ast package               #
#          Copyright (c) 1996-2010 AT&T Intellectual Property          #
#                      and is licensed under the                       #
#                  Common Public License, Version 1.0                  #
#                    by AT&T Intellectual Property                     #
#                                                                      #
#                A copy of the License is available at                 #
#            http://www.opensource.org/licenses/cpl1.0.txt             #
#         (with md5 checksum 059e8cd6165cb4c31e351f2b69388fd9)         #
#                                                                      #
#              Information and Software Systems Research               #
#                            AT&T Research                             #
#                           Florham Park NJ                            #
#                                                                      #
#                 Glenn Fowler <gsf@research.att.com>                  #
#                                                                      #
########################################################################
: mm2html - convert mm/man subset to html

# it keeps going and going ...
#
# \h'0*\w"URL"'HOT-TEXT\h'0'	link goto with alternate url
# \h'0*1'HOT-TEXT\h'0'		link goto
# \h'0/\w"LABEL"'TEXT\h'0'	local link label with alternate text
# \h'0/1'LABEL\h'0'		local link label
#
# .xx meta.NAME="CONTENT"	<meta name="NAME" content="CONTENT">
# .xx label="LABEL"		local link label request
# .xx link="URL\tHOT-TEXT"	link goto with url request
# .xx linkframe="URL\tHOT-TEXT"	link goto with url request (no target=_top)
# .xx link="HOT-TEXT"		link goto request
# .xx linkframe="HOT-TEXT"	link goto request (no target=_top)
# .xx ref="URL\tMIME-TYPE"	head link hint
# .xx begin=internal		begin internal text
# .xx end=internal		end internal text
# .xx index=0|1			stop|reset header index
# .xx noFOO			.xx FOO=0
#
# .sn file			like .so but text copied to output

command=mm2html
version='mm2html (AT&T Research) 2010-01-19' # NOTE: repeated in USAGE
LC_NUMERIC=C
case $(getopts '[-][123:xyz]' opt --xyz 2>/dev/null; echo 0$opt) in
0123)	ARGV0="-a $command"
	USAGE=$'
[-?
@(#)$Id: mm2html (AT&T Research) 2010-01-19 $
]
'$USAGE_LICENSE$'
[+NAME?mm2html - convert mm/man subset to html]
[+DESCRIPTION?\bmm2html\b is a \bsed\b(1)/\bksh\b(1) script (yes!) that
    converts input \bmm\b(1) or \bman\b(1) documents to an \bhtml\b
    document on the standard output. If \afile\a is omitted then the
    standard input is read. \btroff2html\b(1) is similar but does a full
    \btroff\b(1) parse. \adir\a operands and directory components of
    \afile\a operands are added to the included file search list.]
[f:frame?Generate framed HTML files in:]:[name]
    {
        [+man documents]
            {
                [+\aname\a\b.html?The parent frame page.]
                [+\aname\a\b-toc.html?The table of contents (link)
                    frame.]
                [+\aname\a\b-man.html?The (also standalone) man page
                    frame.]
            }
        [+other documents]
            {
                [+\aname\a\b.html?The main body.]
                [+\aname\a\b-index.html?The frame index (if \b--index\b
                    specified).]
                [+\aname\a\b-temp.html?Temporary frame goto labels.]
            }
    }
[g:global-index?Generate a standalone \bindex.html\b for framed HTML.]
[h:html?Read html options from \afile\a. Unknown options are silently
    ignored. See the \b.xx\b request below for a description of the
    options. The file pathname may be followed by URL style \aname=value\a
    pairs that are evaluated as if they came from
    \afile.\a]:[file[??name=value;...]]]
[l:license?Read license identification options from \afile\a. Unknown
    options are silently ignored. See the \b.xx\b request below for a
    description of the options. The file pathname may be followed by URL
    style \aname=value\a pairs that are evaluated as if they came from
    \afile\a.]:[file[??name=value;...]]]
[o:option?Sets a space or \b,\b separated list of \b--license\b
    options. Option values with embedded spaces must be
    quoted.]:[[no]]name=value]
[t:top?Open non-local urls in the top frame.]
[x:index?Generate a standalone \aname\a\b-index.html\b for framed HTML
    where \aname\a is specified by \b--frame\b.]
[ [ dir | file ]
    ... ]
[+EXTENSIONS?\b.xx\b \aname\a[=\avalue\a]] is a special \bmm2html\b
    request that handles program tracing, \bhtml\b extensions and \atroff\a
    macro package magic. Supported operations are:]
    {
        [+author=text?Specifies the contact information for the
            document HEAD section.]
        [+background=URL?Specifies the document background URL.]
        [+logo=URL?Specifies the logo/banner image URL that is centered
            at the top of the document.]
        [+mailto=address?Sets the email \aaddress\a to send comments
            and suggestions.]
        [+meta.name?Emits the \bhtml\b tag \b<META name=\b\aname\a
            \bcontent=\b\acontent\a\b>\b.]
        [+package=text?\atext\a is prepended to the \bhtml\b document
            title.]
        [+title=text?Sets the document title.]
    }
[+?Local URL links are generated for all top level headings. These can
    be referenced by embedding the benign (albeit convoluted) \atroff\a
    construct \\h'\'$'0*\\w\"label\"'\'$'text\\h'\'$'0'\'$', where
    \alabel\a is the local link label and \atext\a is the hot link text. If
    \alabel\a and \atext\a are the same then use
    \\h'\'$'0*1'\'$'text\\h'\'$'0'\'$'.]
[+?\bman\b(1) links are generated for bold or italic identifiers that
    are immediately followed by a parenthesized number.]
[+FILES]
    {
        [+$HOME/.2html?Default rendering info.]
    }
[+SEE ALSO?\btroff2html\b(1), \bhtml2rtf\b(1)]
'
	;;
*)	ARGV0=""
	USAGE='f:gh:l:o:tx [ file ... ]'
	;;
esac

set -o noglob

integer count row n s ndirs=0 nfiles=0 last_level=0
integer fd=0 head=2 line=0 lists=0 nest=0 peek=0 pp=0 so=0 soff=4
integer labels=0 mark=4 reference=1 ident=0 ce=0 nf=0 augment=0 tbl_ns=0 tbl_no=1 tbl_fd=1
typeset -Z2 page=01
typeset -u upper
typeset -x -l OP
typeset -x -A ds map nr outline
typeset cond dirs files fg frame label level list prev text trailer type
typeset license html meta nl mm index authors vg header references ss
typeset mm_AF mm_AF_cur mm_AF_old mm_AU

nl=$'\n'

license=(
	author=
)
html=(
	BODY=(
		bgcolor=#ffffff
	)
	TABLE=(
		bgcolor=#ffd0d0
		border=0
		bordercolor=#ffffff
		frame="void"
		rules="none"
	)
	H1=(
		align=center
	)
	FRAMESET=(
		rows=80,*
		border=0
		frameborder=0
		framespacing=0
	)
	home=(
		href=
	)
	ident=1
	index=(
		left=
		top=
	)
	logo=(
		src=
	)
	magic=(
		plain='<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" "http://www.w3.org/TR/REC-html40/loose.dtd">'
		frame='<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Frameset//EN" "http://www.w3.org/TR/REC-html40/frameset.dtd">'
	)
	width=96%
)

code=0
figure=1
file=
font=
frame=
framebody=
framelink=
framerefs=
hp=
ifs=${IFS-'
	'}
inch="     "
indent=
indent_prev=
macros=
pd='<P>'
pm=
primary=".BL|.LI|.IX"
redirect_old=
redirect_new=
ss="verdana,arial,helvetica,geneva,sans-serif"
top=
vg_ps=20

function setmacros
{
	case $1 in
	man)	mark=6 ;;
	*)	mark=4 ;;
	esac
	macros=$1
}

function options
{
	typeset i o q v
	IFS="${IFS},"
	set $OPTARG
	IFS=$ifs
	for i
	do	case $q in
		'')	o=${i%%=*}
			v=${i#*=}
			case $v in
			\'*)	q=\'; v=${v#?}; continue ;;
			\"*)	q=\"; v=${v#?}; continue ;;
			esac
			;;
		*)	v="$v $i"
			case $i in
			*$q)	v=${v%?} ;;
			*)	continue ;;
			esac
			;;
		esac
		case $o in
		no*)	unset ${o#no}
			;;
		*)	case $v in
			"")	v=1 ;;
			esac
			case $o in
			*.*|[ABCDEFGHIJKLMNOPQRSTUVWXYZ]*)
				eval $o="'$v'"
				;;
			*)	eval license.$o="'$v'"
				;;
			esac
			;;
		esac
	done
}

if	[[ -f $HOME/.2html ]]
then	. $HOME/.2html
fi

while	getopts $ARGV0 "$USAGE" OPT
do	case $OPT in
	f)	frame=$OPTARG
		;;
	g)	index=global
		;;
	h)	case $OPTARG in
		*\?*)	. ${OPTARG%%\?*} || exit 1
			eval "html=( ${OPTARG#*\?} )"
			;;
		*)	. $OPTARG || exit 1
			;;
		esac
		;;
	l)	case $OPTARG in
		*\?*)	. ${OPTARG%%\?*} || exit 1
			eval "license=( ${OPTARG#*\?} )"
			;;
		*)	. $OPTARG || exit 1
			;;
		esac
		;;
	o)	options "$OPTARG"
		;;
	t)	top=1
		;;
	x)	index=local
		;;
	*)	echo "Usage: $command [-x] [-f name] [-h file[?name=value;...]]
	       [-l file[?name=value;...]] [-o [no]name=value] [ file ... ]" >&2
		exit 2
		;;
	esac
done
shift OPTIND-1
case $# in
1)	file=$1
	if	[[ ! -f $file ]]
	then	print -u2 $command: $file: cannot read
		exit 1
	fi
	x="-m $file"
	;;
*)	x=
	;;
esac

HTMLPATH=
for i in $HOME ${PATH//:/ }
do	d=${i%/bin}/lib/html
	if	[[ -d $d ]]
	then	if	[[ -w $d || -x $i/$command ]]
		then	HTMLPATH=$HTMLPATH:$d
			if	[[ -x $i/$command ]]
			then	break
			fi
		fi
	fi
done
HTMLPATH=${HTMLPATH#:}

ds[Cr]='&#169;'
ds[Dt]=$(date -f "%B %d, %Y" $x)
ds[Rf]="\\u[$reference]\\d"
ds[Rg]='&#174;'
ds[CM]='&#169;'
ds[RM]='&#174;'
ds[SM]='<FONT SIZE=-6><B><SUP>SM</SUP></B></FONT>'
ds[TM]='<FONT SIZE=-6><B><SUP>TM</SUP></B></FONT>'

map[.Cs]=.EX
map[.Ce]=.EE
map[.Sh]=.SH
map[.Ss]=.SS
map[.Tp]=.TP

H=H$(( head + 1 ))

function warning
{
	print -u2 "$command: warning: ${file:+"$file: "}line $line:" "$@"
}

function getfiles
{
	sed	\
	-e 's%\\".*%%' \
	-e 's%\\(>=%>=%g' \
	-e 's%\\(<=%<=%g' \
	-e 's%&%\&amp;%g' \
	-e 's%<%\&lt;%g' \
	-e 's%>%\&gt;%g' \
	-e 's%\[%\&#0091;%g' \
	-e 's%\]%\&#0093;%g' \
	-e 's%\\&amp;%<!--NULL-->%g' \
	-e 's%\\'\''%'\''%g' \
	-e 's%\\`%`%g' \
	-e 's%\\-%\&#45;%g' \
	-e 's%\\+%+%g' \
	-e 's%\\0%\&nbsp;%g' \
	-e 's%\\|%\&nbsp;%g' \
	-e 's%\\\^%\&nbsp;%g' \
	-e 's%\\ %\&nbsp;%g' \
	-e 's%\\(+-%\&#177;%g' \
	-e 's%\\(-%=%g' \
	-e 's%\\(ap%~%g' \
	-e 's%\\(bu%\&#183;%g' \
	-e 's%\\(bv%|%g' \
	-e 's%\\(co%\&#169;%g' \
	-e 's%\\(dg%\&#167;%g' \
	-e 's%\\(fm%'\''%g' \
	-e 's%\\(rg%\&#174;%g' \
	-e 's%\\(sq%\&#164;%g' \
	-e 's%\\(\*\([*`'\'']\)%\1%g' \
	-e 's%\\\*\([*`'\'']\)%\1%g' \
	-e 's%\\d\([^\\]*\)\\u%<SUB>\1</SUB>%g' \
	-e 's%\\u\([^\\]*\)\\d%<SUP>\1</SUP>%g' \
	-e 's%\\v\(.\)-\([^\\]*\)\1\(.*\)\\v\1+*\2\1%<SUB>\3</SUB>%g' \
	-e 's%\\v\(.\)+*\([^\\]*\)\1\(.*\)\\v\1-\2\1%<SUP>\3</SUP>%g' \
	-e 's%\\h'\''0\*\\w"\([abcdefghijklmnopqrstuvwxyz]*:[^"]*\)"'\''\([^'\'']*\)\\h'\''0'\''%<A href="\1" target=_top>\2</A>%g' \
	-e 's%\\h'\''0\*\\w"\(/[^"]*\)"'\''\([^'\'']*\)\\h'\''0'\''%<A href="\1" target=_top>\2</A>%g' \
	-e 's%\\h'\''0\*\\w"\([^"]*\)"'\''\([^'\'']*\)\\h'\''0'\''%<A href="\1">\2</A>%g' \
	-e 's%\\h'\''0\*1'\''\([^:/'\'']*\)\\h'\''0'\''%<A href="#\1">\1</A>%g' \
	-e 's%\\h'\''0\*1'\''\([abcdefghijklmnopqrstuvwxyz]*:[^'\'']*\)\\h'\''0'\''%<A href="\1" target=_top>\1</A>%g' \
	-e 's%\\h'\''0\*1'\''\(/[^'\'']*\)\\h'\''0'\''%<A href="\1" target=_top>\1</A>%g' \
	-e 's%\\h'\''0\*1'\''\([^'\'']*\)\\h'\''0'\''%<A href="\1" target=_parent>\1</A>%g' \
	-e 's%\\h'\''0/\\w"\([^"]*\)"'\''\([^'\'']*\)\\h'\''0'\''%<A name="\1">\2</A>%g' \
	-e 's%\\h'\''0/1'\''\([^'\'']*\)\\h'\''0'\''%<A name="\1">\1</A>%g' \
	-e 's%\\s+\(.\)\([^\\]*\)\\s-\1%<FONT SIZE=+\1>\2</FONT>%g' \
	-e 's%\\s+\(.\)\([^\\]*\)\\s0%<FONT SIZE=+\1>\2</FONT>%g' \
	-e 's%\\s-\(.\)\([^\\]*\)\\s+\1%<FONT SIZE=-\1>\2</FONT>%g' \
	-e 's%\\s-\(.\)\([^\\]*\)\\s0%<FONT SIZE=-\1>\2</FONT>%g' \
	-e 's%\\f(\(..\)\([^\\]*\)%<\1>\2</\1>%g' \
	-e 's%\\f[PR]%\\fZ%g' \
	-e 's%\\f\(.\)\([^\\]*\)%<\1>\2</\1>%g' \
	-e 's%&lt;\([abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789][-._abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]*@[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.]*\)&gt;%<SMALL>\&lt;<A href=mailto:\1>\1</A>\&gt;</SMALL>%g' \
	-e 's%\[[ABCDEFGHIJKLMNOPQRSTUVWXYZ][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz]*[0123456789][0123456789][abcdefghijklmnopqrstuvwxyz]*]%<CITE>&</CITE>%g' \
	-e 's%</*Z>%%g' \
	-e 's%<[146789]>%%g' \
	-e 's%</[146789]>%%g' \
	-e 's%<2>%<EM>%g' \
	-e 's%</2>%</EM>%g' \
	-e 's%<3>%<STRONG>%g' \
	-e 's%</3>%</STRONG>%g' \
	-e 's%<5>%<TT>%g' \
	-e 's%</5>%</TT>%g' \
	-e 's%<B>%<STRONG>%g' \
	-e 's%</B>%</STRONG>%g' \
	-e 's%<I>%<EM>%g' \
	-e 's%</I>%</EM>%g' \
	-e 's%<L>%<TT>%g' \
	-e 's%</L>%</TT>%g' \
	-e 's%<X>%<TT>%g' \
	-e 's%</X>%</TT>%g' \
	-e 's%<CW>%<TT>%g' \
	-e 's%</CW>%</TT>%g' \
	-e 's%<EM>\([^<]*\)</EM>(\([0123456789][abcdefghijklmnopqrstuvwxyz]*\))%<NOBR><A href="../man\2/\1.html"><EM>\1</EM></A>(\2)</NOBR>%g' \
	-e 's%<STRONG>\([^<]*\)</STRONG>(\([0123456789][abcdefghijklmnopqrstuvwxyz]*\))%<NOBR><A href="../man\2/\1.html"><STRONG>\1</STRONG></A>(\2)</NOBR>%g' \
	-e 's%<TT>\([^<]*\)</TT>(\([0123456789][abcdefghijklmnopqrstuvwxyz]*\))%<NOBR><A href="../man\2/\1.html"><TT>\1</TT></A>(\2)</NOBR>%g' \
	-e 's%\\s+\(.\)\(.*\)\\s-\1%<FONT SIZE=+\1>\2</FONT>%g' \
	-e 's%\\s-\(.\)\(.*\)\\s+\1%<FONT SIZE=-\1>\2</FONT>%g' \
	-e 's%\\c%<JOIN>%g' \
	-e 's%\\e%\&#0092;%g' \
	-e '/^'\''[abcdefghijklmnopqrstuvwxyz][abcdefghijklmnopqrstuvwxyz]\>/s%.%.%' \
	-e '/^\..*".*\\/s%\\[^\*][^(]%\\&%g' \
	"$@"
}

function getline
{
	integer i n
	typeset data a c d q v x y z
	if	(( peek ))
	then	(( peek = 0 ))
		trap 'set -- "${text[@]}"' 0
		return
	fi
	while	:
	do	data=
		while	:
		do	IFS= read -r -u$fd a || {
				if	(( so > 0 ))
				then	eval exec $fd'>&-'
					if	(( ( fd = --so + soff ) == soff ))
					then	(( fd = 0 ))
					fi
					file=${so_file[so]}
					line=${so_line[so]}
					continue
				fi
				return 1
			}
			(( line++ ))
			case $a in
			*\\)	x=${a%%+(\\)}
				a=${a#"$x"}
				if	(( ! ( ${#a} & 1 ) ))
				then	data=$data$x$a
					break
				fi
				data=$data$x${a%?}
				;;
			*\\\})	data=$data${a%??}
				break
				;;
			*)	data=$data$a
				break
				;;
			esac
		done
		case $data in
		*\\[n\*]*)
			a= i=0
			while	:
			do	c=${data:i++:1}
				case $c in
				"")	break
					;;
				\\)	x=${data:i:1}
					case $x in
					[n\*])	(( i++ ))
						c=${data:i++:1}
						case $c in
						\()	c=${data:i:2}
							(( i += 2 ))
							;;
						\[)	c=${data:i}
							c=${c%%]*}
							(( i += ${#c} + 1 ))
							x='*'
							;;
						esac
						case $x in
						n)	a=$a${nr[$c]} ;;
						*)	a=$a${ds[$c]} ;;
						esac
						continue
						;;
					\\)	a=$a$c
						while	:
						do	c=${data:i++:1}
							case $c in
							\\)	;;
							*)	break ;;
							esac
							a=$a$c
						done
						;;
					esac
					;;
				esac
				a=$a$c
			done
			data=$a
			;;
		esac
		case $data in
		.?*)	case $data in
			*[\"]*)
				unset v
				a= i=0 n=0 q=
				while	:
				do	c=${data:i++:1}
					case $c in
					"")	break ;;
					esac
					case $c in
					$q)	q=
						case $c in
						\>)	;;
						*)	continue
							;;
						esac
						;;
					[\"\<])	case $q in
						"")	case $c in
							\<)	q=\>
								;;
							*)	q=$c
								continue
								;;
							esac
							;;
						esac
						;;
					\ |\	)
						case $q in
						"")	case $a in
							?*)	v[n++]=$a
								a=
								;;
							esac
							continue
							;;
						esac
						;;
					esac
					a=$a$c
				done
				case $a in
				?*)	v[n++]=$a ;;
				esac
				set -A text -- "${v[@]}"
				;;
			*)	set -A text -- $data
				;;
			esac
			case ${text[0]} in
			.el|.ie|.if)
				set -- "${text[@]}"
				shift
				x=$1
				shift
				case ${text[0]} in
				.e*)	if	(( nest <= 0 ))
					then	warning "unmatched ${text[0]}"
						n=0
					else	n=$(( ! ${cond[nest--]} ))
					fi
					;;
				.i*)	case $x in
					!*)	x=${x#?}
						n=1
						;;
					*)	n=0
						;;
					esac
					case $x in
					t|\'@(*)\'\1\'|+([\-+0123456789])=\1)
						(( n = ! n ))
						;;
					+([\-+0123456789])=+([\-+0123456789]))
						;;
					[0123456789]*[0123456789])
						(( n = $x ))
						;;
					esac
					case ${text[0]} in
					.ie)	cond[++nest]=$n ;;
					esac
					;;
				esac
				if	(( ! n ))
				then	case $@ in
					\\\{*)	while	read -r -u$fd data
						do	(( line++ ))
							case $data in
							*\\\})	break ;;
							esac
						done
						;;
					esac
					continue
				fi
				set -A text -- "$@"
				case ${text[0]} in
				\\\{*)	text[0]=${text[0]#??} ;;
				esac
				;;
			.so)	x=${text[1]}
				for d in "${dirs[@]}"
				do	if	[[ -f "$d$x" ]]
					then	(( fd = so + soff ))
						tmp=/tmp/m2h$$
						getfiles "$d$x" > $tmp
						eval exec $fd'< $tmp'
						rm $tmp
						so_file[so]=$file
						file=$d$x
						so_line[so]=$line
						(( line = 0 ))
						(( so++ ))
						continue 2
					fi
				done
				warning "$x: $op cannot read"
				continue
				;;
			.xx)	data=
				set -- "${text[@]}"
				shift
				while	:
				do	case $# in
					0)	break ;;
					esac
					nam=${1%%=*}
					case $nam in
					no?*)	nam=${nam#no}
						val=0
						;;
					*)	val=${1#*=}
						[[ $val ]] || val=1
						;;
					esac
					shift
					case $nam in
					begin|end)
						set -- $val
						case $nam in
						begin)	upper=$1 ;;
						end)	upper=/$1 ;;
						esac
						shift
						case $# in
						0)	val="<!--${upper}-->" ;;
						*)	val="<!--${upper} $@-->" ;;
						esac
						if	(( ident ))
						then	print -r -- "$val"
						else	meta="$meta$nl$val"
						fi
						;;
					index)	case $val in
						0)	labels=-1 ;;
						*)	labels=0 ;;
						esac
						;;
					label|link*|ref)
						case $val in
						*'	'*)
							url=${val%%'	'*}
							txt=${val#*'	'}
							;;
						*'\\t'*)
							url=${val%%'\\t'*}
							txt=${val#*'\\t'}
							;;
						*)	url=$val
							txt=$val
							;;
						esac
						case $url in
						*[:/.]*)	pfx= ;;
						*)		pfx='#' ;;
						esac
						case $url in
						*'${html.'*'}'*)
							eval url=\"$url\"
							;;
						esac
						case $nam in
						label)	if	(( labels >= 0 ))
							then	nam=name
								txt=${txt%%*([-.,])}
								level[label]=$last_level
								label[labels++]=$txt
								print -r -- "<A $nam=\"$url\">$txt</A>"
							fi
							;;
						link*)	tar=
							case $nam in
							link)	case $frame$top$vg in
								?*)	case $url in
									*([abcdefghijklmnopqrstuvwxyz]):*|/*)
										tar=" target=_top"
										;;
									esac
									;;
								esac
								;;
							esac
							nam=href
							if	[[ $frame != '' && $title == '' ]]
							then	[[ -f $framebody ]] && rm $framebody
								framelink=$pfx$url
							else	data="$data	<A $nam=\"$pfx$url\"$tar>$txt</A>"
							fi
							;;
						ref)	case $txt in
							$url)	x="<LINK href=\"$url\">" ;;
							*)	x="<LINK href=\"$url\" type=\"$txt\">" ;;
							esac
							case $framelink in
							'')	meta="$meta$nl$x" ;;
							*)	framerefs="$framerefs$nl$x" ;;
							esac
							;;
						esac
						;;
					meta.*)	meta="$meta$nl<META name=\"${nam#*.}\" content=\"$val\">"
						;;
					logo)	eval html.$nam.src='$'val
						;;
					ident|labels|logo*|title|[ABCDEFGHIJKLMNOPQRSTUVWXYZ]*)
						eval html.$nam='$'val
						;;
					text)	data="$data	$val"
						;;
					*)	eval license.$nam='$'val
						;;
					esac
				done
				case $data in
				'')	continue ;;
				esac
				set -A text -- "$data"
				break
				;;
			esac
			case ${text[0]} in
			@($primary))
				: primary macros take precedence
				;;
			.[BILMRUX]?([BILMRUX])|.F|.FR|.MW|.RF)
				case $macros:${text[0]} in
				mm:.RF)	break ;;
				esac
				typeset font1 font2 op
				set -- "${text[@]}"
				op=$1
				shift
				case $op in
				.[BIL]R)case $#:$2 in
					2':('[0123456789]*')'*([,.?!:;]))
						x=${2#'('*')'}
						y=${2%$x}
						z=${y#'('}
						z=${z%')'}
						case $op in
						.B*)	font1=STRONG ;;
						.L*)	font1=TT ;;
						*)	font1=EM ;;
						esac
						case $macros in
						man)	set -A text -- "<NOBR><A href=\"../man$z/$1.html\"><$font1>$1</$font1></A>$y$x</NOBR>" ;;
						*)	set -A text -- "<NOBR><A href=\"${html.man:=../man}/man$z/$1.html\"><$font1>$1</$font1></A>$y$x</NOBR>" ;;
						esac
						break
						;;
					esac
					;;
				.F)	op=.L ;;
				.FR)	op=.LR ;;
				.MW)	op=.L ;;
				.RF)	op=.RL ;;
				esac
				case $#:$macros:$op in
				0:*)	;;
				*:man:.?)
					set -- "$*"
					;;
				esac
				case $# in
				0)	getline
					set -- "$*"
					;;
				esac
				set -- "$indent""$@"
				case $font in
				"")	data=
					;;
				?*)	data="</$font>"
					font=
					;;
				esac
				font1=${op#.}
				case $font1 in
				?)	font2=R
					;;
				*)	font2=${font1#?}
					font1=${font1%?}
					;;
				esac
				case $font1 in
				B)	font1=STRONG ;;
				I)	font1=EM ;;
				[LMX])	font1=TT ;;
				R)	font1= ;;
				esac
				case $font2 in
				B)	font2=STRONG ;;
				I)	font2=EM ;;
				[LMX])	font2=TT ;;
				R)	font2= ;;
				esac
				font=$font2
				while	:
				do	case $# in
					0)	break ;;
					esac
					case $font in
					$font2)	font=$font1 ;;
					*)	font=$font2 ;;
					esac
					case $1 in
					"")	;;
					*"<FONT SIZE"*)
						case $font in
						"")	data="$data$1" ;;
						*)	data="$data<$font>$1</$font>" ;;
						esac
						;;
					*)	case "$1 $2" in
						*"<FONT SIZE"*)
							case $font in
							"")	data="$data$1 $2" ;;
							*)	data="$data<$font>$1 $2</$font>" ;;
							esac
							shift
							;;
						*)	case $font in
							"")	data="$data$1" ;;
							*)	data="$data<$font>$1</$font>" ;;
							esac
							;;
						esac
						;;
					esac
					shift
				done
				font=
				set -A text -- $data
				;;
			.PD)	case $1 in
				0)	pd= ;;
				*)	pd='<P>' ;;
				esac
				continue
				;;
			.PF|.PH)continue
				;;
			.SB)	set -- "${text[@]}"
				shift
				case $# in
				0)	getline ;;
				esac
				set -A text -- "<FONT SIZE=-2><B>""$@""</B></FONT>"
				;;
			.SG)	continue
				;;
			.SM)	set -- "${text[@]}"
				shift
				case $# in
				0)	getline ;;
				esac
				set -A text -- "<FONT SIZE=-2>""$@""</FONT>"
				;;
			*)	x=${map[${text[0]}]}
				case $x in
				?*)	text[0]=$x ;;
				esac
				;;
			esac
			;;
		*)	set -A text -- "$data"
			;;
		esac
		break
	done
	trap 'set -- "${text[@]}"' 0
}

function ident
{
	ident=1
	case $frame in
	'')	print -r -- "${html.magic.plain}" ;;
	*)	print -r -- "${html.magic.frame}" ;;
	esac
	print -r -- "<HTML>"
	print -r -- "<HEAD>"
	print -r -- "<META name=\"generator\" content=\"$version\">${meta}"
}

indexed=

function index
{
	if	[[ ! $indexed ]]
	then	indexed=1
		if	[[ ! $frame && ${html.labels} ]]
		then	redirect_old=9 redirect_new=8
			eval "exec $redirect_old>&1"
			t=/tmp/m2hl$$
			eval "exec 1>$t"
			eval "exec $redirect_new<$t"
			rm -f $t
		else	print -r -- "<!--INDEX--><!--/INDEX-->"
		fi
	fi
}

function title
{
	ident
	case $header in
	?*)	print -r -- "$header" ;;
	esac
	print -r -- "<TITLE>" $* "</TITLE>"
	case ${license.author} in
	?*)	print -r -- "<META name=\"author\" content=\"${license.author}\">" ;;
	esac
	case $vg in
	?*)	print -r -- "<SCRIPT language='javascript' src='../lib/slide.js'></SCRIPT>"
		case $pages in
		?*)	print -r -- "<SCRIPT language='javascript'>last_slide=$pages</SCRIPT>" ;;
		esac
		print -r -- "<STYLE>
BODY { font-family:times; font-size:$((vg_ps))pt; }
H1 { font-family:times; font-size:$((vg_ps+4))pt; }
H2 { font-family:times; font-size:$((vg_ps+2))pt; }
H3 { font-family:times; font-size:$((vg_ps))pt; }
TH { font-family:${ss}; font-size:$((vg_ps-1))pt; }
TD { font-family:${ss}; font-size:$((vg_ps-1))pt; }
#notes {
	position:relative;
	text-align:center;
	visibility:hidden;
	background:#bbbbbb;
}
#tutorial {
	position:relative;
	text-align:center;
	visibility:hidden;
	background:papayawhip;
}
</STYLE>
</HEAD>"
		return
		;;
	esac
	print -r -- "</HEAD>"
	case ${html.heading} in
	?*)	case ${html.heading} in
		?*)	html.toolbar=
			hit=
			if	[[ -f ${html.heading} ]]
			then	hit=${html.heading}
			elif	[[ -f $HOME/${html.heading} ]]
			then	hit=$HOME/${html.heading}
			else	IFS=:
				set "" $HOME $PATH
				IFS=$ifs
				for i
				do	if	[[ -f ${i%/bin}/lib/${html.heading} ]]
					then	hit=${i%/bin}/lib/${html.heading}
						break
					fi
				done
			fi
			case $hit in
			"")	print -u2 "$command: ${html.heading}: cannot read"
				code=1
				;;
			*)	eval "cat <<!
$(cat $hit)
!"
				;;
			esac
			;;
		esac
		;;
	*)	print -r -- "<BODY" ${html.BODY/'('@(*)')'/\1} ">"
		case $macros:${html.width} in
		man:*|*:)
			;;
		?*)	case ${html.width} in
			*%)	x="align=center " ;;
			*)	x="" ;;
			esac
			print -r -- "<TABLE border=0 ${x}width=${html.width}>$nl<TBODY><TR><TD valign=top align=left>"
			trailer="$nl</TD></TR></TBODY></TABLE>$nl$trailer"
			;;
		esac
		case $frame in
		'')	logo=${html.logo/'('@(*)')'/\1}
			case ${html.ident}:${logo} in
			*:*"src=''"*|*:)
				;;
			1:?*)	case ${html.home.href} in
				?*)	html.home.href=${html.home.href%/*.html}/
					print -r -- "<A" ${html.home/'('@(*)')'/\1} "><IMG" ${logo} "></A>"
					;;
				*)	print -r -- "<IMG" ${logo} ">"
					;;
				esac
				;;
			esac
			;;
		esac
		if	[[ $macros != man ]]
		then	index
		fi
		;;
	esac
	case $pm in
	?*)	print -r "$pm"
		trailer="$pm$nl$trailer"
		;;
	esac
	trailer="${trailer}${nl}</BODY>"
}

function heading
{
	typeset op=$1 i o options beg end txt
	integer count

	shift
	case $op in
	.H)	case $# in
		0)	count=1 ;;
		*)	count=$1; shift ;;
		esac
		options=
		;;
	.H*|.AS)count=1
		options=" align=center"
		;;
	*)	count=2
		options=" align=center"
		;;
	esac
	case "$op $count" in
	".H"*" 1")print -n -r -- "<P><HR>" ;;
	esac
	case $* in
	"")	print -r -- "<P>"
		;;
	*)	eval o='$'{html.H$count}
		for i in $o
		do	case $i in
			align=center)
				beg="$beg<CENTER>"
				end="</CENTER>$end"
				;;
			color=*|face=*)
				beg="$beg<FONT $i>"
				end="</FONT>$end"
				;;
			esac
		done
		(( count += head ))
		print -nr -- "$beg<H$count$options>"
		txt=$*
		txt=${txt%%*([-,.])}
		txt=${txt//\&+([^\;])\;/}
		if	(( labels >= 0 && count < mark ))
		then	print -nr -- "<A name=\"$txt\">$txt</A>"
			last_level=$count
			level[labels]=$count
			label[labels++]=$txt
		else	print -nr "$txt"
		fi
		print -r -- "</H$count>$end"
		;;
	esac
}

function tbl_attributes
{
	typeset d f i u x
	typeset -F0 w
	case $1 in
	[aAcC]*)a="$a align=center" ;;
	[lL]*)	a="$a align=left" ;;
	[nN]*)	a="$a align=right char=." ;;
	[rR]*)	a="$a align=right" ;;
	esac
	case $i in
	*[wW]\(+([0-9.])*\)*)
		x=${i##*[wW]\(}
		x=${x%%\)*}
		u=${x##+([0-9.])}
		x=${x%$u}
		case $u in
		c)	(( w=x*75/2.54 )) ;;
		i)	(( w=x*75 )) ;;
		m)	(( w=x*75*12/72 )) ;;
		n)	(( w=x*75*12/72/2 )) ;;
		p)	(( w=x*75/72 )) ;;
		P)	(( w=x*75/6 )) ;;
		*)	(( w=x*75*12/72/2 )) ;;
		esac
		a="$a width=$w"
		;;
	esac
	case $i in
	*[bB]*)		b="$b<B>" e="</B>$e" ;;
	esac
	case X$i in
	*[!0-9.][iI]*)	b="$b<I>" e="</I>$e" ;;
	esac
}

if	[[ $frame != '' ]]
then	framebody=$frame.html
	exec > $framebody || exit
fi

dirs[++ndirs]=""
for i
do	if [[ -d $i ]]
	then	dirs[++ndirs]=$i/
	else	files[++nfiles]=$i
		if [[ $i == */* ]]
		then	dirs[++ndirs]=${i%/*}/
		fi
	fi
done
document="${files[@]}"

getfiles "${files[@]}" |
while	:
do	getline || {
		[[ $title != '' ]] && break
		set -- .TL
	}
	case $1 in
	.)	: comment
		;;
	.*)	op=$1
		shift
		case $op in
		.AC)	: ignore $op
			;;
		.AE)	: ignore $op
			;;
		.AF)	case $mm_AF_cur in
			"")	mm_AF_cur="<P><I>$*" ;;
			*)	mm_AF_cur="${mm_AF_cur}<BR>$*" ;;
			esac
			;;
		.AL|.[IR]S|.VL)
			case $macros:$op in
			mm:.RS)	Rf="\\u[$reference]\\d"
				references="$references$nl<DT>[$reference]<DD>"
				while	getline
				do	case $1 in
					.RF)	break ;;
					esac
					references="$references$nl$*"
				done
				(( reference++ ))
				continue
				;;
			esac
			type[++lists]=.AL
			list[lists]=DL
			print -r -- "<DL COMPACT>"
			case $op in
			.AL)	case $1 in
				'')	type[++lists]=.al
					list[lists]=OL
					print -r -- "<OL>"
					;;
				[0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ])
					type[++lists]=.al
					list[lists]=OL
					print -r -- "<OL type=\"$1\">"
					;;
				esac
				;;
			.[IR]S)	print -r -- "<DT><DD>"
				;;
			.VL)	case $1 in
				?*)	type[++lists]=.al
					list[lists]=DL
					print -r -- "<DL COMPACT>"
					;;
				esac
				;;
			esac
			;;
		.AS|.H|.HU|.SH|.SS|.ce|.CE)
			if ((nf))
			then	nf=0
				print -r -- "</PRE>"
			fi
			if ((ce))
			then	ce=0
				print -r -- "</CENTER>"
			fi
			case $hp in
			?*)	indent=${indent#$hp}
				hp=
				;;
			esac
			if	(( lists > pp ))
			then	case ${type[@]:0:lists} in
				*.[Aa][Ll]*|*.[IiTt][Pp]*)
					while	:
					do	case ${type[lists]} in
						.[Aa][Ll]|.[IiTt][Pp])
							print -rn -- "</${list[lists--]}>"
							case ${type[lists]} in
							.AL|.IP|.TP)break ;;
							esac
							;;
						*)	break
							;;
						esac
					done
					;;
				esac
			fi
			(( pp = lists ))
			print -r -- "<P>"
			end=
			case ${mm.title} in
			?*)	print -r -- "<HR>"
				case ${mm_AU}${mm.author}${mm.keywords} in
				?*)	print -r -- "<CENTER>" ;;
				esac
				for i in ${html.H1}
				do	case $i in
					align=center)
						beg="$beg<CENTER>"
						end="</CENTER>$end"
						;;
					color=*|face=*)
						beg="$beg<FONT $i>"
						end="</FONT>$end"
						;;
					esac
				done
				print -r -- "<$H>$beg${mm.title}$end</$H>"
				mm.title=
				case ${mm.author} in
				?*)	IFS=:
					set -- ${mm.author#:} : "$@"
					IFS=$ifs
					while	:
					do	case $# in
						0)	break ;;
						esac
						x=$1
						shift
						case $x in
						:)	break ;;
						esac
						print -r -- "$x<BR>"
					done
					;;
				esac
				case $mm_AU in
				?*)	print -r -- "$mm_AU"
					case $mm_AF_cur in
					?*)	mm_AF="${mm_AF_cur}</I>" ;;
					esac
					case $mm_AF in
					?*)	print -r -- "$mm_AF" ;;
					esac
					;;
				esac
				case ${mm_AU}${mm.author}${mm.keywords} in
				?*)	print -r -- "</CENTER>" ;;
				esac
				;;
			esac
			case $op in
			.AS)	print -r -- "<P>"
				heading $op Abstract
				;;
			.ce)	case $# in
				0)	count=1 ;;
				*)	count=$1 ;;
				esac
				print -r -- "<CENTER>"
				while	(( count-- > 0 )) && read -r data
				do	print -r -- "$data<BR>"
				done
				print -r -- "</CENTER>"
				;;
			.CE)	if [[ $1 != 0 ]]
				then	ce=1
					print -r -- "<CENTER>"
				fi
				;;
			.S[HS])	setmacros man
				while	(( lists > 0 ))
				do	print -r -- "</${list[lists]}>"
					case ${type[lists--]} in
					.S[HS])	break ;;
					esac
				done
				type[++lists]=$op
				list[lists]=DL
				print -r -- "<DL COMPACT><DT>"
				case $op in
				.SS)	type[++lists]=.XX
					list[lists]=DL
					print -r -- "<DL COMPACT><DT>"
					;;
				esac
				case $op in
				.SH)	if	[[ $macros == man && $* == DESCRIPTION && ${html.labels} ]]
					then	index
					fi
					heading .H 2 "$@"
					;;
				*)	heading .H 3 "$@"
					;;
				esac
				type[++lists]=.XX
				list[lists]=DL
				case $op:$* in
				.SH:NAME)	print -n -r -- "<DL COMPACT>$nl<DT>" ;;
				*)		print -r -- "<DL COMPACT><DT>" ;;
				esac
				;;
			*)	heading $op "$@"
				;;
			esac
			;;
		.AT)	: ignore $op
			;;
		.AU)	case $mm_AF_cur in
			?*)	case $mm_AF in
				?*)	case $mm_AU in
					?*)	mm_AU="${mm_AU}${mm_AF}" ;;
					esac
					;;
				esac
				mm_AF="${mm_AF_cur}</I>"
				mm_AF_cur=""
				;;
			esac
			mm_AU="${mm_AU}<BR>$1"
			;;
		.BL|.bL|.sL)
			i=
			for ((n = 1; n <= lists; n++))
			do	i=$i${list[n]}
			done
			case $i in
			*UL*UL*)i=disc ;;
			*UL*)	i=circle ;;
			*)	i=square ;;
			esac
			type[++lists]=.AL
			list[lists]=UL
			print -r -- "<UL type=$i>"
			;;
		.BP)	unset parm
			while	[[ $1 == *=* ]]
			do	eval parm="( ${1%%=*}='${1#*=}' )"
				shift
			done
			unset oparm
			oparm=$parm
			i=$1
			if	[[ $i == *.@(gif|png) ]]
			then	for i
				do	f=
					for d in "${dirs[@]}"
					do	if [[ -f "$d$i" ]]
						then	f=$d$i
							break
						fi
					done
					if [[ ! $f ]]
					then	print -u2 "$command: warning: $i: data file not found"
					fi
					if	[[ ! ${oparm.alt} ]]
					then	u=${i##*/}
						u=${i%.*}
						parm=( alt=$u )
					fi
					if	[[ ! ${oparm.title} ]]
					then	u=${i##*/}
						u=${i%.*}
						if	[[ ${parm.category} ]]
						then	u="${parm.category} $u"
						elif	[[ ${oparm.category} ]]
						then	u="${oparm.category} $u"
						fi
						parm=( title=$u )
					fi
					print -r -- "<IMG src=\"$i\"" ${parm/'('@(*)')'/\1}">"
				done
			else	i=${i%.*}.gif
				case $frame in
				?*)	[[ -f $frame-$i ]] && i=$frame-$i ;;
				esac
				f=
				for d in "${dirs[@]}"
				do	if	[[ -f "$d$1" ]]
					then	f=$d$1
						break
					fi
				done
				if [[ ! $f ]]
				then	print -u2 "$command: $1: data file not found"
				elif [[ $f -nt $i ]]
				then	ps2gif $f $i
				fi
				print -r -- "<CENTER><IMG src=\"$i\"></CENTER>"
			fi
			;;
		.CT)	: ignore $op
			;;
		.DE|.dE|.fi)
			if ((nf))
			then	nf=0
				print -r -- "</PRE>"
			fi
			;;
		.DF|.DS|.dS|.nf)
			if ((!nf))
			then	nf=1
				print -r -- "<PRE>"
			fi
			;;
		.DT)	case $macros in
			man)	;;
			*)	print -r -- "${ds[Dt]}" ;;
			esac
			;;
		.EE|.eE)if ((nf))
			then	nf=0
				print -r -- "</PRE>"
			fi
			if [[ $fg ]]
			then	print -r -- "<H4 align=center>$fg</H4>"
			fi
			indent=${indent#$inch}
			;;
		.EX|.eX)if ((!nf))
			then	nf=1
				print -r -- "<PRE>"
			fi
			indent=$inch$indent
			case $# in
			2)	fg="Figure $1: $2" ;;
			*)	fg= ;;
			esac
			;;
		.FE)	print -r -- '&nbsp;]&nbsp'
			;;
		.FG)	print -r -- "<H4 align=center>Figure $figure: $*</H4>"
			(( figure++ ))
			;;
		.FS)	print -r -- '&nbsp;[&nbsp'
			;;
		.HP|.LP|.P|.PP)
			case $hp in
			?*)	indent=${indent#$hp}
				hp=
				;;
			esac
			if	(( lists > pp ))
			then	case ${type[@]:0:lists} in
				*.[Aa][Ll]*|*.[IiTt][Pp]*)
					while	:
					do	case ${type[lists]} in
						.[Aa][Ll]|.[IiTt][Pp])
							print -rn -- "</${list[lists--]}>"
							case ${type[lists]} in
							.AL|.IP|.TP)break ;;
							esac
							;;
						*)	break
							;;
						esac
					done
					;;
				esac
			fi
			(( pp = lists ))
			print -r -- "<P>"
			;;
		.HY)	: ignore $op
			;;
		.IP|.LI|.TF|.TP|.bI|.sI)
			case $macros:$op in
			mm:.TP)	continue ;;
			esac
			case $op in
			.IP|.LP|.TF|.TP)OP=$op
				case ${type[lists]} in
				$op|$OP);;
				*)	type[++lists]=$op
					list[lists]=DL
					print -r -- "$pd<DL COMPACT>"
					case $1 in
					[0123456789]*)
						type[++lists]=.tp
						list[lists]=DL
						print -r -- "<DL COMPACT>"
						;;
					esac
					;;
				esac
				case $op in
				.IP|.LP|.TF)
					set -- "<TT>$*</TT>"
					;;
				.TP)	getline
					;;
				esac
				;;
			esac
			case ${list[lists]} in
			"")	warning "$op: no current list"
				;;
			DL)	case $# in
				0)	getline ;;
				esac
				print -r -- "<DT>$*<DD>"
				;;
			*)	case $op in
				.bI|.sI)print -r -- "<P>" ;;
				esac
				print -r -- "<LI>$*"
				;;
			esac
			;;
		.IX)	: ignore $op
			;;
		.LE|.[IR]E|.bE|.sE)
			case ${type[@]} in
			*.[Aa][Ll]*)
				while	(( lists > 0 ))
				do	print -r -- "</${list[lists]}>"
					case ${type[lists--]} in
					.AL)	break ;;
					esac
				done
				;;
			*)	warning "$op: no current list type"
				;;
			esac
			case $op:$pd in
			.[IR]E:?*)	print -r -- "$pd" ;;
			esac
			;;
		.LX)	: ignore $op
			;;
		.MT)	setmacros mm
			;;
		.ND|.Dt)ds[Dt]=$*
			;;
		.NL)	type[++lists]=.AL
			list[lists]=OL
			print -r -- "<OL>"
			;;
		.OK)	mm.keywords="$*"
			;;
		.OP)	: .OP opt arg arg-append arg-prepend
			x="$4<STRONG>&#45;$1</STRONG><I>$2</I>"
			case $3 in
			'[]')	x="[ $x ]" ;;
			?*)	x="$x$3" ;;
			esac
			print -r -- "$x"
			;;
		.PM|.pM)case ${html.company} in
			'')	pm= ;;
			*)	pm="${html.company//\&/&amp\;} " ;;
			esac
			case $1 in
			'')	pm= ;;
			C|NDA)	pm="${pm}CONFIDENTIAL" ;;
			RG)	pm="${pm}PROPRIETARY (REGISTERED)" ;;
			RS)	pm="${pm}PROPRIETARY (RESTRICTED)" ;;
			*)	pm="${pm}PROPRIETARY" ;;
			esac
			case $pm in
			?*)	case $op in
				.pM)	pm="<TABLE align=center cellpadding=2 border=4 bgcolor=lightgrey><TR><TD><FONT face=\"${ss}\"><B>${pm}</B></FONT></TD></TR></TABLE>" ;;
				*)	pm="<HR><CENTER><$H>${pm}</$H></CENTER>" ;;
				esac
				;;
			esac
			;;
		.PU)	: ignore $op
			;;
		.SA)	: ignore $op
			;;
		.SU)	: ignore $op
			;;
		.TH|.TL): .TL junk junk
			: .TH item section foot_center foot_left head_center
			case $macros:$op in
			:.TH)	setmacros man ;;
			:.TL)	setmacros mm ;;
			esac
			case ${html.title} in
			?*)	title=${html.title}
				;;
			*)	case $title in
				'')	title="$document $macros document" ;;
				esac
				title="$package $title"
				;;
			esac
			title $title
			case $op in
			.TH)	case $3 in
				?*)	dc[++dcs]=$3 ;;
				esac
				case $4 in
				?*)	dl[++dls]=$4 ;;
				esac
				case $5 in
				'')	case $2 in
					1*)	sec="USER COMMANDS " ;;
					2*)	sec="SYSTEM CALLS" ;;
					3C)	sec="COMPATIBILITY FUNCTIONS" ;;
					3F)	sec="FORTRAN LIBRARY ROUTINES" ;;
					3K)	sec="KERNEL VM LIBRARY FUNCTIONS" ;;
					3L)	sec="LIGHTWEIGHT PROCESSES LIBRARY" ;;
					3M)	sec="MATHEMATICAL LIBRARY" ;;
					3N)	sec="NETWORK FUNCTIONS" ;;
					3R)	sec="RPC SERVICES LIBRARY" ;;
					3S)	sec="STANDARD I/O FUNCTIONS" ;;
					3V)	sec="SYSTEM V LIBRARY" ;;
					3X)	sec="MISCELLANEOUS LIBRARY FUNCTIONS" ;;
					3*)	sec="C LIBRARY FUNCTIONS" ;;
					4*)	sec="DEVICES AND NETWORK INTERFACES" ;;
					4F)	sec="PROTOCOL FAMILIES" ;;
					4P)	sec="PROTOCOLS" ;;
					4*)	sec="DEVICES AND NETWORK INTERFACES" ;;
					5*)	sec="FILE FORMATS" ;;
					6*)	sec="GAMES AND DEMOS" ;;
					7*)	sec="PUBLIC FILES AND TABLES" ;;
					8*)	sec="MAINTENANCE COMMANDS" ;;
					L*)	sec="LOCAL COMMANDS" ;;
					*)	sec="SECTION $2" ;;
					esac
					;;
				*)	sec=$5
					;;
				esac
				MAN="$1($2)"
				print -r -- "<A name=\"$MAN\"></A>"
				print -r -- "<H3><TABLE width=100%><TBODY><TR><TH align=left>$MAN</TH><TH align=center><A href=\".\" title=\"Index\" target=\"_top\">$sec</A></TH><TH align=right>$MAN</TH></TR></TBODY></TABLE></H3>"
				print -r -- "<HR>"
				;;
			.TL)	getline || break
				case ${html.title} in
				'')	mm.title=$* ;;
				esac
				;;
			esac
			;;
		.TM)	: ignore $op
			;;
		.TS)	# undent ...

	case $1 in
	H)	tbl_tmp=/tmp/m2h$$tbl
		(( tbl_fd=3 ))
		trap "rm -f $tbl_tmp" 0 1 2 3
		eval exec "$tbl_fd>$tbl_tmp"
		;;
	esac
	unset opts
	eval "opts=${html.TABLE}"
	unset opts.width opts.align
	tbl_ns=3
	tbl_sp='&nbsp;&nbsp;'
	tab=$'\t'
	(( augment=0 ))
	while	:
	do	(( row=0 ))
		while	getline
		do	set -- ${@//[-\|_=]/ }
			case $# in
			0)	continue ;;
			esac
			case $1 in
			'.T&'*)	continue
				;;
			.TE)	if (( tbl_fd != 1 ))
				then	warning ".TS H without .TH"
					eval "exec $tbl_fd>&-"
					(( tbl_fd=1 ))
					rm -f $tbl_tmp
					tbl_tmp=
					trap - 0 1 2 3
					if (( augment ))
					then	print -r -u$tbl_fd -- "</THEAD><TBODY>"
					fi
				fi
				continue 3
				;;
			esac
			(( row++ ))
			tbl[row]=${@//..*/.}
			case $* in
			*";"*)	(( row=0 ))
				for i in ${@//[,\;]/ }
				do	case $i in
					center)	opts.align=center
						;;
					expand)	opts.align=center opts.width="99%"
						;;
					allbox)	opts.border=1
						opts.frame="box"
						opts.rules="all"
						;;
					box)	opts.border=
						opts.frame="void"
						opts.rules="none"
						;;
					doublebox)
						opts.border=2
						opts.frame="box"
						opts.rules="none"
						;;
					linesize'('*')')
						opts.border=${i//*'('@(*)')'*/\1}
						;;
					tab'('*')'*)
						tab=${i//*'('@(*)')'*/\1}
						;;
					tab'(')	case $* in
						*'tab(,'*)	tab=',' ;;
						*'tab(;'*)	tab=';' ;;
						*'tab( '*)	tab=' ' ;;
						*)		tab=$'\t' ;;
						esac
						;;
					esac
				done
				;;
			*".")	break
				;;
			esac
		done
		case ${opts.border} in
		0)	opts.cellpadding=0
			opts.cellspacing=0
			unset opts.bgcolor
			;;
		*)	case ${opts.cellpadding} in
			'')	opts.cellpadding=2 ;;
			esac
			case ${opts.cellspacing} in
			'')	opts.cellspacing=2 ;;
			esac
			;;
		esac
		case ${opts.border} in
		'')	opts.border=0 ;;
		esac
		if ((!augment))
		then	(( augment=1 ))
			print -r -- "<P></P><TABLE border=0 frame=void rules=none width=100%><TBODY><TR><TD>$nl<TABLE" ${opts/'('@(*)')'/\1} ">"
			if (( tbl_fd == 3 ))
			then	print -r -u$tbl_fd -- "<THEAD>"
			else	print -r -u$tbl_fd -- "<TBODY>"
			fi
		fi
		for ((n = 1; n < row; n++))
		do	getline || break
			case $1 in
			[_=]*)	((n--)); continue ;;
			esac
			print -rn -u$tbl_fd -- "<TR>"
			IFS=$tab
			set -- $*
			IFS=$ifs
			set -A Q ${tbl[n]}
			(( q=0 ))
			(( s=1 ))
			while	:
			do	i=${Q[q++]}
				case $i in
				''|.)	break
					;;
				*[0-9]*)x=${i//[pvw]*([()0-9.inm])/}
					x=${i//[[:alpha:]]/}
					case $x in
					+([0-9]))
						if (( $x > tbl_ns ))
						then	tbl_ns=$x
							tbl_sp=''
							for ((m=(tbl_ns+1)/2; m>0; m--))
							do	tbl_sp=${tbl_sp}'&nbsp;'
							done

						fi
					esac
					;;
				esac
				a=
				case $q in
				1)		b='' ;;
				*)		b=$tbl_sp ;;
				esac
				case ${Q[q]}:$# in
				:*|*:[01])	e='' ;;
				*)		e=$tbl_sp ;;
				esac
				case $i in
				[sS]*)	(( s++ )); continue ;;
				esac
				tbl_attributes "$i"
				while	[[ ${Q[q]} == *s* ]]
				do	(( s++ ))
					(( q++ ))
				done
				if	(( s > 1 ))
				then	a="$a colspan=$s"
					(( s=1 ))
				fi
				v=$1
				case $i in
				*'*'*)	case $v in
					*'=>'*)	v="<A href=\"${v##*'=>'}\">${v%'=>'}</A>" ;;
					*)	v="<A href=\"$v\">$v</A>" ;;
					esac
				esac
				print -rn -u$tbl_fd -- "<TD$a>$b$v$e</TD>"
				case $# in
				0|1)	break ;;
				esac
				shift
			done
			print -r -u$tbl_fd -- "</TR>"
		done
		prev=
		set -A Q ${tbl[n]}
		while	getline
		do	case $1 in
			'.T&'*)	(( augment=1 ))
				continue 2
				;;
			.TH*)	if (( tbl_fd != 1 ))
				then	print -r -u$tbl_fd -- "</THEAD>"
					if (( tbl_no == 1 || $# == 1 )) || [[ $2 != N ]]
					then	cat $tbl_tmp
					fi
					eval "exec $tbl_fd>&-"
					(( tbl_fd=1 ))
					rm -f $tbl_tmp
					tbl_tmp=
					trap - 0 1 2 3
				fi
				print -r -u$tbl_fd -- "<TBODY>"
				continue
				;;
			.TE)	(( tbl_no++ ))
				if (( tbl_fd != 1 ))
				then	warning ".TS H without .TH"
					eval "exec $tbl_fd>&-"
					(( tbl_fd=1 ))
					rm -f $tbl_tmp
					tbl_tmp=
					trap - 0 1 2 3
					print -r -u$tbl_fd -- "</THEAD><TBODY>"
				fi
				break
				;;
			esac
			IFS=$tab
			set -- $*
			IFS=$ifs
			case $* in
			*"\\")	prev=$prev$'\n'${*/"\\"/}
				;;
			*)	print -rn -u$tbl_fd -- "<TR>"
				IFS=$'\t'
				set -- $prev$'\n'$*
				IFS=$ifs
				case $* in
				$'\n_')	rule=1
					set -- '_'
					;;
				$'\n=')	rule=1
					set -- '='
					;;
				*)	rule=0
					;;
				esac
				(( q=0 ))
				while	:
				do	i=${Q[q++]}
					case $i in
					''|.)	break
						;;
					*[0-9]*)x=${i//[pvw]*([()0-9.inm])/}
						x=${i//[[:alpha:]]/}
						case $x in
						+([0-9]))
							if (( $x > tbl_ns ))
							then	tbl_ns=$x
								tbl_sp=''
								for ((m=(tbl_ns+1)/2; m>0; m--))
								do	tbl_sp=${tbl_sp}'&nbsp;'
								done

							fi
						esac
						;;
					esac
					a=
					case $q in
					1)		b='' ;;
					*)		b=$tbl_sp ;;
					esac
					case ${Q[q]}:$# in
					:*|*:[01])	e='' ;;
					*)		e=$tbl_sp ;;
					esac
					case $1 in
					$'\n_'|'_')
						print -rn -u$tbl_fd -- "<TD><HR width=100% size=2 noshade></TD>"
						;;
					$'\n='|'=')
						print -rn -u$tbl_fd -- "<TD><HR width=100% size=4 noshade></TD>"
						;;
					*)	tbl_attributes "$i"
						print -rn -u$tbl_fd -- "<TD$a>$b$1$e</TD>"
						;;
					esac
					((rule)) && continue
					case $# in
					0|1)	break ;;
					esac
					shift
				done
				print -r -u$tbl_fd -- "</TR>"
				prev=
				;;
			esac
		done
		break
	done
	print -r -- "</TBODY></TABLE></TD></TR></TBODY></TABLE>"

			# ... indent
			;;
		.TX)	: ignore $op
			;;
		.UC)	: ignore $op
			;;
		.VE)	: ignore $op
			;;
		.VS)	: ignore $op
			;;
		.al)	: ignore $op
			;;
		.bd)	: ignore $op
			;;
		.br)	print -r -- "<BR>"
			;;
		.de|.am.ig)
			end=..
			case $#:$op in
			0:*)	;;
			*:.ig)	end=$1 ;;
			esac
			: ignore $op to $end
			while	getline
			do	case $1 in
				$end)	break ;;
				esac
			done
			;;
		.ds)	op=$1
			shift
			ds[$op]=$*
			;;
		.f[tBILPR])
			case $op in
			.f[BILPR])
				set -- ${op#.f}
				;;
			esac
			case $1 in
			5|TT)	font=TT
				print -n -r -- "<$font>"
				;;
			[BI])	font=$1
				print -n -r -- "<$font>"
				;;
			*)	case $font in
				?*)	print -n -r -- "</$font>"
					font=
					;;
				esac
				;;
			esac
			;;
		.fp)	: ignore $op
			;;
		.hc)	: ignore $op
			;;
		.hw)	: ignore $op
			;;
		.hy)	: ignore $op
			;;
		.in)	indent_prev=$indent
			case $1 in
			"")	i=$indent_prev; indent_prev=$indent; indent=$i ;;
			-*)	indent=${indent#$inch} ;;
			+*)	indent=$inch$indent ;;
			*)	indent=$inch ;;
			esac
			;;
		.lf)	: ignore $op
			;;
		.ll)	: ignore $op
			;;
		.ne)	: ignore $op
			;;
		.nh)	: ignore $op
			;;
		.nr)	op=$1
			shift
			nr[$op]=$*
			;;
		.ns)	: ignore $op
			;;
		.ps|.pS)case $1 in
			[-+][0123456789])
				print -r -- "<FONT SIZE=$1>"
				;;
			esac
			;;
		.sh)	case $HTMLPATH in
			'')	;;
			*)	(( fd = so + soff ))
				file=/tmp/m2h$$
				path=$PATH
				eval PATH=$HTMLPATH "$*" > $file
				PATH=$path
				eval exec $fd'< $file'
				rm $file
				so_file[so]=$file
				file=$1
				so_line[so]=$line
				(( line = 0 ))
				(( so++ ))
				;;
			esac
			;;
		.sn)	for d in "${dirs[@]}"
			do	if	[[ -f "$d$1" ]]
				then	cat "$d$1"
					continue 2
				fi
			done
			warning "$1: $op cannot read"
			;;
		.sp|.SP)case $1 in
			[0123456789]*)
				count=$1
				while	(( count >= 0 ))
				do	(( count-- ))
					print -r -- "<BR>"
				done
				;;
			*)	print -r -- "<P>"
				;;
			esac
			;;
		.ta)	: ignore $op
			;;
		.ti)	: ignore $op
			;;
		.ul)	: ignore $op
			;;
		.vG)	vg=$1
			inch=" "
			set +o noglob
			rm -f [0123456789][0123456789].html index.html outline.html
			set -o noglob
			exec > $page.html
			outline[$page]=$2
			header='<BASEFONT face="geneva,arial,helvetica" size=5>'
			title "${2//\<*([!>])\>/}"
			print "<BODY bgcolor='#ffffff'>"
			print "${pm}<CENTER>"
			print "<BR><H1><FONT color=red>$2</FONT></H1><BR>"
			case $# in
			0|1|2)	;;
			*)	shift 2
				for name
				do	[[ $name == "-" ]] && name=""
					print -- "<BR>$name"
				done
				;;
			esac
			print "</CENTER>"
			print "<P>"
			;;
		.vH)	print -r -- "<CENTER>
<BR>
<H1><FONT color=red> $1 </FONT></H1>
<BR>
</CENTER>"
			;;
		.vP)	while	(( lists > 0 ))
			do	print -r -- "</${list[lists--]}>"
			done
			print -r -- "${trailer}
</HTML>"
			((page++))
			exec > $page.html
			outline[$page]=$1
			trailer=
			title "${1//\<*([!>])\>/}"
			print "<BODY bgcolor='#ffffff'>"
			print "${pm}<CENTER>"
			print "<BR><H1><FONT color=red>$1</FONT></H1>"
			print "</CENTER>"
			print "<P>"
			;;
		.nS)	print -r -- "<DIV id='notes'>"
			;;
		.tS)	print -r -- "<DIV id='tutorial'>"
			;;
		.nE|.tE)print -r -- "</DIV>"
			;;
		*)	warning "$op: unknown op"
			;;
		esac
		;;
	""|[\ \	]*)
		case $macros in
		man)	leading=1
			prefix="<TT><PRE>"
			blank=
			while	:
			do	case $1 in
				*([\ \	]))
					case $leading in
					1)	leading= ;;
					*)	blank=$'\n' ;;
					esac
					;;
				[\ \	]*)
					print -r -- "$prefix$blank$inch$indent$*"
					blank=
					leading=
					prefix=
					;;
				*)	(( peek = 1 ))
					break
					;;
				esac
				getline || break
			done
			case $prefix in
			"")	print -r -- "</PRE></TT>" ;;
			esac
			;;
		*)	print -r -- "$indent$*"
			;;
		esac
		;;
	*)	print -r -- "$indent$*"
		;;
	esac
done
while	(( lists > 0 ))
do	print -r -- "</${list[lists--]}>"
done
case $references in
?*)	heading .H 1 References
	print "<P>"
	print "<DL compact>"
	print -r -- "$references"
	print "</DL>"
	;;
esac
print -r -- "<P>"
print -r -- "<HR>"
print -r -- "<TABLE border=0 align=center width=96%>"
integer dls=0 dcs=0 drs=0
typeset d dl dc dr
case ${html.ident} in
1)	case ${license.author} in
	?*)	IFS=',+'
		set -- ${license.author}
		IFS=$ifs
		d=""
		sp=""
		for a
		do	v=${contributor[$a]}
			case $v in
			?*)	a=$v ;;
			esac
			IFS='<>'
			set -- $a
			IFS=$ifs
			case $2 in
			?*)	d="$d$sp<A href=\"mailto:$2?subject=$title\">"
				sp=", "
				set -- $1
				d="$d$*</A>"
				;;
			esac
		done
		[[ $d ]] && dr[++drs]=$d
		;;
	*)	case ${html.MAILTO} in
		?*)	dr[++drs]="<A href=\"mailto:${html.MAILTO}?subject=$title\">${html.MAILTO}</A>" ;;
		esac
		;;
	esac
	for i in "${license.organization}" "${license.corporation} ${license.company}" "${license.address}" "${license.location}" "${license.phone}"
	do	case $i in
		''|' ')	;;
		*)	dr[++drs]="${i//\&/&amp\;}" ;;
		esac
	done
	;;
esac
dr[++drs]="${ds[Dt]}"
(( drs < dls )) && (( drs = dls ))
for (( dls = 1; dls <= drs; dls++ ))
do	print -r -- "<TR>$nl<TD align=left>${dl[dls]}</TD>$nl<TD align=center>${dc[dls]}</TD>$nl<TD align=right>${dr[dls]}</TD>$nl</TR>"
done
print -r -- "</TABLE>"
print -r -- "<P>"
case ${html.footing} in
?*)	html.toolbar=
	hit=
	if	[[ -f ${html.footing} ]]
	then	hit=${html.footing}
	elif	[[ -f $HOME/${html.footing} ]]
	then	hit=$HOME/${html.footing}
	else	IFS=:
		set "" $HOME $PATH
		IFS=$ifs
		for i
		do	if	[[ -f ${i%/bin}/lib/${html.footing} ]]
			then	hit=${i%/bin}/lib/${html.footing}
				break
			fi
		done
	fi
	case $hit in
	"")	print -u2 "$command: ${html.footing}: cannot read"
		code=1
		;;
	*)	eval "cat <<!
$(cat $hit)
!"
		;;
	esac
	;;
esac
case ${html.toolbar} in
?*)	hit=
	if	[[ -f ${html.toolbar} ]]
	then	hit=${html.toolbar}
	elif	[[ -f $HOME/${html.toolbar} ]]
	then	hit=$HOME/${html.toolbar}
	else	IFS=:
		set "" $HOME $PATH
		IFS=$ifs
		for i
		do	if	[[ -f ${i%/bin}/lib/${html.toolbar} ]]
			then	hit=${i%/bin}/lib/${html.toolbar}
				break
			fi
		done
	fi
	case $hit in
	"")	print -u2 "$command: ${html.toolbar}: cannot read"
		code=1
		;;
	*)	eval "cat <<!
$(cat $hit)
!"
		;;
	esac
	;;
esac
trailer="$trailer$nl</HTML>"
print -r -- "$trailer"
case $frame in
?*)	if	[[ $framelink == '' && ${html.labels} != '' ]] && (( labels > 1 ))
	then	if	[[ $macros == man ]]
		then	mv $framebody $frame-man.html
			{
				ident
				print -r -- "<TITLE>$file table of contents</TITLE>"
				print -r -- "</HEAD>"
				print -r -- "<BODY" ${html.BODY/'('@(*)')'/\1} ">"
				print -r -- "<H3><TABLE width=100%><TBODY><TR><TH align=left><A target=\"man\" href=\"$frame-man.html#$MAN\">$MAN</A></TH><TH align=right><A href=\".\" target=_top title=\"Command Index.\">COMMANDS</A></TH></TR></TBODY></TABLE></H3>"
				print -r -- "<HR>"
				print -r -- "<UL>"
				(( lev = level[0] ))
				for ((n = 0; n < labels; n++))
				do	while	(( lev < level[n] ))
					do	(( lev++ ))
						print -r -- "<UL>"
					done
					while	(( lev > level[n] ))
					do	(( lev-- ))
						print -r -- "</UL>"
					done
					print -r -- "<LI><A href=\"$frame-man.html#${label[n]}\" target=\"man\">${label[n]}</A>"
				done
				while	(( lev > level[0] ))
				do	(( lev-- ))
					print -r -- "</UL>"
				done
				print -r -- "</UL>"
				print -r -- "</BODY>"
				print -r -- "</HTML>"
			} > $frame-toc.html
			{
				ident
				print -r -- "<TITLE>$file frame sets</TITLE>$nl</HEAD>"
				print -r -- "<BASE>"
				print -r -- "</HEAD>"
				print -r -- "<FRAMESET cols=\"20%,80%\">"
				print -r -- "<FRAME src=\"$frame-toc.html\" name=\"toc\">"
				print -r -- "<FRAME src=\"$frame-man.html\" name=\"man\">"
				print -r -- "</FRAMESET>"
				print -r -- "</HTML>"
			} > $framebody
		else	exec > $frame-temp.html || exit
			print -r -- "<B><FONT size=-1 face=\"${ss}\">"
			print -r -- "<TABLE align=center cellpadding=2 border=4 bgcolor=lightgrey><TR>"
			for ((n = 0; n < labels; n++))
			do	print -r -- "<TD><A href=\"#${label[n]}\">${label[n]}</A></TD>"
			done
			print -r -- "</TR></TABLE>"
			print -r -- "</FONT></B>"
			exec > /dev/null || exit
			ed $framebody <<!
/<!--INDEX-->/r $frame-temp.html
w
q
!
			rm $frame-temp.html
		fi
	fi
	case $index in
	?*)	case $index in
		local)	exec > $frame-index.html || exit ;;
		global)	exec > index.html || exit ;;
		esac
		unset html.FRAMESET.rows html.FRAMESET.cols
		ident
		case $framelink in
		'')	framelink=$frame.html ;;
		esac
		print -r -- "<TITLE>$title</TITLE>$framerefs
</HEAD>
<FRAMESET" ${html.FRAMESET/'('@(*)')'/\1} ${html.index.top.FRAMESET/'('@(*)')'/\1} ">
	<FRAME" marginwidth=0 marginheight=0 ${html.FRAME/'('@(*)')'/\1} ${html.index.top.FRAME/'('@(*)')'/\1} "scrolling=no>
	<FRAME" marginwidth=0 marginheight=0 ${html.FRAME/'('@(*)')'/\1} src="'$framelink'" "scrolling=auto>
</FRAMESET>
</HTML>"
		;;
	esac
	;;
*)	if	[[ $redirect_old ]]
	then	eval "exec 1>&$redirect_old $redirect_old>&-"
	fi
	if	[[ ${html.labels} != '' ]] && (( labels > 1 ))
	then
		if	[[ ${html.labels} != +([0-9]) || ${html.labels} == [01] ]]
		then	html.labels=7
		fi
		print -r -- "<!--LABELS-->"
		print -r -- "<B><FONT size=-1 face=\"${ss}\">"
		print -r -- "<TABLE align=center cellpadding=2 border=4 bgcolor=lightgrey width=90%><TR>"
		for ((n = 0; n < labels; n++))
		do	print -r -- "<TD align=center valign=center><A href=\"#${label[n]}\">${label[n]}</A></TD>"
			if (( (n + 1) < labels && !( (n + 1) % ${html.labels} ) ))
			then	print -r -- "</TR><TR>"
			fi
		done
		print -r -- "</TR></TABLE>"
		print -r -- "</FONT></B>"
		print -r -- "<!--/LABELS-->"
	fi
	if	[[ $redirect_new ]]
	then	eval "cat <&$redirect_new"
	fi
	;;
esac
case $vg in
?*)	frame=$vg
	trailer=
	pages=${page#0}
	exec > index.html
	title "${outline[01]//\<*([!>])\>/}"
	print -r -- "<FRAMESET marginheight=0 marginwidth=0 cols='135,*,66' border=0 noresize onload='goto_slide(1)'>
  <FRAME src=../lib/prev.html name='prev' noresize scrolling=no marginwidth=0 marginheight=0>
  <FRAME src=../lib/start.html name='slide' noresize scrolling=auto marginwidth=0 marginheight=0>
  <FRAME src=../lib/next.html name='next' noresize scrolling=no marginwidth=0 marginheight=0>
</FRAMESET>
</HTML>"
	exec > outline.html
	title "${title} outline"
	print -r -- "<BODY bgcolor='#ffffff'>
${pm}<CENTER>
<BR><H1><FONT color=red>outline</FONT></H1>
<P>
<TABLE cellspacing=1 cellpadding=1 border=1 class=box>"
	for ((page=1; page <= pages; page++))
	do	print "<TR><TD><A href='javascript:goto_slide(${page#0})'><CENTER>${outline[$page]}</CENTER></A></TD></TR>"
	done
	print "</TABLE>
</CENTER>
</BODY>
</HTML>"
	;;
esac
exit $code
