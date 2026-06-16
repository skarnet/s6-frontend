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

enum gola_e
{
  GOLA_SET,
  GOLA_N
} ;

void set_status (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'E', .lo = "with-essentials", .clear = GOLB_IGNORE_ESSENTIALS, .set = 0 },
    { .so = 'e', .lo = "without-essentials", .clear = 0, .set = GOLB_IGNORE_ESSENTIALS },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 's', .lo = "set", .i = GOLA_SET },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { [GOLA_SET] = "current" } ;
  unsigned int m = 0 ;
  unsigned int argc ;
  char fmtv[UINT_FMT] = " " ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  argc = env_len(argv) ;
  if (!set_check_service_names(argv, argc))
    strerr_dief1x(100, "invalid service name") ;

  char const *newargv[9 + argc] ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-status" ;
  fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
  newargv[m++] = "-v" ;
  newargv[m++] = fmtv ;
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = wgolb & GOLB_IGNORE_ESSENTIALS ? "--without-essentials" : "--with-essentials" ;
  newargv[m++] = "--" ;
  newargv[m++] = wgola[GOLA_SET] ;
  for (unsigned int i = 0 ; i < argc ; i++) newargv[m++] = argv[i] ;
  newargv[m++] = 0 ;

  main_pretty_exec(newargv) ;
}
