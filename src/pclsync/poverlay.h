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

typedef struct message_ {
  uint32_t type;
  uint64_t length;
  char value[];
} message;

extern int overlays_running;
extern int callbacks_running;

/*! \brief The file manager extension callback.
 *
 * Callback to be registered to be called from file manager extension.
 */
typedef int (*poverlay_callback)(const char *path, void *rep);

/*! \brief Register a file manager callback.
 *
 * Registers file manager extension callback that will be called when packet
 * with id equals to the give one had arrived from extension.  The id must be
 * over or equal to 20 or -1 will be returned.  There is a hard coded maximum
 * of menu items on some OS-s so maximum of 15 ids are available.  Value of -2
 * is returned when id grater then 35 and 0 returned on success.
 *
 * \warning These function are not thread-safe. Use them in single thread or
 *          synchronize.
 */
int psync_add_overlay_callback(int id, poverlay_callback callback);

/*! \brief The main overlay loop.
 *
 * The main loop creates an instance of the named pipe on Windows, or a UNIX
 * socket on Linux/macOS/BSD and then waits for a client to connect to it.
 * When the client connects, a thread is created to handle communications with
 * that client, and this loop is free to wait for the next client connect
 * request. It is an infinite loop.
 */
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

#endif  /* PCLOUD_PCLSYNC_POVERLAY_H_ */
