/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PBUSINESSACCOUNT_H_
#define PCLOUD_PSYNC_PBUSINESSACCOUNT_H_

#include "psynclib.h"
#include "ptypes.h"

int do_psync_account_stopshare(psync_shareid_t usershareids[], int nusershareid, psync_shareid_t teamshareids[], int nteamshareid, char **err);
int do_psync_account_modifyshare(psync_shareid_t usrshrids[], uint32_t uperms[], int nushid,
                           psync_shareid_t tmshrids[], uint32_t tperms[], int ntmshid, char **err);

//int do_psync_account_users(psync_userid_t iserids[], int nids, result_visitor vis, void *param);
//int do_psync_account_teams(psync_userid_t teamids[], int nids, result_visitor vis, void *param);

void get_ba_member_email(uint64_t userid, char** email /*OUT*/, size_t *length /*OUT*/);
void get_ba_team_name(uint64_t teamid, char** name /*OUT*/, size_t *length /*OUT*/);

void cache_account_emails();
void cache_account_teams();
void cache_ba_my_teams();
int api_error_result(binresult* res);

#endif  /* PCLOUD_PSYNC_PBUSINESSACCOUNT_H_ */
