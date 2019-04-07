#ifndef __CRYPTOHASH_H__
#define __CRYPTOHASH_H__

#include <stdint.h>
#include <openssl/evp.h>

typedef struct hash_context
{
      unsigned int digest_len ; 
      EVP_MD_CTX digest_ctx ;
} hash_context ;

#ifndef __FAT_AP__
void hmac_hash_vector( const EVP_MD *digest_ptr, const uint8_t *key, int key_len, int num_elem,
																const uint8_t *addr[], const size_t *len, uint8_t *mac);
#else
void hmac_hash_vector( const EVP_MD *digest_ptr, const uint8_t *key, int key_len, int num_elem,
																const uint8_t *addr[], const int *len, uint8_t *mac);
#endif

void hmac_hash( const EVP_MD *digest_ptr, const uint8_t *key, int key_len, const uint8_t *data, int data_len,
																uint8_t *mac);

void hash_vector( const EVP_MD *digest_ptr, size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac);

void hash_init( const EVP_MD *digest_ptr, struct hash_context *context);

void hash_update( hash_context *context, const void *data, uint32_t len);

void hash_final( unsigned char* digest, struct hash_context *context) ;

#ifdef __FAT_AP__
#ifndef __NO_OPENSSL__
int base64_encode( unsigned char *dst, int *dlen, unsigned char *src, int  slen );
#endif
#endif

#endif

