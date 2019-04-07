#include "cryptohash.h"
#include "sha384.h"

#define WPA_PUT_LE16(a, val)   \
 do {     \
  (a)[1] = ((u16) (val)) >> 8; \
  (a)[0] = ((u16) (val)) & 0xff; \
 } while (0)

void hmac_sha384_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		      const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
     hmac_hash_vector( EVP_sha384(), key, key_len, num_elem, addr, len, mac ) ;
}

void hmac_sha384(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
	       uint8_t *mac)
{
					hmac_hash( EVP_sha384(), key, key_len, data, data_len, mac ) ; 
}

void sha384_vector(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
     hash_vector( EVP_sha384(), num_elem, addr, len, mac ) ;
}

void SHA384Init( SHA384Context *context)
{
     hash_init( EVP_sha384(), context ) ; 
}

void SHA384Update( SHA384Context *context, const void *data, uint32_t len)
{
     hash_update( context, data, len ) ;
}

void SHA384Final(unsigned char digest[SHA384_MAC_LEN], SHA384Context *context)
{
     hash_final( digest, context ) ;
}


void sha384_prf( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len, unsigned char *output, int len)
{
        int i = 0;
        unsigned char input[1024]; /* concatenated input */
        int currentindex = 0;
        int total_len = 0;
        u16 counter = 1; //16 bit counter
        u16 length = 0; // 16 bit length

        length = (u16) len;

        // Concatenate counter||prefix||data||length
        memcpy(input, &counter, 2); // counter is big endian unsigned 16-bit integer
        total_len += 2;
        memcpy(&input[total_len], prefix, prefix_len);
        total_len += prefix_len;
        input[total_len] = 0; /* single null octet */
        total_len += 1;
        memcpy(&input[total_len], data, data_len);
        total_len += data_len;
        memcpy(&input[total_len], &length, 2); // length is big endian unsigned 16-bit integer
        total_len += 2;
        
        for(i = 0; i < (len+47)/48; i++)
        {
                hmac_sha384(input, total_len, key, key_len, &output[currentindex]);
                currentindex += 48;     /* next concatenation location */
                counter ++; // increment counter
                memcpy(input, &counter,2); //update counter in input
        }
}

/**
 * hmac_sha384_kdf - HMAC-SHA384 based KDF (RFC 5295)
 * @secret: Key for KDF
 * @secret_len: Length of the key in bytes
 * @label: A unique label for each purpose of the KDF or %NULL to select
 *	RFC 5869 HKDF-Expand() with arbitrary seed (= info)
 * @seed: Seed value to bind into the key
 * @seed_len: Length of the seed
 * @out: Buffer for the generated pseudo-random key
 * @outlen: Number of bytes of key to generate
 * Returns: 0 on success, -1 on failure.
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key in ERP. This KDF is defined in RFC 5295, Chapter 3.1.2. When used
 * with label = NULL and seed = info, this matches HKDF-Expand() defined in
 * RFC 5869, Chapter 2.3.
 */
void hmac_sha384_kdf(const u8 *secret, size_t secret_len,
		    const char *label, const u8 *seed, size_t seed_len,
		    u8 *out, size_t outlen)
{
	u8 T[SHA384_MAC_LEN];
	u8 iter = 1;
	const unsigned char *addr[4];
	size_t len[4];
	size_t pos, clen;

	addr[0] = T;
	len[0] = SHA384_MAC_LEN;
	if (label) {
		addr[1] = (const unsigned char *) label;
		len[1] = strlen(label) + 1;
	} else {
		addr[1] = (const u8 *) "";
		len[1] = 0;
	}
	addr[2] = seed;
	len[2] = seed_len;
	addr[3] = &iter;
	len[3] = 1;

	hmac_sha384_vector(secret, secret_len, 3, &addr[1], &len[1], T);

	pos = 0;
	for (;;) {
		clen = outlen - pos;
		if (clen > SHA384_MAC_LEN)
			clen = SHA384_MAC_LEN;
		memcpy(out + pos, T, clen);
		pos += clen;

		if (pos == outlen)
			break;

		if (iter == 255) {
			memset(out, 0, outlen);
			memset(T, 0, SHA384_MAC_LEN);
			return;
		}
		iter++;

		hmac_sha384_vector(secret, secret_len, 4, addr, len, T);
	}

	memset(T, 0, SHA384_MAC_LEN);
	return;
}



void kdf_sha384_704( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len,
                unsigned char *output, int buf_len)
{
        u16 counter = 1;
        size_t pos, plen;
        u8 hash[SHA384_MAC_LEN];
        const u8 *addr[4];
        size_t len[4];
        u8 counter_le[2], length_le[2];
       // size_t buf_len = (buf_len_bits + 7) / 8;

        addr[0] = counter_le;
        len[0] = 2;
        addr[1] = (u8 *) prefix;
        len[1] = prefix_len;
        addr[2] = data;
        len[2] = data_len;
        addr[3] = length_le;
        len[3] = sizeof(length_le);

        WPA_PUT_LE16(length_le, buf_len * 8);
        pos = 0;
        while (pos < buf_len) {
                plen = buf_len - pos;
                WPA_PUT_LE16(counter_le, counter);
                if (plen >= SHA384_MAC_LEN) {
                        hmac_sha384_vector(key, key_len, 4, addr, len, &output[pos]);
                        pos += SHA384_MAC_LEN;
                } else {
                        hmac_sha384_vector(key, key_len, 4, addr, len, hash);
                        memcpy(&output[pos], hash, plen);
             //           pos += plen;
                        break;
                }
                counter++;
        }
}
