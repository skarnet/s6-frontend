/* ISC license. */

#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <execline/config.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

void set_list (char const *const *argv)
{
  unsigned int m = 0 ;
  char const *newargv[9] ;
  char fmtv[UINT_FMT] ;
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-status" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = "-L" ;
  newargv[m++] = "--" ;
  newargv[m++] = "current" ;
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}
