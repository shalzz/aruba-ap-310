/*
 * Copyright (C) 2002-2014 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */

#ifndef MAC_UTIL_H
#define MAC_UTIL_H

#include <asm/types.h>

#ifdef NO_UNALIGNED
#define MAC_ADDRESS_COMPARE(MAC1, MAC2) \
        ((((__u8 *)(MAC1))[0] ^ ((__u8 *)(MAC2))[0]) | \
         (((__u8 *)(MAC1))[1] ^ ((__u8 *)(MAC2))[1]) | \
         (((__u8 *)(MAC1))[2] ^ ((__u8 *)(MAC2))[2]) | \
         (((__u8 *)(MAC1))[3] ^ ((__u8 *)(MAC2))[3]) | \
         (((__u8 *)(MAC1))[4] ^ ((__u8 *)(MAC2))[4]) | \
         (((__u8 *)(MAC1))[5] ^ ((__u8 *)(MAC2))[5]))

#define BASE_MAC_ADDRESS_COMPARE(MAC1, MAC2) \
        ((((__u8 *)(MAC1))[0] ^ ((__u8 *)(MAC2))[0]) | \
         (((__u8 *)(MAC1))[1] ^ ((__u8 *)(MAC2))[1]) | \
         (((__u8 *)(MAC1))[2] ^ ((__u8 *)(MAC2))[2]) | \
         (((__u8 *)(MAC1))[3] ^ ((__u8 *)(MAC2))[3]) | \
         (((__u8 *)(MAC1))[4] ^ ((__u8 *)(MAC2))[4]) | \
         ((((__u8 *)(MAC1))[5] ^ ((__u8 *)(MAC2))[5]) & 0xf8))

#define MAC_ADDRESS_COPY(MAC1, MAC2) \
        (((__u8 *)(MAC1))[0] = ((__u8 *)(MAC2))[0], \
         ((__u8 *)(MAC1))[1] = ((__u8 *)(MAC2))[1], \
         ((__u8 *)(MAC1))[2] = ((__u8 *)(MAC2))[2], \
         ((__u8 *)(MAC1))[3] = ((__u8 *)(MAC2))[3], \
         ((__u8 *)(MAC1))[4] = ((__u8 *)(MAC2))[4], \
         ((__u8 *)(MAC1))[5] = ((__u8 *)(MAC2))[5])

#else // NO_UNALIGNED
#define MAC_ADDRESS_COMPARE(S, D) \
	((((__u16 *)(long)(S))[0] ^ ((__u16 *)(long)(D))[0]) | \
	 (((__u16 *)(long)(S))[1] ^ ((__u16 *)(long)(D))[1]) | \
	 (((__u16 *)(long)(S))[2] ^ ((__u16 *)(long)(D))[2]))

#define BASE_MAC_ADDRESS_COMPARE(MAC1, MAC2) \
            (((*(__u32 *)(long)MAC1) ^ (*(__u32 *)(long)MAC2)) | \
              (MAC1[4] ^ MAC2[4]) | ((MAC1[5] & 0xf8) ^ (MAC2[5] & 0xf8)))

#define MAC_ADDRESS_COPY(MAC1, MAC2)  \
	(((__u16 *)(long)(MAC1))[0] = ((__u16 *)(long)(MAC2))[0], \
	 ((__u16 *)(long)(MAC1))[1] = ((__u16 *)(long)(MAC2))[1], \
	 ((__u16 *)(long)(MAC1))[2] = ((__u16 *)(long)(MAC2))[2])

#endif // NO_UNALIGNED

/* 
 * A MAC is a locally administered MAC if the second least significant 
 * bit of the first octet is set to 1. Therefore, the following MACs are 
 * locally administered:
 * x2-xx-xx-xx-xx-xx
 * x6-xx-xx-xx-xx-xx
 * xA-xx-xx-xx-xx-xx
 * xE-xx-xx-xx-xx-xx
 * Macro checks for the second least significant bit being set to 1
 */
#define IS_LOCAL_ADMIN_MAC(MAC)  (MAC ? (((((__u8 *)(MAC))[0]) & 0x02) > 0) : 0)

#endif // MAC_UTIL_H
