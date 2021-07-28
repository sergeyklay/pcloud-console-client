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

#ifndef PCLOUD_PSYNC_PSYNCER_H_
#define PCLOUD_PSYNC_PSYNCER_H_

#include "psynclib.h"

void psync_syncer_init();
void psync_syncer_new(psync_syncid_t syncid);

void psync_increase_local_folder_taskcnt(psync_folderid_t lfolderid);
void psync_decrease_local_folder_taskcnt(psync_folderid_t lfolderid);
psync_folderid_t psync_create_local_folder_in_db(psync_syncid_t syncid, psync_folderid_t folderid, psync_folderid_t localparentfolderid, const char *name);
void psync_add_folder_for_downloadsync(psync_syncid_t syncid, psync_synctype_t synctype, psync_folderid_t folderid, psync_folderid_t lfoiderid);

void psync_add_folder_to_downloadlist(psync_folderid_t folderid);
void psync_del_folder_from_downloadlist(psync_folderid_t folderid);
void psync_clear_downloadlist();
int psync_is_folder_in_downloadlist(psync_folderid_t folderid);

int psync_str_is_prefix(const char *str1, const char *str2);
void psync_syncer_check_delayed_syncs();

#endif /* PCLOUD_PSYNC_PSYNCER_H_ */
