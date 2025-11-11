/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-frontend-internal.h"

#define SET_HELP_MESSAGE "This is the set help message.\n"

void set_help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, SET_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  _exit(0) ;
}
