/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2014 Anton Titov <anton@pcloud.com>
 * (c) 2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PCACHE_H_
#define PCLOUD_PSYNC_PCACHE_H_

#include <time.h>

typedef void (*psync_cache_free_callback)(void *);

void psync_cache_init();
void *psync_cache_get(const char *key);
int psync_cache_has(const char *key);
void psync_cache_add(const char *key, void *ptr, time_t freeafter, psync_cache_free_callback freefunc, uint32_t maxkeys);
void psync_cache_add_free(char *key, void *ptr, time_t freeafter, psync_cache_free_callback freefunc, uint32_t maxkeys);
void psync_cache_del(const char *key);
void psync_cache_clean_all();
void psync_cache_clean_starting_with(const char *prefix);
void psync_cache_clean_starting_with_one_of(const char **prefixes, size_t cnt);

#endif  /* PCLOUD_PSYNC_PCACHE_H_ */
