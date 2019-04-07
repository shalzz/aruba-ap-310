/*
 *  The RSA public-key cryptosystem
 *
 *  Copyright (C) 2006-2007  Christophe Devine
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of XySSL nor the names of its contributors may be
 *      used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  RSA was designed by Ron Rivest, Adi Shamir and Len Adleman.
 *
 *  http://theory.lcs.mit.edu/~rivest/rsapaper.pdf
 *  http://www.cacr.math.uwaterloo.ca/hac/about/chap8.pdf
 */

#include <common.h>
#include <rngs.h>

#include "xyssl/config.h"

#if defined(XYSSL_RSA_C)

#include "xyssl/rsa.h"
#ifdef FIPS_TEST
#include <watchdog.h>
#include <malloc.h>
#define SHA1_RESULT_SIZE (20)
#define RSA_MAX_TEST_MODULUS_BITS 4000
#define RSA_MAX_TEST_MODULUS_HEX RSA_MAX_TEST_MODULUS_BITS/4
#define RSA_MAX_TEST_MODULUS_BYTES    RSA_MAX_TEST_MODULUS_BITS/8
#define RSA_MAX_TEST_EXPONENT_BYTES   8
#define isspace(x) (((x) == ' ') || ((x) == '\t'))
#define isxdigit(c)     (('0' <= (c) && (c) <= '9') \
	|| ('a' <= (c) && (c) <= 'f') \
	|| ('A' <= (c) && (c) <= 'F'))
extern int atoi(char *string);
extern int hex_to_byteval (const char *c2, unsigned char *byteval);
int rsa_sigver_test (char *fips_buf, int bufSize);
#endif
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>

/*
 * Initialize an RSA context
 */
void rsa_init( rsa_context *ctx,
               int padding,
               int hash_id,
               int (*f_rng)(void *),
               void *p_rng )
{
    memset( ctx, 0, sizeof( rsa_context ) );

    ctx->padding = padding;
    ctx->hash_id = hash_id;

    ctx->f_rng = f_rng;
    ctx->p_rng = p_rng;
}

#if defined(XYSSL_GENPRIME)

/*
 * Generate an RSA keypair
 */
int rsa_gen_key( rsa_context *ctx, int nbits, int exponent )
{
    int ret;
    mpi P1, Q1, H, G;

    if( ctx->f_rng == NULL || nbits < 128 || exponent < 3 )
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );

    mpi_init( &P1, &Q1, &H, &G, NULL );

    /*
     * find primes P and Q with Q < P so that:
     * GCD( E, (P-1)*(Q-1) ) == 1
     */
    MPI_CHK( mpi_lset( &ctx->E, exponent ) );

    do
    {
        MPI_CHK( mpi_gen_prime( &ctx->P, ( nbits + 1 ) >> 1, 0, 
                                ctx->f_rng, ctx->p_rng ) );

        MPI_CHK( mpi_gen_prime( &ctx->Q, ( nbits + 1 ) >> 1, 0,
                                ctx->f_rng, ctx->p_rng ) );

        if( mpi_cmp_mpi( &ctx->P, &ctx->Q ) < 0 )
            mpi_swap( &ctx->P, &ctx->Q );

        if( mpi_cmp_mpi( &ctx->P, &ctx->Q ) == 0 )
            continue;

        MPI_CHK( mpi_mul_mpi( &ctx->N, &ctx->P, &ctx->Q ) );
        if( mpi_msb( &ctx->N ) != nbits )
            continue;

        MPI_CHK( mpi_sub_int( &P1, &ctx->P, 1 ) );
        MPI_CHK( mpi_sub_int( &Q1, &ctx->Q, 1 ) );
        MPI_CHK( mpi_mul_mpi( &H, &P1, &Q1 ) );
        MPI_CHK( mpi_gcd( &G, &ctx->E, &H  ) );
    }
    while( mpi_cmp_int( &G, 1 ) != 0 );

    /*
     * D  = E^-1 mod ((P-1)*(Q-1))
     * DP = D mod (P - 1)
     * DQ = D mod (Q - 1)
     * QP = Q^-1 mod P
     */
    MPI_CHK( mpi_inv_mod( &ctx->D , &ctx->E, &H  ) );
    MPI_CHK( mpi_mod_mpi( &ctx->DP, &ctx->D, &P1 ) );
    MPI_CHK( mpi_mod_mpi( &ctx->DQ, &ctx->D, &Q1 ) );
    MPI_CHK( mpi_inv_mod( &ctx->QP, &ctx->Q, &ctx->P ) );

    ctx->len = ( mpi_msb( &ctx->N ) + 7 ) >> 3;

