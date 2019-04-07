#include "cryptohash.h"
#include "sha1.h"

#ifdef __NO_OPENSSL__
#include <ctaocrypt/pwdbased.h>
#endif

void hmac_sha1_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		      const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
     hmac_hash_vector( EVP_sha1(), key, key_len, num_elem, addr, len, mac ) ;
}

void hmac_sha1(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
	       uint8_t *mac)
{
					hmac_hash( EVP_sha1(), key, key_len, data, data_len, mac ) ; 
}

void sha1_vector(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
     hash_vector( EVP_sha1(), num_elem, addr, len, mac ) ;
}

#ifdef __NO_OPENSSL__
void sha1(unsigned char *inputbuf, unsigned long input_len, unsigned char *out_digest)
{
     SHA1Context context ;
     SHA1Init( &context ) ;
     SHA1Update( &context, inputbuf, input_len ) ;
     SHA1Final( out_digest, &context ) ;
}
#endif

void SHA1Init( SHA1Context *context)
{
     hash_init( EVP_sha1(), context ) ; 
}

void SHA1Update( SHA1Context *context, const void *data, uint32_t len)
{
     hash_update( context, data, len ) ;
}

void SHA1Final(unsigned char digest[SHA1_MAC_LEN], SHA1Context *context)
{
     hash_final( digest, context ) ;
}

void sha1_prf(const unsigned char *key, int key_len, const unsigned char *prefix, int prefix_len,
	      const u8 *data, int data_len, unsigned char *buf, int buf_len)
{
								u8 zero = 0, counter = 0;
								size_t pos, plen;
								u8 hash[SHA1_MAC_LEN];
								size_t label_len = strlen(prefix);
								const unsigned char *addr[4];
								size_t len[4];

								addr[0] = (u8 *) prefix;
								len[0] = prefix_len;
								addr[1] = &zero;
								len[1] = 1;
								addr[2] = data;
								len[2] = data_len;
								addr[3] = &counter;
								len[3] = 1;

								pos = 0;
								while (pos < buf_len) {
																plen = buf_len - pos;
																if (plen >= SHA1_MAC_LEN) {
																								hmac_sha1_vector(key, key_len, 4, addr, len,
																																								&buf[pos]);
																								pos += SHA1_MAC_LEN;
																} else {
																								hmac_sha1_vector(key, key_len, 4, addr, len,
																																								hash);
																								memcpy(&buf[pos], hash, plen);
																								break;
																}
																counter++;
								}
}


#if 0

/*
 * PRF -- Length of output is in octets rather than bits
 *     since length is always a multiple of 8 output array is
 *     organized so first N octets starting from 0 contains PRF output
 *
 *     supported inputs are 16, 32, 48, 64
 *     output array must be 80 octets to allow for sha1 overflow
 */
void sha1_prf( const unsigned char *key, int key_len, const unsigned char *prefix, int prefix_len, unsigned char *data, int data_len, 
               unsigned char *output, int len)
{
								int i;
								unsigned char input[1024]; /* concatenated input */
								int currentindex = 0;
								int total_len;

								memcpy(input, prefix, prefix_len);
								input[prefix_len] = 0; /* single octet 0 */
								memcpy(&input[prefix_len+1], data, data_len);
								total_len = prefix_len + 1 + data_len;
								input[total_len] = 0; /* single octet count, starts at 0 */
								total_len++;
								for(i = 0; i < (len+19)/20; i++) 
								{
																hmac_sha1(input, total_len, key, key_len,
																																&output[currentindex]);
																currentindex += 20;	/* next concatenation location */
																input[total_len-1]++; /* increment octet count */
								}
}
#endif

void pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
                int iterations, u8 *buf, size_t buflen)
{
#if __NO_OPENSSL__
        PBKDF2( buf, passphrase, strlen(passphrase), ssid, ssid_len, iterations, buflen, SHA ) ;
#else
        PKCS5_PBKDF2_HMAC_SHA1( passphrase, strlen(passphrase), ssid, ssid_len, iterations, buflen, buf ) ; 
#endif
}

