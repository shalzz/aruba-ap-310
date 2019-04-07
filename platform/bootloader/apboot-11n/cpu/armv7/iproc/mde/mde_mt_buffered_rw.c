/*
  This file is a part of mde_mt
  It defines the memory test suitable for a host that supports direct memory access.
 */


/* ==================== TMT Read Stress Test ============================ */

#define CFG_TMT_RANDOM_MODE 2

/************************************************************************
 * Function: TmtMemWriteAndVerify
 * 
 * This function performs TMT writes to the memory, and performs an immediate
 * readback check.  This ensures that the write data is written correctly to
 * memory. 
 *
 * The underlying assumption is that write failures are due to data bus noise
 * (ISI, reflections), and not due to address bus error.
 *
 * Params:
 *    uStartAddx        [IN] - Uncached starting address.
 *    uSizeLongWords    [IN] - Size to write, in long words (32 bits).
 *    pTiny             [I/O]- Pointer to tinymt32_t context.
 *    uSeed             [IN] - Seed for TMT.
 *    puFailedAddx      [I/O]- Array of failed addresses (correctable ones).
 *    uMaxFailedAddx    [IN] - Max size of the failed addx array.
 *    uMaxWriteRetry    [IN] - Max number of write retry if failed to read 
 *                             read back the correct value. If exceed this 
 *                             number, address has failed.
 *    uFailedAddxOffset [IN] - Apply an offset to the failed address in the
 *                             puFailedAddx, to allow easier comparison later.
 *    uVerbosity        [IN] - Verbosity, >0 to output failed address.
 *
 * Returns:
 *
 * Return the total uncorrectable + correctable number of WR retry failed
 * address count. 
 *  - if <= uMaxFailedAddx, then all failed addresses are accounted
 *    for in puFailedAddx and they are correctable.
 *  - if > uMaxFailedAddx, then only the uMaxFailedAddx count of addresses
 *    are accounted for in puFailedAddx and correctable, the rest were not
 *    in the array and are uncorrectable.
 *
 * The tinymt32_t context is modified during this function.
 *
 ************************************************************************/
