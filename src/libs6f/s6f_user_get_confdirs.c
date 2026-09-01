/* ISC license. */

#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <skalibs/types.h>
#include <skalibs/strerr.h>
#include <skalibs/stralloc.h>

#include "s6f.h"

#define dienomem() strerr_diefu1sys(111, "allocate memory")

static int isabsolutepath (char const *s)
{
  return s && s[0] == '/' ;
}

static void catpath (stralloc *sa, char const *base, char const *suffix)
{
  if ((base && !stralloc_cats(sa, base))
   || !stralloc_cats(sa, suffix)
   || !stralloc_0(sa)) dienomem() ;
}

void s6f_user_get_confdirs (s6f_confdirs *dirs, stralloc *storage, char const *sharedstores)
{
  size_t scanpos, livepos, repopos, bootpos, stmppos, stolpos ;
  size_t homelen = 0 ;
  struct passwd *pw = 0 ;
  char const *home = getenv("HOME") ;
  char const *confighome = getenv("XDG_CONFIG_HOME") ;
  char const *statehome = getenv("XDG_STATE_HOME") ;
  char const *runtime = getenv("XDG_RUNTIME_DIR") ;

  if (!runtime || !*runtime) strerr_dienotset(100, "XDG_RUNTIME_DIR") ;
  if (!isabsolutepath(runtime)) strerr_dief1x(100, "XDG_RUNTIME_DIR must be an absolute path") ;
  if (!isabsolutepath(confighome)) confighome = 0 ;
  if (!isabsolutepath(statehome)) statehome = 0 ;

  if ((!confighome || !statehome) && !isabsolutepath(home))
  {
    uid_t uid = getuid() ;
    errno = 0 ;
    pw = getpwuid(uid) ;
    if (!pw)
    {
      char fmt[UID_FMT] ;
      fmt[uid_fmt(fmt, uid)] = 0 ;
      if (errno) strerr_diefu2sys(111, "getpwuid for user ", fmt) ;
      else strerr_diefu3x(100, "getpwuid for user ", fmt, ": uid not found in passwd database") ;
    }
    if (!isabsolutepath(pw->pw_dir)) strerr_dief1x(100, "home directory must be an absolute path") ;
    homelen = strlen(pw->pw_dir) ;
  }
  char homeinstack[homelen + 1] ;
  if (homelen)
  {
    memcpy(homeinstack, pw->pw_dir, homelen + 1) ;
    home = homeinstack ;
  }

  scanpos = storage->len ;
  catpath(storage, runtime, "/service") ;

  livepos = storage->len ;
  catpath(storage, runtime, "/s6-rc") ;

  repopos = storage->len ;
  if (!statehome)
  {
    if (!stralloc_cats(storage, home) || !stralloc_cats(storage, "/.local/state")) dienomem() ;
  }
  catpath(storage, statehome, "/s6-rc/repository") ;

  bootpos = storage->len ;
  if (!statehome)
  {
    if (!stralloc_cats(storage, home) || !stralloc_cats(storage, "/.local/state")) dienomem() ;
  }
  catpath(storage, statehome, "/s6-rc/compiled/current") ;

  stmppos = storage->len ;
  catpath(storage, runtime, "/s6-frontend") ;

  stolpos = storage->len ;
  if (sharedstores && *sharedstores)
  {
    size_t n = strlen(sharedstores) ;
    if (!stralloc_catb(storage, sharedstores, n)
     || (sharedstores[n - 1] != ':' && !stralloc_catb(storage, ":", 1))) dienomem() ;
  }
  if (!confighome)
  {
    if (!stralloc_cats(storage, home) || !stralloc_cats(storage, "/.config")) dienomem() ;
  }
  catpath(storage, confighome, "/s6/sources") ;

 /* Don't add to storage past this point. */

  dirs->scan = storage->s + scanpos ;
  dirs->live = storage->s + livepos ;
  dirs->repo = storage->s + repopos ;
  dirs->boot = storage->s + bootpos ;
  dirs->stmp = storage->s + stmppos ;
  dirs->stol = storage->s + stolpos ;
}
