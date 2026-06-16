/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/gol.h>

#include <execline/config.h>

#include <s6-rc/config.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_FIXUP = 0x01,
  GOLB_FORCE_ESSENTIAL = 0x02,
  GOLB_FIX = 0x04,
} ;

enum gola_e
{
  GOLA_SET,
  GOLA_N
} ;

void set_check (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'E', .lo = "no-force-essential", .clear = GOLB_FORCE_ESSENTIAL, .set = 0 },
    { .so = 'e', .lo = "force-essential", .clear = 0, .set = GOLB_FORCE_ESSENTIAL },
    { .so = 'd', .lo = "down", .clear = GOLB_FIXUP, .set = 0 },
    { .so = 'u', .lo = "up", .clear = 0, .set = GOLB_FIXUP },
    { .so = 'F', .lo = "fix", .clear = 0, .set = GOLB_FIX },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 's', .lo = "set", .i = GOLA_SET },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { [GOLA_SET] = "current" } ;
  unsigned int m = 0 ;
  char const *newargv[11] ;
  char fmtv[UINT_FMT] = " " ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-fix" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = wgolb & GOLB_FORCE_ESSENTIAL ? "--force-essential" : "--no-force-essential" ;
  newargv[m++] = wgolb & GOLB_FIXUP ? "--fix-up" : "--fix-down" ;
  if (!(wgolb & GOLB_FIX)) newargv[m++] = "--dry-run" ;
  newargv[m++] = "--" ;
  newargv[m++] = wgola[GOLA_SET] ;
  newargv[m++] = 0 ;

  main_exec(newargv) ;
}
