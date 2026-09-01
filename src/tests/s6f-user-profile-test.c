/* ISC license. */

#include <stdio.h>

#include <skalibs/stralloc.h>

#include "s6f.h"

int main (int argc, char const *const *argv)
{
  s6f_confdirs dirs ;
  stralloc storage = STRALLOC_ZERO ;
  char const *sharedstores = argc > 1 ? argv[1] : "" ;

  s6f_user_get_confdirs(&dirs, &storage, sharedstores) ;
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
