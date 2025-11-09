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
#include <skalibs/cspawn.h>
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

int live_startstop (char const *const *argv, int h)
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
  uint64_t wgolb = 0 ;
  unsigned int t = 0 ;
  size_t argc = env_len(argv) ;
  pid_t pid ;
  int wstat ;
  uint32_t nlong, nshort, n ;
  int livelock, compiledlock ;
  size_t livelen = strlen(g->dirs.live) ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (!*argv) strerr_dieusage(100, h ? "s6 live start services..." : "s6 live stop services...") ;

  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &t))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }
  s6f_lock(g->dirs.stmp, 0) ;  /* leaks, it's fine */
  char dbfn[livelen + 10] ;
  memcpy(dbfn, g->dirs.live, livelen) ;
  memcpy(dbfn + livelen, "/compiled", 10) ;
  if (!s6rc_lock(g->dirs.live, 2, &livelock, dbfn, 1, &compiledlock, 1))
    strerr_diefu2sys(111, "take lock in ", g->dirs.live) ;
  if (!s6rc_live_state_size(g->dirs.live, &nlong, &nshort))
    strerr_diefu2sys(111, "read state size in ", g->dirs.live) ;
  n = nlong + nshort ;
  unsigned char oldstate[n] ;
  if (!s6rc_live_state_read(g->dirs.live, oldstate, n))
    strerr_diefu2sys(111, "read state in ", g->dirs.live) ;

  {
    size_t m = 0 ;
    char const *newargv[13 + argc] ;
    char fmtv[UINT_FMT] ;
    char fmtt[UINT_FMT] ;

    newargv[m++] = S6RC_EXTBINPREFIX "s6-rc" ;
    newargv[m++] = "-X" ;  /* we already took the locks */
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    if (t)
    {
      newargv[m++] = "-t" ;
      newargv[m++] = fmtt ;
      fmtt[uint_fmt(fmtt, t)] = 0 ;
    }
    if (wgolb & GOLB_DRYRUN)
    {
      newargv[m++] = "-n" ;
      newargv[m++] = "1" ;
    }
    newargv[m++] = "-l" ;
    newargv[m++] = g->dirs.live ;
    newargv[m++] = "--" ;
    newargv[m++] = h ? "start" : "stop" ;
    while (*argv) newargv[m++] = *argv++ ;
    newargv[m++] = 0 ;
    pid = cspawn(newargv[0], newargv, (char const *const *)environ, CSPAWN_FLAGS_SIGBLOCKNONE, 0, 0) ;
    if (!pid) strerr_diefu2sys(111, "spawn ", newargv[0]) ;
  }
  if (wait_pid(pid, &wstat) <= 0) strerr_diefu1sys(111, "wait for s6-rc") ;
  if ((WIFEXITED(wstat) && !WEXITSTATUS(wstat)) || !g->verbosity)
    return wait_estatus(wstat) ;

  unsigned char newstate[n] ;
  if (!s6rc_live_state_read(g->dirs.live, newstate, n))
    strerr_diefu2sys(111, "read state in ", g->dirs.live) ;
  if (g->verbosity) s6f_report_state_change(n, oldstate, newstate, dbfn, h) ;
  return wait_estatus(wstat) ;
}

void service_start (char const *const *argv)
{
  _exit(live_startstop(argv, 1)) ;
}

void service_stop (char const *const *argv)
{
  _exit(live_startstop(argv, 0)) ;
}
