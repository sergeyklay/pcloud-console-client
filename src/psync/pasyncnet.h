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

#ifndef PCLOUD_PSYNC_PASYNCNET_H_
#define PCLOUD_PSYNC_PASYNCNET_H_

#include "psynclib.h"

#define PSYNC_ASYNC_ERR_FLAG_PERM        0x01 // the error is permanent(ish) and there is no reason to retry
#define PSYNC_ASYNC_ERR_FLAG_RETRY_AS_IS 0x02 // same request may succeed in the future if retried as is
#define PSYNC_ASYNC_ERR_FLAG_SUCCESS     0x04 // like no action performed because of no need - file already exists and so on

#define PSYNC_ASYNC_ERROR_NET       1
#define PSYNC_ASYNC_ERROR_FILE      2
#define PSYNC_ASYNC_ERROR_DISK_FULL 3
#define PSYNC_ASYNC_ERROR_IO        4
#define PSYNC_ASYNC_ERROR_CHECKSUM  5

#define PSYNC_SERVER_ERROR_TOO_BIG  102
#define PSYNC_SERVER_ERROR_NOT_MOD  104

typedef struct {
  uint64_t size;
  uint64_t hash;
  unsigned char sha1hex[40];
} psync_async_file_result_t;

typedef struct {
  uint32_t error;
  uint32_t errorflags;
  union {
    psync_async_file_result_t file;
  };
} psync_async_result_t;

typedef void (*psync_async_callback_t)(void *, psync_async_result_t *);

/* Important! The interface typically expect all passed pointers to be alive until the completion callback is called.
 */

void psync_async_stop();
int psync_async_download_file(psync_fileid_t fileid, const char *localpath, psync_async_callback_t cb, void *cbext);
int psync_async_download_file_if_changed(psync_fileid_t fileid, const char *localpath, uint64_t size, const void *sha1hex, psync_async_callback_t cb, void *cbext);

#endif  /* PCLOUD_PSYNC_PASYNCNET_H_ */
