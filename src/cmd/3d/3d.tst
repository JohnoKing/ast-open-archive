: 3d regression tests
#
# 3d.tst (AT&T Research) 02/29/96
#
# the first section defines the test harness
# the next section defines individual test functions
# the tests are in the last section
#

( vpath ) >/dev/null 2>&1 || {
	print -u2 $0: must be run from 3d shell
	exit 1
}

integer errors=0 seconds=0 tests=0

ACTIVE=
FILE=
FORMAT="%Y-%m-%d+%H:%M:%S"
GROUP=
INIT=
NAME=${0##*/}
NAME=${NAME%%.*}
NEW="new-and-improved"
NUKE=
OLD="original"
OWD=$PWD
PREFIX=
STAMP="2005-07-17+04:05:06"
TMP=/tmp/3d.$$
VIRTUAL=

for i
do	case $ACTIVE in
	"")	ACTIVE=$i ;;
	*)	ACTIVE="$ACTIVE|$i" ;;
	esac
done
case $ACTIVE in
"")	ACTIVE="*" ;;
esac
trap "(( errors++ ))" ERR
trap "DONE" 0 1 2
mkdir $TMP || exit
cd $TMP
mkdir bottom

alias INIT='ACTIVE && DATA $* || return'

function TEST
{
	case $INIT in
	"")	INIT=1
		print "TEST	$NAME"
		;;
	esac
	cd $TMP
	case $NUKE in
	?*)	rm -rf $NUKE; NUKE= ;;
	esac
	PREFIX=
	GROUP=$1
	ACTIVE || return
	vpath - -
	shift
	print "$GROUP	$*"
}

function ACTIVE
{
	[[ $GROUP == @($ACTIVE) ]]
}

function DONE
{
	trap - 0 1 2
	cd $OWD
	rm -rf $TMP
	print "TEST	$NAME, $tests tests, $errors errors"
	exit
}

function FAIL # file message
{
	print -u2 "	FAIL $@"
	rm -rf $1
	(( errors++ ))
}

