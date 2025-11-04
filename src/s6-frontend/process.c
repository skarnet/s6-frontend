/* ISC license. */

#include <string.h>
#include <errno.h>

#include <skalibs/uint64.h>
#include <skalibs/stat.h>
#include <skalibs/types.h>
#include <skalibs/posixplz.h>
#include <skalibs/envexec.h>
#include <skalibs/djbunix.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 process [ process options ] subcommand [ subcommand options ] services... Type \"s6 process help\" for details."
#define dieusage() strerr_dieusage(100, USAGE)

static int check_service (char const *name, size_t scandirlen)
{
  struct stat st ;
  size_t namelen = strlen(name) ;
  char path[scandirlen + namelen + 2] ;
  memcpy(path, g->dirs.scan, scandirlen) ;
  path[scandirlen] = '/' ;
  memcpy(path + scandirlen + 1, name, namelen) ;
  path[scandirlen + 1 + namelen] = 0 ;
  return stat(path, &st) == -1 ? errno == ENOENT ? 0 : -1 : !!S_ISDIR(st.st_mode) ;
}

void process_check_services (char const *const *argv, size_t argc)
{
  size_t const scandirlen = strlen(g->dirs.scan) ;
  for (size_t i = 0 ; i < argc ; i++)
  {
    int r = check_service(argv[i], scandirlen) ;
    if (r == -1)
      strerr_diefu4sys(111, "stat ", g->dirs.scan, "/", argv[i]) ;
    else if (!r)
      strerr_dief3x(100, argv[i], "is not registered as a supervised service in ", g->dirs.scan) ;
  }
}

void process_send_svc (char const *svcopt, char const *const *argv, size_t argc, unsigned int timeout)
{
  size_t const scandirlen = strlen(g->dirs.scan) ;
  size_t len = 0 ;
  unsigned int m = 0 ;
  for (size_t i = 0 ; i < argc ; i++) len += scandirlen + 2 + strlen(argv[i]) ;
  char const *newargv[4 + (timeout ? 2 : 0) + argc] ;
  char s[len] ;
  char fmtt[timeout ? UINT_FMT : 1] ;

  newargv[m++] = S6_EXTBINPREFIX "s6-svc" ;
  newargv[m++] = svcopt ;
  if (timeout)
  {
    fmtt[uint_fmt(fmtt, timeout)] = 0 ;
    newargv[m++] = "-T" ;
    newargv[m++] = fmtt ;
  }
  newargv[m++] = "--" ;

  len = 0 ;
  for (size_t i = 0 ; i < argc ; i++)
  {
    size_t l = strlen(argv[i]) ;
    newargv[m++] = s + len ;
    memcpy(s + len, g->dirs.scan, scandirlen) ; len += scandirlen ;
    s[len++] = '/' ;
    memcpy(s + len, argv[i], l + 1) ; len += l + 1 ;
  }
  newargv[m++] = 0 ;
  xmexec_n(newargv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}


enum golb_e
{
  GOLB_WAIT = 0x01,
} ;

enum gola_e
{
  GOLA_TIMEOUT,
  GOLA_N
} ;

int process (char const *const *argv)
{
  static struct command_s const process_commands[] =
  {
    { .s = "help", .f = &process_help },
    { .s = "kill", .f = &process_kill },
    { .s = "restart", .f = &process_restart },
    { .s = "start", .f = &process_start },
    { .s = "status", .f = &process_status },
    { .s = "stop", .f = &process_stop },
  } ;
  static gol_bool const rgolb[] =
  {
    { .so = 'W', .lo = "nowait", .clear = GOLB_WAIT, .set = 0 },
    { .so = 'w', .lo = "wait",   .clear = 0, .set = GOLB_WAIT }
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 't', .lo = "timeout", .i = GOLA_TIMEOUT },
  } ;

  struct command_s *cmd ;
  process_options options = PROCESS_OPTIONS_ZERO ;
  char const *wgola[GOLA_N] = { 0 } ;

  PROG = "s6-frontend: process" ;
  argv += GOL_argv(argv, rgolb, rgola, &options.flags, wgola) ;
  if (!*argv) dieusage() ;
  if (wgola[GOLA_TIMEOUT] && !uint0_scan(wgola[GOLA_TIMEOUT], &options.timeout))
    strerr_dief1x(100, "timeout must be a numerical argument (milliseconds)") ;
  cmd = BSEARCH(struct command_s, *argv, process_commands) ;
  if (!cmd) dieusage() ;
  return (*cmd->f)(++argv, &options) ;
}
