/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_TOOLS_SOCKETS_H_
#define PCLOUD_TOOLS_SOCKETS_H_

#include "pcloudcc/psync/compat.h"

#define POVERLAY_BUFSIZE 512
#define POVERLAY_PROTOCOL 0
#define POVERLAY_PORT 8989
#define POVERLAY_SOCKET_NAME "pcloud.sock"

#ifdef __cplusplus
extern "C" {
#endif

char * create_socket_path();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_TOOLS_SOCKETS_H_ */
