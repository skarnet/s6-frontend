/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>
#include <skalibs/env.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_DRYRUN = 0x01,
} ;

enum gola_e
{
  GOLA_TIMEOUT,
  GOLA_N
} ;

void live_reload (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int timeout = 0 ;
  unsigned int m = 0 ;
  unsigned int argc ;
  char fmtv[UINT_FMT] ;
  char fmtt[UINT_FMT] ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (!*argv) strerr_die(100, "usage: ", "s6 live reload [ -n ] [ -t timeout ] services...") ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }
  argc = env_len(argv) ;

  char const *newargv[11 + argc] ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  if (timeout)
  {
    fmtt[uint_fmt(fmtt, timeout)] = 0 ;
    newargv[m++] = "-t" ;
    newargv[m++] = fmtt ;
  }
  if (wgolb & GOLB_DRYRUN) newargv[m++] = "-n1" ;
  newargv[m++] = "-l" ;
  newargv[m++] = g->dirs.live ;
  newargv[m++] = "-b" ;
  newargv[m++] = "--" ;
  newargv[m++] = "reload" ;
  for (unsigned int i = 0 ; i < argc ; i++) newargv[m++] = argv[i] ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
