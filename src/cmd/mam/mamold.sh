########################################################################
#                                                                      #
#               This software is part of the ast package               #
#          Copyright (c) 1989-2012 AT&T Intellectual Property          #
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
: convert mam to old make makefile

function convert
{
	typeset buf=$*; typeset -i i
	set -s -A variable ${variable[@]}
	for (( i = ${#variable[@]} - 1; i >= 0; i-- ))
	do	buf=${buf//\$${variable[i]}/\$(${variable[i]})}; done
	print -r -- "$buf"
}
function closure
{
	typeset i j
	for i
	do	[[ " $list " == *" $i "* ]] && continue
		list="$list $i"
		for j in ${implicit[$i]}
		do	closure $j; done
	done
}

typeset -A prereqs implicit action
typeset -i level=0 nvariables=0
typeset rule list order target variable

print "# # oldmake makefile generated by $0 # #"
while read -r op arg val extra junk
do	case $op in
	[0-9]*)	op=$arg
		arg=$val
		val=$extra
		;;
	esac
	case $op in
	setv) variable[nvariables++]=$arg
		convert "$arg = $val"
		;;
	make|prev) rule=${target[level]}
		[[ " $val " == *" implicit "* ]] &&
		implicit[$rule]="${implicit[$rule]} $arg" ||
		prereqs[$rule]="${prereqs[$rule]} $arg"
		[[ $op == prev ]] && continue
		target[++level]=$arg
		[[ " $order " != *" $arg "* ]] && order="$order $arg"
		;;
	exec) [[ $arg == - ]] && arg=${target[level]}
		[[ ${action[$arg]} ]] &&
		action[$arg]=${action[$arg]}$'\n'$'\t'$val ||
		action[$arg]=$'\t'$val
		;;
	done) level=level-1
		;;
	esac
done
for rule in $order
do	[[ ! ${prereqs[$rule]} && ! ${action[$rule]} ]] && continue
	list=
	closure ${prereqs[$rule]} && print && convert "$rule :$list"
	[[ ${action[$rule]} ]] && convert "${action[$rule]}"
done
