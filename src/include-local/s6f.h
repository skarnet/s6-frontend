/* ISC license. */

#ifndef S6F_H
#define S6F_H

#include <sys/stat.h>
#include <stdint.h>

#include <skalibs/stralloc.h>

typedef struct s6f_confdirs_s s6f_confdirs, *s6f_confdirs_ref ;
struct s6f_confdirs_s
{
  char const *scan ;  /* $XDG_RUNTIME_DIR/service */
  char const *live ;  /* $XDG_RUNTIME_DIR/s6-rc */
  char const *repo ;  /* $XDG_DATA_HOME/s6-frontend/repository */
  char const *boot ;  /* $XDG_CONFIG_HOME/s6-rc */
  char const *stmp ;  /* $XDG_RUNTIME_DIR/s6-frontend */
} ;

extern void s6f_mkdirp (char const *, mode_t) ;
extern int s6f_confdir_open (char const *, int) ;
extern int s6f_lock (char const *, int) ;

extern void s6f_report_state_change (uint32_t, unsigned char const *, unsigned char const *, char const *) ;

extern void s6f_user_get_confdirs (s6f_confdirs *, stralloc *) ;

#endif
