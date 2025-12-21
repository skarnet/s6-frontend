/* ISC license. */

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_IGNORE_DEPENDENCIES = 0x02,
  GOLB_DRYRUN = 0x04,
} ;

enum gola_e
{
  GOLA_FORCELEVEL,
  GOLA_N
} ;

static void set_change (char const *const *argv, char const *newsub, char const *cmd) gccattr_noreturn ;
static void set_change (char const *const *argv, char const *newsub, char const *cmd)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'f', .lo = "ignore-dependencies", .clear = 0, .set = GOLB_IGNORE_DEPENDENCIES },
    { .so = 'n', .lo = "dry-run", .clear = 0, .set = GOLB_DRYRUN }
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 'I', .lo = "if-dependencies-found", .i = GOLA_FORCELEVEL }
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  unsigned int argc ;
  char const *wgola[GOLA_N] = { 0 } ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (!*argv) strerr_die(100, "usage: ", "s6 set ", cmd, " [ --ignore-dependencies ] [ --dry-run ] [ --if-dependencies-found=fail|warn|pull ] services...") ;
  argc = env_len(argv) ;
  if (wgola[GOLA_FORCELEVEL])
  {
    if (strcmp(wgola[GOLA_FORCELEVEL], "fail")
     && strcmp(wgola[GOLA_FORCELEVEL], "pull")
     && strcmp(wgola[GOLA_FORCELEVEL], "warn"))
      strerr_dief1x(100, "--if-dependencies-found= argument must be fail, warn or pull") ;
  }

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
  if (wgolb & GOLB_IGNORE_DEPENDENCIES)
    newargv[m++] = "-f" ;
  if (wgolb & GOLB_DRYRUN)
    newargv[m++] = "-n" ;
  if (wgola[GOLA_FORCELEVEL])
  {
    newargv[m++] = "-I" ;
    newargv[m++] = wgola[GOLA_FORCELEVEL] ;
  }
  newargv[m++] = "--" ;
  newargv[m++] = "current" ;
  newargv[m++] = newsub ;
  for (unsigned int i = 0 ; i < argc ; i++)
    newargv[m++] = argv[i] ;
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
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
