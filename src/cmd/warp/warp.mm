.xx meta.description="warp executes a dynamically linked command in a different time frame"
.xx meta.keywords="unix, y2k, date testing"
.MT 4
.TL
AT&T Labs Research warp command
.AF "AT&T Labs Research - Florham Park NJ"
.AU "Glenn Fowler <gsf@research.att.com>"
.H 1 Description
.BR warp (1)
executes a dynamically linked command in a different time frame.
It intercepts time related system calls and modifies the times seen by
the command using the formula:
.EX
time' = time + warp + (time - base) * (factor - 1)
.EE
where:
.BL
.LI
.I time'
The logical system time seen by the command process.
.LI
.I time
The physical system time.
.LI
.I warp
A fixed offset from the physical system time
(i.e., the warp.)
.LI
.I base
The physical system time when
.B warp
was first applied to the process or any ancestor process.
.LI
.I factor
The rate of logical time change with respect to the physical clock
(i.e., the warp factor.)
.LE
.IR warp ,
.IR base ,
and
.I factor
are inherited by children of warped processes so that a child process
is warped in the same time frame as its parent.
Time progresses for warped processes at the rate of
.I factor
times the physical system clock.
Any files created by a warped process
will appear to be in the warped logical time frame
for that process but will appear in physical system time frame
for non-warped processes.
.P
Statically-linked, set-uid or set-gid commands are not warped.
.H 1 Interface
.B warp
[
.B \-b
.I base
] [
.B \-f
.I factor
] [
.B \-n
] [
.B \-t
]
.I date
[
.I command
[
.I arg
\&...
] ]
.P
The components of the warp formula are set as follows:
the warp offset
.I warp
is
.IR date-now ,
.I base
is
.I date
by default,
and
.I factor
is 1 by default.
.P
Command argument date specifications support common conventions:
.EX
yesterday
next week
50 days
2000-02-28/00:00
feb 28 2000
.EE
Absolute seconds since the epoch, a.k.a.
.B time_t
values, are represented by
.BI # seconds.
.P
The
.B \-n
option shows how
.I command
would be executed without actually executing it.
The
.B \-t
option traces each intercepted system call.
.P
.B warp
executes
.I command
with optional
.IR args ,
or
.B $SHELL
if
.I command 
is omitted.
.H 1 Examples
.EX
$ date -f %Y-%m-%d/%H:%M
1998-03-11/13:41
$ warp 2000-02-29/12:30:30 date -f %Y-%m-%d/%H:%M
2000-02-29/12:30
$ date -f %Y-%m-%d/%H:%M
1998-03-11/13:44
$ warp '2 years' date
2000-01-01/00:00

# get a shell where 1 logical day passes for each physical second
$ PS1='(warp) ' warp -f $(60*60*24) 2000-02-29/12:30:30
(warp) date -f %Y-%m-%d/%H:%M
2000-03-07/18:58
(warp) sleep 1
(warp) date -f %Y-%m-%d/%H:%M
2000-03-19/18:58
.EE
.H 1 Implementation
As mentioned above, processes are warped by intercepting
systems calls with a shared library
that is preloaded at process startup before
.I main()
is called.
The intercepted system calls are:
.BR alarm (2),
.BR fstat (2),
.BR getitimer (2),
.BR gettimeofday (2),
.BR lstat (2),
.BR poll (2),
.BR select (2),
.BR setitimer (2),
.BR stat (2),
.BR time (2),
.BR times (2),
.BR utime (2),
.BR utimes (2).
Also intercepted are the `_' and `_libc_' name permutations of the calls,
as well as any 32-bit and 64-bit versions, and the abominable System V
`x' versions of the
.BR stat (2)
family.
.B warp
ignores calls not present in a particular host system.
.P
.B warp
only works on dynamically linked executables that have neither set-uid
nor set-gid permissions.
.P
The
.B warp
implementation consists of three components:
the
.I warp
script, located in one of the directories in the
.B PATH
environment variable,
the warp shared library
.IR libwarp.so ,
located in
.B ../lib
relative to the directories in the
.B PATH
environment variable,
and the
.B ast
.BR date (1)
command, located in one of the directories in the
.B PATH
environment variable,
that supports conversion to/from
.B time_t
values (seconds since the epoch) at the shell level.
Systems like
.B sgi.mips3
that support multiple a.out formats may have multiple versions of
.IR libwarp.so .
.H 1 Download
.B warp
is part of the
.I ast-open
package posted at the
.xx link="http://www.research.att.com/sw/download/	AT&T Software Technology"
download site.
