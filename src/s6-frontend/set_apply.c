/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>
#include <skalibs/djbunix.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_BLOCK = 0x01,
} ;

enum gola_e
{
  GOLA_DEFBUNDLE,
  GOLA_CONVFILE,
  GOLA_SET,
  GOLA_TIMEOUT,
  GOLA_N
} ;

void set_apply (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'b', .lo = "block", .clear = 0, .set = GOLB_BLOCK },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 'D', .lo = "default-bundle", .i = GOLA_DEFBUNDLE },
    { .so = 'f', .lo = "conversion-file", .i = GOLA_CONVFILE },
    { .so = 's', .lo = "set", .i = GOLA_SET },
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int timeout = 0 ;
  unsigned int m = 0 ;
  int wstat ;
  pid_t pid ;
  char const *wgola[GOLA_N] = { 0 } ;
  char const *newargv[15] ;
  char fmtv[UINT_FMT] ;
  char fmtt[UINT_FMT] ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  wgola[GOLA_SET] = "current" ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "timeout must be an integer (milliseconds)") ;
  }

 /* set commit */
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-commit" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = "-D" ;
  newargv[m++] = wgola[GOLA_DEFBUNDLE] ;
  if (g->fdhuser && g->fdhuser[0])
  {
    newargv[m++] = "-h" ;
    newargv[m++] = g->fdhuser ;
  }
  newargv[m++] = "--" ;
  newargv[m++] = wgola[GOLA_SET] ;
  newargv[m++] = 0 ;

  pid = main_spawn(newargv) ;
  if (wait_pid(pid, &wstat) == -1)
    strerr_diefusys(111, "wait for ", newargv[0]) ;
  if (wait_estatus(wstat))
    strerr_dief(wait_estatus(wstat), newargv[0], " failed") ;

 /* live install */
  m = 0 ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-install" ;
  if (g->verbosity != 1)
  {
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = "-c" ;
  newargv[m++] = g->dirs.boot ;
  newargv[m++] = "-l" ;
  newargv[m++] = g->dirs.live ;
  
  if (wgolb & GOLB_BLOCK) newargv[m++] = "-b" ;
  if (wgola[GOLA_CONVFILE])
  {
    newargv[m++] = "-f" ;
    newargv[m++] = wgola[GOLA_CONVFILE] ;
  }
  newargv[m++] = "--" ;
  newargv[m++] = wgola[GOLA_SET] ;
  newargv[m++] = 0 ;

  pid = main_spawn(newargv) ;
  if (wait_pid(pid, &wstat) == -1)
    strerr_diefusys(111, "wait for ", newargv[0]) ;
  if (wait_estatus(wstat))
    strerr_dief(wait_estatus(wstat), newargv[0], " failed") ;

 /* live reset */
  m = 0 ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc" ;
  if (g->verbosity != 1)
  {
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  if (timeout)
  {
    fmtt[uint_fmt(fmtt, timeout)] = 0 ;
    newargv[m++] = "-t" ;
    newargv[m++] = fmtt ;
  }
  newargv[m++] = "-pe" ;
  newargv[m++] = "-l" ;
  newargv[m++] = g->dirs.live ;
  if (wgolb & GOLB_BLOCK) newargv[m++] = "-b" ;
  newargv[m++] = "--" ;
  newargv[m++] = "change" ;
  newargv[m++] = wgola[GOLA_DEFBUNDLE] ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
