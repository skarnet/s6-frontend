/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_DRYRUN = 0x01,
  GOLB_INCLUDE_ESSENTIALS = 0x02,
} ;

enum gola_e
{
  GOLA_TIMEOUT,
  GOLA_N
} ;

void live_stop_everything (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN },
    { .so = 'e', .lo = "without-essentials", .clear = GOLB_INCLUDE_ESSENTIALS, .set = 0 },
    { .so = 'E', .lo = "with-essentials", .clear = 0, .set = GOLB_INCLUDE_ESSENTIALS },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int timeout = 0 ;
  unsigned int m = 0 ;
  char const *newargv[12] ;
  char fmtv[UINT_FMT] ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  if (timeout)
  {
    newargv[m++] = "-t" ;
    newargv[m++] = wgola[GOLA_TIMEOUT] ;
  }
  if (wgolb & GOLB_DRYRUN) newargv[m++] = "-n1" ;
  newargv[m++] = "-l" ;
  newargv[m++] = g->dirs.live ;
  newargv[m++] = wgolb & GOLB_INCLUDE_ESSENTIALS ? "-bDa" : "-bda" ;
  newargv[m++] = "--" ;
  newargv[m++] = "change" ;
  newargv[m++] = 0 ;

  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
