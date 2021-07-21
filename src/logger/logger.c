/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include <limits.h>
#include <pthread.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

#include "logger.h"

void log_lock(bool lock, void *udata) {
  pthread_mutex_t *LOCK = (pthread_mutex_t*)(udata);
  if (lock)
    pthread_mutex_lock(LOCK);
  else
    pthread_mutex_unlock(LOCK);
}

static const char *resolve_filepath(const char* rawpath) {
  char *newpath = NULL;
  const char *homedir;
  int n;

  /* do not try use file for logging if:
   * - rawpath == NULL
   * - rawpath == ""
   * - rawpath == "/"
   */
  if (!rawpath || strlen(rawpath) <= 1) {
    return NULL;
  }

  if (rawpath[0] == '~') {
    if (strlen(rawpath) <= 2) {
      /* rawpath is not empty, but is just "~" or "~/" */
      return NULL;
    }

    /* check the $HOME environment variable */
    if ((homedir = getenv("HOME")) == NULL) {
      /* use getpwuid if env var HOME does not exist */
      homedir = getpwuid(getuid())->pw_dir;
    }

    if (!homedir) {
      /* rawpath is not empty, but we can't resolve homedir */
      return NULL;
    }

    newpath = (char *)malloc(sizeof(char) * PATH_MAX);
    if (!newpath) {
      /* unable to allocate memory */
      return NULL;
    }

    n = snprintf(newpath, PATH_MAX, "%s/%s", homedir, rawpath + 2);
    if (n <= 0) {
      /* filed to create full path */
      return NULL;
    }

    return newpath;
  }

  return rawpath;
}

void setup_logging() {
  /* do nothing if maintainer logs are disabled */
  if (!PCLOUD_MAINTAINER_LOGS) {
    log_set_quiet(true);
    return;
  }

  log_set_level(PCLOUD_LOG_LEVEL);
  const char *filename = resolve_filepath(PCLOUD_LOG_FILE);

  if (filename) {
    FILE* log_file = fopen(filename, "w");
    if (log_file) {
      log_add_fp(log_file, PCLOUD_LOG_LEVEL);
    }
    log_set_quiet(true);
  }
}
