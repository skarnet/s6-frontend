/* ISC license. */

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-internal.h"

int version (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, "s6-frontend v" S6_FRONTEND_VERSION "\n"))
    strerr_diefu1sys(111, "write to stdout") ;
  return 0 ;
}
