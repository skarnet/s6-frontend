/* ISC license. */

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <skalibs/uint64.h>
#include <skalibs/sgetopt.h>
#include <skalibs/buffer.h>
#include <skalibs/strerr.h>
#include <skalibs/stralloc.h>
#include <skalibs/genalloc.h>
#include <skalibs/direntry.h>
#include <skalibs/djbunix.h>
#include <skalibs/skamisc.h>
#include <skalibs/avltree.h>

#define USAGE "s6-frontend-config-preprocess file"
#define dieusage() strerr_dieusage(100, USAGE)
#define dienomem() strerr_diefu1sys(111, "stralloc_catb") ;
#define MAXDEPTH 100

static stralloc sa = STRALLOC_ZERO ;
static unsigned int depth = 0 ;


 /* Name storage */

static stralloc namesa = STRALLOC_ZERO ;

static void *name_dtok (uint32_t pos, void *aux)
{
  (void)aux ;
  return namesa.s + pos ;
}

static int name_cmp (void const *a, void const *b, void *aux)
{
  (void)aux ;
  return strcmp((char const *)a, (char const *)b) ;
}

static avltree namemap = AVLTREE_INIT(8, 3, 8, &name_dtok, &name_cmp, 0) ;


 /* Directory sorting */

static char *dname_cmp_base ;
static int dname_cmp (void const *a, void const *b)
{
  return strcmp(dname_cmp_base + *(size_t *)a, dname_cmp_base + *(size_t *)b) ;
}


 /* Recursive inclusion functions */

static int includefromhere (char const *, int) ;

static inline void includecwd (int once)
{
  genalloc ga = GENALLOC_ZERO ; /* size_t */
  DIR *dir ;
  size_t sabase = sa.len ;
  if (sagetcwd(&sa) < 0 || !stralloc_0(&sa)) dienomem() ;
  dir = opendir(".") ;
  if (!dir) strerr_diefu2sys(111, "opendir ", sa.s + sabase) ;

  for (;;)
  {
    direntry *d ;
    errno = 0 ;
    d = readdir(dir) ;
    if (!d) break ;
    if (d->d_name[0] == '.') continue ;
    if (!genalloc_catb(size_t, &ga, &sa.len, 1)) break ;
    if (!stralloc_catb(&sa, d->d_name, strlen(d->d_name)+1)) break ;
  }
  dir_close(dir) ;
  if (errno) strerr_diefu2sys(111, "readdir ", sa.s + sabase) ;

  dname_cmp_base = sa.s ;
  qsort(genalloc_s(size_t, &ga), genalloc_len(size_t, &ga), sizeof(size_t), &dname_cmp) ;

  for (size_t i = 0 ; i < genalloc_len(size_t, &ga) ; i++)
    if (!includefromhere(sa.s + genalloc_s(size_t, &ga)[i], once))
      strerr_dief4sys(1, "in ", sa.s + sabase, ": unable to include ", sa.s + genalloc_s(size_t, &ga)[i]) ;

  genalloc_free(size_t, &ga) ;
  sa.len = sabase ;
}

static int include (char const *file, int once)
{
  size_t sabase = sa.len ;
  size_t filelen = strlen(file) ;
  if (!sadirname(&sa, file, filelen) || !stralloc_0(&sa)) dienomem() ;
  if (chdir(sa.s + sabase) < 0) return 0 ;
  sa.len = sabase ;
  if (!sabasename(&sa, file, filelen)) dienomem() ;
  {
    char fn[sa.len + 1 - sabase] ;
    memcpy(fn, sa.s + sabase, sa.len - sabase) ;
    fn[sa.len - sabase] = 0 ;
    sa.len = sabase ;
    return includefromhere(fn, once) ;
  }
}

static inline int idcmd (char const *s)
{
  static char const *commands[] =
  {
    "include",
    "includeonce",
    "includedir",
    "includedironce",
    0
  } ;
  for (char const **p = commands ; *p ; p++)
    if (!strcmp(s, *p)) return p - commands ;
  return -1 ;
}

static inline unsigned char cclass (char c)
{
  static unsigned char const classtable[34] = "0444444443144344444444444444444432" ;
  return (unsigned char)c < 34 ? classtable[(unsigned char)c] - '0' : 4 ;
}

