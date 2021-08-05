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

#ifndef PCLOUD_PSYNC_PFILEOPS_H_
#define PCLOUD_PSYNC_PFILEOPS_H_

#include "pcloudcc/compat/compiler.h"

#include "papi.h"
#include "psettings.h"
#include "psynclib.h"

#define PSYNC_INVALID_FOLDERID ((psync_folderid_t)-1)
#define PSYNC_INVALID_PATH NULL

static inline uint64_t psync_get_permissions(const binresult *meta) {
  const binresult *manage = psync_check_result(meta, "canmanage", PARAM_BOOL);
  const binresult *read = psync_find_result(meta, "canread", PARAM_BOOL);
  const binresult *modify = psync_find_result(meta, "canmodify", PARAM_BOOL);
  const binresult *delete = psync_find_result(meta, "candelete", PARAM_BOOL);
  const binresult *create = psync_find_result(meta, "cancreate", PARAM_BOOL);

  return (read && read->num ? PSYNC_PERM_READ : 0) +
      (modify && modify->num ? PSYNC_PERM_MODIFY : 0) +
      (delete &&delete->num ? PSYNC_PERM_DELETE : 0) +
      (create && create->num ? PSYNC_PERM_CREATE : 0) +
      (manage && manage->num ? PSYNC_PERM_MANAGE : 0);
}

void psync_ops_create_folder_in_db(const binresult *meta);
void psync_ops_update_folder_in_db(const binresult *meta);
void psync_ops_delete_folder_from_db(const binresult *meta);
void psync_ops_create_file_in_db(const binresult *meta);
void psync_ops_update_file_in_db(const binresult *meta);
void psync_ops_delete_file_from_db(const binresult *meta);

#endif /* PCLOUD_PSYNC_PFILEOPS_H_ */
