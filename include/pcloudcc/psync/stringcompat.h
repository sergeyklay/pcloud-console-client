/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUDCC_PSYNC_STRINGCOMPAT_H_
#define PCLOUDCC_PSYNC_STRINGCOMPAT_H_

/*! \file stringcompat.h
 *  \brief Some extra string manipulation functions not present in all
 *         standard C runtimes.
 */

#include <pcloudcc/psync/compiler.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_STRLCPY
/*! \brief Size-bounded string copying.
 *
 * Is designed to be safer, more consistent, and less error prone replacements
 * for strncpy(3). Unlike strncpy(), strlcpy() take the full \a size of the
 * buffer (not just the length) and guarantee to NUL-terminate the result
 * (as long as \a size is larger than 0)
 *
 * If libc has strlcpy() then that version will override this implementation.
 *
 * \arg dest Where to copy the string to
 * \arg src Where to copy the string from
 * \arg size Size of destination buffer
 *
 * \note A byte for the NUL should be included in \a size. Also note that
 *       strlcpy() only operate on true "C" strings. This means that for
 *       \a src must be NUL-terminated.
 */
size_t strlcpy(char *dst, const char *src, size_t size);
#endif /* HAVE_STRLCPY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PCLOUDCC_PSYNC_STRINGCOMPAT_H_ */
