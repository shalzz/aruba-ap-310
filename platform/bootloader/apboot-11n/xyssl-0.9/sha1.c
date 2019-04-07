/*
 *  FIPS-180-1 compliant SHA-1 implementation
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
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#ifndef USE_HOSTCC
#include <common.h>
#include <config.h>
#include <watchdog.h>
#else
#include <memory.h>
#include <stdio.h>
#endif
#include <malloc.h>

#include "xyssl/config.h"

#if defined(XYSSL_SHA1_C)

#include "xyssl/sha1.h"
#ifdef FIPS_TEST
#define isspace(x) (((x) == ' ') || ((x) == '\t'))
int atoi(char *string);
int sha_test (char *fips_buf, int bufSize, unsigned int *MDlen,
	unsigned int *msgLen);
#endif
//#include <string.h>
//#include <stdio.h>

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
 * SHA-1 context setup
 */
void sha1_starts( sha1_context *ctx )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
}

static void sha1_process( sha1_context *ctx, unsigned char data[64] )
{
    unsigned long temp, W[16], A, B, C, D, E;

    GET_ULONG_BE( W[ 0], data,  0 );
    GET_ULONG_BE( W[ 1], data,  4 );
    GET_ULONG_BE( W[ 2], data,  8 );
    GET_ULONG_BE( W[ 3], data, 12 );
    GET_ULONG_BE( W[ 4], data, 16 );
    GET_ULONG_BE( W[ 5], data, 20 );
    GET_ULONG_BE( W[ 6], data, 24 );
    GET_ULONG_BE( W[ 7], data, 28 );
    GET_ULONG_BE( W[ 8], data, 32 );
    GET_ULONG_BE( W[ 9], data, 36 );
    GET_ULONG_BE( W[10], data, 40 );
    GET_ULONG_BE( W[11], data, 44 );
    GET_ULONG_BE( W[12], data, 48 );
    GET_ULONG_BE( W[13], data, 52 );
    GET_ULONG_BE( W[14], data, 56 );
    GET_ULONG_BE( W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                            \
(                                                       \
    temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^     \
           W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],      \
    ( W[t & 0x0F] = S(temp,1) )                         \
)

#define P(a,b,c,d,e,x)                                  \
{                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

    P( A, B, C, D, E, W[0]  );
    P( E, A, B, C, D, W[1]  );
    P( D, E, A, B, C, W[2]  );
    P( C, D, E, A, B, W[3]  );
    P( B, C, D, E, A, W[4]  );
    P( A, B, C, D, E, W[5]  );
    P( E, A, B, C, D, W[6]  );
    P( D, E, A, B, C, W[7]  );
    P( C, D, E, A, B, W[8]  );
    P( B, C, D, E, A, W[9]  );
    P( A, B, C, D, E, W[10] );
    P( E, A, B, C, D, W[11] );
    P( D, E, A, B, C, W[12] );
    P( C, D, E, A, B, W[13] );
    P( B, C, D, E, A, W[14] );
    P( A, B, C, D, E, W[15] );
    P( E, A, B, C, D, R(16) );
    P( D, E, A, B, C, R(17) );
    P( C, D, E, A, B, R(18) );
    P( B, C, D, E, A, R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    P( A, B, C, D, E, R(20) );
    P( E, A, B, C, D, R(21) );
    P( D, E, A, B, C, R(22) );
    P( C, D, E, A, B, R(23) );
    P( B, C, D, E, A, R(24) );
    P( A, B, C, D, E, R(25) );
    P( E, A, B, C, D, R(26) );
    P( D, E, A, B, C, R(27) );
    P( C, D, E, A, B, R(28) );
    P( B, C, D, E, A, R(29) );
    P( A, B, C, D, E, R(30) );
    P( E, A, B, C, D, R(31) );
    P( D, E, A, B, C, R(32) );
    P( C, D, E, A, B, R(33) );
    P( B, C, D, E, A, R(34) );
    P( A, B, C, D, E, R(35) );
    P( E, A, B, C, D, R(36) );
    P( D, E, A, B, C, R(37) );
    P( C, D, E, A, B, R(38) );
    P( B, C, D, E, A, R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    P( A, B, C, D, E, R(40) );
    P( E, A, B, C, D, R(41) );
    P( D, E, A, B, C, R(42) );
    P( C, D, E, A, B, R(43) );
    P( B, C, D, E, A, R(44) );
    P( A, B, C, D, E, R(45) );
    P( E, A, B, C, D, R(46) );
    P( D, E, A, B, C, R(47) );
    P( C, D, E, A, B, R(48) );
    P( B, C, D, E, A, R(49) );
    P( A, B, C, D, E, R(50) );
    P( E, A, B, C, D, R(51) );
    P( D, E, A, B, C, R(52) );
    P( C, D, E, A, B, R(53) );
    P( B, C, D, E, A, R(54) );
    P( A, B, C, D, E, R(55) );
    P( E, A, B, C, D, R(56) );
    P( D, E, A, B, C, R(57) );
    P( C, D, E, A, B, R(58) );
    P( B, C, D, E, A, R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    P( A, B, C, D, E, R(60) );
    P( E, A, B, C, D, R(61) );
    P( D, E, A, B, C, R(62) );
    P( C, D, E, A, B, R(63) );
    P( B, C, D, E, A, R(64) );
    P( A, B, C, D, E, R(65) );
    P( E, A, B, C, D, R(66) );
    P( D, E, A, B, C, R(67) );
    P( C, D, E, A, B, R(68) );
    P( B, C, D, E, A, R(69) );
    P( A, B, C, D, E, R(70) );
    P( E, A, B, C, D, R(71) );
    P( D, E, A, B, C, R(72) );
    P( C, D, E, A, B, R(73) );
    P( B, C, D, E, A, R(74) );
    P( A, B, C, D, E, R(75) );
    P( E, A, B, C, D, R(76) );
    P( D, E, A, B, C, R(77) );
    P( C, D, E, A, B, R(78) );
    P( B, C, D, E, A, R(79) );

#undef K
#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
}

/*
 * SHA-1 process buffer
 */
void sha1_update( sha1_context *ctx, unsigned char *input, int ilen )
{
    int fill;
    unsigned long left;

    if( ilen <= 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (unsigned long) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left),
                (void *) input, fill );
        sha1_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        sha1_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
    {
        memcpy( (void *) (ctx->buffer + left),
                (void *) input, ilen );
    }
}

static const unsigned char sha1_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-1 final digest
 */
void sha1_finish( sha1_context *ctx, unsigned char output[20] )
{
    unsigned long last, padn;
    unsigned long high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_ULONG_BE( high, msglen, 0 );
    PUT_ULONG_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha1_update( ctx, (unsigned char *) sha1_padding, padn );
    sha1_update( ctx, msglen, 8 );

    PUT_ULONG_BE( ctx->state[0], output,  0 );
    PUT_ULONG_BE( ctx->state[1], output,  4 );
    PUT_ULONG_BE( ctx->state[2], output,  8 );
    PUT_ULONG_BE( ctx->state[3], output, 12 );
    PUT_ULONG_BE( ctx->state[4], output, 16 );
}

/*
 * output = SHA-1( input buffer )
 */
void sha1( unsigned char *input, int ilen, unsigned char output[20] )
{
    sha1_context ctx;

    sha1_starts( &ctx );
    sha1_update( &ctx, input, ilen );
    sha1_finish( &ctx, output );

    memset( &ctx, 0, sizeof( sha1_context ) );
}

#if 0
/*
 * output = SHA-1( file contents )
 */
int sha1_file( char *path, unsigned char output[20] )
{
    FILE *f;
    size_t n;
    sha1_context ctx;
    unsigned char buf[1024];

    if( ( f = fopen( path, "rb" ) ) == NULL )
        return( 1 );

    sha1_starts( &ctx );

    while( ( n = fread( buf, 1, sizeof( buf ), f ) ) > 0 )
        sha1_update( &ctx, buf, (int) n );

    sha1_finish( &ctx, output );

    memset( &ctx, 0, sizeof( sha1_context ) );

    if( ferror( f ) != 0 )
    {
        fclose( f );
        return( 2 );
    }

    fclose( f );
    return( 0 );
}
#endif  // 0

/*
 * SHA-1 HMAC context setup
 */
void sha1_hmac_starts( sha1_context *ctx, unsigned char *key, int keylen )
{
    int i;
    unsigned char sum[20];

    if( keylen > 64 )
    {
        sha1( key, keylen, sum );
        keylen = 20;
        key = sum;
    }

    memset( ctx->ipad, 0x36, 64 );
    memset( ctx->opad, 0x5C, 64 );

    for( i = 0; i < keylen; i++ )
    {
        ctx->ipad[i] = (unsigned char)( ctx->ipad[i] ^ key[i] );
        ctx->opad[i] = (unsigned char)( ctx->opad[i] ^ key[i] );
    }

    sha1_starts( ctx );
    sha1_update( ctx, ctx->ipad, 64 );

    memset( sum, 0, sizeof( sum ) );
}

/*
 * SHA-1 HMAC process buffer
 */
void sha1_hmac_update( sha1_context *ctx, unsigned char *input, int ilen )
{
    sha1_update( ctx, input, ilen );
}

/*
 * SHA-1 HMAC final digest
 */
void sha1_hmac_finish( sha1_context *ctx, unsigned char output[20] )
{
    unsigned char tmpbuf[20];

    sha1_finish( ctx, tmpbuf );
    sha1_starts( ctx );
    sha1_update( ctx, ctx->opad, 64 );
    sha1_update( ctx, tmpbuf, 20 );
    sha1_finish( ctx, output );

    memset( tmpbuf, 0, sizeof( tmpbuf ) );
}

/*
 * output = HMAC-SHA-1( hmac key, input buffer )
 */
void sha1_hmac( unsigned char *key, int keylen,
                unsigned char *input, int ilen,
                unsigned char output[20] )
{
    sha1_context ctx;

    sha1_hmac_starts( &ctx, key, keylen );
    sha1_hmac_update( &ctx, input, ilen );
    sha1_hmac_finish( &ctx, output );

    memset( &ctx, 0, sizeof( sha1_context ) );
}

/*
 * Output = SHA-1( input buffer ). Trigger the watchdog every 'chunk_sz'
 * bytes of input processed.
 */
void sha1_csum_wd (unsigned char *input, int ilen, unsigned char output[20],
			unsigned int chunk_sz)
{
	sha1_context ctx;
#if defined(CONFIG_HW_WATCHDOG) || defined(CONFIG_WATCHDOG)
	unsigned char *end, *curr;
	int chunk;
#endif

	sha1_starts (&ctx);

#if defined(CONFIG_HW_WATCHDOG) || defined(CONFIG_WATCHDOG)
	curr = input;
	end = input + ilen;
	while (curr < end) {
		chunk = end - curr;
		if (chunk > chunk_sz)
			chunk = chunk_sz;
		sha1_update (&ctx, curr, chunk);
		curr += chunk;
		WATCHDOG_RESET ();
	}
#else
	sha1_update (&ctx, input, ilen);
#endif

	sha1_finish (&ctx, output);
}

#if defined(XYSSL_SELF_TEST)
/*
 * FIPS-180-1 test vectors
 */
static unsigned char sha1_test_buf[3][57] = 
{
    { "abc" },
    { "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" },
    { "" }
};

static const int sha1_test_buflen[3] =
{
    3, 56, 1000
};

static const unsigned char sha1_test_sum[3][20] =
{
    { 0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E,
      0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D },
    { 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE,
      0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1 },
    { 0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
      0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F }
};

/*
 * Checkup routine
 */
int sha1_self_test( int verbose )
{
    int i, j, buflen;
    unsigned char *buf = NULL;
    unsigned char sha1sum[20];
    sha1_context ctx;
    int ret = 0;

    /*
     * SHA-1
     */
    if(NULL == (buf = malloc(1024)))
    {
	printf(" SHA POST : malloc failure \n");
	ret = 1;
        goto exit;
    }
    memset(buf, '\x0', 1024);
    for( i = 0; i < 3; i++ )
    {
        if( verbose != 0 )
            printf( "  SHA-1 test #%d: ", i + 1 );

        sha1_starts( &ctx );

        if( i == 2 )
        {
            memset( buf, 'a', buflen = 1000 );

            for( j = 0; j < 1000; j++ )
                sha1_update( &ctx, buf, buflen );
        }
        else
            sha1_update( &ctx, sha1_test_buf[i],
                               sha1_test_buflen[i] );

        sha1_finish( &ctx, sha1sum );

        if( memcmp( sha1sum, sha1_test_sum[i], 20 ) != 0 )
        {
	    printf( "SHA1 POST failed\n" );
	    ret = 1;
	    goto exit;
        }

        if( verbose != 0 )
            printf( "passed\n" );
    }

    if( verbose != 0 )
        printf( "\n" );

exit: 
    if(NULL != buf)
	free(buf);
    return ret;
}
#endif

#ifdef FIPS_TEST
int hex_to_byteval (const char *c2, unsigned char *byteval)
{
    int i;
    unsigned char offset;

    *byteval = 0;
    for (i = 0; i < 2; i++)
    {
	if (c2[i] >= '0' && c2[i] <= '9')
	{
	    offset = c2[i] - '0';
	    *byteval |= offset << 4 * (1 - i);
	}
	else if (c2[i] >= 'a' && c2[i] <= 'f')
	{
	    offset = c2[i] - 'a';
	    *byteval |= (offset + 10) << 4 * (1 - i);
	}
	else if (c2[i] >= 'A' && c2[i] <= 'F')
	{
	    offset = c2[i] - 'A';
	    *byteval |= (offset + 10) << 4 * (1 - i);
	}
	else
	{
	    return -1;
	}
    }
    return 0;
}

int atoi(char *string)
{
    int res = 0;
    while (*string>='0' && *string <='9')
    {
	res *= 10;
	res += *string-'0';
	string++;
    }

    return res;
}
int sha1_self_test_mct(unsigned int MDlen, unsigned char *seed)
{
    int i, j, buflen = 20*3;
    int verbose = 1;
    unsigned char sha1sum[20];
    unsigned char buf[1024];
    sha1_context ctx;
    unsigned char MD_i3[25]; /* MD[i-3] */
    unsigned char MD_i2[25]; /* MD[i-2] */
    unsigned char MD_i1[25]; /* MD[i-1] */
    /*
     * SHA-1
     */

    for(i = 0; i < 100; i++)
    {
	memcpy (MD_i3, seed, MDlen);
	memcpy (MD_i2, seed, MDlen);
	memcpy (MD_i1, seed, MDlen);
	for(j = 3; j< 1003; j++)
	{
	    sha1_starts( &ctx );
	    memcpy (buf, MD_i3, MDlen);
	    memcpy (&buf[MDlen], MD_i2, MDlen);
	    memcpy (&buf[MDlen * 2], MD_i1, MDlen);
	    sha1_update( &ctx, buf, buflen );
	    sha1_finish( &ctx, sha1sum );
	    memcpy (MD_i3, MD_i2, MDlen);
	    memcpy (MD_i2, MD_i1, MDlen);
	    memcpy (MD_i1, sha1sum, MDlen);
	}

	memcpy (seed, sha1sum, 20);
	if( verbose != 0 )
	    printf( "\nCOUNT = %d\nMD = ",i );
	for(j = 0; j < 20; j++)
	{
	    if( verbose != 0 )
		printf( "%02x",sha1sum[j] );

	}

    printf("\n");
    }

    return( 0 );
}

/*
 *  Perform the SHA Tests.
 *
 * reqfn is the pathname of the input REQUEST file.
 *
 * The output RESPONSE file is written to stdout.
 */
int sha_test (char *fips_buf, int bufSize, unsigned int *MDlen, unsigned int *msgLen)
{
    int rv = 0;
    unsigned int i, j;
    unsigned char *msg = NULL;	/* holds the message to digest. */
    char *buf = NULL;		/* holds one line from the input
				 * REQUEST file. */
    unsigned char MD[21];	/* message digest */
    unsigned char seed[100];
    int remaining_size = bufSize;
    int verbose = 1;
    sha1_context ctx;

    if (fips_buf == NULL)
    {
	printf("fips_bull is null \n");
	goto loser;
    }
    /* zeroize the variables for the test with this data set */
    memset (seed, 0, sizeof(seed));

    buf = fips_buf;
    while(remaining_size > 0) {
	/* a comment or blank line */
	if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\r')
	{
	    goto next_line;
	}
	/* [L = Length of the Message Digest and sha_type */
	if (buf[0] == '[')
	{
	    if (strncmp (&buf[1], "L ", 1) == 0)
	    {
		i = 2;
		while (isspace (buf[i]) || buf[i] == '=')
		{
		    i++;
		}
		*MDlen = atoi (&buf[i]);
		printf(" got hash len %d \n", *MDlen);
		goto next_line;
	    }
	}
	/* Len = Length of the Input Message Length  ... */
	if (strncmp (buf, "Len", 3) == 0)
	{
	    i = 3;
	    while (isspace(buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    if (msg)
	    {
		free(msg);
		msg = NULL;
	    }
	    *msgLen = atoi (&buf[i]);	/* in bits */
	    printf("\nLen = %d\n", *msgLen);
	    if (*msgLen == 0)
	    {
		msg = malloc (1);
	    }
	    else
	    {
		if (*msgLen % 8 != 0)
		{
		    printf ("SHA tests are incorrectly configured for "
			    "BIT oriented implementations\r\n");
		    goto loser;
		}
		*msgLen = *msgLen / 8;	/* convert to bytes */
		msg =  malloc(*msgLen);
	    }
	    if (msg == NULL && *msgLen != 0)
	    {
		printf(" malloc failed \n");
		goto loser;
	    }
	    goto next_line;
	}
	/* MSG = ... */
	if (strncmp (buf, "Msg", 3) == 0)
	{
	    i = 3;
	    if(msg == NULL) {
		if (*msgLen == 0)
		{
		    msg = malloc (1);
		}
		else
		{
		    msg =  malloc(*msgLen);
		}
		if (msg == NULL && *msgLen != 0)
		{
		    printf(" malloc failed \n");
		    goto loser;
		}
	    }

	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    for (j = 0; j < *msgLen; i += 2, j++)
	    {
		hex_to_byteval (&buf[i], &msg[j]);
	    }
#ifdef PRINT_SHA_MSG
	    printf("Msg = ");
	    for(j = 0; j < *msgLen; j++)
	    {
		if( verbose != 0 )
		    printf( "%02x",msg[j] );

	    }
	    printf("\n");
#endif

	    /* calculate the Message Digest */
	    memset (MD, 0, sizeof MD);
	    sha1_starts( &ctx );
	    sha1_update( &ctx, msg, *msgLen );
	    sha1_finish( &ctx, MD);
	    printf("MD = ");
	    for(j = 0; j < 20; j++)
	    {
		if( verbose != 0 )
		    printf( "%02x",MD[j] );

	    }
	    printf("\n");
	}
	/* Seed = ... */
	if (strncmp (buf, "Seed", 4) == 0)
	{
	    i = 4;
	    while (isspace (buf[i]) || buf[i] == '=')
	    {
		i++;
	    }
	    for (j = 0; j < sizeof seed; i += 2, j++)
	    {
		hex_to_byteval (&buf[i], &seed[j]);
	    }
	    /* do the Monte Carlo test */
	    sha1_self_test_mct(*MDlen, seed);
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
    if(msg != NULL)
	free(msg);
    return rv;
}

#endif

#endif
