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

#ifndef PCLOUD_PSYNC_PFSUPLOAD_H_
#define PCLOUD_PSYNC_PFSUPLOAD_H_

#include "psynclib.h"

void psync_fsupload_init();
void psync_fsupload_wake();
void psync_fsupload_stop_upload_locked(uint64_t taskid);
int psync_fsupload_in_current_small_uploads_batch_locked(uint64_t taskid);

#endif  /* PCLOUD_PSYNC_PFSUPLOAD_H_ */
