/*
 * SHA256 hash implementation and interface functions
 * Copyright (c) 2003-2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef SHA256_H
#define SHA256_H
#include <stdarg.h>

#define SHA256_MAC_LEN 32

struct sha256_state {
	u64 length;
	u32 state[8], curlen;
	u8 buf[64];
};

void hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
		      const u8 *addr[], const size_t *len, u8 *mac);
void hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
		 size_t data_len, u8 *mac);
void sha256_prf( const unsigned char *key, int key_len, unsigned char *prefix, int prefix_len, unsigned char *data, int data_len,
                 unsigned char *output, int buf_len ) ;
void sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
                   u8 *mac);
void sha256_vectorized(u8 *mac, int nparams, ...);

void sha256_init(struct sha256_state *md);
int sha256_process(struct sha256_state *md, const unsigned char *in,
		  unsigned long inlen);
int sha256_done(struct sha256_state *md, unsigned char *out);

#define SHA256Init sha256_init
#define SHA256Update sha256_process
#define SHA256Final( CTX, DIGEST ) sha256_done( DIGEST, CTX )

typedef struct sha256_state SHA256Context ;

#endif /* SHA256_H */
