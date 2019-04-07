
/****************************************************************************
 *                                                                          *
 * Harsha Nagaraja                                                          *
 *                                                                          *
 * This is a test program that AES wrap's a data                            *
 *                                                                          *
 * Usage : <prog_name> <plain_text_data>                                    *
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


int main( int argc, char **argv )
{
    /*unsigned char plaintext[24] = "etaonrislcub94y!#-";
    unsigned char ciphertext[32];
    unsigned char buf[16];
    */
    char *plaintext, *ciphertext, *buf;
    char *asc_ciphertext;

    int ptxt_len = 0;
    int ctxt_len = 0;
    int wrap_key_len = 0;
    int tmod = 0;

    ptxt_len = strlen(argv[1]);

    if(argc!=2 || 0 == ptxt_len){
        fprintf(stderr, "\n%s <buffer_string_to_be_aes_wrapped>\n\n", argv[0]);
        return -1;
    }

    /* input buffer is byte aligned if necessary */
    tmod = ((ptxt_len%8) ==0) ? 0: 8;
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
    wrap_key_len = (ptxt_len + tmod)/8;
    /*
     * Converting wrapped_key which represents "64-bit" block into bytes
     * Adding 8 bytes more to have encrypted
     * value which is 8 bytes (64-bit) longer than padded plain text (n+1)
     *
     * Ex : If len_of_wraped_key = 1, len_of_plain_text = 1 byte
     *                                (1*8) + 8 ==> 16 bytes
     *      If len_of_wraped_key = 2, len_of_plain_text = 16 bytes
     *                                (2*8) + 8 ==> 24 bytes
     *      If len_of_wraped_key = 3, len_of_plain_text = 17 bytes
     *                                (3*8) + 8 ==> 32 bytes
     *
     **/
    ctxt_len = (wrap_key_len * 8 ) + 8;
    plaintext  = malloc(ptxt_len);
    ciphertext = malloc(ctxt_len);
    buf        = malloc(ptxt_len);
    asc_ciphertext = malloc(ctxt_len*2);

    memset(plaintext, 0, ptxt_len);
    memset(ciphertext, 0, ctxt_len);
    memset(asc_ciphertext, 0, ctxt_len);
    memset(buf, 0, ptxt_len);

    memcpy(plaintext, argv[1], ptxt_len);

    aes_wrap_wrapper(wrap_key_len, plaintext, ciphertext);

    /*For printing, converting the binary data to ascii printable form*/
    bytes_to_asc(ciphertext, ctxt_len, asc_ciphertext);
    fprintf(stderr, "\nAsc Cipher Txt        : %s\n", asc_ciphertext);
    fprintf(stderr, "Asc Cipher Txt length : %d\n", strlen(asc_ciphertext));

    /*wrap key len is obtained by reducing the Asci cipher text lenght by half*/
    wrap_key_len = (strlen(asc_ciphertext)/2- 8)/8;
    /*Note : asciii converted buffer is NOT used for decrypting*/
    aes_unwrap_wrapper(wrap_key_len, ciphertext, buf);
    fprintf(stderr, "buf                   : %s\n", buf);

    /*note : memory is not being freed here !*/

#if 0
    memset(buf2, 0, sizeof(buf2));
    memset(ciphertext2, 0, sizeof(ciphertext2));
    if (AES_set_encrypt_key( gkek2, 192, &key ))
	return 0;
    AES_encrypt( plaintext2, ciphertext2, &key);

    fprintf(stderr, "--------------------------- \n");
    fprintf(stderr, "AES_encrypt Txt :");
    for (i=0; i< sizeof(ciphertext2); i++)
        fprintf(stderr, "%02X ", ciphertext2[i]);
    fprintf(stderr, "\n");

    if (AES_set_decrypt_key( gkek2, 192, &dkey ))
        return 0;
    AES_decrypt( ciphertext2, buf2, &dkey);
    fprintf(stderr, "AES_decrypt buf :%s\n\n\n", buf2);
#endif

    return 0;
}

