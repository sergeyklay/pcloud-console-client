/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "config.h" /* HAVE_STRLCPY */

#include <stddef.h> /* size_t */
#include <string.h> /* memcpy, strlen */

#include "pcloudcc/compat/string.h" /* strlcpy */
#include "pcloudcc/compat/compiler.h" /* PSYNC_WEAK */

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
