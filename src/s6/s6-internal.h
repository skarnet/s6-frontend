/* ISC license. */

#ifndef S6_INTERNAL_H
#define S6_INTERNAL_H

#include <stdlib.h>

#include <skalibs/functypes.h>
#include <s6-frontend/config.h>


 /* util */

extern int keycmp (void const *, void const *) ;
#define BSEARCH(type, key, array) bsearch(key, (array), sizeof(array)/sizeof(type), sizeof(type), &keycmp)


 /* help */

extern int help (char const *const *) ;


 /* version */

extern int version (char const *const *) ;


 /* main */

struct global_s
{
  unsigned int verbosity ;
} ;
#define GLOBAL_ZERO \
{ \
  .verbosity = 1, \
}

extern struct global_s *g ;

struct command_s
{
  char const *s ;
  main_func_ref f ;
} ;
#define COMMAND_ZERO { .s = 0, .f = 0 }

#endif
