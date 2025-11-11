/* ISC license. */

#include <unistd.h>

#include <skalibs/envexec.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 set subcommand [ subcommand options ] [ services... ] Type \"s6 set help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

static void set_disable (char const *const *argv) gccattr_noreturn ;
static void set_disable (char const *const *argv)
{
  set_change(argv, "latent", "disable") ;
}

static void set_enable (char const *const *argv) gccattr_noreturn ;
static void set_enable (char const *const *argv)
{
  set_change(argv, "active", "enable") ;
}

static void set_mask (char const *const *argv) gccattr_noreturn ;
static void set_mask (char const *const *argv)
{ 
  set_change(argv, "masked", "mask") ;
}

static void set_unmask (char const *const *argv) gccattr_noreturn ;
static void set_unmask (char const *const *argv)
{
  set_change(argv, "latent", "unmask") ;
}

void set (char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "disable", .f = &set_disable },
    { .s = "enable", .f = &set_enable },
    { .s = "help", .f = &set_help },
//    { .s = "list", .f = &set_list },
//    { .s = "load", .f = &set_load },
    { .s = "mask", .f = &set_mask },
//    { .s = "save", .f = &set_save },
//    { .s = "status", .f = &set_status },
//    { .s = "sync", .f = &set_sync },
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
