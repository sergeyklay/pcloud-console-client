/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2015 Anton Titov <anton@pcloud.com>
 * (c) 2015 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#include "prunratelimit.h"
#include "plibs.h"
#include "ptimer.h"
#include "ptree.h"
#include "logger.h"

typedef struct {
  psync_tree tree;
  psync_run_ratelimit_callback0 call;
  const char *name;
  unsigned char scheduled;
} psync_rr_tree_node;

static pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
static psync_tree *tasks = PSYNC_TREE_EMPTY;

static void psync_run_ratelimited_timer(psync_timer_t timer, void *ptr) {
  psync_rr_tree_node *node;
  const char *name;
  psync_run_ratelimit_callback0 call;
  int run;
  node=(psync_rr_tree_node *)ptr;
  pthread_mutex_lock(&task_mutex);
  if (node->scheduled) {
    run=1;
    node->scheduled=0;
    call=node->call;
    name=node->name;
  }
  else {
    run=0;
    psync_tree_del(&tasks, &node->tree);
  }
  pthread_mutex_unlock(&task_mutex);
  if (run) {
    log_info("running %s in a thread", name);
    psync_run_thread(name, call);
  }
  else {
    psync_timer_stop(timer);
    psync_free(node);
  }
}

void psync_run_ratelimited(const char *name, psync_run_ratelimit_callback0 call, uint32_t minintervalsec, int runinthread) {
  psync_tree *tr, **addto;
  psync_rr_tree_node *node;
  int found;
  found=0;
  pthread_mutex_lock(&task_mutex);
  tr=tasks;
  if (tr) {
    while (1) {
      node=psync_tree_element(tr, psync_rr_tree_node, tree);
      if (call<node->call) {
        if (tr->left)
          tr=tr->left;
        else {
          addto=&tr->left;
          break;
        }
      }
      else if (call>node->call) {
        if (tr->right)
          tr=tr->right;
        else {
          addto=&tr->right;
          break;
        }
      }
      else {
        found=1;
        break;
      }
    }
  }
  else
    addto=&tasks;
  if (found) {
    if (node->scheduled)
      log_info("skipping run of %s as it is already scheduled", name);
    else {
      log_info("scheduling run of %s on existing timer", name);
      node->scheduled=1;
    }
  }
  else {
    node=psync_new(psync_rr_tree_node);
    node->call=call;
    node->name=name;
    node->scheduled=0;
    *addto=&node->tree;
    psync_tree_added_at(&tasks, tr, &node->tree);
  }
  pthread_mutex_unlock(&task_mutex);
  if (!found) {
    if (runinthread) {
      log_info("running %s in a thread", name);
      psync_run_thread(name, call);
    }
    else {
      log_info("running %s on this thread", name);
      call();
    }
    psync_timer_register(psync_run_ratelimited_timer, minintervalsec, node);
  }
}
