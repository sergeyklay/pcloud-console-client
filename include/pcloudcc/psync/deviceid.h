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

/*! \file deviceid.h
 *  \brief Provides device information used during the API calls.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Set the name (and version) of the operating system that is passed
 *         to the server during token creation. If not set, it will be
 *         automatically detected.
 */
void psync_set_os_name(const char *name);

/*! \brief Get the name (and version) of the operating system that was set by
 *         psync_set_os_name(). If not set, it will be automatically detected.
 */
char *psync_get_os_name();

/*! \brief Set the name (and version) of the software that is passed to the
 *         server during token creation.
 *
 * This function is to be called before psync_start_sync() and it is acceptable
 * to call it even before psync_init().
 *
 * \note Library will not make its own copy, so either pass a static string or
 *       make a copy of your dynamic string.
 */
void psync_set_software_name(const char *name);

/*! \brief Get the name (and version) of the software that is passed to the
 *         server during token creation.
 *
 *  If not set by psync_set_software_name() will return #PSYNC_VERSION_FULL.
 */
const char *psync_get_software_name();

/*! \brief Get the full name of the device.
 *
 * This function uses psync_get_os_name() and psync_get_software_name() to
 * provide the full name of the device.
 */
char *psync_get_device_string();

/*! \brief Get current device ID.
 *
 * This function tries to get the device ID from the database, generated from
 * previous launches. If it fails, a new identifier will be generated and
 * saved to the database.
 */
char *psync_get_device_id();

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_PSYNC_PDEVICEID_H_ */
