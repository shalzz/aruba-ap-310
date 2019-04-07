/******************************************************************************/
/*                                                                            */
/*  Copyright 2011  Broadcom Corporation                                      */
/*                                                                            */
/*     Unless you and Broadcom execute a separate written software license    */
/*     agreement governing  use of this software, this software is licensed   */
/*     to you under the terms of the GNU General Public License version 2     */
/*     (the GPL), available at                                                */
/*                                                                            */
/*          http://www.broadcom.com/licenses/GPLv2.php                        */
/*                                                                            */
/*     with the following added to such license:                              */
/*                                                                            */
/*     As a special exception, the copyright holders of this software give    */
/*     you permission to link this software with independent modules, and to  */
/*     copy and distribute the resulting executable under terms of your       */
/*     choice, provided that you also meet, for each linked independent       */
/*     module, the terms and conditions of the license of that module.        */
/*     An independent module is a module which is not derived from this       */
/*     software.  The special exception does not apply to any modifications   */
/*     of the software.                                                       */
/*     Notwithstanding the above, under no circumstances may you combine      */
/*     this software in any way with any other Broadcom software provided     */
/*     under a license other than the GPL, without Broadcom's express prior   */
/*     written consent.                                                       */
/*                                                                            */
/******************************************************************************/
/*
 * Generic Broadcom Utilities
 *
 * $Id: bcm_utils.h 241182 2011-02-17 21:50:03Z gmo $
 */

#ifndef _bcm_utils_h_
#define _bcm_utils_h_

#include <malloc.h>
#include <stdbool.h>
#include <config.h>
#include <common.h>

#define TRUE	true
#define FALSE	false

#define uint8	uint8_t
#define uint16	uint16_t
#define uint32	uint32_t

#define int8	int8_t
#define int16	int16_t
#define int32	int32_t

typedef unsigned int	uintptr;

/* general purpose memory allocation */
#define	MALLOC(osh, size)	malloc((size))
#define	MFREE(osh, addr, size)	free((addr))
#define	MALLOCED(osh)		(0)
#define	BZERO_SM(r, len)	memset((r), 0, (len))

/* host/bus architecture-specific address byte swap */
#define BUS_SWAP32(v)		(v)
#define ltoh16(i) (i)

#define	ROUNDUP(x, y)		((((x) + ((y) - 1)) / (y)) * (y))

#define osl_t void
#define si_t void

/*
 * Spin at most 'us' microseconds while 'exp' is true.
 * Caller should explicitly test 'exp' when this completes
 * and take appropriate error action if 'exp' is still true.
 */
#define SPINWAIT(exp, us) { \
	uint countdown = (us) + 9; \
	while ((exp) && (countdown >= 10)) {\
		udelay(10); \
		countdown -= 10; \
	} \
}



#endif	/* _bcm_utils_h_ */
