.xx meta.keywords="regular expression pattern match regression test"
.MT 4
.TL
AT&T Labs Research regex(3) regression tests
.AF "AT&T Labs Research - Florham Park NJ"
.AU "Glenn Fowler <gsf@research.att.com>"
.H 1
.xx link="testregex.c	testregex.c 2002-06-26"
is the latest source for the AT&T Labs Research regression test
harness for the X/Open
.xx link="http://www.opengroup.org/onlinepubs/007904975/functions/regcomp.html	X/Open regex"
pattern match interface.
See
.BR testregex (1)
for option and test input details.
The source and test data posted here are license free.
.P
The primary purpose of the test harness is to verify stability for a particular
implementation in the face of source code and/or compilation environment
changes.
So, by design, each test provides for only one correct answer.
A secondary purpose is to verify standard compliance for all implementations.
This is much more difficult, since
.I compliance
is an elusive concept; it is possible for two rational, technically sound
interpretations of the standard to be in complete disagreement.
.H 1 "Reference Implementations"
.B testregex
is currently built against these reference implementations:
.TS
center;
rb cb lb
r c l.
NAME	LABEL	AUTHORS
AT&T ast	\h'0*\w"http://www.research.att.com/sw/download/"'A\h'0'	Glenn Fowler and Doug McIlroy
bsd	\h'0*\w"ftp://ftp.netbsd.org/pub/NetBSD/NetBSD-1.5.2/source/sets/src.tgz"'B\h'0'	\|
gnu	\h'0*\w"http://www.gnu.org"'G\h'0'	\|
irix	\h'0*\w"http://www.sgi.com"'I\h'0'	\|
regex++	\h'0*\w"http://ourworld.compuserve.com/homepages/John_Maddock/regexpp.htm"'M\h'0'	John Maddox
pcre perl compatible	\h'0*\w"http://www.pcre.org/"'P\h'0'	Philip Hazel
rx	\h'0*\w"ftp://regexps.com/pub/src/hackerlab/"'R\h'0'	Tom Lord
spencer	\h'0*\w"http://arglist.com/regex/rxspencer-alpha3.8.g2.tar.gz"'S\h'0'	Henry Spencer
libtre	\h'0*\w"http://kouli.iki.fi/~vlaurika/libtre/"'T\h'0'	Ville Laurikari
.TE
.H 1 "Test Data Repository"
Test data file names posted in the repository will reflect the 
nature of the tests:
.VL 6
.LI
.BI *-[ LABEL ...].dat
Tests that the
.IR LABEL ...
reference implementations pass.
For example,
.B foo-AR.dat
would contain tests that both the
.B "AT&T ast"
.RB ( A )
and the
.B rx
.RB ( R )
implementations pass.
.LI
.BI *- nnnn .dat
Test results for a standard interpretation proposal in the
<austin-regexp-l-request@opengroup.org> regex mailing group.
Numbering will start from
.B 0001
for each topic.
.LI
\fB*.dat\fP \fIbut not\fP \fB*-*.dat\fP
Tests that all standard compliant implementations should pass.
.P
The test repository:
.TS
center;
r l.
\h'0*\w"basic.dat"'basic.dat\h'0'	\|\|basic regex(3) -- all implementations should pass these
\h'0*\w"nullsubexpr-A.dat"'nullsubexpr-A.dat\h'0'	\|\|null (...)* tests
\h'0*\w"leftassoc.dat"'leftassoc.dat\h'0'	\|\|left associative catenation implementation must pass these
\h'0*\w"rightassoc.dat"'rightassoc.dat\h'0'	\|\|right associative catenation implementation must pass these
\h'0*\w"forcedassoc.dat"'forced-assoc.dat\h'0'	\|\|subexpression grouping to force associativity
.TE
.H 1 "Usage"
To run the
.B basic.dat
tests:
.EX
testregex < basic.dat
.EE
.P
If the local implementation hangs or dumps on some tests then run with
the \fB-c\fP option.
The \fB-h\fP option lists the test data format details.
The test data files exercise all features;
the test harness detects and ignores features not
supported by the local implementation.
.H 1 "Reference Implementation Notes"
.H 2 "D: diet libc"
The
.xx link="http://www.fefe.de/dietlibc/	diet libc"
implementation is currently omitted because it fails all but one
.B basic.dat
test.
.H 2 "P: PCRE"
The
.B P
implementation emulates
.BR perl (1)
and is not X/Open compliant by design.
The main differences are:
.BL
.LI
.B P
.I "leftmost-first"
matching as opposed to the X/Open
.IR "leftmost-longest" .
.LI
.B REG_EXTENDED
patterns only.
.LE
.P
However, the
.B P
package regression tests, and
.BR perl (1)
features creeping into other implementations,
make it reasonable to include here.
.H 1 "testregex Notes"
Extensions to the standard terminology are derived from the AT&T
implementation, unified under
.B <regex.h>
with these modes:
.TS
center allbox;
cb lb lb
r l l.
MODE	FLAGS	DESCRIPTION
BRE	0	basic RE
ERE	REG_EXTENDED	egrep RE with perl (...) extensions
ARE	REG_AUGMENTED	ERE with ! negation, <> word boundaries
SRE	REG_SHELL	sh patterns
KRE	REG_SHELL|REG_AUGMENTED	ksh93 patterns: ! @ ( | & ) { }
LRE	REG_LITERAL	fgrep patterns
.TE
.P
and a few flags to handle
.BR fnmatch (3):
.TS
center allbox;
lb lb
l l.
regex FLAG	fnmatch FLAG
REG_SHELL_ESCAPED	FNM_NOESCAPE
REG_SHELL_PATH	FNM_PATHNAME
REG_SHELL_DOT	FNM_PERIOD
.TE
.P
The original
.L testregex.c
was done by Doug McIlroy at Bell Labs.
The current implementation is maintained by Glenn Fowler <gsf@research.att.com>.
