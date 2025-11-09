/* ISC license. */

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/stat.h>
#include <skalibs/types.h>
#include <skalibs/posixplz.h>
#include <skalibs/envexec.h>
#include <skalibs/djbunix.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 live subcommand [ subcommand options ] [ services... ] Type \"s6 live help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

void live (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "help", .f = &live_help },
    { .s = "restart", .f = &live_restart },
    { .s = "start", .f = &live_start },
    { .s = "status", .f = &live_status },
    { .s = "stop", .f = &live_stop },
  } ;
  struct command_s *cmd ;

  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
