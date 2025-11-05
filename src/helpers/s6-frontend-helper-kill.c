/* ISC license. */

#include <unistd.h>
#include <signal.h>

#include <skalibs/types.h>
#include <skalibs/prog.h>
#include <skalibs/strerr.h>

#define USAGE "s6-frontend-helper-kill signal pid..."
#define dieusage() strerr_dieusage(100, USAGE)

int main (int argc, char const *const *argv)
{
  unsigned int sig ;
  PROG = "s6-frontend-helper-kill" ;
  if (argc-- < 2) dieusage() ;
  argv++ ;
  if (!uint0_scan(*argv++, &sig)) dieusage() ;
  if (--argc)
  {
    pid_t pids[argc] ;
    for (unsigned int i = 0 ; i < argc ; i++)
      if (!pid0_scan(argv[i], pids + i))
        strerr_dief1x(100, "pids must be numerical") ;

    for (unsigned int i = 0 ; i < argc ; i++)
      kill(sig, pids[i]) ;
  }
  _exit(0) ;
}
