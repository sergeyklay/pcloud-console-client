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

#ifndef PCLOUD_PCLSYNC_PCALLBACKS_H_
#define PCLOUD_PCLSYNC_PCALLBACKS_H_

#include "psynclib.h"

void psync_callbacks_get_status(pstatus_t *status);
void psync_set_status_callback(pstatus_change_callback_t callback);
void psync_send_status_update();
void psync_set_event_callback(pevent_callback_t callback);
void psync_send_event_by_id(psync_eventtype_t eventid, psync_syncid_t syncid, const char *localpath, psync_fileorfolderid_t remoteid);
void psync_send_event_by_path(psync_eventtype_t eventid, psync_syncid_t syncid, const char *localpath, psync_fileorfolderid_t remoteid, const char *remotepath);
void psync_send_eventid(psync_eventtype_t eventid);
void psync_send_eventdata(psync_eventtype_t eventid, void *eventdata);

#endif  /* PCLOUD_PCLSYNC_PCALLBACKS_H_ */
