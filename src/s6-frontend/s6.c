/* ISC license. */

#include <stdlib.h>

#include <skalibs/envexec.h>
#include <skalibs/uint64.h>

#include <execline/config.h>

#include <s6-frontend/config.h>

#include "s6f-main-options.h"

static int argv_isuser (int argc, char const *const *argv)
{
  uint64_t wgolb = 0 ;
  char const *wgola[S6F_GOLA_N] = { 0 } ;
  GOL_main(argc, argv, s6f_main_rgolb, s6f_main_rgola, &wgolb, wgola) ;
  return !!(wgolb & S6F_GOLB_USER) ;
}

int main (int argc, char const *const *argv)
{
  char const *conffile = getenv("S6_CONF") ;
  PROG = "s6" ;
  if (!conffile) conffile = argv_isuser(argc, argv) ? S6_FRONTEND_USER_CONF : S6_FRONTEND_CONF ;
  if (!argc--) strerr_dief1x(103, "invalid argc/argv") ;
  argv++ ;

  unsigned int m = 0 ;
  char const *newargv[6 + argc] ;
  newargv[m++] = EXECLINE_EXTBINPREFIX "envfile" ;
  newargv[m++] = "-I" ;
  newargv[m++] = "--" ;
  newargv[m++] = conffile ;
  newargv[m++] = S6_FRONTEND_LIBEXECPREFIX "s6-frontend" ;
  while (argc--) newargv[m++] = *argv++ ;
  newargv[m++] = 0 ;
  xexec(newargv) ;
}
