/* ISC license. */

#include <unistd.h>

#include <skalibs/types.h>
#include <skalibs/buffer.h>
#include <skalibs/gol.h>
#include <skalibs/stralloc.h>
#include <skalibs/strerr.h>
#include <skalibs/skamisc.h>

#include <s6-frontend/config.h>
#include "s6-frontend-internal.h"
#include "s6f.h"

#define dieout() strerr_diefusys(111, "write to stdout") ;

typedef void out_func (char const *, char const *, stralloc *) ;
typedef out_func *out_func_ref ;

enum golb_e
{
  GOLB_QUOTE = 0x01,
} ;

static void print_quoted (char const *var, char const *value, stralloc *sa)
{
  sa->len = 0 ;
  if (!string_quotes(sa, value)) strerr_diefusys(111, "stralloc_catb") ;
  if (buffer_puts(buffer_1, var) == -1
   || buffer_put(buffer_1, "=", 1) < 1
   || buffer_put(buffer_1, sa->s, sa->len) < sa->len
   || buffer_put(buffer_1, "\n", 1) < 1) dieout() ;
}

static void print_unquoted (char const *var, char const *value, stralloc *sa)
{
  if (buffer_puts(buffer_1, var) == -1
   || buffer_put(buffer_1, "=", 1) < 1
   || buffer_puts(buffer_1, value) == -1
   || buffer_put(buffer_1, "\n", 1) < 1) dieout() ;
  (void)sa ;
}

void version_export (char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'q', .lo = "quote", .clear = 0, .set = GOLB_QUOTE },
  } ;
  stralloc sa = STRALLOC_ZERO ;
  uint64_t wgolb = 0 ;
  out_func_ref f = &print_unquoted ;
  size_t len ;
  char fmt[UINT_FMT] ;

  argv += gol_argv(argv, rgolb, 1, 0, 0, &wgolb, 0) ;
  if (wgolb & GOLB_QUOTE) f = &print_quoted ;

  if (!buffer_puts(buffer_1, "version=" S6_FRONTEND_VERSION "\n")) dieout() ;
  (*f)("scandir", g->dirs.scan, &sa) ;
  (*f)("livedir", g->dirs.live, &sa) ;
  (*f)("repodir", g->dirs.repo, &sa) ;
  (*f)("bootdb", g->dirs.scan, &sa) ;
  (*f)("stmpdir", g->dirs.stmp, &sa) ;
  (*f)("storelist", g->dirs.stol, &sa) ;
  len = uint_fmt(fmt, g->verbosity) ; fmt[len++] = '\n' ;
  if (!buffer_puts(buffer_1, "verbosity=")
   || !buffer_putflush(buffer_1, fmt, len)) dieout() ;
  _exit(0) ;
}
