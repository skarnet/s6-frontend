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

void s6f_user_get_confdirs (s6f_confdirs *dirs, stralloc *storage)
{
  size_t scanpos, livepos, repopos, bootpos, stmppos ;
  size_t homelen = 0 ;
  struct passwd *pw = 0 ;
  char const *home = 0 ;
  char const *datahome = getenv("XDG_DATA_HOME") ;
  char const *confighome = getenv("XDG_DATA_HOME") ;
  char const *runtime = getenv("XDG_RUNTIME_DIR") ;
  if (!runtime) strerr_dienotset(100, "XDG_RUNTIME_DIR") ;

  if (!datahome || !confighome)
  {
    home = getenv("HOME") ;
    if (!home)
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
      homelen = strlen(pw->pw_dir) ;
    }
  }
  char homeinstack[homelen + 1] ;
  if (homelen)
  {
    memcpy(homeinstack, pw->pw_dir, homelen) ;
    homeinstack[homelen] = 0 ;
    home = homeinstack ;
  }
  
  scanpos = storage->len ;
  if (!stralloc_cats(storage, runtime)
   || !stralloc_cats(storage, "/service")
   || !stralloc_0(storage)) dienomem() ;

  livepos = storage->len ;
  if (!stralloc_cats(storage, runtime)
   || !stralloc_cats(storage, "/s6-rc")
   || !stralloc_0(storage)) dienomem() ;

  repopos = storage->len ;
  if (!(datahome ? stralloc_cats(storage, datahome) : stralloc_cats(storage, home) && stralloc_cats(storage, "/.local/share"))
   || !stralloc_cats(storage, "/s6-frontend/repository")
   || !stralloc_0(storage)) dienomem() ;
  
  bootpos = storage->len ;
  if (!(confighome ? stralloc_cats(storage, confighome) : stralloc_cats(storage, home) && stralloc_cats(storage, "/.config"))
   || !stralloc_cats(storage, "/s6-rc/compiled/current")
   || !stralloc_0(storage)) dienomem() ;

  stmppos = storage->len ;
  if (!stralloc_cats(storage, runtime)
   || !stralloc_cats(storage, "/s6-frontend")
   || !stralloc_0(storage)) dienomem() ;

 /* Don't add to storage past this point */

  dirs->scan = storage->s + scanpos ;
  dirs->live = storage->s + livepos ;
  dirs->repo = storage->s + repopos ;
  dirs->boot = storage->s + bootpos ;
  dirs->stmp = storage->s + stmppos ;
}
