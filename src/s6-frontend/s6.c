/* ISC license. */

#include <stdlib.h>

#include <skalibs/envexec.h>
#include <skalibs/stralloc.h>
#include <skalibs/djbunix.h>

#include <execline/config.h>

#include <s6-frontend/config.h>

enum golb_e
{
  GOLB_HELP = 0x01,
  GOLB_VERSION = 0x02,
  GOLB_USER = 0x04,
} ;

enum gola_e
{
  GOLA_SCANDIR,
  GOLA_LIVEDIR,
  GOLA_REPODIR,
  GOLA_BOOTDB,
  GOLA_STMPDIR,
  GOLA_STORELIST,
  GOLA_VERBOSITY,
  GOLA_FDHUSER,
  GOLA_COLOR,
  GOLA_N
} ;

#define dienomem() strerr_diefusys(111, "build command line")

static inline enum gola_e gola_pos (gol_arg const *tab, size_t n, enum gola_e i)
{
  for (enum gola_e j = 0 ; j < n ; j++) if (tab[j].i == i) return j ;
  strerr_dief(101, "can't happen: rgola does not cover all gola_e values") ;
}

int main (int argc, char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'h', .lo = "help", .clear = 0, .set = GOLB_HELP },
    { .so = 'u', .lo = "user", .clear = 0, .set = GOLB_USER },
    { .so = 'S', .lo = "system", .clear = GOLB_USER, .set = 0 },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 's', .lo = "scandir", .i = GOLA_SCANDIR },
    { .so = 'l', .lo = "livedir", .i = GOLA_LIVEDIR },
    { .so = 'r', .lo = "repodir", .i = GOLA_REPODIR },
    { .so = 'c', .lo = "bootdb", .i = GOLA_BOOTDB },
    { .so = 0,   .lo = "stmpdir", .i = GOLA_STMPDIR },
    { .so = 0,   .lo = "storelist", .i = GOLA_STORELIST },
    { .so = 'v', .lo = "verbosity", .i = GOLA_VERBOSITY },
    { .so = 0,   .lo = "fdholder-user", .i = GOLA_FDHUSER },
    { .so = 0,   .lo = "color", .i = GOLA_COLOR },
  } ;

  stralloc sa = STRALLOC_ZERO ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { 0 } ;
  size_t optpos[GOLA_N] = { 0 } ;
  unsigned int conf_overrides = 0 ;
  unsigned int m = 0 ;
  PROG = "s6" ;

  {
    unsigned int golc = GOL_main(argc, argv, rgolb, rgola, &wgolb, wgola) ;
    argc -= golc ; argv += golc ;
  }

  for (enum gola_e i = 0 ; i < GOLA_N ; i++) if (wgola[i])
  {
    enum gola_e j = gola_pos(rgola, sizeof(rgola)/sizeof(gol_arg const), i) ;
    optpos[i] = sa.len ;
    if (!stralloc_catb(&sa, "--", 2)
     || !stralloc_cats(&sa, rgola[j].lo)
     || !stralloc_catb(&sa, "=", 1)
     || !stralloc_cats(&sa, wgola[i])
     || !stralloc_0(&sa))
      dienomem() ;
    conf_overrides++ ;
  }

  char const *newargv[(wgolb & GOLB_USER ? 17 : 4) + 1 + conf_overrides + 1 + (wgolb & GOLB_HELP ? 1 : argc) + 1] ;

  if (wgolb & GOLB_USER)
  {
    static char const *const xdgvar[5] =
    {
      "XDG_RUNTIME_DIR",
      "XDG_DATA_HOME",
      "XDG_CONFIG_HOME",
      "XDG_STATE_HOME",
      "XDG_CACHE_HOME",
    } ;
    ssize_t xdgpos[5] = { -1, -1, -1, -1, -1 } ;
    int dosubst = 0 ;
    int fd ;
    for (unsigned int i = 0 ; i < 5 ; i++)
    {
      char const *x = getenv(xdgvar[i]) ;
      if (x)
      {
        dosubst = 1 ;
        xdgpos[i] = sa.len ;
        if (!stralloc_cats(&sa, " s|\\$")
         || !stralloc_cats(&sa, xdgvar[i])
         || !stralloc_cats(&sa, "|")
         || !stralloc_cats(&sa, x)
         || !stralloc_cats(&sa, "|g;s|\\${")
         || !stralloc_cats(&sa, xdgvar[i])
         || !stralloc_cats(&sa, "}|")
         || !stralloc_cats(&sa, x)
         || !stralloc_cats(&sa, "|g")
         || !stralloc_0(&sa))
          dienomem() ;
      }
    }
    {
      char const *x = getenv("S6_USER_CONF") ;
      if (!x) x = S6_FRONTEND_USER_CONF ;
      fd = open_readb(x) ;
      if (fd == -1) strerr_diefusys(111, "open ", x, " for reading") ;
      if (fd_move(0, fd) == -1) strerr_diefusys(111, "fd_move") ;
    }

    if (dosubst)
    {
      newargv[m++] = EXECLINE_BINPREFIX "pipeline" ;
      newargv[m++] = " sed" ;
      for (unsigned int i = 0 ; i < 5 ; i++) if (xdgpos[i] >= 0)
      {
        newargv[m++] = " -e" ;
        newargv[m++] = sa.s + xdgpos[i] ;
      }
    }
    newargv[m++] = "" ;
    newargv[m++] = EXECLINE_EXTBINPREFIX "envfile" ;
    newargv[m++] = "-I" ;
    newargv[m++] = "--" ;
    newargv[m++] = "-" ;
  }
  else
  {
    char const *conffile = getenv("S6_CONF") ;
    if (!conffile) conffile = S6_FRONTEND_CONF ;
    newargv[m++] = EXECLINE_EXTBINPREFIX "envfile" ;
    newargv[m++] = "-I" ;
    newargv[m++] = "--" ;
    newargv[m++] = conffile ;
  }

  newargv[m++] = S6_FRONTEND_LIBEXECPREFIX "s6-frontend" ;
  for (enum gola_e i = 0 ; i < GOLA_N ; i++)
    if (wgola[i]) newargv[m++] = sa.s + optpos[i] ;
  newargv[m++] = "--" ;
  if (wgolb & GOLB_HELP) newargv[m++] = "help" ;
  else while (argc--) newargv[m++] = *argv++ ;

  newargv[m++] = 0 ;
  xexec(newargv) ;
}
