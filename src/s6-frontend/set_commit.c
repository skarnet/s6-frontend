/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_FORCE = 0x01,
  GOLB_KEEPOLD = 0x02,
} ;

enum gola_e
{
  GOLA_DEFBUNDLE,
  GOLA_FDHUSER,
  GOLA_N
} ;

void set_commit (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'f', .lo = "force", .clear = 0, .set = GOLB_FORCE },
    { .so = 'K', .lo = "keep-old", .clear = 0, .set = GOLB_KEEPOLD },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 'D', .lo = "default-bundle", .i = GOLA_DEFBUNDLE },
    { .so = 'h', .lo = "fdholder-user", .i = GOLA_FDHUSER }
  } ;

  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  char const *newargv[14] ;
  char fmtv[UINT_FMT] ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

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
  if (wgolb & GOLB_FORCE) newargv[m++] = "-f" ;
  if (wgolb & GOLB_KEEPOLD) newargv[m++] = "-K" ;
  if (wgola[GOLA_FDHUSER])
  {
    newargv[m++] = "-h" ;
    newargv[m++] = wgola[GOLA_FDHUSER] ;
  }
  newargv[m++] = "--" ;
  newargv[m++] = "current" ;
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
