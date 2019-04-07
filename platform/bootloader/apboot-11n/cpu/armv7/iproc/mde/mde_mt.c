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
    *  Filename: mde_mt.c
    *
    *  Function: Memory test package
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include <mde_common_headers.h>


/*
  Memory tester is created by local client
  One memory tester is created for each memory region associated with a PHY

  start;
  bsize;

  // There may or may not be such client

  ddr_interface_stress_client_init
  ddr_interface_stress_client_start
  ddr_interface_stress_client_status
  ddr_interface_stress_client_stop

 */


/*
  mt_set_config
  mt_show_config
  mt_auto_run
 */

unsigned mtEditConfig ( sMde_t *mde )
{
  // usage mtec <which memtester>
  unsigned which;
  char discard [DIAG_SIZE_SMALL];
  MEM_TESTER *this;
 
  mdeScanf (mde, "%s%x", discard, &which);
  if (which < LOCAL_MEM_TESTER_COUNT) {
	this = &mde->memtester [which];
	host_printf ("Editing memory test configuration #%d - %s\nEnter new value or CR to skip\n\n",
				 this -> id, this -> name);

	// edit all items
	
	//	unsigned mdeQuery ( sMde_t *mde, char *question, unsigned original)
	mde->memtester [which].start_addr_cached = mdeQuery ( mde, "start_addr_cached", mde->memtester [which].start_addr_cached );
	mde->memtester [which].start_addr_uncached  = mdeQuery ( mde, "start_addr_uncached",  mde->memtester [which].start_addr_uncached );
	mde->memtester [which].test_bsize    = mdeQuery ( mde, "test_bsize",  mde->memtester [which].test_bsize );
	mde->memtester [which].test_wsize    = mde->memtester [which].test_bsize / 4;
	mde->memtester [which].enabled       = mdeQuery ( mde, "enable flag",  mde->memtester [which].enabled );

	if ( mde->memtester [which].stressor_present) {
	  mde->memtester [which].stressor_address = mdeQuery ( mde, "stressor_address", mde->memtester [which].stressor_address );
	  mde->memtester [which].stressor_bsize   = mdeQuery ( mde, "stressor_bsize",   mde->memtester [which].stressor_bsize );
	}

	//	mde->memtester [which].  = mdeQuery ( mde, "",  mde->memtester [which]. );
	return DIAG_RET_OK;
  } 
  
  host_printf ("No such memory tester #%d (%d installed. Enter id from 0, 1...)\n", which, LOCAL_MEM_TESTER_COUNT);
  return DIAG_RET_BAD;
}

unsigned mtShowConfig ( sMde_t *mde )
{
  int inx;
  int count = mde->memtester_count;

  host_printf ("Number of memory test configuration found = %d\n", count);
  for (inx = 0; inx < count; inx ++) {
	host_printf ("\n    Configuration #%d - %s\t", mde->memtester[inx].id, mde->memtester[inx].name);

	if (mde->memtester[inx].enabled) host_printf ("\n");
	else host_printf ("**** DISALBED (will not run test)\n");

	host_printf ("\tStart address (cached)\t\t%08x\n",   mde->memtester[inx].start_addr_cached);
	host_printf ("\tStart address (uncached)\t%08x\n", mde->memtester[inx].start_addr_uncached);
	host_printf ("\tNumber of bytes to test\t\t%08x\n",  mde->memtester[inx].test_bsize);
	if (mde->memtester[inx].stressor_present) {
	  host_printf ("\n\tMemory interface stressor runtime parameters:\n");
	  host_printf ("\t  Address for memory test\t%08x\n", mde->memtester[inx].stressor_address);
	  host_printf ("\t  Number of bytes to test\t%08x\n", mde->memtester[inx].stressor_bsize);
	} else {
	  host_printf ("\n\tNo memory interface stressor installed\n");
	}
  }
  return DIAG_RET_OK;
}

unsigned mtAutoTest ( sMde_t *mde )
{
  return DIAG_RET_OK;
}

extern unsigned host_get_elapsed_msec(void);
extern void host_status_report(void);

