/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_SOCKETS_H_
#define PCLOUD_PSYNC_SOCKETS_H_

/*! \file sockets.h
 *  \brief Provides common socket utils and named constants.
 *  \note This header should be used on UNIX/Linux only.
 */

/*! \brief Buffer size in bytes to read from the socket. */
#define POVERLAY_BUFSIZE 512

/*! \brief Protocol to be used with the socket. */
#define POVERLAY_PROTOCOL 0

/*! \brief The listen port for IP socket. */
#define POVERLAY_PORT 8989

/*! \brief The file name for Unix Domain Socket. */
#define POVERLAY_SOCKET_NAME "pcloud.sock"

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Get full path to Unix Domain Socket. */
char *psync_unix_socket_path();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_PSYNC_SOCKETS_H_ */
