/* ISC license. */

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-internal.h"

#define HELP_MESSAGE "This is the main help message.\n"

int help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  return 0 ;
}
