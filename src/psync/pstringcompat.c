/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include <stddef.h>
#include <string.h>

#include "config.h"

#include "pcloudcc/psync/stringcompat.h"

#if !defined(HAVE_STRLCPY) || !HAVE_STRLCPY
size_t PSYNC_WEAK strlcpy(char *dst, const char *src, size_t size) {
  size_t ret = strlen(src);

  if (size) {
    size_t len = (ret >= size) ? size - 1 : ret;
    memcpy(dst, src, len);
    dst[len] = '\0';
  }

  return ret;
}
#endif  /* !defined(HAVE_STRLCPY) || !HAVE_STRLCPY */
