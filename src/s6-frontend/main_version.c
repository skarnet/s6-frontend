/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

void main_version (char const *const *argv)
{
  if (!buffer_putsflush(buffer_1, "s6-frontend v" S6_FRONTEND_VERSION "\n"))
    strerr_diefu1sys(111, "write to stdout") ;
  (void)argv ;
  _exit(0) ;
}
