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
    *  Broadcom Common Diagnostics Environment (CDE)
    *********************************************************************
    *  Filename: cde_main.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include "mde_common_headers.h"
#include "mde_local_headers.h"

#define BYTEMASK  0xFF
#define SHORTMASK 0xFFFF

unsigned mdeMemRead ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
  // mde has read 

  mdeScanf (mde, "%s%x", discard, &mde->bread.address);

  host_printf ("Location @%08x = ", mde->bread.address);

  mde->bread.address &= ALIGN_32BITS;
  mde->bread.size_ints = 1; // read 1 32-bit word
  // do the read 
  mde -> mem_read ( mde ); 
  // done with the read, the result is in mde->bread.ibuf [0]
  host_printf ("%08x\n", mde->bread.data [0]);

  return DIAG_RET_OK;
}

unsigned mdeMemWrite ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];

  mdeScanf (mde, "%s%x%x", discard, &mde->bwrit.address, &mde->bwrit.data [0]);
  mde->bwrit.address &= ALIGN_32BITS;
  mde->bwrit.size_ints = 1; // read 1 32-bit word
  host_printf ("Location @%08x <- ", &mde->bwrit.address);
  mde -> mem_writ ( mde );
  host_printf ("%08x\n", mde->bwrit.data [0]);

  return DIAG_RET_OK;
}

unsigned mdeShowMemory ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
  unsigned retval;

  mdeScanf (mde, "%s%x", discard, &mde->bread.address);

  mde->bread.address &= ALIGN_32BITS;
  mde->bread.size_ints = MEMBUF_SIZE_INTS; // read 1 32-bit word
  // read a block, show a block - stop when user hit 'q'
  while (1) {
	host_printf ("\n");
	retval = mde -> mem_read ( mde );
	mdeShowBlk (mde, retval);
	if ( retval != mde->bread.size_ints ) {
	  return DIAG_RET_OK;
	}

	host_printf ("\nEnter 'q' to exit or CR to see next page: ");
	if (host_console_read () == 'q') break;
	mde -> bread.address += MEMBUF_SIZE_BYTES;
  }
  host_printf ("\n");
  return DIAG_RET_OK;
}

unsigned mdeMemFill ( sMde_t *mde )
{
  // Usage: mf <address><count><data><incrementer>

  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_ints, data, incrementer;
  unsigned blocks;
  unsigned partial;
  unsigned retval;

  unsigned *pfill;
  unsigned fval;

  int binx; /// block index
  int finx; /// fill index
  
  mdeScanf (mde, "%s%x%x%x%x", discard, &address, &count_ints, &data, &incrementer);
  address &= ALIGN_32BITS;
  blocks  = count_ints / MEMBUF_SIZE_INTS;
  partial = count_ints % MEMBUF_SIZE_INTS;

  host_printf ("Filling memory at %08x with data %08x inc %08x (size %08x)\n",
				 address, data, incrementer, count_ints );

  host_printf ("blocks %d , partial size %d (bytes covered %d)\n",
				 blocks, partial, ((blocks * MEMBUF_SIZE_INTS)+partial)*4 );
  mde->bwrit.address = address & ALIGN_32BITS;
  mde->bwrit.size_ints = MEMBUF_SIZE_INTS; // write 256

  for ( binx = 0, fval = data; binx < blocks; binx ++ ) {
	// Fill the write buffer 
	pfill = & mde -> bwrit.data [0];
	for ( finx = 0; finx < MEMBUF_SIZE_INTS; finx ++, fval += incrementer ) {
	  *pfill++ = fval;
	}

	// Push out the write buffer
	if ( mde -> mem_writ ( mde ) != mde->bwrit.size_ints) {
	  host_printf ("No more write allowed - exit\n\n");
	  return DIAG_RET_BAD;
	}
	// Update the address for the next block
	mde -> bwrit.address += MEMBUF_SIZE_BYTES;
  }

  return DIAG_RET_OK;
}

unsigned mdeMemVerify ( sMde_t *mde )
{
#define ERROR_LIMIT 16

  // Usage: mv <address><count><data><incrementer>
  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_ints, data, incrementer;
  unsigned blocks;
  unsigned partial;
  unsigned retval;

  unsigned expected [MEMBUF_SIZE_INTS];
  unsigned blk_size = MEMBUF_SIZE_INTS;
  unsigned actual_read;

  unsigned *pfill;
  unsigned fval;
  unsigned error = 0;

  int binx; /// block index
  int finx; /// fill index
  
  mdeScanf (mde, "%s%x%x%x%x", discard, &address, &count_ints, &data, &incrementer);
  address &= ALIGN_32BITS;
  blocks  = count_ints / MEMBUF_SIZE_INTS;
  partial = count_ints % MEMBUF_SIZE_INTS;

  host_printf ("Verifying memory at %08x with data %08x inc %08x (size %08x)\n",
				 address, data, incrementer, count_ints );

  host_printf ("blocks %d , partial size %d (bytes covered %d)\n",
				 blocks, partial, ((blocks * MEMBUF_SIZE_INTS)+partial)*4 );

  ///// fill the local buffer with expected data
  ///// read 256 bytes and compare
  ///// iterate

  mde->bread.address = address & ALIGN_32BITS;
  mde->bread.size_ints = blk_size;
  
  for ( binx = 0, fval = data; binx < blocks; binx ++ ) {
	// setup the local expected data
	for ( finx = 0; finx < blk_size; finx ++, fval += incrementer ) {
	  expected [finx] = fval;
	}

	// read 256 bytes from target memory region
	actual_read =  mde -> mem_read ( mde );
	if ( actual_read != mde->bread.size_ints) {
	  blk_size = actual_read;
	}
	// compare the 256 bytes just read
	for ( finx = 0; finx < blk_size; finx ++ ) {
	  if ( expected [finx] == mde -> bread.data [finx]) continue;
	  host_printf (" ** Failed @address %08x: expected %08x read %08x XOR %08x\n",
					 (binx * MEMBUF_SIZE_BYTES)+address + finx *4, expected [finx], mde -> bread.data [finx],
					 (expected [finx] ^ mde -> bread.data [finx]));
	  if (error ++ > ERROR_LIMIT) {
		host_printf ("\nToo many errors (%d) abort now\n", error );
		return DIAG_RET_BAD;
	  }
	}
	// Update the address for the next block
	mde -> bread.address += MEMBUF_SIZE_BYTES;
  }

  return DIAG_RET_OK;
}

