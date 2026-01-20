/* ISC license. */

#include <unistd.h>

#include <skalibs/uint64.h>
#include <skalibs/buffer.h>
#include <skalibs/envexec.h>

#define USAGE "s6-frontend-helper-echo [ -n ] [ -s sep ] args..."

enum golb_e
{
  GOLB_CHOMP = 0x01,
} ;

enum gola_e
{
  GOLA_SEP,
  GOLA_N
} ;

int main (int argc, char const *const *argv)
{
  static gol_bool const rgolb[] =
  {
    { .so = 'n', .lo = "chomp", .clear = 0, .set = GOLB_CHOMP },
  } ;
  static gol_arg const rgola[] =
  {
    { .so = 's', .lo = "separator", .i = GOLA_SEP },
  } ;
  uint64_t wgolb = 0 ;
  char const *wgola[GOLA_N] = { [GOLA_SEP] = " " } ;
  PROG = "s6-frontend-helper-echo" ;
  {
    unsigned int golc = GOL_main(argc, argv, rgolb, rgola, &wgolb, wgola) ;
    argc -= golc ; argv += golc ;
  }

  for (; *argv ; argv++)
  {
    if (buffer_puts(buffer_1small, *argv) == -1) goto err ;
    if (argv[1] && buffer_put(buffer_1small, wgola[GOLA_SEP], 1) == -1) goto err ;
  }
  if (!(wgolb & GOLB_CHOMP))
  {
    if (buffer_put(buffer_1small, "\n", 1) == -1) goto err ;
  }
  if (!buffer_flush(buffer_1small)) goto err ;
  _exit(0) ;

err:
  strerr_diefu1sys(111, "write to stdout") ;
}
