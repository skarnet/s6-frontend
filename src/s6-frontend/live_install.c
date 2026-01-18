/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_BLOCK = 0x01,
  GOLB_KEEPOLD = 0x02,
  GOLB_INIT = 0x04,
} ;

enum gola_e
{
  GOLA_CONVFILE,
  GOLA_N
} ;

void live_install (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'b', .lo = "block", .clear = 0, .set = GOLB_BLOCK },
    { .so = 'K', .lo = "keep-old", .clear = 0, .set = GOLB_KEEPOLD },
    { .so = 0, .lo = "init", .clear = 0, .set = GOLA_INIT },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 'f', .lo = "conversion-file", .i = GOLA_CONVFILE },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  char const *newargv[17] ;
  char fmtv[UINT_FMT] ;

  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-install" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
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
  if (wgolb & GOLB_KEEPOLD) newargv[m++] = "-K" ;
  if (wgola[GOLA_CONVFILE])
  {
    newargv[m++] = "-f" ;
    newargv[m++] = wgola[GOLA_CONVFILE] ;
  }
  if (wgolb & GOLB_INIT) newargv[m++] = "--no-update" ;
  newargv[m++] = "--" ;
  newargv[m++] = "current" ;
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
