/*
 * Mail -- a mail program
 *
 * String data and global state.
 */

#include "mailx.h"

static const char	id[] = "\n@(#)$Id: mailx (AT&T/BSD) 9.9 2002-05-31 $\0\n";

static const char	terms[] = "\n\
@(#)Copyright (c) 1980, 1993, 1996, 2002\n\
\tThe Regents of the University of California.  All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions\n\
are met:\n\
\n\
1. Redistributions of source code must retain the above copyright\n\
   notice, this list of conditions and the following disclaimer.\n\
2. Redistributions in binary form must reproduce the above copyright\n\
   notice, this list of conditions and the following disclaimer in the\n\
   documentation and/or other materials provided with the distribution.\n\
3. All advertising materials mentioning features or use of this software\n\
   must display the following acknowledgement:\n\
	This product includes software developed by the University of\n\
	California, Berkeley and its contributors.\n\
4. Neither the name of the University nor the names of its contributors\n\
   may be used to endorse or promote products derived from this software\n\
   without specific prior written permission.\n\
\n\
THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND\n\
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n\
ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE\n\
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n\
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS\n\
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)\n\
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n\
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY\n\
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF\n\
SUCH DAMAGE.\n\
";

/*
 * Commands -- sorted ignoring [ and ]
 */

#define CMD(x)		(Cmd_f)x

static const struct cmd	cmdtab[] =
{

"B[last]",	CMD(Blast),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nList the selected messages and all headers as name=value pairs suitable for input to the shell."),
"C[opy]",	CMD(Copy),		M|STRLIST,	0,	0,
	X("[ message ... ]\nCopy the selected messages to a file name derived from the sender of the first message without marking the messages SAVE."),
"F[ollowup]",	CMD(Followup),	R|I|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nReply to the first message, sending the message to the sender of each selected message. The reply is recorded in a file name derived from the sender of the first message."),
"Fr[om]",	CMD(From),		MSGLIST,	0,	MMNORM,
	X("[ message ]\nList the status and sender for the selected messages."),
"G[et]",	CMD(Get),		M|RAWLIST,	0,	ARG_MAX,
	X("[ attachment [ file ... ] ]\nEquivalent to get except the ${MAILCAP} command is not executed."),
"J[oin]",	CMD(Join),		R|I|MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nEquivalent to the command sequence Reply ~m ~v."),
"M[ore]",	CMD(More),		MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nPipe the selected messages and all headers through ${PAGER}."),
"Pa[ge]",	CMD(More),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to More [ message ... ]."),
"P[rint]",	CMD(Type),		MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nList the selected messages and all headers on the standard output."),
"R[eply]",	CMD(Reply),		R|I|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nMail a reply message to the sender of each selected message. The subject will be prefixed with Re:<space> if not already in that form. See reply if ${flipr} is set."),
"R[espond]",	CMD(Reply),		R|I|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to Reply [ message ... ]."),
"S[ave]",	CMD(Save),		STRLIST,	0,	0,
	X("[ message ... ]\nCopy the selected messages to a file name derived from the sender of the first message, marking the messages SAVE."),
"Sp[lit]",	CMD(Split),		M|STRLIST,	2,	2,
	X("[ message ... ] start directory\nSplit the messages into files numbered from start in the named directory, marking the messages SAVE. All headers are ignored.  Attachments are named n-m. A line containing <path> <n> <lines> <chars> is listed for each message. Optional attachment information is appended to each line: <n>-<m> <name> <type> <lines> <chars>."),
"T[ype]",	CMD(Type),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to Print [ message ... ]"),
"a[lias]",	CMD(alias),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ name [ value ] | < file ]\nDeclare an address alias value for name. If value is omitted then the alias for name is listed. If name is omitted then all aliases are listed. < file reads sendmail aliases from file."),
"alt[ernates]",	CMD(alternates),	M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ name ... ]\nDeclare a list of alternate names for the current user. The names are translated to aliases of the current user login name. If name is omitted then the alternate names are listed."),
"b[last]",	CMD(blast),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nList the selected messages as name=value pairs suitable for input to the shell."),
"cd",		CMD(cd),		M|Z|RAWLIST,	0,	1,
	X("[ directory ]\nChange the current directory. If directory is omitted then ${HOME} is used; if directory is - then the previous current working directory is used; if ${CDPATH} is set then directories not starting with / and whose first component is neither . nor .. then directory is searched for on the : separated list of directories in ${CDPATH}. If directory is - or is found on ${CDPATH} then the full current working directory path is printed."),
