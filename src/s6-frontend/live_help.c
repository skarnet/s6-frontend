/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-frontend-internal.h"

#define LIVE_HELP_MESSAGE "This is the live help message.\n"

void live_help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, LIVE_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  _exit(0) ;
}
