#include "cryptohash.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>


void hmac_hash_vector( const EVP_MD *digest_ptr, const uint8_t *key, int key_len, int num_elem,
																const uint8_t *addr[], const int *len, uint8_t *mac)
{
        int iter = 0 ;
        unsigned int mac_len = 0 ;

        HMAC_CTX ctx ;
        HMAC_Init( &ctx, key, key_len, digest_ptr ) ;
  
        for( iter = 0 ; iter < num_elem; iter++ )
        {
            HMAC_Update( &ctx, addr[iter], len[iter] );
        }
 
        HMAC_Final( &ctx, mac, &mac_len ) ; 
        HMAC_cleanup( &ctx) ;
}


void hmac_hash( const EVP_MD *digest_ptr, const uint8_t *key, int key_len, const uint8_t *data, int data_len,
																uint8_t *mac)
{
    HMAC_CTX ctx;

    HMAC_Init(&ctx, key, key_len, digest_ptr ) ;
    HMAC_Update(&ctx, data, data_len);
    HMAC_Final(&ctx, mac, NULL);
    HMAC_cleanup( &ctx) ;
}


void hash_vector( const EVP_MD *digest_ptr, size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
								size_t i;
								int digest_len = 0 ; 

								EVP_MD_CTX digest_ctx ;
								EVP_MD_CTX_init( &digest_ctx );
								EVP_DigestInit( &digest_ctx, digest_ptr);

								for (i = 0; i < num_elem; i++)
								{   
																EVP_DigestUpdate( &digest_ctx, addr[i], len[i] ) ; 
								}   

								EVP_DigestFinal_ex( &digest_ctx, mac, &digest_len ) ; 
								EVP_MD_CTX_cleanup( &digest_ctx ) ; 
}


void hash_init( const EVP_MD *digest_ptr, struct hash_context *context)
{
								EVP_MD_CTX_init( &context->digest_ctx );
								EVP_DigestInit( &context->digest_ctx, digest_ptr);
}

void hash_update( struct hash_context *context, const void *data, uint32_t len)
{ 
								EVP_DigestUpdate( &context->digest_ctx, data, len ) ;
}

void hash_final( unsigned char* digest, struct hash_context *context)
{
								EVP_DigestFinal_ex(  &context->digest_ctx, digest, &context->digest_len ) ;
								EVP_MD_CTX_cleanup( &context->digest_ctx ) ;
}

