/* ISC license. */

#include <unistd.h>
#include <sys/wait.h>

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>
#include <skalibs/cspawn.h>
#include <skalibs/djbunix.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_DRYRUN = 0x01,
} ;

enum gola_e
{
  GOLA_PREFIX,
  GOLA_TIMEOUT,
  GOLA_DEFBUNDLE,
  GOLA_N
} ;

static gol_bool const rgolb[] =
{
  { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN },
} ;
static gol_arg const rgola[] =
{
  { .so = 'p', .lo = "prefix", .i = GOLA_PREFIX },
  { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  { .so = 'D', .lo = "default-bundle", .i = GOLA_DEFBUNDLE },
} ;

static void live_init_fill (char const **argv, char const *prefix)
{
  unsigned int m = 0 ;
  argv[m++] = S6RC_EXTBINPREFIX "s6-rc-init" ;
  if (prefix)
  {
    argv[m++] = "-p" ;
    argv[m++] = prefix ;
  }
  argv[m++] = "-l" ;
  argv[m++] = g->dirs.live ;
  argv[m++] = "-c" ;
  argv[m++] = g->dirs.boot ;
  argv[m++] = "-b" ;
  argv[m++] = "--" ;
  argv[m++] = g->dirs.scan ;
  argv[m++] = 0 ;
}

void live_init (char const *const *argv)
{
  char const *wgola[GOLA_N] = { 0 } ;
  char const *newargv[13] ;
  argv += gol_argv(argv, 0, 0, rgola, 1, 0, wgola) ;
  live_init_fill(newargv, wgola[GOLA_PREFIX]) ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

static void live_init_spawn (char const *prefix)
{
  pid_t pid ;
  int wstat ;
  char const *newargv[13] ;
  live_init_fill(newargv, prefix) ;
  pid = xmspawn_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n, 0, 0, 0) ;
  if (wait_pid(pid, &wstat) == -1)
    strerr_diefu2sys(111, "wait for ", newargv[0]) ;
  if (wait_estatus(wstat))
    strerr_dief2x(wait_estatus(wstat), newargv[0], " failed") ;
}

static void exec_live_start_everything (unsigned int timeout, int dryrun, char const *defbundle) gccattr_noreturn ;
static void exec_live_start_everything (unsigned int timeout, int dryrun, char const *defbundle)
{
  unsigned int m = 0 ;
  char const *argv[13] ;
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
  if (dryrun) argv[m++] = "-n1" ;
  argv[m++] = "-l" ;
  argv[m++] = g->dirs.live ;
  argv[m++] = "-b" ;
  argv[m++] = "--" ;
  argv[m++] = "start" ;
  argv[m++] = defbundle ;
  argv[m++] = 0 ;
  xmexec_n(argv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

void live_start_everything (char const *const *argv)
{
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int timeout = 0 ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  argv += gol_argv(argv, rgolb, 1, rgola + 1, 2, &wgolb, wgola) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }
  exec_live_start_everything(timeout, !!(wgolb & GOLB_DRYRUN), wgola[GOLA_DEFBUNDLE]) ;
}

void live_boot (char const *const *argv)
{
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int timeout = 0 ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
    strerr_dief1x(100, "--timeout= argument must be an unsigned integer (in milliseconds)") ;
  }
  live_init_spawn(wgola[GOLA_PREFIX]) ;
  exec_live_start_everything(timeout, !!(wgolb & GOLB_DRYRUN), wgola[GOLA_DEFBUNDLE]) ;
}
