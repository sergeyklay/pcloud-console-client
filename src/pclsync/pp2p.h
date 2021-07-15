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

#ifndef PCLOUD_PCLSYNC_PP2P_H_
#define PCLOUD_PCLSYNC_PP2P_H_

#include "psynclib.h"

void psync_p2p_init();
void psync_p2p_change();
int psync_p2p_check_download(psync_fileid_t fileid, const unsigned char *filehashhex, uint64_t fsize, const char *filename);

#endif  /* PCLOUD_PCLSYNC_PP2P_H_ */
