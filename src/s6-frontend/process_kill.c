/* ISC license. */

#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/posixplz.h>
#include <skalibs/types.h>
#include <skalibs/envexec.h>
#include <skalibs/sig.h>

#include <s6/config.h>
#include <s6/supervise.h>

#include "s6f.h"
#include "s6-frontend-internal.h"

#define USAGE "s6 process kill [ --signal=sig ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

static void process_kill_hack_kill (int sig, char const *const *argv, unsigned int argc, int dowait, unsigned int timeout) gccattr_noreturn ;
static void process_kill_hack_kill (int sig, char const *const *argv, unsigned int argc, int dowait, unsigned int timeout)
{
  size_t scandirlen = strlen(g->dirs.scan) ;
  size_t l = 0 ;
  size_t equotelen = dowait ? s6f_equote_space(argv, argc, g->dirs.scan) : 1 ;
  unsigned int m = 0 ;
  char const *newargv[3 + argc + (dowait ? 4 + argc + (timeout ? 2 : 0) : 0)] ;
  char equotestorage[equotelen] ;
  char fmt[UINT_FMT + PID_FMT * argc] ;
  l = uint_fmt(fmt, (unsigned int)sig) ; fmt[l++] = 0 ;
  if (g->verbosity)
    strerr_warnw3x("signal ", fmt, " is not natively supported by s6-svc, cannot ensure reliability") ;
  if (dowait)
  {
    newargv[m++] = S6_EXTBINPREFIX "s6-svlisten" ;
    newargv[m++] = "-D" ;
    if (timeout)
    {
      fmt[uint_fmt(fmt, timeout)] = 0 ;
      newargv[m++] = "-t" ;
      newargv[m++] = fmt ;
    }
    newargv[m++] = "--" ;
    m += s6f_equote(newargv + m, argv, argc, g->dirs.scan, equotestorage) ;
  }

  newargv[m++] = S6_FRONTEND_LIBEXECPREFIX "s6-frontend-helper-kill" ;
  newargv[m++] = fmt ;
  for (unsigned int i = 0 ; i < argc ; i++)
  {
    s6_svstatus_t status ;
    size_t arglen = strlen(argv[i]) ;
    char path[scandirlen + arglen + 2] ;
    memcpy(path, g->dirs.scan, scandirlen) ;
    path[scandirlen] = '/' ;
    memcpy(path + scandirlen + 1, argv[i], arglen) ;
    path[scandirlen + 1 + arglen] = 0 ;
    if (!s6_svstatus_read(path, &status))
      strerr_diefu2sys(111, "read status file for service ", path) ;
    if (status.pid && !status.flagfinishing)
    {
      newargv[m++] = fmt + l ;
      l += pid_fmt(fmt + l, status.pid) ; fmt[l++] = 0 ;
    }
  }
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}


enum gola_e
{
  GOLA_SIGNAL,
  GOLA_N
} ;

static gol_arg const rgola[] =
{
  { .so = 's', .lo = "signal", .i = GOLA_SIGNAL },
} ;

void process_kill (char const *const *argv, process_options const *options)
{
  char const *wgola[GOLA_N] = { 0 } ;
  size_t argc ;
  int sig = SIGTERM ;
  char svcopt[5] = "-!\0\0\0" ;
  PROG = "s6-frontend: process kill" ;

  argv += gol_argv(argv, 0, 0, rgola, GOLA_N, 0, wgola) ;
  if (!argv) dieusage() ;
  if (wgola[GOLA_SIGNAL])
  {
    if (!sig0_scan(wgola[GOLA_SIGNAL], &sig))
      strerr_dief1x(100, "--signal= argument must be the name or number of a signal") ;
  }
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  switch (sig)
  {
    case SIGALRM : svcopt[1] = 'a' ; break ;
    case SIGABRT : svcopt[1] = 'b' ; break ;
    case SIGQUIT : svcopt[1] = 'q' ; break ;
    case SIGHUP  : svcopt[1] = 'h' ; break ;
    case SIGKILL : svcopt[1] = 'k' ; break ;
    case SIGTERM : svcopt[1] = 't' ; break ;
    case SIGINT  : svcopt[1] = 'i' ; break ;
    case SIGUSR1 : svcopt[1] = '1' ; break ;
    case SIGUSR2 : svcopt[1] = '2' ; break ;
    case SIGSTOP : svcopt[1] = 'p' ; break ;
    case SIGCONT : svcopt[1] = 'c' ; break ;
    case SIGWINCH: svcopt[1] = 'y' ; break ;
  }
  if (options->flags & 1) { svcopt[2] = 'w' ; svcopt[3] = 'D' ; }
  if (svcopt[1] != '!') process_send_svc(svcopt, argv, argc, options->timeout) ;
  else process_kill_hack_kill(sig, argv, argc, options->flags & 1, options->timeout) ;
}
