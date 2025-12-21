/* ISC license. */

#include <unistd.h>
#include <sys/wait.h>

#include <skalibs/gccattributes.h>
#include <skalibs/envexec.h>
#include <skalibs/cspawn.h>
#include <skalibs/stralloc.h>
#include <skalibs/djbunix.h>
#include <skalibs/skamisc.h>

#include <execline/config.h>

#include <s6-rc/config.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

#define USAGE "s6 live status services..."
#define dieusage() strerr_dieusage(100, USAGE)
#define dienomem() strerr_diefu1sys(111, "stralloc_catb")

static void live_status_all (void) gccattr_noreturn ;
static void live_status_all (void)
{
  char const *argv[23] ;
  unsigned int m = 0 ;
#ifdef S6_FRONTEND_USE_UTIL_LINUX
  if (g->color)
  {
    argv[m++] = EXECLINE_EXTBINPREFIX "pipeline" ;
    argv[m++] = "-w" ;
    argv[m++] = "--" ;
    argv[m++] = " column" ;
    argv[m++] = " -ts/" ;
    argv[m++] = "" ;
  }
#endif
  argv[m++] = EXECLINE_EXTBINPREFIX "if" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "pipeline" ;
  argv[m++] = "  " S6RC_EXTBINPREFIX "s6-rc" ;
  argv[m++] = "  -ua" ;
  argv[m++] = "  list" ;
  argv[m++] = " " ;
  argv[m++] = " sed" ;
  argv[m++] = " s|$|/up" ;
  argv[m++] = "" ;
  argv[m++] = EXECLINE_EXTBINPREFIX "pipeline" ;
  argv[m++] = " " S6RC_EXTBINPREFIX "s6-rc" ;
  argv[m++] = " -da" ;
  argv[m++] = " list" ;
  argv[m++] = "" ;
  argv[m++] = "sed" ;
  argv[m++] = "s|$|/down" ;
  argv[m++] = 0 ;
  xmexec_n(argv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

static int get_list_of_up (stralloc *storage)
{
  int swasnull = !storage->s ;
  size_t sabase = storage->len ;
  int fd ;
  int wstat ;
  char const *argv[7] = { S6RC_EXTBINPREFIX "s6-rc", "-l", g->dirs.live, "-ba", "--", "list", 0 } ;
  pid_t pid = child_spawn1_pipe(argv[0], argv, (char const *const *)environ, &fd, 1) ;
  if (!pid) { strerr_warnfu2sys("spawn ", argv[0]) ; return 111 ; }
  if (!slurpn(fd, storage, 0)) { strerr_warnfu2sys("read output from ", argv[0]) ; return 111 ; }
  fd_close(fd) ;
  if (wait_pid(pid, &wstat) == -1)
  {
    strerr_warnfu2sys("wait for ", argv[0]) ;
    goto err ;
  }
  if (WIFSIGNALED(wstat))
  {
    char fmt[UINT_FMT] ;
    fmt[uint_fmt(fmt, WTERMSIG(wstat))] = 0 ;
    strerr_warnf3x(argv[0], " crashed with signal ", fmt) ;
    goto err ;
  }
  return wait_estatus(wstat) ;

 err:
  if (swasnull) stralloc_free(storage) ;
  else storage->len = sabase ;
  return 111 ;
}

static int get_atomics (char const *const *services, unsigned int n, stralloc *storage)
{
  int swasnull = !storage->s ;
  size_t sabase = storage->len ;
  pid_t pid ;
  int fd ;
  int wstat ;
  unsigned int m = 0 ;
  char const *argv[7 + n] ;
  argv[m++] = S6RC_EXTBINPREFIX "s6-rc-db" ;
  argv[m++] = "-l";
  argv[m++] = g->dirs.live ;
  argv[m++] = "-b" ;
  argv[m++] = "--" ;
  argv[m++] = "atomics" ;
  for (unsigned int i = 0 ; i < n ; i++) argv[m++] = services[i] ;
  argv[m++] = 0 ;
  pid = child_spawn1_pipe(argv[0], argv, (char const *const *)environ, &fd, 1) ;
  if (!pid) { strerr_warnfu2sys("spawn ", argv[0]) ; return 111 ; }
  if (!slurpn(fd, storage, 0)) { strerr_warnfu2sys("read output from ", argv[0]) ; return 111 ; }
  fd_close(fd) ;
  if (wait_pid(pid, &wstat) == -1)
  {
    strerr_warnfu2sys("wait for ", argv[0]) ;
    goto err ;
  }
  if (WIFSIGNALED(wstat))
  {
    char fmt[UINT_FMT] ;
    fmt[uint_fmt(fmt, WTERMSIG(wstat))] = 0 ;
    strerr_warnf3x(argv[0], " crashed with signal ", fmt) ;
    goto err ;
  }
  return wait_estatus(wstat) ;

 err:
  if (swasnull) stralloc_free(storage) ;
  else storage->len = sabase ;
  return 111 ;
}

static void live_status_some (char const *const *services) gccattr_noreturn ;
static void live_status_some (char const *const *services)
{
  stralloc sa = STRALLOC_ZERO ;
  unsigned int m = 0 ;
  size_t uplistpos ;
  int e ;
  char const *argv[59] ;
  if (!stralloc_catb(&sa, " ", 1)) dienomem() ;
  e = get_atomics(services, env_len(services), &sa) ;
  if (e) _exit(e) ;
  if (!stralloc_catb(&sa, "\0  ", 3)) dienomem() ;
  uplistpos = sa.len ;
  e = get_list_of_up(&sa) ;
  if (e) _exit(e) ;
  if (!stralloc_0(&sa)) dienomem() ;

#ifdef S6_FRONTEND_USE_UTIL_LINUX
  if (g->color)
  {
    argv[m++] = EXECLINE_EXTBINPREFIX "pipeline" ;
    argv[m++] = "-w" ;
    argv[m++] = "--" ;
    argv[m++] = " column" ;
    argv[m++] = " -ts/" ;
    argv[m++] = "",
  }
#endif
  argv[m++] = EXECLINE_EXTBINPREFIX "if" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "piperw" ;
  argv[m++] = " 3" ;
  argv[m++] = " 4" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "background" ;
  argv[m++] = "  " EXECLINE_EXTBINPREFIX "fdclose" ;
  argv[m++] = "  3" ;
  argv[m++] = "  " EXECLINE_EXTBINPREFIX "fdmove" ;
  argv[m++] = "  1" ;
  argv[m++] = "  4" ;
  argv[m++] = "  " EXECLINE_EXTBINPREFIX "heredoc" ;
  argv[m++] = "  0" ;
  argv[m++] = sa.s + uplistpos - 2 ;
  argv[m++] = " " ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "fdclose" ;
  argv[m++] = " 4" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "heredoc" ;
  argv[m++] = " 0" ;
  argv[m++] = sa.s ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "pipeline" ;
  argv[m++] = "  grep" ;
  argv[m++] = "  -Fxf" ;
  argv[m++] = "  /dev/fd/3" ;
  argv[m++] = " " ;
  argv[m++] = " sed" ;
  argv[m++] = " s|$|/up" ;
  argv[m++] = "" ;
  argv[m++] = EXECLINE_EXTBINPREFIX "piperw" ;
  argv[m++] = "3" ;
  argv[m++] = "4" ;
  argv[m++] = EXECLINE_EXTBINPREFIX "background" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "fdclose" ;
  argv[m++] = " 3" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "fdmove" ;
  argv[m++] = " 1" ;
  argv[m++] = " 4" ;
  argv[m++] = " " EXECLINE_EXTBINPREFIX "heredoc" ;
  argv[m++] = " 0" ;
  argv[m++] = sa.s + uplistpos - 1 ;
  argv[m++] = "" ;
  argv[m++] = EXECLINE_EXTBINPREFIX "fdclose" ;
  argv[m++] = "4" ;
  argv[m++] = EXECLINE_EXTBINPREFIX "heredoc" ;
  argv[m++] = "0" ;
  argv[m++] = sa.s + 1 ;
  argv[m++] = EXECLINE_EXTBINPREFIX "pipeline" ;
  argv[m++] = " grep" ;
  argv[m++] = " -Fxf" ;
  argv[m++] = " /dev/fd/3" ;
  argv[m++] = "" ;
  argv[m++] = "sed" ;
  argv[m++] = "s|$|/down" ;
  argv[m++] = 0 ;
  xmexec_n(argv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

void live_status (char const *const *argv)
{
  argv += gol_argv(argv, 0, 0, 0, 0, 0, 0) ;
  if (!argv) live_status_all() ;
  else live_status_some(argv) ;
  _exit(0) ;
}
