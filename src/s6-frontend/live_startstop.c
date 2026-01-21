/* ISC license. */

#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <skalibs/posixplz.h>
#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>
#include <skalibs/env.h>
#include <skalibs/djbunix.h>

#include <s6-rc/config.h>
#include <s6-rc/s6rc-utils.h>

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

static unsigned int parse_options (char const *const *argv, uint64_t *wgolb, unsigned int *timeout)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  } ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int argc = GOL_argv(argv, rgolb, rgola, wgolb, wgola) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], timeout))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }
  return argc ;
}

static int run_s6rc_change (char const *const *services, unsigned int n, int h, int dryrun, unsigned int timeout, unsigned int nstate, char const *dbfn)
{
  pid_t pid ;
  int wstat ;
  unsigned int m = 0 ;
  char const *argv[12 + n] ;
  char fmtv[UINT_FMT] ;
  char fmtt[UINT_FMT] ;
  unsigned char oldstate[nstate] ;
  if (!s6rc_live_state_read(g->dirs.live, oldstate, nstate))
    strerr_diefu2sys(111, "read state in ", g->dirs.live) ;

  argv[m++] = S6RC_EXTBINPREFIX "s6-rc" ;
  argv[m++] = "--no-lock" ;  /* we already hold the lock */
  argv[m++] = "-v" ;
  fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
  argv[m++] = fmtv ;
  if (timeout)
  {
    argv[m++] = "-t" ;
    fmtt[uint_fmt(fmtt, timeout)] = 0 ;
    argv[m++] = fmtt ;
  }
  if (dryrun) argv[m++] = "-n1" ;
  argv[m++] = "-l" ;
  argv[m++] = g->dirs.live ;
  argv[m++] = "--" ;
  argv[m++] = h ? "start" : "stop" ;
  while (*services) argv[m++] = *services++ ;
  argv[m++] = 0 ;
  pid = main_spawn(argv) ;
  if (!pid) strerr_diefu2sys(111, "spawn ", argv[0]) ;
  if (wait_pid(pid, &wstat) <= 0) strerr_diefu1sys(111, "wait for s6-rc") ;
  if (wait_estatus(wstat) && g->verbosity)
  {
    unsigned char newstate[nstate] ;
    if (!s6rc_live_state_read(g->dirs.live, newstate, nstate))
      strerr_diefu2sys(111, "read state in ", g->dirs.live) ;
    if (g->verbosity) s6f_report_state_change(n, oldstate, newstate, dbfn, h) ;
  }

  return wait_estatus(wstat) ;
}

static int live_startstop (char const *const *argv, int h)
{
  uint64_t wgolb = 0 ;
  unsigned int timeout = 0 ;
  uint32_t nlong, nshort ;
  int livelock, compiledlock ;
  size_t livelen = strlen(g->dirs.live) ;

  argv += parse_options(argv, &wgolb, &timeout) ;
  if (!*argv) strerr_dieusage(100, h ? "s6 live start services..." : "s6 live stop services...") ;

  char dbfn[livelen + 10] ;
  memcpy(dbfn, g->dirs.live, livelen) ;
  memcpy(dbfn + livelen, "/compiled", 10) ;
  if (!s6rc_lock(g->dirs.live, 2, &livelock, dbfn, 1, &compiledlock, 1))
    strerr_diefu2sys(111, "take lock in ", g->dirs.live) ;
  if (!s6rc_live_state_size(g->dirs.live, &nlong, &nshort))
    strerr_diefu2sys(111, "read state size in ", g->dirs.live) ;

  return run_s6rc_change(argv, env_len(argv), h, !!(wgolb & GOLB_DRYRUN), timeout, nlong + nshort, dbfn) ;
}

void live_start (char const *const *argv)
{
  _exit(live_startstop(argv, 1)) ;
}

void live_stop (char const *const *argv)
{
  _exit(live_startstop(argv, 0)) ;
}

void live_restart (char const *const *argv)
{
  uint64_t wgolb = 0 ;
  unsigned int timeout = 0 ;
  int e ;
  unsigned int argc ;
  uint32_t nlong, nshort ;
  int livelock, compiledlock ;
  size_t livelen = strlen(g->dirs.live) ;
 
  argv += parse_options(argv, &wgolb, &timeout) ;
  if (!*argv) strerr_dieusage(100, "s6 live restart services...") ;

  argc = env_len(argv) ;
  char dbfn[livelen + 10] ;
  memcpy(dbfn, g->dirs.live, livelen) ;
  memcpy(dbfn + livelen, "/compiled", 10) ;
  if (!s6rc_lock(g->dirs.live, 2, &livelock, dbfn, 1, &compiledlock, 1))
    strerr_diefu2sys(111, "take lock in ", g->dirs.live) ;
  if (!s6rc_live_state_size(g->dirs.live, &nlong, &nshort))
    strerr_diefu2sys(111, "read state size in ", g->dirs.live) ;

  e = run_s6rc_change(argv, argc, 0, !!(wgolb & GOLB_DRYRUN), timeout, nlong + nshort, dbfn) ;
  if (e) _exit(e) ;
  e = run_s6rc_change(argv, argc, 1, !!(wgolb & GOLB_DRYRUN), timeout, nlong + nshort, dbfn) ;
  _exit(e) ;
}
