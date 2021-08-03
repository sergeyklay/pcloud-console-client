/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 Anton Titov <anton@pcloud.com>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include <string.h>

#include "config.h"

#include <pthread.h>

#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include "pcloudcc/psync/compat.h"
#include "pcloudcc/psync/stringcompat.h"
#include "pcloudcc/psync/deviceid.h"

#ifdef P_OS_LINUX
#include <sys/sysinfo.h>
#endif

#ifdef P_OS_MACOSX
#include <sys/attr.h>
#endif

#ifdef P_OS_POSIX
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <utime.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>

extern char **environ;

#ifdef MAP_ANONYMOUS
# define PSYNC_MAP_ANONYMOUS MAP_ANONYMOUS
# elif defined(MAP_ANON)
# define PSYNC_MAP_ANONYMOUS MAP_ANON
#endif  /* MAP_ANONYMOUS */
#endif  /* P_OS_POSIX */

#include "psynclib.h"
#include "plibs.h"
#include "psettings.h"
#include "pssl.h"
#include "ptimer.h"
#include "logger.h"

#define PROXY_NONE    0
#define PROXY_CONNECT 1

typedef struct {
  psync_thread_start0 run;
  const char *name;
} psync_run_data0;

typedef struct {
  psync_thread_start1 run;
  void *ptr;
  const char *name;
} psync_run_data1;

#if defined(P_OS_POSIX)
static uid_t psync_uid;
static gid_t psync_gid;
static gid_t *psync_gids;
static int psync_gids_cnt;
#endif

static int proxy_type=PROXY_NONE;
static int proxy_detected=0;
static char proxy_host[256];
static char proxy_port[8];

static int psync_page_size;

__thread const char *psync_thread_name="no name";
static pthread_mutex_t socket_mutex=PTHREAD_MUTEX_INITIALIZER;

const unsigned char psync_invalid_filename_chars[256]={
#if defined(P_OS_LINUX)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#elif defined(P_OS_MACOSX)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
#else
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
#endif
};

int psync_user_is_admin() {
#if defined(P_OS_MACOSX)
  struct group *ag;
  int i;
  if (psync_uid==0)
    return 1;
  else if (psync_gids_cnt==0)
    return 0;
  ag=getgrnam("admin");
  if (!ag)
    return 0;
  for (i=0; i<psync_gids_cnt; i++)
    if (ag->gr_gid==psync_gids[i])
      return 1;
  return 0;
#else
  return 0;
#endif
}

void psync_compat_init() {
#if defined(P_OS_POSIX)
  struct rlimit limit;
  limit.rlim_cur=limit.rlim_max=2048;
  if (setrlimit(RLIMIT_NOFILE, &limit))
    log_error("setrlimit failed errno=%d", errno);
#if IS_DEBUG
  if (getrlimit(RLIMIT_CORE, &limit))
    log_error("getrlimit failed errno=%d", errno);
  else {
    limit.rlim_cur=limit.rlim_max;
    if (setrlimit(RLIMIT_CORE, &limit))
      log_error("setrlimit failed errno=%d", errno);
  }
#endif /* IS_DEBUG */
  signal(SIGPIPE, SIG_IGN);
  psync_uid=getuid();
  psync_gid=getgid();
  psync_gids_cnt=getgroups(0, NULL);
  psync_gids=psync_new_cnt(gid_t, psync_gids_cnt);
  if (unlikely_log(getgroups(psync_gids_cnt, psync_gids)!=psync_gids_cnt))
    psync_gids_cnt=0;
  psync_page_size=sysconf(_SC_PAGESIZE);
#endif
  log_info("detected page size %d", psync_page_size);
}

int psync_stat_mode_ok(psync_stat_t *buf, unsigned int bits) {
#if defined(P_OS_POSIX)
  int i;
  if (psync_uid==0)
    return 1;
  if (buf->st_uid==psync_uid) {
    bits<<=6;
    return (buf->st_mode&bits)==bits;
  }
  if (buf->st_gid==psync_gid) {
    bits<<=3;
    return (buf->st_mode&bits)==bits;
  }
  for (i=0; i<psync_gids_cnt; i++)
    if (buf->st_gid==psync_gids[i]) {
      bits<<=3;
      return (buf->st_mode&bits)==bits;
    }
  return (buf->st_mode&bits)==bits;
#else
  return 1;
#endif
}

char *psync_get_default_database_path_old() {
#if defined(P_OS_POSIX)
  struct stat st;
  const char *dir;
  dir=getenv("HOME");
  if (unlikely_log(!dir) || unlikely_log(stat(dir, &st)) || unlikely_log(!psync_stat_mode_ok(&st, 7))) {
    struct passwd pwd;
    struct passwd *result;
    char buff[4096];
    if (unlikely_log(getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &result)) || unlikely_log(stat(result->pw_dir, &st)) ||
        unlikely_log(!psync_stat_mode_ok(&st, 7)))
      return NULL;
    dir=result->pw_dir;
  }
  return psync_strcat(dir, PSYNC_DIRECTORY_SEPARATOR, PSYNC_DEFAULT_POSIX_DBNAME, NULL);
#else
#error "Function not implemented for your operating system"
#endif
}

static char *psync_get_pcloud_path_nc() {
#if defined(P_OS_POSIX)
  struct stat st;
  const char *dir;
  dir=getenv("HOME");
  if (unlikely_log(!dir) || unlikely_log(stat(dir, &st)) || unlikely_log(!psync_stat_mode_ok(&st, 7))) {
    struct passwd pwd;
    struct passwd *result;
    char buff[4096];
    if (unlikely_log(getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &result)) || unlikely_log(stat(result->pw_dir, &st)) ||
        unlikely_log(!psync_stat_mode_ok(&st, 7)))
      return NULL;
    dir=result->pw_dir;
  }
  return psync_strcat(dir, PSYNC_DIRECTORY_SEPARATOR, PSYNC_DEFAULT_POSIX_DIR, NULL);
#else
#error "Function not implemented for your operating system"
#endif
}

char *psync_get_pcloud_path() {
  char *path;
  psync_stat_t st;
  path=psync_get_pcloud_path_nc();
  if (unlikely_log(!path))
    return NULL;
  if (psync_stat(path, &st) && unlikely_log(psync_mkdir(path))) {
    psync_free(path);
    return NULL;
  }
  return path;
}

char *psync_get_private_dir(char *name) {
  char *path, *rpath;
  psync_stat_t st;
  path=psync_get_pcloud_path();
  if (!path)
    return NULL;
  rpath=psync_strcat(path, PSYNC_DIRECTORY_SEPARATOR, name, NULL);
  free(path);
  if (psync_stat(rpath, &st) && psync_mkdir(rpath)) {
    psync_free(rpath);
    return NULL;
  }
  return rpath;
}

char *psync_get_private_tmp_dir() {
  return psync_get_private_dir(PSYNC_DEFAULT_TMP_DIR);
}

char *psync_get_default_database_path() {
  char *dirpath, *path;
  psync_stat_t st;
  dirpath=psync_get_pcloud_path();
  if (!dirpath)
    return NULL;
  path=psync_strcat(dirpath, PSYNC_DIRECTORY_SEPARATOR, PSYNC_DEFAULT_DB_NAME, NULL);
  psync_free(dirpath);
  if (psync_stat(path, &st) && (dirpath=psync_get_default_database_path_old())) {
    if (!psync_stat(dirpath, &st)) {
      if (psync_sql_reopen(dirpath)) {
        psync_free(path);
        return dirpath;
      }

      if (unlikely(rename(dirpath, path) != 0)) {
        log_error("failed to rename %s to %s:", dirpath, path,
                  strerror(errno));
      }
    }
    psync_free(dirpath);
  }
  return path;
}

char *psync_get_home_dir() {
#if defined(P_OS_POSIX)
  struct stat st;
  const char *dir;
  dir=getenv("HOME");
  if (unlikely_log(!dir) || unlikely_log(stat(dir, &st)) || unlikely_log(!psync_stat_mode_ok(&st, 7))) {
    struct passwd pwd;
    struct passwd *result;
    char buff[4096];
    if (unlikely_log(getpwuid_r(getuid(), &pwd, buff, sizeof(buff), &result)) || unlikely_log(stat(result->pw_dir, &st)) ||
        unlikely_log(!psync_stat_mode_ok(&st, 7)))
      return NULL;
    dir=result->pw_dir;
  }
  return psync_strdup(dir);
#else
#error "Function not implemented for your operating system"
#endif
}

void psync_yield_cpu() {
#if defined(_POSIX_PRIORITY_SCHEDULING)
  sched_yield();
#else
  psync_milisleep(1);
#endif
}

static void thread_started() {
  //log_info("thread started"); //This repeats too many times because of the overlays
}

static void thread_exited() {
 // log_info("thread exited"); //This repeats too many times because of the overlays
}

static void *thread_entry0(void *data) {
  psync_thread_start0 run;
  run=((psync_run_data0 *)data)->run;
  psync_thread_name=((psync_run_data0 *)data)->name;
  psync_free(data);
  thread_started();
  run();
  thread_exited();
  return NULL;
}

void psync_run_thread(const char *name, psync_thread_start0 run) {
  psync_run_data0 *data;
  pthread_t thread;
  pthread_attr_t attr;
  data=psync_new(psync_run_data0);
  data->run=run;
  data->name=name;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize(&attr, PSYNC_STACK_SIZE);
  pthread_create(&thread, &attr, thread_entry0, data);
  pthread_attr_destroy(&attr);
}

static void *thread_entry1(void *data) {
  psync_thread_start1 run;
  void *ptr;
  run=((psync_run_data1 *)data)->run;
  ptr=((psync_run_data1 *)data)->ptr;
  psync_thread_name=((psync_run_data1 *)data)->name;
  psync_free(data);
  thread_started();
  run(ptr);
  thread_exited();
  return NULL;
}

void psync_run_thread1(const char *name, psync_thread_start1 run, void *ptr) {
  psync_run_data1 *data;
  pthread_t thread;
  pthread_attr_t attr;
  data=psync_new(psync_run_data1);
  data->run=run;
  data->ptr=ptr;
  data->name=name;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_attr_setstacksize(&attr, PSYNC_STACK_SIZE);
  pthread_create(&thread, &attr, thread_entry1, data);
  pthread_attr_destroy(&attr);
}

static void psync_check_no_sql_lock(uint64_t millisec) {
#if IS_DEBUG
  if (psync_sql_islocked()) {
    log_fatal("trying to sleep while holding sql lock, aborting");
    psync_sql_dump_locks();
    abort();
  }
#endif
}

void psync_milisleep_nosqlcheck(uint64_t millisec) {
#if defined(P_OS_POSIX)
  struct timespec tm;
  tm.tv_sec=millisec/1000;
  tm.tv_nsec=(millisec%1000)*1000000;
  nanosleep(&tm, NULL);
#else
#error "Function not implemented for your operating system"
#endif
}

void psync_milisleep(uint64_t millisec) {
  psync_check_no_sql_lock(millisec);
  psync_milisleep_nosqlcheck(millisec);
}

