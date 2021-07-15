/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PCLSYNC_POVERLAY_H_
#define PCLOUD_PCLSYNC_POVERLAY_H_

#include "psynclib.h"

typedef struct _message {
  uint32_t type;
  uint64_t length;
  char value[];
} message;

extern int overlays_running;
extern int callbacks_running;

void overlay_main_loop();
void instance_thread(void* payload);
void get_answer_to_request(message *request, message *replay);
void psync_stop_overlays();
void psync_start_overlays();
void psync_stop_overlay_callbacks();
void psync_start_overlay_callbacks();
int psync_overlays_running();
int psync_ovr_callbacks_running();

void init_overlay_callbacks();
int psync_add_overlay_callback(int id, poverlay_callback callback);


#endif  /* PCLOUD_PCLSYNC_POVERLAY_H_ */
