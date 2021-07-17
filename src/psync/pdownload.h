/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013 Anton Titov <anton@pcloud.com>
 * (c) 2013 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PDOWNLOAD_H_
#define PCLOUD_PSYNC_PDOWNLOAD_H_

#include "psynclib.h"
#include "psettings.h"
#include "pssl.h"

typedef unsigned char psync_hex_hash[PSYNC_HASH_DIGEST_HEXLEN];

typedef struct {
  size_t hashcnt;
  psync_hex_hash hashes[];
} downloading_files_hashes;

void psync_download_init();
void psync_wake_download();
void psync_delete_download_tasks_for_file(psync_fileid_t fileid, psync_syncid_t syncid, int deltemp);
void psync_stop_file_download(psync_fileid_t fileid, psync_syncid_t syncid);
void psync_stop_sync_download(psync_syncid_t syncid);
void psync_stop_all_download();
downloading_files_hashes *psync_get_downloading_hashes();

#endif  /* PCLOUD_PSYNC_PDOWNLOAD_H_ */