time_t psync_time() {
#if defined(P_OS_MACOSX)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec;
#elif defined(_POSIX_TIMERS) && _POSIX_TIMERS>0
  struct timespec ts;
  if (likely_log(clock_gettime(CLOCK_REALTIME, &ts)==0))
    return ts.tv_sec;
  else
    return time(NULL);
#else
  return time(NULL);
#endif
}

void psync_nanotime(struct timespec *tm) {
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS>0
  clock_gettime(CLOCK_REALTIME, tm);
#elif defined(P_OS_POSIX)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  tm->tv_sec=tv.tv_sec;
  tm->tv_nsec=tv.tv_usec*1000;
#else
#error "Function not implemented for your operating system"
#endif
}

uint64_t psync_millitime() {
  struct timespec tm;
  psync_nanotime(&tm);
  return tm.tv_sec*1000+tm.tv_nsec/1000000;
}

#if defined(P_OS_POSIX)
static void psync_add_file_to_seed(const char *fn, psync_lhash_ctx *hctx, size_t max) {
  char buff[4096];
  ssize_t rd;
  int fd, mode;
  mode=O_RDONLY;
#if defined(O_NONBLOCK)
  mode+=O_NONBLOCK;
#elif defined(O_NDELAY)
  mode+=O_NDELAY;
#endif
  fd=open(fn, mode);
  if (fd!=-1) {
    if (!max || max>sizeof(buff))
      max=sizeof(buff);
    rd=read(fd, buff, max);
    if (rd>0)
      psync_lhash_update(hctx, buff, rd);
    close(fd);
  }
}
#endif

#if defined(P_OS_LINUX)
static void psync_get_random_seed_linux(psync_lhash_ctx *hctx) {
  struct sysinfo si;
  if (likely_log(!sysinfo(&si)))
    psync_lhash_update(hctx, &si, sizeof(si));
  psync_add_file_to_seed("/proc/stat", hctx, 0);
  psync_add_file_to_seed("/proc/vmstat", hctx, 0);
  psync_add_file_to_seed("/proc/meminfo", hctx, 0);
  psync_add_file_to_seed("/proc/modules", hctx, 0);
  psync_add_file_to_seed("/proc/mounts", hctx, 0);
  psync_add_file_to_seed("/proc/diskstats", hctx, 0);
  psync_add_file_to_seed("/proc/interrupts", hctx, 0);
  psync_add_file_to_seed("/proc/net/dev", hctx, 0);
  psync_add_file_to_seed("/proc/net/arp", hctx, 0);
}
#endif

static void psync_get_random_seed_from_query(psync_lhash_ctx *hctx, psync_sql_res *res) {
  psync_variant_row row;
  struct timespec tm;
  int i;
  while ((row=psync_sql_fetch_row(res))) {
    for (i=0; i<res->column_count; i++)
      if (row[i].type==PSYNC_TSTRING)
        psync_lhash_update(hctx, row[i].str, row[i].length);
    psync_lhash_update(hctx, row, sizeof(psync_variant)*res->column_count);
  }
  psync_sql_free_result(res);
  psync_nanotime(&tm);
  psync_lhash_update(hctx, &tm, sizeof(&tm));
}

static void psync_get_random_seed_from_db(psync_lhash_ctx *hctx) {
  psync_sql_res *res;
  struct timespec tm;
  unsigned char rnd[PSYNC_LHASH_DIGEST_LEN];
  psync_nanotime(&tm);
  psync_lhash_update(hctx, &tm, sizeof(&tm));
  res=psync_sql_query_rdlock("SELECT * FROM setting ORDER BY RANDOM()");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM resolver ORDER BY RANDOM() LIMIT 50");
  psync_get_random_seed_from_query(hctx, res);
/*  res=psync_sql_query_rdlock("SELECT * FROM filerevision ORDER BY RANDOM() LIMIT 50");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM file ORDER BY RANDOM() LIMIT 50");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM localfile ORDER BY RANDOM() LIMIT 50");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM folder ORDER BY RANDOM() LIMIT 25");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM localfolder ORDER BY RANDOM() LIMIT 25");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM hashchecksum ORDER BY RANDOM() LIMIT 25");
  psync_get_random_seed_from_query(hctx, res);
  res=psync_sql_query_rdlock("SELECT * FROM pagecache WHERE type=1 AND rowid>(ABS(RANDOM())%(SELECT MAX(rowid)+1 FROM pagecache)) ORDER BY rowid LIMIT 50");
  psync_get_random_seed_from_query(hctx, res); */
  psync_sql_statement("REPLACE INTO setting (id, value) VALUES ('random', RANDOM())");
  psync_nanotime(&tm);
  psync_lhash_update(hctx, &tm, sizeof(&tm));
  psync_sql_sync();
  psync_nanotime(&tm);
  psync_lhash_update(hctx, &tm, sizeof(&tm));
  sqlite3_randomness(sizeof(rnd), rnd);
  psync_lhash_update(hctx, rnd, sizeof(rnd));
}

static void psync_rehash_cnt(unsigned char *hashbin, psync_uint_t cnt) {
  psync_lhash_ctx hctx;
  psync_uint_t i;
  struct timespec tm;
  for (i=0; i<cnt; i++) {
    psync_lhash_init(&hctx);
    if ((i&511)==0) {
      psync_nanotime(&tm);
      psync_lhash_update(&hctx, &tm, sizeof(&tm));
    }
    else
      psync_lhash_update(&hctx, &i, sizeof(i));
    psync_lhash_update(&hctx, hashbin, PSYNC_LHASH_DIGEST_LEN);
    psync_lhash_final(hashbin, &hctx);
  }
}

static void psync_store_seed_in_db(const unsigned char *seed) {
  psync_sql_res *res;
  unsigned char hashbin[PSYNC_LHASH_DIGEST_LEN];
  char hashhex[PSYNC_LHASH_DIGEST_HEXLEN], nm[16];
  memcpy(hashbin, seed, PSYNC_LHASH_DIGEST_LEN);
  psync_rehash_cnt(hashbin, 2000);
  psync_binhex(hashhex, hashbin, PSYNC_LHASH_DIGEST_LEN);
  res=psync_sql_prep_statement("REPLACE INTO setting (id, value) VALUES ('randomhash', ?)");
  psync_sql_bind_lstring(res, 1, hashhex, PSYNC_LHASH_DIGEST_HEXLEN);
  psync_sql_run_free(res);
  psync_rehash_cnt(hashbin, 2000);
  psync_binhex(hashhex, hashbin, PSYNC_LHASH_DIGEST_LEN);
  memcpy(nm, "randomhash", 10);
  nm[10]=hashhex[0];
  nm[11]=0;
  res=psync_sql_prep_statement("REPLACE INTO setting (id, value) VALUES (?, ?)");
  psync_sql_bind_lstring(res, 1, nm, 11);
  psync_sql_bind_lstring(res, 2, hashhex, PSYNC_LHASH_DIGEST_HEXLEN);
  psync_sql_run_free(res);
}

void psync_get_random_seed(unsigned char *seed, const void *addent, size_t aelen, int fast) {
  static unsigned char lastseed[PSYNC_LHASH_DIGEST_LEN];
  psync_lhash_ctx hctx;
  struct timespec tm;
  psync_stat_t st;
  char *home;
  void *ptr;
  psync_uint_t i, j;
  int64_t i64;
  pthread_t threadid;
  unsigned char lsc[64][PSYNC_LHASH_DIGEST_LEN];
#if defined(P_OS_POSIX)
  struct utsname un;
  struct statvfs stfs;
  char **env;
  pid_t pid;
  psync_nanotime(&tm);
  psync_lhash_init(&hctx);
  psync_lhash_update(&hctx, &tm, sizeof(tm));
  if (likely_log(!uname(&un)))
    psync_lhash_update(&hctx, &un, sizeof(un));
  pid=getpid();
  psync_lhash_update(&hctx, &pid, sizeof(pid));
  if (!statvfs("/", &stfs))
    psync_lhash_update(&hctx, &stfs, sizeof(stfs));
  for (env=environ; *env!=NULL; env++)
    psync_lhash_update(&hctx, *env, strlen(*env));
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS>0 && defined(_POSIX_MONOTONIC_CLOCK)
  if (likely_log(!clock_gettime(CLOCK_MONOTONIC, &tm)))
    psync_lhash_update(&hctx, &tm, sizeof(tm));
#endif
#if defined(P_OS_LINUX)
  psync_add_file_to_seed("/dev/urandom", &hctx, PSYNC_HASH_DIGEST_LEN);
#else
  psync_add_file_to_seed("/dev/random", &hctx, PSYNC_HASH_DIGEST_LEN);
#endif
#endif
#if defined(P_OS_LINUX)
  psync_get_random_seed_linux(&hctx);
#endif
  threadid=pthread_self();
  psync_lhash_update(&hctx, &threadid, sizeof(threadid));
  ptr=(void *)&ptr;
  psync_lhash_update(&hctx, &ptr, sizeof(ptr));
  ptr=(void *)psync_get_random_seed;
  psync_lhash_update(&hctx, &ptr, sizeof(ptr));
  ptr=(void *)pthread_self;
  psync_lhash_update(&hctx, &ptr, sizeof(ptr));
  ptr=(void *)malloc;
  psync_lhash_update(&hctx, &ptr, sizeof(ptr));
  ptr=(void *)&lastseed;
  psync_lhash_update(&hctx, &ptr, sizeof(ptr));
  home=psync_get_home_dir();
  if (home) {
    i64=psync_get_free_space_by_path(home);
    psync_lhash_update(&hctx, &i64, sizeof(i64));
    psync_lhash_update(&hctx, home, strlen(home));
    if (likely_log(!psync_stat(home, &st)))
      psync_lhash_update(&hctx, &st, sizeof(st));
    psync_free(home);
  }
  if (!fast) {
    log_info("getting a random seed from the database");
    psync_get_random_seed_from_db(&hctx);
  }
  if (aelen)
    psync_lhash_update(&hctx, addent, aelen);
  log_debug("adding bulk data");
  for (i=0; i<ARRAY_SIZE(lsc); i++) {
    memcpy(&lsc[i], lastseed, PSYNC_LHASH_DIGEST_LEN);
    for (j=0; j<PSYNC_LHASH_DIGEST_LEN; j++)
      lsc[i][j]^=(unsigned char)i;
  }
  for (j=fast?3:0; j<5; j++) {
    for (i=0; i<100; i++) {
      psync_lhash_update(&hctx, &i, sizeof(i));
      psync_lhash_update(&hctx, &j, sizeof(j));
      psync_lhash_update(&hctx, lsc, sizeof(lsc));
    }
    psync_nanotime(&tm);
    psync_lhash_update(&hctx, &tm, sizeof(&tm));
  }
  psync_lhash_final(seed, &hctx);
  memcpy(lastseed, seed, PSYNC_LHASH_DIGEST_LEN);
  log_info("storing a random see in the database");
  psync_store_seed_in_db(seed);
}

