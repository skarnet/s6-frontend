/* ISC license. */

#include <string.h>

#include <skalibs/strerr.h>
#include <skalibs/djbunix.h>
#include <skalibs/unix-transactional.h>

#include "s6f.h"

int s6f_lock (char const *stmpdir, int w)
{
  int fdstmp = s6f_confdir_open(stmpdir, 1) ;
  int fd = openc_truncat(fdstmp, "lock") ;
  if (fd == -1) strerr_diefu4sys(111, "open ", stmpdir, "/lock", " for writing") ;
  fd_close(fdstmp) ;
  if (fd_lock(fd, w, 0) < 1)
    strerr_diefu3sys(111, "lock ", stmpdir, "/lock") ;
  return fd ;
}
