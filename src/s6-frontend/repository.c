/* ISC license. */

#include <unistd.h>

#include <skalibs/envexec.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 repository subcommand [ subcommand options ] [ services... ] Type \"s6 repository help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

void repository (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "check", .f = &repository_check },
    { .s = "help", .f = &repository_help },
    { .s = "init", .f = &repository_init },
    { .s = "list", .f = &repository_list },
    { .s = "sync", .f = &repository_sync },
  } ;
  struct command_s *cmd ;
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