function PREFIX
{
	ACTIVE || return
	PREFIX=$1
	case $1 in
	/*)	NUKE="$NUKE $1" ;;
	*)	NUKE="$NUKE $PWD/$1" ;;
	esac
}

function VIRTUAL
{
	ACTIVE || return
	case $VIRTUAL in
	?*)	pwd=$PWD
		cd $TMP
		rm -rf $TMP/$VIRTUAL
		cd $pwd
		;;
	esac
	VIRTUAL=$1
}

function CD
{
	ACTIVE || return
	cd $TMP/$1
}

function VPATH
{
	ACTIVE || return
	vpath "$@"
}

function LN
{
	ACTIVE || return
	ln "$@"
	shift $#-1
	NUKE="$NUKE $1"
}

function MKDIR
{
	ACTIVE || return
	mkdir -p $*
	for i
	do	case $i in
		/*)	NUKE="$NUKE $i" ;;
		*)	NUKE="$NUKE $i" ;;
		esac
	done
}

function DATA
{
	ACTIVE || return 1
	VIRTUAL $VIRTUAL
	case $1 in
	-)	remove=1; shift ;;
	*)	remove=0 ;;
	esac
	case $# in
	0)	return 0 ;;
	1)	;;
	*)	return 1 ;;
	esac
	(( tests++ ))
	path=$1
	case $PREFIX in
	"")	FILE=$path ;;
	*)	FILE=$PREFIX/$path ;;
	esac
	file=bottom/$path
	if	[[ ! -f $TMP/$file ]]
	then	case $remove in
		0)	if	[[ $path == */* && ! -d $TMP/${file%/*} ]]
			then	mkdir -p $TMP/${file%/*} || FAIL $TMP/${file%/*} DATA mkdir
			fi
			print $OLD > $TMP/$file
			mode=${file%???}
			mode=${file#$mode}
			chmod $mode $TMP/$file || FAIL $TMP/$file DATA chmod
			;;
		esac
	else	case $remove in
		1)	rm -f $TMP/$file ;;
		esac
	fi
	return 0
}

#
# the remaining functions implement individiual parameterized tests
#

function APPEND
{
	INIT
	print "$NEW" >> $FILE || FAIL $FILE write error
	if	[[ $(<$FILE) != "$OLD"$'\n'"$NEW" ]]
	then	FAIL $FILE unchanged by $0
	elif	[[ -f $FILE/... && $(<$FILE/...) != "$OLD" ]]
	then	FAIL $FILE/... changed by $0
	fi
}

function MODE
{
	INIT
	chmod 000 $FILE || FAIL $FILE chmod error
	if	[[ -f $FILE/... && ! -r $FILE/... ]]
	then	FAIL $FILE/... changed by $0
	elif	[[ -r $FILE ]]
	then	FAIL $FILE unchanged by $0
	fi
}

function REMOVE
{
	INIT
	rm $FILE || FAIL $FILE rm error
	if	[[ ! -f $FILE/... ]]
	then	FAIL $FILE/... changed by $0
	fi
	print "$NEW" > $FILE || FAIL $FILE write error
	rm $FILE || FAIL $FILE rm error
	if	[[ $(<$FILE) != "$OLD" ]]
	then	FAIL $FILE unchanged by $0
	elif	[[ $(<$FILE/...) != "$OLD" ]]
	then	FAIL $FILE/... changed by $0
	fi
}

function TOUCH
{
	INIT
	touch -r $FILE -t "$seconds seconds hence" $TMP/reference || FAIL $TMP/reference touch error
	(( seconds++ ))
	touch -t "$seconds seconds hence" $FILE || FAIL $FILE touch error
	if	[[ $FILE/... -nt $TMP/reference ]]
	then	FAIL $FILE/... changed by $0
	elif	[[ ! $FILE -nt $TMP/reference ]]
	then	FAIL $FILE unchanged by $0
	fi
	touch -t $STAMP $FILE
	if	[[ $(date -m -f $FORMAT $FILE) != "$STAMP" ]]
	then	FAIL $FILE modfiy time does not match $STAMP
	fi
}

function UPDATE
{
	INIT
	print "$NEW" 1<> $FILE || FAIL $FILE write error
	if	[[ $(<$FILE) != "$NEW" ]]
	then	FAIL $FILE unchanged by $0
	elif	[[ -f $FILE/... && $(<$FILE/...) != "$OLD" ]]
	then	FAIL $FILE/... changed by $0
	fi
}

function WRITE
{
	INIT
	print "$NEW" > $FILE || FAIL $FILE write error
	if	[[ $(<$FILE) != "$NEW" ]]
	then	FAIL $FILE unchanged by $0
	elif	[[ -f $FILE/... && $(<$FILE/...) != "$OLD" ]]
	then	FAIL $FILE/... changed by $0
	fi
}

function RUN
{
	INIT
	WRITE	w666
	WRITE	w600
	TOUCH	t777
	MODE	m444
	WRITE	dir/w666
	WRITE	dir/w600
	TOUCH	dir/t777
	MODE	dir/m444
	UPDATE	u644
	UPDATE	u640
	APPEND	a644
	APPEND	a640
	UPDATE	dir/u644
	UPDATE	dir/u640
	APPEND	dir/a644
	APPEND	dir/a640
	VIRTUAL
	REMOVE	r644
	WRITE	r644
	REMOVE	r644
}

#
# finally the tests
#

TEST 01 PWD==top top exists
	VPATH top bottom
	MKDIR top
	CD top
	RUN

TEST 02 PWD!=top top exists
	VPATH top bottom
	MKDIR top
	MKDIR junk
	CD junk
	PREFIX ../top
	RUN

TEST 03 PWD==top top virtual
	VIRTUAL top
	VPATH top bottom
	CD top
	RUN

TEST 04 PWD!=top top virtual
	VIRTUAL top
	VPATH top bottom
	MKDIR junk
	CD junk
	PREFIX ../top
	RUN

TEST 05 top symlink
	if	LN -s text link
	then	[[ -L link ]] || FAIL lstat does stat
	fi
