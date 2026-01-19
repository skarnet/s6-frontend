/* ISC license. */

#include <stddef.h>

#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 process restart [ -W|--no-wait | -w|--wait ] [ -t timeout | --timeout=timeout ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum golb_e
{
  GOLB_WAIT = 0x01,
} ;

enum gola_e
{
  GOLA_TIMEOUT,
  GOLA_N
} ;

void process_restart (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'W', .lo = "no-wait", .clear = GOLB_WAIT, .set = 0 },
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
  if (!*argv) dieusage() ;
  if (wgola[GOLA_TIMEOUT])
  {
    if (!uint0_scan(wgola[GOLA_TIMEOUT], &timeout))
      strerr_dief1x(100, "--timeout= argument must be an unsigned integer (in milliseconds)") ;
  }
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  process_send_svc(wgolb & GOLB_WAIT ? "-rwR" : "-r", argv, argc, timeout) ;
}