cleanup:

    mpi_free( &G, &H, &Q1, &P1, NULL );

    if( ret != 0 )
    {
        rsa_free( ctx );
        return( XYSSL_ERR_RSA_KEY_GEN_FAILED | ret );
    }

    return( 0 );   
}

#endif

/*
 * Check a public RSA key
 */
int rsa_check_pubkey( rsa_context *ctx )
{
    if( ( ctx->N.p[0] & 1 ) == 0 || 
        ( ctx->E.p[0] & 1 ) == 0 ) {
        printf("\n P XYSSL_ERR_RSA_KEY_CHECK_FAILED ");
        return( XYSSL_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mpi_msb( &ctx->N ) < 128 ||
        mpi_msb( &ctx->N ) > 4096 ) {
        printf("\n N XYSSL_ERR_RSA_KEY_CHECK_FAILED ");
        return( XYSSL_ERR_RSA_KEY_CHECK_FAILED );
    }

    if( mpi_msb( &ctx->E ) < 2 ||
        mpi_msb( &ctx->E ) > 64 ) {
        printf("\n E XYSSL_ERR_RSA_KEY_CHECK_FAILED ");
        return( XYSSL_ERR_RSA_KEY_CHECK_FAILED );
    }

    return( 0 );
}

/*
 * Check a private RSA key
 */
int rsa_check_privkey( rsa_context *ctx )
{
    int ret;
    mpi PQ, DE, P1, Q1, H, I, G;

    if( ( ret = rsa_check_pubkey( ctx ) ) != 0 )
        return( ret );

    mpi_init( &PQ, &DE, &P1, &Q1, &H, &I, &G, NULL );

    MPI_CHK( mpi_mul_mpi( &PQ, &ctx->P, &ctx->Q ) );
    MPI_CHK( mpi_mul_mpi( &DE, &ctx->D, &ctx->E ) );
    MPI_CHK( mpi_sub_int( &P1, &ctx->P, 1 ) );
    MPI_CHK( mpi_sub_int( &Q1, &ctx->Q, 1 ) );
    MPI_CHK( mpi_mul_mpi( &H, &P1, &Q1 ) );
    MPI_CHK( mpi_mod_mpi( &I, &DE, &H  ) );
    MPI_CHK( mpi_gcd( &G, &ctx->E, &H  ) );

    if( mpi_cmp_mpi( &PQ, &ctx->N ) == 0 &&
        mpi_cmp_int( &I, 1 ) == 0 &&
        mpi_cmp_int( &G, 1 ) == 0 )
    {
        mpi_free( &G, &I, &H, &Q1, &P1, &DE, &PQ, NULL );
        return( 0 );
    }

cleanup:

    mpi_free( &G, &I, &H, &Q1, &P1, &DE, &PQ, NULL );
    return( XYSSL_ERR_RSA_KEY_CHECK_FAILED | ret );
}

/*
 * Do an RSA public key operation
 */
int rsa_public( rsa_context *ctx,
                unsigned char *input,
                unsigned char *output )
{
    int ret, olen ;
    mpi T;

    mpi_init( &T, NULL );

    MPI_CHK( mpi_read_binary( &T, input, ctx->len ) );

    if( mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        mpi_free( &T, NULL );
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );
    }

    olen = ctx->len;
    MPI_CHK( mpi_exp_mod( &T, &T, &ctx->E, &ctx->N, &ctx->RN ) );
    MPI_CHK( mpi_write_binary( &T, output, olen ) );

cleanup:

    mpi_free( &T, NULL );

    if( ret != 0 )
        return( XYSSL_ERR_RSA_PUBLIC_FAILED | ret );

    return( 0 );
}

/*
 * Do an RSA private key operation
 */
