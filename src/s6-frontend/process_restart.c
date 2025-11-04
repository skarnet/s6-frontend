/* ISC license. */

#include <stddef.h>

#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 process restart [ -W|--nowait ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

void process_restart (char const *const *argv, process_options const *options)
{
  size_t argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  process_send_svc(options->flags & 1 ? "-rwR" : "-r", argv, argc, options->timeout) ;
}
