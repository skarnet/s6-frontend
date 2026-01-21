/* ISC license. */

#include <string.h>

#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

void set_delete (char const *const *argv)
{
  unsigned int argc ;
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) strerr_die(100, "usage: s6 set delete names...") ;

  argc = env_len(argv) ;
  for (unsigned int i = 0 ; i < argc ; i++)
    if (argv[i][0] == '.' || strchr(argv[i], '/') || strchr(argv[i], '\n')
     || !strcmp(argv[i], "current"))
      strerr_dief(100, "invalid set name: ", argv[i]) ;

  unsigned int m = 0 ;
  char const *newargv[7 + argc] ;
  char fmtv[UINT_FMT] ;

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-delete" ;
  fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
  newargv[m++] = "-v" ;
  newargv[m++] = fmtv ;
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  newargv[m++] = "--" ;
  while (argc--) newargv[m++] = *argv++ ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