uint32_t TmtMemWriteAndVerify ( uint32_t    uStartAddx,
                               uint32_t    uSizeLongWords, 
                               tinymt32_t* pTiny,
                               uint32_t    uSeed,
                               uint32_t*   puFailedAddx,
                               uint32_t    uMaxFailedAddx,
                               uint32_t    uMaxWriteRetry,
                               uint32_t    uVerbose,
							   sMde_t *mde)
{
   uint32_t *p1, uExpect = 0, uRead, ii, jj, kk;
   uint32_t uFailCnt = 0, uMismatch = 0;
   tinymt32_t ltiny;
   if (uStartAddx == INVALID_ADDRESS) return 0;
   

   host_printf ("TmtMemWriteAndVerify address (direct read) %08x size %08x seed %08x\n", uStartAddx,uSizeLongWords,uSeed);

   // initialize tiny
   ltiny.mat1 = 0; ltiny.mat2 = 0; ltiny.tmat = 0;
   tinymt32_init(&ltiny, uSeed);
   
   // write the full size from start to end
   p1 = (uint32_t*)uStartAddx;
   for (ii = 0; ii < uSizeLongWords; ii++ )
   {
#if CFG_TMT_RANDOM_MODE == 2
	 if ((ii &1) == 0) uExpect = tinymt32_generate_uint32(&ltiny);
      else uExpect = ~uExpect;
#else
	 uExpect = tinymt32_generate_uint32(&ltiny);
#endif
	  // DEBUG ONLY
	  //	  if (ii < 10) printf ("\twrite to %08x with %08x\n", p1, uExpect);
	  //	  else {	  if (ii % 0x8000 == 0) printf ("%08x ", ii); }

      *p1++ = uExpect;
   }
   // flush data - 
   // NOTE: need to determine if flushing is required
   //   flush_data_cache(uStartAddx, uSizeLongWords * BYTES_PER_WORD, 0);
      
   // re-initialize tiny to generate the expected values
   tinymt32_init(&ltiny, uSeed);
   
   // read one word at a time from start to end
   for (ii = 0, p1 = (uint32_t*)uStartAddx; ii < uSizeLongWords; ii++, p1++)
   {
#if CFG_TMT_RANDOM_MODE == 2
	 if ((ii &1) == 0) uExpect = tinymt32_generate_uint32(&ltiny);
	 else uExpect = ~uExpect;
#else
	 uExpect = tinymt32_generate_uint32(&ltiny);
#endif
	 uRead = *((volatile uint32_t*)p1);
      
      if (uExpect != uRead)
      {
         uMismatch++;
         
         // read multiple times
         for (kk = 0; kk < ADDR_FAIL_CNT; kk++)
         {
            uRead = *((volatile uint32_t *)p1);
            // stop if correct
            if (uRead == uExpect)
               break;
         }

         // still incorrect, retry write and
         if (uExpect != uRead)
         {
            jj = 0;
            do
            {
               // write up to uMaxWriteRetry times
               *p1 = uExpect;
               
               // read multiple times
               for (kk = 0; kk < ADDR_FAIL_CNT; kk++)
               {
                  uRead = *((volatile uint32_t *)p1);
                  // if correct, done
                  if (uRead == uExpect)
                     break;
               }
               jj++;
            } while (jj < uMaxWriteRetry && uRead != uExpect);  

            // cannot match in max number of tries, this address has failed
            if (uExpect != uRead)
            {
               // save the failing address into the array
               // that has max uMaxFailedAddx entries
               if (uFailCnt < uMaxFailedAddx)
                  puFailedAddx[uFailCnt] = ((uint32_t)p1);
               // increment total failure count;
               uFailCnt++; 
               if (uVerbose)
                  host_printf ("  **WriteFailed at %08x Expected %08x Read %08x XOR %08x Uncorrectable after %d Retries\n",
							   p1, uExpect, uRead, uExpect ^ uRead, uMaxWriteRetry);
            }
         }
         // otherwise seen correct value in multiple reads, continue    
      } 
      // else first read is good, continue
   }

   host_printf(" Write Verify: corrected %d entries\n", (uMismatch - uFailCnt));
   if (uFailCnt)
   {
      if (uFailCnt > uMaxFailedAddx)
         host_printf(" %d uncorrectable. This is more than %d, test results will contain write failures.\n", uFailCnt, uMaxFailedAddx);
      else
         host_printf(" %d uncorrectable, test will ignore the first %d entries\n", uFailCnt, uMaxFailedAddx);
   }

   return uFailCnt; 
}

#define SPINNER_LIMIT 0x10000000
#define ERROR_LIMIT   16

extern unsigned int fdq_binary(unsigned int addx, unsigned int logical_err_map);

