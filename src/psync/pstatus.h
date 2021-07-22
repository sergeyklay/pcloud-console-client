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

#ifndef PCLOUD_PSYNC_PSTATUS_H_
#define PCLOUD_PSYNC_PSTATUS_H_

#include <stdint.h>

#define PSTATUS_NUM_STATUSES 6

#define PSTATUS_TYPE_RUN       0
#define PSTATUS_TYPE_ONLINE    1
#define PSTATUS_TYPE_AUTH      2
#define PSTATUS_TYPE_ACCFULL   3
#define PSTATUS_TYPE_DISKFULL  4
#define PSTATUS_TYPE_LOCALSCAN 5

#define PSTATUS_INVALID   0

#define PSTATUS_RUN_RUN   1
#define PSTATUS_RUN_PAUSE 2
#define PSTATUS_RUN_STOP  4

#define PSTATUS_ONLINE_CONNECTING 1
#define PSTATUS_ONLINE_SCANNING   2
#define PSTATUS_ONLINE_ONLINE     4
#define PSTATUS_ONLINE_OFFLINE    8

#define PSTATUS_AUTH_PROVIDED 1
#define PSTATUS_AUTH_REQUIRED 2
#define PSTATUS_AUTH_MISMATCH 4
#define PSTATUS_AUTH_BADLOGIN 8
#define PSTATUS_AUTH_BADTOKEN 16
#define PSTATUS_AUTH_EXPIRED  32
#define PSTATUS_AUTH_TFAERR   64

#define PSTATUS_ACCFULL_QUOTAOK   1
#define PSTATUS_ACCFULL_OVERQUOTA 2

#define PSTATUS_DISKFULL_OK   1
#define PSTATUS_DISKFULL_FULL 2

#define PSTATUS_LOCALSCAN_SCANNING 1
#define PSTATUS_LOCALSCAN_READY    2

#define PSTATUS_COMBINE(type, statuses) (((type)<<24)+(statuses))

void psync_status_init();
void psync_status_recalc_to_download();
void psync_status_recalc_to_download_async();
void psync_status_recalc_to_upload();
void psync_status_recalc_to_upload_async();
uint32_t psync_status_get(uint32_t statusid);
void psync_set_status(uint32_t statusid, uint32_t status);
void psync_wait_status(uint32_t statusid, uint32_t status);
void psync_terminate_status_waiters();
void psync_wait_statuses_array(const uint32_t *combinedstatuses, uint32_t cnt);
void psync_wait_statuses(uint32_t first, ...);
int psync_statuses_ok_array(const uint32_t *combinedstatuses, uint32_t cnt);

void psync_status_set_download_speed(uint32_t speed);
void psync_status_set_upload_speed(uint32_t speed);;

void psync_status_send_update();

static inline int psync_status_is_offline(){
  return psync_status_get(PSTATUS_TYPE_ONLINE)==PSTATUS_ONLINE_OFFLINE;
}
#endif  /* PCLOUD_PSYNC_PSTATUS_H_ */
