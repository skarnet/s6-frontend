#!/bin/sh -e

if test -z "$1" ; then
  echo "$0: usage: $0 name < textfile > cfile" 1>&2
  exit 1
fi

echo '/* ISC license. */'
echo
echo '#include <unistd.h>'
echo
echo '#include <skalibs/buffer.h>'
echo '#include <skalibs/strerr.h>'
echo
echo '#include "s6-frontend-internal.h"'
echo
echo "void ${1}_help (char const *const *argv)"
echo '{'
echo "  static char const *const ${1}_help_message ="

sed 's/"/\\"/g; s/^/    "/; s/$/\\n"/'

echo '  ;'
echo "  if (!buffer_putsflush(buffer_1, ${1}_help_message))"
echo '    strerr_diefu1sys(111, "write to stdout") ;'
echo '  (void)argv ;'
echo '  _exit(0) ;'
echo '}'
