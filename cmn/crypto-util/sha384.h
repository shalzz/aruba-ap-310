#ifndef SHA384_H
#define SHA384_H

#define SHA384_MAC_LEN 48

#include "common.h"
#include "cryptohash.h"

void hmac_sha384_vector(const uint8_t *key, size_t key_len, size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac);

void hmac_sha384(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac);

void sha384_prf( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len,
                unsigned char *output, int len) ;

void kdf_sha384_704( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len,
                unsigned char *output, int buf_len);

void sha384_t_prf(const uint8_t *key, size_t key_len, const char *label, const uint8_t *seed, size_t seed_len, uint8_t *buf, size_t buf_len);

void pbkdf2_sha384(const char *passphrase, const char *ssid, size_t ssid_len, int iterations, uint8_t *buf, size_t buflen);

void sha384(unsigned char *inputbuf, unsigned long input_len, unsigned char *out_digest) ;
void sha384_vector(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac) ;

typedef hash_context SHA384Context ;

void SHA384Init( SHA384Context *context);
void SHA384Update( SHA384Context *context, const void *data, uint32_t len);
void SHA384Final(unsigned char digest[SHA384_MAC_LEN], SHA384Context *context);

void hmac_sha384_kdf(const u8 *secret, size_t secret_len,
            const char *label, const u8 *seed, size_t seed_len,
            u8 *out, size_t outlen);
#endif /* SHA384_H */