static int psync_wait_socket_writable_microsec(psync_socket_t sock, long sec, long usec) {
  fd_set wfds;
  struct timeval tv;
  int res;
  tv.tv_sec=sec;
  tv.tv_usec=usec;
  FD_ZERO(&wfds);
  FD_SET(sock, &wfds);
  res=select(sock+1, NULL, &wfds, NULL, &tv);
  if (res==1)
    return 0;
  if (res==0)
    psync_sock_set_err(P_TIMEDOUT);
  return SOCKET_ERROR;
}

#define psync_wait_socket_writable(sock, sec) psync_wait_socket_writable_microsec(sock, sec, 0)

int psync_wait_socket_write_timeout(psync_socket_t sock) {
  return psync_wait_socket_writable(sock, PSYNC_SOCK_WRITE_TIMEOUT);
}

static int psync_wait_socket_readable_microsec(psync_socket_t sock, long sec, long usec) {
  fd_set rfds;
  struct timeval tv;
#if IS_DEBUG
  struct timespec start, end;
  unsigned long msec;
#endif
  int res;
  tv.tv_sec=sec;
  tv.tv_usec=usec;
  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);
#if IS_DEBUG
  psync_nanotime(&start);
#endif
  res=select(sock+1, &rfds, NULL, NULL, &tv);
  if (res==1) {
#if IS_DEBUG
    psync_nanotime(&end);
    msec=(end.tv_sec-start.tv_sec)*1000+end.tv_nsec/1000000-start.tv_nsec/1000000;
    if (msec>=30000)
      log_warn("got response from socket after %lu milliseconds", msec);
    else if (msec>=5000)
      log_info("got response from socket after %lu milliseconds", msec);
#endif
    return 0;
  }
  if (res==0) {
    if (sec)
      log_warn("socket read timeouted on %ld seconds", sec);
    psync_sock_set_err(P_TIMEDOUT);
  }
  else
    log_warn("select returned %d", res);
  return SOCKET_ERROR;
}

#define psync_wait_socket_readable(sock, sec) psync_wait_socket_readable_microsec(sock, sec, 0)

int psync_wait_socket_read_timeout(psync_socket_t sock) {
  return psync_wait_socket_readable(sock, PSYNC_SOCK_READ_TIMEOUT);
}

static psync_socket_t connect_res(struct addrinfo *res) {
  psync_socket_t sock;
#if defined(SOCK_NONBLOCK)
#if defined(SOCK_CLOEXEC)
#define PSOCK_TYPE_OR (SOCK_NONBLOCK|SOCK_CLOEXEC)
#else
#define PSOCK_TYPE_OR SOCK_NONBLOCK
#endif
#else
#define PSOCK_TYPE_OR 0
#define PSOCK_NEED_NOBLOCK
#endif
  while (res) {
    sock=socket(res->ai_family, res->ai_socktype|PSOCK_TYPE_OR, res->ai_protocol);
    if (likely_log(sock!=INVALID_SOCKET)) {
#if defined(PSOCK_NEED_NOBLOCK)
#if defined(P_OS_POSIX)
      fcntl(sock, F_SETFD, FD_CLOEXEC);
      fcntl(sock, F_SETFL, fcntl(sock, F_GETFL)|O_NONBLOCK);
#else
#error "Need to set non-blocking for your OS"
#endif
#endif
      if ((connect(sock, res->ai_addr, res->ai_addrlen)!=SOCKET_ERROR) ||
          (psync_sock_err()==P_INPROGRESS && !psync_wait_socket_writable(sock, PSYNC_SOCK_CONNECT_TIMEOUT)))
        return sock;
      psync_close_socket(sock);
    }
    res=res->ai_next;
  }
  return INVALID_SOCKET;
}

psync_socket_t psync_create_socket(int domain, int type, int protocol) {
  psync_socket_t ret;
  ret=socket(domain, type, protocol);
  return ret;
}

static void addr_save_to_db(const char *host, const char *port, struct addrinfo *addr) {
  psync_sql_res *res;
  uint64_t id;
  if (psync_sql_isrdlocked()) {
    if (psync_sql_tryupgradelock())
      return;
    else
      log_info("upgraded read to write lock to save data to DB");
  }
  psync_sql_start_transaction();
  res=psync_sql_prep_statement("DELETE FROM resolver WHERE hostname=? AND port=?");
  psync_sql_bind_string(res, 1, host);
  psync_sql_bind_string(res, 2, port);
  psync_sql_run_free(res);
  res=psync_sql_prep_statement("INSERT INTO resolver (hostname, port, prio, created, family, socktype, protocol, data) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
  psync_sql_bind_string(res, 1, host);
  psync_sql_bind_string(res, 2, port);
  psync_sql_bind_uint(res, 4, psync_timer_time());
  id=0;
  do {
    psync_sql_bind_uint(res, 3, id++);
    psync_sql_bind_int(res, 5, addr->ai_family);
    psync_sql_bind_int(res, 6, addr->ai_socktype);
    psync_sql_bind_int(res, 7, addr->ai_protocol);
    psync_sql_bind_blob(res, 8, (char *)addr->ai_addr, addr->ai_addrlen);
    psync_sql_run(res);
    addr=addr->ai_next;
  } while (addr);
  psync_sql_free_result(res);
  psync_sql_commit_transaction();
}

static struct addrinfo *addr_load_from_db(const char *host, const char *port) {
  psync_sql_res *res;
  psync_uint_row row;
  psync_variant_row vrow;
  struct addrinfo *ret;
  char *data;
  const char *str;
  uint64_t i;
  size_t len;
  psync_sql_rdlock();
  res=psync_sql_query_nolock("SELECT COUNT(*), SUM(LENGTH(data)) FROM resolver WHERE hostname=? AND port=?");
  psync_sql_bind_string(res, 1, host);
  psync_sql_bind_string(res, 2, port);
  if (!(row=psync_sql_fetch_rowint(res)) || row[0]==0) {
    psync_sql_free_result(res);
    psync_sql_rdunlock();
    return NULL;
  }
  ret=(struct addrinfo *)psync_malloc(sizeof(struct addrinfo)*row[0]+row[1]);
  data=(char *)(ret+row[0]);
  for (i=0; i<row[0]-1; i++)
    ret[i].ai_next=&ret[i+1];
  ret[i].ai_next=NULL;
  psync_sql_free_result(res);
  res=psync_sql_query_nolock("SELECT family, socktype, protocol, data FROM resolver WHERE hostname=? AND port=? ORDER BY prio");
  psync_sql_bind_string(res, 1, host);
  psync_sql_bind_string(res, 2, port);
  i=0;
  while ((vrow=psync_sql_fetch_row(res))) {
    ret[i].ai_family=psync_get_snumber(vrow[0]);
    ret[i].ai_socktype=psync_get_snumber(vrow[1]);
    ret[i].ai_protocol=psync_get_snumber(vrow[2]);
    str=psync_get_lstring(vrow[3], &len);
    ret[i].ai_addr=(struct sockaddr *)data;
    ret[i].ai_addrlen=len;
    i++;
    memcpy(data, str, len);
    data+=len;
  }
  psync_sql_free_result(res);
  psync_sql_rdunlock();
  return ret;
}

static int addr_still_valid(struct addrinfo *olda, struct addrinfo *newa) {
  struct addrinfo *a;
  do {
    a=newa;
    while (1) {
      if (a->ai_addrlen==olda->ai_addrlen && !memcmp(a->ai_addr, olda->ai_addr, a->ai_addrlen))
        break;
      a=a->ai_next;
      if (!a)
        return 0;
    }
    olda=olda->ai_next;
  } while (olda);
  return 1;
}

typedef struct {
  const char *host;
  const char *port;
} resolve_host_port;

static void connect_res_callback(void *h, void *ptr) {
  struct addrinfo *res;
  psync_socket_t sock;
  int r;
  res=(struct addrinfo *)ptr;
  sock=connect_res(res);
  r=psync_task_complete(h, (void *)(uintptr_t)sock);
  psync_free(res);
  if (r && sock!=INVALID_SOCKET)
    psync_close_socket(sock);
}

static void resolve_callback(void *h, void *ptr) {
  resolve_host_port *hp;
  struct addrinfo *res;
  struct addrinfo hints;
  int rc;
  hp=(resolve_host_port *)ptr;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  res=NULL;
  rc=getaddrinfo(hp->host, hp->port, &hints, &res);
  if (unlikely(rc!=0))
    res=NULL;
  psync_task_complete(h, res);
}

#if defined(P_OS_MACOSX) && 0

#define PSYNC_HAS_PROXY_CODE

const void *get_value_cstr(CFDictionaryRef dict, const char *key) {
  CFStringRef str;
  const void *ret;
  str=CFStringCreateWithCString(NULL, key, kCFStringEncodingUTF8);
  ret=CFDictionaryGetValue(dict, str);
  CFRelease(str);
  return ret;
}

#endif

#if defined(PSYNC_HAS_PROXY_CODE)
static int recent_detect() {
  static time_t lastdetect=0;
  if (psync_timer_time()<lastdetect+60)
    return 1;
  else {
    lastdetect=psync_timer_time();
    return 0;
  }
}
#endif

static void detect_proxy() {
#if defined(P_OS_MACOSX)
/*  CFDictionaryRef proxies;
  CFStringRef hostr, portr;
  CFNumberRef enabledr;
  UInt32 enabled;
  if (recent_detect())
    return;
  proxies=SCDynamicStoreCopyProxies(NULL);
  enabledr=(CFNumberRef)get_value_cstr(proxies, "HTTPSEnable");
  if (enabledr!=NULL) {
    if (CFNumberGetValue(enabledr, kCFNumberIntType, &enabled) && enabled) {
      hostr=(CFStringRef)get_value_cstr(proxies, "HTTPSProxy");
      portr=(CFStringRef)get_value_cstr(proxies, "HTTPSPort");
      if (hostr!=NULL && portr!=NULL) {

      }
    }
  }
  CFRelease(proxies);*/
#endif
}

static psync_socket_t connect_socket_direct(const char *host, const char *port) {
  struct addrinfo *res, *dbres;
  struct addrinfo hints;
  psync_socket_t sock;
  int rc;
  log_info("connecting to %s:%s", host, port);
  dbres = addr_load_from_db(host, port);
  if (dbres) {
    resolve_host_port resolv;
    void *params[2];
    psync_task_callback_t callbacks[2];
    psync_task_manager_t tasks;
    resolv.host=host;
    resolv.port=port;
    params[0]=dbres;
    params[1]=&resolv;
    callbacks[0]=connect_res_callback;
    callbacks[1]=resolve_callback;
    tasks=psync_task_run_tasks(callbacks, params, 2);
    res=(struct addrinfo *)psync_task_get_result(tasks, 1);
    if (unlikely(!res)) {
      psync_task_free(tasks);
      detect_proxy();
      log_warn("failed to resolve %s", host);
      return INVALID_SOCKET;
    }
    addr_save_to_db(host, port, res);
    if (addr_still_valid(dbres, res)) {
      log_info("successfully reused cached IP for %s:%s", host, port);
      sock=(psync_socket_t)(uintptr_t)psync_task_get_result(tasks, 0);
    }
    else {
      log_info("cached IP not valid for %s:%s", host, port);
      sock=connect_res(res);
    }
    freeaddrinfo(res);
    psync_task_free(tasks);
  }
  else {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    res=NULL;
    rc=getaddrinfo(host, port, &hints, &res);
    if (unlikely(rc!=0)) {
      log_warn("failed to resolve %s", host);
      detect_proxy();
      return INVALID_SOCKET;
    }
    addr_save_to_db(host, port, res);
    sock=connect_res(res);
    freeaddrinfo(res);
  }
  if (likely(sock!=INVALID_SOCKET)) {
    int sock_opt=1;
#if defined(P_OS_LINUX)
    setsockopt(sock, SOL_TCP, TCP_NODELAY, (char *)&sock_opt, sizeof(sock_opt));
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&sock_opt, sizeof(sock_opt));
#elif defined(P_OS_MACOSX) || defined(P_OS_BSD)
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char  *)&sock_opt, sizeof(sock_opt));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPALIVE, (char*)&sock_opt, sizeof(sock_opt));
#endif
#if defined(SOL_TCP)
#if defined(TCP_KEEPCNT)
    sock_opt=3;
    setsockopt(sock, SOL_TCP, TCP_KEEPCNT, (char *)&sock_opt, sizeof(sock_opt));
