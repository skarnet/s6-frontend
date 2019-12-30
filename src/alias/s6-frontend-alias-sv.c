 /* ISC license. */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <skalibs/uint32.h>
#include <skalibs/sgetopt.h>
#include <skalibs/strerr2.h>
#include <skalibs/djbunix.h>

#include <s6/config.h>
#include <s6/s6-supervise.h>

#define USAGE "s6-frontend-alias-sv [ -v ] [ -w sec ] command services..."
#define dieusage() strerr_dieusage(100, USAGE)
#define dienomem() strerr_diefu1sys(111, "stralloc_catb")

typedef int execfunc_t (char const *, char const *const *) ;
typedef execfunc_t *execfunc_t_ref ;

typedef struct info_s info_t, *info_t_ref ;
struct info_s
{
  char const *name ;
  execfunc_t_ref f ;
} ;

static int dowait = 0 ;
static uint32_t secs = 7 ;

static void warnnolog (void)
{
  strerr_warnw1x("s6-svc only sends commands to a single service, even if it has a dedicated logger") ;
}

static void warnnokill (void)
{
  strerr_warnw1x("s6-supervise pilots a kill signal via the timeout-kill file in the service directory") ;
}

static int info_cmp (void const *a, void const *b)
{
  char const *name = a ;
  info_t const *info = b ;
  return strcmp(name, info->name) ;
}

static int spawnit (char const *const *argv, char const *const *envp)
{
  int wstat ;
  pid_t r ;
  pid_t pid = child_spawn0(argv[0], argv, envp) ;
  if (!pid)
  {
    strerr_warnwu2sys("spawn ", argv[0]) ;
    return 1 ;
  }
  r = wait_pid(pid, &wstat) ;
  if (r != pid)
  {
    strerr_warnwu2sys("wait for ", argv[0]) ;
    return 1 ;
  }
  return !!WIFSIGNALED(wstat) || !!WEXITSTATUS(wstat) ;
}

static int simple_svc (char const *dir, char const *options, char const *const *envp)
{
  char const *argv[5] = { S6_EXTBINPREFIX "s6-svc", options, "--", dir, 0 } ;
  return spawnit(argv, envp) ;
}

static int complex_svc (char const *dir, char const *order, char waitfor, char const *const *envp)
{
  char warg[4] = "-w?" ;
  char fmt[2 + UINT32_FMT] = "-T" ;
  char const *argv[7] = { S6_EXTBINPREFIX "s6-svc", warg, fmt, order, "--", dir, 0 } ;
  fmt[2 + uint32_fmt(fmt + 2, 1000 * secs)] = 0 ;
  warg[2] = waitfor ;
  return spawnit(argv, envp) ;
}

static int status (char const *dir, char const *const *envp)
{
  char const *argv[4] = { S6_EXTBINPREFIX "s6-svstat", "--", dir, 0 } ;
  return spawnit(argv, envp) ;
}

static int usr1_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-1", envp) ;
}

static int usr2_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-2", envp) ;
}

static int alarm_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-a", envp) ;
}

static int cont_h (char const *dir, char const *const *envp)
{
  if (dowait)
  {
    complex_svc(dir, "-o", 'U', envp) ;
    return status(dir, envp) ;
  }
  else return simple_svc(dir, "-c", envp) ;
}

static int down (char const *dir, char const *const *envp)
{
  if (dowait)
  {
    complex_svc(dir, "-d", 'D', envp) ;
    return status(dir, envp) ;
  }
  else return simple_svc(dir, "-d", envp) ;
}

static int bail (char const *dir, char const *const *envp)
{
  warnnolog() ;
  if (dowait)
  {
    complex_svc(dir, "-d", 'D', envp) ;
    status(dir, envp) ;
  }
  return simple_svc(dir, "-xd", envp) ;
}

static int hup_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-h", envp) ;
}

static int int_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-i", envp) ;
}

static int kill_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-k", envp) ;
}

static int once (char const *dir, char const *const *envp)
{
  if (dowait)
  {
    complex_svc(dir, "-o", 'U', envp) ;
    return status(dir, envp) ;
  }
  else return simple_svc(dir, "-o", envp) ;
}

static int pause_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-p", envp) ;
}

static int quit_h (char const *dir, char const *const *envp)
{
  return simple_svc(dir, "-q", envp) ;
}

static int term_h (char const *dir, char const *const *envp)
{
  if (dowait)
  {
    complex_svc(dir, "-r", 'R', envp) ;
    return status(dir, envp) ;
  }
  else return simple_svc(dir, "-t", envp) ;
}

static int up (char const *dir, char const *const *envp)
{
  if (dowait)
  {
    complex_svc(dir, "-u", 'U', envp) ;
    return status(dir, envp) ;
  }
  else return simple_svc(dir, "-u", envp) ;
}

static int check (char const *dir, char const *const *envp)
{
  s6_svstatus_t svst ;
  char warg[3] = "-?" ;
  char fmt[2 + UINT32_FMT] = "-t" ;
  char const *argv[6] = { S6_EXTBINPREFIX "s6-svwait", warg, fmt, "--", dir, 0 } ;
  fmt[2 + uint32_fmt(fmt + 2, 1000 * secs)] = 0 ;
  if (!s6_svstatus_read(dir, &svst)) return 1 ;
  warg[1] = svst.flagwantup ? 'U' : 'D' ;
  spawnit(argv, envp) ;
  return status(dir, envp) ;
}

static int lsb_reload (char const *dir, char const *const *envp)
{
  hup_h(dir, envp) ;
  return status(dir, envp) ;
}

static int lsb_restart (char const *dir, char const *const *envp)
{
  complex_svc(dir, "-ru", 'U', envp) ;
  return status(dir, envp) ;
}

