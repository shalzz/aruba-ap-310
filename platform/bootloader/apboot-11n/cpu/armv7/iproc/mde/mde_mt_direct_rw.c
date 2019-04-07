/*
  This file is a part of mde_mt
  It defines the memory test suitable for a host that supports direct memory access.
 */


/* ==================== TMT Read Stress Test ============================ */


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
   uint32_t *p1, uExpect, uRead, ii, jj, kk;
   uint32_t uFailCnt = 0, uMismatch = 0;

   if (uStartAddx == INVALID_ADDRESS) return 0;
   

   host_printf ("TmtMemWriteAndVerify address (direct read) %08x size %08x seed %08x\n", uStartAddx,uSizeLongWords,uSeed);

   // initialize tiny
   tinymt32_init(pTiny, uSeed);
   
   // write the full size from start to end
   p1 = (uint32_t*)uStartAddx;
   for (ii = 0; ii < uSizeLongWords; ii++ )
   {
#if CFG_TMT_RANDOM_MODE == 2
      if ((ii &1) == 0) uExpect = tinymt32_generate_uint32(pTiny);
      else uExpect = ~uExpect;
#else
      uExpect = tinymt32_generate_uint32(pTiny);
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
   tinymt32_init(pTiny, uSeed);
   
   // read one word at a time from start to end
   for (ii = 0, p1 = (uint32_t*)uStartAddx; ii < uSizeLongWords; ii++, p1++)
   {
#if CFG_TMT_RANDOM_MODE == 2
      if ((ii &1) == 0) uExpect = tinymt32_generate_uint32(pTiny);
      else uExpect = ~uExpect;
#else
      uExpect = tinymt32_generate_uint32(pTiny);
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

unsigned mtMode013ReadTest (  sMde_t *mde )
{
  unsigned ii, jj, kk;

  //  unsigned expected;

  unsigned diff;
  unsigned size = mde->memtester [0].test_wsize;
  unsigned read_time;
  unsigned mt;
  unsigned exp_data;
  unsigned fdq_result;
  MT_TIME read_start;
  MT_TIME read_stop;
  tinymt32_t tinymt;
  // Do cached read
  host_printf ("MT mode 013 with local tiny -> Reading Data:\n");

  // initialization
  tinymt.mat1 = 0;
  tinymt.mat2 = 0;
  tinymt.tmat = 0;

  for (mt = 0; mt < mde->memtester_count; mt ++ ) {
	// set the read pointer to the cached address of this memtester
	mde->memtester [mt].p_read = (unsigned *)mde->memtester [mt].start_addr_cached;
	tinymt32_init ( &mde->memtester [mt].tiny , mde->memtester [mt].seed );
	tinymt32_init ( &tinymt , mde->memtester [mt].seed );
  }

  read_start.msec = host_get_elapsed_msec ();

  // Do staggered read, memc0, memc1, memc0, memc1, ...
  // Do test for all memtesterd found: read a location, compare against expected
  for (ii = 0, jj = 0, kk = 0; ii < size;   ii++ ) {
	// First, establish the expected data - use mtester 0 tiny because the data are the same in all regions
	// There is at least one memory tester 
#if CFG_TMT_RANDOM_MODE == 0
	exp_data = tinymt32_generate_uint32( &tinymt);
#endif
      
#if CFG_TMT_RANDOM_MODE == 1
	if ( mt == 0) exp_data = tinymt32_generate_uint32( &tinymt);
#endif
      
#if CFG_TMT_RANDOM_MODE == 2 
	if ((ii & 1) == 0) exp_data = tinymt32_generate_uint32( &tinymt);
	else exp_data = ~exp_data;
#endif


	mde->memtester[0].act_data = *mde->memtester [0].p_read;

      // expected now has the correct value
	if (mde->memtester[0].act_data != exp_data) {
	  // check if the address is in failed array
	  // assume that this failing address has been recorded
	  diff = 0;
	  
	  for (; jj < mde->memtester [0].uAddrFailCnt; jj++) {
		if ((unsigned)mde->memtester [0].p_read < mde->memtester[0].uAddxFail[jj]) {
		  // read address smaller than what's in the array, not found
		  break;
		}
		else if ((unsigned)mde->memtester[0].p_read == mde->memtester[0].uAddxFail[jj]) {
		  // found in the array, increment array to next address
		  diff = 1;
		  continue;
		}
		else {
		  // read address larger than what's in the array, this is unexpected
		  host_printf(" **Unexpected index %d uAddxFail1 %08x addr %08x read %08x\n", \
			   jj, mde->memtester[0].uAddxFail[jj], mde->memtester [0].p_read, mde->memtester[0].act_data);
		}
	  }              
	  // do not accumulate this error if found in failed array
	  if (diff) {
		//debug code
		DBGP (" **Found index %d uAddxFail1 %08x addr %08x read %08x\n", \
			  jj-1, mde->memtester[0].uAddxFail[jj-1], mde->memtester[0].p_read, mde->memtester[0].act_data);
	  } else {
		// otherwise accumulate this error
		// The failing address is at mde->memtester[0].p_read
		mde->memtester[0].uTotalErr++;
		//// accumulate error statistics
		diff = mde->memtester[0].act_data ^ exp_data;
		mde->memtester[0].uTotalErrMap |= fdq_binary ( mde->memtester[0].p_read, diff);
		/// print this error first
		//     find_all_errors (p1, data1, expected1, 0);
		// find_all_errors needs addx, read data, expected data
		
		mde->memtester[0].addx = (unsigned) mde->memtester [0].p_read;
		mde->memtester[0].exp_data = exp_data;
		
		mde->fdq_log_error ( mde, 0 );
		
		DBGP ("  **Failed: mt %d: @%08x Expected %08x read %08x XOR %08x\n", \
			  0, mde->memtester[0].addx, mde->memtester[0].exp_data, mde->memtester[0].act_data, diff);
	  }
	}
	// update the read pointer for first memory tester
	mde->memtester [0].p_read ++;
	  
	// continue testing if there is more than one memory tester
	  
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
	mde->memtester [1].p_read ++;
#endif
  } // for loop reading the full size

  // done testing for each tester to cover the entire range
  // accumulate the total read time in ms
  read_stop.msec = host_get_elapsed_msec ();
  read_time = read_stop.msec - read_start.msec;
  mde->mt_total_read_time_ms += read_time;

  return DIAG_RET_OK;
}



