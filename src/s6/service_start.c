/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-internal.h"

#define USAGE "s6 service start services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum service_start_golb_e
{
  SERVICE_START_GOLB_WAIT,
  SERVICE_START_GOLB_N
} ;

static gol_bool const service_start_golb[2] =
{
  { .so = 'W', .lo = "nowait", .set = 0, .mask = 1 << SERVICE_START_GOLB_WAIT },
  { .so = 'w', .lo = "wait", .set = 1, .mask = 1 << SERVICE_START_GOLB_WAIT }
} ;

int service_start (char const *const *argv)
{
  uint64_t golb = 0 ;
  PROG = "s6 service start" ;

  argv += gol_argv(argv, service_start_golb, 0, 0, 0, &golb, 0) ;
  if (!argv) dieusage() ;
  return 0 ;
}
