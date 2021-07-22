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

#include <string.h>
#include <stddef.h>

#include "pssl.h"
#include "psynclib.h"
#include "pmemlock.h"

static void psync_ssl_free_psync_encrypted_data_t(psync_encrypted_data_t e) {
  psync_ssl_memclean(e->data, e->datalen);
  psync_locked_free(e);
}

void psync_ssl_rsa_free_binary(psync_binary_rsa_key_t bin) {
  psync_ssl_free_psync_encrypted_data_t(bin);
}

void psync_ssl_free_symmetric_key(psync_symmetric_key_t key) {
  psync_ssl_memclean(key->key, key->keylen);
  psync_locked_free(key);
}

psync_encrypted_symmetric_key_t psync_ssl_alloc_encrypted_symmetric_key(size_t len) {
  psync_encrypted_symmetric_key_t ret;
  ret=psync_malloc(offsetof(psync_encrypted_data_struct_t, data)+len);
  ret->datalen=len;
  return ret;
}

psync_encrypted_symmetric_key_t psync_ssl_copy_encrypted_symmetric_key(psync_encrypted_symmetric_key_t src) {
  psync_encrypted_symmetric_key_t ret;
  ret=psync_malloc(offsetof(psync_encrypted_data_struct_t, data)+src->datalen);
  ret->datalen=src->datalen;
  memcpy(ret->data, src->data, src->datalen);
  return ret;
}