#define OVERFLOW_IF_MULTIPLIED_BY_1000 (0xFFFFFFFF / 1000 )
unsigned mtCheckAndShowTestStatus ( sMde_t *mde )
{

  unsigned stressor_status;
  unsigned bandwidthMB;
  unsigned sec, min, hr, rd;
  unsigned elapse_time_sec;
  unsigned elapse_time_msec;

  int inx;

  // Calculate MB
	
  mde->mt_total_mb_tested += mde->mt_total_mb_per_loop;

  // check time now
  mde->time_now.msec = host_get_elapsed_msec ();
  mde->time_now.sec = mde->time_now.msec / 1000;
  // we recorded the start time in mde->time_start

  elapse_time_msec = mde->time_now.msec - mde->time_start.msec;
  elapse_time_sec  = elapse_time_msec / 1000;
  
  hr = elapse_time_sec / 3600;
  rd = elapse_time_sec % 3600; // rd is the seconds within the partial hour

  sec = rd % 60;
  min = rd / 60;
  // Note: time_now.sec is offset by start time, so it is elapsed time

  if (mde->mt_total_mb_tested  <= OVERFLOW_IF_MULTIPLIED_BY_1000) {
	bandwidthMB = mde->mt_total_mb_tested * 1000 / mde->mt_total_read_time_ms; //// elapse_time_msec;
  } else {
	bandwidthMB = mde->mt_total_mb_tested / elapse_time_sec;
  }

  // check stressor status
  for (inx = 0; inx < LOCAL_MEM_TESTER_COUNT; inx ++ ) {
	if ( mde->memtester [inx].stressor_present ) {
	  stressor_status = mde->memtester [inx].stressor_status ( mde );
	  host_printf ("\tstressor %d status %08x\n", inx, stressor_status);
	}
  }

  /// show test error status

#if LOCAL_MEM_TESTER_COUNT > 1
  if (( mde->memtester [0].uTotalErrMap ) || ( mde->memtester [1].uTotalErrMap )) {
	host_printf ("MEMC0 error status %08x count %d\tMEMC1 error status %08x count %d\n", 
				 mde->memtester [0].uTotalErrMap, mde->memtester [0].uTotalErr,
				 mde->memtester [1].uTotalErrMap, mde->memtester [1].uTotalErr );
  } else {
	host_printf ("No MIPS error found\n");
  }
#else // single MEMC only (cannot handle more than 2
  if ( mde->memtester [0].uTotalErrMap ) {
	host_printf ("MEMC0 error status %08x count %d\n", 
				 mde->memtester [0].uTotalErrMap, mde->memtester [0].uTotalErr );
  } else {
	host_printf ("No MIPS error found\n");
  }
#endif
  /// each chip can specify how the per loop report should look like
  host_status_report ( /*mde*/ );
  host_printf ("Up time %02d:%02d:%02d Total read time %d ms Total read %d MB (%dMB/s)\n",
			   hr, min, sec,  mde->mt_total_read_time_ms, mde->mt_total_mb_tested, bandwidthMB );
  host_printf ("===========================================================\n");

  return DIAG_RET_OK;
}


////
#if HOST_SUPPORTS_BUFFERED_MEM_RW
#include <mde_mt_buffered_rw.c>
#endif


#if HOST_SUPPORTS_DIRECT_MEM_RW
#include <mde_mt_direct_rw.c>
#endif

unsigned mtMode0Handler ( sMde_t *mde )
{
  //  unsigned uLoopCnt = mde -> memtetser_loop
  // <mode> 0 - write once, then each loop do read;

  int this_mt;
  int cache_to_uncache_address_offset;
  int ii;


  if ( mde -> memtester_loop == 0) {
	// write once, for all memtesters
	for (this_mt = 0; this_mt < mde->memtester_count; this_mt ++ ) {
	  mde -> memtester[this_mt].uAddrFailCnt = TmtMemWriteAndVerify (mde->memtester[this_mt].start_addr_uncached,
																	 mde->memtester[this_mt].test_wsize,
																	 &mde->memtester[this_mt].tiny,
																	 mde->random_seed,
																	 mde->memtester[this_mt].uAddxFail,
																	 ADDR_FAIL_CNT,
																	 ADDR_FAIL_CNT,
																	 0, mde);

	  if ( mde -> memtester[this_mt].uAddrFailCnt > ADDR_FAIL_CNT)
		mde -> memtester[this_mt].uAddrFailCnt = ADDR_FAIL_CNT;

      // translate uncached to cached address
	  cache_to_uncache_address_offset = mde->memtester[this_mt].start_addr_cached - mde->memtester[this_mt].start_addr_uncached;

      for (ii = 0; ii < mde -> memtester[this_mt].uAddrFailCnt; ii++)
      {
		mde->memtester[this_mt].uAddxFail [ii] = mde->memtester[this_mt].uAddxFail[ii] + cache_to_uncache_address_offset; 
      }
	}
  }
  /// mode 0 done with preparation - now do the read test

  mtMode013ReadTest ( mde );

  return DIAG_RET_OK;
}

