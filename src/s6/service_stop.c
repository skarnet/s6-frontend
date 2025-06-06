/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-internal.h"

#define USAGE "s6 service stop services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum service_stop_golb_e
{
  SERVICE_STOP_GOLB_WAIT,
  SERVICE_STOP_GOLB_N
} ;

static gol_bool const service_stop_golb[2] =
{
  { .so = 'W', .lo = "nowait", .set = 0, .mask = 1 << SERVICE_STOP_GOLB_WAIT },
  { .so = 'w', .lo = "wait", .set = 1, .mask = 1 << SERVICE_STOP_GOLB_WAIT }
} ;

int service_stop (char const *const *argv)
{
  uint64_t golb = 0 ;
  PROG = "s6 service stop" ;

  argv += gol_argv(argv, service_stop_golb, 0, 0, 0, &golb, 0) ;
  if (!argv) dieusage() ;
  return 0 ;
}
