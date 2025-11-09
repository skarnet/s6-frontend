/* ISC license. */

#include <unistd.h>

#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 live status services..."
#define dieusage() strerr_dieusage(100, USAGE)

void live_status (char const *const *argv)
{
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!argv) dieusage() ;
  _exit(0) ;
}
