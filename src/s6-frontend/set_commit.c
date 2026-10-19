/* ISC license. */

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/gol.h>
#include <skalibs/strerr.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_FORCE = 0x01,
  GOLB_KEEPOLD = 0x02,
  GOLB_NOSNAP = 0x04,
} ;

enum gola_e
{
  GOLA_DEFBUNDLE,
  GOLA_N
} ;

void set_commit (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'f', .lo = "force", .clear = 0, .set = GOLB_FORCE },
    { .so = 'K', .lo = "keep-old", .clear = 0, .set = GOLB_KEEPOLD },
    { .so = 0, .lo = "snapshot", .clear = GOLB_NOSNAP, .set = 0 },
    { .so = 0, .lo = "no-snapshot", .clear = 0, .set = GOLB_NOSNAP },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 'D', .lo = "default-bundle", .i = GOLA_DEFBUNDLE },
  } ;

  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  size_t slen = strlen(g->dirs.scan) ;
  char const *wgola[GOLA_N] = { 0 } ;
  char const *set ;
  char const *newargv[17] ;
  char fmtv[UINT_FMT] ;
  char catchall[slen + 15] ;

  wgola[GOLA_DEFBUNDLE] = S6_FRONTEND_DEFBUNDLE ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

  set = *argv ? *argv : "current" ;
  if (set[0] == '.' || strchr(set, '/') || strchr(set, '\n'))
      strerr_dief(100, "invalid set name: ", set) ;

  memcpy(catchall, g->dirs.scan, slen) ;
  memcpy(catchall + slen, "/s6-svscan-log", 15) ;

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
  if (wgolb & GOLB_NOSNAP) newargv[m++] = "--no-snapshot" ;
  if (g->fdhuser && g->fdhuser[0])
  {
    newargv[m++] = "-h" ;
    newargv[m++] = g->fdhuser ;
  }
  newargv[m++] = "-L" ;
  newargv[m++] = catchall ;
  newargv[m++] = "--" ;
  newargv[m++] = set ;
  newargv[m++] = 0 ;
  main_exec(newargv) ;
}
