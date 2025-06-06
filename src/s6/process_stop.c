/* ISC license. */

#include <stddef.h>

#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-internal.h"

#define USAGE "s6 process stop [ -W|--nowait | -w|--wait ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

enum process_stop_golb_e
{
  PROCESS_STOP_GOLB_WAIT,
  PROCESS_STOP_GOLB_N
} ;

static gol_bool const process_stop_golb[2] =
{
  { .so = 'W', .lo = "nowait", .set = 0, .mask = 1 << PROCESS_STOP_GOLB_WAIT },
  { .so = 'w', .lo = "wait", .set = 1, .mask = 1 << PROCESS_STOP_GOLB_WAIT }
} ;

int process_stop (char const *const *argv)
{
  uint64_t golb = 1 << PROCESS_STOP_GOLB_WAIT ;
  size_t argc ;
  PROG = "s6 process stop" ;

  argv += gol_argv(argv, process_stop_golb, 2, 0, 0, &golb, 0) ;
  if (!argv) dieusage() ;
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  return process_send_svc(golb & 1 << PROCESS_STOP_GOLB_WAIT ? "-dwD" : "-d", argv, argc) ;
}
