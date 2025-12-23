/* ISC license. */

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"

#ifdef S6_FRONTEND_USE_S6LI

#include <skalibs/exec.h>

static void system_hpr (char const *arg) gccattr_noreturn ;
static void system_hpr (char const *arg)
{
  unsigned int m = 0 ;
  char const *argv[4] ;
  argv[m++] =  S6_LINUX_INIT_EXTBINPREFIX "s6-linux-init-hpr" ;
  argv[m++] = arg ;
  argv[m++] = "--" ;
  argv[m++] = 0 ;
  xmexec_n(argv, cleanup_modif.s, cleanup_modif.len, cleanup_modif.n) ;
}

void system_halt (char const *const *argv)
{
  system_hpr("-h") ;
}

void system_poweroff (char const *const *argv)
{
  system_hpr("-p") ;
}

void system_reboot (char const *const *argv)
{
  system_hpr("-r") ;
}

#else

#include <skalibs/strerr.h>

static void system_hpr (char const *arg) gccattr_noreturn ;
static void system_hpr (char const *const *arg)
{
  strerr_dief3x(100, "s6 system ", arg, " needs s6-frontend to be built with s6-linux-init support") ;
}

void system_halt (char const *const *argv)
{
  system_hpr("halt") ;
}

void system_poweroff (char const *const *argv)
{
  system_hpr("poweroff") ;
}

void system_reboot (char const *const *argv)
{
  system_hpr("reboot") ;
}

#endif
