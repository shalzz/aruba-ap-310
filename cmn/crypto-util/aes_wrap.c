#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <aes_wrap.h>

static unsigned char gkek[24] = { 0x1e, 0xa5, 0xbe, 0x28, 0x1f, 0xaf, 0x21, 0x73,
								0xf8, 0x75, 0x8a, 0xb2, 0xc3, 0x49, 0x04, 0x50,
								0x32, 0x94, 0x13, 0x84, 0x81, 0x10, 0xc5, 0xb3};

#define BLOCK_SIZE 16

#if 0 //uncomment this EVP interface usage once it works
static void * aes_encrypt_init_mode(const uint8_t *key, const EVP_CIPHER *evp_cipher, const unsigned char *iv )
{
        EVP_CIPHER_CTX *p_ctx = EVP_CIPHER_CTX_new() ;
        EVP_CIPHER_CTX_init(p_ctx);
								EVP_EncryptInit_ex( p_ctx, evp_cipher, NULL, key, iv ) ;

								return p_ctx ;
}

static void aes_encrypt_len(void *ctx, const uint8_t *plain, uint8_t *crypt, int *cipher_len, int data_len )
{
        int c_len, f_len ;        
        EVP_CIPHER_CTX *p_ctx = (EVP_CIPHER_CTX*)ctx ;
        EVP_EncryptUpdate( p_ctx, crypt, &c_len, plain, data_len ) ;
        EVP_EncryptFinal_ex( p_ctx, crypt+c_len, &f_len ) ;
        *cipher_len  = c_len + f_len ;
}

void * aes_encrypt_init(const uint8_t *key, size_t len)
{ 
        return aes_encrypt_init_mode( key, EVP_aes_128_ecb(), NULL ) ;
}

void aes_encrypt(void *ctx, const uint8_t *plain, uint8_t *crypt)
{
        int cipher_len = BLOCK_SIZE ;
        aes_encrypt_len( ctx, plain, crypt, &cipher_len, BLOCK_SIZE ) ;
}

void aes_encrypt_deinit(void *ctx)
{
        EVP_CIPHER_CTX_cleanup( (EVP_CIPHER_CTX*)ctx ) ;
}

static void * aes_decrypt_init_mode(const uint8_t *key, const EVP_CIPHER *evp_cipher, const unsigned char *iv )
{
        EVP_CIPHER_CTX *p_ctx = EVP_CIPHER_CTX_new() ;
        EVP_CIPHER_CTX_init(p_ctx);
								EVP_DecryptInit_ex( p_ctx, evp_cipher, NULL, key, iv ) ;

								return p_ctx ;
}

static void aes_decrypt_len(void *ctx, const uint8_t *crypt, uint8_t *plain, int *plain_len, int cipher_len )
{
        int f_len = 0, plen = cipher_len ;
        EVP_CIPHER_CTX *p_ctx = (EVP_CIPHER_CTX*)ctx ;
        EVP_DecryptUpdate( p_ctx, plain, &plen, crypt, cipher_len ) ;
        EVP_DecryptFinal_ex( p_ctx, plain+plen, &f_len ) ;
								*plain_len = plen + f_len ;
}

void * aes_decrypt_init(const uint8_t *key, size_t len)
{
        return aes_decrypt_init_mode( key, EVP_aes_128_ecb(), NULL ) ;
}

void aes_decrypt(void *ctx, const uint8_t *crypt, uint8_t *plain)
{
        int plain_len ;
        aes_decrypt_len( ctx, crypt, plain, &plain_len, BLOCK_SIZE ) ;
}

void aes_decrypt_deinit(void *ctx)
{
        EVP_CIPHER_CTX_cleanup( (EVP_CIPHER_CTX*)ctx ) ;
}

#endif


void * aes_encrypt_init(const uint8_t *key, size_t len)
{
								AES_KEY *ak;
								ak = malloc(sizeof(*ak));
								if (ak == NULL)
																return NULL;
								if (AES_set_encrypt_key(key, 8 * len, ak) < 0) {
																free(ak);
																return NULL;
								}
								return ak;
}

