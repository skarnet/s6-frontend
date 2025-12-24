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

#include "s6-frontend-internal.h"

#define USAGE "s6 [ generic options ] command [ command options ] command_arguments... Type \"s6 help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

#define CLEANUP_MODIF "scandir\0livedir\0repodir\0bootdb\0stmpdir\0verbosity\0defaultbundle"
struct modif_s const cleanup_modif =
{
  .s = CLEANUP_MODIF,
  .len = sizeof(CLEANUP_MODIF),
  .n = 6
} ;

enum golb_e
{
  GOLB_HELP = 0x01,
  GOLB_VERSION = 0x02,
  GOLB_USER = 0x04,
} ;

enum gola_e
{
  GOLA_SCANDIR,
  GOLA_LIVEDIR,
  GOLA_REPODIR,
  GOLA_BOOTDB,
  GOLA_STMPDIR,
  GOLA_VERBOSITY,
  GOLA_COLOR,
  GOLA_N
} ;

struct global_s *g ;

int main (int argc, char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'h', .lo = "help", .clear = 0, .set = GOLB_HELP },
    { .so = 'V', .lo = "version", .clear = 0, .set = GOLB_VERSION },
//    { .so = 0, .lo = "user", .clear = 0, .set = GOLB_USER },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 's', .lo = "scandir", .i = GOLA_SCANDIR },
    { .so = 'l', .lo = "livedir", .i = GOLA_LIVEDIR },
    { .so = 'r', .lo = "repodir", .i = GOLA_REPODIR },
    { .so = 'c', .lo = "bootdb", .i = GOLA_BOOTDB },
    { .so = 0,   .lo = "stmpdir", .i = GOLA_STMPDIR },
    { .so = 'v', .lo = "verbosity", .i = GOLA_VERBOSITY },
    { .so = 0,   .lo = "color", .i = GOLA_COLOR },
  } ;
  static struct command_s const commands[] =
  {
    { .s = "help", .f = &main_help },
    { .s = "live", .f = &live },
    { .s = "process", .f = &process },
    { .s = "repository", .f = &repository },
    { .s = "set", .f = &set },
    { .s = "system", .f = &s6system },
    { .s = "version", .f = &main_version },
  } ;

  struct global_s globals_in_the_stack = GLOBAL_ZERO ;
  uint64_t wgolb = 0 ;
  unsigned int golc ;
  struct command_s *cmd ;
  char const *wgola[GOLA_N] =
  {
    [GOLA_SCANDIR] = getenv("scandir"),
    [GOLA_LIVEDIR] = getenv("livedir"),
    [GOLA_REPODIR] = getenv("repodir"),
    [GOLA_BOOTDB] = getenv("bootdb"),
    [GOLA_STMPDIR] = getenv("stmpdir"),
    [GOLA_VERBOSITY] = getenv("verbosity"),
    [GOLA_COLOR] = 0
  } ;
  PROG = "s6-frontend" ;
  g = &globals_in_the_stack ;

  golc = GOL_main(argc, argv, rgolb, rgola, &wgolb, wgola) ;
  argc -= golc ; argv += golc ;

  if (wgola[GOLA_VERBOSITY] && !uint0_scan(wgola[GOLA_VERBOSITY], &g->verbosity))
    strerr_dief1x(100, "verbosity must be an unsigned integer") ;

  if (wgolb & GOLB_VERSION) main_version(argv) ;
  if (wgolb & GOLB_HELP) main_help(argv) ;
  if (wgolb & (GOLB_VERSION | GOLB_HELP)) return 0 ;

  if (wgola[GOLA_SCANDIR]) g->dirs.scan = wgola[GOLA_SCANDIR] ;
  if (wgola[GOLA_LIVEDIR]) g->dirs.live = wgola[GOLA_LIVEDIR] ;
  if (wgola[GOLA_REPODIR]) g->dirs.repo = wgola[GOLA_REPODIR] ;
  if (wgola[GOLA_BOOTDB]) g->dirs.boot = wgola[GOLA_BOOTDB] ;
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

//  g->isuser = !!(wgolb & GOLB_USER) ;
//  if (g->isuser) s6f_user_get_confdirs(&g->dirs, &g->userstorage) ;

  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
}
