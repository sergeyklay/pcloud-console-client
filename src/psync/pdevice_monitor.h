#ifndef PCLOUD_PSYNC_PDEVICE_MONITOR_H_
#define PCLOUD_PSYNC_PDEVICE_MONITOR_H_

/*! \brief Devices monitoring functions.
 */

#include <stdint.h>

#include "psynclib.h"

typedef struct _pdevice_info pdevice_info;

struct _pdevice_info {
  pdevice_types type;
  int isextended;
  char * filesystem_path;
};

typedef struct _pdevice_extended_info pdevice_extended_info;

struct _pdevice_extended_info {
  pdevice_types type;
  int isextended;
  char *filesystem_path;
  char *vendor;
  char *product;
  char *device_id;
  pdevice_extended_info* next;
  pdevice_extended_info* prev;
};

#ifdef __cplusplus
extern "C" {
#endif

  void pinit_device_monitor();

  /*! \brief Adds device monitoring callback which is invoked every time a new
   *         not disabled device arrives.
   */
  void pnotify_device_callbacks(pdevice_extended_info *param, device_event event);

#ifdef __cplusplus
}
#endif

#endif  /* PCLOUD_PSYNC_PDEVICE_MONITOR_H_ */
