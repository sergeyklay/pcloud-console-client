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

#ifndef PCLOUD_PSYNC_PMEMLOCK_H_
#define PCLOUD_PSYNC_PMEMLOCK_H_

#include <stdlib.h>

void psync_locked_init();

int psync_mem_lock(void *ptr, size_t size);
int psync_mem_unlock(void *ptr, size_t size);

void *psync_locked_malloc(size_t size);
void psync_locked_free(void *ptr);

#endif  /* PCLOUD_PSYNC_PMEMLOCK_H_ */
