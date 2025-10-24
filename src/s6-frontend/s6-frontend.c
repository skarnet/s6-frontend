/* ISC license. */

#include <string.h>
#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/buffer.h>
#include <skalibs/prog.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>

#include <s6/config.h>
#include <s6-rc/config.h>

#include <s6-frontend/config.h>
#include "s6-internal.h"

#define USAGE "s6-frontend [ generic options ] command [ command options ] command_arguments... Type \"s6 help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)


int version (char const *const *argv)
{
  (void)argv ;
  if (!buffer_putsflush(buffer_1, "s6-frontend v" S6_FRONTEND_VERSION "\n"))
    strerr_diefu1sys(111, "write to stdout") ;
  return 0 ;
}

enum golb_e
{
  GOLB_HELP,
  GOLB_VERSION,
  GOLB_USER,
  GOLB_N
} ;

enum gola_e
{
  GOLA_SCANDIR,
  GOLA_LIVEDIR,
  GOLA_REPODIR,
  GOLA_BOOTDIR,
  GOLA_STMPDIR,
  GOLA_VERBOSITY,
  GOLA_COLOR,
  GOLA_N
} ;

static gol_bool const rgolb[GOLB_N] =
{
  { .so = 'h', .lo = "help", .clear = 0, .set = 1 << GOLB_HELP },
  { .so = 'V', .lo = "version", .clear = 0, .set = 1 << GOLB_VERSION },
  { .so = 0, .lo = "user", .clear = 0, .set = 1 << GOLB_USER }
} ;

static gol_arg const rgola[GOLA_N] =
{
  { .so = 's', .lo = "scandir", .i = GOLA_SCANDIR },
  { .so = 'l', .lo = "livedir", .i = GOLA_LIVEDIR },
  { .so = 'r', .lo = "repodir", .i = GOLA_REPODIR },
  { .so = 'b', .lo = "bootdir", .i = GOLA_BOOTDIR },
  { .so = 0,   .lo = "stmpdir", .i = GOLA_STMPDIR },
  { .so = 'v', .lo = "verbosity", .i = GOLA_VERBOSITY },
  { .so = 0,   .lo = "color", .i = GOLA_COLOR }
} ;

struct global_s *g ;

static struct command_s const commands[] =
{
  { .s = "help", .f = &help },
  { .s = "process", .f = &process },
  { .s = "service", .f = &service },
  { .s = "version", .f = &version },
} ;

int main (int argc, char const *const *argv, char const *const *envp)
{
  struct global_s globals_in_the_stack = GLOBAL_ZERO ;
  uint64_t wgolb = 0 ;
  unsigned int golc ;
  struct command_s *cmd ;
  char const *wgola[GOLA_N] =
  {
    [GOLA_SCANDIR] = getenv("scandir"),
    [GOLA_LIVEDIR] = getenv("livedir"),
    [GOLA_REPODIR] = getenv("repodir"),
    [GOLA_BOOTDIR] = getenv("bootdir"),
    [GOLA_STMPDIR] = getenv("stmpdir"),
    [GOLA_VERBOSITY] = 0,
    [GOLA_COLOR] = 0
  } ;
  PROG = "s6-frontend" ;
  g = &globals_in_the_stack ;

  golc = gol_main(argc, argv, rgolb, GOLB_N, rgola, GOLA_N, &wgolb, wgola) ;
  argc -= golc ; argv += golc ;

  if (wgola[GOLA_VERBOSITY] && !uint0_scan(wgola[GOLA_VERBOSITY], &g->verbosity))
    strerr_dief1x(100, "verbosity must be an unsigned integer") ;

  if (wgolb & 1 << GOLB_VERSION) version(argv) ;
  if (wgolb & 1 << GOLB_HELP) help(argv) ;
  if (wgolb & (1 << GOLB_VERSION | 1 << GOLB_HELP)) return 0 ;

  if (wgola[GOLA_SCANDIR]) g->dirs.scan = wgola[GOLA_SCANDIR] ;
  if (wgola[GOLA_LIVEDIR]) g->dirs.live = wgola[GOLA_LIVEDIR] ;
  if (wgola[GOLA_REPODIR]) g->dirs.repo = wgola[GOLA_REPODIR] ;
  if (wgola[GOLA_BOOTDIR]) g->dirs.boot = wgola[GOLA_BOOTDIR] ;
  if (wgola[GOLA_STMPDIR]) g->dirs.stmp = wgola[GOLA_STMPDIR] ;

  {
    int force_color = 0 ;
    g->istty = isatty(1) ;
    if (wgola[GOLA_COLOR])
    {
      if (!strcmp(wgola[GOLA_COLOR], "yes"))
      {
        force_color = 1 ;
        g->color = 1 ;
      }
      else if (!strcmp(wgola[GOLA_COLOR], "no"))
      {
        force_color = 1 ;
        g->color = 0 ;
      }
      else if (strcmp(wgola[GOLA_COLOR], "auto"))
        strerr_dief1x(100, "--color value must be yes, no, or auto") ;
    }
    if (!force_color) g->color = g->istty ;
  }

  g->isuser = !!(wgolb & 1 << GOLB_USER) ;
  if (g->isuser) s6f_user_get_confdirs(&g->dirs, &g->userstorage) ;

  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  return (*cmd->f)(++argv) ;
}
