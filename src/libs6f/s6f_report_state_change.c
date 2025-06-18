/* ISC license. */

#include <string.h>

#include <skalibs/prog.h>
#include <skalibs/strerr.h>
#include <skalibs/djbunix.h>

#include <s6-rc/s6rc-db.h>

#include "s6f.h"

void s6f_report_state_change (uint32_t n, unsigned char const *oldstate, unsigned char const *newstate, char const *compiled)
{
  if (!memcmp(oldstate, newstate, n))
  {
    if (buffer_puts(buffer_1, PROG) < 0
     || buffer_puts(buffer_1, ": info: ") < 0
     || buffer_putsflush(buffer_1, "the global state has not changed.\n") < 0)
      strerr_diefu1sys(111, "write to stdout") ;
  }
  else
  {
    s6rc_db_t db ;
    int fd = openc_readb(compiled) ;
    if (fd == -1) strerr_diefu2sys(111, "open ", compiled) ;
    if (!s6rc_db_read_sizes(fd, &db))
      strerr_diefu2sys(111, "read db sizes for ", compiled) ;

    {
      s6rc_service_t serviceblob[n] ;
      char const *argvblob[db.nargvs] ;
      uint32_t depsblob[db.ndeps << 1] ;
      uint32_t producersblob[db.nproducers] ;
      int r ;
      char stringblob[db.stringlen] ;
      db.services = serviceblob ;
      db.argvs = argvblob ;
      db.deps = depsblob ;
      db.producers = producersblob ;
      db.string = stringblob ;

      r = s6rc_db_read(fd, &db) ;
      if (r < 0) strerr_diefu3sys(111, "read ", compiled, "/db") ;
      if (!r) strerr_dief3x(4, "invalid service database in ", compiled, "/db") ;

      if (buffer_puts(buffer_1, PROG) < 0
       || buffer_puts(buffer_1, ": info: the following atomic services have changed state:\n") < 0)
        strerr_diefu1sys(111, "write to stdout") ;
      for (uint32_t i = 0 ; i < n ; i++)
      {
        if ((oldstate[i] & 1) != (newstate[i] & 1))
        {
          if (buffer_puts(buffer_1, db.string + db.services[i].name) < 0
           || buffer_put(buffer_1, "\n", 1) < 1)
            strerr_diefu1sys(111, "write to stdout") ;
        }
      }
      if (!buffer_flush(buffer_1))
        strerr_diefu1sys(111, "write to stdout") ;
    }
    fd_close(fd) ;
  }
}
