/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 Anton Titov <anton@pcloud.com>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PDIFF_H_
#define PCLOUD_PSYNC_PDIFF_H_

#include "pcloudcc/psync/accountevents.h"
#include "papi.h"

void psync_diff_init();
void psync_diff_lock();
void psync_diff_unlock();
void psync_diff_wake();
void psync_diff_create_file(const binresult *meta);
void psync_diff_update_file(const binresult *meta);
void psync_diff_delete_file(const binresult *meta);
void psync_diff_update_folder(const binresult *meta);
void psync_diff_delete_folder(const binresult *meta);

void do_register_account_events_callback(paccount_cache_callback_t callback);

#endif  /* PCLOUD_PSYNC_PDIFF_H_ */
