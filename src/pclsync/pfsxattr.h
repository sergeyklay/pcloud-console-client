/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2016 Ivan Stoev <ivan.stoev@pcloud.com>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PCLSYNC_PFSXATTR_H_
#define PCLOUD_PCLSYNC_PFSXATTR_H_

#include "pcloudcc/psync/compat.h"
#include "psynclib.h"

#ifdef P_OS_MACOSX
#define PFS_XATTR_IGN , uint32_t ign
#else
#define PFS_XATTR_IGN
#endif

int psync_fs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags PFS_XATTR_IGN);
int psync_fs_getxattr(const char *path, const char *name, char *value, size_t size PFS_XATTR_IGN);
int psync_fs_listxattr(const char *path, char *list, size_t size);
int psync_fs_removexattr(const char *path, const char *name);

void psync_fs_file_deleted(psync_fileid_t fileid);
void psync_fs_folder_deleted(psync_folderid_t folderid);
void psync_fs_task_deleted(uint64_t taskid);

void psync_fs_task_to_file(uint64_t taskid, psync_fileid_t fileid);
void psync_fs_task_to_folder(uint64_t taskid, psync_folderid_t folderid);
void psync_fs_static_to_task(uint64_t statictaskid, uint64_t taskid);
void psync_fs_file_to_task(psync_fileid_t fileid, uint64_t taskid);

#endif  /* PCLOUD_PCLSYNC_PFSXATTR_H_ */
