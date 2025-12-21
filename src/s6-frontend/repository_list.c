/* ISC license. */

#include <skalibs/envexec.h>
#include <skalibs/types.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

void repository_list (char const *const *argv)
{
  char const *newargv[9] ;
  unsigned int m = 0 ;
  char fmtv[UINT_FMT] ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-repo-list" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = "-x" ;
  newargv[m++] = "current" ;
  newargv[m++] = "--" ;
  newargv[m++] = 0 ;

  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
