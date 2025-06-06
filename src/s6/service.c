/* ISC license. */

#include <skalibs/prog.h>
#include <skalibs/strerr.h>

#include "s6-internal.h"

#define USAGE "s6 service [ service options ] subcommand [ subcommand options ] services... Type \"s6 service help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

static struct command_s const service_commands[] =
{
  { .s = "help", .f = &service_help },
  { .s = "start", .f = &service_start },
  { .s = "status", .f = &service_status },
  { .s = "stop", .f = &service_stop },
} ;

int service (char const *const *argv)
{
  struct command_s *cmd ;

  PROG = "s6 service" ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, service_commands) ;
  if (!cmd) dieusage() ;
  return (*cmd->f)(++argv) ;
}
