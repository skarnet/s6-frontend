/* ISC license. */

#include <stddef.h>

#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-internal.h"

#define USAGE "s6 process restart [ -W|--nowait ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

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

int process_restart (char const *const *argv)
{
  uint64_t wgolb = 1 << GOLB_WAIT ;
  size_t argc ;
  PROG = "s6 process restart" ;

  argv += gol_argv(argv, rgolb, 2, 0, 0, &wgolb, 0) ;
  if (!argv) dieusage() ;
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  return process_send_svc(wgolb & 1 << GOLB_WAIT ? "-rwR" : "-r", argv, argc) ;
}
