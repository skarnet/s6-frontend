/* ISC license. */

#include <skalibs/types.h>
#include <skalibs/gol.h>

#include <execline/config.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_IGNORE_ESSENTIALS = 0x01,
} ;

void set_list (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'E', .lo = "with-essentials", .clear = GOLB_IGNORE_ESSENTIALS, .set = 0 },
    { .so = 'e', .lo = "without-essentials", .clear = 0, .set = GOLB_IGNORE_ESSENTIALS },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  char const *newargv[10] ;
  char fmtv[UINT_FMT] ;
  argv += gol_argv(argv, rgolb, 2, 0, 0, &wgolb, 0) ;

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-status" ;
  fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
  newargv[m++] = "-v" ;
  newargv[m++] = fmtv ;
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = "-L" ;
  newargv[m++] = wgolb & GOLB_IGNORE_ESSENTIALS ? "--without-essentials" : "--with-essentials" ;
  newargv[m++] = "--" ;
  newargv[m++] = "current" ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
