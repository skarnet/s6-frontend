/* ISC license. */

#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include <skalibs/posixplz.h>
#include <skalibs/strerr.h>
#include <skalibs/cspawn.h>
#include <skalibs/djbunix.h>

#include <s6/config.h>

#include "s6-internal.h"

static int check_service (char const *name, size_t scandirlen)
{
  struct stat st ;
  size_t namelen = strlen(name) ;
  char path[scandirlen + namelen + 2] ;
  memcpy(path, g->scandir, scandirlen) ;
  path[scandirlen] = '/' ;
  memcpy(path + scandirlen + 1, name, namelen) ;
  path[scandirlen + 1 + namelen] = 0 ;
  return stat(path, &st) == -1 ? errno == ENOENT ? 0 : -1 : !!S_ISDIR(st.st_mode) ;
}

void process_check_services (char const *const *argv, size_t argc)
{
  size_t scandirlen = strlen(g->scandir) ;
  for (size_t i = 0 ; i < argc ; i++)
  {
    int r = check_service(argv[i], scandirlen) ;
    if (r == -1)
      strerr_diefu4sys(111, "stat ", g->scandir, "/", argv[i]) ;
    else if (!r)
      strerr_dief3x(100, argv[i], "is not registered as a supervised service in ", g->scandir) ;
  }
}

int process_send_svc (char const *svcopt, char const *const *argv, size_t argc)
{
  char const *newargv[5] = { S6_EXTBINPREFIX "s6-svc", svcopt, "--", 0, 0 } ;
  size_t scandirlen = strlen(g->scandir) ;
  int wstat ;
  pid_t pids[argc] ;

  for (size_t i = 0 ; i < argc ; i++)
  {
    size_t arglen = strlen(argv[i]) ;
    char path[scandirlen + arglen + 2] ;
    memcpy(path, g->scandir, scandirlen) ;
    path[scandirlen] = '/' ;
    memcpy(path + scandirlen + 1, argv[i], arglen) ;
    path[scandirlen + 1 + arglen] = 0 ;
    newargv[3] = path ;
    pids[i] = cspawn(newargv[0], newargv, (char const *const *)environ, 0, 0, 0) ;
    if (!pids[i])
      strerr_diefu4sys(111, "spawn ", newargv[0], " command for service ", argv[i]) ;
  }
  waitn_posix(pids, argc, &wstat) ;
  return 0 ;
}
