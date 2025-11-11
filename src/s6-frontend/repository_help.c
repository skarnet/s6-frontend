/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-frontend-internal.h"

#define REPOSITORY_HELP_MESSAGE "This is the live help message.\n"

void repository_help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, REPOSITORY_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  _exit(0) ;
}
