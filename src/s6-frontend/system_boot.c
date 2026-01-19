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

static void system_init_fill (char const **argv, char const *prefix)
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

void system_init (char const *const *argv)
{
  char const *wgola[GOLA_N] = { 0 } ;
  char const *newargv[13] ;
  argv += gol_argv(argv, 0, 0, rgola, 1, 0, wgola) ;
  system_init_fill(newargv, wgola[GOLA_PREFIX]) ;
  main_exec(newargv) ;
}

static void system_init_spawn (char const *prefix)
{
  pid_t pid ;
  int wstat ;
  char const *newargv[13] ;
  system_init_fill(newargv, prefix) ;
  pid = xmspawn_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n, 0, 0, 0) ;
  if (wait_pid(pid, &wstat) == -1)
    strerr_diefu2sys(111, "wait for ", newargv[0]) ;
  if (wait_estatus(wstat))
    strerr_dief2x(wait_estatus(wstat), newargv[0], " failed") ;
}

void system_boot (char const *const *argv)
{
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  unsigned int timeout = 0 ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
    strerr_dief1x(100, "timeout must be an unsigned integer (in milliseconds)") ;
  }
  system_init_spawn(wgola[GOLA_PREFIX]) ;
  exec_live_start_everything(timeout, !!(wgolb & GOLB_DRYRUN), wgola[GOLA_DEFBUNDLE]) ;
}
