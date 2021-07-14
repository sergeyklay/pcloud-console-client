/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_LOGGER_H_
#define PCLOUD_LOGGER_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <log.h>

#define PCLOUD_LOG_LEVEL LOG_DEBUG

#ifndef PCLOUD_MAINTAINER_LOGS
#define PCLOUD_MAINTAINER_LOGS false
#endif

#ifndef PCLOUD_LOG_FILE
#define PCLOUD_LOG_FILE NULL
#endif

pthread_mutex_t MUTEX_LOG;

void log_lock(bool lock, void *udata);
void setup_logging();

#ifdef __cplusplus
}
#endif

#endif  /* PCLOUD_LOGGER_H_ */
