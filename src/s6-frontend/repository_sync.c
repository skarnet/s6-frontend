/* ISC license. */

#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum gola_e
{
  GOLA_FDHUSER,
  GOLA_N
} ;

void repository_sync (char const *const *argv)
{
  static gol_arg const rgola[] =
  {
    { .so = 'h', .lo = "fdholder-user", .i = GOLA_FDHUSER },
  } ;
  unsigned int m = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  char fmtv[UINT_FMT] ;

  argv += gol_argv(argv, 0, 0, rgola, GOLA_N, 0, wgola) ;

  char const *newargv[9] ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-repo-sync" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  if (wgola[GOLA_FDHUSER])
  {
    newargv[m++] = "-h" ;
    newargv[m++] = wgola[GOLA_FDHUSER] ;
  }
  newargv[m++] = "--" ;
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
