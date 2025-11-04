/* ISC license. */

#include <stdlib.h>

#include <skalibs/prog.h>
#include <skalibs/strerr.h>
#include <skalibs/exec.h>

#include <execline/config.h>

#include <s6-frontend/config.h>

int main (int argc, char const *const *argv)
{
  char const *conffile = getenv("S6_FRONTEND_CONF") ;
  PROG = "s6" ;
  if (!conffile) conffile = S6_FRONTEND_CONF ;
  if (!argc--) strerr_dief1x(103, "invalid argc/argv") ;
  argv++ ;

  unsigned int m = 0 ;
  char const *newargv[6 + argc] ;
  newargv[m++] = EXECLINE_EXTBINPREFIX "envfile" ;
  newargv[m++] = "-I" ;
  newargv[m++] = "--" ;
  newargv[m++] = conffile ;
  newargv[m++] = S6_FRONTEND_BINPREFIX "s6-frontend" ;
  while (argc--) newargv[m++] = *argv++ ;
  newargv[m++] = 0 ;
  xexec(newargv) ;
}
