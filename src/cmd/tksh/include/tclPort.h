#include "tksh.h"
#include <dirent.h>
#include <pwd.h>
#include <times.h>
#include <sys/param.h>

#ifndef MAXPATHLEN
#   ifdef PATH_MAX
#       define MAXPATHLEN PATH_MAX
#   else
#       define MAXPATHLEN 2048
#   endif
#endif

#ifndef NBBY
#   define NBBY	(CHAR_BIT*sizeof(fd_mask))
#endif
#ifndef howmany
#   define howmany(x, y) (((x)+((y)-1))/(y))
#endif
#ifndef MASK_SIZE
#   define MASK_SIZE howmany(FD_SETSIZE, NFDBITS)
#endif
#ifndef SELECT_MASK
#   define SELECT_MASK fd_set
#endif


/* this should be done by iffe */
#if sun
#define HAVE_GETTIMEOFDAY
#else
#define HAVE_TIMEZONE_VAR
#endif

#define TclpGetDate(t,u) ((u) ? gmtime((t)) : localtime((t)))
#define TclStrftime(s,m,f,t) (strftime((s),(m),(f),(t)))

#define sprintf		_stdsprintf
