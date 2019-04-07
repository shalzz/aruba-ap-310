#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <asm/types.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "des_enc.h"
#include <ctaocrypt/des3.h>

#ifdef USE_SYSLOG
#include <syslog.h>
#include <syslog/syslogutil.h>
#include <syslog/arubalog.h>
#else
#define arubaLog(pri, fmt, args...) fprintf(stderr, fmt, ##args)
#endif

unsigned char ci_tmp[MAX_ENCODED_SZ + 16];  /* cipher text */
void prn_ci_txt(unsigned char *data, int len);
unsigned char my_keys[3][8] = {
    {0x17, 0xb2, 0x03, 0x9c, 0x3d, 0x22, 0x08},
    {0xa5, 0x45, 0xee, 0xb1, 0x83, 0x37, 0x22},
    {0x82, 0x73, 0x49, 0x34, 0xcc, 0x1f, 0x04}
};

static char hex_dig[] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F'
};

void
bytes_to_asc(__u8 * in, int ilen, __u8 * out)
{
    int i;
    for (i = 0; i < ilen; ++i) {
        out[2 * i] = (__u8) (hex_dig[((in[i] & 0xf0) >> 4)]);
        out[2 * i + 1] = (__u8) (hex_dig[in[i] & 0x0f]);
    }
}

void
asc_to_bytes(__u8 * in, int ilen, __u8 * out)
{
    int i;
    __u8 tmp1, tmp2;
    for (i = 0; i < ilen; i += 2) {
        tmp1 = (in[i] > '9' ? (in[i] - 'A' + 10) : (in[i] - '0')) & 0xF;
        tmp2 =
            (in[i + 1] > '9' ? (in[i + 1] - 'A' + 10) : (in[i + 1] - '0')) & 0xF;
        out[i / 2] = ((tmp1 << 4) | tmp2);
    }
}

int
ap_prov_decrypt(__u8 * in_cipher, int ilen, __u8 * clear, int *olen)
{
#ifdef USE_XYSSL
    __u8 ivec[8];
    des3_context des3_ctx;
#else
    Des3 des3_ctx ;
#endif
    __u8 ivec_bytes[8], *cipher;
    int dec_len = 0;

#ifdef USE_XYSSL
    des3_set3key_dec(&des3_ctx, my_keys[0]);
#endif
    cipher = (__u8 *) 0;
    if (!(cipher = (__u8 *) malloc(ilen)))
        return -1;
    asc_to_bytes(in_cipher, ilen, cipher);
    ilen /= 2;
    if (ilen < 8) {
        goto done;
    }
    memcpy(ivec_bytes, cipher, 8);
#ifdef USE_XYSSL
    des3_crypt_cbc(&des3_ctx, DES_DECRYPT, ilen - 8, ivec,
            cipher + 8, clear);
#else
    Des3_SetKey(&des3_ctx, my_keys[0], ivec_bytes, DES_DECRYPTION);
    Des3_CbcDecrypt(&des3_ctx, clear, cipher+8, ilen - 8 );
#endif
    dec_len = ilen - 8 - (ivec_bytes[7] & 0xF);
    if (dec_len < 0) {
        dec_len = 0;
    }

done:
    if (olen)
        *olen = dec_len;
    if (cipher)
        free(cipher);
    clear[dec_len] = '\0';
    return 0;
}

int
ap_prov_decrypt_cert(__u8 * in_cipher, int ilen, __u8 * clear, int *olen)
{
    return ap_prov_decrypt( in_cipher, ilen, clear, olen ) ;
}


int
pad_8x(__u8 * input, int ilen)
{
    int pad, i;
    pad = 8 * ((ilen + 7) / 8) - ilen;
    for (i = 0; i < pad; ++i)
        input[ilen + i] = 0;
    return pad;
}

int
ap_prov_encrypt(__u8 * clear, int ilen, __u8 * cipher, int *olen)
{
#ifdef USE_XYSSL
    __u8 ivec[8];
    des3_context des3_ctx;
#else
    Des3 des3_ctx ;
#endif
    __u8 ivec_bytes[8], *input;
    __u8 pad;

#ifdef USE_XYSSL
    des3_set3key_enc(&des3_ctx, my_keys[0]);
#endif
    input = (__u8 *) malloc(8 * ((ilen + 7) / 8));
    memcpy(input, clear, ilen);
    pad = pad_8x(input, ilen);
    ilen += pad;
    *(__u16 *) ivec_bytes = (__u16) (rand() & 0xFFFF);
    *(__u16 *) (ivec_bytes + 2) = (__u16) (rand() & 0xFFFF);
    *(__u16 *) (ivec_bytes + 4) = (__u16) (rand() & 0xFFFF);
    *(__u16 *) (ivec_bytes + 6) = (__u16) (rand() & 0xFFFF);
    ivec_bytes[7] = (ivec_bytes[7] & 0xF0) | pad;
#ifdef USE_XYSSL
    des3_crypt_cbc(&des3_ctx, DES_ENCRYPT, ilen, ivec,
            input, ci_tmp);
#else
    Des3_SetKey(&des3_ctx, my_keys[0], ivec_bytes, DES_ENCRYPTION ); 
    Des3_CbcEncrypt(&des3_ctx, ci_tmp, input, ilen ) ;
#endif
    free(input);
    bytes_to_asc(ivec_bytes, 8, cipher);
    bytes_to_asc(ci_tmp, ilen, &cipher[16]);
    if (olen)
        *olen = ilen * 2 + 16;
    cipher[ilen * 2 + 16] = '\0';
    return 1;
}

int
ap_prov_encrypt_cert(__u8 * clear, int ilen, __u8 * cipher, int *olen)
{
    return ap_prov_encrypt( clear, ilen, cipher, olen ) ;
}