int rsa_private( rsa_context *ctx,
                 unsigned char *input,
                 unsigned char *output )
{
    int ret, olen;
    mpi T, T1, T2;

    mpi_init( &T, &T1, &T2, NULL );

    MPI_CHK( mpi_read_binary( &T, input, ctx->len ) );

    if( mpi_cmp_mpi( &T, &ctx->N ) >= 0 )
    {
        mpi_free( &T, NULL );
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );
    }

#if 0
    MPI_CHK( mpi_exp_mod( &T, &T, &ctx->D, &ctx->N, &ctx->RN ) );
#else
    /*
     * faster decryption using the CRT
     *
     * T1 = input ^ dP mod P
     * T2 = input ^ dQ mod Q
     */
    MPI_CHK( mpi_exp_mod( &T1, &T, &ctx->DP, &ctx->P, &ctx->RP ) );
    MPI_CHK( mpi_exp_mod( &T2, &T, &ctx->DQ, &ctx->Q, &ctx->RQ ) );

    /*
     * T = (T1 - T2) * (Q^-1 mod P) mod P
     */
    MPI_CHK( mpi_sub_mpi( &T, &T1, &T2 ) );
    MPI_CHK( mpi_mul_mpi( &T1, &T, &ctx->QP ) );
    MPI_CHK( mpi_mod_mpi( &T, &T1, &ctx->P ) );

    /*
     * output = T2 + T * Q
     */
    MPI_CHK( mpi_mul_mpi( &T1, &T, &ctx->Q ) );
    MPI_CHK( mpi_add_mpi( &T, &T2, &T1 ) );
#endif

    olen = ctx->len;
    MPI_CHK( mpi_write_binary( &T, output, olen ) );

cleanup:

    mpi_free( &T, &T1, &T2, NULL );

    if( ret != 0 )
        return( XYSSL_ERR_RSA_PRIVATE_FAILED | ret );

    return( 0 );
}

/*
 * Add the message padding, then do an RSA operation
 */
int rsa_pkcs1_encrypt( rsa_context *ctx,
                       int mode, int  ilen,
                       unsigned char *input,
                       unsigned char *output )
{
    int nb_pad, olen;
    unsigned char *p = output;

    olen = ctx->len;

    switch( ctx->padding )
    {
        case RSA_PKCS_V15:

            if( ilen < 0 || olen < ilen + 11 )
                return( XYSSL_ERR_RSA_BAD_INPUT_DATA );

            nb_pad = olen - 3 - ilen;

            *p++ = 0;
            *p++ = RSA_CRYPT;

            while( nb_pad-- > 0 )
            {
                do {
                    *p = (unsigned char) rand();
                } while( *p == 0 );
                p++;
            }
            *p++ = 0;
            memcpy( p, input, ilen );
            break;

        default:

            return( XYSSL_ERR_RSA_INVALID_PADDING );
    }

    return( ( mode == RSA_PUBLIC )
            ? rsa_public(  ctx, output, output )
            : rsa_private( ctx, output, output ) );
}

/*
 * Do an RSA operation, then remove the message padding
 */
int rsa_pkcs1_decrypt( rsa_context *ctx,
                       int mode, int *olen,
                       unsigned char *input,
                       unsigned char *output )
{
    int ret, ilen;
    unsigned char *p;
    unsigned char buf[512];

    ilen = ctx->len;

    if( ilen < 16 || ilen > (int) sizeof( buf ) )
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );

    ret = ( mode == RSA_PUBLIC )
          ? rsa_public(  ctx, input, buf )
          : rsa_private( ctx, input, buf );

    if( ret != 0 )
        return( ret );

    p = buf;

    switch( ctx->padding )
    {
        case RSA_PKCS_V15:

            if( *p++ != 0 || *p++ != RSA_CRYPT )
                return( XYSSL_ERR_RSA_INVALID_PADDING );

            while( *p != 0 )
            {
                if( p >= buf + ilen - 1 )
                    return( XYSSL_ERR_RSA_INVALID_PADDING );
                p++;
            }
            p++;
            break;

        default:

            return( XYSSL_ERR_RSA_INVALID_PADDING );
    }

    *olen = ilen - (int)(p - buf);
    memcpy( output, p, *olen );

    return( 0 );
}

/*
 * Do an RSA operation to sign the message digest
 */
