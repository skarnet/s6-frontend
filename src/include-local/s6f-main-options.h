/* ISC license. */

#ifndef S6F_MAIN_OPTIONS_H
#define S6F_MAIN_OPTIONS_H

#include <skalibs/gol.h>

#define S6F_GOLB_HELP 0x01
#define S6F_GOLB_USER 0x04

enum s6f_gola_e
{
  S6F_GOLA_SCANDIR,
  S6F_GOLA_LIVEDIR,
  S6F_GOLA_REPODIR,
  S6F_GOLA_BOOTDB,
  S6F_GOLA_STMPDIR,
  S6F_GOLA_STORELIST,
  S6F_GOLA_VERBOSITY,
  S6F_GOLA_FDHUSER,
  S6F_GOLA_COLOR,
  S6F_GOLA_N
} ;

static gol_bool const s6f_main_rgolb[] =
{
  { .so = 'h', .lo = "help", .clear = 0, .set = S6F_GOLB_HELP },
  { .so = 'u', .lo = "user", .clear = 0, .set = S6F_GOLB_USER },
} ;

static gol_arg const s6f_main_rgola[] =
{
  { .so = 's', .lo = "scandir", .i = S6F_GOLA_SCANDIR },
  { .so = 'l', .lo = "livedir", .i = S6F_GOLA_LIVEDIR },
  { .so = 'r', .lo = "repodir", .i = S6F_GOLA_REPODIR },
  { .so = 'c', .lo = "bootdb", .i = S6F_GOLA_BOOTDB },
  { .so = 0,   .lo = "stmpdir", .i = S6F_GOLA_STMPDIR },
  { .so = 0,   .lo = "storelist", .i = S6F_GOLA_STORELIST },
  { .so = 'v', .lo = "verbosity", .i = S6F_GOLA_VERBOSITY },
  { .so = 0,   .lo = "fdholder-user", .i = S6F_GOLA_FDHUSER },
  { .so = 0,   .lo = "color", .i = S6F_GOLA_COLOR },
} ;

#endif
