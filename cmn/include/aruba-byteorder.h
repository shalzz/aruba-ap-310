/*
 * Copyright (C) 2002-2013 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */

#ifndef __aruba_byteorder_h__
#define __aruba_byteorder_h__

#ifdef __KERNEL__

#include <asm/byteorder.h>
#ifdef __BIG_ENDIAN
#define ARUBA_IS_BIG_ENDIAN 1
#define ARUBA_IS_LITTLE_ENDIAN 0
#else
#define ARUBA_IS_BIG_ENDIAN 0
#define ARUBA_IS_LITTLE_ENDIAN 1
#endif /* BIG_ENDIAN? */

#else	/* __KERNEL__ */

#include <endian.h>
#if (__BYTE_ORDER == __BIG_ENDIAN)
#define ARUBA_IS_BIG_ENDIAN 1
#define ARUBA_IS_LITTLE_ENDIAN 0
#else
#define ARUBA_IS_BIG_ENDIAN 0
#define ARUBA_IS_LITTLE_ENDIAN 1
#endif /* BIG_ENDIAN? */

#endif	/* __KERNEL */

#endif /* __aruba_byteorder_h__ */
