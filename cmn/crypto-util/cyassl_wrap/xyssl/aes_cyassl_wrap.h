#ifndef _AES_WRAP_XYSSL_H_
#define _AES_WRAP_XYSSL_H_

#include <ctaocrypt/aes.h>

#define AES_DECRYPT 0
#define AES_ENCRYPT 1

#define aes_context     Aes

static inline void aes_setkey_enc(Aes *ctx, const byte *key, int keylen) 
{
    AesSetKey(ctx, key, keylen/8, NULL, AES_ENCRYPTION);
}

static inline void aes_setkey_dec(Aes *ctx, const byte *key, int keylen) 
{
    AesSetKey(ctx, key, keylen/8, NULL, AES_DECRYPTION);
}

static inline void aes_crypt_ecb(Aes *ctx, int mode, byte *in, byte *out)
{
    if (mode == AES_DECRYPT){
        AesDecryptDirect(ctx, out, in);
    } else { //AES_ENCRYPT
        AesEncryptDirect(ctx, out, in);
    }
}

#endif
