/* ISC license. */

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

#ifdef S6_FRONTEND_USE_S6LI

#include <skalibs/exec.h>

static void live_hpr (char const *arg) gccattr_noreturn ;
static void live_hpr (char const *arg)
{
  unsigned int m = 0 ;
  char const *argv[4] ;
  argv[m++] =  S6_LINUX_INIT_EXTBINPREFIX "s6-linux-init-hpr" ;
  argv[m++] = arg ;
  argv[m++] = "--" ;
  argv[m++] = 0 ;
  xmexec_n(argv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

void live_halt (char const *const *argv)
{
  live_hpr("-h") ;
}

void live_poweroff (char const *const *argv)
{
  live_hpr("-p") ;
}

void live_reboot (char const *const *argv)
{
  live_hpr("-r") ;
}

#else

#include <skalibs/strerr.h>

static void live_hpr (char const *arg) gccattr_noreturn ;
static void live_hpr (char const *const *arg)
{
  strerr_dief3x(100, "s6 live ", arg, " needs s6-frontend to be built with s6-linux-init support") ;
}

void live_halt (char const *const *argv)
{
  live_hpr("halt") ;
}

void live_poweroff (char const *const *argv)
{
  live_hpr("poweroff") ;
}

void live_reboot (char const *const *argv)
{
  live_hpr("reboot") ;
}

#endif
