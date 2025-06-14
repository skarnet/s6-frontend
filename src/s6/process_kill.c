/* ISC license. */

#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/posixplz.h>
#include <skalibs/types.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>
#include <skalibs/env.h>
#include <skalibs/sig.h>

#include <s6/supervise.h>

#include "s6-internal.h"

#define USAGE "s6 process kill [ --signal=sig ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

static int process_kill_hack_kill (int sig, char const *const *argv)
{
  size_t scandirlen = strlen(g->dirs.scan) ;
  if (g->verbosity)
  {
    char fmt[INT_FMT] ;
    fmt[int_fmt(fmt, sig)] = 0 ;
    strerr_warnw3x("signal ", fmt, " is not natively supported by s6-svc, results may be unreliable") ;
  }

  for (; *argv ; argv++)
  {
    s6_svstatus_t status ;
    size_t arglen = strlen(*argv) ;
    char path[scandirlen + arglen + 2] ;
    memcpy(path, g->dirs.scan, scandirlen) ;
    path[scandirlen] = '/' ;
    memcpy(path + scandirlen + 1, *argv, arglen) ;
    path[scandirlen + 1 + arglen] = 0 ;
    if (!s6_svstatus_read(path, &status))
      strerr_diefu2sys(111, "read status file for service ", path) ;
    if (status.pid && !status.flagfinishing) kill(status.pid, sig) ;
  }
  return 0 ;
}


enum process_kill_gola_e
{
  PROCESS_KILL_GOLA_SIGNAL,
  PROCESS_KILL_GOLA_N
} ;

static gol_arg const process_kill_gola[PROCESS_KILL_GOLA_N] =
{
  { .so = 's', .lo = "signal", .i = PROCESS_KILL_GOLA_SIGNAL },
} ;

int process_kill (char const *const *argv)
{
  uint64_t golb = 0 ;
  char const *gola[PROCESS_KILL_GOLA_N] = { 0 } ;
  char const *svcopt = 0 ;
  size_t argc ;
  int sig = SIGTERM ;
  PROG = "s6 process kill" ;

  argv += gol_argv(argv, 0, 0, process_kill_gola, PROCESS_KILL_GOLA_N, &golb, gola) ;
  if (!argv) dieusage() ;
  if (gola[PROCESS_KILL_GOLA_SIGNAL])
  {
    if (!sig0_scan(gola[PROCESS_KILL_GOLA_SIGNAL], &sig))
      strerr_dief1x(100, "--signal= argument must be the name or number of a signal") ;
  }
  argc = env_len(argv) ;
  process_check_services(argv, argc) ;
  switch (sig)
  {
    case SIGALRM : svcopt = "-a" ; break ;
    case SIGABRT : svcopt = "-b" ; break ;
    case SIGQUIT : svcopt = "-q" ; break ;
    case SIGHUP  : svcopt = "-h" ; break ;
    case SIGKILL : svcopt = "-k" ; break ;
    case SIGTERM : svcopt = "-t" ; break ;
    case SIGINT  : svcopt = "-i" ; break ;
    case SIGUSR1 : svcopt = "-1" ; break ;
    case SIGUSR2 : svcopt = "-2" ; break ;
    case SIGSTOP : svcopt = "-p" ; break ;
    case SIGCONT : svcopt = "-c" ; break ;
    case SIGWINCH: svcopt = "-y" ; break ;
  }
  return svcopt ? process_send_svc(svcopt, argv, argc) : process_kill_hack_kill(sig, argv) ;
}
