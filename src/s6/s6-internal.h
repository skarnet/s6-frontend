/* ISC license. */

#ifndef S6_INTERNAL_H
#define S6_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <skalibs/functypes.h>
#include <skalibs/stralloc.h>

#include <s6-frontend/config.h>
#include "s6f.h"

 /* util */

extern int keycmp (void const *, void const *) ;
#define BSEARCH(type, key, array) bsearch(key, (array), sizeof(array)/sizeof(type), sizeof(type), &keycmp)


 /* help */

extern int process_help (char const *const *) ;
extern int service_help (char const *const *) ;

extern int help (char const *const *) ;


 /* process */

extern void process_check_services (char const *const *, size_t) ;
extern int process_send_svc (char const *, char const *const *, size_t) ;

extern int process_kill (char const *const *) ;
extern int process_restart (char const *const *) ;
extern int process_start (char const *const *) ;
extern int process_status (char const *const *) ;
extern int process_stop (char const *const *) ;

extern int process (char const *const *) ;


 /* service */

extern int service_start (char const *const *) ;
extern int service_status (char const *const *) ;
extern int service_stop (char const *const *) ;

extern int service (char const *const *) ;


 /* main */

struct global_s
{
  unsigned int verbosity ;
  s6f_confdirs dirs ;
  stralloc userstorage ;
  uint8_t isuser : 1 ;
  uint8_t istty : 1 ;
  uint8_t color : 1 ;
} ;
#define GLOBAL_ZERO \
{ \
  .verbosity = 1, \
  .dirs = \
  { \
    .scan = S6_FRONTEND_SCANDIR, \
    .live = S6_FRONTEND_LIVEDIR, \
    .repo = S6_FRONTEND_REPODIR, \
    .boot = S6_FRONTEND_BOOTDIR, \
  }, \
  .userstorage = STRALLOC_ZERO, \
  .isuser = 0, \
  .istty = 0, \
  .color = 0 \
}

struct command_s
{
  char const *s ;
  main_func_ref f ;
} ;
#define COMMAND_ZERO { .s = 0, .f = 0 }

extern struct global_s *g ;

extern int version (char const *const *) ;

#endif
