#ifndef _AES_OPENSSL_CYASSL_WRAP_H_
#define _AES_OPENSSL_CYASSL_WRAP_H_

#include <ctaocrypt/aes.h>

typedef Aes  AES_KEY;


static inline void AES_set_encrypt_key(const byte *key, int keylen, AES_KEY *ctx) 
{
    AesSetKey(ctx, key, keylen/8, NULL, AES_ENCRYPTION);
}

static inline void AES_encrypt(const byte *in, byte *out, AES_KEY *ctx)
{
    AesEncryptDirect(ctx, out, in);
}
#endif

