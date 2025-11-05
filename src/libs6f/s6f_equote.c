/* ISC license. */

#include <string.h>

#include "s6f.h"

size_t s6f_equote_space (char const *const *argv, unsigned int argc, char const *prefix)
{
  size_t l = (prefix ? strlen(prefix) + 3 : 2) * argc ;
  for (unsigned int i = 0 ; i < argc ; i++) l += strlen(argv[i]) ;
  return l ;
}

unsigned int s6f_equote (char const **newargv, char const *const *argv, unsigned int argc, char const *prefix, char *s)
{
  size_t prefixlen = prefix ? strlen(prefix) : 0 ;
  for (unsigned int i = 0 ; i < argc ; i++)
  {
    newargv[i] = s ;
    *s++ = ' ' ;
    if (prefix)
    {
      memcpy(s, prefix, prefixlen) ;
      *s++ = '/' ;
    }
    s = stpcpy(s, argv[i]) + 1 ;
  }
  newargv[argc] = "" ;
  return argc + 1 ;
}
