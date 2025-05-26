/* ISC license. */

#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include <s6/config.h>
#include <s6-rc/config.h>

#include <s6-frontend/config.h>
#include "s6-internal.h"

#define USAGE "s6 [ generic options ] subcommand [ command options ] command_arguments... Type \"s6 help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

enum main_golb_e
{
  MAIN_GOLB_HELP,
  MAIN_GOLB_VERSION,
  MAIN_GOLB_N
} ;

enum main_gola_e
{
  MAIN_GOLA_SCANDIR,
  MAIN_GOLA_LIVEDIR,
  MAIN_GOLA_REPODIR,
  MAIN_GOLA_VERBOSITY,
  MAIN_GOLA_N
} ;

static gol_bool const main_golb[MAIN_GOLB_N] =
{
  { .so = 'h', .lo = "help", .set = 1, .mask = 1 << MAIN_GOLB_HELP },
  { .so = 'V', .lo = "version", .set = 1, .mask = 1 << MAIN_GOLB_VERSION }
} ;

static gol_arg const main_gola[MAIN_GOLA_N] =
{
  { .so = 's', .lo = "scandir", .i = MAIN_GOLA_SCANDIR },
  { .so = 'l', .lo = "livedir", .i = MAIN_GOLA_LIVEDIR },
  { .so = 'r', .lo = "repodir", .i = MAIN_GOLA_REPODIR },
  { .so = 'v', .lo = "verbosity", .i = MAIN_GOLA_VERBOSITY }
} ;

struct global_s *g ;

static struct command_s const main_commands[] =
{
  { .s = "help", .f = &help },
  { .s = "version", .f = &version },
} ;

int main (int argc, char const *const *argv, char const *const *envp)
{
  struct global_s globals_in_the_stack = GLOBAL_ZERO ;
  char const *gola[MAIN_GOLA_N] = { 0 } ;
  uint64_t golb = 0 ;
  unsigned int golc ;
  struct command_s *cmd ;
  PROG = "s6" ;
  g = &globals_in_the_stack ;

  golc = gol_main(argc, argv, main_golb, MAIN_GOLB_N, main_gola, MAIN_GOLA_N, &golb, gola) ;
  argc -= golc ; argv += golc ;

  if (gola[MAIN_GOLA_VERBOSITY] && !uint0_scan(gola[MAIN_GOLA_VERBOSITY], &g->verbosity))
    strerr_dief1x(100, "verbosity must be an unsigned integer") ;

  if (golb & (1 << MAIN_GOLB_VERSION)) version(argv) ;
  if (golb & (1 << MAIN_GOLB_HELP)) help(argv) ;
  if (golb & ((1 << MAIN_GOLB_VERSION) | (1 << MAIN_GOLB_HELP))) return 0 ;

  if (gola[MAIN_GOLA_SCANDIR]) strerr_warni("scandir is ", gola[MAIN_GOLA_SCANDIR]) ;
  if (gola[MAIN_GOLA_LIVEDIR]) strerr_warni("livedir is ", gola[MAIN_GOLA_LIVEDIR]) ;
  if (gola[MAIN_GOLA_REPODIR]) strerr_warni("repodir is ", gola[MAIN_GOLA_REPODIR]) ;

  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, main_commands) ;
  if (!cmd) dieusage() ;
  return (*cmd->f)(++argv) ;
}