#endif
#if defined(TCP_KEEPIDLE)
    sock_opt=60;
    setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, (char *)&sock_opt, sizeof(sock_opt));
#endif
#if defined(TCP_KEEPINTVL)
    sock_opt=20;
    setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, (char *)&sock_opt, sizeof(sock_opt));
#endif
#endif
  }
  else {
    detect_proxy();
    log_warn("failed to connect to %s:%s", host, port);
  }
  return sock;
}

static int check_http_resp(char *str) {
  if (memcmp(str, "HTTP", 4)) {
    log_warn("bad proxy response %s", str);
    return 0;
  }
  while (*str && !isspace(*str))
    str++;
  while (*str && isspace(*str))
    str++;
  if (!isdigit(*str)) {
    log_warn("bad proxy response %s", str);
    return 0;
  }
  if (atoi(str)!=200) {
    log_info("proxy returned HTTP code %d", atoi(str));
    return 0;
  }
  return 1;
}

static psync_socket_t connect_socket_connect_proxy(const char *host, const char *port) {
  char buff[2048], *str;
  psync_socket_t sock;
  int ln, wr, r, rc;
  sock=connect_socket_direct(proxy_host, proxy_port);
  if (unlikely(sock==INVALID_SOCKET)) {
    log_info("connection to proxy %s:%s failed", proxy_host, proxy_port);
    goto err0;
  }
  ln = psync_slprintf(
      buff,
      sizeof(buff),
      "CONNECT %s:%s HTTP/1.0\015\012User-Agent: %s\015\012\015\012",
      host,
      port,
      psync_get_software_name()
  );
  wr=0;
  while (wr<ln) {
    r=psync_write_socket(sock, buff+wr, ln-wr);
    if (unlikely(r==SOCKET_ERROR)) {
      if (likely_log((psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN || psync_sock_err()==P_INTR) && !psync_wait_socket_write_timeout(sock)))
        continue;
      else
        goto err1;
    }
    wr+=r;
  }
  wr=0;
  rc=0;
  while (1) {
    if (unlikely(psync_wait_socket_read_timeout(sock))) {
      log_warn("connection to %s:%s via %s:%s timeouted", host, port, proxy_host, proxy_port);
      goto err1;
    }
    r=psync_read_socket(sock, buff+wr, sizeof(buff)-1-wr);
    if (unlikely(r==0 || r==SOCKET_ERROR)) {
      if (r==0) {
        log_info("proxy server %s:%s closed connection", proxy_host, proxy_port);
        goto err1;
      }
      if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN || psync_sock_err()==P_INTR))
        continue;
      else
        goto err1;
    }
    wr+=r;
    buff[wr]=0;
    str=strstr(buff, "\015\012\015\012");
    if (str) {
      if (rc || check_http_resp(buff)) {
        log_info("connected to %s:%s via %s:%s", host, port, proxy_host, proxy_port);
        return sock;
      }else
        goto err1;
    }
    if (wr==sizeof(buff)-1) {
      rc=check_http_resp(buff);
      if (!rc)
        goto err1;
      memcpy(buff, buff+sizeof(buff)-8, 8);
      wr=7; // yes, 7
    }
  }
err1:
  psync_close_socket(sock);
err0:
  detect_proxy();
  if (proxy_type!=PROXY_CONNECT)
    return connect_socket_direct(host, port);
  else
    return INVALID_SOCKET;
}

static psync_socket_t connect_socket(const char *host, const char *port) {
  if (unlikely(!proxy_detected)) {
    proxy_detected=1;
    detect_proxy();
  }

  if (likely(proxy_type != PROXY_CONNECT))
    return connect_socket_direct(host, port);

  return connect_socket_connect_proxy(host, port);
}

static int wait_sock_ready_for_ssl(psync_socket_t sock) {
  fd_set fds, *rfds, *wfds;
  struct timeval tv;
  int res;
  FD_ZERO(&fds);
  FD_SET(sock, &fds);
  if (psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ) {
    rfds=&fds;
    wfds=NULL;
    tv.tv_sec=PSYNC_SOCK_READ_TIMEOUT;
  }
  else if (psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE) {
    rfds=NULL;
    wfds=&fds;
    tv.tv_sec=PSYNC_SOCK_WRITE_TIMEOUT;
  }
  else {
    log_error("this functions should only be called when SSL returns WANT_READ/WANT_WRITE");
    psync_sock_set_err(P_INVAL);
    return SOCKET_ERROR;
  }
  tv.tv_usec=0;
  res=select(sock+1, rfds, wfds, NULL, &tv);
  if (res==1)
    return 0;
  if (res==0) {
    log_warn("socket timeouted");
    psync_sock_set_err(P_TIMEDOUT);
  }
  return SOCKET_ERROR;
}

psync_socket *psync_socket_connect(const char *host, int unsigned port, int ssl) {
  psync_socket *ret;
  void *sslc;
  psync_socket_t sock;
  char sport[8];
  psync_slprintf(sport, sizeof(sport), "%d", port);
  sock = connect_socket(host, sport);

  if (unlikely(sock == INVALID_SOCKET)) {
    log_warn("failed to connect socket %s:%d", host, port);
    return NULL;
  }

  if (ssl) {
    ssl = psync_ssl_connect(sock, &sslc, host);
    while (ssl == PSYNC_SSL_NEED_FINISH) {
      if (wait_sock_ready_for_ssl(sock)) {
        psync_ssl_free(sslc);
        break;
      }
      ssl = psync_ssl_connect_finish(sslc, host);
    }

    if (unlikely(ssl != PSYNC_SSL_SUCCESS)) {
      psync_close_socket(sock);
      return NULL;
    }
  } else {
    sslc = NULL;
  }

  ret = psync_new(psync_socket);
  ret->ssl = sslc;
  ret->buffer = NULL;
  ret->sock=sock;
  ret->pending=0;

  return ret;
}

void psync_socket_close(psync_socket *sock) {
  if (sock->ssl)
    while (psync_ssl_shutdown(sock->ssl)==PSYNC_SSL_NEED_FINISH)
      if (wait_sock_ready_for_ssl(sock->sock)) {
        psync_ssl_free(sock->ssl);
        break;
      }
  psync_socket_clear_write_buffered(sock);
  psync_close_socket(sock->sock);
  psync_free(sock);
}

void psync_socket_close_bad(psync_socket *sock) {
  if (sock->ssl)
    psync_ssl_free(sock->ssl);
  psync_socket_clear_write_buffered(sock);
  psync_close_socket(sock->sock);
  psync_free(sock);
}

void psync_socket_set_write_buffered(psync_socket *sock) {
  psync_socket_buffer *sb;
  if (sock->buffer)
    return;
  sb=(psync_socket_buffer *)psync_malloc(offsetof(psync_socket_buffer, buff)+PSYNC_FIRST_SOCK_WRITE_BUFF_SIZE);
  sb->next=NULL;
  sb->size=PSYNC_FIRST_SOCK_WRITE_BUFF_SIZE;
  sb->woffset=0;
  sb->roffset=0;
  sock->buffer=sb;
}

void psync_socket_set_write_buffered_thread(psync_socket *sock) {
  pthread_mutex_lock(&socket_mutex);
  psync_socket_set_write_buffered(sock);
  pthread_mutex_unlock(&socket_mutex);
}

void psync_socket_clear_write_buffered(psync_socket *sock) {
  psync_socket_buffer *nb;
  while (sock->buffer) {
    nb=sock->buffer->next;
    free(sock->buffer);
    sock->buffer=nb;
  }
}

void psync_socket_clear_write_buffered_thread(psync_socket *sock) {
  pthread_mutex_lock(&socket_mutex);
  psync_socket_clear_write_buffered(sock);
  pthread_mutex_unlock(&socket_mutex);
}

int psync_socket_set_recvbuf(psync_socket *sock, int bufsize) {
#if defined(SO_RCVBUF) && defined(SOL_SOCKET)
  return setsockopt(sock->sock, SOL_SOCKET, SO_RCVBUF, (const char*)&bufsize, sizeof(bufsize));
#else
  return -1;
#endif
}

int psync_socket_set_sendbuf(psync_socket *sock, int bufsize) {
#if defined(SO_SNDBUF) && defined(SOL_SOCKET)
  return setsockopt(sock->sock, SOL_SOCKET, SO_SNDBUF, (const char*)&bufsize, sizeof(bufsize));
#else
  return -1;
#endif
}

int psync_socket_isssl(psync_socket *sock) {
  if (sock->ssl)
    return 1;
  else
    return 0;
}

int psync_socket_pendingdata(psync_socket *sock) {
  if (sock->pending)
    return 1;
  if (sock->ssl)
    return psync_ssl_pendingdata(sock->ssl);
  else
    return 0;
}

int psync_socket_pendingdata_buf(psync_socket *sock) {
  int ret;
#if defined(P_OS_POSIX) && defined(FIONREAD)
  if (ioctl(sock->sock, FIONREAD, &ret))
    return -1;
#endif
  if (sock->ssl)
    ret+=psync_ssl_pendingdata(sock->ssl);
  return ret;
}

int psync_socket_pendingdata_buf_thread(psync_socket *sock) {
  int ret;
  pthread_mutex_lock(&socket_mutex);
  ret=psync_socket_pendingdata_buf(sock);
  pthread_mutex_unlock(&socket_mutex);
  return ret;
}

