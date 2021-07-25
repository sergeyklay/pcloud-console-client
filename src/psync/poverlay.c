/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "pcloudcc/psync/compat.h"

#include "plibs.h"
#include "poverlay.h"
#include "ppathstatus.h"
#include "pcache.h"

int overlays_running = 1;
int callbacks_running = 1;

#ifdef P_OS_WINDOWS
#include "poverlay-windows.c"
#elif defined P_OS_POSIX
#include "poverlay-posix.c"
#else
void overlay_main_loop(void) {}
void instance_thread(void* payload) {}
#endif /* P_OS_WINDOWS */

poverlay_callback *callbacks;
static int callbacks_size = 15;
static const int callbacks_lower_band = 20;

int psync_add_overlay_callback(int id, poverlay_callback callback) {
  poverlay_callback *callbacks_old = callbacks;
  int callbacks_size_old = callbacks_size;
  if (id < callbacks_lower_band)
    return -1;

  if (id > (callbacks_lower_band + callbacks_size)) {
     callbacks_size = id - callbacks_lower_band + 1;
     init_overlay_callbacks();
     memcpy(callbacks, callbacks_old,
            callbacks_size_old * sizeof(poverlay_callback));
     psync_free(callbacks_old);
  }

  callbacks[id - callbacks_lower_band] = callback;
  return 0;
}

void init_overlay_callbacks() {
  callbacks = (poverlay_callback *) psync_malloc(sizeof(poverlay_callback) * callbacks_size);
  memset(callbacks, 0, sizeof(poverlay_callback) * callbacks_size);
}

void psync_stop_overlays() {
  overlays_running = 0;
}

void psync_start_overlays() {
  overlays_running = 1;
}

void psync_stop_overlay_callbacks() {
  callbacks_running = 0;
}

void psync_start_overlay_callbacks() {
  callbacks_running = 1;
}

void get_answer_to_request(message *request, message *response) {
  psync_path_status_t stat = PSYNC_PATH_STATUS_NOT_OURS;
  memcpy(response->value, "Ok.", 4);
  response->length = sizeof(message) + 4;
  int max_band;

  if (request->type < 20 ) {
    if (overlays_running)
      stat = psync_path_status_get(request->value);
    switch (psync_path_status_get_status(stat)) {
      case PSYNC_PATH_STATUS_IN_SYNC:
        response->type = 10;
        break;
      case PSYNC_PATH_STATUS_IN_PROG:
        response->type = 12;
        break;
      case PSYNC_PATH_STATUS_PAUSED:
      case PSYNC_PATH_STATUS_REMOTE_FULL:
      case PSYNC_PATH_STATUS_LOCAL_FULL:
        response->type = 11;
        break;
      default:
        response->type = 13;
        memcpy(response->value, "No.", 4);
    }

    return; /* exit */
  }

  max_band = callbacks_lower_band + callbacks_size;
  if (psync_overlays_running() && (request->type < max_band)) {
    uint32_t ind = request->type - 20;
    int ret;
    message *rep = NULL;

    if (callbacks[ind]) {
      if ((ret = callbacks[ind](request->value, rep)) == 0) {
        if (rep) {
          psync_free(response);
          response = rep;
        } else
          response->type = 0;
      } else {
        response->type = ret;
        memcpy(response->value, "No.", 4);
      }
    } else {
      response->type = 13;
      memcpy(response->value, "No callback with this id registered.", 37);
      response->length = sizeof(message)+37;
    }

    return;  /* exit */
  }

  response->type = 13;
  memcpy(response->value, "Invalid type.", 14);
  response->length = sizeof(message) + 14;
}

int psync_overlays_running() {
  return overlays_running;
}

int psync_ovr_callbacks_running() {
  return callbacks_running;
}
