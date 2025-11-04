/* ISC license. */

#include <string.h>

#include "s6f.h"

size_t s6f_equote_space (char const *const *argv, unsigned int argc)
{
  size_t l = 0 ;
  for (unsigned int i = 0 ; i < argc ; i++) l += i + 2 + strlen(argv[i]) ;
  return l ;
}

unsigned int s6f_equote (char const **newargv, char const *const *argv, unsigned int argc, char *s)
{
  for (unsigned int i = 0 ; i < argc ; i++)
  {
    newargv[i] = s ;
    *s++ = ' ' ;
    s = stpcpy(s, argv[i]) + 1 ;
  }
  newargv[argc] = "" ;
  return argc + 1 ;
}
