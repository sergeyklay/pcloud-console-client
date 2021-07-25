/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_OVERLAY_H_
#define PCLOUD_PSYNC_OVERLAY_H_

#include <stdint.h> /* uint32_t, uint64_t */

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief A type definition for the overlay message. */
typedef struct {
  uint32_t type;
  uint64_t length;
  char value[];
} overlay_message_t;

/*! \brief The overlay callback.
 *
 * Callback to be registered to be called from file manager extension or
 * commands mode (pcloudcc -k).  Meant for psync_overlay_add_callback().
 */
typedef int (*overlay_callback)(const char *path, void *rep);

/*! \brief Register an overlay callback.
 *
 * Registers overlay callback that will be called when packet with \a id
 * equals to the give one had arrived from extension.  The \a id must be over
 * or equal to 20 or -1 will be returned.  Value of 0 returned on success.
 *
 * \warning These function are not thread-safe. Use them in single thread or
 *          synchronize.
 */
int psync_overlay_add_callback(int id, overlay_callback callback);

/*! \brief Process a \a request. */
void psync_overlay_process_request(overlay_message_t *request,
                                   overlay_message_t *response);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PCLOUD_PSYNC_OVERLAY_H_ */
