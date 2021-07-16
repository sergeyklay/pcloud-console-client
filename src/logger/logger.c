/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include <pthread.h>

#include "logger.h"

void log_lock(bool lock, void* udata) {
  pthread_mutex_t *LOCK = (pthread_mutex_t*)(udata);
  if (lock)
    pthread_mutex_lock(LOCK);
  else
    pthread_mutex_unlock(LOCK);
}

void setup_logging() {
  /* Do nothing if maintainer logs are disabled */
  if (!PCLOUD_MAINTAINER_LOGS) {
    log_set_quiet(true);
    return;
  }

  /* If file name is set log to the file, otherwise to the stdout */
  if (PCLOUD_LOG_FILE) {
    FILE* log_file = fopen(PCLOUD_LOG_FILE, "w");
    log_add_fp(log_file, PCLOUD_LOG_LEVEL);
    log_set_quiet(true);
  }
}
