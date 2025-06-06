/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-internal.h"

#define USAGE "s6 service status services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum service_status_golb_e
{
  SERVICE_STATUS_GOLB_WAIT,
  SERVICE_STATUS_GOLB_N
} ;

static gol_bool const service_status_golb[2] =
{
  { .so = 'w', .lo = "wait", .set = 1, .mask = 1 << SERVICE_STATUS_GOLB_WAIT }
} ;

int service_status (char const *const *argv)
{
  uint64_t golb = 0 ;
  PROG = "s6 service status" ;

  argv += gol_argv(argv, service_status_golb, 0, 0, 0, &golb, 0) ;
  if (!argv) dieusage() ;
  return 0 ;
}