#if 1
unsigned mtMode013ReadTest (  sMde_t *mde )
{
#define RDBUF_ISIZE 4096
#define RDBUF_BSIZE (RDBUF_ISIZE*4)

  unsigned ii, jj, kk;

  //  unsigned expected;

  unsigned diff;
  unsigned size = mde->memtester [0].test_wsize;
  unsigned read_time;
  unsigned mt;
  unsigned exp_data = 0;
//  unsigned fdq_result;
  // allocate read buffer

  // NOTE: The size of total memory to test MUST be a multiple of 1024
  // This should not be an issue if the block size is in MEGA bytes range

  unsigned readbuf [RDBUF_ISIZE];
//  unsigned *pbfr;
  unsigned *pbfv;

//  int brinx;
  int bvinx;

  MT_TIME read_start;
  MT_TIME read_stop;

  tinymt32_t ltiny;
  unsigned act_data;
  unsigned error = 0;
  // Do cached read


#define USE_MCP 1

#ifndef METHOD
#define METHOD "Free running"
#endif

  // announce the error handling method
  host_printf ("MT mode 013 -> MCP read (size %d INTS) [%s]\n", RDBUF_ISIZE, METHOD);

  // initialization

  ltiny.mat1 = 0;
  ltiny.mat2 = 0;
  ltiny.tmat = 0;

  for (mt = 0; mt < mde->memtester_count; mt ++ ) {
	// set the read pointer to the cached address of this memtester
	mde->memtester [mt].p_read = (unsigned *)mde->memtester [mt].start_addr_cached;
	tinymt32_init ( &mde->memtester [mt].tiny , mde->memtester [mt].seed );
	tinymt32_init ( &ltiny , mde->memtester [mt].seed );
  }
  // record the start time before read test
  read_start.msec = host_get_elapsed_msec ();

  // Do staggered read, memc0, memc1, memc0, memc1, ...
  // Do test for all memtesterd found: read a location, compare against expected

  // WARNING: hardwired to memtester 0

  for (ii = 0, jj = 0, kk = 0; ii < size;
	   /* DO NOT increment ii here - do it inside the looop*/ ) {
	// read a block of data 
#if USE_MCP
	memcpy ( readbuf, mde->memtester [0].p_read, RDBUF_BSIZE );
#else
	for (brinx = 0, pbfr = mde->memtester [0].p_read; brinx < RDBUF_ISIZE; brinx++ ) {
	  readbuf [ brinx ] = *pbfr++;
	}
#endif


	// verify a block of data - one INT at a time (as in direct mode)
	for (bvinx = 0, pbfv = mde->memtester [0].p_read; bvinx < RDBUF_ISIZE; bvinx++, pbfv++ ) {

#if CFG_TMT_RANDOM_MODE == 0
#warning: RANDOM_MODE 0
	  exp_data = tinymt32_generate_uint32( &ltiny);
#endif
      
#if CFG_TMT_RANDOM_MODE == 1
#warning: RANDOM_MODE 1
	  if ( mt == 0) exp_data = tinymt32_generate_uint32( &ltiny);
#endif
      
#if CFG_TMT_RANDOM_MODE == 2 
//#warning: RANDOM_MODE 2
	  if ((bvinx & 1) == 0) exp_data = tinymt32_generate_uint32( &ltiny);
	  else exp_data = ~exp_data;
#endif
	  
	  if ( readbuf [bvinx] == exp_data ) continue;

      // Set actual data
      act_data = readbuf[ bvinx ];
      mde->memtester[ 0 ].act_data = readbuf[ bvinx ];

#if DO_ERROR_COUNT
	  error ++;
#endif

#if DO_PRINTF_ONLY 
	  host_printf ("Failed\n");
#endif

#if DO_FULL_ERROR_REPORT
      // expected now has the correct value
	  // check if the address is in failed array
	  // assume that this failing address has been recorded
	  diff = 0;
	  
	  for (; jj < mde->memtester [0].uAddrFailCnt; jj++) {
		if ((unsigned)pbfv < mde->memtester[0].uAddxFail[jj]) {
		  // read address smaller than what's in the array, not found
		  break;
		}
		else if ((unsigned)pbfv == mde->memtester[0].uAddxFail[jj]) {
		  // found in the array, increment array to next address
		  diff = 1;
		  continue;
		}
		else {
		  // read address larger than what's in the array, this is unexpected
		  host_printf(" **Unexpected index %d uAddxFail1 %08x addr %08x read %08x\n", \
					  jj, mde->memtester[0].uAddxFail[jj], pbfv, act_data);
		}
	  }              
	  // do not accumulate this error if found in failed array
	  if (diff) {
		//debug code
		DBGP (" **Found index %d uAddxFail1 %08x addr %08x read %08x\n", \
			  jj-1, mde->memtester[0].uAddxFail[jj-1], pbfv, act_data);
	  } else {
		// otherwise accumulate this error
		// The failing address is at pbfv
		mde->memtester[0].uTotalErr++;
		//// accumulate error statistics
		diff = mde->memtester[0].act_data ^ exp_data;
		mde->memtester[0].uTotalErrMap |= fdq_binary ( (unsigned)pbfv, diff);
		/// print this error first
		//     find_all_errors (p1, data1, expected1, 0);
		// find_all_errors needs addx, read data, expected data
		
		mde->memtester[0].addx = (unsigned) pbfv;
		mde->memtester[0].exp_data = exp_data;
		
		mde->fdq_log_error ( mde, 0 );
		
		host_printf ("  **Failed: mt %d: @%08x Expected %08x read %08x XOR %08x\n", \
					 0, mde->memtester[0].addx, mde->memtester[0].exp_data, mde->memtester[0].act_data, diff);
	  }
#endif
	}

	// FLUSH the data - to prevent L2 getting filled
	//	SHMOO_FLUSH_DATA_TO_DRAM ( readbuf, RDBUF_BSIZE);
	invalidate_dcache_range ( (unsigned long)mde->memtester [0].p_read, ((unsigned) mde->memtester [0].p_read+RDBUF_BSIZE));
	//	SHMOO_FLUSH_DATA_TO_DRAM ( mde->memtester [0].p_read, RDBUF_BSIZE);

	mde->memtester [0].p_read = (unsigned*)((unsigned)(mde->memtester [0].p_read) + RDBUF_BSIZE);
	ii += RDBUF_ISIZE;

	// continue testing if there is more than one memory tester
#if 0 // need to upgrade this	  
#if LOCAL_MEM_TESTER_COUNT > 1 // more than one memory tester
	mde->memtester[1].act_data = *mde->memtester [1].p_read;

	// expected now has the correct value
	if (mde->memtester[1].act_data != exp_data) {
	  // check if the address is in failed array
	  diff = 0;
	  for (; jj < mde->memtester [1].uAddrFailCnt; jj++) {
		if ((unsigned)mde->memtester [1].p_read < mde->memtester[1].uAddxFail[jj]) {
		  // read address smaller than what's in the array, not found
		  break;
		}
		else if ((unsigned)mde->memtester[1].p_read == mde->memtester[1].uAddxFail[jj]) {
		  // found in the array, increment array to next address
		  diff = 1;
		  continue;
		}
		else {
		  // read address larger than what's in the array, this is unexpected
		  DBGP(" **Unexpected index %d uAddxFail1 %08x addr %08x read %08x\n", \
			   jj, mde->memtester[1].uAddxFail[jj], mde->memtester [1].p_read, mde->memtester[1].act_data);
		}
	  }              
	  // do not accumulate this error if found in failed array
	  if (diff) {
		//debug code
		DBGP (" **Found index %d uAddxFail1 %08x addr %08x read %08x\n", \
			  jj-1, mde->memtester[1].uAddxFail[jj-1], mde->memtester[1].p_read, mde->memtester[1].act_data);
	  } else {
		// otherwise accumulate this error
		mde->memtester[1].uTotalErr++;
		//// accumulate error statistics
		diff = mde->memtester[1].act_data ^ exp_data;
		mde->memtester[1].uTotalErrMap |= fdq_binary ( mde->memtester[1].p_read, diff);
		mde->memtester[1].addx = (unsigned) mde->memtester [1].p_read;
		mde->memtester[1].exp_data = exp_data;
		
		mde->fdq_log_error ( mde, 1 );
		
		DBGP ("  **Failed: mt %d: @%08x Expected %08x read %08x XOR %08x\n", \
			  1, mde->memtester[1].addx, mde->memtester[1].exp_data, mde->memtester[1].act_data, diff);
	  }
	}
#endif
#endif
  } // for loop reading the full size

  // done testing for each tester to cover the entire range
  // accumulate the total read time in ms
  read_stop.msec = host_get_elapsed_msec ();
  read_time = read_stop.msec - read_start.msec;
  mde->mt_total_read_time_ms += read_time;
  return error;
  //  return DIAG_RET_OK;
}



