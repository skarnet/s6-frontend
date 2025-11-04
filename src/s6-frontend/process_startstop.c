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
  size_t argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  process_send_svc(options->flags & 1 ? h ? "-uwU" : "-dwD" : h ? "-u" : "-d", argv, argc, options->timeout) ;
}

void process_start (char const *const *argv, process_options const *options)
{
  return process_startstop(argv, options, 1) ;
}

void process_stop (char const *const *argv, process_options const *options)
{
  return process_startstop(argv, options, 0) ;
}
