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

enum s6_gb_e
{
  S6_GB_HELP,
  S6_GB_VERSION,
  S6_GB_N
} ;

enum s6_ga_e
{
  S6_GA_SCANDIR,
  S6_GA_LIVEDIR,
  S6_GA_REPODIR,
  S6_GA_VERBOSITY,
  S6_GA_N
} ;

static gol_bool const s6_gb[S6_GB_N] =
{
  { .so = 'h', .lo = "help", .set = 1, .mask = 1 << S6_GB_HELP },
  { .so = 'V', .lo = "version", .set = 1, .mask = 1 << S6_GB_VERSION }
} ;

static gol_arg const s6_ga[S6_GA_N] =
{
  { .so = 's', .lo = "scandir", .i = S6_GA_SCANDIR },
  { .so = 'l', .lo = "livedir", .i = S6_GA_LIVEDIR },
  { .so = 'r', .lo = "repodir", .i = S6_GA_REPODIR },
  { .so = 'v', .lo = "verbosity", .i = S6_GA_VERBOSITY }
} ;

struct global_s *g ;

static struct command_s const main_commands[] =
{
  { .s = "help", .f = &help },
  { .s = "version", .f = &version },
} ;

int main (int argc, char const *const *argv, char const *const *envp)
{
  struct global_s global = GLOBAL_ZERO ;
  char const *gola[S6_GA_N] = { 0 } ;
  uint64_t golb = 0 ;
  struct command_s *cmd ;
  unsigned int golc ;
  PROG = "s6" ;
  g = &global ;

  golc = gol_main(argc, argv, s6_gb, S6_GB_N, s6_ga, S6_GA_N, &golb, gola) ;
  argc -= golc ; argv += golc ;

  if (gola[S6_GA_VERBOSITY] && !uint0_scan(gola[S6_GA_VERBOSITY], &g->verbosity))
    strerr_dief1x(100, "verbosity must be an unsigned integer") ;

  if (golb & (1 << S6_GB_VERSION)) version(argv) ;
  if (golb & (1 << S6_GB_HELP)) help(argv) ;
  if (golb & ((1 << S6_GB_VERSION) | (1 << S6_GB_HELP))) return 0 ;

  if (gola[S6_GA_SCANDIR]) strerr_warni("scandir is ", gola[S6_GA_SCANDIR]) ;
  if (gola[S6_GA_LIVEDIR]) strerr_warni("livedir is ", gola[S6_GA_LIVEDIR]) ;
  if (gola[S6_GA_REPODIR]) strerr_warni("repodir is ", gola[S6_GA_REPODIR]) ;

  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, main_commands) ;
  if (!cmd) dieusage() ;
  return (*cmd->f)(argv+1) ;
}
