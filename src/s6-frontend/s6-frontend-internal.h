/* ISC license. */

#ifndef S6_FRONTEND_INTERNAL_H
#define S6_FRONTEND_INTERNAL_H

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

#define BSEARCH(type, key, array) bsearch(key, (array), sizeof(array)/sizeof(type), sizeof(type), &stringkey_bcmp)


 /* help */

extern void help (char const *const *) gccattr_noreturn ;
extern void version (char const *const *) gccattr_noreturn ;


 /* live */

extern void live (char const *const *) gccattr_noreturn ;
extern void live_help (char const *const *) gccattr_noreturn ;

extern void live_boot (char const *const *) gccattr_noreturn ;
extern void live_init (char const *const *) gccattr_noreturn ;
extern void live_start_everything (char const *const *) gccattr_noreturn ;

extern void live_halt (char const *const *) gccattr_noreturn ;
extern void live_poweroff (char const *const *) gccattr_noreturn ;
extern void live_reboot (char const *const *) gccattr_noreturn ;

extern void live_restart (char const *const *) gccattr_noreturn ;
extern void live_start (char const *const *) gccattr_noreturn ;
extern void live_status (char const *const *) gccattr_noreturn ;
extern void live_stop (char const *const *) gccattr_noreturn ;
extern void live_stop_everything (char const *const *) gccattr_noreturn ;


 /* process */

extern void process_check_services (char const *const *, unsigned int) ;
extern void process_send_svc (char const *, char const *const *, unsigned int, unsigned int) gccattr_noreturn ;

extern void process (char const *const *) gccattr_noreturn ;
extern void process_help (char const *const *) gccattr_noreturn ;

extern void process_kill (char const *const *) gccattr_noreturn ;
extern void process_restart (char const *const *) gccattr_noreturn ;
extern void process_start (char const *const *) gccattr_noreturn ;
extern void process_stop (char const *const *) gccattr_noreturn ;
extern void process_status (char const *const *) gccattr_noreturn ;


 /* repository */

extern void repository (char const *const *) gccattr_noreturn ;
extern void repository_help (char const *const *) gccattr_noreturn ;

extern void repository_init (char const *const *) gccattr_noreturn ;
extern void repository_sync (char const *const *) gccattr_noreturn ;
extern void repository_list (char const *const *) gccattr_noreturn ;


 /* set */

extern int set_check_service_names (char const *const *, unsigned int) ;

extern void set (char const *const *) gccattr_noreturn ;
extern void set_help (char const *const *) gccattr_noreturn ;

extern void set_list (char const *const *) gccattr_noreturn ;
extern void set_status (char const *const *) gccattr_noreturn ;

extern void set_load (char const *const *) gccattr_noreturn ;
extern void set_save (char const *const *) gccattr_noreturn ;

extern void set_mask (char const *const *) gccattr_noreturn ;
extern void set_unmask (char const *const *) gccattr_noreturn ;
extern void set_enable (char const *const *) gccattr_noreturn ;
extern void set_disable (char const *const *) gccattr_noreturn ;


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

typedef void command_func (char const *const *) ;
typedef command_func *command_func_ref ;

struct command_s
{
  char const *s ;
  command_func_ref f ;
} ;
#define COMMAND_ZERO { .s = 0, .f = 0 }

extern struct global_s *g ;

#endif