int psync_socket_try_write_buffer(psync_socket *sock) {
  if (sock->buffer) {
    psync_socket_buffer *b;
    int wrt, cw;
    wrt=0;
    while ((b=sock->buffer)) {
      if (b->roffset==b->woffset) {
        sock->buffer=b->next;
        psync_free(b);
        continue;
      }
      if (sock->ssl) {
        cw=psync_ssl_write(sock->ssl, b->buff+b->roffset, b->woffset-b->roffset);
        if (cw==PSYNC_SSL_FAIL) {
          if (likely_log(psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE))
            break;
          else {
            if (!wrt)
              wrt=-1;
            break;
          }
        }
      }
      else {
        cw=psync_write_socket(sock->sock, b->buff+b->roffset, b->woffset-b->roffset);
        if (cw==SOCKET_ERROR) {
          if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN || psync_sock_err()==P_INTR))
            break;
          else {
            if (!wrt)
              wrt=-1;
            break;
          }
        }
      }
      wrt+=cw;
      b->roffset+=cw;
      if (b->roffset!=b->woffset)
        break;
    }
    if (wrt>0)
      log_info("wrote %d bytes to socket from buffers", wrt);
    return wrt;
  }
  else
    return 0;
}

int psync_socket_try_write_buffer_thread(psync_socket *sock) {
  int ret;
  pthread_mutex_lock(&socket_mutex);
  ret=psync_socket_try_write_buffer(sock);
  pthread_mutex_unlock(&socket_mutex);
  return ret;
}

int psync_socket_readable(psync_socket *sock) {
  psync_socket_try_write_buffer(sock);
  if (sock->ssl && psync_ssl_pendingdata(sock->ssl))
    return 1;
  else if (psync_wait_socket_readable(sock->sock, 0))
    return 0;
  else {
    sock->pending=1;
    return 1;
  }
}

int psync_socket_writable(psync_socket *sock) {
  if (sock->buffer)
    return 1;
  return !psync_wait_socket_writable(sock->sock, 0);
}

static int psync_socket_read_ssl(psync_socket *sock, void *buff, int num) {
  int r;
  psync_socket_try_write_buffer(sock);
  if (!psync_ssl_pendingdata(sock->ssl) && !sock->pending && psync_wait_socket_read_timeout(sock->sock))
    return -1;
  sock->pending=0;
  while (1) {
    psync_socket_try_write_buffer(sock);
    r=psync_ssl_read(sock->ssl, buff, num);
    if (r==PSYNC_SSL_FAIL) {
      if (likely_log(psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE)) {
        if (wait_sock_ready_for_ssl(sock->sock)) {
          if (sock->buffer)
            log_warn("timeouted on socket with pending buffers");
          return -1;
        }
        else
          continue;
      }
      else {
        psync_sock_set_err(P_CONNRESET);
        return -1;
      }
    }
    else
      return r;
  }
}

static int psync_socket_read_plain(psync_socket *sock, void *buff, int num) {
  int r;
  while (1) {
    psync_socket_try_write_buffer(sock);
    if (sock->pending)
      sock->pending=0;
    else if (psync_wait_socket_read_timeout(sock->sock)) {
      log_warn("timeouted on socket with pending buffers");
      return -1;
    }
    else
        psync_socket_try_write_buffer(sock);
    r=psync_read_socket(sock->sock, buff, num);
    if (r==SOCKET_ERROR) {
      if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN))
        continue;
      else
        return -1;
    }
    else
      return r;
  }
}

int psync_socket_read(psync_socket *sock, void *buff, int num) {
  if (sock->ssl)
    return psync_socket_read_ssl(sock, buff, num);
  else
    return psync_socket_read_plain(sock, buff, num);
}

static int psync_socket_read_noblock_ssl(psync_socket *sock, void *buff, int num) {
  int r;
  r=psync_ssl_read(sock->ssl, buff, num);
  if (r==PSYNC_SSL_FAIL) {
    sock->pending=0;
    if (likely_log(psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE))
      return PSYNC_SOCKET_WOULDBLOCK;
    else {
      psync_sock_set_err(P_CONNRESET);
      return -1;
    }
  }
  else
    return r;
}

static int psync_socket_read_noblock_plain(psync_socket *sock, void *buff, int num) {
  int r;
  r=psync_read_socket(sock->sock, buff, num);
  if (r==SOCKET_ERROR) {
    sock->pending=0;
    if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN))
      return PSYNC_SOCKET_WOULDBLOCK;
    else
      return -1;
  }
  else
    return r;
}

int psync_socket_read_noblock(psync_socket *sock, void *buff, int num) {
  psync_socket_try_write_buffer(sock);
  if (sock->ssl)
    return psync_socket_read_noblock_ssl(sock, buff, num);
  else
    return psync_socket_read_noblock_plain(sock, buff, num);
}

static int psync_socket_read_ssl_thread(psync_socket *sock, void *buff, int num) {
  int r;
  pthread_mutex_lock(&socket_mutex);
  psync_socket_try_write_buffer(sock);
  pthread_mutex_unlock(&socket_mutex);
  if (!psync_ssl_pendingdata(sock->ssl) && !sock->pending && psync_wait_socket_read_timeout(sock->sock))
    return -1;
  sock->pending=0;
  while (1) {
    pthread_mutex_lock(&socket_mutex);
    psync_socket_try_write_buffer(sock);
    r=psync_ssl_read(sock->ssl, buff, num);
    pthread_mutex_unlock(&socket_mutex);
    if (r==PSYNC_SSL_FAIL) {
      if (likely_log(psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE)) {
        if (wait_sock_ready_for_ssl(sock->sock))
          return -1;
        else
          continue;
      }
      else {
        psync_sock_set_err(P_CONNRESET);
        return -1;
      }
    }
    else
      return r;
  }
}

static int psync_socket_read_plain_thread(psync_socket *sock, void *buff, int num) {
  int r;
  pthread_mutex_lock(&socket_mutex);
  psync_socket_try_write_buffer(sock);
  pthread_mutex_unlock(&socket_mutex);
  while (1) {
    if (sock->pending)
      sock->pending=0;
    else if (psync_wait_socket_read_timeout(sock->sock))
      return -1;
    pthread_mutex_lock(&socket_mutex);
    psync_socket_try_write_buffer(sock);
    r=psync_read_socket(sock->sock, buff, num);
    pthread_mutex_unlock(&socket_mutex);
    if (r==SOCKET_ERROR) {
      if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN))
        continue;
      else
        return -1;
    }
    else
      return r;
  }
}

int psync_socket_read_thread(psync_socket *sock, void *buff, int num) {
  if (sock->ssl)
    return psync_socket_read_ssl_thread(sock, buff, num);
  else
    return psync_socket_read_plain_thread(sock, buff, num);
}

static int psync_socket_write_to_buf(psync_socket *sock, const void *buff, int num) {
  psync_socket_buffer *b;
  assert(sock->buffer);
  b=sock->buffer;
  while (b->next)
    b=b->next;
  if (likely(b->size-b->woffset>=num)) {
    memcpy(b->buff+b->woffset, buff, num);
    b->woffset+=num;
    return num;
  }
  else {
    uint32_t rnum, wr;
    rnum=num;
    do {
      wr=b->size-b->woffset;
      if (!wr) {
        b->next=(psync_socket_buffer *)psync_malloc(offsetof(psync_socket_buffer, buff)+PSYNC_SECOND_SOCK_WRITE_BUFF_SIZE);
        b=b->next;
        b->next=NULL;
        b->size=PSYNC_SECOND_SOCK_WRITE_BUFF_SIZE;
        b->woffset=0;
        b->roffset=0;
        wr=PSYNC_SECOND_SOCK_WRITE_BUFF_SIZE;
      }
      if (wr>rnum)
        wr=rnum;
      memcpy(b->buff+b->woffset, buff, wr);
      b->woffset+=wr;
      buff=(const char *)buff+wr;
      rnum-=wr;
    } while (rnum);
    return num;
  }
}

int psync_socket_write(psync_socket *sock, const void *buff, int num) {
  int r;
  if (sock->buffer)
    return psync_socket_write_to_buf(sock, buff, num);
  if (psync_wait_socket_write_timeout(sock->sock))
    return -1;
  if (sock->ssl) {
    r=psync_ssl_write(sock->ssl, buff, num);
    if (r==PSYNC_SSL_FAIL) {
      if (likely_log(psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE))
        return 0;
      else
        return -1;
    }
  }
  else {
    r=psync_write_socket(sock->sock, buff, num);
    if (r==SOCKET_ERROR) {
      if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN || psync_sock_err()==P_INTR))
        return 0;
      else
        return -1;
    }
  }
  return r;
}

static int psync_socket_readall_ssl(psync_socket *sock, void *buff, int num) {
  int br, r;
  br=0;
  psync_socket_try_write_buffer(sock);
  if (!psync_ssl_pendingdata(sock->ssl) && !sock->pending && psync_wait_socket_read_timeout(sock->sock))
    return -1;
  sock->pending=0;
  while (br<num) {
    psync_socket_try_write_buffer(sock);
    r=psync_ssl_read(sock->ssl, (char *)buff+br, num-br);
    if (r == PSYNC_SSL_FAIL) {
      if (likely(psync_ssl_errno == PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno == PSYNC_SSL_ERR_WANT_WRITE)) {
        if (wait_sock_ready_for_ssl(sock->sock))
          return -1;
        else
          continue;
      } else {
        log_warn("psync_ssl_errno is on unexpected state: %d", psync_ssl_errno);
        psync_sock_set_err(P_CONNRESET);
        return -1;
      }
    }
    if (r==0)
      return br;
    br+=r;
  }
  return br;
}

static int psync_socket_readall_plain(psync_socket *sock, void *buff, int num) {
  int br, r;
  br = 0;
  while (br < num) {
    psync_socket_try_write_buffer(sock);

    if (sock->pending)
      sock->pending = 0;
    else if (psync_wait_socket_read_timeout(sock->sock))
      return -1;
    else
        psync_socket_try_write_buffer(sock);

    r = psync_read_socket(sock->sock, (char *)buff + br, num - br);
    if (r == SOCKET_ERROR) {
      if (likely(psync_sock_err() == P_WOULDBLOCK || psync_sock_err() == P_AGAIN)) {
        continue;
      }

      log_warn("socket read error %s", strerror(psync_sock_err()));
      return -1;
    }

    if (r == 0)
      return br;
    br += r;
  }

  return br;
}

int psync_socket_readall(psync_socket *sock, void *buff, int num) {
  if (sock->ssl)
    return psync_socket_readall_ssl(sock, buff, num);

  return psync_socket_readall_plain(sock, buff, num);
}

static int psync_socket_writeall_ssl(psync_socket *sock, const void *buff, int num) {
  int br, r;
  br=0;
  while (br<num) {
    r=psync_ssl_write(sock->ssl, (char *)buff+br, num-br);
    if (r==PSYNC_SSL_FAIL) {
      if (psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE) {
        if (wait_sock_ready_for_ssl(sock->sock))
          return -1;
        else
          continue;
      }
      else {
        psync_sock_set_err(P_CONNRESET);
        return -1;
      }
    }
    if (r==0)
      return br;
    br+=r;
  }
  return br;
}