unsigned mtMode1Handler ( sMde_t *mde )
{
  //  unsigned uLoopCnt = mde -> memtetser_loop
  // <mode> 1 - Each loop do write then read;

  int this_mt;
  int cache_to_uncache_address_offset;
  int ii;


  // write for all memtesters
  for (this_mt = 0; this_mt < mde->memtester_count; this_mt ++ ) {
	mde -> memtester[this_mt].uAddrFailCnt = TmtMemWriteAndVerify (mde->memtester[this_mt].start_addr_uncached,
																   mde->memtester[this_mt].test_wsize,
																   &mde->memtester[this_mt].tiny,
																   mde->random_seed,
																   mde->memtester[this_mt].uAddxFail,
																   ADDR_FAIL_CNT,
																   ADDR_FAIL_CNT,
																   0, mde);
	
	if ( mde -> memtester[this_mt].uAddrFailCnt > ADDR_FAIL_CNT)
	  mde -> memtester[this_mt].uAddrFailCnt = ADDR_FAIL_CNT;
	
	// translate uncached to cached address
	cache_to_uncache_address_offset = mde->memtester[this_mt].start_addr_cached - mde->memtester[this_mt].start_addr_uncached;
	
	for (ii = 0; ii < mde -> memtester[this_mt].uAddrFailCnt; ii++)
      {
		mde->memtester[this_mt].uAddxFail [ii] = mde->memtester[this_mt].uAddxFail[ii] + cache_to_uncache_address_offset; 
      }
  }
  
  /// mode 1 done with write - now do the read test

  mtMode013ReadTest ( mde );

  return DIAG_RET_OK;
}

#define SPINNER_LIMIT 0x10000000

