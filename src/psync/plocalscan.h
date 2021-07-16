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

#ifndef PCLOUD_PSYNC_PLOCALSCAN_H_
#define PCLOUD_PSYNC_PLOCALSCAN_H_

void psync_localscan_init();
void psync_wake_localscan();
void psync_restart_localscan();
void psync_stop_localscan();
void psync_resume_localscan();

#endif  /* PCLOUD_PSYNC_PLOCALSCAN_H_ */
