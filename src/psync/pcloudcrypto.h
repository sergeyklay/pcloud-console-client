/*
 * This file is part of the pCloud Console Client.
 *
 * (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
 * (c) 2014 Anton Titov <anton@pcloud.com>
 * (c) 2014 pCloud Ltd
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 */

#ifndef PCLOUD_PSYNC_PCLOUDCRYPTO_H_
#define PCLOUD_PSYNC_PCLOUDCRYPTO_H_

#include "pcloudcc/compat/compiler.h"

#include "pfs.h"
#include "pcrypto.h"
#include "papi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSYNC_CRYPTO_SYM_FLAG_ISDIR 1

#define PSYNC_CRYPTO_SECTOR_SIZE 4096

#define PSYNC_CRYPTO_MAX_ERROR 511

#define PSYNC_CRYPTO_SETUP_SUCCESS       0
#define PSYNC_CRYPTO_SETUP_KEYGEN_FAILED 1
#define PSYNC_CRYPTO_SETUP_CANT_CONNECT  2
#define PSYNC_CRYPTO_SETUP_NOT_LOGGED_IN 3
#define PSYNC_CRYPTO_SETUP_ALREADY_SETUP 4
#define PSYNC_CRYPTO_SETUP_UNKNOWN_ERROR 5

#define PSYNC_CRYPTO_START_SUCCESS            0
#define PSYNC_CRYPTO_START_ALREADY_STARTED    1
#define PSYNC_CRYPTO_START_CANT_CONNECT       2
#define PSYNC_CRYPTO_START_NOT_LOGGED_IN      3
#define PSYNC_CRYPTO_START_NOT_SETUP          4
#define PSYNC_CRYPTO_START_UNKNOWN_KEY_FORMAT 5
#define PSYNC_CRYPTO_START_BAD_PASSWORD       6
#define PSYNC_CRYPTO_START_KEYS_DONT_MATCH    7
#define PSYNC_CRYPTO_START_UNKNOWN_ERROR      8

#define PSYNC_CRYPTO_STOP_SUCCESS     0
#define PSYNC_CRYPTO_STOP_NOT_STARTED 1

#define PSYNC_CRYPTO_HINT_SUCCESS       0
#define PSYNC_CRYPTO_HINT_NOT_PROVIDED  1
#define PSYNC_CRYPTO_HINT_CANT_CONNECT  2
#define PSYNC_CRYPTO_HINT_NOT_LOGGED_IN 3
#define PSYNC_CRYPTO_HINT_UNKNOWN_ERROR 4

#define PSYNC_CRYPTO_RESET_SUCCESS           0
#define PSYNC_CRYPTO_RESET_CRYPTO_IS_STARTED 1
#define PSYNC_CRYPTO_RESET_CANT_CONNECT      2
#define PSYNC_CRYPTO_RESET_NOT_LOGGED_IN     3
#define PSYNC_CRYPTO_RESET_NOT_SETUP         4
#define PSYNC_CRYPTO_RESET_UNKNOWN_ERROR     5

#define PSYNC_CRYPTO_SUCCESS                0
#define PSYNC_CRYPTO_NOT_STARTED          (-1)
#define PSYNC_CRYPTO_RSA_ERROR            (-2)
#define PSYNC_CRYPTO_FOLDER_NOT_FOUND     (-3)
#define PSYNC_CRYPTO_FILE_NOT_FOUND       (-4)
#define PSYNC_CRYPTO_INVALID_KEY          (-5)
#define PSYNC_CRYPTO_CANT_CONNECT         (-6)
#define PSYNC_CRYPTO_FOLDER_NOT_ENCRYPTED (-7)
#define PSYNC_CRYPTO_INTERNAL_ERROR       (-8)

#define PSYNC_CRYPTO_INVALID_FOLDERID ((psync_folderid_t) - 1)

#define PSYNC_CRYPTO_UNLOADED_SECTOR_ENCODER ((psync_crypto_aes256_sector_encoder_decoder_t)(PSYNC_CRYPTO_MAX_ERROR+1))
#define PSYNC_CRYPTO_LOADING_SECTOR_ENCODER  ((psync_crypto_aes256_sector_encoder_decoder_t)(PSYNC_CRYPTO_MAX_ERROR+2))
#define PSYNC_CRYPTO_FAILED_SECTOR_ENCODER   ((psync_crypto_aes256_sector_encoder_decoder_t)(PSYNC_CRYPTO_MAX_ERROR+3))

const char *psync_cloud_crypto_strstart(int status);

void psync_cloud_crypto_clean_cache();

int psync_cloud_crypto_setup(const char *password, const char *hint);
int psync_cloud_crypto_get_hint(char **hint);
int psync_cloud_crypto_start(const char *password);
int psync_cloud_crypto_stop();
int psync_cloud_crypto_isstarted();
int psync_cloud_crypto_reset();
int psync_cloud_crypto_mkdir(psync_folderid_t folderid, const char *name, const char **err, psync_folderid_t *newfolderid);

psync_crypto_aes256_text_decoder_t psync_cloud_crypto_get_folder_decoder(psync_fsfolderid_t folderid);
void psync_cloud_crypto_release_folder_decoder(psync_fsfolderid_t folderid, psync_crypto_aes256_text_decoder_t decoder);
char *psync_cloud_crypto_decode_filename(psync_crypto_aes256_text_decoder_t decoder, const char *name);

psync_crypto_aes256_text_encoder_t psync_cloud_crypto_get_folder_encoder(psync_fsfolderid_t folderid);
void psync_cloud_crypto_release_folder_encoder(psync_fsfolderid_t folderid, psync_crypto_aes256_text_encoder_t encoder);
char *psync_cloud_crypto_encode_filename(psync_crypto_aes256_text_encoder_t encoder, const char *name);

psync_crypto_aes256_sector_encoder_decoder_t psync_cloud_crypto_get_file_encoder(psync_fsfileid_t fileid, uint64_t hash, int nonetwork);
psync_crypto_aes256_sector_encoder_decoder_t psync_cloud_crypto_get_file_encoder_from_binresult(psync_fileid_t fileid, binresult *res);
void psync_cloud_crypto_release_file_encoder(psync_fsfileid_t fileid, uint64_t hash, psync_crypto_aes256_sector_encoder_decoder_t encoder);

char *psync_cloud_crypto_get_file_encoded_key(psync_fsfileid_t fileid, uint64_t hash, size_t *keylen);
char *psync_cloud_crypto_get_new_encoded_key(uint32_t flags, size_t *keylen);
char *psync_cloud_crypto_get_new_encoded_and_plain_key(uint32_t flags, size_t *keylen, psync_symmetric_key_t *deckey);

static inline int psync_crypto_is_error(const void *ptr) {
  return (uintptr_t)ptr<=PSYNC_CRYPTO_MAX_ERROR;
}

static inline int psync_crypto_to_error(const void *ptr) {
  return -((int)(uintptr_t)ptr);
}

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* PCLOUD_PSYNC_PCLOUDCRYPTO_H_ */