unsigned mtMode2Handler ( sMde_t *mde )
{
#if 0 //// for NS - 
#define BITFLIP_LOOP_LIM        20
#define DEFAULT_ADDRCHUNK_SIZE  64
#define CHUNK_END_POS           27
  //---------------------------------------------------------------
  // IMPORTANT NOTE:
  //
  //   Filename: amt_mode_2_bitflip_dual_memc.c
  // If you make any change in this file, please also update
  // amt_mode_2_bitflip_one_memc.c
  //
  //---------------------------------------------------------------
  uint32_t start1, start2, size;
  int32_t uBitflipLoopCnt = BITFLIP_LOOP_LIM;
  uint32_t *p1, *p2, data1, data2, ii, jj, kk, mode;
  uint32_t seed, expected1, expected2, pass, diff;
  uint32_t aborted = 0;
  uint32_t uSizePerLoopMB;
  uint32_t uLoopCnt;
  uint32_t verbose = 0;
  uint32_t nn, startpos = 0;
  uint32_t chunkInWords = DEFAULT_ADDRCHUNK_SIZE;

  uint32_t chunks = size / chunkInWords;
  uint32_t exp;
  uint32_t addm1, addm2;
  volatile uint32_t *m1;
  volatile uint32_t *m2;
  
  // starting pos for chunk
  addm1 = chunkInWords;
  while (addm1 >>= 1)	startpos++;
  startpos += 2;
  
  host_printf("bitflip test. S0=%08X, S1=%08X, L=%dMB Loop Count=%d chunks=%d of size %dLW startpos %d\n",
              start1, start2, (size * BYTES_PER_WORD / ONE_MB) * 2,
              uBitflipLoopCnt, chunks, chunkInWords, startpos);
  //
  //  check_read_fifo_status (MEMC0, FALSE);
  //  check_read_fifo_status (MEMC1, FALSE);
  
  uSizePerLoopMB = DIAG_SIZE_32 * uBitflipLoopCnt * (size * BYTES_PER_WORD / ONE_MB) * 2;
  
  uPeriodStartInSec = get_system_time ();
  uPeriodStartInMilliSec = get_system_ms_time();
  // loop through all bits
  for (jj = 0; jj < DIAG_SIZE_32; jj++)	{
	// remove the check if user hit q
	// Prepare a pattern: all bits a reset and only one is set
	//
	expected1 = BIT_31 >> jj;
	host_printf("testing bit %02d ", 31-jj);
    
	// Repeat test 
	//
	for (kk = 0; kk < uBitflipLoopCnt; kk++)	{
	  // Inverse pattern, now it becomes all bits are set except one, that is reset
	  //
	  expected1 = ~expected1;
	  
	  // ** WRITE **
	  // Initialize pointers to the beginning of the buffers (for filling of the buffers)
	  //
	  p1 = (uint32_t *) start1;
	  p2 = (uint32_t *) start2;
      
	  for (ii = 0; ii < chunks; ii++) {
		addm1 = mirror(((uint32_t)p1), CHUNK_END_POS, startpos);
		addm2 = mirror(((uint32_t)p2), CHUNK_END_POS, startpos);
		m1 = (volatile uint32_t*)(addm1);
		m2 = (volatile uint32_t*)(addm2);
		for (nn = 0; nn < chunkInWords; nn++)  {
		  // If iteration is even, write (111101111) pattern to both locations
		  //                  odd, write (000010000) pattern to both locations
		  //                  *(buf1++)  = (l % 2) == 0 ? pattern : ~pattern;
		  exp = (nn % 2) == 0 ? expected1 : ~expected1;
		  *m1++ = exp;
		  *m2++ = exp;
		}
		p1 += chunkInWords;
		p2 += chunkInWords;
	  }
      
	  // ** READ + CHECK **
	  // Initialize pointers to the beginning of the buffers (for the check)
	  //
	  p1 = (uint32_t*)start1;
	  p2 = (uint32_t*)start2;
	  for (ii = 0; ii < chunks; ii++) {
		addm1 = mirror(((uint32_t)p1), CHUNK_END_POS, startpos);
		addm2 = mirror(((uint32_t)p2), CHUNK_END_POS, startpos);
		m1 = (volatile uint32_t*)(addm1);
		m2 = (volatile uint32_t*)(addm2);
        
		for (nn = 0; nn < chunkInWords; nn++, m1++, m2++)  {
		  exp = (nn % 2) == 0 ? expected1 : ~expected1;
          
		  data1 = *m1;
		  data2 = *m2;
          
		  // Compare data in 2 buffers, they should be the same.
		  //
		  if (data1 != exp) {
			uTotalErr1 ++;
			// accumulate error statistics
			diff = data1 ^ exp;
			// get error status based on the failing dq (instead of XOR of expected and actual data
			uTotalErrMap1 |= fdq_binary ( p1 , diff );
			//    find_all_errors (p, data, expected, FIND_DQ_DEBUG);
			//    find_all_errors (p, data, expected, 0);
			uAddrFailCnt1 += FIND_ALL_ERRORS_ALL (g_err_per_dq_cyc3, m1, data1, exp, 0);
			// print this error first
			if (verbose) host_printf ("  **Failed: @%08x Expected %08x read %08x XOR %08x\n",
									 m1, exp, data1, diff);                       
		  } // if (data1
               
		  if (data2 != exp) {
			uTotalErr2 ++;
			// accumulate error statistics
			diff = data2 ^ exp;
			// get error status based on the failing dq (instead of XOR of expected and actual data
			uTotalErrMap2 |= fdq_binary (p2, diff);
			//    find_all_errors (p, data, expected, FIND_DQ_DEBUG);
			//    find_all_errors (p, data, expected, 0);
			uAddrFailCnt2 += FIND_ALL_ERRORS_ALL (g_err_per_dq_cyc4, m1, data2, exp, 0);
			// print this error first
			if (verbose) host_printf ("  **Failed: @%08x Expected %08x read %08x XOR %08x\n",
									 m2, exp, data2, diff);                       
		  } // if (data2                 
		} // for ( nn
	  } // for ( ii
	  
	  p1 += chunkInWords;
	  p2 += chunkInWords;
	  host_printf(".");
	} // for ( kk
	host_printf(" errs=%0d,%0d %dC %dmV (%d,%d bits mismatch)\n", 
			   uTotalErr1, uTotalErr2, get_PVT_temp(), get_PVT_voltage(),
			   uAddrFailCnt1, uAddrFailCnt2);        
  }
#endif
  return DIAG_RET_OK;
}

