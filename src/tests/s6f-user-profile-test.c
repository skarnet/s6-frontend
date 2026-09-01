/* ISC license. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <skalibs/stralloc.h>

#include <s6-frontend/config.h>

#include "s6f.h"

#define BUFSIZE 8192

static int check (char const *name, char const *actual, char const *expected)
{
  if (!strcmp(actual, expected)) return 1 ;
  fprintf(stderr, "%s: expected %s, got %s\n", name, expected, actual) ;
  return 0 ;
}

static int isabsolutepath (char const *s)
{
  return s && s[0] == '/' ;
}

int main (int argc, char const *const *argv)
{
  s6f_confdirs dirs ;
  stralloc storage = STRALLOC_ZERO ;
  char const *home = getenv("HOME") ;
  char const *runtime = getenv("XDG_RUNTIME_DIR") ;
  char const *confighome = getenv("XDG_CONFIG_HOME") ;
  char const *statehome = getenv("XDG_STATE_HOME") ;
  char const *sharedstores = argc > 1 ? argv[1] : S6_FRONTEND_USER_STORELIST ;
  char configbase[BUFSIZE] ;
  char statebase[BUFSIZE] ;
  char expected[BUFSIZE] ;
  int ok = 1 ;

  s6f_user_get_confdirs(&dirs, &storage, sharedstores) ;
  if (!isabsolutepath(confighome))
  {
    if (!isabsolutepath(home)) return 100 ;
    if (snprintf(configbase, sizeof configbase, "%s/.config", home) >= (int)sizeof configbase) return 100 ;
    confighome = configbase ;
  }
  if (!isabsolutepath(statehome))
  {
    if (!isabsolutepath(home)) return 100 ;
    if (snprintf(statebase, sizeof statebase, "%s/.local/state", home) >= (int)sizeof statebase) return 100 ;
    statehome = statebase ;
  }

#define CHECK_PATH(field, base, suffix) \
  do { \
    if (snprintf(expected, sizeof expected, "%s%s", base, suffix) >= (int)sizeof expected) return 100 ; \
    ok &= check(#field, dirs.field, expected) ; \
  } while (0)

  CHECK_PATH(scan, runtime, "/service") ;
  CHECK_PATH(live, runtime, "/s6-rc") ;
  CHECK_PATH(repo, statehome, S6_FRONTEND_USER_REPO_SUBDIR) ;
  CHECK_PATH(boot, statehome, S6_FRONTEND_USER_BOOTDB_SUBDIR) ;
  CHECK_PATH(stmp, runtime, "/s6-frontend") ;
  if (sharedstores && *sharedstores)
  {
    size_t n = strlen(sharedstores) ;
    if (snprintf(expected, sizeof expected, "%s%s%s%s", sharedstores,
      sharedstores[n - 1] == ':' ? "" : ":", confighome,
      S6_FRONTEND_USER_SOURCE_SUBDIR) >= (int)sizeof expected) return 100 ;
  }
  else if (snprintf(expected, sizeof expected, "%s%s", confighome,
    S6_FRONTEND_USER_SOURCE_SUBDIR) >= (int)sizeof expected) return 100 ;
  ok &= check("stol", dirs.stol, expected) ;
  if (!ok) return 1 ;

  if (printf("scandir=%s\n"
             "livedir=%s\n"
             "repodir=%s\n"
             "bootdb=%s\n"
             "stmpdir=%s\n"
             "storelist=%s\n",
             dirs.scan, dirs.live, dirs.repo, dirs.boot, dirs.stmp, dirs.stol) < 0) return 111 ;
  stralloc_free(&storage) ;
  return 0 ;
}
