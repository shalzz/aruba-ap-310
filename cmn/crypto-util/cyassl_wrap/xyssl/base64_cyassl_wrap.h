#ifndef _BASE64_CYASSL_WRAP_H_
#define _BASE64_CYASSL_WRAP_H_

#include <cyassl/ctaocrypt/coding.h>

int base64_encode( unsigned char *dst, int *dlen, unsigned char *src, int  slen )
{
    return Base64_Encode(src, slen, dst, dlen);
}

#endif
