#include "cryptohash.h"
#include "sha256.h"

void hmac_sha256_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		      const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
     hmac_hash_vector( EVP_sha256(), key, key_len, num_elem, addr, len, mac ) ;
}

void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
	       uint8_t *mac)
{
					hmac_hash( EVP_sha256(), key, key_len, data, data_len, mac ) ; 
}

void sha256_vector(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
     hash_vector( EVP_sha256(), num_elem, addr, len, mac ) ;
}

void SHA256Init( SHA256Context *context)
{
     hash_init( EVP_sha256(), context ) ; 
}

void SHA256Update( SHA256Context *context, const void *data, uint32_t len)
{
     hash_update( context, data, len ) ;
}

void SHA256Final(unsigned char digest[SHA256_MAC_LEN], SHA256Context *context)
{
     hash_final( digest, context ) ;
}

#define WPA_PUT_LE16(a, val)   \
 do {     \
  (a)[1] = ((u16) (val)) >> 8; \
  (a)[0] = ((u16) (val)) & 0xff; \
 } while (0)


void sha256_prf( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len,
                 unsigned char *output, int buf_len )
{

								u16 counter = 1; /* was 0 in the previous drafts */
								size_t pos, plen;
								u8 hash[SHA256_MAC_LEN];
								const u8 *addr[4];
								size_t len[4];
								u8 counter_le[2], length_le[2];

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
																if (plen >= SHA256_MAC_LEN) {
																								hmac_sha256_vector(key, key_len, 4, addr, len,
																																								&output[pos]);
																								pos += SHA256_MAC_LEN;
																} else {
																								hmac_sha256_vector(key, key_len, 4, addr, len, hash);
																								memcpy(&output[pos], hash, plen);
																								break;
																}
																counter++;
								}
}

/**
 * hmac_sha256_kdf - HMAC-SHA256 based KDF (RFC 5295)
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
void hmac_sha256_kdf(const uint8_t *secret, size_t secret_len,
		    const uint8_t *label, const uint8_t *seed, size_t seed_len,
		    uint8_t *out, size_t outlen)
{
	uint8_t T[SHA256_MAC_LEN];
	uint8_t iter = 1;
	const uint8_t *addr[4];
	size_t len[4];
	size_t pos, clen;

	addr[0] = T;
	len[0] = SHA256_MAC_LEN;
	if (label) {
		addr[1] = (const uint8_t *) label;
		len[1] = strlen(label) + 1;
	} else {
		addr[1] = (const uint8_t *) "";
		len[1] = 0;
	}
	addr[2] = seed;
	len[2] = seed_len;
	addr[3] = &iter;
	len[3] = 1;

	hmac_sha256_vector(secret, secret_len, 3, &addr[1], &len[1], T);

	pos = 0;
	for (;;) {
		clen = outlen - pos;
		if (clen > SHA256_MAC_LEN)
			clen = SHA256_MAC_LEN;
		memcpy(out + pos, T, clen);
		pos += clen;

		if (pos == outlen)
			break;

		if (iter == 255) {
			memset(out, 0, outlen);
			memset(T, 0, SHA256_MAC_LEN);
			return;
		}
		iter++;

		hmac_sha256_vector(secret, secret_len, 4, addr, len, T);

	}

	memset(T, 0, SHA256_MAC_LEN);
}

