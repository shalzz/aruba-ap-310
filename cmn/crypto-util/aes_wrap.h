#ifndef __AES_WRAP_H__

#include <stdint.h>

int aes_wrap_wrapper(int n, const uint8_t *plain, uint8_t *cipher);
int aes_unwrap_wrapper(int n, const uint8_t *cipher, uint8_t *plain);

int aes_wrap(const uint8_t *kek, int n, const uint8_t *plain, uint8_t *cipher) ;
int aes_unwrap(const uint8_t *kek, int n, const uint8_t *cipher, uint8_t *plain) ;

void * aes_encrypt_init(const uint8_t *key, size_t len) ;
void aes_encrypt(void *ctx, const uint8_t *plain, uint8_t *crypt) ;
void aes_encrypt_deinit(void *ctx) ;

void * aes_decrypt_init(const uint8_t *key, size_t len) ;

void aes_decrypt(void *ctx, const uint8_t *crypt, uint8_t *plain) ;

void aes_decrypt_deinit(void *ctx) ;
int omac1_aes_128(const uint8_t *key, const uint8_t *data, size_t data_len, uint8_t *mac) ;

int aes_128_encrypt_block(const uint8_t *key, const uint8_t *in, uint8_t *out) ;
int aes_128_ctr_encrypt(const uint8_t *key, const uint8_t *nonce,
																uint8_t *data, size_t data_len) ;
int aes_128_eax_encrypt(const uint8_t *key, const uint8_t *nonce, size_t nonce_len,
																const uint8_t *hdr, size_t hdr_len,
																uint8_t *data, size_t data_len, uint8_t *tag) ;
int aes_128_eax_decrypt(const uint8_t *key, const uint8_t *nonce, size_t nonce_len,
																const uint8_t *hdr, size_t hdr_len,
																uint8_t *data, size_t data_len, const uint8_t *tag) ;
int aes_128_cbc_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len) ;
int aes_128_cbc_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *ciphertext, size_t cipher_len) ;

#endif /*endof __AES_WRAP_H__*/
