/* ISC license. */

#include <stdint.h>
#include <unistd.h>

#include <skalibs/buffer.h>
#include <skalibs/prog.h>
#include <skalibs/strerr.h>

#define USAGE "s6-frontend-helper-echore list"

/*
	0	1	2
st\ev	\0	\n	other

0			^p
START	END	START	WORD

1	n	n	p
WORD	END	START	WORD

^	0x10	print ^
n	0x20	print /\n
p	0x40	print cur
*/

static inline uint8_t cclass (char c)
{
  switch (c)
  {
    case '\0' : return 0 ;
    case '\n' : return 1 ;
    default : return 2 ;
  }
}

static void do_print (char const *s)
{
  static uint8_t const table[2][3] =
  {
    { 0x02, 0x00, 0x51 },
    { 0x22, 0x20, 0x41 },
  } ;
  uint8_t state = 0 ;
  while (state < 2)
  {
    char cur = *s++ ;
    uint8_t c = table[state][cclass(cur)] ;
    state = c & 0x03 ;
    if (c & 0x10) if (buffer_put(buffer_1, "^", 1) < 1) strerr_diefusys(111, "write to stdout") ;
    if (c & 0x20) if (buffer_put(buffer_1, "/\n", 2) < 2) strerr_diefusys(111, "write to stdout") ;
    if (c & 0x40) if (buffer_put(buffer_1, &cur, 1) < 1) strerr_diefusys(111, "write to stdout") ;
  }
}

int main (int argc, char const *const *argv)
{
  PROG = "s6-frontend-helper-echore" ;
  if (argc < 2) strerr_dieusage(100, USAGE) ;
  do_print(argv[1]) ;
  if (!buffer_flush(buffer_1)) strerr_diefusys(111, "write to stdout") ;
  _exit(0) ;
}
