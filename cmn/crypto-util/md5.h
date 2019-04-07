/*
 * MD5 hash implementation and interface functions
 * Copyright (c) 2003-2005, Jouni Malinen <jkmaline@cc.hut.fi>
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

#ifndef MD5_H
#define MD5_H
#include <stdint.h>
#include <sys/types.h>

#define MD5_MAC_LEN 16

void hmac_md5_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		     const uint8_t *addr[], const size_t *len, uint8_t *mac);
void hmac_md5(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
	      uint8_t *mac);

typedef struct MD5Context {
	uint32_t buf[4];
	uint32_t bits[2];
	uint8_t in[64];
} MD5Context;


void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf,
	       unsigned len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);

#endif /* MD5_H */
