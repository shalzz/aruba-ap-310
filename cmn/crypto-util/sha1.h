#ifndef SHA1_H
#define SHA1_H

#define SHA1_MAC_LEN 20

#include "common.h"
#include "cryptohash.h"

void hmac_sha1_vector(const uint8_t *key, size_t key_len, size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac);

void hmac_sha1(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *mac);

void sha1_prf(const unsigned char *key, int key_len, const unsigned char *prefix, int prefix_len,
	      const u8 *data, int data_len, unsigned char *buf, int buf_len) ;

#if 0
void sha1_prf( const unsigned char *key, int key_len, const unsigned char *prefix, int prefix_len, unsigned char *data, int data_len, unsigned char *output, int len ) ;
#endif
void pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len, int iterations, uint8_t *buf, size_t buflen);

void sha1(unsigned char *inputbuf, unsigned long input_len, unsigned char *out_digest) ;
void sha1_vector(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac) ;

typedef hash_context SHA1Context ;

void SHA1Init( SHA1Context *context);
void SHA1Update( SHA1Context *context, const void *data, uint32_t len);
void SHA1Final(unsigned char digest[SHA1_MAC_LEN], SHA1Context *context);

#endif /* SHA1_H */