"ch[dir]",	CMD(cd),		M|Z|RAWLIST,	0,	1,
	X("[ directory ]\nEquivalent to cd [ directory ]."),
"c[opy]",	CMD(cmdcopy),		M|STRLIST,	0,	0,
	X("[ message ... ] [ file ]\nCopy the selected messages to file without marking the messages SAVE. If file is omitted then ${MBOX} is used."),
"d[elete]",	CMD(cmddelete),	W|P|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nMark the the selected messages to be DELETE from the mailbox. If ${autoprint} is set then the next unREAD message will be listed."),
"di[scard]",	CMD(ignore),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ header ...]\nEquivalent to ignore [ header ]."),
"dot",		CMD(dot),		Z|NOLIST,	0,	0,
	X("\nList the current message number. Equivalent to `.'."),
"dp",		CMD(deltype),		W|MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nEquivalent to delete [ message ... ] print"),
"dt",		CMD(deltype),		W|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to dp."),
"du[plicate]",	CMD(duplicate),		M|STRLIST,	0,	0,
	X("[ message ... ] address ...\nDuplicate the selected messages to address ... without marking the messages SAVE. The original sender information is preserved. The messages are transferred via SMTP using the ${smtp} host."),
"ec[ho]",	CMD(echo),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ arg ... ]\nThe arguments are listed after mail and shell file name expansion."),
"e[dit]",	CMD(editor),		I|MSGLIST,	0,	MMNORM,
	X("[ message ... ]\nPlace each selected message in a temporary file and invoke ${EDITOR} on the file. The mailbox is not affected."),
"el[se]",	CMD(cmdelse),		C|M|Z|RAWLIST,	0,	0,
	X("\nThe else portion of the current if command."),
"en[dif]",	CMD(cmdendif),	C|M|Z|RAWLIST,	0,	0,
	X("\nTerminates an if/else nesting level."),
"ex[it]",	CMD(cmdexit),		M|Z|NOLIST,	0,	0,
	X("\nExit without changing the mailbox."),
"fi[le]",	CMD(folder),		M|Z|RAWLIST,	0,	1,
	X("[ file ]\nQuit the current mailbox and make file the new mailbox. If file is omitted then the current mailbox status is listed. If file is a directory then mh semantics are assumed."),
"fold[er]",	CMD(folder),		M|Z|RAWLIST,	0,	1,
	X("[ file ]\nEquivalent to file [ file ]."),
"folders",	CMD(folders),		M|Z|NOLIST,	0,	0,
	X("\nList the contents of the ${folder} directory using ${LISTER}."),
"fo[llowup]",	CMD(followup),	R|I|MSGLIST,	0,	MMNDEL,
	X("[ message ]\nReply to message, recording the reply on a file name derived from the sender of the message."),
"f[rom]",	CMD(from),		MSGLIST,	0,	MMNORM,
	X("[ message ]\nList the header summary for the selected messages."),
"g[et]",	CMD(get),		M|RAWLIST,	0,	ARG_MAX,
	X("[ attachment [ file ... ] ]\nCopy the attachments in the attachment number list from the current message into file. If file is omitted then the file name from the message attachment line is used. If the content type matches an entry in ${MAILCAP} then the corresponding command is executed. If all arguments are omitted then list the attachment summary for the current message. Valid only after a print or type command has been issued on the current message. A file name of - lists the contents on the standard output. A directory file argument copies the attachment to that directory."),
"gr[oup]",	CMD(alias),		M|RAWLIST,	0,	ARG_MAX,
	X("[ name [ value ] ]\nEquivalent to alias [ name [ value ] ]."),
"h[eaders]",	CMD(headers),		Z|MSGLIST,	0,	MMNDEL,
	X("[ message ]\nList a page of header summaries that includes the selected message. ${screen} determines the numbers of summaries per page."),