int rsa_pkcs1_sign( rsa_context *ctx,
                    int mode,
                    int hash_id,
                    int hashlen,
                    unsigned char *hash,
                    unsigned char *sig )
{
    int nb_pad, olen;
    unsigned char *p = sig;

    olen = ctx->len;

    switch( ctx->padding )
    {
        case RSA_PKCS_V15:

            switch( hash_id )
            {
                case RSA_RAW:
                    nb_pad = olen - 3 - hashlen;
                    break;

                case RSA_MD2:
                case RSA_MD4:
                case RSA_MD5:
                    nb_pad = olen - 3 - 34;
                    break;

                case RSA_SHA1:
                    nb_pad = olen - 3 - 35;
                    break;

                default:
                    return( XYSSL_ERR_RSA_BAD_INPUT_DATA );
            }

            if( nb_pad < 8 )
                return( XYSSL_ERR_RSA_BAD_INPUT_DATA );

            *p++ = 0;
            *p++ = RSA_SIGN;
            memset( p, 0xFF, nb_pad );
            p += nb_pad;
            *p++ = 0;
            break;

        default:

            return( XYSSL_ERR_RSA_INVALID_PADDING );
    }

    switch( hash_id )
    {
        case RSA_RAW:
            memcpy( p, hash, hashlen );
            break;

        case RSA_MD2:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 2; break;

        case RSA_MD4:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 4; break;

        case RSA_MD5:
            memcpy( p, ASN1_HASH_MDX, 18 );
            memcpy( p + 18, hash, 16 );
            p[13] = 5; break;

        case RSA_SHA1:
            memcpy( p, ASN1_HASH_SHA1, 15 );
            memcpy( p + 15, hash, 20 );
            break;

	 case RSA_SHA256:
            memcpy( p, ASN1_HASH_SHA256, 19 );
            memcpy( p + 19, hash, 32 );
            break;

        default:
            return( XYSSL_ERR_RSA_BAD_INPUT_DATA );
    }

    return( ( mode == RSA_PUBLIC )
            ? rsa_public(  ctx, sig, sig )
            : rsa_private( ctx, sig, sig ) );
}

/*
 * Do an RSA operation and check the message digest
 */
int rsa_pkcs1_verify( rsa_context *ctx,
                      int mode,
                      int hash_id,
                      int hashlen,
                      unsigned char *hash,
                      unsigned char *sig )
{
    int ret, len, siglen;
    unsigned char *p, c;
    unsigned char buf[512];

    siglen = ctx->len;

    if( siglen < 16 || siglen > (int) sizeof( buf ) ) {
        printf("\n bad input ");
        return( XYSSL_ERR_RSA_BAD_INPUT_DATA );
    }

    ret = ( mode == RSA_PUBLIC )
          ? rsa_public(  ctx, sig, buf )
          : rsa_private( ctx, sig, buf );

    if( ret != 0 ) {
        printf("\nrsa public failed ");
        return( ret );
    }

    p = buf;

    switch( ctx->padding )
    {
        case RSA_PKCS_V15:

            if( *p++ != 0 || *p++ != RSA_SIGN ) {
                printf("\n invalid pad rsa sign ");
                return( XYSSL_ERR_RSA_INVALID_PADDING );
            }

            while( *p != 0 )
            {
                if( p >= buf + siglen - 1 || *p != 0xFF ) {
                    printf("\n invalid pad in while ");
                    return( XYSSL_ERR_RSA_INVALID_PADDING );
                }
                p++;
            }
            p++;
            break;

        default:
            printf("\n default case - not pkcs v15 "); 
            return( XYSSL_ERR_RSA_INVALID_PADDING );
    }

    len = siglen - (int)( p - buf );

    if( len == 34 )
    {
        c = p[13];
        p[13] = 0;

        if( memcmp( p, ASN1_HASH_MDX, 18 ) != 0 ) {
            return( XYSSL_ERR_RSA_VERIFY_FAILED );
        }

        if( ( c == 2 && hash_id == RSA_MD2 ) ||
            ( c == 4 && hash_id == RSA_MD4 ) ||
            ( c == 5 && hash_id == RSA_MD5 ) )
        {
            if( memcmp( p + 18, hash, 16 ) == 0 ) 
                return( 0 );
            else {
                printf("\n memcmp failed rsa verify ");
                return( XYSSL_ERR_RSA_VERIFY_FAILED );
            }
        }
    }

    if( len == 35 && hash_id == RSA_SHA1 )
    {
        if( memcmp( p, ASN1_HASH_SHA1, 15 ) == 0 &&
            memcmp( p + 15, hash, 20 ) == 0 )
            return( 0 );
        else {
            return( XYSSL_ERR_RSA_VERIFY_FAILED );
        }
   }
    if( len == 51 && hash_id == RSA_SHA256 )
    {
        if( memcmp( p, ASN1_HASH_SHA256, 19 ) == 0 &&
                memcmp( p + 19, hash, 32 ) == 0 )
            return( 0 );
        else {
            return( XYSSL_ERR_RSA_VERIFY_FAILED );
        }
    }

    if( len == hashlen && hash_id == RSA_RAW )
    {
        if( memcmp( p, hash, hashlen ) == 0 )
            return( 0 );
        else { 
            return( XYSSL_ERR_RSA_VERIFY_FAILED );
        }
    }

    return( XYSSL_ERR_RSA_INVALID_PADDING );
}

