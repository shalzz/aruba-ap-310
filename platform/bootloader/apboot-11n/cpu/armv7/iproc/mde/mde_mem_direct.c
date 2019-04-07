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
/*
  mdeMemRead - This is a command handler for the command 'memory-read'

  inputs:     mde is the data struct which has the raw input text entered by the user
              The syntax of memory-read :   mr <address>

			  <address>: The address to read

  effect:     The specified memory location is read, and the data displayed to the console
  return:     Always returns 0 for success

 */
unsigned mdeMemRead ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
  unsigned address;
  unsigned val;

  // mde has read 

  mdeScanf (mde, "%s%x", discard, &address);
  address &= ALIGN_32BITS;

  host_printf ("Location @%08x = ", address);

  // do the read 
  val = host_read_32bit_data(address);
  host_printf ("%08x\n", val);

  return DIAG_RET_OK;
}

/*
  mdeMemWrite - This is a command handler for the command 'memory-write'

  inputs:     mde is the data struct which has the raw input text entered by the user
              The syntax of memory-write :   mw <address><data>
			  <address> The address to write to
			  <data>    The data to write

  effect:     The specified memory location is written with the data
  return:     Always returns 0 for success

 */
unsigned mdeMemWrite ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
  unsigned address;
  unsigned data;

  mdeScanf (mde, "%s%x%x", discard, &address, &data );
  address &= ALIGN_32BITS;

  host_printf ("Location @%08x <- %08x\n", address, data);

  host_write_32bit_data(address, data);

  return DIAG_RET_OK;
}

/*
  mdeShowMemory - This is a command handler for the command 'memory-display'

  inputs:     mde is the data struct which has the raw input text entered by the user
              The syntax of memory-display :   md <address>
			  <address>   The address of the first block of data to read

  effect:     The specified memory location is read and displayed to the console block-by-block
              The user can enter CR for subsequent blocks until the key 'q' is hit

  return:     Always returns 0 for success

 */

unsigned mdeShowMemory ( sMde_t *mde )
{
  // make sure display size is reasonable

#define MD_MEMBUF_SIZE_BYTES 256
#define MD_MEMBUF_SIZE_INTS (MD_MEMBUF_SIZE_BYTES/4)

  char discard [DIAG_SIZE_SMALL];

  mdeScanf (mde, "%s%x", discard, &mde->bread.address);

  mde->bread.address    &= ALIGN_32BITS;
  mde->bread.size_ints = MD_MEMBUF_SIZE_INTS; // read 1 32-bit word


  host_printf ("\nShow memory from address %08x\n", mde->bread.address);

  while (1) {
	host_printf ("\n");
	host_block_read_32bit_data (mde->bread.address, MD_MEMBUF_SIZE_INTS, mde->bread.data );
	mdeShowBlk (mde, MD_MEMBUF_SIZE_INTS );

	host_printf ("\nEnter 'q' to exit or CR to see next page: ");
	if (host_console_read () == 'q') break;
	mde -> bread.address += MD_MEMBUF_SIZE_BYTES;
  }
  host_printf ("\n");
  return DIAG_RET_OK;
}

/*
  mdeMemFill - This is a command handler for the command 'memory-fill'

  inputs:     mde is the data struct which has the raw input text entered by the user
              The syntax of memory-fill :   mf <addr> <count> <data> <incrementer>
			  <addr>: The address of the memory block to fill
			  <count>: Number of 32-bit INTS to fill
			  <data>: The first data pattern to use
			  <incrementer>: The value to be added to subsequent data patterns to fill

  effect:     The specified memory block is written with incrementing data patterns

  return:     Always returns 0 for success

 */

unsigned mdeMemFill ( sMde_t *mde )
{
  // Usage: mf <address><count><data><incrementer>

  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_ints, data, incrementer;

  unsigned *pfill;
  unsigned fval;

  int finx; /// fill index
  
  mdeScanf (mde, "%s%x%x%x%x", discard, &address, &count_ints, &data, &incrementer);
  address &= ALIGN_32BITS;

  host_printf ("Direct fill memory at %08x with data %08x inc %08x (size %08x)\n",
				 address, data, incrementer, count_ints );

  for (finx = 0, pfill = (unsigned*)address, fval = data; finx < count_ints; finx ++, fval += incrementer ) {
	*pfill ++ = fval;
  }

  return DIAG_RET_OK;
}

