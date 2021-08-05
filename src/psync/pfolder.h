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

#ifndef PCLOUD_PSYNC_PFOLDER_H_
#define PCLOUD_PSYNC_PFOLDER_H_

#include "pcloudcc/compat/compiler.h"
#include "psynclib.h"

#define PSYNC_INVALID_FOLDERID ((psync_folderid_t)-1)
#define PSYNC_INVALID_PATH NULL

#define PSYNC_FOLDER_FLAG_ENCRYPTED 1
#define PSYNC_FOLDER_FLAG_INVISIBLE 2

psync_folderid_t psync_get_folderid_by_path(const char *path) PSYNC_NONNULL(1) PSYNC_PURE;
psync_folderid_t psync_get_folderid_by_path_or_create(const char *path) PSYNC_NONNULL(1);
char *psync_get_path_by_folderid(psync_folderid_t folderid, size_t *retlen);
char *psync_get_path_by_folderid_sep(psync_folderid_t folderid, const char *sep, size_t *retlen);
char *psync_get_path_by_fileid(psync_fileid_t fileid, size_t *retlen);
char *psync_local_path_for_local_folder(psync_folderid_t localfolderid, psync_syncid_t syncid, size_t *retlen);
char *psync_local_path_for_local_file(psync_fileid_t localfileid, size_t *retlen);
//char *psync_local_path_for_remote_folder(psync_folderid_t folderid, psync_syncid_t syncid, size_t *retlen);
//char *psync_local_path_for_remote_file(psync_fileid_t fileid, psync_syncid_t syncid, size_t *retlen);
//char *psync_local_path_for_remote_file_or_folder_by_name(psync_folderid_t parentfolderid, const char *filename, psync_syncid_t syncid, size_t *retlen);
pfolder_list_t *psync_list_remote_folder(psync_folderid_t folderid, psync_listtype_t listtype);
pfolder_list_t *psync_list_local_folder(const char *path, psync_listtype_t listtype) PSYNC_NONNULL(1);
pentry_t *psync_folder_stat_path(const char *remotepath);

psync_folder_list_t *psync_list_get_list();

#endif /* PCLOUD_PSYNC_PFOLDER_H_ */
