/* ISC license. */

#include <unistd.h>

#include <skalibs/envexec.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 set subcommand [ subcommand options ] [ services... ] Type \"s6 set help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

int set_check_service_names (char const *const *names, unsigned int n)
{
  for (unsigned int i = 0 ; i < n ; i++)
    if (names[i][0] == '.' || strchr(names[i], '/') || strchr(names[i], '\n'))
      return 0 ;
  return 1 ;
}

void set (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "disable", .f = &set_disable },
    { .s = "enable", .f = &set_enable },
    { .s = "help", .f = &set_help },
    { .s = "list", .f = &set_list },
    { .s = "load", .f = &set_load },
    { .s = "mask", .f = &set_mask },
    { .s = "save", .f = &set_save },
    { .s = "status", .f = &set_status },
    { .s = "unmask", .f = &set_unmask },
  } ;
  struct command_s *cmd ;
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;
}
