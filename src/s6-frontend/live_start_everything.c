/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_DRYRUN = 0x01,
} ;

enum gola_e
{
  GOLA_TIMEOUT,
  GOLA_DEFBUNDLE,
  GOLA_N
} ;

void exec_live_start_everything (unsigned int timeout, char const *defbundle, unsigned int options)
{
  unsigned int m = 0 ;
  char const *argv[14] ;
  char fmtv[UINT_FMT] ;
  char fmtt[UINT_FMT] ;

  argv[m++] = S6RC_EXTBINPREFIX "s6-rc" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    argv[m++] = "-v" ;
    argv[m++] = fmtv ;
  }
  if (timeout)
  {
    fmtt[uint_fmt(fmtt, timeout)] = 0 ;
    argv[m++] = "-t" ;
    argv[m++] = fmtt ;
  }
  if (options & 1) argv[m++] = "-n1" ;
  if (options & 2) argv[m++] = "-p" ;
  argv[m++] = "-l" ;
  argv[m++] = g->dirs.live ;
  argv[m++] = "-b" ;
  argv[m++] = "--" ;
  argv[m++] = "change" ;
  argv[m++] = defbundle ;
  argv[m++] = 0 ;
  main_exec(argv) ;
}

static void live_start_default (char const *const *argv, int prune) gccattr_noreturn ;
static void live_start_default (char const *const *argv, int prune)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
    { .so = 'D', .lo = "default-bundle", .i = GOLA_DEFBUNDLE },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int timeout = 0 ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }
  exec_live_start_everything(timeout, wgola[GOLA_DEFBUNDLE], (wgolb & GOLB_DRYRUN) | (!!prune << 1) ) ;
}

void live_start_everything (char const *const *argv)
{
  live_start_default(argv, 0) ;
}

void live_reset (char const *const *argv)
{
  live_start_default(argv, 1) ;
}
