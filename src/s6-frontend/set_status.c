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
  GOLB_IGNORE_ESSENTIALS = 0x01,
} ;

void set_status (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'E', .lo = "with-essentials", .clear = GOLB_IGNORE_ESSENTIALS, .set = 0 },
    { .so = 'e', .lo = "without-essentials", .clear = 0, .set = GOLB_IGNORE_ESSENTIALS },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  unsigned int argc ;
  char fmtv[UINT_FMT] = " " ;

  argv += gol_argv(argv, rgolb, 2, 0, 0, &wgolb, 0) ;
  argc = env_len(argv) ;
  if (!set_check_service_names(argv, argc))
    strerr_dief1x(100, "invalid service name") ;

  char const *newargv[15 + argc] ;

#ifdef S6_FRONTEND_USE_UTIL_LINUX
  if (g->color)
  {
    newargv[m++] = EXECLINE_EXTBINPREFIX "pipeline" ;
    newargv[m++] = "-w" ;
    newargv[m++] = "--" ;
    newargv[m++] = " column" ;
    newargv[m++] = " -ts/" ;
    newargv[m++] = "" ;
  }
#endif
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-status" ;
  fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
  newargv[m++] = "-v" ;
  newargv[m++] = fmtv ;
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = wgolb & GOLB_IGNORE_ESSENTIALS ? "--without-essentials" : "--with-essentials" ;
  newargv[m++] = "--" ;
  newargv[m++] = "current" ;
  for (unsigned int i = 0 ; i < argc ; i++) newargv[m++] = argv[i] ;
  newargv[m++] = 0 ;

  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
