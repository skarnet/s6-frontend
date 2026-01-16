/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

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

void repository_check (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'E', .lo = "no-force-essential", .clear = GOLB_FORCE_ESSENTIAL, .set = 0 },
    { .so = 'e', .lo = "force-essential", .clear = 0, .set = GOLB_FORCE_ESSENTIAL },
    { .so = 'd', .lo = "down", .clear = GOLB_FIXUP, .set = 0 },
    { .so = 'u', .lo = "up", .clear = 0, .set = GOLB_FIXUP },
    { .so = 'F', .lo = "fix", .clear = 0, .set = GOLB_FIX },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  char const *newargv[9] ;
  char fmtv[UINT_FMT] = " " ;

  argv += gol_argv(argv, rgolb, 4, 0, 0, &wgolb, 0) ;

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
  newargv[m++] = 0 ;

  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
