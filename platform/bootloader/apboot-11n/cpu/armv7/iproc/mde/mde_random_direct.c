/*  *********************************************************************
    *  Copyright 2012
    *  Broadcom Corporation. All rights reserved.
    *
    *  This software is furnished under license and may be used and
    *  copied only in accordance with the following terms and
    *  conditions.  Subject to these conditions, you may download,
    *  copy, install, use, modify and distribute modified or unmodified
    *  copies of this software in source and/or binary form.  No title
    *  or ownership is transferred hereby.
    *
    *  1) Any source code used, modified or distributed must reproduce
    *     and retain this copyright notice and list of conditions as
    *     they appear in the source file.
    *
    *  2) No right is granted to use any trade name, trademark, or
    *     logo of Broadcom Corporation. Neither the "Broadcom
    *     Corporation" name nor any trademark or logo of Broadcom
    *     Corporation may be used to endorse or promote products
    *     derived from this software without the prior written
    *     permission of Broadcom Corporation.
    *
    *  3) THIS SOFTWARE IS PROVIDED "AS-IS" AND ANY EXPRESS OR
    *     IMPLIED WARRANTIES, INCLUDING BUT NOT LIMITED TO, ANY IMPLIED
    *     WARRANTIES OF MRCHANTABILITY, FITNESS FOR A PARTICULAR
    *     PURPOSE, OR NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT
    *     SHALL BROADCOM BE LIABLE FOR ANY DAMAGES WHATSOEVER, AND IN
    *     PARTICULAR, BROADCOM SHALL NOT BE LIABLE FOR DIRECT, INDIRECT,
    *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    *     GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    *     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
    *     OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    *     TORT (INCLUDING NEGLIGENCE OR OTHERWISE), EVEN IF ADVISED OF
    *     THE POSSIBILITY OF SUCH DAMAGE.
    *********************************************************************
    *  Broadcom Memory Diagnostics Environment (MDE)
    *********************************************************************
    *  Filename: mde_random.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

//// Include non-broadcom headers

#include <tinymt32.h>

#include <mde_common_headers.h>

#include <tinymt32.c>


///// random test support
unsigned mdeRandomFill ( sMde_t *mde )
{
  // Usage: rf <address><count><seed>

  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_bytes;
  unsigned count_ints;
  unsigned *pfill;
  unsigned seed;
  tinymt32_t tinymt;

  int binx; /// block index
  
  mdeScanf (mde, "%s%x%x%x", discard, &address, &count_bytes, &seed);

  address &= ALIGN_32BITS;
  count_ints = count_bytes / 4;

  pfill = (unsigned*) address;

  if ( seed == 0 ) {
	seed = mde -> get_local_random_seed ( mde );
  } 

  host_printf ("Filling memory at %08x with random data (size %08x INTS) seed %08x\n",
				 pfill, count_ints, seed);

  // initialize tiny
  tinymt.mat1 = 0;
  tinymt.mat2 = 0;
  tinymt.tmat = 0;

  tinymt32_init(&tinymt, seed);


  for ( binx = 0; binx < count_ints; binx ++ ) {
	*pfill++ = tinymt32_generate_uint32(&tinymt);
  }

  host_printf ("Done fill with random data (seed %08x)\n", seed);

  return DIAG_RET_OK;
}

unsigned mdeRandomVerify ( sMde_t *mde )
{
#define ERROR_LIMIT 16

  // Usage: mv <address><count><data><incrementer>
  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_bytes;
  unsigned count_ints;
  unsigned retval;
  unsigned actual_read;
  unsigned expected;
  unsigned *pread;
  unsigned seed;
  unsigned error = 0;
  int binx; /// block index
  tinymt32_t tinymt;
  
  mdeScanf (mde, "%s%x%x%x", discard, &address, &count_bytes, &seed);
  address &= ALIGN_32BITS;
  pread = (unsigned*)address;

  count_ints = count_bytes/4;
  if ( seed == 0 ) {
	seed = mde -> get_local_random_seed ( mde );
  } 

  host_printf ("Verifying memory at %08x with random data (size %08x INTS) seed %08x\n",
				 pread, count_ints, seed );

  // initialize tiny
  tinymt.mat1 = 0;
  tinymt.mat2 = 0;
  tinymt.tmat = 0;

  tinymt32_init(&tinymt, seed);

  for ( binx = 0; binx < count_ints; binx ++, pread ++ ) {
	expected = tinymt32_generate_uint32(&tinymt);
	actual_read = *pread;
	if ( actual_read == expected) contine;
	host_printf (" ** Failed @address %08x: expected %08x read %08x XOR %08x\n",
				 pread , expected , actual_read, (expected ^ actual_read));
	if (error ++ > ERROR_LIMIT) {
	  host_printf ("\nToo many errors (%d) abort now\n", error );
	  return DIAG_RET_BAD;
	}
  }
  return DIAG_RET_OK;
}

