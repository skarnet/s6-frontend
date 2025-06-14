/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>
#include <skalibs/env.h>

#include <s6-rc/config.h>

#include "s6-internal.h"

#define USAGE "s6 service start services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum service_start_golb_e
{
  SERVICE_START_GOLB_DRYRUN,
  SERVICE_START_GOLB_WAIT,
  SERVICE_START_GOLB_N
} ;

static gol_bool const service_start_golb[3] =
{
  { .so = 'n', .lo = "dryrun", .set = 1, .mask = 1 << SERVICE_START_GOLB_DRYRUN },
  { .so = 'W', .lo = "nowait", .set = 0, .mask = 1 << SERVICE_START_GOLB_WAIT },
  { .so = 'w', .lo = "wait", .set = 1, .mask = 1 << SERVICE_START_GOLB_WAIT }
} ;

int service_start (char const *const *argv)
{
  size_t argc = env_len(argv) ;
  uint64_t golb = 0 ;
  PROG = "s6 service start" ;

  argv += gol_argv(argv, service_start_golb, 0, 0, 0, &golb, 0) ;
  if (!argv) dieusage() ;

  {
    size_t m = 0 ;
    char const *newargv[10 + argc] ;
    char fmtv[UINT_FMT] ;

    newargv[m++] =  S6RC_EXTBINPREFIX "s6-rc" ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    if (golb & 1 << SERVICE_START_GOLB_DRYRUN)
    {
      newargv[m++] = "-n" ;
      newargv[m++] = "1" ;
    }
    newargv[m++] = "--" ;
  }
  return 0 ;
}
