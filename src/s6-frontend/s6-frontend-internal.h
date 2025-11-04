/* ISC license. */

#ifndef S6_INTERNAL_H
#define S6_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <skalibs/gccattributes.h>
#include <skalibs/uint64.h>
#include <skalibs/functypes.h>
#include <skalibs/bytestr.h>
#include <skalibs/stralloc.h>

#include <s6-frontend/config.h>
#include "s6f.h"

 /* util */

#define BSEARCH(type, key, array) bsearch(key, (array), sizeof(array)/sizeof(type), sizeof(type), &str_cmp)


 /* help */

extern int process_help (char const *const *) ;
extern int service_help (char const *const *) ;

extern int help (char const *const *) ;


 /* process */

typedef struct process_options_s process_options, *process_options_ref ;
struct process_options_s
{
  uint64_t flags ;
  unsigned int timeout ;
} ;
#define PROCESS_OPTIONS_ZERO { .flags = 0, .timeout = 0 }

extern void process_check_services (char const *const *, size_t) ;
extern int process_send_svc (char const *, char const *const *, size_t, unsigned int) gccattr_noreturn ;

extern int process_kill (char const *const *, process_options const *) ;
extern void process_restart (char const *const *, process_options const *) gccattr_noreturn ;
extern void process_start (char const *const *, process_options const *) gccattr_noreturn ;
extern void process_stop (char const *const *, process_options const *) gccattr_noreturn ;
extern int process_status (char const *const *, process_options const *) ;

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
    .boot = S6_FRONTEND_BOOTDB, \
    .stmp = S6_FRONTEND_STMPDIR, \
  }, \
  .userstorage = STRALLOC_ZERO, \
  .isuser = 0, \
  .istty = 0, \
  .color = 0 \
}

struct modif_s
{
  char const *s ;
  size_t len ;
  unsigned int n ;
} ;

extern struct modif_s const cleanup_modif ;

struct command_s
{
  char const *s ;
  main_func_ref f ;
} ;
#define COMMAND_ZERO { .s = 0, .f = 0 }

extern struct global_s *g ;

extern int version (char const *const *) ;

#endif
