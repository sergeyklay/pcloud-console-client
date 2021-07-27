/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PUBLICLINKS_H_
#define PCLOUD_PSYNC_PUBLICLINKS_H_

#include "psynclib.h"

int64_t do_psync_file_public_link(const char *path, int64_t* plinkid /*OUT*/, char **code /*OUT*/, char **err /*OUT*/,  /*OUT*/uint64_t expire, int maxdownloads, int maxtraffic);
int64_t do_psync_screenshot_public_link(const char *path, int hasdelay, uint64_t delay, char **code /*OUT*/, char **err /*OUT*/);
int64_t do_psync_folder_public_link(const char *path, char **code /*OUT*/, char **err /*OUT*/, uint64_t expire, int maxdownloads, int maxtraffic);
int64_t do_psync_tree_public_link(const char *linkname, const char *root, char **folders, int numfolders, char **files, int numfiles, char **code /*OUT*/, char **err /*OUT*/,
                                  uint64_t expire, int maxdownloads, int maxtraffic);
plink_info_list_t *do_psync_list_links(char **err /*OUT*/);
int do_psync_delete_link(int64_t linkid, char **err /*OUT*/);

int64_t do_psync_upload_link(const char *path, const char *comment, char **code /*OUT*/, char **err /*OUT*/, uint64_t expire, int maxspace, int maxfiles);
int do_psync_delete_upload_link(int64_t uploadlinkid, char **err /*OUT*/);

plink_contents_t *do_show_link(const char *code, char **err /*OUT*/);

void cache_links_all();
int cache_upload_links(char **err /*OUT*/);
int cache_links(char **err /*OUT*/);

int do_delete_all_folder_links(psync_folderid_t folderid, char**err);
int do_delete_all_file_links(psync_fileid_t fileid, char**err);

#endif /* PCLOUD_PSYNC_PUBLICLINKS_H_ */