"hel[p]",	CMD(help),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ all | command | ~ [ command ] | set [ all | variable ] ]\nList help information for all or selected commands or variables. Equivalent to `?'."),
"ho[ld]",	CMD(preserve),	W|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to preserve [ message ... ]"),
"i[f]",		CMD(cmdif),		C|M|RAWLIST,	1,	3,
	X("[ [!] variable | [ d | f ] path | \"a\" [!][=|~] \"b\"\nConditional command execution to be combined with else and endif. r is equivalent to receive and s is equivalent to !receive. d path is true if path is a directory, f path is true if path is a regular file. [!]= is for string [in]equality and [!]~ is for fnmatch(\"b\",\"a\",0)."),
"ig[nore]",	CMD(ignore),		M|RAWLIST,	0,	ARG_MAX,
	X("[ name ...]\nAdd the name arguments to the list of header field names to be suppressed by the copy, print, and save commands. If name is omitted then the ignored header field names are listed. If both retain and ignore are given then the ignore command is ignored."),
"im[ap]",	CMD(imap_command),		STRLIST,	0,	0,
	X("[ command ... | dump [ fl[ags] | f[older] | m[essage] | q[ueue] | s[tate] ] ]\nSend command to the IMAP server with message response tracing enabled for the duration of the command, or dump portions of the internal IMAP client state."),
"in[corporate]",CMD(incorporate),	Z|NOLIST,	0,	0,
	X("\nAppend any new mail to the current mailbox."),
"j[oin]",	CMD(join),		R|I|MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nEquivalent to the command sequence reply ~m ~v."),
"lic[ense]",	CMD(license),		M|Z|NOLIST,	0,	0,
	X("\nList the software license."),
"l[ist]",	CMD(list),		M|Z|NOLIST,	0,	0,
	X("\nList the command names."),
"ls",		CMD(headers),		Z|MSGLIST,	0,	MMNDEL,
	X("[ message .. ]\nEquivalent to headers [ message ...]"),
"m[ail]",	CMD(mail),		I|M|R|STRLIST,	0,	0,
	X("address ...\nCompose and send a message to the address arguments after alias expansion."),
"map",		CMD(map),		M|Z|STRLIST,	0,	0,
	X("address ...\nList the result of alias expansion on the address arguments. map! gives the step-by-step details."),
"mar[k]",	CMD(mark),		STRLIST,	0,	0,
	X("[ message ... ] [no|un]mark[,...]\nSet or clear marks on the selected messages.  The marks are: delete, dot (>), mbox (M), new (N), preserve (P), read (U), save (*), spam (X), touch. Multiple marks may be separated by , or |."),
"mb[ox]",	CMD(mboxit),		W|STRLIST,	0,	0,
	X("[ message ... ]\nAppend the selected messages to ${MBOX} on normal exit."),
"mi[me]",	CMD(capability),	M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ pattern [;] [ command ] | < file ]\nDeclare a command to be executed by the get command for attachments whose content type matches pattern. If pattern does not contain `/' then it is treated as `pattern/*'; `/*' is the only recognized metacharacter sequence. If command is omitted and `;' is not the command for pattern is deleted, otherwise if `;' is omitted the command for pattern is listed. If command is omitted then all capabilities matching pattern are listed. < file reads mailcap entries from file. Default capabilities are initialized from ${MAILCAP}. `#' is the comment character and `\\' escapes the special meaning of `;', `\\', and <newline>. `%s' in command expands to the attachment file name, `%t' expands to the content type, `%{name}' expands to the value of the name option in the content type header. If `%s' is omitted then attachment is piped to command."),
"mk[dir]",	CMD(cmdmkdir),		M|Z|RAWLIST,	1,	1,
	X("directory\nCreate a new folder directory."),
"mo[re]",	CMD(more),		MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nPipe the selected messages through ${PAGER}."),
"[next]",	CMD(next),		NDMLIST,	0,	MMNDEL,
	X("\nList the next unREAD message."),
"pa[ge]",	CMD(more),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to more [ message ... ]."),
"pi[pe]",	CMD(cmdpipe),		STRLIST,	0,	0,
	X("[ [ message ... ] command ]\nPipe the selected messages to command, which must be given as a single, possibly quoted, argument. If command is omitted then ${cmd} is used. If ${page} is set then a <formfeed> is placed between each message on the pipe."),