unsigned mtMode3Handler ( sMde_t *mde )
{

  // mode 3 is the same as mode 1 using cached address

  int this_mt;
//  int cache_to_uncache_address_offset;
//  int ii;


  // write for all memtesters
  for (this_mt = 0; this_mt < mde->memtester_count; this_mt ++ ) {
	mde -> memtester[this_mt].uAddrFailCnt = TmtMemWriteAndVerify (mde->memtester[this_mt].start_addr_cached,
																   mde->memtester[this_mt].test_wsize,
																   &mde->memtester[this_mt].tiny,
																   mde->random_seed,
																   mde->memtester[this_mt].uAddxFail,
																   ADDR_FAIL_CNT,
																   ADDR_FAIL_CNT,
																   0, mde);
	
	// The failing addresses are cacheable addresses 
	// There is no need to translate as in case 0 and 1
#if 0
	if ( mde -> memtester[this_mt].uAddrFailCnt > ADDR_FAIL_CNT)
	  mde -> memtester[this_mt].uAddrFailCnt = ADDR_FAIL_CNT;
	
	// translate uncached to cached address
	cache_to_uncache_address_offset = mde->memtester[this_mt].start_addr_cached - mde->memtester[this_mt].start_addr_uncached;
	
	for (ii = 0; ii < mde -> memtester[this_mt].uAddrFailCnt; ii++)  {
		mde->memtester[this_mt].uAddxFail [ii] = mde->memtester[this_mt].uAddxFail[ii] + cache_to_uncache_address_offset; 
	}
#endif

  }
  
  /// mode 3 done with write - now do the read test

  mtMode013ReadTest ( mde );

  return DIAG_RET_OK;

}

unsigned mtMode4Handler ( sMde_t *mde )
{
  // mode 4 is a NOP
  return 0;
}

unsigned mtMode5Handler ( sMde_t *mde )
{
  
  unsigned spinner = 0;
  unsigned sum = 0;
  while (spinner ++ < SPINNER_LIMIT*4) {
	sum += spinner;
  }
  return spinner;
}

MT_MODE_HANDLER all_mt_handlers [] = {
  { 0, "MT MODE0", mtMode0Handler },
  { 1, "MT MODE1", mtMode1Handler },
  { 2, "MT MODE2", mtMode2Handler },
  { 3, "MT MODE3", mtMode3Handler },
  { 4, "MT MODE4", mtMode4Handler },
  { 5, "MT MODE5", mtMode5Handler },
};

#define MT_MODE_HANDLER_COUNT (sizeof(all_mt_handlers)/sizeof(MT_MODE_HANDLER))


/*
  mtTestMain - This is the entry point to start the memory stress test

  inputs:     mde is the data struct which has the raw input text entered by the user
              all required test parameters are defined within mde
			  mode: Test mode is used to select a handler for a particular mode

  Function:   The memory stress starts with the specified options   
              Error statistics will be shown after the completion of one pass
  return:     Always returns 0 for success

 */
