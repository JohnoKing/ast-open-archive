########################################################################
#                                                                      #
#               This software is part of the ast package               #
#          Copyright (c) 1989-2011 AT&T Intellectual Property          #
#                      and is licensed under the                       #
#                 Eclipse Public License, Version 1.0                  #
#                    by AT&T Intellectual Property                     #
#                                                                      #
#                A copy of the License is available at                 #
#          http://www.eclipse.org/org/documents/epl-v10.html           #
#         (with md5 checksum b35adb5213ca9657e911e9befb180842)         #
#                                                                      #
#              Information and Software Systems Research               #
#                            AT&T Research                             #
#                           Florham Park NJ                            #
#                                                                      #
#                 Glenn Fowler <gsf@research.att.com>                  #
#                                                                      #
########################################################################
: make abstract machine executor with state

#
# NOTE: variables defined in this script may conflict with mamfile variables
#

_command_=mamexec
case `(getopts '[-][123:xyz]' opt --xyz; echo 0$opt) 2>/dev/null` in
0123)	ARGV0="-a $_command_"
	USAGE=$'
[-?
@(#)$Id: mamexec (AT&T Labs Research) 1994-04-17 $
]
'$USAGE_LICENSE$'
[+NAME?mamexec - make abstract machine executor]
[+DESCRIPTION?\bmamexec\b reads MAM (Make Abstract Machine) target and
	prerequisite file descriptions from the standard input and executes
	actions to update targets that are older than their prerequisites.
	Mamfiles are generated by the \b--mam\b option of \bnmake\b(1) and
	\bgmake\b(1) and are portable to environments that only have
	\bsh\b(1) and \bcc\b(1).]
[+?A separate \bmamstate\b(1) program is used to compare current and state
	file target times. \bmamexec\b is a \bksh\b(1) script implementation
	that has been replaced by the standalone \bmamake\b(1). New
	applications should use \bmamake\b.]
[d:debug?Set the debug trace level to \alevel\a. Higher levels produce
	more output.]
[f:force?Force all targets to be out of date.]
[i:ignore?Ignore shell action errors.]
[n!:exec?Enable shell action execution.]
[s:silent?Do not trace shell actions as they are executed.]

[ target ... ]

[+SEE ALSO?\bmamake\b(1), \bmamstate\b(1), \bnmake\b(1)]
'
	;;
*)	ARGV0=""
	USAGE="dfins"
	;;
esac

usage()
{
	OPTIND=0
	getopts $ARGV0 "$USAGE" OPT '-?'
	exit 2
}

_debug_=:
_diff_=.
_error_='exit 1'
_exec_=eval
_force_=
_list_=
_same_=.
_set_=
_silent_=
_state_list_=MAMlist
_state_time_=MAMtime
_tmp_=/tmp/mam.$$.mam

trap 'rm -f $_tmp_' 0
trap 'exit 2' 1 2

while	getopts $ARGV0 "$USAGE" OPT
do	case $OPT in
	d)	_debug_=$OPT ;;
	f)	_force_=1 ;;
	i)	_error_=: ;;
	n)	_exec_=echo ;;
	s)	_silent_=1 ;;
	*)	usage ;;
	esac
done

_select_=
while	:
do	case $# in
	0)	break ;;
	esac
	case $1 in
	*=*)	eval $1
		;;
	*)	case $_select_ in
		"")	_select_=$1 ;;
		*)	_select_="$_select_|$1" ;;
		esac
		;;
	esac
	shift
done
case $_select_ in
"")	_select_="*" ;;
esac
(set -e; false || true) && _set_=e || echo $_command_: command errors ignored because of shell botch >&2
case $_silent_ in
"")	_set_=x$_set_
	;;
*)	case $_exec_ in
	"echo")	_exec_=: ;;
	esac
	;;
esac
case $_exec_ in
"eval")	_begin_="("
	_end_=") </dev/null"
	case $_set_ in
	?*)	_set_="set -$_set_;" ;;
	esac
	;;
*)	_set_=
	;;
esac

if	test -f $_state_list_ -a -f $_state_time_
then	mamstate $_state_list_ < $_state_list_ | sort > $_tmp_
	for _i_ in `comm -12 $_state_time_ $_tmp_ | sed 's/ .*//'`
	do	case $_same_ in
		.)	_same_=$_i_ ;;
		*)	_same_="$_same_|$_i_" ;;
		esac
	done
fi

_index_=_
_match_=
case `(echo ok | (read -r a; echo $a) 2>/dev/null)` in
ok)	_read_='read -r'
	;;
*)	# read strips \ -- thanks a lot
	# tmp file avoids char at a time read
	_read_=read
	sed 's/\\/\\\\/g' > $_tmp_
	exec < $_tmp_
	rm -f $_tmp_
	;;
