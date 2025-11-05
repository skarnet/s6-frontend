/* ISC license. */

#include <skalibs/buffer.h>
#include <skalibs/strerr.h>

#include "s6-frontend-internal.h"

#define SERVICE_HELP_MESSAGE "This is the service help message.\n"

int service_help (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, SERVICE_HELP_MESSAGE))
    strerr_diefu1sys(111, "write to stdout") ;
  return 0 ;
}
