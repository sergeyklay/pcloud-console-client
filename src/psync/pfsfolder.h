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

#ifndef PCLOUD_PSYNC_PFSFOLDER_H_
#define PCLOUD_PSYNC_PFSFOLDER_H_

#include <stdint.h>
#include <time.h>

typedef int64_t psync_fsfolderid_t;
typedef int64_t psync_fsfileid_t;

#define PSYNC_INVALID_FSFOLDERID INT64_MIN

typedef struct {
  psync_fsfolderid_t folderid;
  const char *name;
  uint32_t shareid;
  uint16_t permissions;
  uint16_t flags;
} psync_fspath_t;

psync_fspath_t *psync_fsfolder_resolve_path(const char *path);
psync_fsfolderid_t psync_fsfolderid_by_path(const char *path, uint32_t *pflags);
int psync_fsfolder_crypto_error();

#endif  /* PCLOUD_PSYNC_PFSFOLDER_H_ */
