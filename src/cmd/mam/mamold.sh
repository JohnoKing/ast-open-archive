####################################################################
#                                                                  #
#             This software is part of the ast package             #
#                Copyright (c) 1989-2003 AT&T Corp.                #
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
#               Glenn Fowler <gsf@research.att.com>                #
#                                                                  #
####################################################################
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
while read -r op arg val
do	case $op in
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
