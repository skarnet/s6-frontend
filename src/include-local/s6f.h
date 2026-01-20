/* ISC license. */

#ifndef S6F_H
#define S6F_H

#include <stdint.h>

#include <skalibs/stralloc.h>

typedef struct s6f_confdirs_s s6f_confdirs, *s6f_confdirs_ref ;
struct s6f_confdirs_s
{
  char const *scan ;  /* $XDG_RUNTIME_DIR/service */
  char const *live ;  /* $XDG_RUNTIME_DIR/s6-rc */
  char const *repo ;  /* $XDG_DATA_HOME/s6-frontend/repository */
  char const *boot ;  /* $XDG_CONFIG_HOME/s6-rc/compiled/current */
  char const *stmp ;  /* $XDG_RUNTIME_DIR/s6-frontend */
  char const *stol ;  /* $XDG_CONFIG_HOME/s6-rc/sources */
} ;

extern int s6f_confdir_open (char const *, int) ;

extern void s6f_report_state_change (uint32_t, unsigned char const *, unsigned char const *, char const *, int) ;

extern void s6f_user_get_confdirs (s6f_confdirs *, stralloc *) ;

extern size_t s6f_equote_space (char const *const *, unsigned int, char const *) ;
extern unsigned int s6f_equote (char const **, char const *const *, unsigned int, char const *, char *) ;

#endif