void aes_encrypt(void *ctx, const uint8_t *plain, uint8_t *crypt)
{
								AES_encrypt(plain, crypt, ctx);
}


void aes_encrypt_deinit(void *ctx)
{
								free(ctx);
}


void * aes_decrypt_init(const uint8_t *key, size_t len)
{
								AES_KEY *ak;
								ak = malloc(sizeof(*ak));
								if (ak == NULL)
																return NULL;
								if (AES_set_decrypt_key(key, 8 * len, ak) < 0) {
																free(ak);
																return NULL;
								}
								return ak;
}

void aes_decrypt(void *ctx, const uint8_t *crypt, uint8_t *plain)
{
								AES_decrypt(crypt, plain, ctx);
}


void aes_decrypt_deinit(void *ctx)
{
								free(ctx);
}

/**
	* aes_wrap - Wrap keys with AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
	* @kek: Key encryption key (KEK)
	* @n: Length of the wrapped key in 64-bit units; e.g., 2 = 128-bit = 16 bytes
	* @plain: Plaintext key to be wrapped, n * 64 bit
	* @cipher: Wrapped key, (n + 1) * 64 bit
	* Returns: 0 on success, -1 on failure
	*/
int aes_wrap(const uint8_t *kek, int n, const uint8_t *plain, uint8_t *cipher)
{
								uint8_t *a, *r, b[16];
								int i, j;
								void *ctx;

								a = cipher;
								r = cipher + 8;

								/* 1) Initialize variables. */
								memset(a, 0xa6, 8); 
								memcpy(r, plain, 8 * n); 

								ctx = aes_encrypt_init(kek, 16);
								if (ctx == NULL)
																return -1; 

								/* 2) Calculate intermediate values.
									*   * For j = 0 to 5
									*     *     For i=1 to n
									*       *         B = AES(K, A | R[i])
									*         *         A = MSB(64, B) ^ t where t = (n*j)+i
									*           *         R[i] = LSB(64, B)
									*             */
								for (j = 0; j <= 5; j++) {
																r = cipher + 8;
																for (i = 1; i <= n; i++) {
																								memcpy(b, a, 8); 
																								memcpy(b + 8, r, 8); 
																								aes_encrypt(ctx, b, b); 
																								memcpy(a, b, 8); 
																								a[7] ^= n * j + i;
																								memcpy(r, b + 8, 8); 
																								r += 8;
																}
								}
								aes_encrypt_deinit(ctx);

								return 0;
}

/**
	* aes_unwrap - Unwrap key with AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
	* @kek: Key encryption key (KEK)
	* @n: Length of the wrapped key in 64-bit units; e.g., 2 = 128-bit = 16 bytes
	* @cipher: Wrapped key to be unwrapped, (n + 1) * 64 bit
	* @plain: Plaintext key, n * 64 bit
	* Returns: 0 on success, -1 on failure (e.g., integrity verification failed)
	*/
int aes_unwrap(const uint8_t *kek, int n, const uint8_t *cipher, uint8_t *plain)
{
								uint8_t a[8], *r, b[16];
								int i, j;
								void *ctx;

								/* 1) Initialize variables. */
								memcpy(a, cipher, 8);
								r = plain;
								memcpy(r, cipher + 8, 8 * n);

								ctx = aes_decrypt_init(kek, 16);
								if (ctx == NULL)
																return -1;

								/* 2) Compute intermediate values.
									*   * For j = 5 to 0
									*     *     For i = n to 1
									*       *         B = AES-1(K, (A ^ t) | R[i]) where t = n*j+i
									*         *         A = MSB(64, B)
									*           *         R[i] = LSB(64, B)
									*             */
								for (j = 5; j >= 0; j--) {
																r = plain + (n - 1) * 8;
																for (i = n; i >= 1; i--) {
																								memcpy(b, a, 8);
																								b[7] ^= n * j + i;

																								memcpy(b + 8, r, 8);
																								aes_decrypt(ctx, b, b);
																								memcpy(a, b, 8);
																								memcpy(r, b + 8, 8);
																								r -= 8;
																}
								}
								aes_decrypt_deinit(ctx);

								/* 3) Output results.
									*   *
									*     * These are already in @plain due to the location of temporary
									*       * variables. Just verify that the IV matches with the expected value.
									*         */
								for (i = 0; i < 8; i++) {
																if (a[i] != 0xa6)
																								return -1;
								}

								return 0;
}

