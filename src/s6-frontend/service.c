/* ISC license. */

#include <unistd.h>

#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 service [ service options ] subcommand [ subcommand options ] services... Type \"s6 service help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

void service (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "help", .f = &service_help },
//    { .s = "start", .f = &service_start },
//    { .s = "status", .f = &service_status },
//    { .s = "stop", .f = &service_stop },
  } ;
  struct command_s *cmd ;

  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
