/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-frontend-internal.h"

#define PROCESS_HELP_MESSAGE "This is the process help message.\n"

void process_help (char const *const *argv)
{
  if (!buffer_putsflush(buffer_1, PROCESS_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  (void)argv ;
  _exit(0) ;
}