int aes_wrap_wrapper(int n, const uint8_t *plain, uint8_t *cipher)
{
								return aes_wrap(gkek, n, plain, cipher);
}

int aes_unwrap_wrapper(int n, const uint8_t *cipher, uint8_t *plain)
{
								return aes_unwrap(gkek, n, cipher, plain);
}

/**
 * aes_128_encrypt_block - Perform one AES 128-bit block operation
 * @key: Key for AES
 * @in: Input data (16 bytes)
 * @out: Output of the AES block operation (16 bytes)
 * Returns: 0 on success, -1 on failure
 */
int aes_128_encrypt_block(const uint8_t *key, const uint8_t *in, uint8_t *out)
{
	void *ctx;
	ctx = aes_encrypt_init(key, 16);
	if (ctx == NULL)
		return -1;
	aes_encrypt(ctx, in, out);
	aes_encrypt_deinit(ctx);
	return 0;
}



/**
	* omac1_aes_128_vector - One-Key CBC MAC (OMAC1) hash with AES-128
	* @key: 128-bit key for the hash operation
	* @num_elem: Number of elements in the data vector
	* @addr: Pointers to the data areas
	* @len: Lengths of the data blocks
	* @mac: Buffer for MAC (128 bits, i.e., 16 bytes)
	* Returns: 0 on success, -1 on failure
	*
	* This is a mode for using block cipher (AES in this case) for authentication.
	* OMAC1 was standardized with the name CMAC by NIST in a Special Publication
	* (SP) 800-38B.
	*/

static void gf_mulx(uint8_t *pad)
{
								int i, carry;

								carry = pad[0] & 0x80;
								for (i = 0; i < BLOCK_SIZE - 1; i++)
																pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
								pad[BLOCK_SIZE - 1] <<= 1;
								if (carry)
																pad[BLOCK_SIZE - 1] ^= 0x87;
}


/**
	* omac1_aes_128 - One-Key CBC MAC (OMAC1) hash with AES-128 (aka AES-CMAC)
	* @key: 128-bit key for the hash operation
	* @data: Data buffer for which a MAC is determined
	* @data_len: Length of data buffer in bytes
	* @mac: Buffer for MAC (128 bits, i.e., 16 bytes)
	* Returns: 0 on success, -1 on failure
	*
	* This is a mode for using block cipher (AES in this case) for authentication.
	* OMAC1 was standardized with the name CMAC by NIST in a Special Publication
	* (SP) 800-38B.
	*/
int omac1_aes_128(const uint8_t *key, const uint8_t *data, size_t data_len, uint8_t *mac)
{
								void *ctx;
								uint8_t cbc[BLOCK_SIZE], pad[BLOCK_SIZE];
								const uint8_t *pos = data;
								size_t i, left = data_len;

								ctx = aes_encrypt_init(key, 16);
								if (ctx == NULL)
																return -1; 
								memset(cbc, 0, BLOCK_SIZE);

								while (left >= BLOCK_SIZE) {
																for (i = 0; i < BLOCK_SIZE; i++)
																								cbc[i] ^= *pos++;
																if (left > BLOCK_SIZE)
																								aes_encrypt(ctx, cbc, cbc);
																left -= BLOCK_SIZE;
								}

								memset(pad, 0, BLOCK_SIZE);
								aes_encrypt(ctx, pad, pad);
								gf_mulx(pad);

								if (left || data_len == 0) {
																for (i = 0; i < left; i++)
																								cbc[i] ^= *pos++;
																cbc[left] ^= 0x80;
																gf_mulx(pad);
								}

								for (i = 0; i < BLOCK_SIZE; i++)
																pad[i] ^= cbc[i];
								aes_encrypt(ctx, pad, mac);
								aes_encrypt_deinit(ctx);
								return 0;
}


