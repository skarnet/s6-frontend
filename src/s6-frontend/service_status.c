/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 service status services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum golb_e
{
  GOLB_WAIT,
  GOLB_N
} ;

static gol_bool const rgolb[2] =
{
  { .so = 'w', .lo = "wait", .clear = 0, .set = 1 << GOLB_WAIT }
} ;

int service_status (char const *const *argv)
{
  uint64_t wgolb = 0 ;
  PROG = "s6 service status" ;

  argv += gol_argv(argv, rgolb, 0, 0, 0, &wgolb, 0) ;
  if (!argv) dieusage() ;
  return 0 ;
}
