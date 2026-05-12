/* ISC license. */

#include <unistd.h>

#include <skalibs/envexec.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 version subcommand [ subcommand options ] Type \"s6 version help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

void version (char const *const *argv)
{
  static struct command_s const subcommands[] =
  {
    { .s = "export", .f = &version_export },
    { .s = "help", .f = &version_help },
    { .s = "show", .f = &version_show },
  } ;
  struct command_s *cmd ;
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) version_show(argv) ;
  cmd = BSEARCH(struct command_s, *argv, subcommands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
