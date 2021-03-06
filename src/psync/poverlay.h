/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_POVERLAY_H_
#define PCLOUD_PSYNC_POVERLAY_H_

#include "pcloudcc/psync/overlay.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int overlays_running;
extern int callbacks_running;

/*! \brief The main overlay loop.
 *
 * The main loop creates an instance of a UNIX socket and then waits for a
 * client to connect to it. When the client connects, a thread is created to
 * handle communications with that client, and this loop is free to wait for
 * the next client connect request. It is an infinite loop.
 */
void overlay_main_loop();

void instance_thread(void* payload);

void psync_stop_overlays();
void psync_start_overlays();
void psync_stop_overlay_callbacks();
void psync_start_overlay_callbacks();
int psync_overlays_running();
int psync_ovr_callbacks_running();

void init_overlay_callbacks();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_PSYNC_POVERLAY_H_ */
