/* ISC license. */

#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>
#include <skalibs/cspawn.h>
#include <skalibs/djbunix.h>

#include <s6-rc/config.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_FORCE = 0x01,
  GOLB_UPDATE = 0x02,
} ;

enum gola_e
{
  GOLA_FDHUSER,
  GOLA_STOREPATH,
  GOLA_N
} ;

void repository_init (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'f', .lo = "force", .clear = 0, .set = GOLB_FORCE },
    { .so = 'U', .lo = "update-stores", .clear = 0, .set = GOLB_UPDATE },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 'h', .lo = "fdholder-user", .i = GOLA_FDHUSER },
    { .so = 'p', .lo = "store-path", .i = GOLA_STOREPATH },
  } ;
  uint64_t wgolb = 0 ;
  unsigned int m = 0 ;
  unsigned int n = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  size_t len ;
  char fmtv[UINT_FMT] ;

  wgola[GOLA_STOREPATH] = S6_FRONTEND_STOREPATH ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;

  len = strlen(wgola[GOLA_STOREPATH]) ;
  char storage[len+1] ;
  for (size_t i = 0 ; i < len ; i++)
  {
    if (wgola[GOLA_STOREPATH][i] == ':') { n++ ; storage[i] = 0 ; }
    else storage[i] = wgola[GOLA_STOREPATH][i] ;
  }
  if (storage[len-1]) { n++ ; storage[len] = 0 ; }

  char const *newargv[11 + n] ;
  fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
  newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-repo-init" ;
  newargv[m++] = "-v" ;
  newargv[m++] = fmtv ;
  newargv[m++] = "-r" ;
  newargv[m++] = g->dirs.repo ;
  if (wgolb & GOLB_FORCE) newargv[m++] = "-f" ;
  if (wgolb & GOLB_UPDATE) newargv[m++] = "-U" ;
  if (wgola[GOLA_FDHUSER])
  {
    newargv[m++] = "-h" ;
    newargv[m++] = wgola[GOLA_FDHUSER] ;
  }
  newargv[m++] = "--" ;
  len = 0 ;
  for (unsigned int i = 0 ; i < n ; i++)
  {
    newargv[m++] = storage + len ;
    len += strlen(storage + len) + 1 ;
  }
  newargv[m++] = 0 ;
  if (wgolb & GOLB_UPDATE)
    xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
  else
  {
    int wstat ;
    pid_t pid = xmspawn_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n, 0, 0, 0) ;
    if (wait_pid(pid, &wstat) == -1) strerr_diefu1sys(111, "wait_pid") ;
    if (wait_estatus(wstat)) _exit(wait_estatus(wstat)) ;
    m = 0 ;
    newargv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-new" ;
    if (g->verbosity != 1)
    {
      newargv[m++] = "-v" ;
      newargv[m++] = fmtv ;
    }
    newargv[m++] = "-r" ;
    newargv[m++] = g->dirs.repo ;
    newargv[m++] = "--" ;
    newargv[m++] = "current" ;
    newargv[m++] = 0 ;
    xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
  }
}