esac
_old_=1
_ifs_=$IFS
while	IFS=' '; $_read_ _op_ _arg_ _val_
do	IFS=$_ifs_
	case $_op_ in
	"note")	continue
		;;
	"info")	case $_arg_ in
		"mam")	_old_= ;;
		esac
		continue
		;;
	"setv") eval _data_='$'$_arg_
		case $_index_:$_data_ in
		__*:*|*:)
			case $_exec_ in
			"eval")	;;
			*)	echo "$_arg_=$_val_" ;;
			esac
			eval $_arg_="$_val_" "</dev/null"
			;;
		esac
		case $_arg_:$mam_cc_L in
		"CC:")	(
			set -
			mkdir /tmp/mam$$
			cd /tmp/mam$$
			echo 'main(){return 0;}' > main.c
			code=1
			if	$CC -c main.c 2>/dev/null
			then	if	$CC -L. main.o -lc 2>/dev/null
				then	$CC -L. main.o -lc > libc.a 2>/dev/null || code=0
				fi
			fi
			cd /tmp
			rm -rf /tmp/mam$$
			exit $code
			) </dev/null && mam_cc_L=' '
			;;
		esac
		continue
		;;
	"make")	eval _name_$_index_=$_name_
		eval _prev_$_index_=$_prev_
		eval _cmds_$_index_='"$_cmds_"'
		eval _attr_$_index_=$_attr_
		eval _name_=$_arg_
		_prev_=$_index_
		case " $_val_ " in
		*" metarule "*)	_attr_=m$_attr_ ;;
		esac
		_cmds_=
		eval "	case \"$_name_\" in
			$_select_)
				case \"$_select_\" in
				\"*\")	_select_=$_name_ ;;
				esac
				_match_=1
				_attr_=x$_attr_
				;;
			esac"
		case $_force_ in
		"")	eval "	case \"$_name_\" in
				$_diff_)_attr_=u$_attr_ ;;
				$_same_);;
				*)	_attr_=u$_attr_ ;;
				esac"
			;;
		*)	_attr_=u$_attr_
			;;
		esac
		case $_attr_ in
		*u*)	case $_diff_ in
			.)	_diff_=$_name_ ;;
			*)	_diff_="$_diff_|$_name_" ;;
			esac
			;;
		esac
		_index_=_$_index_
		eval _name_$_index_=$_name_
		eval _prev_$_index_=$_prev_
		eval _cmds_$_index_=$_cmds_
		eval _attr_$_index_=$_attr_
		eval _list_='"'"$_list_"'
'$_name_'"'
		continue
		;;
	"prev")	case $_force_ in
		"")	eval "	case \"$_arg_\" in
				$_diff_)_attr_=u$_attr_ ;;
				$_same_)	;;
				*)	_attr_=u$_attr_ ;;
				esac"
			;;
		*)	_attr_=u$_attr_
			;;
		esac
		continue
		;;
	esac
	case $_index_ in
	_)	echo $_op_: missing make op >&2; continue ;;
	esac
	case $_op_ in
	"attr")	case $_val_ in
		"meta"|"suff")	_attr_=m$_attr_ ;;
		esac
		;;
	"exec"|"....")
		case $_old_ in
		"")	_arg_=$_val_
			;;
		*)	case $_val_ in
			?*)	_arg_="$_arg_ $_val_" ;;
			esac
			;;
		esac
		case $_cmds_ in
		"")	_cmds_=$_arg_
			;;
		*)	_cmds_="$_cmds_
$_arg_"
			;;
		esac
		;;
	"done")	eval _arg_=$_arg_
		_prop_=
		case $_arg_ in
		$_name_)case $_attr_ in
			*m*)	;;
			*x*u*|*u*x*)
				case $_cmds_ in
				"")	case $_attr_ in
					*u*)	_prop_=u ;;
					esac
					;;
				*)	$_exec_ "$_begin_$_set_$_cmds_$_end_" ||
					{
					_code_=$?
					case $_set_ in
					*-*e*)	;;
					*)	case $_cmds_ in
						*if*then*fi*|"||")	_code_=0 ;;
						esac
						;;
					esac
					case $_code_ in
					0)	;;
					*)	echo "*** exit code $_code_ making $_name_" >&2
						$_error_
						;;
					esac
					}
					_prop_=u
					;;
				esac
				;;
			esac
			_index_=$_prev_
			eval _name_='$'_name_$_index_
			eval _prev_='$'_prev_$_index_
			eval _cmds_='$'_cmds_$_index_
			eval _attr_=$_prop_'$'_attr_$_index_
			;;
		*)	echo $_val_: $_op_ $_name_ expected >&2
			;;
		esac
		;;
	esac
done
IFS=$_ifs_
case $_match_ in
"")	echo "$_command_: don't know how to make $_select_" >&2; $_error_ ;;
esac
case $_exec_ in
"eval")	echo "$_list_" > $_state_list_
	mamstate $_state_list_ < $_state_list_ | sort > $_state_time_
	;;
esac