static int psync_socket_writeall_plain(psync_socket_t sock, const void *buff, int num) {
  int br, r;
  br=0;
  while (br<num) {
    r=psync_write_socket(sock, (const char *)buff+br, num-br);
    if (r==SOCKET_ERROR) {
      if (psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN) {
        if (psync_wait_socket_write_timeout(sock))
          return -1;
        else
          continue;
      }
      else
        return -1;
    }
    br+=r;
  }
  return br;
}

int psync_socket_writeall(psync_socket *sock, const void *buff, int num) {
  if (sock->buffer)
    return psync_socket_write_to_buf(sock, buff, num);
  if (sock->ssl)
    return psync_socket_writeall_ssl(sock, buff, num);
  else
    return psync_socket_writeall_plain(sock->sock, buff, num);
}

static int psync_socket_readall_ssl_thread(psync_socket *sock, void *buff, int num) {
  int br, r;
  br=0;
  pthread_mutex_lock(&socket_mutex);
  psync_socket_try_write_buffer(sock);
  r=psync_ssl_pendingdata(sock->ssl);
  pthread_mutex_unlock(&socket_mutex);
  if (!r && !sock->pending && psync_wait_socket_read_timeout(sock->sock))
    return -1;
  sock->pending=0;
  while (br<num) {
    pthread_mutex_lock(&socket_mutex);
    psync_socket_try_write_buffer(sock);
    r=psync_ssl_read(sock->ssl, (char *)buff+br, num-br);
    pthread_mutex_unlock(&socket_mutex);
    if (r==PSYNC_SSL_FAIL) {
      if (likely_log(psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE)) {
        if (wait_sock_ready_for_ssl(sock->sock))
          return -1;
        else
          continue;
      }
      else {
        psync_sock_set_err(P_CONNRESET);
        return -1;
      }
    }
    if (r==0)
      return br;
    br+=r;
  }
  return br;
}

static int psync_socket_readall_plain_thread(psync_socket *sock, void *buff, int num) {
  int br, r;
  br=0;
  pthread_mutex_lock(&socket_mutex);
  psync_socket_try_write_buffer(sock);
  pthread_mutex_unlock(&socket_mutex);
  while (br<num) {
    if (sock->pending)
      sock->pending=0;
    else if (psync_wait_socket_read_timeout(sock->sock))
      return -1;
    pthread_mutex_lock(&socket_mutex);
    psync_socket_try_write_buffer(sock);
    r=psync_read_socket(sock->sock, (char *)buff+br, num-br);
    pthread_mutex_unlock(&socket_mutex);
    if (r==SOCKET_ERROR) {
      if (likely_log(psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN))
        continue;
      else
        return -1;
    }
    if (r==0)
      return br;
    br+=r;
  }
  return br;
}

int psync_socket_readall_thread(psync_socket *sock, void *buff, int num) {
  if (sock->ssl)
    return psync_socket_readall_ssl_thread(sock, buff, num);
  else
    return psync_socket_readall_plain_thread(sock, buff, num);
}

static int psync_socket_writeall_ssl_thread(psync_socket *sock, const void *buff, int num) {
  int br, r;
  br=0;
  while (br<num) {
    pthread_mutex_lock(&socket_mutex);
    if (sock->buffer)
      r=psync_socket_write_to_buf(sock, buff, num);
    else
      r=psync_ssl_write(sock->ssl, (char *)buff+br, num-br);
    pthread_mutex_unlock(&socket_mutex);
    if (r==PSYNC_SSL_FAIL) {
      if (psync_ssl_errno==PSYNC_SSL_ERR_WANT_READ || psync_ssl_errno==PSYNC_SSL_ERR_WANT_WRITE) {
        if (wait_sock_ready_for_ssl(sock->sock))
          return -1;
        else
          continue;
      }
      else {
        psync_sock_set_err(P_CONNRESET);
        return -1;
      }
    }
    if (r==0)
      return br;
    br+=r;
  }
  return br;
}

static int psync_socket_writeall_plain_thread(psync_socket *sock, const void *buff, int num) {
  int br, r;
  br=0;
  while (br<num) {
    pthread_mutex_lock(&socket_mutex);
    if (sock->buffer)
      r=psync_socket_write_to_buf(sock, buff, num);
    else
      r=psync_write_socket(sock->sock, (const char *)buff+br, num-br);
    pthread_mutex_unlock(&socket_mutex);
    if (r==SOCKET_ERROR) {
      if (psync_sock_err()==P_WOULDBLOCK || psync_sock_err()==P_AGAIN) {
        if (psync_wait_socket_write_timeout(sock->sock))
          return -1;
        else
          continue;
      }
      else
        return -1;
    }
    br+=r;
  }
  return br;
}

int psync_socket_writeall_thread(psync_socket *sock, const void *buff, int num) {
  if (sock->ssl)
    return psync_socket_writeall_ssl_thread(sock, buff, num);
  else
    return psync_socket_writeall_plain_thread(sock, buff, num);
}

static void copy_address(struct sockaddr_storage *dst, const struct sockaddr *src) {
  dst->ss_family=src->sa_family;
  if (src->sa_family==AF_INET)
    memcpy(&((struct sockaddr_in *)dst)->sin_addr, &((const struct sockaddr_in *)src)->sin_addr, sizeof(((struct sockaddr_in *)dst)->sin_addr));
  else
    memcpy(&((struct sockaddr_in6 *)dst)->sin6_addr, &((const struct sockaddr_in6 *)src)->sin6_addr, sizeof(((struct sockaddr_in6 *)dst)->sin6_addr));
}

psync_interface_list_t *psync_list_ip_adapters() {
  psync_interface_list_t *ret;
  size_t cnt, ret_size;

#if defined(P_OS_POSIX)
  struct ifaddrs *addrs, *addr;
  sa_family_t family;
  size_t sz;

  if (unlikely(getifaddrs(&addrs) != 0)) {
    log_error("failed to obtain network interfaces: %s", strerror(errno));
    goto empty;
  }

  cnt = 0;
  addr = addrs;
  while (addr) {
    if (addr->ifa_addr) {
      family = addr->ifa_addr->sa_family;
      if (family != AF_INET && family != AF_INET6) {
        addr = addr->ifa_next;
        continue;
      }

      if (!addr->ifa_broadaddr || !addr->ifa_netmask) {
        addr = addr->ifa_next;
        continue;
      }

      cnt++;
    }
    addr = addr->ifa_next;
  }

  ret_size = offsetof(psync_interface_list_t, interfaces) +
      sizeof(psync_interface_t) * cnt;
  ret = psync_malloc(ret_size);
  memset(ret, 0, ret_size);
  ret->interfacecnt = cnt;

  addr = addrs;
  cnt = 0;
  while (addr) {
    if (addr->ifa_addr) {
      family = addr->ifa_addr->sa_family;
      if ((family == AF_INET || family == AF_INET6) && addr->ifa_broadaddr &&
          addr->ifa_netmask) {
        if (family == AF_INET)
          sz = sizeof(struct sockaddr_in);
        else
          sz = sizeof(struct sockaddr_in6);

        copy_address(&ret->interfaces[cnt].address, addr->ifa_addr);
        copy_address(&ret->interfaces[cnt].broadcast, addr->ifa_broadaddr);
        copy_address(&ret->interfaces[cnt].netmask, addr->ifa_netmask);
        ret->interfaces[cnt].addrsize = sz;
        cnt++;
      }
    }
    addr = addr->ifa_next;
  }
  freeifaddrs(addrs);
  return ret;
#endif
  empty:
  ret = psync_malloc(offsetof(psync_interface_list_t, interfaces));
  ret->interfacecnt = 0;
  return ret;
}

#if !defined(P_OS_POSIX)
static int psync_compat_socketpair(psync_socket_t sockfd[2]) {
  psync_socket_t sock;
  struct sockaddr_in addr;
  socklen_t addrlen;
  sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock==INVALID_SOCKET) {
      goto err0;
  }
  memset(&addr, 0, sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
  addrlen=sizeof(addr);
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr))==SOCKET_ERROR ||
      listen(sock, 1)==SOCKET_ERROR ||
      getsockname(sock, (struct sockaddr *)&addr, &addrlen)==SOCKET_ERROR ||
      (sockfd[0]=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==INVALID_SOCKET)
    goto err1;
  if (connect(sockfd[0], (struct sockaddr *)&addr, addrlen)==SOCKET_ERROR ||
      (sockfd[1]=accept(sock, NULL, NULL))==INVALID_SOCKET)
    goto err2;
  psync_close_socket(sock);
  return 0;
err2:
  psync_close_socket(sockfd[0]);
err1:
  psync_close_socket(sock);
err0:
  return SOCKET_ERROR;
}
#endif

int psync_pipe(psync_socket_t pipefd[2]) {
#if defined(P_OS_POSIX)
  return pipe(pipefd);
#else
  return psync_compat_socketpair(pipefd);
#endif
}

int psync_pipe_close(psync_socket_t pfd) {
  return psync_close_socket(pfd);
}

int psync_pipe_read(psync_socket_t pfd, void *buff, int num) {
#if defined(P_OS_POSIX)
  return read(pfd, buff, num);
#endif
}

int psync_pipe_write(psync_socket_t pfd, const void *buff, int num) {
#if defined(P_OS_POSIX)
  return write(pfd, buff, num);
#endif
}

int psync_socket_pair(psync_socket_t sfd[2]) {
#if defined(P_OS_POSIX)
  return socketpair(AF_UNIX, SOCK_STREAM, 0, sfd);
#else
  return psync_compat_socketpair(sfd);
#endif
}

int psync_socket_is_broken(psync_socket_t sock) {
  fd_set rfds;
  struct timeval tv;
  memset(&tv, 0, sizeof(tv));
  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);
  return select(sock+1, NULL, NULL, &rfds, &tv)==1;
}

int psync_select_in(psync_socket_t *sockets, int cnt, int64_t timeoutmillisec) {
  fd_set rfds;
  struct timeval tv, *ptv;
  psync_socket_t max;
  int i;
  if (timeoutmillisec<0)
    ptv=NULL;
  else {
    tv.tv_sec=timeoutmillisec/1000;
    tv.tv_usec=(timeoutmillisec%1000)*1000;
    ptv=&tv;
  }
  FD_ZERO(&rfds);
  max=0;
  for (i=0; i<cnt; i++) {
    FD_SET(sockets[i], &rfds);
    if (sockets[i]>=max)
      max=sockets[i]+1;
  }
  i=select(max, &rfds, NULL, NULL, ptv);
  if (i>0) {
    for (i=0; i<cnt; i++)
      if (FD_ISSET(sockets[i], &rfds))
        return i;
  }
  else if (i==0)
    psync_sock_set_err(P_TIMEDOUT);
  return SOCKET_ERROR;
}

/*! \brief Traverses a \a path and calls a \a cb with \a ptr for every file
 *         or directory found.
 *
 * \todo Consider to move this, and other fs-related functions to a dedicated
 *       place.
 */
