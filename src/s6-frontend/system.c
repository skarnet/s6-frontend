/* ISC license. */

#include <unistd.h>

#include <skalibs/envexec.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 system subcommand [ subcommand options ] [ services... ] Type \"s6 system help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

void s6system (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "boot", .f = &system_boot },
    { .s = "halt", .f = &system_halt },
    { .s = "help", .f = &system_help },
    { .s = "poweroff", .f = &system_poweroff },
    { .s = "reboot", .f = &system_reboot },
    { .s = "shutdown", .f = &system_poweroff },
  } ;
  struct command_s *cmd ;

  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
