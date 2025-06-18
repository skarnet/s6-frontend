/* ISC license. */

#include <skalibs/bsdsnowflake.h>

#include <unistd.h>
#include <errno.h>

#include <skalibs/stat.h>
#include <skalibs/strerr.h>
#include <skalibs/djbunix.h>
#include <skalibs/unix-transactional.h>

#include "s6f.h"

int s6f_confdir_open (char const *s, int flagcreate)
{
  struct stat st ;
  int fd = open_read(s) ;
  if (fd == -1)
  {
    if (errno != ENOENT || !flagcreate)
      strerr_diefu3sys(111, "open ", s, " for reading") ;
    if (flagcreate)
    {
      s6f_mkdirp(s, 02755) ;
      fd = open_read(s) ;
      if (fd == -1)
        strerr_diefu3sys(111, "open ", s, " for reading") ;
    }
  }
  if (fstat(fd, &st) == -1)
    strerr_diefu2sys(111, "fstat ", s) ;
  if (!S_ISDIR(st.st_mode))
    strerr_dief2x(111, s, " is not a directory") ;
  if (access_at(fd, ".", R_OK|W_OK, 1) == -1)
    strerr_diefu2sys(111, "access ", s) ;
  return fd ;
}
