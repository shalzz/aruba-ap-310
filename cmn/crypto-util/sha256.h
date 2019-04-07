#ifndef SHA256_H
#define SHA256_H

#define SHA256_MAC_LEN 32

#include "common.h"
#include "cryptohash.h"

void hmac_sha256_vector(const uint8_t *key, size_t key_len, size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac);

void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac);

void sha256_prf( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len,
                 unsigned char *output, int buf_len ) ;

void sha256_t_prf(const uint8_t *key, size_t key_len, const char *label, const uint8_t *seed, size_t seed_len, uint8_t *buf, size_t buf_len);

void pbkdf2_sha256(const char *passphrase, const char *ssid, size_t ssid_len, int iterations, uint8_t *buf, size_t buflen);

void sha256(unsigned char *inputbuf, unsigned long input_len, unsigned char *out_digest) ;
void sha256_vector(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac) ;

typedef hash_context SHA256Context ;

void SHA256Init( SHA256Context *context);
void SHA256Update( SHA256Context *context, const void *data, uint32_t len);
void SHA256Final(unsigned char digest[SHA256_MAC_LEN], SHA256Context *context);

void hmac_sha256_kdf(const uint8_t *secret, size_t secret_len, const uint8_t *label, const uint8_t *seed, size_t seed_len, uint8_t *out, size_t outlen);

#endif /* SHA256_H */
