/* ISC license. */

#ifndef S6F_H
#define S6F_H

#include <skalibs/stralloc.h>

typedef struct s6f_confdirs_s s6f_confdirs, *s6f_confdirs_ref ;
struct s6f_confdirs_s
{
  char const *scan ;  /* $XDG_RUNTIME_DIR/service */
  char const *live ;  /* $XDG_RUNTIME_DIR/s6-rc */
  char const *repo ;  /* $XDG_DATA_HOME/s6-frontend/repository */
  char const *boot ;  /* $XDG_CONFIG_HOME/s6-rc */
} ;

extern void s6f_user_get_confdirs (s6f_confdirs *, stralloc *) ;

#endif
