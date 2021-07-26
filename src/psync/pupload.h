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

#ifndef PCLOUD_PSYNC_PUPLOAD_H_
#define PCLOUD_PSYNC_PUPLOAD_H_

#include "psynclib.h"

void psync_upload_init();
void psync_upload_inc_uploads();
void psync_upload_dec_uploads();
void psync_upload_dec_uploads_cnt(uint32_t cnt);
void psync_upload_add_bytes_uploaded(uint64_t bytes);
void psync_upload_sub_bytes_uploaded(uint64_t bytes);
void psync_wake_upload();
void psync_delete_upload_tasks_for_file(psync_fileid_t localfileid);
void psync_stop_sync_upload(psync_syncid_t syncid);
void psync_stop_all_upload();

#endif /* PCLOUD_PSYNC_PUPLOAD_H_ */