/*
  mdeMemVerify - This is a command handler for the command 'memory-verify'

  inputs:     mde is the data struct which has the raw input text entered by the user
              The syntax of memory-verify :   mv <addr> <count> <data> <incrementer>
			  <addr>: The address of the memory block to verify
			  <count>: Number of 32-bit INTS to verify
			  <data>: The first data pattern to use
			  <incrementer>: The value to be added to subsequent data patterns

  effect:     The specified memory location is read and compared with expected patterns
              The comparison stops if too many data mismatch is found (limit = 18)
  return:     Always returns 0 for success

 */
unsigned mdeMemVerify ( sMde_t *mde )
{
#define ERROR_LIMIT 16

  // Usage: mv <address><count><data><incrementer>
  char discard [DIAG_SIZE_SMALL];
  unsigned address, count_ints, data, incrementer;
  unsigned val;
  unsigned expect;

  unsigned *pread;
  unsigned error = 0;
  int rinx;

  mdeScanf (mde, "%s%x%x%x%x", discard, &address, &count_ints, &data, &incrementer);
  address &= ALIGN_32BITS;

  host_printf ("Direct Verify memory at %08x with data %08x inc %08x (size %08x)\n",
				 address, data, incrementer, count_ints );

  for (rinx = 0, pread = (unsigned*)address, expect = data; 
	   rinx < count_ints; 
	   rinx ++, expect += incrementer, pread ++ ) {
	val = *pread;
	if (val == expect) continue;
	host_printf (" ** Failed @address %08x: expected %08x read %08x XOR %08x\n",
				 pread, expect, val, (expect ^ val));

	if (error ++ > ERROR_LIMIT) {
	  host_printf ("\nToo many errors (%d) abort now\n", error );
	  return DIAG_RET_BAD;
	}
  }
  return DIAG_RET_OK;
}

extern unsigned host_get_elapsed_msec(void);

//////
///// random test support
unsigned mdeRandomFill ( sMde_t *mde )
{
  // Usage: rf <address><count><seed>

  char discard [DIAG_SIZE_SMALL];
  unsigned address;
  unsigned count_ints;
  unsigned *pfill;
  unsigned seed;
  tinymt32_t tinymt;

  int binx; /// block index
  
  mdeScanf (mde, "%s%x%x%x", discard, &address, &count_ints, &seed);

  address &= ALIGN_32BITS;

  pfill = (unsigned*) address;

  if ( seed == 0 ) {
	seed = get_random_seed ();
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
  unsigned address;
  unsigned count_ints;

  unsigned actual_read;
  unsigned expected;
  unsigned *pread;
  unsigned seed;
  unsigned error = 0;
  int binx; /// block index
  tinymt32_t tinymt;
  
  mdeScanf (mde, "%s%x%x%x", discard, &address, &count_ints, &seed);
  address &= ALIGN_32BITS;
  pread = (unsigned*)address;

  if ( seed == 0 ) {
	seed = get_random_seed ();
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
	if ( actual_read == expected) continue;
	host_printf (" ** Failed @address %08x: expected %08x read %08x XOR %08x\n",
				 pread , expected , actual_read, (expected ^ actual_read));
	if (error ++ > ERROR_LIMIT) {
	  host_printf ("\nToo many errors (%d) abort now\n", error );
	  return DIAG_RET_BAD;
	}
  }
  return DIAG_RET_OK;
}
///////////


unsigned mdeRegRead ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
  unsigned which_phy = 0; /* XXX */
  unsigned offset;
  unsigned val;

  mdeScanf (mde, "%s%x", discard, &offset);
  val = host_reg_read ( offset);
  host_printf ("Read PHY#%d register at %08x = %08x\n", which_phy, offset, val );


  return DIAG_RET_OK;
}

unsigned mdeRegWrite ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
//  unsigned which_phy;
  unsigned offset;
  unsigned val;

  mdeScanf (mde, "%s%x%x", discard, &offset, &val);
  host_printf ("Write PHY#%d register at %08x with data %08x\n", offset, val);
  host_reg_write ( offset, val );

  return DIAG_RET_OK;
}