int psync_list_dir(const char *path, psync_list_dir_callback cb, void *ptr) {
#if defined(P_OS_POSIX)
  psync_pstat pst;
  DIR *dh;
  char *cpath;
  size_t pl;
  long namelen;
  struct dirent *ent;
  int ret;

  dh = opendir(path);
  if (unlikely(!dh)) {
    log_error("failed to open directory %s: %s", path, strerror(errno));
    psync_error = PERROR_LOCAL_FOLDER_NOT_FOUND;
    return -1;
  }

  pl = strlen(path);
  namelen = pathconf(path, _PC_NAME_MAX);
  if (unlikely(namelen == -1))
    namelen=255;

  if (namelen < sizeof(ent->d_name) - 1)
    namelen = sizeof(ent->d_name) - 1;

  cpath = (char *)psync_malloc(pl + namelen + 2);
  memcpy(cpath, path, pl);
  if (!pl || cpath[pl - 1] != PSYNC_DIRECTORY_SEPARATORC)
    cpath[pl++] = PSYNC_DIRECTORY_SEPARATORC;
  pst.path = cpath;

  while (NULL != (ent = readdir(dh))) {
    if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
      continue;
    }

    strlcpy(cpath + pl, ent->d_name, namelen + 1);
    ret = lstat(cpath, &pst.stat);
    if (unlikely(ret == -1)) {
      log_error("filed to get status for %s: %s", cpath, strerror(errno));
      continue;
    }

    if ((S_ISREG(pst.stat.st_mode) || S_ISDIR(pst.stat.st_mode))) {
      pst.name = ent->d_name;
      cb(ptr, &pst);
    }
  }

  psync_free(cpath);
  closedir(dh);
  return 0;
#endif
}

/*! \brief Traverses a \a path and calls a \a cb with \a ptr for every file
 *         or directory found.
 *
 *  A fast version of psync_list_dir().
 *
 * \todo Consider to move this, and other fs-related functions to a dedicated
 *       place.
 */
int psync_list_dir_fast(const char *path, psync_list_dir_callback_fast cb, void *ptr) {
#if defined(P_OS_POSIX)
  psync_pstat_fast pst;
  struct stat st;
  DIR *dh;
  char *cpath;
  size_t pl;
  long namelen;
  struct dirent *ent;
  int ret;

  dh = opendir(path);
  if (unlikely(!dh)){
    log_error("failed to open directory %s: %s", path, strerror(errno));
    psync_error = PERROR_LOCAL_FOLDER_NOT_FOUND;
    return -1;
  }

  pl = strlen(path);
  namelen = pathconf(path, _PC_NAME_MAX);
  if (namelen == -1)
    namelen = 255;

  if (namelen < sizeof(ent->d_name) - 1)
    namelen = sizeof(ent->d_name) - 1;

  cpath = (char *)psync_malloc(pl + namelen + 2);
  memcpy(cpath, path, pl);
  if (!pl || cpath[pl - 1] != PSYNC_DIRECTORY_SEPARATORC)
    cpath[pl++] = PSYNC_DIRECTORY_SEPARATORC;

  while (NULL != (ent = readdir(dh))) {
    if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
      continue;
    }

#if defined(DT_UNKNOWN) && defined(DT_DIR) && defined(DT_REG)
    pst.name = ent->d_name;
    if (ent->d_type == DT_UNKNOWN) {
      strlcpy(cpath + pl, ent->d_name, namelen + 1);
      ret = lstat(cpath, &st);
      if (unlikely(ret == -1)) {
        log_error("filed to get status for %s: %s", cpath, strerror(errno));
        continue;
      }
      pst.isfolder = S_ISDIR(st.st_mode);
    } else if (ent->d_type == DT_DIR)
      pst.isfolder = 1;
    else if (ent->d_type == DT_REG)
      pst.isfolder = 0;
    else
      continue;

    cb(ptr, &pst);
#else
    strlcpy(cpath + pl, ent->d_name, namelen + 1);
    ret = lstat(cpath, &st);
    if (unlikely(ret == -1)) {
      log_error("filed to get status for %s: %s", cpath, strerror(errno));
      continue;
    }

    pst.name = ent->d_name;
    pst.isfolder = S_ISDIR(st.st_mode);

    cb(ptr, &pst);
#endif
  }
  psync_free(cpath);
  closedir(dh);
  return 0;
#endif
}

int64_t psync_get_free_space_by_path(const char *path) {
#if defined(P_OS_POSIX)
  struct statvfs buf;
  if (statvfs(path, &buf) == -1) {
    log_error("filed retrieve file system information for path: %s",
              path, strerror(errno));
    return -1;
  }

  return (int64_t)buf.f_bavail * (int64_t)buf.f_frsize;
#endif
}

int psync_mkdir(const char *path) {
#if defined(P_OS_POSIX)
  return mkdir(path, PSYNC_DEFAULT_POSIX_FOLDER_MODE);
#endif
}

int psync_rmdir(const char *path) {
#if defined(P_OS_POSIX)
  return rmdir(path);
#endif
}

int psync_file_rename_overwrite(const char *oldpath, const char *newpath) {
  if (!psync_filename_cmp(oldpath, newpath))
    return 0;
  return rename(oldpath, newpath);
}

int psync_file_delete(const char *path) {
#if defined(P_OS_POSIX)
  return unlink(path);
#endif
}

psync_file_t psync_file_open(const char *path, int access, int flags) {
#if defined(P_OS_POSIX)
  int fd;
#if defined(O_CLOEXEC)
  flags|=O_CLOEXEC;
#endif
#if defined(O_NOATIME)
  flags|=O_NOATIME;
#endif
  fd=open(path, access|flags, PSYNC_DEFAULT_POSIX_FILE_MODE);
  if (unlikely(fd==-1)) {
    while (errno==EINTR) {
      log_info("got EINTR while opening file");
      fd=open(path, access|flags, PSYNC_DEFAULT_POSIX_FILE_MODE);
      if (fd!=-1)
        return fd;
    }
  }
  return fd;
#endif
}

int psync_file_close(psync_file_t fd) {
#if defined(P_OS_POSIX)
  return close(fd);
#endif
}

