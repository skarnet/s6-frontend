/* ISC license. */

#include <unistd.h>

#include <skalibs/envexec.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 live subcommand [ subcommand options ] [ services... ] Type \"s6 live help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

void live (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "boot", .f = &live_boot },
    { .s = "halt", .f = &live_halt },
    { .s = "help", .f = &live_help },
    { .s = "poweroff", .f = &live_poweroff },
    { .s = "reboot", .f = &live_reboot },
    { .s = "restart", .f = &live_restart },
    { .s = "start", .f = &live_start },
    { .s = "start-everything", .f = &live_start_everything },
    { .s = "status", .f = &live_status },
    { .s = "stop", .f = &live_stop },
    { .s = "stop-everything", .f = &live_stop_everything },
  } ;
  struct command_s *cmd ;

  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
