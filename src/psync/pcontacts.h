/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PCONTACTS_H_
#define PCLOUD_PSYNC_PCONTACTS_H_

#include "ptypes.h"
#include "psynclib.h"

pcontacts_list_t *do_psync_list_contacts();
pcontacts_list_t *do_psync_list_myteams();
int do_call_contactlist(result_visitor vis, void *param);
void cache_contacts();
void cache_shares();

#endif  /* PCLOUD_PSYNC_PCONTACTS_H_ */
