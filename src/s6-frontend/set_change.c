/* ISC license. */

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_DRYRUN = 0x02,
  GOLB_FAIL_ON_DEPS = 0x04,
  GOLB_PULL_DEPS = 0x08,
} ;

enum gola_e
{
  GOLA_SET,
  GOLA_N
} ;

static void set_change (char const *const *argv, char const *newrx, char const *cmd) gccattr_noreturn ;
static void set_change (char const *const *argv, char const *newrx, char const *cmd)
{
  static gol_bool const rgolb[] =
  {
    { .so = 0, .lo = "no-dry-run", .clear = GOLB_DRYRUN, .set = 0 },
    { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN },
    { .so = 'I', .lo = "no-fail-on-dependencies", .clear = GOLB_FAIL_ON_DEPS, .set = 0 },
    { .so = 'i', .lo = "fail-on-dependencies", .clear = 0, .set = GOLB_FAIL_ON_DEPS },
    { .so = 'P', .lo = "no-pull-dependencies", .clear = GOLB_PULL_DEPS, .set = 0 },
    { .so = 'p', .lo = "pull-dependencies", .clear = 0, .set = GOLB_PULL_DEPS },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 's', .lo = "set", .i = GOLA_SET },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { [GOLA_SET] = "current" } ;
  unsigned int m = 0 ;

  unsigned int argc ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

  if (!*argv) strerr_die(100, "usage: ", "s6 set ", cmd, " [ --dry-run ] [ --fail-on-dependencies | --pull-dependencies ] [ --set=setname ] services...") ;
  argc = env_len(argv) ;
  char fmtv[UINT_FMT] ;
  char const *newargv[13 + argc] ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-change" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  if (wgolb & GOLB_DRYRUN) newargv[m++] = "-n" ;
  if (!strcmp(newrx, "always")) newargv[m++] = "-e" ;
  if (wgolb & GOLB_FAIL_ON_DEPS) newargv[m++] = "-i" ;
  if (wgolb & GOLB_PULL_DEPS) newargv[m++] = "-p" ;
  newargv[m++] = "--" ;
  newargv[m++] = wgola[GOLA_SET] ;
  newargv[m++] = newrx ;
  for (unsigned int i = 0 ; i < argc ; i++) newargv[m++] = argv[i] ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}

void set_disable (char const *const *argv)
{
  set_change(argv, "usable", "disable") ;
}

void set_enable (char const *const *argv)
{
  set_change(argv, "active", "enable") ;
}

void set_mask (char const *const *argv)
{
  set_change(argv, "masked", "mask") ;
}

void set_unmask (char const *const *argv)
{
  set_change(argv, "usable", "unmask") ;
}

void set_make_essential (char const *const *argv)
{
  set_change(argv, "always", "make-essential") ;
}
