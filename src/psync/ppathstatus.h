/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2016 Anton Titov <anton@pcloud.com>
 * (c) 2016 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */


#ifndef PCLOUD_PSYNC_PPATHSTATUS_H_
#define PCLOUD_PSYNC_PPATHSTATUS_H_

#include "psynclib.h"

typedef uint32_t psync_path_status_t;

#define PSYNC_PATH_STATUS_IN_SYNC     0
#define PSYNC_PATH_STATUS_IN_PROG     1
#define PSYNC_PATH_STATUS_PAUSED      2
#define PSYNC_PATH_STATUS_REMOTE_FULL 3
#define PSYNC_PATH_STATUS_LOCAL_FULL  4
#define PSYNC_PATH_STATUS_NOT_OURS    5
#define PSYNC_PATH_STATUS_NOT_FOUND   6

#define PSYNC_PATH_STATUS_BITS 5

#define PSYNC_PATH_STATUS_IS_ENCRYPTED   (1<<(PSYNC_PATH_STATUS_BITS))
#define PSYNC_PATH_STATUS_IS_IGNORED     (1<<(PSYNC_PATH_STATUS_BITS+1))

#define psync_path_status_get_status(x) ((x)&((1<<PSYNC_PATH_STATUS_BITS)-1))
#define psync_path_status_is_encryted(x) ((x)&PSYNC_PATH_STATUS_IS_ENCRYPTED)
#define psync_path_status_is_ignored(x) ((x)&PSYNC_PATH_STATUS_IS_IGNORED)

// All of the functions take sql write lock, so they can only be called under write lock or with no sql lock at all (but not with read lock taken).
// It is advised to call the function under the lock that made the changes, for reliability (and because recursive locks are almost free in the current
// implementation).
// All of the functions are designed to be very fast, with complexity proportional (only) to the path/folder depth (that is number of folders between the root
// and the folder, not number of sub folders) with additional cache to skip sql queries.

// Init does not allocate anything at this point and can be used as re-init (e.g. after unlink).
void psync_path_status_init();
void psync_path_status_reload_syncs();
void psync_path_status_clear_path_cache();
void psync_path_status_clear_sync_path_cache();

void psync_path_status_del_from_parent_cache(psync_folderid_t folderid);
void psync_path_status_drive_folder_changed(psync_folderid_t folderid);

void psync_path_status_folder_moved(psync_folderid_t folderid, psync_folderid_t old_parent_folderid, psync_folderid_t new_parent_folderid);
void psync_path_status_folder_deleted(psync_folderid_t folderid);

void psync_path_status_sync_folder_task_added_locked(psync_syncid_t syncid, psync_folderid_t localfolderid);
void psync_path_status_sync_folder_task_added(psync_syncid_t syncid, psync_folderid_t localfolderid);
void psync_path_status_sync_folder_task_completed(psync_syncid_t syncid, psync_folderid_t localfolderid);
void psync_path_status_sync_delete(psync_syncid_t syncid);
void psync_path_status_sync_folder_moved(psync_folderid_t folderid, psync_syncid_t old_syncid, psync_folderid_t old_parent_folderid,
                                         psync_syncid_t new_syncid, psync_folderid_t new_parent_folderid);
void psync_path_status_sync_folder_deleted(psync_syncid_t syncid, psync_folderid_t folderid);

psync_path_status_t psync_path_status_get(const char *path);

#endif  /* PCLOUD_PSYNC_PPATHSTATUS_H_ */