"pre[serve]",	CMD(preserve),	W|STRLIST,	0,	0,
	X("[ message ]\nMark the selected messaged PRESERVE. These messages will not be deleted from the mailbox on exit."),
"p[rint]",	CMD(type),		MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nList the selected messages on the standard output."),
"pwd",		CMD(pwd),		Z|NOLIST,	0,	0,
	X("\nPrint the full path of the current working directory."),
"q[uit]",	CMD(cmdquit),		Z|NOLIST,	0,	0,
	X("\nExit, storing READ messages in ${MBOX}, deleting DELETE and SAVE messages from the current mailbox, and retaining all other messages."),
"ra",		CMD(replyall),	I|R|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to replyall."),
"ren[ame]",	CMD(cmdrename),		M|Z|RAWLIST,	2,	2,
	X("old new\nRename a folder or folder directory."),
"r[eply]",	CMD(reply),		I|R|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nMail a reply message to all recipients in the header of each selected message. The subject will be prefixed with Re:<space> if not already in that form. See Reply if ${flipr} is set."),
"replya[ll]",	CMD(replyall),	I|R|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to reply, except ${flipr} is ignored."),
"replys[ender]",CMD(replysender),	I|R|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to Reply, except ${flipr} is ignored."),
"r[espond]",	CMD(reply),		I|R|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to reply [ message ... ]."),
"ret[ain]",	CMD(retain),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ name ...]\nAdd the name arguments to the list of header field names to be retained by the copy, print, and save commands. Header names not on the retained list are suppressed. If name is omitted then the retained header field names are listed. If both retain and ignore are given then the ignore command is ignored."),
"rm[dir]",	CMD(cmdrmdir),		M|Z|RAWLIST,	1,	1,
	X("directory\nRemove an empty folder directory."),
"rs",		CMD(replyall),	I|R|MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to replyall."),
"s[ave]",	CMD(save),		STRLIST,	0,	0,
	X("[ message ... ] [ file ]\nCopy the selected messages to file, marking the messages SAVE. If file is omitted then ${MBOX} is used."),
"savei[gnore]",	CMD(saveignore),	M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ name ... ]\nSimilar to ignore [ name ... ] but only has affect on the save command."),
"saver[etain]",	CMD(saveretain),	M|Z|RAWLIST,	0,	ARG_MAX,
	X("[ name ... ]\nSimilar to retain [ name ... ] but only has affect on the save command."),
"se[t]",	CMD(set),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("[no]name[=value] ...\nAssign value to the variable name. If =value is omitted then the name is turned on. noname unsets the value of name. Some unset variables revert to their default values. With no arguments all set variables are listed. set all lists all set and unset variables."),
"sh[ell]",	CMD(shell),		I|Z|STRLIST,	0,	0,
	X("\nInvoke an interactive ${SHELL}."),
"si[ze]",	CMD(size),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nList the lines/chars of each selected message."),
"so[urce]",	CMD(source),		M|Z|RAWLIST,	1,	1,
	X("file\nRead and execute commands from file. Some commands are not valid in source files."),
"sp[lit]",	CMD(split),		M|STRLIST,	2,	2,
	X("[ message ... ] start directory\nThe same as Split except all headers are retained."),
"to[p]",	CMD(top),		MSGLIST,	0,	MMNDEL,
	X("[ message ...]\nList the first ${toplines} lines of the selected messages."),
"tou[ch]",	CMD(cmdtouch),	W|STRLIST,	0,	0,
	X("[ message ... ]\nTouch the selected messages by placing unDELETE and unSAVE messages in ${MBOX} on normal exit."),
"t[ype]",	CMD(type),		MSGLIST,	0,	MMNDEL,
	X("[ message ... ]\nEquivalent to print [ message ... ]"),
"una[lias]",	CMD(unalias),		M|Z|RAWLIST,	1,	ARG_MAX,
	X("[ name ... ]\nDelete the aliases for the name arguments."),