int psync_file_sync(psync_file_t fd) {
#if defined(F_FULLFSYNC) && defined(P_OS_POSIX)
  if (unlikely(fcntl(fd, F_FULLFSYNC))) {
    while (errno==EINTR) {
      log_info("got EINTR while fsyncing file");
      if (!fcntl(fd, F_FULLFSYNC))
        return 0;
    }
    log_info("got error %d, when doing fcntl(F_FULLFSYNC), trying fsync()", (int)errno);
    if (fsync(fd)) {
      log_info("fsync also failed, error %d", (int)errno);
      return -1;
    }
    else {
      log_info("fsync succeded");
      return 0;
    }
  }
  else
    return 0;
#elif defined(P_OS_POSIX)
#if _POSIX_SYNCHRONIZED_IO>0
  if (unlikely(fdatasync(fd))) {
#else
  if (unlikely(fsync(fd))) {
#endif
    while (errno==EINTR) {
      log_info("got EINTR while fsyncing file");
      if (!fsync(fd))
        return 0;
    }
    log_info("got error %d", (int)errno);
    return -1;
  }
  else
    return 0;
#else
#error "Function not implemented for your operating system"
#endif
}

int psync_file_schedulesync(psync_file_t fd) {
#if defined(P_OS_LINUX) && defined(SYNC_FILE_RANGE_WRITE)
  return sync_file_range(fd, 0, 0, SYNC_FILE_RANGE_WRITE);
#elif defined(P_OS_POSIX) && _POSIX_MAPPED_FILES>0 && _POSIX_SYNCHRONIZED_IO>0
  struct stat st;
  void *fmap;
  int ret;
  if (unlikely(fstat(fd, &st))) {
    log_info("fstat failed, errno=%d", (int)errno);
    return -1;
  }
  fmap=mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (unlikely(fmap==MAP_FAILED)) {
    log_info("mmap failed, errno=%d", (int)errno);
    return -1;
  }
  ret=msync(fmap, st.st_size, MS_ASYNC);
  if (unlikely(ret))
    log_info("msync failed, errno=%d", (int)errno);
  munmap(fmap, st.st_size);
  return ret;
#else
  return 0;
#endif
}

int psync_folder_sync(const char *path) {
#if defined(P_OS_POSIX)
  int fd, ret;
  fd=open(path, O_RDONLY);
  if (fd==-1) {
    log_info("could not open folder %s, error %d", path, (int)errno);
    return -1;
  }
  if (unlikely(psync_file_sync(fd))) {
    log_info("could not fsync folder %s, error %d", path, (int)errno);
    ret=-1;
  }
  else
    ret=0;
  close(fd);
  return ret;
#endif
}

psync_file_t psync_file_dup(psync_file_t fd) {
#if defined(P_OS_POSIX)
  return dup(fd);
#endif
}

int psync_file_set_creation(psync_file_t fd, time_t ctime) {
#if defined(P_OS_MACOSX)
  struct attrlist attr;
  struct timespec crtime;
  memset(&attr, 0, sizeof(attr));
  attr.bitmapcount=ATTR_BIT_MAP_COUNT;
  attr.commonattr=ATTR_CMN_CRTIME;
  crtime.tv_sec=ctime;
  crtime.tv_nsec=0;
  return fsetattrlist(fd, &attr, &crtime, sizeof(struct timespec), FSOPT_NOFOLLOW);
#else
  return -1;
#endif
}

int psync_set_crtime_mtime(const char *path, time_t crtime, time_t mtime) {
#if defined(P_OS_MACOSX)
  if (crtime) {
    struct attrlist attr;
    struct timespec crtimes;
    memset(&attr, 0, sizeof(attr));
    attr.bitmapcount=ATTR_BIT_MAP_COUNT;
    attr.commonattr=ATTR_CMN_CRTIME;
    crtimes.tv_sec=crtime;
    crtimes.tv_nsec=0;
    if (setattrlist(path, &attr, &crtimes, sizeof(struct timespec), FSOPT_NOFOLLOW))
      return -1;
  }
  if (mtime) {
    struct timeval tm[2];
    tm[0].tv_sec=mtime;
    tm[0].tv_usec=0;
    tm[1].tv_sec=mtime;
    tm[1].tv_usec=0;
    return utimes(path, tm);
  }
  return 0;
#elif defined(P_OS_POSIX)
  if (mtime) {
    struct timeval tm[2];
    tm[0].tv_sec=mtime;
    tm[0].tv_usec=0;
    tm[1].tv_sec=mtime;
    tm[1].tv_usec=0;
    if (unlikely(utimes(path, tm))) {
      log_info("got errno %d while setting modification time of %s to %lu: %s", errno, path, (unsigned long)mtime, strerror(errno));
      return -1;
    }
    else
      return 0;
  }
  else
    return 0;
#else
  return -1;
#endif
}

int psync_set_crtime_mtime_by_fd(psync_file_t fd, const char *path, time_t crtime, time_t mtime) {
#if defined(P_OS_MACOSX)
  if (crtime) {
    struct attrlist attr;
    struct timespec crtimes;
    memset(&attr, 0, sizeof(attr));
    attr.bitmapcount=ATTR_BIT_MAP_COUNT;
    attr.commonattr=ATTR_CMN_CRTIME;
    crtimes.tv_sec=crtime;
    crtimes.tv_nsec=0;
    if (fsetattrlist(fd, &attr, &crtimes, sizeof(struct timespec), FSOPT_NOFOLLOW))
      return -1;
  }
  if (mtime) {
    struct timeval tm[2];
    tm[0].tv_sec=mtime;
    tm[0].tv_usec=0;
    tm[1].tv_sec=mtime;
    tm[1].tv_usec=0;
    return futimes(fd, tm);
  }
  return 0;
#elif defined(_BSD_SOURCE) || defined(P_OS_BSD)
  if (mtime) {
    struct timeval tm[2];
    tm[0].tv_sec=mtime;
    tm[0].tv_usec=0;
    tm[1].tv_sec=mtime;
    tm[1].tv_usec=0;
    if (unlikely(futimes(fd, tm))) {
      log_info("got errno %d while setting modification time of %s to %lu: %s", errno, path, (unsigned long)mtime, strerror(errno));
      return -1;
    }
    else
      return 0;
  }
  else
    return 0;
#else
  return psync_set_crtime_mtime(path, crtime, mtime);
#endif
}

typedef struct {
  uint64_t offset;
  size_t count;
  psync_file_t fd;
} psync_file_preread_t;

static void psync_file_preread_thread(void *ptr) {
  char buff[16*1024];
  psync_file_preread_t *pr;
  ssize_t rd;
  pr=(psync_file_preread_t *)ptr;
  while (pr->count) {
    rd=psync_file_pread(pr->fd, buff, pr->count>sizeof(buff)?sizeof(buff):pr->count, pr->offset);
    if (rd<=0)
      break;
    pr->offset+=rd;
    pr->count-=rd;
  }
  psync_file_close(pr->fd);
  psync_free(pr);
}

int psync_file_preread(psync_file_t fd, uint64_t offset, size_t count) {
  psync_file_preread_t *pr;
  psync_file_t cfd;
  cfd=psync_file_dup(fd);
  if (cfd==INVALID_HANDLE_VALUE)
    return -1;
  pr=psync_new(psync_file_preread_t);
  pr->offset=offset;
  pr->count=count;
  pr->fd=cfd;
  psync_run_thread1("pre-read (readahead) thread", psync_file_preread_thread, pr);
  return 0;
}

int psync_file_readahead(psync_file_t fd, uint64_t offset, size_t count) {
#if defined(P_OS_POSIX) && defined(POSIX_FADV_WILLNEED)
  return posix_fadvise(fd, offset, count, POSIX_FADV_WILLNEED);
#elif defined(P_OS_POSIX) && defined(F_RDADVISE)
  struct radvisory ra;
  ra.ra_offset=offset;
  ra.ra_count=count;
  return fcntl(fd, F_RDADVISE, &ra);
#else
  return psync_file_preread(fd, offset, count);
#endif
}

ssize_t psync_file_read(psync_file_t fd, void *buf, size_t count) {
#if defined(P_OS_POSIX)
  ssize_t ret;
  ret=read(fd, buf, count);
  if (unlikely(ret==-1)) {
    while (errno==EINTR) {
      log_info("got EINTR while reading from file");
      ret=read(fd, buf, count);
      if (ret!=-1)
        return ret;
    }
    log_info("got error %d", (int)errno);
  }
  return ret;
#endif
}

ssize_t psync_file_pread(psync_file_t fd, void *buf, size_t count, uint64_t offset) {
#if defined(P_OS_POSIX)
  ssize_t ret;
  ret=pread(fd, buf, count, offset);
  if (unlikely(ret==-1)) {
    while (errno==EINTR) {
      log_info("got EINTR while writing to file");
      ret=pread(fd, buf, count, offset);
      if (ret!=-1)
        return ret;
    }
    log_info("got error %d", (int)errno);
  }
  return ret;
#endif
}

ssize_t psync_file_write(psync_file_t fd, const void *buf, size_t count) {
#if defined(P_OS_POSIX)
  ssize_t ret;
  ret=write(fd, buf, count);
  if (unlikely(ret==-1)) {
    while (errno==EINTR) {
      log_info("got EINTR while writing to file");
      ret=write(fd, buf, count);
      if (ret!=-1)
        return ret;

    }
    log_info("got error %d", (int)errno);
  }
  return ret;
#endif
}

ssize_t psync_file_pwrite(psync_file_t fd, const void *buf, size_t count, uint64_t offset) {
#if defined(P_OS_POSIX)
  ssize_t ret;
  ret=pwrite(fd, buf, count, offset);
  if (unlikely(ret==-1)) {
    while (errno==EINTR) {
      log_info("got EINTR while writing to file");
      ret=pwrite(fd, buf, count, offset);
      if (ret!=-1)
        return ret;
    }
    log_info("got error %d", (int)errno);
  }
  return ret;
#endif
}

int64_t psync_file_seek(psync_file_t fd, uint64_t offset, int whence) {
#if defined(P_OS_POSIX)
  return lseek(fd, offset, whence);
#endif
}

int psync_file_truncate(psync_file_t fd) {
#if defined(P_OS_POSIX)
  off_t off;
  off=lseek(fd, 0, SEEK_CUR);
  if (likely_log(off!=(off_t)-1)) {
    if (unlikely(ftruncate(fd, off))) {
      while (errno==EINTR) {
        log_info("got EINTR while truncating file");
        if (!ftruncate(fd, off))
          return 0;
      }
      log_info("got error %d", (int)errno);
      return -1;
    }
    else
      return 0;
  }
  else
    return -1;
#endif
}

int64_t psync_file_size(psync_file_t fd) {
#if defined(P_OS_POSIX)
  struct stat st;
  if (unlikely_log(fstat(fd, &st)))
    return -1;
  else
    return st.st_size;
#endif
}

int psync_run_update_file(const char *path) {
#if defined(P_OS_LINUX) || defined(P_OS_MACOSX)
#if defined(P_OS_LINUX)
#if defined(P_OS_DEBIAN)
/* TODO: Sort out wuth this */
#define PSYNC_RUN_CMD "/usr/lib/psyncgui/debinstall.sh"
#else
#define PSYNC_RUN_CMD "qapt-deb-installer"
#endif
#else
#define PSYNC_RUN_CMD "open"
#endif
  pid_t pid;
  log_info("running %s with "PSYNC_RUN_CMD, path);
  pid=fork();
  if (unlikely(pid==-1)) {
    log_error("fork failed");
    return -1;
  }
  else if (pid) {
    int status;
    psync_milisleep(100);
    if (waitpid(pid, &status, WNOHANG)==0)
      return 0;
    else
      return -1;
  }
  else {
    char *ex;
    int fd;
    fd=open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    setsid();
    ex=psync_strcat(PSYNC_RUN_CMD" \"", path, "\"", NULL);
    execl("/bin/sh", "/bin/sh", "-c", ex, NULL);
    log_error("exec of %s failed", ex);
    psync_free(ex);
    exit(1);
  }
#endif
}

int psync_invalidate_os_cache_needed() {
#if defined(P_OS_MACOSX)
  return 0;
#else
  return 0;
#endif
}

extern int overlays_running;

#if defined(P_OS_MACOSX)
void psync_rebuild_icons() {
  int ret = 0;

  if (!overlays_running)
    return;

  log_info("Stopping finder plugin to refresh all icons.");
  ret = system("/bin/sh -c \"pluginkit -e ignore -i com.pcloud.pcloud.macos.pCloudFinderExt;sleep 0.5;pluginkit -e use -i com.pcloud.pcloud.macos.pCloudFinderExt;\"");
  log_error("Reseting Finder Ext");
}
#else
void psync_rebuild_icons() {
  if (!overlays_running)
    return;
  return;
}
#endif

int psync_invalidate_os_cache(const char *path) {
#if defined(P_OS_MACOSX)
  int pfds[2];
  pid_t pid;
  log_info("running osascript to refresh finder");
  if (unlikely(pipe(pfds))) {
    log_error("pipe failed");
    return -1;
  }
  pid=fork();
  if (unlikely(pid==-1)) {
    close(pfds[0]);
    close(pfds[1]);
    log_error("fork failed");
    return -1;
  }
  else if (pid) {
    const char *cmd="tell application \"Finder\"\n\
  repeat with i from 1 to count of Finder windows\n\
    tell window i\n\
      try\n\
        update every item in every folder with necessity\n\
      end try\n\
    end tell\n\
  end repeat\n\
end tell\n";
    int status;
    close(pfds[0]);
    status=strlen(cmd);
    if (write(pfds[1], cmd, status)!=status || close(pfds[1])!=0) {
      log_error("write to pipe failed");
      kill(pid, SIGKILL);
      waitpid(pid, &status, 0);
      return -1;
    }
    if (waitpid(pid, &status, 0)==0 && WIFEXITED(status) && WEXITSTATUS(status)==0) {
      log_info("execution of osascript succeded");
      return 0;
    }
    else {
      log_error("execution of osascript failed");
      return -1;
    }
  }
  else {
    int fd;
    close(pfds[1]);
    dup2(pfds[0], STDIN_FILENO);
    close(pfds[0]);
    fd=open("/dev/null", O_RDWR);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    execl("/bin/sh", "/bin/sh", "-c", "osascript", NULL);
    log_error("exec of \"/bin/sh -c osascript\" failed");
    exit(1);
  }
#else
  return 0;
#endif
}

void *psync_mmap_anon(size_t size) {
#if defined(PSYNC_MAP_ANONYMOUS)
  return mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|PSYNC_MAP_ANONYMOUS, -1, 0);
#endif
}

PSYNC_NOINLINE static void *psync_mmap_anon_emergency(size_t size) {
  void *ret;
  log_warn("could not allocate %lu bytes", (unsigned long)size);
  psync_try_free_memory();
  ret=psync_mmap_anon(size);
  if (likely(ret))
    return ret;
  else {
    log_fatal("could not allocate %lu bytes even after freeing some memory, aborting", (unsigned long)size);
    abort();
    return NULL;
  }
}

void *psync_mmap_anon_safe(size_t size) {
  void *ret;
  ret=psync_mmap_anon(size);
  if (likely(ret))
    return ret;
  else
    return psync_mmap_anon_emergency(size);
}

int psync_munmap_anon(void *ptr, size_t size) {
#if defined(PSYNC_MAP_ANONYMOUS)
  return munmap(ptr, size);
#endif
}

void psync_anon_reset(void *ptr, size_t size) {
#if defined(PSYNC_MAP_ANONYMOUS) && defined(MADV_DONTNEED)
  madvise(ptr, size, MADV_DONTNEED);
#endif
}

int psync_mlock(void *ptr, size_t size) {
#if defined(_POSIX_MEMLOCK_RANGE)
  return mlock(ptr, size);
#endif
}

int psync_munlock(void *ptr, size_t size) {
#if defined(_POSIX_MEMLOCK_RANGE)
  return munlock(ptr, size);
#endif
}

int psync_get_page_size() {
  return psync_page_size;
}
