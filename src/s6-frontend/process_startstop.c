/* ISC license. */

#include <stddef.h>

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/envexec.h>

#include "s6-frontend-internal.h"

enum golb_e
{
  GOLB_PERMANENT = 0x02,
  GOLB_WAIT = 0x04,
} ;

enum gola_e
{
  GOLA_TIMEOUT,
  GOLA_N
} ;

static void process_startstop (char const *const *argv, int h, char const *usage) gccattr_noreturn ;
static void process_startstop (char const *const *argv, int h, char const *usage)
{
  static char const *svcopts[8] = { "-d", "-u", "-D", "-U", "-dwD", "-uwU", "-DwD", "-UwU" } ;
  static gol_bool const rgolb[] =
  {
    { .so = 'P', .lo = "no-permanent", .clear = GOLB_PERMANENT, .set = 0 },
    { .so = 'p', .lo = "permanent", .clear = 0, .set = GOLB_PERMANENT },
    { .so = 'W', .lo = "nowait", .clear = GOLB_WAIT, .set = 0 },
    { .so = 'w', .lo = "wait", .clear = 0, .set = GOLB_WAIT },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  } ;
  char const *wgola[GOLA_N] = { 0 } ;
  uint64_t wgolb = 0 ;
  unsigned int timeout = 0 ;
  unsigned int argc ;
  argv += GOL_argv(argv, rgolb, rgola, &wgolb, wgola) ;
  if (!*argv) strerr_dieusage(100, usage) ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "--timeout= argument must be an unsigned integer (in milliseconds)") ;
  }
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  process_send_svc(svcopts[h|GOLB_PERMANENT|GOLB_WAIT], argv, argc, timeout) ;
}

void process_start (char const *const *argv)
{
  static char const *const usage = "s6 process start [ --permanent ] [ --wait ] [ --timeout=millisecs ] services..." ;
  process_startstop(argv, 1, usage) ;
}

void process_stop (char const *const *argv)
{
  static char const *const usage = "s6 process stop [ --permanent ] [ --wait ] [ --timeout=millisecs ] services..." ;
  process_startstop(argv, 0, usage) ;
}
