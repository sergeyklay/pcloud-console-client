/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_OVERLAY_CLIENT_H_
#define PCLOUD_OVERLAY_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum overlay_file_state_ {
  FILE_STATE_IN_SYNC = 0,
  FILE_STATE_NO_SYNC,
  FILE_STATE_IN_PROGRESS,
  FILE_STATE_INVALID
} overlay_file_state_t;

typedef enum overlay_command_ {
  STARTCRYPTO = 20,
  STOPCRYPTO,
  FINALIZE,
  LISTSYNC,
  ADDSYNC,
  STOPSYNC
} overlay_command_t;

int query_state(overlay_file_state_t *state, char *path);

int send_call(overlay_command_t cmd, const char *path, int *ret, char **out);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_OVERLAY_CLIENT_H_ */