"u[ndelete]",	CMD(undelete),	P|MSGLIST,	MDELETE,MMNDEL,
	X("[ message ... ]\nRestore the selected messages by removing the DELETE mark. If no messages are specified then the most recent DELETE message is restored. If ${autoprint} is on then the last restored message is listed."),
"ung[roup]",	CMD(unalias),		M|Z|RAWLIST,	1,	ARG_MAX,
	X("[ name ... ]\nEquivalent to unalias [ name ... ]."),
"unr[ead]",	CMD(unread),		STRLIST,	0,	0,
	X("[ message ... ]\nRemove the READ mark from the selected messages. If no messages are specified then the most recent DELETE message is restored."),
"uns[et]",	CMD(unset),		M|Z|RAWLIST,	0,	ARG_MAX,
	X("name ...\nEquivalent to set noname no... With no arguments all unset variables are listed. unset all lists all set and unset variables."),
"ve[rsion]",	CMD(version),		M|Z|NOLIST,	0,	0,
	X("\nList the current program version."),
"v[isual]",	CMD(visual),		I|MSGLIST,	0,	MMNORM,
	X("[ message ... ]\nPlace each selected message in a temporary file and invoke ${VISUAL} on the file. The mailbox is not affected."),
"w[rite]",	CMD(cmdwrite),	STRLIST,	0,	0,
	X("[ message ... ] file\nCopy the selected messages to file minus the message headers, marking the messages SAVE. write! overwrites file instead of appending."),
"x[it]",	CMD(cmdexit),		M|Z|NOLIST,	0,	0,
	X("\nEquivalent to exit."),
"z",		CMD(scroll),		M|STRLIST,	0,	0,
	X("[ + | - ]\nScroll the header summary forward (+ or no argument), or backward (-) by ${screen} lines."),
0,		0,			0,		0,	0, 0

};

/*
 * Escape commands -- sorted
 */

static const struct esc esctab[] =
{

"!",	X("command\tInvoke command."),
".",	X("\tSimulate EOF."),
":",	X("command\tExecute command level command."),
"_",	X("command\tEquivalent to ~_."),
"?",	X("[command]\tEscape command help."),
"A",	X("\tEquivalent to ~iSign."),
"F",	X("[message...]\tSame as ~f, but retain all headers."),
"M",	X("[message...]\tSame as ~m, but retain all headers."),
"a",	X("\tEquivalent to ~isign."),
"b",	X("name...\tAdd names to Bcc: list."),
"c",	X("name...\tAdd names to Cc: list."),
"d",	X("\tRead in ${DEAD}."),
"e",	X("\tInvoke ${EDITOR} on message."),
"f",	X("[message...]\tRead in messages."),
"g",	X("file\tInterpolate a multipart attachment from file."),
"h",	X("\tPrompt for To:, Subject:, and Cc: lists."),
"i",	X("name\tInsert ${name}<newline>."),
"m",	X("[message...]\tRead in messages, indented by ${indentprefix}."),
"p",	X("\tPrint the message buffer."),
"q",	X("\tQuit from input mode as if an interrupt was raised."),
"r",	X("file|!cmd\tRead file|pipeline into the message buffer."),
"s",	X("subject\tSet Subject:."),
"t",	X("name...\tAdd names to To: list."),
"v",	X("\tInvoke ${VISUAL} on message."),
"w",	X("file\tWrite message onto file without headers."),
"x",	X("\tLike ~q but message not saved in ${DEAD}."),
"<",	X("file\tEquivalent to ~rfile."),
"|",	X("command\tPipe the message through command."),
"~",	X("\tQuote a single tilde."),

 0, 0
 };


/*
 * Variables -- sorted
 */

