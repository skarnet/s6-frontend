/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-frontend-internal.h"

#define SYSTEM_HELP_MESSAGE "This is the system help message.\n"

void system_help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, SYSTEM_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  _exit(0) ;
}
