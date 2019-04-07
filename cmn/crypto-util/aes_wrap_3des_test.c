/****************************************************************************
 *                                                                          *
 * Harsha Nagaraja                                                          *
 *                                                                          *
 * This is a test program that AES wrap's a 3DES encrypted data             *
 *                                                                          *
 * Usage : <prog_name> <serial_number> <plain_text_data>                    *
 *                                                                          *
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <asm/types.h>
#include <sys/types.h>
#include <openssl/aes.h>
#include <aruba_defs.h>
#include <netinet/in.h>

#include "des_enc.h"
#include <des.h>
#include "aes_wrap.h"

static void *encbuf = NULL;
static int encbufsize = 0;
static void *decbuf = NULL;
static int decbufsize = 0;
static int aes_enc_buf_len = 0;
static int aes_buf_align = 0;
static int aes_wrap_key_len = 0;
static char *aes_wrap_enc_buf = NULL;
static char *aes_wrap_ascii_buf = NULL;
static char *aes_wrap_unenc_buf = NULL;

typedef enum _sxdr_enum_types {
    SXDR_TYPE_STR, SXDR_TYPE_MAC, SXDR_TYPE_U8, SXDR_TYPE_U16,
    SXDR_TYPE_U32, SXDR_TYPE_IP, SXDR_TYPE_S32, SXDR_TYPE_BOOL,
    SXDR_TYPE_HEX, SXDR_TYPE_U64, SXDR_TYPE_LAST
} sxdr_enum_types;

static char hex_dig[] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F'
};

extern __u32
sxdr_size_str (__u8 * str)
{
    return (sizeof (__u16) + strlen (str) + sizeof (__u8));
}

extern __u32
sxdr_write_str (__u8 * str, __u8 * buffer)
{
    __u16 len = strlen (str);
    __u16 nlen;
    __u16 i;

    for (i = 0; i < len; i ++) {
        if ( ! isprint (str[i]) && ! isspace(str[i])) {
            return (0);
        }
    }

    *buffer = (__u8) SXDR_TYPE_STR;
    buffer ++;

    nlen = htons (len);

    memcpy (buffer, &nlen, sizeof (__u16));
    buffer = buffer + sizeof (__u16);

    memcpy (buffer, str, len);
    return (len + sizeof (__u16) + sizeof (__u8));
}

extern __u32
sxdr_read_str (__u8 * str, __u8 * buffer)
{
    __u16 len;
    __u16 nlen;
    __u16 i;

    str[0] = 0;

    if (*buffer != (__u8) SXDR_TYPE_STR)
        return 0;
    buffer ++;

    memcpy (&nlen, buffer, sizeof (__u16));
    buffer = buffer + sizeof (__u16);

    len = ntohs (nlen);

    for (i = 0; i < len; i ++) {
        if ( ! isprint (buffer[i]) && ! isspace(buffer[i])) {
            return (0);
        }
    }

    memcpy (str, buffer, len);
    str[len] = 0;
    return (len + sizeof (__u16) + sizeof (__u8));
}

int sap_grow_buffer(void **buf, int *size, int needed, int chunk)
{
    void *nbuf;
    int newsize;

    if (needed <= *size) {
        return 1;
    }

    newsize = ((needed + chunk - 1) / chunk) * chunk;

    if (!(nbuf = realloc(*buf, newsize))) {
        return 0;
    }

    *buf = nbuf;
    *size = newsize;

    return 1;
}

int main(int argc, char *argv[])
{
    int plen = 0;
    int serial_len = 0;
    int tlen = 0;
    int esize = 0;
    char *serial = NULL, *pstr = NULL;
    __u32 n = 0, size = 0 ;
    char buf[1024];

    if(argc==3){
        serial_len = strlen(argv[1]);
        plen = strlen(argv[2]);
    }
    if(argc!=3 || 0 == plen || 0 == serial_len){
        fprintf(stderr, "\n%s <serial> <buffer_string_to_be_des_encrypted_followed_by_aes_wrapped>\n\n", argv[0]);
        return -1;
    }
    serial = argv[1];
    pstr = argv[2];
    tlen = serial_len + plen;
    esize = 2 * tlen + 32;

    if (!sap_grow_buffer(&decbuf, &decbufsize, tlen, 128)
            || !sap_grow_buffer(&encbuf, &encbufsize, esize, 128)) {
        fprintf(stderr, "\nError : mem aloc failure \n\n");
        return -1;
    }
    memset(buf, 0, sizeof(buf));
    memcpy(decbuf, serial, serial_len);
    memcpy(((__u8 *) decbuf) + serial_len, pstr, plen);
    ap_prov_encrypt(decbuf, tlen, encbuf, NULL);
    size = sxdr_size_str(encbuf);

    fprintf(stderr, "3DES encrypted buffer :\n%s - %d\n", (char *)encbuf, strlen(encbuf));

    /* input buffer is byte aligned if necessary */
    aes_buf_align = ((size%8) ==0) ? 0: 8;
    /*
     * Length of wrapped key is <lenth of plain_text + "byte aligned">/8
     * Ex : If len_of_plain_text = 1, aes_buf_align = 8
     *                            (1+8)/8 = 1
     *      If len_of_plain_text = 16, aes_buf_align = 0
     *                            (16+0)/8 = 2
     *      If len_of_plain_text = 17, aes_buf_align = 8
     *                            (16+8)/8 = 3
     *
     **/
    aes_wrap_key_len = (size + aes_buf_align)/8;
    /*
     * Converting wrapped_key which represents "64-bit" block into bytes
     * Adding 8 bytes more to have encrypted
     * value which is 8 bytes (64-bit) longer than padded plain text (n+1)
     *
     * Ex : If len_of_wraped_key = 1, len_of_plain_text = 1 byte
     *          len_of_encrypted_buf_len is :               (1*8) + 8 ==> 16 bytes
     *      If len_of_wraped_key = 2, len_of_plain_text = 16 bytes
     *          len_of_encrypted_buf_len is :               (2*8) + 8 ==> 24 bytes
     *      If len_of_wraped_key = 3, len_of_plain_text = 17 bytes
     *          len_of_encrypted_buf_len is :               (3*8) + 8 ==> 32 bytes
     *
     **/
    aes_enc_buf_len = (aes_wrap_key_len * 8 ) + 8;
    aes_wrap_enc_buf = malloc(aes_enc_buf_len);
    aes_wrap_ascii_buf = malloc(aes_enc_buf_len*2+1);
    memset(aes_wrap_enc_buf, 0, aes_enc_buf_len);
    memset(aes_wrap_ascii_buf, 0, aes_enc_buf_len*2);
    /*aes wrap the data*/
    if( aes_wrap_wrapper(aes_wrap_key_len, encbuf, aes_wrap_enc_buf)){
        fprintf(stderr, "\nError : aes_wrap failed... \n");
        return -1;
    }
    bytes_to_asc(aes_wrap_enc_buf, aes_enc_buf_len, aes_wrap_ascii_buf);
    fprintf(stderr, "AES_WRAP of 3DES encrypted buffer : %s\n\n", aes_wrap_ascii_buf);
    /*write aes wrapped ascii converted buffer*/
    n += sxdr_write_str(aes_wrap_ascii_buf, &buf[n]);
    aes_enc_buf_len = strlen(aes_wrap_ascii_buf);
    /*free aes_wrap_enc_buf*/
    free(aes_wrap_enc_buf);
    free(aes_wrap_ascii_buf);

    n = 0;
    {
        int enclen = 0, declen = 0, dlen=0;

        enclen = aes_enc_buf_len;
        declen = enclen / 2;

        if (!sap_grow_buffer(&encbuf, &encbufsize, enclen, 128)
                || !sap_grow_buffer(&decbuf, &decbufsize,
                    declen, 128)) {
            fprintf(stderr, "\nError : mem aloc failure \n\n");
            return -1;
        }
        /*encrypted buf is ASCII bytes of aes wrap*/
        aes_wrap_enc_buf = malloc(enclen);
        aes_wrap_unenc_buf = malloc(enclen/2);
        aes_wrap_ascii_buf = malloc(enclen/2);

        memset(decbuf, 0, declen);
        memset(aes_wrap_enc_buf, 0, enclen);
        memset(aes_wrap_unenc_buf, 0, enclen/2);
        memset(aes_wrap_ascii_buf, 0, enclen/2);

        /*Read encypted buffer to aes_wrap_enc_buf*/
        n += sxdr_read_str(aes_wrap_enc_buf, &buf[n]);
        fprintf(stderr, "(Reading back)AES_WRAP of 3DES encrypted buffer : %s\n", aes_wrap_enc_buf);
        /*convert ASCII encrypted data to binary data */
        asc_to_bytes(aes_wrap_enc_buf, enclen, aes_wrap_ascii_buf);
        /*aes wrapped contents are 64-bits padded, enclen is ascii bytes length
         * the real length is half off ascii length*/
        aes_wrap_key_len = ((enclen/2)-8)/8;
        /*decrypt the converted data and place the contents in aes_wrap_unenc_buf*/
        if( aes_unwrap_wrapper(aes_wrap_key_len, aes_wrap_ascii_buf,
                                                        aes_wrap_unenc_buf)){
            fprintf(stderr, "\nError : aes_wrap failed... \n");
            return -1;
        }
        fprintf(stderr, "AES_UNWRAP of 3DES encrypted buffer :\n%s -%d\n", aes_wrap_unenc_buf, strlen(aes_wrap_unenc_buf));
        memset(aes_wrap_ascii_buf, 0, enclen/2);
        /*aes_wrap_unenc_buf is ASCII bytes of des encryption*/
        ap_prov_decrypt(aes_wrap_unenc_buf, strlen(aes_wrap_unenc_buf), aes_wrap_ascii_buf, &dlen);

        fprintf(stderr, "3DES decrypted buffer :\n%s - %d\n", (char *)aes_wrap_ascii_buf, strlen(aes_wrap_ascii_buf));

        /*free aes_wrap_unenc_buf and aes_wrap_enc_buf*/
        free(aes_wrap_enc_buf);
        free(aes_wrap_ascii_buf);
        free(aes_wrap_unenc_buf);

    }
    return 0;
}
