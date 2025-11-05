/* ISC license. */

#include <stddef.h>

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-frontend-internal.h"

static void process_startstop (char const *const *argv, process_options const *options, int h) gccattr_noreturn ;
static void process_startstop (char const *const *argv, process_options const *options, int h)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'P', .lo = "no-permanent", .clear = 0x01, .set = 0 },
    { .so = 'p', .lo = "permanent", .clear = 0, .set = 0x01 }
  } ;
  static char const *svcopts[2][2][2] = { { { "-d", "-u" }, { "-D", "-U" } }, { { "-dwD", "-uwU" }, { "-DwD", "-UwU" } } } ;
  size_t argc = env_len(argv) ;
  uint64_t wgolb = 0 ;
  unsigned int golc = gol_argv(argv, rgolb, 2, 0, 0, &wgolb, 0) ;
  argc -= golc ; argv += golc ;
  if (!argc) strerr_dief3x(100, "usage: s6 process ", h ? "start" : "stop", " [ -P | -p ] services...") ;

  process_check_services(argv, argc) ;
  process_send_svc(svcopts[options->flags & 1][wgolb & 1][h], argv, argc, options->timeout) ;
}

void process_start (char const *const *argv, process_options const *options)
{
  process_startstop(argv, options, 1) ;
}

void process_stop (char const *const *argv, process_options const *options)
{
  process_startstop(argv, options, 0) ;
}
