/* ISC license. */

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include <skalibs/posixplz.h>
#include <skalibs/uint64.h>
#include <skalibs/env.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr.h>
#include <skalibs/gol.h>
#include <skalibs/djbunix.h>

#include <s6/config.h>

#include "s6-frontend-internal.h"

#define USAGE "s6 process status [ -L | -l ] services..."
#define dieusage() strerr_dieusage(100, USAGE)

static int spawn_and_wait (char const *const *argv)
{
  int wstat ;
  pid_t r ;
  pid_t pid = main_spawn(argv) ;
  if (!pid)
  {
    if (g->verbosity) strerr_warnwu2sys("spawn ", argv[0]) ;
    return 1 ;
  }
  r = wait_pid(pid, &wstat) ;
  if (r != pid)
  {
    if (g->verbosity) strerr_warnwu2sys("wait for ", argv[0]) ;
    return 1 ;
  }
  return !!WIFSIGNALED(wstat) || !!WEXITSTATUS(wstat) ;
}

static inline int do_status (char const *dir, int withlog)
{
  int e ;
  char const *argv[4] = { S6_EXTBINPREFIX "s6-svstat", "--", dir, 0 } ;
  size_t dirlen = strlen(dir) ;
  buffer_puts(buffer_1, dir) ;
  buffer_putsflush(buffer_1, ": ") ;
  e = spawn_and_wait(argv) ;
  if (withlog && (dirlen < 5 || strcmp(dir + dirlen - 4, "/log")))
  {
    struct stat st ;
    char log[dirlen + 5] ;
    memcpy(log, dir, dirlen) ;
    memcpy(log + dirlen, "/log", 5) ;
    if (stat(log, &st) < 0)
    {
      if (errno != ENOENT)
      {
        if (g->verbosity) strerr_warnwu2sys("stat", log) ;
        e = 1 ;
      }
    }
    else if (S_ISDIR(st.st_mode))
    {
      argv[2] = log ;
      buffer_puts(buffer_1, log) ;
      buffer_putsflush(buffer_1, ": ") ;
      e |= spawn_and_wait(argv) ;
    }
  }
  return e ;
}

enum golb_e
{
  GOLB_WITHLOGS = 0x01,
} ;

void process_status (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'L', .lo = "without-logs", .clear = GOLB_WITHLOGS, .set = 0 },
    { .so = 'l', .lo = "with-logs", .clear = 0, .set = GOLB_WITHLOGS },
  } ;
  size_t scandirlen = strlen(g->dirs.scan) ;
  uint64_t wgolb = 0 ;
  int e = 0 ;

  argv += gol_argv(argv, rgolb, 2, 0, 0, &wgolb, 0) ;
  if (!*argv) dieusage() ;
  process_check_services(argv, env_len(argv)) ;
  for (; *argv ; argv++)
  {
    size_t len = strlen(*argv) ;
    char path[scandirlen + len + 2] ;
    memcpy(path, g->dirs.scan, scandirlen) ;
    path[scandirlen] = '/' ;
    memcpy(path + scandirlen + 1, *argv, len+1) ;
    if (do_status(path, wgolb & GOLB_WITHLOGS)) e = 1 ;
  }
  _exit(e) ;
}
