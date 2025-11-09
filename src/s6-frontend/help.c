/* ISC license. */

#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

#define MAIN_HELP_MESSAGE "This is the main help message.\n"

void version (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, "s6-frontend v" S6_FRONTEND_VERSION "\n"))
    strerr_diefu1sys(111, "write to stdout") ;
  _exit(0) ;
}

void help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, MAIN_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  _exit(0) ;
}