static const struct var	vartab[] =
{

"CDPATH",	&state.var.cdpath,		D|E|N,	0,0,
	X(": separated list of directories to search for `cd' command."),
"DEAD",		&state.var.dead,		D|E|N,	"~/dead.letter",0,
	X("Save partial messages on interrupt or delivery error in this file."),
"EDITOR",	&state.var.editor,		D|E|N,	"ed",0,
	X("The utility invoked for the edit and ~e commands."),
"HOME",		&state.var.home,		D|E|N,	".",0,
	X("The user's home directory, also named \"~\"."),
"IMAP",		&state.var.imap,		D|E|N,	"~/.imap",0,
	X("IMAP host and authentication file.  Each line with `host name LOGIN name password' or `host name AUTHENTICATE method'."),
"LISTER",	&state.var.lister,		D|E|N,	"ls",0,
	X("The utility invoked for the folders command."),
"MAIL",		&state.var.mail,		E,	"",set_mail,
	X("The system mailbox path."),
"MAILCAP",	&state.var.mailcap,		E,	"~/.mailcap:/usr/local/etc/mailcap:/usr/etc/mailcap:/etc/mailcap:/etc/mail/mailcap:/usr/public/lib/mailcap",set_mailcap,
	X("A `:' separated list of MIME mailcap files. The get command matches attachment content types to ${MAILCAP} entries to determine any special command(s) for content viewing."),
"MAILRC",	&state.var.mailrc,		D|E|N,	"~/.mailrc",0,
	X("The user start-up file."),
"MBOX",		&state.var.mbox,		D|E|N,	"~/mbox",0,
	X("Save READ system mailbox messages in this file."),
"OLDPWD",	&state.var.pwd,			D|N,	0,0,
	X("The full path of the previous current working directory."),
"PAGER",	&state.var.pager,		D|E|N,	"more",0,
	X("The utility invoked for terminal output pagination."),
"PWD",		&state.var.pwd,			D|N,	"",set_pwd,
	X("The full path of the current working directory."),
"Replyall",	&state.var.flipr,		A,	0,0,
	X("flipr"),
"SHELL",	&state.var.shell,		D|N,	"",set_shell,
	X("The preferred command interpreter."),
"Sign",		&state.var.Sign,		0,	0,0,
	X("The string inserted by the ~A command."),
"VISUAL",	&state.var.visual,		D|E|N,	"vi",0,
	X("The utility invoked for the visual and ~v commands."),
"allnet",	&state.var.allnet,		0,	0,0,
	X("Treat network names with matching login components as identical."),
"append",	&state.var.append,		0,	0,0,
	X("Append messages at the end of ${MBOX} instead of the beginning."),
"ask",		&state.var.asksub,		A,	0,0,
	X("asksub"),
"askbcc",	&state.var.askbcc,		0,	0,set_askbcc,
	X("Prompt for a Bcc: line on outgoing mail if not already specified."),
"askcc",	&state.var.askcc,		0,	0,set_askcc,
	X("Prompt for a Cc: line on outgoing mail if not already specified."),
"askheaders",	&state.var.askheaders,		0,	0,set_askheaders,
	X("Prompt for this list of header names on outgoing mail if not already specified."),
"asksub",	&state.var.asksub,		0,	0,set_asksub,
	X("Prompt for a Subject: line on outgoing mail if not already specified."),
"attachments",	&state.var.attachments,		0,	0,0,
	X("Place relative ouput files for the get and Get commands in this directory."),
"autoinc",	&state.var.autoinc,		0,	0,0,
	X("Check for new mail at each prompt."),
"autoprint",	&state.var.autoprint,		0,	0,0,
	X("Print the next unREAD message after the delete and undelete commands."),
"bang",		&state.var.bang,		0,	0,0,
	X("Disable the special meaning of ! in the escape command."),
"cmd",		&state.var.cmd,			0,	0,0,
	X("The default command invoked by the pipe command."),
"coprocess",	&state.var.coprocess,		0,	0,set_coprocess,
	X("Run in mode where stdin and stdout are pipes."),
"crt",		(char**)&state.var.crt,		I,	0,set_crt,
	X("Pipe messages longer than this through ${PAGER}."),
"debug",	&state.var.debug,		0,	0,0,
	X("Enable verbose debugging diagnostics and do not deliver messages."),
"domain",	&state.var.domain,		0,	0,0,
	X("The local mail address domain."),
"dot",		&state.var.dot,			0,	0,0,
	X("Make . on a line by itself equivalent to EOF on mail input."),
"editheaders",	&state.var.editheaders,		0,	0,set_editheaders,
	X("Place this list of headers in files to be edited by ~e and ~v."),
"escape",	&state.var.escape,		N,	"~",0,
	X("The command escape character.  Null means no command escapes."),
"fixedheaders",	&state.var.fixedheaders,	0,	0,0,
	X("Add these header lines to outgoing messages."),
"flipr",	&state.var.flipr,		0,	0,0,
	X("Reverse the meanings of the reply and Reply commands."),
"folder",	&state.var.folder,		0,	0,0,
	X("The default directory for saving mail files, also named \"+\"."),
"followup",	&state.var.followup,		N,	0,0,
	X("The default file save directory for the followup and Followup commands."),
"header",	&state.var.header,		0,	"",0,
	X("Write a header summary when entering receive mode."),
"headerbotch",	&state.var.headerbotch,		0,	0,0,
	X("Ignore blank lines in message headers."),
"hold",		&state.var.hold,		0,	0,0,
	X("Preserve all READ messages in the system mailbox rather than in ${MBOX}."),
"hostname",	&state.var.hostname,		0,	"localhost",0,
	X("The local host name."),
"ignore",	&state.var.ignore,		0,	0,0,
	X("Ignore interrupts on message input."),
"ignoreeof",	&state.var.ignoreeof,		0,	0,0,
	X("Ignore the EOF char on message input. `.' or ~. must terminate the message."),
"imap",		&state.var.imap,		0,	0,0,
	X("IMAP host and authentication file.  Each line with `host name LOGIN name password' or `host name AUTHENTICATE method'."),
"inbox",	&state.var.inbox,		0,	"+inbox",0,
	X("mh incoming mailbox. If ${inbox} is a directory then SAVE messages are removed on exit."),
"indentprefix",	&state.var.indentprefix,	N,	"\t",0,
	X("Prefix this string to each line of messages inserted by ~m or ~M."),
"interactive",	&state.var.interactive,		0,	0,0,
	X("Interactive mode."),
"justcheck",	&state.var.justcheck,		C,	0,0,
	X("Exit 0 if there is new mail, 1 otherwise."),
"justfrom",	(char**)&state.var.justfrom,	I,	0,set_justfrom,
	X("Print the status and sender for the specified number of most recent messages and exit."),
"justheaders",	&state.var.justheaders,		C,	0,0,
	X("Print a new message header summary and exit."),
"keep",		&state.var.keep,		0,	0,0,
	X("Truncate an empty mailbox to zero length instead of removing it."),
"keepsave",	&state.var.keepsave,		0,	0,0,
	X("Keep SAVE system mailbox messages instead of deleting them."),
"local",	&state.var.local,		0,	0,set_list,
	X("List of domain name suffixes to delete from all addresses."),
"lock",		&state.var.lock,		0,	"5",0,
	X("System mailbox lock hang seconds delay. If the lock hangs longer than this then it is ignored -- just for the NFS lock daemon."),
"log",		&state.var.log,			N,	0,0,
	X("Log outgoing messages in this file."),
"master",	&state.var.master,		0,	_PATH_MASTER_RC,0,
	X("The master initialization file."),
"metoo",	&state.var.metoo,		0,	0,0,
	X("Don't delete the current user name when sending to an alias or replying."),
"more",		&state.var.more,		0,	0,set_more,
	X("Enable the internal pager using this string as the prompt."),
"news",		&state.var.news,		0,	0,set_news,
	X("Treat mail files as netnews article folders."),
"onehop",	&state.var.onehop,		0,	0,0,
	X("A no-op just for the standard."),
"outfolder",	&state.var.outfolder,		0,	0,0,
	X("Place relative output message record files in the folder directory."),
"page",		&state.var.page,		0,	0,0,
	X("Insert a <formfeed> between each message sent to the pipe command."),
"prompt",	&state.var.prompt,		N,	"? ",0,
	X("The interactive command mode prompt."),
"quiet",	&state.var.quiet,		0,	0,0,
	X("Suppress the opening message and version on program entry."),
"receive",	&state.var.receive,		R,	0,0,
	X("Set if currently in receive mode."),
"record",	&state.var.log,			A,	0,0,
	X("log"),
"replyall",	&state.var.flipr,		A,	0,0,
	X("flipr"),
"rule",		&state.var.rule,		0,	0,0,
	X("Place this string after ${editheaders} for the ~e and ~v commands."),
"save",		&state.var.save,		0,	"",0,
	X("Save messages in ${DEAD} on interrupt or delivery error."),
"screen",	(char**)&state.var.screen,	I,	"",set_screen,
	X("The number of header lines printed by the z command."),
"searchheaders",&state.var.searchheaders,	0,	0,0,
	X("Accept /header:string in message addresses."),
"sendheaders",	&state.var.sendheaders,		S,	0,0,
	X("Interpret headers in send mode messages."),
"sendmail",	&state.var.sendmail,		D|N,	"",set_sendmail,
	X("The utility invoked to deliver messages. smtp://[host] uses SMTP to host or to ${smtp} if host is omitted."),
"sendwait",	&state.var.sendwait,		0,	0,0,
	X("Wait for sendmail to complete before returning."),
"show-rcpt",	&state.var.showto,		A,	0,0,
	X("showto"),
"showto",	&state.var.showto,		0,	0,0,
	X("Show To: info instead of From if message sender is the current user."),
"sign",		&state.var.sign,		N,	0,0,
	X("The string inserted by the ~a command."),
"signature",	&state.var.signature,		N,	"~/.signature",0,
	X("Read the default value for ${sign} from this file."),
"smtp",		&state.var.smtp,		D|E|N,	"local",0,
	X("The SMTP server host name. Used by the duplicate command and sendmail=\"smtp://\"."),
"spam",		&state.var.spam,		0,	0,0,
	X("Mark candidate spam messages X. All spam matching patterns are case-insensitive ksh(1) patterns."),
"spamdelay",	(char**)&state.var.spamdelay,	I,	"3600",0,
	X("Maximum seconds delay between hops."),
"spamfrom",	&state.var.spamfrom,		N,	"-,delete,ignore,remove,unsubscribe",set_list,
	X("Case-insensitive candidate spammer address list."),
"spamfromok",	&state.var.spamfromok,		N,	0,set_list,
	X("Case-insensitive non-spammer address list."),
"spamlog",	&state.var.spamlog,		N,	0,0,
	X("Log system mailbox spam messages in this file."),
"spamok",	&state.var.spamfromok,		A,	0,set_list,
	X("spamfromok"),
"spamsub",	&state.var.spamsub,		N,	"ad,adv,advertisement,spam",set_list,
	X("Case-insensitive candidate spam Subject: word prefix list."),
"spamtest",	(char**)&state.var.spamtest,	I,	"0",0,
	X("Spam test bitmask -- see the source, Luke."),
"spamto",	&state.var.spamto,		N,	"-,*@(friend|money|suppressed|undisclosed)*,u,you",set_list,
	X("Case-insensitive candidate spam recipient address list."),
"spamtook",	&state.var.spamtook,		N,	0,set_list,
	X("Case-insensitive non-spam recipient address list."),
"spamvia",	&state.var.spamviaok,		N,	0,set_list,
	X("Case-insensitive spammer host hop address list."),
"spamviaok",	&state.var.spamvia,		N,	0,set_list,
	X("Case-insensitive non-spam host hop address list."),
"toplines",	(char**)&state.var.toplines,	I,	"",set_toplines,
	X("The number of lines written by the top command."),
"trace",	&state.var.trace,		0,	0,set_trace,
	X("Enable implementation dependent tracing."),
"user",		&state.var.user,		D|N,	"",set_user,
	X("The login name of the current user."),
"verbose",	&state.var.verbose,		0,	0,0,
	X("Enable verbose diagnostic messages."),
0,		0,				0,	0,0

};

/*
 * Standard header labels -- layout order
 */

static const struct lab	hdrtab[] =
{

"To: ", 	GTO,
"Subject: ", 	GSUB,
"Cc: ", 	GCC,
"Bcc: ", 	GBCC,
"Status: ",	GSTATUS,
0,		0

};

State_t		state =
{
	id,
	terms + 5,
	"",
	cmdtab,
	elementsof(cmdtab) - 1,
	esctab,
	elementsof(esctab) - 1,
	vartab,
	elementsof(vartab) - 1,
	hdrtab,
	elementsof(hdrtab) - 1,
	GTO,
};
