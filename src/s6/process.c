/* ISC license. */

#include <skalibs/posixplz.h>
#include <skalibs/strerr.h>
#include <skalibs/cspawn.h>
#include <skalibs/djbunix.h>
#include <s6/config.h>

#include "s6-internal.h"

#define USAGE "s6 process [ process options ] subcommand [ subcommand options ] services... Type \"s6 process help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

static struct command_s const process_commands[] =
{
  { .s = "help", .f = &process_help },
  { .s = "kill", .f = &process_kill },
  { .s = "restart", .f = &process_restart },
  { .s = "start", .f = &process_start },
  { .s = "status", .f = &process_status },
  { .s = "stop", .f = &process_stop },
} ;

int process (char const *const *argv)
{
  struct command_s *cmd ;

  PROG = "s6 process" ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, process_commands) ;
  if (!cmd) dieusage() ;
  return (*cmd->f)(++argv) ;
}
