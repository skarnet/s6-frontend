/* ISC license. */

#include <string.h>

#include <skalibs/posixplz.h>
#include <skalibs/strerr.h>

#include "s6f.h"

void s6f_mkdirp (char const *s, mode_t mode)
{
  size_t len = strlen(s) ;
  char tmp[len + 1] ;
  memcpy(tmp, s, len + 1) ;
  if (mkdirp(tmp, mode) == -1)
    strerr_diefu2sys(111, "mkdir ", tmp) ;
}
