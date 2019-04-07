/*************************************************************************
Copyright (c) 2005 Cavium Networks (support@cavium.com). All rights
reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Cavium Networks' name may not be used to endorse or promote products
derived from this software without specific prior written permission.

This Software, including technical data, may be subject to U.S. export
control laws, including the U.S. Export Administration Act and its
associated regulations, and may be subject to export or import
regulations in other countries. You warrant that You will comply
strictly in all respects with all such regulations and acknowledge that
you have the responsibility to obtain licenses to export, re-export or
import the Software.

TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
AND WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR
WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY)
WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE
RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.

*************************************************************************/

/**
 * @file octeon_ipd_bp.S
 *
 * Workaround to enable IPD backpressure on chips with Errata SPI-400
 *
 * $Id: octeon_ipd_bp_enable.S,v 1.6 2006/11/28 18:24:04 rfranz Exp $ $Name: SDK_1_5_0_build_187 $
 *
 */


#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>

#define CVMCTL			$9,7
#define IPD_CTL_STATUS 		0x80014F0000000018
#define IPD_CLK_COUNT		0x80014F0000000338
#define BACKPRESSURE_ENABLE	0x8
#define CORRECT_MODULO		33

	//
	// count = octeon_ipd_bp_enable()
	//
	// This function turns on the global IPD backpressure enable. This
	// really just enables the possibilty of backpressure. Other CSRs
	// are needed to configure if and when it will come on. The should
	// only be run on CN38XX/CN36XX.
	//
	// Due to an IPD errata, the backpressure enable must be written on a
	// specific cycle. This function loops until we're on the correct cycle,
	// then does the actual write. It returns the number of times we had to
	// loop. This should never be more than 36. 37 is returned on failure.
	//
	.set push
	.set noreorder
	.globl octeon_ipd_bp_enable
	.text
	.ent octeon_ipd_bp_enable
octeon_ipd_bp_enable:
	sync			// Make sure all delayed transactions are done. We need to be idle
	li	a0, CORRECT_MODULO
	li	a1, BACKPRESSURE_ENABLE
	dmfc0	a2, CVMCTL
	ori	t0, a2, 1<<13	// Temorarily disable instruction prefetching
	dmtc0	t0, CVMCTL
	dli	a3, IPD_CTL_STATUS
	dli	t4, IPD_CLK_COUNT
	li	t2, 36		// We need the cycle counter modulo 36
	li	t3, 37		// This is the maximum number of loops
	li	v0, 0		// We'll return the number of times we execute the loop

	// The following loop must be written such that it takes a fixed
	// number of cycles to run. It also must take an odd number of cycles
	// so that we are guaranteed that we will eventually hit the correct
	// modulo 36 cycle.
	
	.set noat		// Lets not let the assembler change things
	.align 7		// These instructions must be in the same cache block
1:
	beq	v0, t3, 2f	// (1 cycle)	Bail if we've looped too many times
	 li	t1, 0		// (dual issue)	We will write zero if this isn't the correct cycle
	ld	t0, 0(t4)	// (70 cycles) 	Read the IPD cycle counter
	nop			// (dual issue)	Fill the second pipe
	ddivu	$0, t0, t2	// (72 cycles)	Divide the cycle counter by 36
	nop			// (dual issue)	Fill the second pipe
	mfhi	t0		// (5 cycles)	Get the remainder
	nop			// (dual issue)	Fill the second pipe
	dsub	t0, a0		// (1 cycle)	Subtract the currect value from the correct value
	nop			// (dual issue)	Fill the second pipe
	movz	t1, a1, t0	// (2 cycles)	Only write the real value if we're on the right cycle
	nop			// (dual issue)	Fill the second pipe
	nop; nop		// (1 cycle)	Delay to make loop odd number of cycles
	sd	t1, 0(a3)	// (1 cycle)	Store to IPD_CTL_STATUS
	nop			// (dual issue)	Fill the second pipe
	beqz	t1, 1b		// (2 cycles)	Try again if this wasn't the correct cycle. Takes 4 for first two iter
	 addi	v0, 1		// (dual issue) Count the number of time we do the loop
2:	
	sd	$0, 0(a3)	// Restore IPD_CTL_STATUS back to zero
	jr ra			// We're done, return to the caller
	 dmtc0	a2, CVMCTL	// Restore instruction prefetching to previous value
	.end octeon_ipd_bp_enable
	.set pop