/*
 * Free the components of an RSA key
 */
void rsa_free( rsa_context *ctx )
{
    mpi_free( &ctx->RQ, &ctx->RP, &ctx->RN,
              &ctx->QP, &ctx->DQ, &ctx->DP,
              &ctx->Q,  &ctx->P,  &ctx->D,
              &ctx->E,  &ctx->N,  NULL );
}

#if defined(XYSSL_SELF_TEST) & (__FIPS_BUILD__)

#include "xyssl/sha1.h"

/*
 * Example RSA-2048 keypair, for test purposes
 * We use the same key parameters as used in openssl-Fips-2.0.2
 * RSA signature POST test.
 */
#define KEY_LEN 256
static const unsigned char rsa_test_2048_n[] = {
	"DB101AC2A3F1DCFF136BED44"
	"DFF0026D13C788DA706B54F1"
	"E827DCC30F996AFAC667FF1D"
	"1E3C1DC1B55F6CC0B2073A6D"
	"41E42599ACFCD20F02D3D154"
	"061A5177BDB6BFEAA75C06A9"
	"5D698445D7F505BA47F01BD7"
	"2B24ECCB9B1B108D81A0BEB1"
	"8C33E436B843EB192A818DDE"
	"810A9948B6F6BCCD49343A8F"
	"2694E328821A7C8F599F45E8"
	"5D1A4576045605A1D01B8C77"
	"6DAF53FA71E267E09AFE03A9"
	"85D2C9AABA2ABCF4A008F513"
	"98135DF0D933342A61C38955"
	"F0AE1A9C22EE19058D32FEEC"
	"9C84BAB7F96C3A4F07FC45EB"
	"12E57BFD55E62969D1C2E8B9"
	"7859F67910C64EEB6A5EB99A"
	"C7C45B63DAA33F5E927A815E"
	"D6B0E2628F7426C20CD39A17"
	"47E68EAB"
};
static const unsigned char rsa_test_2048_e[] = {
	"10001"
}; 

/* Known Answer Test (KAT) data for the above RSA private key signing
 *   kat_tbs.
 */
#define PT_LEN 37

unsigned char kat_tbs[] = "OpenSSL FIPS 140-2 Public Key RSA KAT";