unsigned mtTestMain ( sMde_t *mde ) 
{
  unsigned total_bytes_per_loop;
  int inx;

  if ( mde->memtester_mode >= MT_MODE_HANDLER_COUNT) {
	host_printf ("Test mode %d not supported. Only mode 0 to %d is valid - exit\n", 
				 mde->memtester_mode, (MT_MODE_HANDLER_COUNT-1) );
	return DIAG_RET_BAD;
  }
  //
  // install the memory test handler according to the mode specified
  // all valid test handlers are defined in all_mt_handlers 
  // 
  mde->mt = &all_mt_handlers [mde->memtester_mode];
  mde->mt_mode_count  = MT_MODE_HANDLER_COUNT;

  //	do test init:

  mde->memtester_loop = 0;
  mde->memtester_exit = 0;

  if ( mde->random_seed == 0 ) mde->random_seed = get_random_seed ();

  /*
	Initialize the tester - clear error, reset random seed
   */
  for (inx = 0, total_bytes_per_loop = 0; inx < mde->memtester_count; inx ++) {
	host_printf ("initializing memtester #%d - %s\n", 
				 inx, mde->memtester [inx].name);

	if (mde->memtester [inx].stressor_present)	mde->memtester [inx].stressor_init ( mde );
	// initialize test done flag before entering the main test loop
	host_printf ("\tsetup test parameter\n");
	mde->memtester [inx].uTotalErr    = 0; 
	mde->memtester [inx].uTotalErrMap = 0; 
	mde->memtester [inx].done = 0; 

	mde->memtester [inx].tiny.mat1 = 0;
	mde->memtester [inx].tiny.mat2 = 0;
	mde->memtester [inx].tiny.tmat = 0;
	mde->memtester [inx].seed = mde->random_seed;	
	host_printf ("\tusing random seed %08x\n", mde->random_seed);
	mde->fdq_init ( mde, inx );
	total_bytes_per_loop += mde->memtester [inx].test_bsize;
  }

  // calcuate the number of MB tested for each iteration
  mde->mt_total_mb_per_loop = total_bytes_per_loop / (1024*1024);
  // The next two variables determine the bandwidth of the test
  mde->mt_total_mb_tested = 0;
  mde->mt_total_read_time_ms = 0;

  host_printf ("Total MB per loop %d\n", mde->mt_total_mb_per_loop );
  host_printf ("\n\nRequested memory test in mode %d [%s]\nCFG_TMT_RANDOM_MODE = %d\n", 
			   mde->mt -> id, mde->mt -> name, CFG_TMT_RANDOM_MODE );

  // mark the time test starts (normalized to elapsed seconds)
  mde->time_start.msec = host_get_elapsed_msec ( /*&mde->time_start.sec*/);

  host_printf ("\tTest to run ");
  if (mde->memtester_pass == 0) {
	if (mde->memtester_time_seconds == 0) host_printf ("non-stop\n");
	else host_printf ("for %d min %d sec\n", mde->memtester_time_seconds / 60, 
					  mde->memtester_time_seconds % 60 );
  } else {
	host_printf ("for %d passes\n", mde->memtester_pass);
  }

  host_printf ("\nHit q to abort - testing will exit upon completion of current pass\n\n");

  // do test main loop
  while ( mde->memtester_exit == 0 ) {

	host_printf ("\nLoop #%d:\n",mde->memtester_loop + 1 );
	/// perform memory test according to the mode selected
	mde-> mt->handler (  mde );
	// Completed one loop - do test status check and summary
	mde->memtester_loop ++;
	// check and show test status
	mtCheckAndShowTestStatus ( mde );


	// check exit condition
	// First, check if user hit 'q' key
	if ((host_console_rx_ready ()) && (host_console_read () == 'q')) {
          host_printf ("\nTesting aborted by user during loop %d run time %d seconds\n",
					   mde->memtester_loop, mde->time_now.sec);
		  mde->memtester_exit = 1; 
	}

	if ( mde->memtester_time_seconds != 0) {
	  // exit condition is run time expires, or pass count has been reached
	  if ( mde->time_now.sec >= mde->memtester_time_seconds) {
		mde->memtester_exit = 1; // exit due to time
		host_printf ("\n\nRun time set at %d seconds expired, exiting now\n",
					 mde->memtester_time_seconds );
	  }
	} 

	if ( mde->memtester_pass != 0 ) {
	  if (mde->memtester_loop >= mde->memtester_pass) {
		mde->memtester_exit = 1; // exit due to pass count
		host_printf ("\nCompleted %d loops - stop testing now\n", 
					 mde->memtester_pass );
	  }
	}

  }

  // testing completed - clean up first
  for (inx = 0; inx < mde->memtester_count; inx ++) {
	if (mde->memtester [inx].stressor_present)
	  mde->memtester [inx].stressor_stop ( mde );
  }

  // do test statistics and error summary
  for (inx = 0; inx < mde->memtester_count; inx ++) {
	mde->fdq_show_result ( mde, inx );
  }
  return DIAG_RET_OK;
}


/*
  mtRunTest - This is the command handler for the command 'mestt-run-test'

  inputs:     mde is the data struct which has the raw input text entered by the user
              Input text format: mtrt <mode><pass><run-time><seed>
			  <mode> 0 - write once, then each loop do read; 1 - each loop do write and read
			  2 - bitflip; 3 - write and read cached per loop; 4 - bitflip in address chunk
			  5 - write and read cached in address chunk per loop
			  <pass> is the number of passes to run. Enter decimal count (or 0 to run contnuously)
			  <run_time> The time to run test in seconds (dec)
			  NOTE: To run non-stop: set pass=0 and run_time=0
			  <seed> A value to seed the random number generator. Enter 0 to get a value provided by the system

  Function:   The memory stress starts with the specified options   
              Error statistics will be shown after the completion of one pass
  return:     Always returns 0 for success

 */

unsigned mtRunTest ( sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];

  mdeScanf (mde, "%s%d%d%d%x", discard, &mde->memtester_mode, 
			&mde->memtester_pass, &mde->memtester_time_seconds,
			&mde->random_seed );
  
  mtTestMain ( mde );

  return DIAG_RET_OK;
  
}

