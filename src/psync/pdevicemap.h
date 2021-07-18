/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2018 Alexander Dimitrov <alexander.dimitrov@pcloud.com>
 * (c) 2018 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PDEVICEMAP_H_
#define PCLOUD_PSYNC_PDEVICEMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

void do_notify_device_callbacks_in(void * param);
void do_notify_device_callbacks_out(void * param);

pdevice_extended_info* construct_deviceininfo(
    pdevice_types type, int isextended, const char *filesystem_path,
    const char *vendor,const char *product,const char *device_id);

void destruct_deviceininfo(pdevice_extended_info* device);

void add_device(pdevice_types type, int isextended, const char *filesystem_path, const char *vendor, const  char *product,const char *device_id);
void remove_device(const char *filesystem_path);
void filter_unconnected_device();
void init_devices ();
void print_stree();
void print_device_info(pdevice_extended_info *ret );

/*! \brief Lists all stored devices. */
pdevice_item_list_t * psync_list_devices(char **err);

/*! \brief Enables device.
 *
 * This info is stored in the database so will be present after restart.
 */
void penable_device(const char* device_id);

/*! \brief Disable device. */
void pdisable_device(const char* device_id);

/*! \brief Remove db information about device. */
void premove_device(const char* device_id);

#ifdef __cplusplus
}
#endif

#endif  /* PCLOUD_PSYNC_PDEVICEMAP_H_ */
