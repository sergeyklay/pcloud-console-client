/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013-2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUDCC_PSYNC_ACCOUNTEVENTS_H_
#define PCLOUDCC_PSYNC_ACCOUNTEVENTS_H_

/*! \file accountevents.h
 *  \brief Provides base data types to handle account events.
 */

#include <stdint.h>

/*! \brief Account changed everything. */
#define PACCOUNT_CHANGE_ALL 1

/*! \brief Account changed its links. */
#define PACCOUNT_CHANGE_LINKS 2

/*! \brief Account changed its contacts. */
#define PACCOUNT_CHANGE_CONTACTS 3

/*! \brief Account changed its team. */
#define PACCOUNT_CHANGE_TEAMS 4

/*! \brief Account changed its emails. */
#define PACCOUNT_CHANGE_EMAILS 5

/*! \brief A type definition for the type of account changes.
 *
 * Meant for use with \ref paccount_cache_callback_t.
 */
typedef uint32_t psync_changetype_t;

/*! \brief A type definition for an account cache change callback.
 *
 * Called every time account team, members, contacts or links has been change.
 *
 * \warning You must free event or it will leak.
 */
typedef void (*paccount_cache_callback_t)(void *event);

#endif  /* PCLOUDCC_PSYNC_ACCOUNTEVENTS_H_ */