static int lsb_start (char const *dir, char const *const *envp)
{
  complex_svc(dir, "-u", 'U', envp) ;
  return status(dir, envp) ;
}

static int lsb_stop (char const *dir, char const *const *envp)
{
  complex_svc(dir, "-d", 'D', envp) ;
  return status(dir, envp) ;
}

static int lsb_shutdown (char const *dir, char const *const *envp)
{
  warnnolog() ;
  complex_svc(dir, "-d", 'D', envp) ;
  status(dir, envp) ;
  return simple_svc(dir, "-x", envp) ;
}

static int lsb_forcereload (char const *dir, char const *const *envp)
{
  warnnokill() ;
  return lsb_reload(dir, envp) ;
}

static int lsb_forcerestart (char const *dir, char const *const *envp)
{
  warnnokill() ;
  return lsb_restart(dir, envp) ;
}

static int lsb_forcestop (char const *dir, char const *const *envp)
{
  warnnokill() ;
  return lsb_stop(dir, envp) ;
}

static int lsb_forceshutdown (char const *dir, char const *const *envp)
{
  warnnokill() ;
  return lsb_shutdown(dir, envp) ;
}

static int lsb_tryrestart (char const *dir, char const *const *envp)
{
  s6_svstatus_t svst ;
  if (s6_svstatus_read(dir, &svst) && svst.flagwantup && svst.pid && !svst.flagfinishing)
    complex_svc(dir, "-r", 'U', envp) ;    
  return status(dir, envp) ;
}

static info_t const commands[] =
{
  { .name = "1", .f = &usr1_h },
  { .name = "2", .f = &usr2_h },
  { .name = "a", .f = &alarm_h },
  { .name = "alarm", .f = &alarm_h },
  { .name = "c", .f = &cont_h },
  { .name = "check", .f = &check },
  { .name = "cont", .f = &cont_h },
  { .name = "d", .f = &down },
  { .name = "down", .f = &down },
  { .name = "e", .f = &bail },
  { .name = "exit", .f = &bail },
  { .name = "force-reload", .f = &lsb_forcereload },
  { .name = "force-restart", .f = &lsb_forcerestart },
  { .name = "force-shutdown", .f = &lsb_forceshutdown },
  { .name = "force-stop", .f = &lsb_forcestop },
  { .name = "h", .f = &hup_h },
  { .name = "hup", .f = &hup_h },
  { .name = "i", .f = &int_h },
  { .name = "interrupt", .f = &int_h },
  { .name = "k", .f = &kill_h },
  { .name = "kill", .f = &kill_h },
  { .name = "o", .f = &once },
  { .name = "once", .f = &once },
  { .name = "p", .f = &pause_h },
  { .name = "pause", .f = &pause_h },
  { .name = "q", .f = &quit_h },
  { .name = "quit", .f = &quit_h },
  { .name = "reload", .f = &lsb_reload },
  { .name = "restart", .f = &lsb_restart },
  { .name = "s", .f = &status },
  { .name = "shutdown", .f = &lsb_shutdown },
  { .name = "start", .f = &lsb_start },
  { .name = "status", .f = &status },
  { .name = "stop", .f = &lsb_stop },
  { .name = "t", .f = &term_h },
  { .name = "term", .f = &term_h },
  { .name = "try-restart", .f = &lsb_tryrestart },
  { .name = "u", .f = &up },
  { .name = "up", .f = &up }
} ;

int main (int argc, char const *const *argv, char const *const *envp)
{
  int e = 0 ;
  info_t *p ;
  char const *x = getenv("SVWAIT") ;
  char const *scandir = getenv("SVDIR") ;
  size_t scandirlen ;
  PROG = "s6-frontend-alias-sv" ;
  if (!scandir) scandir = "/run/service" ; /* TODO: infer from s6li config */
  scandirlen = strlen(scandir) ;
  if (x)
  {
    if (!uint320_scan(x, &secs))
      strerr_warnw1x("invalid SVWAIT value") ;
  }

  {
    subgetopt_t l = SUBGETOPT_ZERO ;
    for (;;)
    {
      int opt = subgetopt_r(argc, argv, "vw:", &l) ;
      if (opt == -1) break ;
      switch (opt)
      {
        case 'v' : dowait = 1 ; break ;
        case 'w' : if (!uint320_scan(l.arg, &secs)) dieusage() ; break ;
        default : dieusage() ;
      }
    }
    argc -= l.ind ; argv += l.ind ;
  }

  if (argc < 2) dieusage() ;
  p = bsearch(argv[0], commands, sizeof(commands) / sizeof(info_t), sizeof(info_t), &info_cmp) ;
  if (!p) strerr_dief2x(100, "unknown command: ", argv[0]) ;

  for (argv++ ; *argv ; argv++)
  {
    if ((argv[0][0] == '.' && (argv[0][1] == '/' || (argv[0][1] == '.' && argv[0][2] == '/'))) || argv[0][0] == '/')
      e += (*p->f)(*argv, envp) ;
    else
    {
      int what = 1 ;
      struct stat st ;
      size_t len = strlen(*argv) ;
      char fn[scandirlen + len + 2] ;
      memcpy(fn, scandir, scandirlen) ;
      fn[scandirlen] = '/' ;
      memcpy(fn + scandirlen + 1, *argv, len + 1) ;
      if (stat(fn, &st) < 0)  /* XXX: TOCTOU but we don't care */
      {
        if (errno != ENOENT)
        {
          e++ ;
          what = 0 ;
          strerr_warnwu2sys("stat ", fn) ;
        }
      }
      else if (S_ISDIR(st.st_mode)) what = 2 ;
      if (what) e += (*p->f)(what > 1 ? fn : *argv, envp) ;
    }
  }
  return e ;
}