#else
inline unsigned mtMode013ReadTest (  sMde_t *mde )
{
#define RDBUF_ISIZE 4096
#define RDBUF_BSIZE (RDBUF_ISIZE*4)

  unsigned ii, jj, kk;

  //  unsigned expected;

  unsigned diff;
  unsigned size = mde->memtester [0].test_wsize;
  unsigned read_time;
  unsigned mt;
  unsigned exp_data;
  unsigned fdq_result;
  // allocate read buffer

  // NOTE: The size of total memory to test MUST be a multiple of 1024
  // This should not be an issue if the block size is in MEGA bytes range

  unsigned readbuf [RDBUF_ISIZE];
  unsigned *pbfr;
  unsigned *pbfv;

  int brinx;
  int bvinx;

  MT_TIME read_start;
  MT_TIME read_stop;
  tinymt32_t tinymt;

  // initialization
  tinymt.mat1 = 0;
  tinymt.mat2 = 0;
  tinymt.tmat = 0;

#define USE_MCP 1

#if USE_MCP
  host_printf ("MT mode 013 -> MCP read (size %d INTS):\n", RDBUF_ISIZE);
#else
  host_printf ("MT mode 013 -> Buffered read (size %d INTS):\n", RDBUF_ISIZE);
#endif
  // initialization
  
  for (mt = 0; mt < mde->memtester_count; mt ++ ) {
	// set the read pointer to the cached address of this memtester
	mde->memtester [mt].p_read = (unsigned *)mde->memtester [mt].start_addr_cached;
	tinymt32_init ( &mde->memtester [mt].tiny , mde->memtester [mt].seed );
  }
  // record the start time before read test
  read_start.msec = host_get_elapsed_msec ();

  // Do staggered read, memc0, memc1, memc0, memc1, ...
  // Do test for all memtesterd found: read a location, compare against expected

  // WARNING: hardwired to memtester 0

  for (ii = 0, jj = 0, kk = 0; ii < size;
	   /* DO NOT increment ii here - do it inside the looop*/ ) {
	// read a block of data 
#if USE_MCP
	memcpy ( readbuf, mde->memtester [0].p_read, RDBUF_BSIZE );
#else
	for (brinx = 0, pbfr = mde->memtester [0].p_read; brinx < RDBUF_ISIZE; brinx++ ) {
	  readbuf [ brinx ] = *pbfr++;
	}
#endif

	// verify a block of data - one INT at a time (as in direct mode)
	for (bvinx = 0, pbfv = mde->memtester [0].p_read; bvinx < RDBUF_ISIZE; bvinx++, pbfv++ ) {
#if CFG_TMT_RANDOM_MODE == 0
#warning: RANDOM_MODE 0
	  exp_data = tinymt32_generate_uint32( &tinymt);
#endif
      
#if CFG_TMT_RANDOM_MODE == 1
#warning: RANDOM_MODE 1
	  if ( mt == 0) exp_data = tinymt32_generate_uint32( &tinymt);
#endif
      
#if CFG_TMT_RANDOM_MODE == 2 
//#warning: RANDOM_MODE 2
	  if ((bvinx & 1) == 0) exp_data = tinymt32_generate_uint32( &tinymt);
	  else exp_data = ~exp_data;
#endif

	  mde->memtester[0].act_data = *pbfv;

      // expected now has the correct value
	  if (mde->memtester[0].act_data != exp_data) {
		// check if the address is in failed array
		// assume that this failing address has been recorded
		diff = 0;
		
		for (; jj < mde->memtester [0].uAddrFailCnt; jj++) {
		  if ((unsigned)pbfv < mde->memtester[0].uAddxFail[jj]) {
			// read address smaller than what's in the array, not found
			break;
		  }
		  else if ((unsigned)pbfv == mde->memtester[0].uAddxFail[jj]) {
			// found in the array, increment array to next address
			diff = 1;
			continue;
		  }
		  else {
			// read address larger than what's in the array, this is unexpected
			host_printf(" **Unexpected index %d uAddxFail1 %08x addr %08x read %08x\n", \
						jj, mde->memtester[0].uAddxFail[jj], pbfv, mde->memtester[0].act_data);
		  }
		}              
		// do not accumulate this error if found in failed array
		if (diff) {
		  //debug code
		  RDI_PRINTF (" **Found index %d uAddxFail1 %08x addr %08x read %08x\n", \
					  jj-1, mde->memtester[0].uAddxFail[jj-1], pbfv, mde->memtester[0].act_data);
		} else {
		  // otherwise accumulate this error
		  // The failing address is at pbfv
		  mde->memtester[0].uTotalErr++;
		  //// accumulate error statistics
		  diff = mde->memtester[0].act_data ^ exp_data;
		  mde->memtester[0].uTotalErrMap |= fdq_binary ( pbfv, diff);
		  /// print this error first
		  //     find_all_errors (p1, data1, expected1, 0);
		  // find_all_errors needs addx, read data, expected data
		  
		  mde->memtester[0].addx = (unsigned) pbfv;
		  mde->memtester[0].exp_data = exp_data;
		  
		  mde->fdq_log_error ( mde, 0 );
		  
		  RDI_PRINTF ("  **Failed: mt %d: @%08x Expected %08x read %08x XOR %08x\n", \
					  0, mde->memtester[0].addx, mde->memtester[0].exp_data, mde->memtester[0].act_data, diff);
		}
	  }
	}
	mde->memtester [0].p_read = (unsigned*)((unsigned)(mde->memtester [0].p_read) + RDBUF_BSIZE);
	ii += RDBUF_ISIZE;

	// continue testing if there is more than one memory tester
#if 0 // need to upgrade this	  
#if LOCAL_MEM_TESTER_COUNT > 1 // more than one memory tester
	mde->memtester[1].act_data = *mde->memtester [1].p_read;

	// expected now has the correct value
	if (mde->memtester[1].act_data != exp_data) {
	  // check if the address is in failed array
	  diff = 0;
	  for (; jj < mde->memtester [1].uAddrFailCnt; jj++) {
		if ((unsigned)mde->memtester [1].p_read < mde->memtester[1].uAddxFail[jj]) {
		  // read address smaller than what's in the array, not found
		  break;
		}
		else if ((unsigned)mde->memtester[1].p_read == mde->memtester[1].uAddxFail[jj]) {
		  // found in the array, increment array to next address
		  diff = 1;
		  continue;
		}
		else {
		  // read address larger than what's in the array, this is unexpected
		  DBGP(" **Unexpected index %d uAddxFail1 %08x addr %08x read %08x\n", \
			   jj, mde->memtester[1].uAddxFail[jj], mde->memtester [1].p_read, mde->memtester[1].act_data);
		}
	  }              
	  // do not accumulate this error if found in failed array
	  if (diff) {
		//debug code
		DBGP (" **Found index %d uAddxFail1 %08x addr %08x read %08x\n", \
			  jj-1, mde->memtester[1].uAddxFail[jj-1], mde->memtester[1].p_read, mde->memtester[1].act_data);
	  } else {
		// otherwise accumulate this error
		mde->memtester[1].uTotalErr++;
		//// accumulate error statistics
		diff = mde->memtester[1].act_data ^ exp_data;
		mde->memtester[1].uTotalErrMap |= fdq_binary ( mde->memtester[1].p_read, diff);
		mde->memtester[1].addx = (unsigned) mde->memtester [1].p_read;
		mde->memtester[1].exp_data = exp_data;
		
		mde->fdq_log_error ( mde, 1 );
		
		DBGP ("  **Failed: mt %d: @%08x Expected %08x read %08x XOR %08x\n", \
			  1, mde->memtester[1].addx, mde->memtester[1].exp_data, mde->memtester[1].act_data, diff);
	  }
	}
#endif
#endif
  } // for loop reading the full size

  // done testing for each tester to cover the entire range
  // accumulate the total read time in ms
  read_stop.msec = host_get_elapsed_msec ();
  read_time = read_stop.msec - read_start.msec;
  mde->mt_total_read_time_ms += read_time;

  return DIAG_RET_OK;
}



#endif
