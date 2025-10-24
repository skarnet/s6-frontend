/* ISC license. */

#include <stddef.h>

#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-internal.h"

enum golb_e
{
  GOLB_WAIT,
  GOLB_N
} ;

static gol_bool const rgolb[2] =
{
  { .so = 'W', .lo = "nowait", .clear = 1 << GOLB_WAIT, .set = 0 },
  { .so = 'w', .lo = "wait",   .clear = 0, .set = 1 << GOLB_WAIT }
} ;

static int process_startstop (char const *const *argv, int h)
{
  uint64_t wgolb = 1 << GOLB_WAIT ;
  size_t argc ;
  PROG = h ? "s6 process start" : "s6 process stop" ;

  argv += gol_argv(argv, rgolb, 2, 0, 0, &wgolb, 0) ;
  if (!argv) strerr_dien(100, 4, PROG, ": usage: ", PROG, " [ -W|--nowait | -w|--wait ] services...") ;
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  return process_send_svc(wgolb & 1 << GOLB_WAIT ? h ? "-uwU" : "-dwD" : h ? "-u" : "-d", argv, argc) ;
}

int process_start (char const *const *argv)
{
  return process_startstop(argv, 1) ;
}

int process_stop (char const *const *argv)
{
  return process_startstop(argv, 0) ;
}
