/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUDCC_COMPAT_LIMITS_H_
#define PCLOUDCC_COMPAT_LIMITS_H_

/*! \file limits.h
 *  \brief Some limits are not unified in all standard C runtimes.
 */

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Maximum length of file name including null */
#ifdef _MSC_VER
#include <windows.h>
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif /* PATH_MAX */
#endif /* _MSC_VER */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PCLOUDCC_COMPAT_LIMITS_H_ */
