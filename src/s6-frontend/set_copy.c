/* ISC license. */

#include <string.h>

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_FORCE = 0x01,
} ;

void set_copy (char const *const *argv)
{
 static gol_bool const rgolb[] =
  {
    { .so = 'f', .lo = "force", .clear = 0, .set = GOLB_FORCE },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  char const *newargv[10] ;
  char fmtv[UINT_FMT] ;

  argv += gol_argv(argv, rgolb, 1, 0, 0, &wgolb, 0) ;
  if (!argv[0] || !argv[1]) strerr_die(100, "usage: ", "s6 set ", "copy [ --force ] source destination") ;
  if (argv[0][0] == '.' || strchr(argv[0], '/') || strchr(argv[0], '\n'))
    strerr_dief(100, "invalid ", "source", " set name") ;
  if (argv[1][0] == '.' || strchr(argv[1], '/') || strchr(argv[1], '\n'))
    strerr_dief(100, "invalid ", "destination", " set name") ;

  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-copy" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    newargv[m++] = "-v" ;
    newargv[m++] = fmtv ;
  }
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  if (wgolb & GOLB_FORCE) newargv[m++] = "-f" ;
  newargv[m++] = "--" ;
  newargv[m++] = argv[0] ;
  newargv[m++] = argv[1] ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
