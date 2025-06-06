/* ISC license. */

#ifndef S6_INTERNAL_H
#define S6_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
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


 /* process */

extern void process_check_services (char const *const *, size_t) ;
extern int process_send_svc (char const *, char const *const *, size_t) ;

extern int process (char const *const *) ;
extern int process_help (char const *const *) ;
extern int process_kill (char const *const *) ;
extern int process_restart (char const *const *) ;
extern int process_start (char const *const *) ;
extern int process_status (char const *const *) ;
extern int process_stop (char const *const *) ;


 /* main */

struct global_s
{
  unsigned int verbosity ;
  char const *scandir ;
  uint8_t color : 1 ;
} ;
#define GLOBAL_ZERO \
{ \
  .verbosity = 1, \
  .scandir = "/run/service", \
  .color = 0 \
}

extern struct global_s *g ;

struct command_s
{
  char const *s ;
  main_func_ref f ;
} ;
#define COMMAND_ZERO { .s = 0, .f = 0 }

#endif
