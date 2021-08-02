/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2013 Anton Titov <anton@pcloud.com>
 * (c) 2013 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PAPI_H_
#define PCLOUD_PSYNC_PAPI_H_

#include "config.h"

#include <string.h>
#include <stdint.h>

#include "pcloudcc/psync/compat.h"
#include "pcloudcc/psync/compiler.h"
#include "logger.h"

#define PARAM_STR   0
#define PARAM_NUM   1
#define PARAM_BOOL  2

#define PARAM_ARRAY 3
#define PARAM_HASH  4
#define PARAM_DATA  5

#define PARAM_END 255

#define PTR_OK ((binresult *)1)

#define ASYNC_RES_NEEDMORE 0
#define ASYNC_RES_READY    1

typedef struct {
  uint16_t paramtype;
  uint16_t paramnamelen;
  uint32_t opts;
  const char *paramname;
  union {
    uint64_t num;
    const char *str;
  };
} binparam;

struct _binresult;

typedef struct _hashpair {
  const char *key;
  struct _binresult *value;
} hashpair;

typedef struct _binresult{
  uint32_t type;
  uint32_t length;
  union {
    uint64_t num;
    const char str[8];
    struct _binresult **array;
    struct _hashpair *hash;
  };
} binresult;

typedef struct {
  binresult *result;
  uint32_t state;
  uint32_t bytesread;
  uint32_t bytestoread;
  uint32_t respsize;
  unsigned char *data;
} async_result_reader;

#define P_STR(name, val) {PARAM_STR, strlen(name), strlen(val), (name), {(uint64_t)((uintptr_t)(val))}}
#define P_LSTR(name, val, len) {PARAM_STR, strlen(name), (len), (name), {(uint64_t)((uintptr_t)(val))}}
#define P_NUM(name, val) {PARAM_NUM, strlen(name), 0, (name), {(val)}}
#define P_BOOL(name, val) {PARAM_BOOL, strlen(name), 0, (name), {(val)?1:0}}

#define send_command(sock, cmd, params) do_send_command(sock, cmd, strlen(cmd), params, sizeof(params)/sizeof(binparam), -1, 1)
#define send_command_no_res(sock, cmd, params) do_send_command(sock, cmd, strlen(cmd), params, sizeof(params)/sizeof(binparam), -1, 0)

#define send_command_thread(sock, cmd, params) do_send_command(sock, cmd, strlen(cmd), params, sizeof(params)/sizeof(binparam), -1, 1|2)
#define send_command_no_res_thread(sock, cmd, params) do_send_command(sock, cmd, strlen(cmd), params, sizeof(params)/sizeof(binparam), -1, 2)

#define prepare_command_data_alloc(cmd, params, datalen, alloclen, retlen) \
  do_prepare_command(cmd, strlen(cmd), params, sizeof(params)/sizeof(binparam), datalen, alloclen, retlen)

#define psync_find_result(res, name, type) \
  psync_do_find_result(res, name, type, __FILENAME__, __FUNCTION__, __LINE__)
#define psync_check_result(res, name, type) \
  psync_do_check_result(res, name, type, __FILENAME__, __FUNCTION__, __LINE__)

psync_socket *psync_api_connect(const char *hostname, int usessl);
void psync_api_conn_fail_inc();
void psync_api_conn_fail_reset();

binresult *get_result(psync_socket *sock) PSYNC_NONNULL(1);
binresult *get_result_thread(psync_socket *sock) PSYNC_NONNULL(1);
void async_result_reader_init(async_result_reader *reader) PSYNC_NONNULL(1);
void async_result_reader_destroy(async_result_reader *reader) PSYNC_NONNULL(1);
int get_result_async(psync_socket *sock, async_result_reader *reader) PSYNC_NONNULL(1, 2);
unsigned char *do_prepare_command(const char *command, size_t cmdlen, const binparam *params, size_t paramcnt, int64_t datalen, size_t additionalalloc, size_t *retlen);
binresult *do_send_command(psync_socket *sock, const char *command, size_t cmdlen, const binparam *params, size_t paramcnt, int64_t datalen, int readres) PSYNC_NONNULL(1, 2);
const binresult *psync_do_find_result(const binresult *res, const char *name, uint32_t type, const char *file, const char *function, int unsigned line) PSYNC_NONNULL(2) PSYNC_PURE;
const binresult *psync_do_check_result(const binresult *res, const char *name, uint32_t type, const char *file, const char *function, int unsigned line)  PSYNC_NONNULL(2) PSYNC_PURE;

#endif  /* PCLOUD_PSYNC_PAPI_H_ */
