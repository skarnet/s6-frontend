/* ISC license. */

#include <skalibs/types.h>
#include <skalibs/gol.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

void repository_sync (char const *const *argv)
{
  unsigned int m = 0 ;
  char fmtv[UINT_FMT] ;

  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;

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
  if (g->fdhuser && g->fdhuser[0])
  {
    newargv[m++] = "-h" ;
    newargv[m++] = g->fdhuser ;
  }
  newargv[m++] = "--" ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
