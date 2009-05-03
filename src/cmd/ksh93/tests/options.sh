########################################################################
#                                                                      #
#               This software is part of the ast package               #
#          Copyright (c) 1982-2009 AT&T Intellectual Property          #
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
#                  David Korn <dgk@research.att.com>                   #
#                                                                      #
########################################################################
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r ${Command}[$1]: "${@:2}"
	let Errors+=1
}
alias err_exit='err_exit $LINENO'

Command=${0##*/}
integer Errors=0

unset HISTFILE
export LC_ALL=C ENV=

ulimit -c 0

tmp=$(mktemp -dt) || { err_exit mktemp -dt failed; exit 1; }
trap "cd /; rm -rf $tmp" EXIT

if	[[ $( ${SHELL-ksh} -s hello<<-\!
		print $1
		!
	 ) != hello ]]
then	err_exit "${SHELL-ksh} -s not working"
fi
x=$(
	set -e
	false && print bad
	print good
)
if	[[ $x != good ]]
then	err_exit 'sh -e not working'
fi
[[ $($SHELL -D -c 'print hi; print $"hello"') == '"hello"' ]] || err_exit 'ksh -D not working'

rc=$tmp/.kshrc
print $'PS1=""\nfunction env_hit\n{\n\tprint OK\n}' > $rc
export ENV=/.$rc
if	[[ -o privileged ]]
then
	[[ $(print env_hit | $SHELL 2>&1) == "OK" ]] &&
		err_exit 'privileged nointeractive shell reads $ENV file'
	[[ $(print env_hit | $SHELL -E 2>&1) == "OK" ]] &&
		err_exit 'privileged -E reads $ENV file'
	[[ $(print env_hit | $SHELL +E 2>&1) == "OK" ]] &&
		err_exit 'privileged +E reads $ENV file'
	[[ $(print env_hit | $SHELL --rc 2>&1) == "OK" ]] &&
		err_exit 'privileged --rc reads $ENV file'
	[[ $(print env_hit | $SHELL --norc 2>&1) == "OK" ]] &&
		err_exit 'privileged --norc reads $ENV file'
else
	[[ $(print env_hit | $SHELL 2>&1) == "OK" ]] &&
		err_exit 'nointeractive shell reads $ENV file'
	[[ $(print env_hit | $SHELL -E 2>&1) == "OK" ]] ||
		err_exit '-E ignores $ENV file'
	[[ $(print env_hit | $SHELL +E 2>&1) == "OK" ]] &&
		err_exit '+E reads $ENV file'
	[[ $(print env_hit | $SHELL --rc 2>&1) == "OK" ]] ||
		err_exit '--rc ignores $ENV file'
	[[ $(print env_hit | $SHELL --norc 2>&1) == "OK" ]] &&
		err_exit '--norc reads $ENV file'
	[[ $(print env_hit | $SHELL -i 2>&1) == "OK" ]] ||
		err_exit '-i ignores $ENV file'
fi

export ENV=
if	[[ -o privileged ]]
then
	[[ $(print env_hit | HOME=$tmp $SHELL 2>&1) == "OK" ]] &&
		err_exit 'privileged nointeractive shell reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL -E 2>&1) == "OK" ]] &&
		err_exit 'privileged -E ignores empty $ENV'
	[[ $(print env_hit | HOME=$tmp $SHELL +E 2>&1) == "OK" ]] &&
		err_exit 'privileged +E reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL --rc 2>&1) == "OK" ]] &&
		err_exit 'privileged --rc ignores empty $ENV'
	[[ $(print env_hit | HOME=$tmp $SHELL --norc 2>&1) == "OK" ]] &&
		err_exit 'privileged --norc reads $HOME/.kshrc file'
else
	[[ $(print env_hit | HOME=$tmp $SHELL 2>&1) == "OK" ]] &&
		err_exit 'nointeractive shell reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL -E 2>&1) == "OK" ]] &&
		err_exit '-E ignores empty $ENV'
	[[ $(print env_hit | HOME=$tmp $SHELL +E 2>&1) == "OK" ]] &&
		err_exit '+E reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL --rc 2>&1) == "OK" ]] &&
		err_exit '--rc ignores empty $ENV'
	[[ $(print env_hit | HOME=$tmp $SHELL --norc 2>&1) == "OK" ]] &&
		err_exit '--norc reads $HOME/.kshrc file'
fi

unset ENV
if	[[ -o privileged ]]
then
	[[ $(print env_hit | HOME=$tmp $SHELL 2>&1) == "OK" ]] &&
		err_exit 'privileged nointeractive shell reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL -E 2>&1) == "OK" ]] &&
		err_exit 'privileged -E reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL +E 2>&1) == "OK" ]] &&
		err_exit 'privileged +E reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL --rc 2>&1) == "OK" ]] &&
		err_exit 'privileged --rc reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL --norc 2>&1) == "OK" ]] &&
		err_exit 'privileged --norc reads $HOME/.kshrc file'
else
	[[ $(print env_hit | HOME=$tmp $SHELL 2>&1) == "OK" ]] &&
		err_exit 'nointeractive shell reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL -E 2>&1) == "OK" ]] ||
		err_exit '-E ignores $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL +E 2>&1) == "OK" ]] &&
		err_exit '+E reads $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL --rc 2>&1) == "OK" ]] ||
		err_exit '--rc ignores $HOME/.kshrc file'
	[[ $(print env_hit | HOME=$tmp $SHELL --norc 2>&1) == "OK" ]] &&
		err_exit '--norc reads $HOME/.kshrc file'
fi

rm -rf $tmp/.kshrc

if	command set -G 2> /dev/null
then	cd $tmp
	mkdir bar foo
	> bar.c > bam.c
	> bar/foo.c > bar/bam.c
	> foo/bam.c
	set -- **.c
	expected='bam.c bar.c'
	[[ $* == $expected ]] ||
		err_exit "-G **.c failed -- expected '$expected', got '$*'"
	set -- **
	expected='bam.c bar bar.c bar/bam.c bar/foo.c foo foo/bam.c'
	[[ $* == $expected ]] ||
		err_exit "-G ** failed -- expected '$expected', got '$*'"
	set -- **/*.c
	expected='bam.c bar.c bar/bam.c bar/foo.c foo/bam.c'
	[[ $* == $expected ]] ||
		err_exit "-G **/*.c failed -- expected '$expected', got '$*'"
	set -- **/bam.c
	expected='bam.c bar/bam.c foo/bam.c'
	[[ $* == $expected ]] ||
		err_exit "-G **/bam.c failed -- expected '$expected', got '$*'"
	cd ~-
fi

cd $tmp
t="<$$>.profile.<$$>"
echo "echo '$t'" > .profile
cp $SHELL ./-ksh
if	[[ -o privileged ]]
then
	[[ $(HOME=$PWD $SHELL -l </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged -l reads .profile'
	[[ $(HOME=$PWD $SHELL --login </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged --login reads .profile'
	[[ $(HOME=$PWD $SHELL --login-shell </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged --login-shell reads .profile'
	[[ $(HOME=$PWD $SHELL --login_shell </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged --login_shell reads .profile'
	[[ $(HOME=$PWD exec -a -ksh $SHELL </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged exec -a -ksh ksh reads .profile'
	[[ $(HOME=$PWD ./-ksh -i </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged ./-ksh reads .profile'
	[[ $(HOME=$PWD ./-ksh -ip </dev/null 2>&1) == *$t* ]] &&
		err_exit 'privileged ./-ksh -p reads .profile'
else
	[[ $(HOME=$PWD $SHELL -l </dev/null 2>&1) == *$t* ]] ||
		err_exit '-l ignores .profile'
	[[ $(HOME=$PWD $SHELL --login </dev/null 2>&1) == *$t* ]] ||
		err_exit '--login ignores .profile'
	[[ $(HOME=$PWD $SHELL --login-shell </dev/null 2>&1) == *$t* ]] ||
		err_exit '--login-shell ignores .profile'
	[[ $(HOME=$PWD $SHELL --login_shell </dev/null 2>&1) == *$t* ]] ||
		err_exit '--login_shell ignores .profile'
	[[ $(HOME=$PWD exec -a -ksh $SHELL </dev/null 2>/dev/null) == *$t* ]] ||
		err_exit 'exec -a -ksh ksh 2>/dev/null ignores .profile'
	[[ $(HOME=$PWD exec -a -ksh $SHELL </dev/null 2>&1) == *$t* ]] ||
		err_exit 'exec -a -ksh ksh 2>&1 ignores .profile'
	[[ $(HOME=$PWD ./-ksh -i </dev/null 2>&1) == *$t* ]] ||
		err_exit './-ksh ignores .profile'
	[[ $(HOME=$PWD ./-ksh -ip </dev/null 2>&1) == *$t* ]] &&
		err_exit './-ksh -p does not ignore .profile'
fi
cd ~-
rm -rf $tmp/.profile

# { exec interactive login_shell restricted xtrace } in the following test

for opt in \
	allexport all-export all_export \
	bgnice bg-nice bg_nice \
	clobber emacs \
	errexit err-exit err_exit \
	glob \
	globstar glob-star glob_star \
	gmacs \
	ignoreeof ignore-eof ignore_eof \
	keyword log markdirs monitor notify \
	pipefail pipe-fail pipe_fail \
	trackall track-all track_all \
	unset verbose vi \
	viraw vi-raw vi_raw
do	old=$opt
	if [[ ! -o $opt ]]
	then	old=no$opt
	fi

	set --$opt || err_exit "set --$opt failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"
	[[ -o no$opt ]] && err_exit "[[ -o no$opt ]] failed"
	[[ -o no-$opt ]] && err_exit "[[ -o no-$opt ]] failed"
	[[ -o no_$opt ]] && err_exit "[[ -o no_$opt ]] failed"
	[[ -o ?$opt ]] || err_exit "[[ -o ?$opt ]] failed"
	[[ -o ?no$opt ]] || err_exit "[[ -o ?no$opt ]] failed"
	[[ -o ?no-$opt ]] || err_exit "[[ -o ?no-$opt ]] failed"
	[[ -o ?no_$opt ]] || err_exit "[[ -o ?no_$opt ]] failed"

	set --no$opt || err_exit "set --no$opt failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	[[ -o $opt ]] && err_exit "[[ -o $opt ]] failed"

	set --no-$opt || err_exit "set --no-$opt failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	[[ -o $opt ]] && err_exit "[[ -o $opt ]] failed"

	set --no_$opt || err_exit "set --no_$opt failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	[[ -o $opt ]] && err_exit "[[ -o $opt ]] failed"

	set -o $opt || err_exit "set -o $opt failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"
	set -o $opt=1 || err_exit "set -o $opt=1 failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"
	set -o no$opt=0 || err_exit "set -o no$opt=0 failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"
	set --$opt=1 || err_exit "set --$opt=1 failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"
	set --no$opt=0 || err_exit "set --no$opt=0 failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"

	set -o no$opt || err_exit "set -o no$opt failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	set -o $opt=0 || err_exit "set -o $opt=0 failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	set -o no$opt=1 || err_exit "set -o no$opt=1 failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	set --$opt=0 || err_exit "set --$opt=0 failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"
	set --no$opt=1 || err_exit "set --no$opt=1 failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"

	set -o no-$opt || err_exit "set -o no-$opt failed"
	[[ -o no-$opt ]] || err_exit "[[ -o no-$opt ]] failed"

	set -o no_$opt || err_exit "set -o no_$opt failed"
	[[ -o no_$opt ]] || err_exit "[[ -o no_$opt ]] failed"

	set +o $opt || err_exit "set +o $opt failed"
	[[ -o no$opt ]] || err_exit "[[ -o no$opt ]] failed"

	set +o no$opt || err_exit "set +o no$opt failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"

	set +o no-$opt || err_exit "set +o no-$opt failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"

	set +o no_$opt || err_exit "set +o no_$opt failed"
	[[ -o $opt ]] || err_exit "[[ -o $opt ]] failed"

	set --$old
done

for opt in \
	exec interactive login_shell login-shell logi privileged \
	rc restricted xtrace
do	[[ -o $opt ]]
	y=$?
	[[ -o no$opt ]]
	n=$?
	case $y$n in
	10|01)	;;
	*)	err_exit "[[ -o $opt ]] == [[ -o no$opt ]]" ;;
	esac
done

for opt in \
	foo foo-bar foo_bar
do	if	[[ -o ?$opt ]]
	then	err_exit "[[ -o ?$opt ]] should fail"
	fi
	if	[[ -o ?no$opt ]]
	then	err_exit "[[ -o ?no$opt ]] should fail"
	fi
done

[[ $(set +o) == $(set --state) ]] || err_exit "set --state different from set +o"
set -- $(set --state)
[[ $1 == set && $2 == --default ]] || err_exit "set --state failed -- expected 'set --default *', got '$1 $2 *'"
shift
restore=$*
shift
off=
for opt
do	case $opt in
	--not*)	opt=${opt/--/--no} ;;
	--no*)	opt=${opt/--no/--} ;;
	--*)	opt=${opt/--/--no} ;;
	esac
	off="$off $opt"
done
set $off
state=$(set --state)
default=$(set --default --state)
[[ $state == $default ]] || err_exit "set --state for default options failed: expected '$default', got '$state'"
set $restore
state=$(set --state)
[[ $state == "set $restore" ]] || err_exit "set --state after restore failed: expected 'set $restore', got '$state'"

typeset -a pipeline
pipeline=(
	( nopipefail=0 pipefail=1 command='false|true|true' )
	( nopipefail=0 pipefail=1 command='true|false|true' )
	( nopipefail=1 pipefail=1 command='true|true|false' )
	( nopipefail=1 pipefail=1 command='false|false|false' )
	( nopipefail=0 pipefail=0 command='true|true|true' )
	( nopipefail=0 pipefail=0 command='print hi|(sleep 1;/bin/cat)>/dev/null' )
)
set --nopipefail
for ((i = 0; i < ${#pipeline[@]}; i++ ))
do	eval ${pipeline[i].command}
	status=$?
	expected=${pipeline[i].nopipefail}
	[[ $status == $expected ]] ||
	err_exit "--nopipefail '${pipeline[i].command}' exit status $status -- expected $expected"
done
ftt=0
set --pipefail
for ((i = 0; i < ${#pipeline[@]}; i++ ))
do	eval ${pipeline[i].command}
	status=$?
	expected=${pipeline[i].pipefail}
	if	[[ $status != $expected ]]
	then	err_exit "--pipefail '${pipeline[i].command}' exit status $status -- expected $expected"
		(( i == 0 )) && ftt=1
	fi
done
if	(( ! ftt ))
then	exp=10
	got=$(for((n=1;n<exp;n++))do $SHELL --pipefail -c '(sleep 0.1;false)|true|true' && break; done; print $n)
	[[ $got == $exp ]] || err_exit "--pipefail -c '(sleep 0.1;false)|true|true' fails with exit status 0 (after $got/$exp iterations)"
fi

echo=$(whence -p echo)
for ((i=0; i < 20; i++))
do	if	! x=$(true | $echo 123)
	then	err_exit 'command substitution with wrong exit status with pipefai'
		break
	fi
done
(
	set -o pipefail
	false | true
	(( $? )) || err_exit 'pipe not failing in subshell with pipefail'
) | wc >/dev/null
$SHELL -c 'set -o pipefail; false | $(whence -p true);' && err_exit 'pipefail not returning failure with sh -c'
exp='1212 or 1221'
got=$(
	set --pipefail
	pipe() { date | cat > /dev/null ;}
	print $'1\n2' |
	while	read i
	do 	if	pipe $tmp
		then	{ print -n $i; sleep 2; print -n $i; } &
		fi
	done
	wait
)
[[ $got == @((12|21)(12|21)) ]] || err_exit "& job delayed by --pipefail, expected '$exp', got '$got'"
$SHELL -c '[[ $- == *c* ]]' || err_exit 'option c not in $-'
> $tmp/.profile
for i in i l r s D E a b e f h k n t u v x B C G H
do	HOME=$tmp ENV= $SHELL -$i >/dev/null 2>&1 <<- ++EOF++ || err_exit "option $i not in \$-"
	[[ \$- == *$i* ]] || exit 1
	++EOF++
done
letters=ilrabefhknuvxBCGE
integer j=0
for i in interactive login restricted allexport notify errexit \
	noglob trackall keyword noexec nounset verbose xtrace braceexpand \
	noclobber globstar rc
do	HOME=$tmp ENV= $SHELL -o $i >/dev/null 2>&1 <<- ++EOF++ || err_exit "option $i not equivalent to ${letters:j:1}"
	[[ \$- == *${letters:j:1}* ]] || exit 1
	++EOF++
	((j++))
done

export ENV=
histfile=$tmp/history
exp=$(HISTFILE=$histfile $SHELL -c $'function foo\n{\ncat\n}\ntype foo')
for var in HISTSIZE HISTFILE
do	got=$( ( HISTFILE=$histfile $SHELL -ic $'unset '$var$'\nfunction foo\n{\ncat\n}\ntype foo\nexit' ) 2>&1 )
	got=${got##*': '} 
	[[ $got == "$exp" ]] || err_exit "function definition inside (...) with $var unset fails -- got '$got', expected '$exp'"
	got=$( { HISTFILE=$histfile $SHELL -ic $'unset '$var$'\nfunction foo\n{\ncat\n}\ntype foo\nexit' ;} 2>&1 )
	got=${got##*': '} 
	[[ $got == "$exp" ]] || err_exit "function definition inside {...;} with $var unset fails -- got '$got', expected '$exp'"
done
( unset HISTFILE; $SHELL -ic "HISTFILE=$histfile" 2>/dev/null ) || err_exit "setting HISTFILE when not in environment fails"

# the next tests loop on all combinations of
#	{ SUB PAR CMD ADD }

SUB=(
	( BEG='$( '	END=' )'	)
	( BEG='${ '	END='; }'	)
)
PAR=(
	( BEG='( '	END=' )'	)
	( BEG='{ '	END='; }'	)
)
CMD=(	command-kill	script-kill	)
ADD=(	''		'; :'		)

cd $tmp
print $'#!'$SHELL$'\nkill -KILL $$' > command-kill
print $'kill -KILL $$' > script-kill
chmod +x command-kill script-kill
export PATH=.:$PATH
exp='Killed'
for ((S=0; S<${#SUB[@]}; S++))
do	for ((P=0; P<${#PAR[@]}; P++))
	do	for ((C=0; C<${#CMD[@]}; C++))
		do	for ((A=0; A<${#ADD[@]}; A++))
			do	cmd="${SUB[S].BEG}${PAR[P].BEG}${CMD[C]}${PAR[P].END} 2>&1${ADD[A]}${SUB[S].END}"
				eval got="$cmd"
				got=${got##*': '}
				got=${got%%'('*}
				[[ $got == "$exp" ]] || err_exit "$cmd failed -- got '$got', expected '$exp'"
			done
		done
	done
done

exit $((Errors))
