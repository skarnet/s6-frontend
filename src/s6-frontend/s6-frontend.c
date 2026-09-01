/* ISC license. */

#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/types.h>
#include <skalibs/buffer.h>
#include <skalibs/envexec.h>
#include <skalibs/cspawn.h>
#include <skalibs/djbunix.h>

#include <execline/config.h>

#include <s6/config.h>
#include <s6-rc/config.h>

#include "s6f.h"
#include "s6f-main-options.h"
#include "s6-frontend-internal.h"

#define USAGE "s6 [ generic options ] command [ command options ] command_arguments... Type \"s6 help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

#define CLEANUP_MODIF "scandir\0livedir\0repodir\0bootdb\0stmpdir\0storelist\0verbosity\0fdhuser"

struct global_s *g ;

static char const *getenv_nonempty (char const *name)
{
  char const *s = getenv(name) ;
  return s && *s ? s : 0 ;
}

pid_t main_spawn (char const *const *argv)
{
  return xmspawn_m(argv, CLEANUP_MODIF, sizeof(CLEANUP_MODIF), 0, 0, 0) ;
}

void main_exec (char const *const *argv)
{
  xmexec_m(argv, CLEANUP_MODIF, sizeof(CLEANUP_MODIF)) ;
}

void main_try (void (*f)(char const *const *), char const *const *argv)
{
  pid_t pid = fork() ;
  if (pid == -1) strerr_diefusys(111, "fork") ;
  if (!pid)
  {
    PROG = "s6-frontend (child)" ;
    (*f)(argv) ;
    strerr_dief(101, "can't happen: in main_try, noreturn function returned") ;
  }
  else
  {
    int wstat ;
    pid_t r = wait_pid(pid, &wstat) ;
    if (r == -1) strerr_diefusys(111, "waitpid") ;
    if (WIFSIGNALED(wstat) || WEXITSTATUS(wstat)) _exit(wait_estatus(wstat)) ;
  }
}

void main_pretty_exec (char const *const *argv)
{
#ifdef S6_FRONTEND_USE_UTIL_LINUX
  if (g->color)
  {
    unsigned int m = 0 ;
    unsigned int argc = env_len(argv) ;
    size_t len = s6f_equote_space(argv, argc, 0) ;
    char const *newargv[argc + 6] ;
    char espace[len] ;
    newargv[m++] = EXECLINE_EXTBINPREFIX "pipeline" ;
    newargv[m++] = "--" ;
    m += s6f_equote(newargv + m, argv, argc, 0, espace) ;
    newargv[m++] = "column" ;
    newargv[m++] = "-ts/" ;
    newargv[m++] = 0 ;
    main_exec(newargv) ;
  }
  else
#endif
  main_exec(argv) ;
}

int main (int argc, char const *const *argv)
{
  static struct command_s const commands[] =
  {
    { .s = "apply", .f = &set_apply },
    { .s = "disable", .f = &set_disable },
    { .s = "enable", .f = &set_enable },
    { .s = "help", .f = &main_help },
    { .s = "kill", .f = &process_kill },
    { .s = "l", .f = &live },
    { .s = "live", .f = &live },
    { .s = "p", .f = &process },
    { .s = "proc", .f = &process },
    { .s = "process", .f = &process },
    { .s = "r", .f = &repository },
    { .s = "repo", .f = &repository },
    { .s = "repository", .f = &repository },
    { .s = "set", .f = &set },
    { .s = "start", .f = &live_start },
    { .s = "stop", .f = &live_stop },
    { .s = "system", .f = &s6system },
    { .s = "version", .f = &version },
  } ;

  struct global_s globals_in_the_stack = GLOBAL_ZERO ;
  uint64_t wgolb = 0 ;
  unsigned int golc ;
  struct command_s *cmd ;
  char const *confstorelist = getenv_nonempty("storelist") ;
  int storelistfromcli ;
  char const *wgola[S6F_GOLA_N] =
  {
    [S6F_GOLA_SCANDIR] = getenv_nonempty("scandir"),
    [S6F_GOLA_LIVEDIR] = getenv_nonempty("livedir"),
    [S6F_GOLA_REPODIR] = getenv_nonempty("repodir"),
    [S6F_GOLA_BOOTDB] = getenv_nonempty("bootdb"),
    [S6F_GOLA_STMPDIR] = getenv_nonempty("stmpdir"),
    [S6F_GOLA_STORELIST] = confstorelist,
    [S6F_GOLA_VERBOSITY] = getenv_nonempty("verbosity"),
    [S6F_GOLA_FDHUSER] = getenv_nonempty("fdhuser"),
    [S6F_GOLA_COLOR] = 0
  } ;
  PROG = "s6-frontend" ;
  g = &globals_in_the_stack ;

  golc = GOL_main(argc, argv, s6f_main_rgolb, s6f_main_rgola, &wgolb, wgola) ;
  argc -= golc ; argv += golc ;
  storelistfromcli = wgola[S6F_GOLA_STORELIST] != confstorelist ;

  if (wgola[S6F_GOLA_VERBOSITY] && !uint0_scan(wgola[S6F_GOLA_VERBOSITY], &g->verbosity))
    strerr_dief1x(100, "verbosity must be an unsigned integer") ;

  if (wgolb & S6F_GOLB_HELP)  { main_help(argv) ; _exit(0) ; }

  g->isuser = !!(wgolb & S6F_GOLB_USER) ;
  if (g->isuser)
  {
    s6f_user_get_confdirs(&g->dirs, &g->userstorage,
      confstorelist ? confstorelist : S6_FRONTEND_USER_STORELIST) ;
    g->fdhuser = "" ;
  }

  if (wgola[S6F_GOLA_SCANDIR]) g->dirs.scan = wgola[S6F_GOLA_SCANDIR] ;
  if (wgola[S6F_GOLA_LIVEDIR]) g->dirs.live = wgola[S6F_GOLA_LIVEDIR] ;
  if (wgola[S6F_GOLA_REPODIR]) g->dirs.repo = wgola[S6F_GOLA_REPODIR] ;
  if (wgola[S6F_GOLA_BOOTDB]) g->dirs.boot = wgola[S6F_GOLA_BOOTDB] ;
  if (wgola[S6F_GOLA_STMPDIR]) g->dirs.stmp = wgola[S6F_GOLA_STMPDIR] ;
  if (wgola[S6F_GOLA_STORELIST] && (!g->isuser || storelistfromcli))
    g->dirs.stol = wgola[S6F_GOLA_STORELIST] ;
  if (wgola[S6F_GOLA_FDHUSER]) g->fdhuser = wgola[S6F_GOLA_FDHUSER] ;

  {
    int force_color = 0 ;
    g->istty = isatty(1) ;
    if (wgola[S6F_GOLA_COLOR])
    {
      if (!strcmp(wgola[S6F_GOLA_COLOR], "yes"))
      {
        force_color = 1 ;
        g->color = 1 ;
      }
      else if (!strcmp(wgola[S6F_GOLA_COLOR], "no"))
      {
        force_color = 1 ;
        g->color = 0 ;
      }
      else if (strcmp(wgola[S6F_GOLA_COLOR], "auto"))
        strerr_dief1x(100, "--color value must be yes, no, or auto") ;
    }
    if (!force_color) g->color = g->istty ;
  }

  if (!*argv) dieusage() ;
  cmd = BSEARCH(struct command_s, *argv, commands) ;
  if (!cmd) dieusage() ;
  (*cmd->f)(++argv) ;
  _exit(101) ;  /* not reached */
}
