/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "pcloudcc/psync/compat.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcloudcc/psync/sockets.h"
#include "logger.h"

char *psync_unix_socket_path() {
  char *path = (char *)malloc(sizeof(char) * PATH_MAX);
  if (!path) {
    return NULL;
  }

  int n;
  const char *runtime_dir = getenv("XDG_RUNTIME_DIR");
  if (runtime_dir) {
    n = snprintf(path, PATH_MAX, "%s/%s", runtime_dir,
                 POVERLAY_SOCKET_NAME);
  } else {
    runtime_dir = getenv("TMPDIR");
    if (runtime_dir) {
      n = snprintf(path, PATH_MAX, "%s/%s", runtime_dir,
                   POVERLAY_SOCKET_NAME);
    } else {
      n = snprintf(path, strlen(POVERLAY_SOCKET_NAME) + 5, "/tmp/%s",
                   POVERLAY_SOCKET_NAME);
    }
  }

  if (n <= 0) {
    log_fatal("failed to create a socket path");
    free(path);
    return NULL;
  }

  return path;
}
