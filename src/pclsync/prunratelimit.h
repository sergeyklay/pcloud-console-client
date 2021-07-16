/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2015 Anton Titov <anton@pcloud.com>
 * (c) 2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PCLSYNC_PRUNRATELIMIT_H_
#define PCLOUD_PCLSYNC_PRUNRATELIMIT_H_

#include <stdint.h>

typedef void (*psync_run_ratelimit_callback0)();

void psync_run_ratelimited(const char *name, psync_run_ratelimit_callback0 call, uint32_t minintervalsec, int runinthread);

#endif  /* PCLOUD_PCLSYNC_PRUNRATELIMIT_H_ */
