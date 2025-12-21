/* ISC license. */

#include <string.h>

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>

#include <s6-rc/config.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_FORCE = 0x01,
} ;

static void set_copy (char const *from, char const *to, int force) gccattr_noreturn ;
static void set_copy (char const *from, char const *to, int force)
{
  unsigned int m = 0 ;
  char const *argv[10] ;
  char fmtv[UINT_FMT] ;
  argv[m++] = S6RC_EXTBINPREFIX "s6-rc-set-copy" ;
  if (g->verbosity != 1)
  {
    fmtv[uint_fmt(fmtv, g->verbosity)] = 0 ;
    argv[m++] = "-v" ;
    argv[m++] = fmtv ;
  }
  argv[m++] = "-r" ;
  argv[m++] = g->dirs.repo ;
  if (force) argv[m++] = "-f" ;
  argv[m++] = "--" ;
  argv[m++] = from ;
  argv[m++] = to ;
  argv[m++] = 0 ;
  xmexec_n(argv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

void set_save (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'f', .lo = "force", .clear = 0, .set = GOLB_FORCE },
  } ;
  uint64_t wgolb = 0 ;
  argv += gol_argv(argv, rgolb, 1, 0, 0, &wgolb, 0) ;
  if (!*argv) strerr_die(100, "usage: ", "s6 set ", "save [ --force ] name") ;
  if (argv[0][0] == '.' || strchr(argv[0], '/') || strchr(argv[0], '\n')
   || !strcmp(argv[0], "current"))
    strerr_dief1x(100, "invalid set name") ;
  set_copy("current", argv[0], !!(wgolb & GOLB_FORCE)) ;
}

void set_load (char const *const *argv)
{
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) strerr_die(100, "usage: ", "s6 set ", "load name") ;
  if (argv[0][0] == '.' || strchr(argv[0], '/') || strchr(argv[0], '\n')
   || !strcmp(argv[0], "current"))
    strerr_dief1x(100, "invalid set name") ;
  set_copy(argv[0], "current", 1) ;
}
