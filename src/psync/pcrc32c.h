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

#ifndef PCLOUD_PSYNC_PCRC32_H_
#define PCLOUD_PSYNC_PCRC32_H_

#include <stdint.h>
#include <stddef.h>

#define PSYNC_CRC_INITIAL 0

#define PSYNC_FAST_HASH256_LEN    32
#define PSYNC_FAST_HASH256_HEXLEN 64

#define PSYNC_FAST_HASH256_BLOCK_LEN 64

typedef struct{
  uint64_t state[6];
  uint64_t length;
  union {
    uint64_t buff64[PSYNC_FAST_HASH256_BLOCK_LEN/sizeof(uint64_t)];
    unsigned char buff[PSYNC_FAST_HASH256_BLOCK_LEN];
  };
} psync_fast_hash256_ctx;

uint32_t psync_crc32c(uint32_t crc, const void *ptr, size_t len);

/* psync_fast_hash256 is supposed to be fast, non-cryptographic strength, non-collision resistant hash function
 * with large output. Think of it as CRC32 but faster, with 256bit output and for cases that you can't live with
 * the chances of CRC32 _random_ collision. It is intended for large inputs, finalization is relatively expensive.
 */

void psync_fast_hash256_init(psync_fast_hash256_ctx *ctx);
void psync_fast_hash256_init_seed(psync_fast_hash256_ctx *ctx, const void *seed, size_t seedlen);
void psync_fast_hash256_update(psync_fast_hash256_ctx *ctx, const void *data, size_t len);
void psync_fast_hash256_final(void *hash, psync_fast_hash256_ctx *ctx);

#endif  /* PCLOUD_PSYNC_PCRC32_H_ */