/**
	* aes_128_ctr_encrypt - AES-128 CTR mode encryption
	* @key: Key for encryption (16 bytes)
	* @nonce: Nonce for counter mode (16 bytes)
	* @data: Data to encrypt in-place
	* @data_len: Length of data in bytes
	* Returns: 0 on success, -1 on failure
	*/
int aes_128_ctr_encrypt(const uint8_t *key, const uint8_t *nonce,
																uint8_t *data, size_t data_len)
{
								void *ctx;
								size_t j, len, left = data_len;
								int i;
								uint8_t *pos = data;
								uint8_t counter[BLOCK_SIZE], buf[BLOCK_SIZE];

								ctx = aes_encrypt_init(key, 16);
								if (ctx == NULL)
																return -1; 
								memcpy(counter, nonce, BLOCK_SIZE);

								while (left > 0) {
																aes_encrypt(ctx, counter, buf);

																len = (left < BLOCK_SIZE) ? left : BLOCK_SIZE;
																for (j = 0; j < len; j++)
																								pos[j] ^= buf[j];
																pos += len;
																left -= len;

																for (i = BLOCK_SIZE - 1; i >= 0; i--) {
																								counter[i]++;
																								if (counter[i])
																																break;
																}
								}
								aes_encrypt_deinit(ctx);
								return 0;
}

/**
	* aes_128_cbc_encrypt - AES-128 CBC encryption
	* @key: Encryption key
	* @iv: Encryption IV for CBC mode (16 bytes)
	* @data: Data to encrypt in-place
	* @data_len: Length of data in bytes (must be divisible by 16)
	* Returns: 0 on success, -1 on failure
	*/
int aes_128_cbc_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len)
{
        int c_len, f_len ;
								EVP_CIPHER_CTX ctx  ;
        EVP_CIPHER_CTX_init(&ctx);
								EVP_EncryptInit_ex( &ctx, EVP_aes_128_cbc(), NULL, key, iv ) ;
        EVP_EncryptUpdate( &ctx, data, &c_len, data, data_len ) ;
        EVP_EncryptFinal( &ctx, data+c_len, &f_len ) ;

        return 0 ;
}


/**
	* aes_128_cbc_decrypt - AES-128 CBC decryption
	* @key: Decryption key
	* @iv: Decryption IV for CBC mode (16 bytes)
	* @data: Data to decrypt in-place
	* @data_len: Length of data in bytes (must be divisible by 16)
	* Returns: 0 on success, -1 on failure
	*/
int aes_128_cbc_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *ciphertext, size_t cipher_len)
{
        int f_len = 0;
        EVP_CIPHER_CTX ctx ; 
        EVP_CIPHER_CTX_init(&ctx);
								EVP_DecryptInit_ex( &ctx, EVP_aes_128_cbc(), NULL, key, iv ) ;
        EVP_DecryptUpdate( &ctx, ciphertext, (int *)&cipher_len, ciphertext, cipher_len ) ;
        EVP_DecryptFinal( &ctx, ciphertext+cipher_len, &f_len ) ;
								
        return 0;
}


/**
 * aes_128_eax_encrypt - AES-128 EAX mode encryption
 * @key: Key for encryption (16 bytes)
 * @nonce: Nonce for counter mode
 * @nonce_len: Nonce length in bytes
 * @hdr: Header data to be authenticity protected
 * @hdr_len: Length of the header data bytes
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes
 * @tag: 16-byte tag value
 * Returns: 0 on success, -1 on failure
 */