unsigned char kat_RSA_SHA1[] = {
        0xD0,0x5E,0x82,0x33,0x05,0xD7,0x5C,0x23,0xEF,0x38,0x99,0x74,
        0xA3,0x51,0x08,0x95,0xD7,0xA3,0x1D,0x44,0xC7,0x58,0xAA,0xC5,
        0xD1,0x4B,0xE1,0xF9,0x86,0x41,0xE0,0x5B,0x5E,0x2B,0xBB,0xEB,
        0x47,0x2A,0xF1,0x2C,0xE4,0xF8,0x90,0x7D,0xA7,0x3A,0x06,0x8F,
        0x36,0x1D,0x32,0xAB,0xA9,0x0F,0xDF,0x75,0x47,0xE8,0xD6,0x6C,
        0xD5,0xA6,0x63,0xAD,0xE8,0xED,0x36,0x01,0xF8,0x6B,0x72,0x73,
        0xC1,0x3A,0xFE,0x9E,0x92,0xD9,0x42,0x60,0x96,0x41,0x2A,0x94,
        0xC5,0x89,0x99,0x85,0x8D,0x0B,0x7A,0xA5,0xE2,0x42,0x64,0x4E,
        0xCE,0x8D,0x0B,0x40,0xAD,0x48,0xB5,0xD2,0x77,0x92,0xA3,0xF9,
        0x4E,0x62,0xB1,0x1E,0x21,0x1D,0x1B,0xF3,0xBB,0x59,0x89,0x4F,
        0x2B,0xCF,0xD6,0xF8,0x03,0x7A,0x4D,0x1B,0x29,0x35,0xBE,0x3A,
        0xFA,0x7F,0x72,0x3A,0xF5,0xF7,0x68,0xB9,0xA7,0x0E,0x21,0x31,
        0x5A,0x4E,0xF5,0xF3,0x58,0xA4,0x24,0xCE,0xCC,0x69,0x72,0x0B,
        0xEA,0xF5,0x89,0x95,0x44,0xDC,0xB8,0x51,0x5A,0x0F,0x90,0x90,
        0x56,0xEE,0x3C,0xFD,0x3B,0xEE,0x2E,0x8E,0x0C,0x3D,0x54,0x99,
        0xB6,0x74,0xE9,0x5D,0xD7,0x65,0xE6,0x65,0xCC,0x7A,0xD3,0xBB,
        0x41,0xD5,0x57,0x5C,0x6E,0xC0,0xD9,0xEF,0xD7,0x17,0xE6,0xCC,
        0x4B,0x2F,0x9C,0x59,0xCA,0xCB,0xB1,0xBB,0x3B,0x67,0x36,0xB7,
        0x14,0xAE,0xA0,0x60,0xE9,0xB2,0x58,0x31,0xCD,0xA3,0x7B,0xAC,
        0xB8,0xAD,0xDF,0x7E,0x43,0xEA,0x40,0xAF,0xB1,0xCB,0xD2,0x7C,
        0xDF,0xCF,0x37,0x9C,0xD2,0x70,0xB0,0x4B,0x4B,0xB2,0x23,0x8D,
        0xFF,0xD6,0x11,0x68
};

/*
 * Checkup routine
 * We only perform POST for RSA signature verification
 * We do not perform POST for RSA encryption/decryption
 * or signature construction because Polarssl library is
 * used for those operations. xyssl is not used for such
 * operations. 
 */
int rsa_self_test( int verbose )
{
    rsa_context rsa;
    int ret = 0;
    unsigned char sha1sum[20];
    memset( &rsa, 0, sizeof( rsa_context ) );

    rsa.len = KEY_LEN;
    mpi_read_string( &rsa.N , 16, (char *)rsa_test_2048_n  );
    mpi_read_string( &rsa.E , 16, (char *)rsa_test_2048_e  );

    if( verbose != 0 )
        printf( "  RSA key validation: \n" );

    if( rsa_check_pubkey( &rsa ) != 0)
    {
	printf( "RSA POST failed to load the RSA key\n" );
        ret = 1;
        goto exit;
    }

    sha1( kat_tbs, PT_LEN, sha1sum );

    if( rsa_pkcs1_verify( &rsa, RSA_PUBLIC, RSA_SHA1, 20,
		sha1sum, kat_RSA_SHA1) != 0 )
    {
	printf( "RSA POST failed for RSA Verification operation\n" );
        ret = 1 ;
        goto exit;
    } 
    if( verbose != 0 )
        printf( "passed\n  PKCS#1 sig. verify: " );

    if( verbose != 0 )
        printf( "passed\n\n" );

exit : 
    return(ret);
}

int fips_post_process(int verbose)
{
    int status = 0;
    if(1 == rsa_self_test(verbose)) 
    {
	printf(" RSA POST failed \n");
	status = 1;
    }
    if(1 == sha1_self_test(verbose))
    {
	printf(" SHA POST failed \n");
	status =1;
    }
     return status;
}

