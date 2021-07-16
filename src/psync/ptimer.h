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

#ifndef PCLOUD_PSYNC_PTIMER_H_
#define PCLOUD_PSYNC_PTIMER_H_

#include <time.h>
#include <stdlib.h>

#include "pcloudcc/psync/compiler.h"
#include "plist.h"
#include "logger.h"

#define PSYNC_INVALID_TIMER NULL

extern time_t psync_current_time;

struct _psync_timer_t;

typedef void (*psync_timer_callback)(struct _psync_timer_t *, void *);
typedef void (*psync_exception_callback)();

typedef struct _psync_timer_t {
  psync_list list;
  psync_timer_callback call;
  void *param;
  time_t numsec;
  time_t runat;
  uint32_t level;
  uint32_t opts;
} psync_timer_structure_t, *psync_timer_t;

void psync_timer_init();
time_t psync_timer_time();
void psync_timer_wake();
psync_timer_t psync_timer_register(psync_timer_callback func, time_t numsec, void *param);
int psync_timer_stop(psync_timer_t timer);
void psync_timer_exception_handler(psync_exception_callback func);
void psync_timer_sleep_handler(psync_exception_callback func);
void psync_timer_do_notify_exception();
void psync_timer_wait_next_sec();

#define psync_timer_notify_exception() do { \
    log_info("sending exception");          \
    psync_timer_do_notify_exception();      \
  } while (0)

#endif  /* PCLOUD_PSYNC_PTIMER_H_ */
