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

#ifndef PCLOUD_PCLSYNC_PPAGECACHE_H_
#define PCLOUD_PCLSYNC_PPAGECACHE_H_

#include "pfs.h"

typedef struct {
  uint64_t offset;
  uint64_t size;
  char *buf;
} psync_pagecache_read_range;

void psync_pagecache_init();
int psync_pagecache_flush();
int psync_pagecache_read_modified_locked(psync_openfile_t *of, char *buf, uint64_t size, uint64_t offset);
int psync_pagecache_read_unmodified_locked(psync_openfile_t *of, char *buf, uint64_t size, uint64_t offset);
int psync_pagecache_read_unmodified_encrypted_locked(psync_openfile_t *of, char *buf, uint64_t size, uint64_t offset);
int psync_pagecache_readv_locked(psync_openfile_t *of, psync_pagecache_read_range *ranges, int cnt);
void psync_pagecache_creat_to_pagecache(uint64_t taskid, uint64_t hash, int onthisthread);
void psync_pagecache_modify_to_pagecache(uint64_t taskid, uint64_t hash, uint64_t oldhash);
int psync_pagecache_have_all_pages_in_cache(uint64_t hash, uint64_t size);
int psync_pagecache_copy_all_pages_from_cache_to_file_locked(psync_openfile_t *of, uint64_t hash, uint64_t size);
int psync_pagecache_lock_pages_in_cache();
void psync_pagecache_unlock_pages_from_cache();
void psync_pagecache_resize_cache();
uint64_t psync_pagecache_free_from_read_cache(uint64_t size);
void psync_pagecache_clean_cache();
void psync_pagecache_reopen_read_cache();
void psync_pagecache_clean_read_cache();
int psync_pagecache_move_cache(const char *path);

#endif  /* PCLOUD_PCLSYNC_PPAGECACHE_H_ */