#endif
#ifdef FIPS_TEST
int hex_to_ascii(const char *c2, unsigned char *byteval)
{
    unsigned char offset;

    *byteval = 0;
    if (*c2 >= '0' && *c2 <= '9')
    {
	offset = *c2 - '0';
	*byteval = offset;
    }
    else if (*c2 >= 'a' && *c2 <= 'f')
    {
	offset = *c2 - 'a';
	*byteval = offset + 10;
    }
    else if (*c2 >= 'A' && *c2 <= 'F')
    {
	offset = *c2 - 'A';
	*byteval = (offset + 10);
    }
    else
    {
	return -1;
    }
    return 0;
}

/*
 *  Perform the RSA Signature Verification Test.
 *
 * reqfn is the pathname of the REQUEST file.
 *
 * The output RESPONSE file is written to stdout.
 */
int rsa_sigver_test (char *fips_buf, int bufSize)
{
    int rv = 0;

    /*
     * buf holds one line from the input REQUEST file or to the output
     * RESPONSE file. s = 2x for HEX output + 1 for \n
     */
    int i, j;
    unsigned int shaLength = 0;	/* actual length of the digest */
    int modulus = 0;		/* the Modulus size */
    int nLength = 0;
    int sigLen = RSA_MAX_TEST_MODULUS_HEX;
    unsigned char *signature = NULL;
    int remaining_size = bufSize;
    char *  buf = fips_buf;
    sha1_context sha_ctx;
    int shaAlgo = 0;

    unsigned char MD[20];
    rsa_context rsa_ctx;
    int rsa_len = 0;
    rsa_context *rsa = &rsa_ctx;

    char tbuf[1024];

    memset(rsa, 0, sizeof(rsa_context));

    if (fips_buf == NULL)
    {
	printf("fips_buff is null \n");
	goto loser;
    }

    while (remaining_size > 0)
    {
	/* a comment or blank line */
	if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\r')
	{
	    goto next_line;
	}
	/* [mod = ...] */
	if (buf[0] == '[')
	{
	    if (strncmp (&buf[1], "mod ", 3) == 0)
	    {
		i = 4;
		while (isspace (buf[i]) || buf[i] == '=')
		{
		    i++;
		}
		modulus = atoi (&buf[i]);
		printf(" got modulus %d \n", modulus);
	    } else
		goto next_line;

	    if (modulus > RSA_MAX_TEST_MODULUS_BITS)
	    {
		printf ("ERROR: modulus greater than test maximum\r\n");
		goto loser;
	    }
	    nLength = modulus / 8;
	    // if modulus =1024, rsa.len = 128
	    rsa_len = rsa->len = nLength;

	    printf("[mod = %d]\n\n", modulus);

	    goto next_line;
	}
	/* n = ... modulus */
	if (buf[0] == 'n')
	{
	    char *nmod = NULL;

		memset(rsa, 0, sizeof(rsa_context));
		rsa->len = rsa_len;
		
	    i = 1;
	    // if rsa.len = 128 then in HEX modulus is 128*8/4 = 256
	    nmod = malloc((2*rsa->len)+1);
	    if(nmod == NULL) {
		printf(" nmod mem aloc failed \n");
		goto loser;
	    }
	    memset(nmod, '\0', ((2*rsa->len)+1)); 
	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    j = 0;
	    while((isxdigit(buf[i])) && j < (2*rsa->len)){
		nmod[j] = buf[i];
		++j; ++i;
	    }
	    nmod[j] = '\0';

	    mpi_read_string( &(rsa->N) , 16, nmod);

	    free(nmod);

	    memcpy(tbuf, buf, i);
	    tbuf[i] = '\0';
	    printf("%s\n\n", tbuf);

	    goto next_line;
	}
	/* SHAAlg = ... */
	if (strncmp (buf, "SHAAlg", 6) == 0)
	{
	    i = 6;
	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    /* set the SHA Algorithm */
	    if (strncmp (&buf[i], "SHA1", 4) == 0)
	    {
		shaLength = SHA1_RESULT_SIZE;
		shaAlgo = RSA_SHA1;

	    memcpy(tbuf, buf, i + 4); 
	    tbuf[i + 4] = '\0';
		printf("%s\n", tbuf);
	    }
	    else
	    {
		shaLength = 0;
		shaAlgo = 0;
		//    printf ("ERROR: Unable to find SHAAlg type");
	    }
	    goto next_line;
	}
	/* e = ... public Key */
	if (buf[0] == 'e')
	{
	    char *exponent = NULL;

	    if(shaAlgo != RSA_SHA1){
		goto next_line;
	    }

	    exponent = malloc(RSA_MAX_TEST_EXPONENT_BYTES+1);

	    if(exponent == NULL) {
		printf(" Exponent mem alloc failed \n");
		goto loser;
	    }
	    memset (exponent, '\0', (RSA_MAX_TEST_EXPONENT_BYTES+1));
	    i = 1;
	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    /* skip leading zero's */
	    while (buf[i] == '0')
	    {
		++i;
	    }
	    /* get the exponent */
	    j = 0;
	    while(isxdigit(buf[i]) && j < RSA_MAX_TEST_EXPONENT_BYTES){
		exponent[j] = buf[i];
		++j; ++i;
	    }

	    mpi_read_string( &(rsa->E) , 16, exponent );

	    free(exponent);

	    if( rsa_check_pubkey(rsa)  != 0 )
	    {
		printf( "failed rsa pub check \n");

		goto loser; 
	    }

	    memcpy(tbuf, buf, i); 
	    tbuf[i] = '\0';
		printf("%s\n", tbuf);

	    goto next_line;
	}
	/* Msg = ... */
	if (strncmp (buf, "Msg", 3) == 0)
	{
	    unsigned char *msg = NULL;	

	    if(shaAlgo != RSA_SHA1){
		goto next_line;
	    }

	    // If modulus is 1024, then we need 128 chars to store the message
	    msg = malloc(rsa->len +1);

	    if(msg == NULL) {
		printf(" msg mem alloc failed \n");
		goto loser;
	    }
	    memset (msg, 0, (rsa->len +1));
	    i = 3;
	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    for (j = 0; isxdigit (buf[i]) && j < (rsa->len); i += 2, j++)
	    {
		hex_to_byteval (&buf[i], &msg[j]);
	    }
	    memset (MD, 0, sizeof MD);
	    if (shaLength == SHA1_RESULT_SIZE)
	    {
		/* calculate the Message Digest */
		sha1_starts( &sha_ctx );
		sha1_update( &sha_ctx, msg, j);
		sha1_finish( &sha_ctx, MD);
#ifdef DEBUG_PRINT_RSA_VERIFY_MD
		printf("MD =");
		for(j = 0; j < 20; j++)
		{
		    printf( "%02x",MD[j] );
		}
		printf("\n");
#endif
	    }
	    else
	    {
		printf ("ERROR: SHAAlg not defined.");
	    }
	    free(msg);

	    memcpy(tbuf, buf, i); 
	    tbuf[i] = '\0';
		printf("%s\n", tbuf);

	    goto next_line;
	}
	/* S = ... */
	if (buf[0] == 'S')
	{
	    int status = 0;

	    signature = malloc (sigLen);
	    if (signature == NULL)
		goto loser;

	    if(shaAlgo != RSA_SHA1){
		goto next_line;
	    }

	    i = 1;
	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    for (j = 0; isxdigit (buf[i]) && j < sigLen; i +=2, j++)
	    {
		hex_to_byteval (&buf[i], &signature[j]);
	    }

	    memcpy(tbuf, buf, i); 
	    tbuf[i] = '\0';
		printf("%s\n", tbuf);

	    /*
	     * Verify the signature, it doesn't memory compare
	     * the output, we need to do that from our own
	     */
	    status =  rsa_pkcs1_verify( rsa, RSA_PUBLIC, RSA_SHA1, 20,
		    MD, signature);
	    if(status != 0)
	    {
		printf( "Result = F\n\n", status );

	    } else {
		printf( "Result = P\n\n" );
	    }
	    free (signature);
	    signature = NULL;
	    goto next_line;
	}
next_line:
        WATCHDOG_RESET();
        if (ctrlc()) {
            break;
        }

	i=0;
	while (buf[i] != '\n' && remaining_size > 0) {
	    --remaining_size;
	    ++i;
	}
	buf = buf+i;
	if(remaining_size > 0){
	    ++buf;--remaining_size;
	}
    }
loser:
    return rv;
}

#endif

#endif
