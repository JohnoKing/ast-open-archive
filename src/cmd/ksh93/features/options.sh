####################################################################
#                                                                  #
#             This software is part of the ast package             #
#                Copyright (c) 1982-2003 AT&T Corp.                #
#        and it may only be used by you under license from         #
#                       AT&T Corp. ("AT&T")                        #
#         A copy of the Source Code Agreement is available         #
#                at the AT&T Internet web site URL                 #
#                                                                  #
#       http://www.research.att.com/sw/license/ast-open.html       #
#                                                                  #
#    If you have copied or used this software without agreeing     #
#        to the terms of the license you are infringing on         #
#           the license and copyright and are violating            #
#               AT&T's intellectual property rights.               #
#                                                                  #
#            Information and Software Systems Research             #
#                        AT&T Labs Research                        #
#                         Florham Park NJ                          #
#                                                                  #
#                David Korn <dgk@research.att.com>                 #
#                                                                  #
####################################################################
: SHOPT_* option probe

eval $1

: check for shell magic #!
cat > /tmp/file$$ <<!
#! /bin/echo
exit 1
!
chmod 755 /tmp/file$$
if	/tmp/file$$ > /dev/null
then	echo "#define SHELLMAGIC	1"
fi
rm -f /tmp/file$$

option() # name value
{
	case $2 in
	0)	echo "#ifndef SHOPT_$1"
		echo "#   define SHOPT_$1	1"
		echo "#endif"
		;;
	*)	echo "#undef  SHOPT_$1"
		;;
	esac
}

case `pwd` in
/usr/src/*|/opt/ast/*)
	option CMDLIB_DIR 0
	;;
esac
test -d /dev/fd
option DEVFD $?
case  `echo a | tr a '\012' | wc -l` in
*1*)	option MULTIBYTE 0 ;;
esac
test -x /bin/pfexec -o -x /usr/bin/pfexec
option PFSH $?
/bin/test ! -l . 2> /dev/null
option TEST_L $?
test -f /bin/universe && univ=`/bin/universe` > /dev/null 2>&1 -a ucb = "$univ"
option UCB $?

cat <<\!
#if !_PACKAGE_ast && ( (MB_LEN_MAX-1)<=0 || !defined(_lib_mbtowc) )
#   undef SHOPT_MULTIBYTE
#endif
!
