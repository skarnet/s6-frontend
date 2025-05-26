/* ISC license. */

#include <string.h>

#include "s6-internal.h"

struct starts_with_a_string_key_s
{
  char const *s ;
} ;

int keycmp (void const *a, void const *b)
{
  return strcmp((char const *)a, ((struct starts_with_a_string_key_s const *)b)->s) ;
}