int aes_128_eax_encrypt(const uint8_t *key, const uint8_t *nonce, size_t nonce_len,
			const uint8_t *hdr, size_t hdr_len,
			uint8_t *data, size_t data_len, uint8_t *tag)
{
								uint8_t *buf;
								size_t buf_len;
								uint8_t nonce_mac[BLOCK_SIZE], hdr_mac[BLOCK_SIZE], data_mac[BLOCK_SIZE];
								int i, ret = -1;

								if (nonce_len > data_len)
																buf_len = nonce_len;
								else
																buf_len = data_len;
								if (hdr_len > buf_len)
																buf_len = hdr_len;
								buf_len += 16;

								buf = malloc(buf_len);
								if (buf == NULL)
																return -1;

								memset(buf, 0, 15);

								buf[15] = 0;
								memcpy(buf + 16, nonce, nonce_len);
								if (omac1_aes_128(key, buf, 16 + nonce_len, nonce_mac))
																goto fail;

								buf[15] = 1;
								memcpy(buf + 16, hdr, hdr_len);
								if (omac1_aes_128(key, buf, 16 + hdr_len, hdr_mac))
																goto fail;

								if (aes_128_ctr_encrypt(key, nonce_mac, data, data_len))
																goto fail;
								buf[15] = 2;
								memcpy(buf + 16, data, data_len);
								if (omac1_aes_128(key, buf, 16 + data_len, data_mac))
																goto fail;

								for (i = 0; i < BLOCK_SIZE; i++)
																tag[i] = nonce_mac[i] ^ data_mac[i] ^ hdr_mac[i];

								ret = 0;
fail:
								free(buf);

								return ret;
}


/**
 * aes_128_eax_decrypt - AES-128 EAX mode decryption
 * @key: Key for decryption (16 bytes)
 * @nonce: Nonce for counter mode
 * @nonce_len: Nonce length in bytes
 * @hdr: Header data to be authenticity protected
 * @hdr_len: Length of the header data bytes
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes
 * @tag: 16-byte tag value
 * Returns: 0 on success, -1 on failure, -2 if tag does not match
 */
int aes_128_eax_decrypt(const uint8_t *key, const uint8_t *nonce, size_t nonce_len,
			const uint8_t *hdr, size_t hdr_len,
			uint8_t *data, size_t data_len, const uint8_t *tag)
{
								uint8_t *buf;
								size_t buf_len;
								uint8_t nonce_mac[BLOCK_SIZE], hdr_mac[BLOCK_SIZE], data_mac[BLOCK_SIZE];
								int i;

								if (nonce_len > data_len)
																buf_len = nonce_len;
								else
																buf_len = data_len;
								if (hdr_len > buf_len)
																buf_len = hdr_len;
								buf_len += 16;

								buf = malloc(buf_len);
								if (buf == NULL)
																return -1;

								memset(buf, 0, 15);

								buf[15] = 0;
								memcpy(buf + 16, nonce, nonce_len);
								if (omac1_aes_128(key, buf, 16 + nonce_len, nonce_mac)) {
																free(buf);
																return -1;
								}

								buf[15] = 1;
								memcpy(buf + 16, hdr, hdr_len);
								if (omac1_aes_128(key, buf, 16 + hdr_len, hdr_mac)) {
																free(buf);
																return -1;
								}

								buf[15] = 2;
								memcpy(buf + 16, data, data_len);
								if (omac1_aes_128(key, buf, 16 + data_len, data_mac)) {
																free(buf);
																return -1;
								}

								free(buf);

								for (i = 0; i < BLOCK_SIZE; i++) {
																if (tag[i] != (nonce_mac[i] ^ data_mac[i] ^ hdr_mac[i]))
																								return -2;
								}

								return aes_128_ctr_encrypt(key, nonce_mac, data, data_len);
}

