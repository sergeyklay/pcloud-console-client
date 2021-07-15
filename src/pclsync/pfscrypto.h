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

#ifndef PCLOUD_PCLSYNC_PFSCRYPTO_H_
#define PCLOUD_PCLSYNC_PFSCRYPTO_H_

#include "pfs.h"
#include "pcloudcrypto.h"

#define PSYNC_CRYPTO_MAX_SECTORID (UINT32_MAX-1)
#define PSYNC_CRYPTO_INVALID_SECTORID (UINT32_MAX)

#define PSYNC_CRYPTO_HASH_TREE_SECTORS (PSYNC_CRYPTO_SECTOR_SIZE/PSYNC_CRYPTO_AUTH_SIZE)

typedef uint32_t psync_crypto_sectorid_t;
typedef int32_t psync_crypto_sectorid_diff_t;

typedef struct {
  psync_tree tree;
  psync_crypto_sectorid_t sectorid;
  uint32_t logoffset;
  psync_crypto_sector_auth_t auth;
} psync_sector_inlog_t;

typedef psync_crypto_sector_auth_t psync_crypto_auth_sector_t[PSYNC_CRYPTO_HASH_TREE_SECTORS];

int psync_fs_crypto_init_log(psync_openfile_t *of);
int psync_fs_crypto_read_newfile_locked(psync_openfile_t *of, char *buf, uint64_t size, uint64_t offset);
int psync_fs_crypto_write_newfile_locked(psync_openfile_t *of, const char *buf, uint64_t size, uint64_t offset);
int psync_fs_crypto_read_modified_locked(psync_openfile_t *of, char *buf, uint64_t size, uint64_t offset);
int psync_fs_crypto_write_modified_locked(psync_openfile_t *of, const char *buf, uint64_t size, uint64_t offset);
int psync_fs_crypto_ftruncate(psync_openfile_t *of, uint64_t size);
int psync_fs_crypto_flush_file(psync_openfile_t *of);

psync_crypto_sectorid_t psync_fs_crypto_data_sectorid_by_sectorid(psync_crypto_sectorid_t sectorid);
void psync_fs_crypto_offsets_by_plainsize(uint64_t size, psync_crypto_offsets_t *offsets);
uint64_t psync_fs_crypto_plain_size(uint64_t cryptosize);
uint64_t psync_fs_crypto_crypto_size(uint64_t plainsize);
void psync_fs_crypto_get_auth_sector_off(psync_crypto_sectorid_t sectorid, uint32_t level, psync_crypto_offsets_t *offsets,
                                         uint64_t *offset, uint32_t *size, uint32_t *authid);

void psync_fs_crypto_check_logs();

#endif  /* PCLOUD_PCLSYNC_PFSCRYPTO_H_ */