static int includefromhere (char const *file, int once)
{
  size_t sabase = sa.len ;
  if (sarealpath(&sa, file) < 0 || !stralloc_0(&sa)) return 0 ;

  if (once)
  {
    uint32_t dummy ;
    if (avltree_search(&namemap, sa.s + sabase, &dummy)) goto end ;
    {
      size_t cur = namesa.len ;
      size_t filelen = strlen(file) ;
      if (cur + filelen + 2 < cur || cur + filelen + 2 > UINT32_MAX)
        strerr_dief3x(3, "in ", sa.s + sabase, ": too much nesting") ;
      if (!stralloc_catb(&namesa, sa.s + sabase, sa.len - sabase)) dienomem() ;
      if (!avltree_insert(&namemap, cur)) dienomem() ;
    }
  }

  if (depth++ > MAXDEPTH)
    strerr_dief3x(3, "in ", sa.s + sabase, ": too much nesting") ;

  {
    static unsigned char const table[8][5] =
    {
      { 0x08, 0x10, 0x02, 0x11, 0x11 },
      { 0x08, 0x10, 0x11, 0x11, 0x11 },
      { 0x08, 0x00, 0x03, 0x04, 0x25 },
      { 0x08, 0x00, 0x03, 0x03, 0x03 },
      { 0x09, 0x09, 0x09, 0x04, 0x25 },
      { 0x09, 0x09, 0x09, 0x46, 0x25 },
      { 0x0a, 0x0a, 0x07, 0x06, 0x27 },
      { 0x88, 0x80, 0x27, 0x27, 0x27 }
    } ;
    stralloc localsa = STRALLOC_ZERO ;
    uint64_t line = 1 ;
    size_t sacur = sa.len ;
    int cmd = -1 ;
    buffer b ;
    char buf[4096] ;
    unsigned char state = 0 ;
    int fd = open_readb(file) ;
    if (fd < 0) strerr_diefu2sys(111, "open ", sa.s + sabase) ;
    buffer_init(&b, &buffer_read, fd, buf, 4096) ;

    if (buffer_put(buffer_1, "! 0 ", 4) < 4
     || buffer_put(buffer_1, sa.s + sabase, sacur - 1 - sabase) < sacur - 1 - sabase
     || buffer_put(buffer_1, "\n", 1) < 1)
      strerr_diefu1sys(111, "write to stdout") ;

    while (state < 8)
    {
      uint16_t what ;
      char c = 0 ;
      if (buffer_get(&b, &c, 1) < 0)
        strerr_diefu2sys(111, "read from ", sa.s + sabase) ;
      what = table[state][cclass((unsigned char)c)] ;
      state = what & 0x000f ;
      if (what & 0x0010)
        if (buffer_put(buffer_1, &c, 1) < 1)
          strerr_diefu1sys(111, "write to stdout") ;
      if (what & 0x0020)
        if (!stralloc_catb(&localsa, &c, 1)) dienomem() ;
      if (what & 0x0040)
      {
        if (!stralloc_0(&localsa)) dienomem() ;
        cmd = idcmd(localsa.s) ;
        if (cmd < 0)
        {
          char fmt[UINT64_FMT] ;
          fmt[uint64_fmt(fmt, line)] = 0 ;
          strerr_dief6x(2, "in ", sa.s + sabase, " line ", fmt, ": unrecognized directive: ", localsa.s) ;
        }
        localsa.len = 0 ;
      }
      if (what & 0x0080)
      {
        int fdhere = open2(".", O_RDONLY | O_DIRECTORY) ;
        if (fdhere < 0)
          strerr_dief3sys(111, "in ", sa.s + sabase, ": unable to open base directory: ") ;
        if (!stralloc_0(&localsa)) dienomem() ;
        if (cmd & 2)
        {
          if (chdir(localsa.s) < 0)
          {
            char fmt[UINT64_FMT] ;
            fmt[uint64_fmt(fmt, line)] = 0 ;
            strerr_dief6sys(111, "in ", sa.s + sabase, " line ", fmt, ": unable to chdir to ", localsa.s) ;
          }
          includecwd(cmd & 1) ;
        }
        else if (!include(localsa.s, cmd & 1))
        {
          char fmt[UINT64_FMT] ;
          fmt[uint64_fmt(fmt, line)] = 0 ;
          strerr_dief6sys(111, "in ", sa.s + sabase, " line ", fmt, ": unable to include ", localsa.s) ;
        }
        if (fchdir(fdhere) < 0)
          strerr_dief3sys(111, "in ", sa.s + sabase, ": unable to fchdir back") ;
        fd_close(fdhere) ;
        localsa.len = 0 ;
        {
          char fmt[UINT64_FMT] ;
          size_t fmtlen = uint64_fmt(fmt, line) ;
          fmt[fmtlen++] = ' ' ;
          if (buffer_put(buffer_1, "! ", 2) < 2
           || buffer_put(buffer_1, fmt, fmtlen) < fmtlen
           || buffer_put(buffer_1, sa.s + sabase, sacur - 1 - sabase) < sacur - 1 - sabase
           || buffer_put(buffer_1, "\n", 1) < 1)
            strerr_diefu1sys(111, "write to stdout") ;
        }
      }
      if (c == '\n' && state <= 8) line++ ;
    }
    if (state > 8)
    {
      char fmt[UINT64_FMT] ;
      fmt[uint64_fmt(fmt, line)] = 0 ;
      strerr_dief5x(2, "in ", sa.s + sabase, " line ", fmt, ": syntax error: invalid ! line") ;
    }
    fd_close(fd) ;
    stralloc_free(&localsa) ;
  }

 depth-- ;
 end:
  sa.len = sabase ;
  return 1 ;
}

int main (int argc, char const *const *argv, char const *const *envp)
{
  PROG = "s6-frontend-config-preprocess" ;
  {
    subgetopt l = SUBGETOPT_ZERO ;
    for (;;)
    {
      int opt = subgetopt_r(argc, argv, "", &l) ;
      if (opt == -1) break ;
      switch (opt)
      {
        default : dieusage() ;
      }
    }
    argc -= l.ind ; argv += l.ind ;
  }
  if (!argc) dieusage() ;

  if (!include(argv[0], 1)) strerr_diefu2sys(1, "preprocess ", argv[0]) ;
  if (!buffer_flush(buffer_1))
    strerr_diefu1sys(111, "write to stdout") ;
  return 0 ;
}
