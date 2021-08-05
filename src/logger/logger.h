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

#include <string.h>
#include <log.h>

#include "pcloudcc/compat/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PCLOUD_LOG_LEVEL
#define PCLOUD_LOG_LEVEL LOG_DEBUG
#endif

#ifndef PCLOUD_MAINTAINER_LOGS
#define PCLOUD_MAINTAINER_LOGS false
#endif

#ifndef PCLOUD_LOG_FILE
#define PCLOUD_LOG_FILE NULL
#endif

#ifndef PCLOUD_SOURCE_PATH
/*! \brief Represents the base project path. */
#define PCLOUD_SOURCE_PATH ""
#endif

#ifndef PCLOUD_SOURCE_PATH_SIZE
/*! \brief Represents string lengths of the base project path. */
#define PCLOUD_SOURCE_PATH_SIZE 0
#endif

/*! \brief Expand relative path of the current input file.
 *
 * This macro expands to the relative path of the current input file, in the
 * form of a C string constant.  For example, processing
 * "/home/egrep/work/pcloudcc/include/myheader.h" would set this macro to
 * "include/myheader.h".
 */
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __FILENAME__
#if __has_builtin(__builtin_strstr)
#define __FILENAME__                                     \
  __builtin_strstr(__FILE__, PCLOUD_SOURCE_PATH)         \
      ? __builtin_strstr(__FILE__, PCLOUD_SOURCE_PATH) + \
            PCLOUD_SOURCE_PATH_SIZE                      \
      : __FILE__
#else
#define __FILENAME__                                                   \
  strstr(__FILE__, PCLOUD_SOURCE_PATH)                                 \
      ? strstr(__FILE__, PCLOUD_SOURCE_PATH) + PCLOUD_SOURCE_PATH_SIZE \
      : __FILE__
#endif
#endif /* __FILENAME__ */
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#undef log_trace
#undef log_debug
#undef log_info
#undef log_warn
#undef log_error
#undef log_fatal

/* Redefine macro to use relative paths */
#define log_trace(...) log_log(LOG_TRACE, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__)

void log_lock(bool lock, void *udata);
void setup_logging();

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif  /* PCLOUD_LOGGER_H_ */
