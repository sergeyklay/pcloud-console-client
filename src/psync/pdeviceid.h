/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PDEVICEID_H_
#define PCLOUD_PSYNC_PDEVICEID_H_

#ifdef __cplusplus
extern "C" {
#endif

char *psync_get_device_id();

/*! \brief Set the name and version of the operating system that is passed to
 *         the server during token creation. If not set, it will be
 *         automatically detected.
 */
void psync_set_os_name(const char *name);

void psync_set_software_name(const char *name);

const char *psync_get_software_name();

char *psync_get_device_os();

char *psync_get_device_string();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_PSYNC_PDEVICEID_H_ */
