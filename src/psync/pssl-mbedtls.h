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

#ifndef PCLOUD_PSYNC_PSSL_MBEDTLS_H_
#define PCLOUD_PSYNC_PSSL_MBEDTLS_H_

#include <stdint.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha512.h>
#include <mbedtls/rsa.h>
#include <mbedtls/aes.h>

#include "plibs.h"
#include "logger.h"

#define PSYNC_INVALID_RSA NULL
#define PSYNC_INVALID_SYM_KEY NULL

#define PSYNC_SHA1_BLOCK_LEN 64
#define PSYNC_SHA1_DIGEST_LEN 20
#define PSYNC_SHA1_DIGEST_HEXLEN 40
#define psync_sha1_ctx mbedtls_sha1_context
#define psync_sha1(data, datalen, checksum) mbedtls_sha1(data, datalen, checksum)
#define psync_sha1_init(pctx) mbedtls_sha1_starts(pctx)
#define psync_sha1_update(pctx, data, datalen) mbedtls_sha1_update(pctx, (const unsigned char *)data, datalen)
#define psync_sha1_final(checksum, pctx) mbedtls_sha1_finish(pctx, checksum)

#define PSYNC_SHA512_BLOCK_LEN 128
#define PSYNC_SHA512_DIGEST_LEN 64
#define PSYNC_SHA512_DIGEST_HEXLEN 128
#define psync_sha512_ctx mbedtls_sha512_context
#define psync_sha512(data, datalen, checksum) mbedtls_sha512(data, datalen, checksum, 0)
#define psync_sha512_init(pctx) mbedtls_sha512_starts(pctx, 0)
#define psync_sha512_update(pctx, data, datalen) mbedtls_sha512_update(pctx, (const unsigned char *)data, datalen)
#define psync_sha512_final(checksum, pctx) mbedtls_sha512_finish(pctx, checksum)

typedef mbedtls_rsa_context *psync_rsa_t;
typedef mbedtls_rsa_context *psync_rsa_publickey_t;
typedef mbedtls_rsa_context *psync_rsa_privatekey_t;

typedef struct {
  size_t keylen;
  unsigned char key[];
} psync_symmetric_key_struct_t, *psync_symmetric_key_t;

typedef mbedtls_aes_context *psync_aes256_encoder;
typedef mbedtls_aes_context *psync_aes256_decoder;


#if defined(__GNUC__) && (defined(__amd64__) || defined(__x86_64__) || defined(__i386__))
#define PSYNC_AES_HW
#define PSYNC_AES_HW_GCC
#endif

#define mbedtls_log_error(errnum) do {              \
  char error_buf[100];                              \
  mbedtls_strerror(errnum, error_buf, 100);         \
  log_error("mbedtls[%d]: %s", errnum, error_buf);  \
} while (0)

#ifdef PSYNC_AES_HW
extern uint32_t psync_ssl_hw_aes;

void psync_aes256_encode_block_hw(psync_aes256_encoder enc, const unsigned char *src, const unsigned char *dst);
void psync_aes256_decode_block_hw(psync_aes256_decoder enc, const unsigned char *src, const unsigned char *dst);
void psync_aes256_encode_2blocks_consec_hw(psync_aes256_encoder enc, const unsigned char *src, const unsigned char *dst);
void psync_aes256_decode_2blocks_consec_hw(psync_aes256_decoder enc, const unsigned char *src, const unsigned char *dst);
void psync_aes256_decode_4blocks_consec_xor_hw(psync_aes256_decoder enc, const unsigned char *src, const unsigned char *dst, const unsigned char *bxor);
void psync_aes256_decode_4blocks_consec_xor_sw(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst, const unsigned char *bxor);

static inline void psync_aes256_encode_block(psync_aes256_encoder enc, const unsigned char *src, unsigned char *dst) {
  if (likely(psync_ssl_hw_aes))
    psync_aes256_encode_block_hw(enc, src, dst);
  else
    mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_ENCRYPT, src, dst);
}

static inline void psync_aes256_decode_block(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst) {
  if (likely(psync_ssl_hw_aes))
    psync_aes256_decode_block_hw(enc, src, dst);
  else
    mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src, dst);
}

static inline void psync_aes256_encode_2blocks_consec(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst) {
  if (likely(psync_ssl_hw_aes))
    psync_aes256_encode_2blocks_consec_hw(enc, src, dst);
  else{
    mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_ENCRYPT, src, dst);
    mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_ENCRYPT, src+PSYNC_AES256_BLOCK_SIZE, dst+PSYNC_AES256_BLOCK_SIZE);
  }
}

static inline void psync_aes256_decode_2blocks_consec(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst) {
  if (likely(psync_ssl_hw_aes))
    psync_aes256_decode_2blocks_consec_hw(enc, src, dst);
  else{
    mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src, dst);
    mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src+PSYNC_AES256_BLOCK_SIZE, dst+PSYNC_AES256_BLOCK_SIZE);
  }
}

static inline void psync_aes256_decode_4blocks_consec_xor(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst, unsigned char *bxor) {
  if (psync_ssl_hw_aes)
    psync_aes256_decode_4blocks_consec_xor_hw(enc, src, dst, bxor);
  else
    psync_aes256_decode_4blocks_consec_xor_sw(enc, src, dst, bxor);
}
#else
static inline void psync_aes256_encode_block(psync_aes256_encoder enc, const unsigned char *src, unsigned char *dst) {
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_ENCRYPT, src, dst);
}

static inline void psync_aes256_decode_block(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst) {
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src, dst);
}

static inline void psync_aes256_encode_2blocks_consec(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst) {
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_ENCRYPT, src, dst);
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_ENCRYPT, src+PSYNC_AES256_BLOCK_SIZE, dst+PSYNC_AES256_BLOCK_SIZE);
}

static inline void psync_aes256_decode_2blocks_consec(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst) {
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src, dst);
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src+PSYNC_AES256_BLOCK_SIZE, dst+PSYNC_AES256_BLOCK_SIZE);
}

static inline void psync_aes256_decode_4blocks_consec_xor(psync_aes256_decoder enc, const unsigned char *src, unsigned char *dst, unsigned char *bxor) {
  unsigned long i;
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src, dst);
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src+PSYNC_AES256_BLOCK_SIZE, dst+PSYNC_AES256_BLOCK_SIZE);
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src+PSYNC_AES256_BLOCK_SIZE*2, dst+PSYNC_AES256_BLOCK_SIZE*2);
  mbedtls_aes_crypt_ecb(enc, MBEDTLS_AES_DECRYPT, src+PSYNC_AES256_BLOCK_SIZE*3, dst+PSYNC_AES256_BLOCK_SIZE*3);
  for (i=0; i<PSYNC_AES256_BLOCK_SIZE*4/sizeof(unsigned long); i++)
    ((unsigned long *)dst)[i]^=((unsigned long *)bxor)[i];
}
#endif  /* PSYNC_AES_HW */
#endif  /* PCLOUD_PSYNC_PSSL_MBEDTLS_H_ */
