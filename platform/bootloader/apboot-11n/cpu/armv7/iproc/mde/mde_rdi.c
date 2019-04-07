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
    *  Filename: cde_rd_eye.c
    *
    *  Function: plot rd eye shmoo
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
    * 
    * $Id::                                                       $:
    * $Rev::file =  : Global SVN Revision = 2057                  $:
    * 
 */

#include "mde_common_headers.h"


#include "tinymt32.h"
#include "ddr40_phy_control_regs.h"
#include "cde_local_memc_ddr_0.h"
#include "cde_local_memc_gen_0.h"
#include "local_memc_arb_0.h"
#include "ddr40_phy_word_lane_0.h"

extern int mdeCountTokens(char * str);
extern void mdeLineScanf ( char *line, char *ctl, ...);
extern int is_big_endian(void);
extern void endian_swap(char * p);

extern void find_dq_show_test(void);
extern void flush_data_cache (unsigned addr, unsigned bytes, unsigned verbose);
#if PHY_WIDTH == 32
extern unsigned int fdq_binary(unsigned int addx, unsigned int logical_err_map);
#else
extern unsigned int fdq_binary_16(unsigned int addx, unsigned int logical_err_map);
#endif

//DDR-EYE-SHMOO: remove static, so mde_wr_eye.c can call this function
/*static*/void  host_wait_not_busy ( unsigned offset ) ;


#if RDI_DBGP
#define ENTER(a) RDI_PRINT("Enter %s\n",a)
#define EXIT(a)  RDI_PRINT("Exit  %s\n",a)
#else
#define ENTER(a) //
#define EXIT(a)  //
#endif

void generate_dq_masks(char *ptr_mask_non_shuff, char * ptr_mask_shuff, char * ptr_non_shuff_override_value,
					   char * ptr_shuff_override_value, unsigned int dq_override, unsigned int dq_override_val);


int find_dq_inverse(unsigned int byte_addx, int DQ);

int fdq_binary_dual_cycle(unsigned int addx, unsigned int logical_err_map,
						  unsigned int *dq_error,
						  int is_32b_phy ) ;

static void timeout_ns( unsigned ticks )
{
  while ( ticks-- > 0 ) /* nothing */ ;
}

//#define NL() 

static void NL (void)
{
  host_console_write(LF); 
  host_console_write(CR); 
}

void prt_rd_eye_shmoo_header (unsigned print_flag)
{
  char result_str [100];


  int i;
  char prefix_str [] = "rd_eye:             ";

  result_str [0] = 0;
  if (print_flag) host_printf ("\n%s", prefix_str);
  for (i = 0; i < 64; i ++ ) {
	if (print_flag) host_console_write ('0' + i/10);
  }
  if (print_flag) host_printf ("\n%s", prefix_str);
  for (i = 0; i < 64; i ++ ) {
	if (print_flag) host_console_write ('0' + (i % 10));
  }
  if (print_flag) NL();
}

static inline void prt_rd_eye_shmoo_header_mode1 (unsigned print_flag)
{
  int i;
  char prefix_str [] = "rd_eye:               ";

  if (print_flag) host_printf ("\n%s", prefix_str);
  for (i = 0; i < 64; i ++ ) {
	if (print_flag) host_console_write ('0' + i/10);
  }
  if (print_flag) host_printf ("\n%s", prefix_str);
  for (i = 0; i < 64; i ++ ) {
	if (print_flag) host_console_write ('0' + (i % 10));
  }
  if (print_flag) NL();
}


typedef struct {
  unsigned vdl_r_p;
  unsigned vdl_r_n;  
} VDL_R_PN;

#define VDL_R_MASK 0xFF // 8 bit value

void prt_rd_eye_shmoo_result_166 (MEMC_INFO *this_memc,
                                  unsigned *p_init_setting,
                                  unsigned phy,
                                  unsigned print_flag,
                                  unsigned int override)
{
   /*
     For JIRA 166
     init_setting has 64 values, for R_VDL_P and R_VDL_N of this memc
     For each vref to plot, mark the original R_P with a 'p', R_N with a 'N'
     If the R_P and the R_N are identical, then mark with an 'X'


   */

   char result_str [100];

   int bit;
   unsigned bit_masked_result;
   unsigned upper_limit;
   unsigned vref, vdl, mV;
   unsigned zero_val;
   unsigned space_val;
   unsigned x_val;
   unsigned plus_val;
   unsigned minus_val;
   unsigned original_p = 0, original_n = 0;
   unsigned int ignore_bits;
   unsigned int startpos, start_vref = 10, end_vref = 55, mid_vref = 32;


   VDL_R_PN *p_this_pn_pair = (VDL_R_PN*)p_init_setting;
   unsigned *results_array = this_memc->results_array[this_memc->memc_id][0];

#ifdef BCHP_SID_BIGRAM_REG_START
   ALL_RDI_VDL *allvdl = (ALL_RDI_VDL*)RDI_STA_AREA_START;
#endif
   
#ifdef BCHP_SID_BIGRAM_REG_START
   if (this_memc->mode == 2)
   {
      reg_writ ((unsigned)(&(allvdl->data_valid_flag)), RDI_DATE_CODE);
      reg_writ ((unsigned)(&(allvdl->sta_vref)), this_memc->sta_vref);
      start_vref = this_memc->sta_vref;
      mid_vref = start_vref;
      end_vref = start_vref + 1;
   }
#endif
  
   // 0xFFFFFFFF = mask off so show all DQ
   if (override==0xFFFFFFFF)
   {
      ignore_bits = 0x00000000;
   }
   else
   {
      ignore_bits = override;
   }

   //  if (print_flag) dump_array ( p_init_setting, 64);

   zero_val  = 48;
   space_val = 32;
   x_val     = 32; // 120111 CKD: do not print the X per TD
   plus_val  = 43;
   minus_val = 45;

   upper_limit = PHY_WIDTH;

   for (bit = 0; bit < upper_limit; bit ++ )
   {
     
      if (ignore_bits & (1 << bit))
      {
         continue;
      }
      // For each bit, determine the original R_P and R_N value if we need to print
      if (print_flag)
      {
         original_p = (p_this_pn_pair [bit].vdl_r_p) & VDL_R_MASK;
         original_n = (p_this_pn_pair [bit].vdl_r_n) & VDL_R_MASK;

         host_printf ("\nrd_eye:  ********************  bit = %d (shmoo picked vdl = %d:%d)\n", bit, 
                    original_p, original_n );
      }
      if (this_memc->mode == 0)
         prt_rd_eye_shmoo_header(print_flag);
      else
         prt_rd_eye_shmoo_header_mode1(print_flag);

      for (vref = start_vref; vref < end_vref; vref ++ )
      {
         //     mV = (1500 * vref) \ 64    'ddr3 supply voltage = 1500 mV
         // NOTE: the divide in BBS is different

         //    mV = (1500 * vref) / 64 ; // 'ddr3 supply voltage = 1500 mV
         mV = VREF_TO_MV(vref); // 'ddr3 supply voltage = 1500 mV
         startpos = RDI_STA_VDL_INVALID;
         for (vdl = 0; vdl < 64; vdl ++ )
         {
            bit_masked_result = results_array[(64*vref)+vdl] & (1<<bit);
            if (bit_masked_result != 0)
               result_str [vdl] = minus_val;
            else
            {
               result_str [vdl] = plus_val;
#ifdef BCHP_SID_BIGRAM_REG_START
               if (this_memc->mode == 2 && vref == this_memc->sta_vref)
               {
                  if (startpos == RDI_STA_VDL_INVALID)
                     startpos = vdl;
                  endpos = vdl;
               }
#endif
            }
         }

#ifdef BCHP_SID_BIGRAM_REG_START
         if (this_memc->mode == 2 && vref == this_memc->sta_vref)
         {
            if (startpos == RDI_STA_VDL_INVALID)
            {
               reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].p_start)),
                         RDI_STA_VDL_INVALID);
               reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].p_end)),
                         RDI_STA_VDL_INVALID);
            }
            else
            {
               midpos = (startpos + endpos) / 2;
               wstart = startpos;
               wend = endpos;
               for (ii = midpos - 1; ii >= 0; ii--)
               {
                  if (result_str[ii] != plus_val)
                  {
                     wstart = ii + 1;
                     break;
                  }
               }
               for (ii = midpos + 1; ii < 64; ii++)
               {
                  if (result_str[ii] != plus_val)
                  {
                     wend = ii - 1;
                     break;
                  }
               }
               reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].p_start)),
                         wstart);
               reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].p_end)),
                         wend);
            }
         }
#endif
            
         // for the mid point vref, show the shmoo picked vdl
         if (vref == mid_vref)
         {
            // Mode 1 only cares about P
            result_str [original_p] = 'P';
            if (this_memc->mode == 0)
            {
               result_str [original_n] = 'N';
               if ( original_n == original_p )
                  result_str [original_p] = 'X';
            }
         } 
         
         result_str [vdl] = 0; /// terminate the string
         //        print "rd_eye: " & space(2) & mV & "mV " & Join(result_str,"")
         if (print_flag)
         {
            if (this_memc->mode == 0)
               host_printf ("rd_eye_%d_%02d: %04dmV %s\n", phy, bit, mV, result_str);
            else
               host_printf ("rd_eye_%d_%02d_p: %04dmV %s\n", phy, bit, mV, result_str);
         }
      }

      // mode 1 and 2 also print N
      if (this_memc->mode > 0)
      {
         if (print_flag)
            host_printf ("\n");
         
         results_array = this_memc->results_array[this_memc->memc_id][1];

         for (vref = start_vref; vref < end_vref; vref ++ )
         {
            //     mV = (1500 * vref) \ 64    'ddr3 supply voltage = 1500 mV
            // NOTE: the divide in BBS is different
            
            //    mV = (1500 * vref) / 64 ; // 'ddr3 supply voltage = 1500 mV
            mV = VREF_TO_MV(vref); // 'ddr3 supply voltage = 1500 mV
            startpos = RDI_STA_VDL_INVALID;
            for (vdl = 0; vdl < 64; vdl ++ )
            {
               bit_masked_result = results_array[(64*vref)+vdl] & (1<<bit);
               if (bit_masked_result != 0)
                  result_str [vdl] = minus_val;
               else
               {
                  result_str [vdl] = plus_val;
#ifdef BCHP_SID_BIGRAM_REG_START
                  if (this_memc->mode == 2 && vref == this_memc->sta_vref)
                  {
                     if (startpos == RDI_STA_VDL_INVALID)
                        startpos = vdl;
                     endpos = vdl;
                  }
#endif
               }
            }

#ifdef BCHP_SID_BIGRAM_REG_START
            if (this_memc->mode == 2 && vref == this_memc->sta_vref)
            {
               if (startpos == RDI_STA_VDL_INVALID)
               {
                  reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].n_start)),
                            RDI_STA_VDL_INVALID);
                  reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].n_end)),
                            RDI_STA_VDL_INVALID);
               }
               else
               {
                  midpos = (startpos + endpos) / 2;
                  wstart = startpos;
                  wend = endpos;
                  for (ii = midpos - 1; ii >= 0; ii--)
                  {
                     if (result_str[ii] != plus_val)
                     {
                        wstart = ii + 1;
                        break;
                     }
                  }
                  for (ii = midpos + 1; ii < 64; ii++)
                  {
                     if (result_str[ii] != plus_val)
                     {
                        wend = ii - 1;
                        break;
                     }
                  }
                  reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].n_start)),
                            wstart);
                  reg_writ ((unsigned)(&(allvdl->all_dqs[this_memc->memc_id][bit].n_end)),
                            wend);
               }
            }
#endif
				
            // for the mid point vref, show the shmoo picked vdl
            if (vref == mid_vref)
               result_str [original_n] = 'N';
               
            result_str [vdl] = 0; /// terminate the string
            //        print "rd_eye: " & space(2) & mV & "mV " & Join(result_str,"")
            if (print_flag)
               host_printf ("rd_eye_%d_%02d_n: %04dmV %s\n", phy, bit, mV, result_str);
         }
         
         results_array = this_memc->results_array[this_memc->memc_id][0];
      }
            
   }
   if (print_flag)  NL (); 
}
/////////////////


#if RDI_USE_MCP
//#warning: RDI test memory with memcpy method
#define MAX_BYTES_PER_CHUNK 4096
#else
#warning: RDI test memory with direct pointer method
#define MAX_BYTES_PER_CHUNK 0x8000
#endif

#define MAX_INT_PER_CHUNK  (MAX_BYTES_PER_CHUNK/4)

#define BYTES_PER_CHUNK    (MAX_BYTES_PER_CHUNK)
#define INTS_PER_CHUNK     (MAX_BYTES_PER_CHUNK/4)

#define BITS_IN_UNSIGNED 32
#define BITS_PER_NIBBLE  4
#define NIBBLE           0xF
#define NIBBLE_COUNT     (BITS_IN_UNSIGNED/BITS_PER_NIBBLE) // 8
#define MS_NIBBLE_POS    (BITS_IN_UNSIGNED-BITS_PER_NIBBLE)  // 28 : we need to print from MS bits to LS
#define A_F_BASE         ('A' - 10) // 0x37

void no_reg_writ (unsigned addr, unsigned val)
{
   // this is a NOP
}

void no_send_uart (const char c)
{
   // this is a NOP
}

int no_printf (const char *templat,...)
{
   unsigned sum = 0;
   char val;
   char *p = (char *) templat;

   while (1)
   {
      if ((val = *p ++) == 0) break;
      sum += (unsigned)val;
   }
   return sum;
}


//unsigned results_array[SIZE_RESULT_ARRAY];
void srdi_fill_memory (unsigned from, unsigned size_bytes, unsigned seed, OverrideInfo_t * pOverride)
{
   unsigned chunk = (size_bytes / BYTES_PER_CHUNK);
   unsigned *pf = (unsigned*)from;
   unsigned *pff;
  
   tinymt32_t tinymt;

   int inxChunk, inxFill;  

   unsigned int temp;
   unsigned int shuffled;
  
   unsigned int * data_buffer_32_mask_shuff;
   unsigned int * data_buffer_32_mask_non_shuff;
   unsigned int * data_buffer_32_shuff_override_val;
   unsigned int * data_buffer_32_non_shuff_override_val;

   ENTER("srdi_fill_memory");
   tinymt.mat1 = 0; tinymt.mat2 = 0; tinymt.tmat = 0;
   tinymt32_init(&tinymt, seed);

   // No override
   if (pOverride->uDqOverride == DQ_OVERRIDE_OFF) {
	 for (inxChunk = 0; inxChunk < chunk; inxChunk ++ )  {
	   pff = pf; 
	   // do one chunk
	   for (inxFill = 0; inxFill < INTS_PER_CHUNK; inxFill ++, pf++ ) {
		 *pf = tinymt32_generate_uint32(&tinymt);
	   }
	   flush_data_cache ((unsigned) pff, BYTES_PER_CHUNK, 0);
	 }
   }
   // Override
   else
   {
      data_buffer_32_mask_shuff              = (unsigned int *) pOverride->pMaskShuff;
      data_buffer_32_mask_non_shuff          = (unsigned int *) pOverride->pMaskNonShuff;
      data_buffer_32_shuff_override_val      = (unsigned int *) pOverride->pMaskShuffOverrideValue;
      data_buffer_32_non_shuff_override_val  = (unsigned int *) pOverride->pMaskNonShuffOverrideValue;
     
      for (inxChunk = 0; inxChunk < chunk; inxChunk ++ ) 
      {
         pff = pf; 
         // do one chunk
         for (inxFill = 0; inxFill < INTS_PER_CHUNK; inxFill ++, pf++ )
         {
            temp = tinymt32_generate_uint32(&tinymt);
         
           
           
            shuffled = (((unsigned int) pf >>8)&1 ) ^ (((unsigned int) pf>>7)&1);
            if (shuffled)
            {
               temp &=  ~data_buffer_32_mask_shuff[inxFill & 0x7]; 
               temp |=  (data_buffer_32_mask_shuff[inxFill & 0x7] & data_buffer_32_shuff_override_val[inxFill & 0x7]); 
            
               *pf  = temp;   
            }
            else
            {
               temp &=  ~data_buffer_32_mask_non_shuff[inxFill & 0x7];  
               temp |=  (data_buffer_32_mask_non_shuff[inxFill & 0x7] & data_buffer_32_non_shuff_override_val[inxFill & 0x7]);
               *pf  = temp;
            }
            
            //if ( (inxChunk==0) && (inxFill < 16) )  host_printf ("*** orig random %x %x\n", inxFill, temp);
            
         }
         flush_data_cache ((unsigned) pff, BYTES_PER_CHUNK, 0);
      }
   
   }
   EXIT("srdi_fill_memory");
}

/////
unsigned mips_test_memory (unsigned source,
                           unsigned size,
                           tinymt32_t *ptinymt,
                           unsigned seed,
                           OverrideInfo_t * pOverride)
{
  unsigned inx;
   unsigned src; 

   unsigned chunk_count = size / MAX_INT_PER_CHUNK;
   unsigned chunk;
   unsigned *ps,  ps_flush;
   unsigned cum_err = 0;
   unsigned cum_err_t = 0;
   unsigned test_over = 0;
   unsigned int expected;
   unsigned int shuffled;
  
   unsigned int * data_buffer_32_mask_shuff;
   unsigned int * data_buffer_32_mask_non_shuff;
   unsigned int * data_buffer_32_shuff_override_val;
   unsigned int * data_buffer_32_non_shuff_override_val;
   unsigned int error_mask;


   ENTER("mips_test_memory");
   ps = (unsigned*) source;
  
   error_mask = ~pOverride->uDqOverride;

   // override off
   if (pOverride->uDqOverride == 0xFFFFFFFF)
   {
      for (chunk = 0; chunk < chunk_count; chunk ++)
      {
         // host_printf ("testing chunk %08x  size %08x\n", ps, MAX_INT_PER_CHUNK );
   
         ps_flush = (unsigned) ps;
   
         for (inx = 0; inx < MAX_INT_PER_CHUNK; inx ++, ps++ )
         {
            src = *ps;
            expected = tinymt32_generate_uint32(ptinymt);
        
            if (src == expected) continue;
            // test failed
            // check if all DQ has failed at least once - if so, testing is over
            cum_err_t = (src ^ expected);
#if PHY_WIDTH == 32
            cum_err |= fdq_binary (ps, cum_err_t);
#else
            cum_err |= fdq_binary_16 ((unsigned)ps, cum_err_t);
#endif
            
            if (cum_err == MT_ALL_FAILED) {
               test_over = 1;
               break;
            }
         }
         // done with one chunk, flush D cache
         flush_data_cache (ps_flush, MAX_BYTES_PER_CHUNK, 0);
      
         if (test_over)
            break;
      }
   }
   // overrride on
   else
   {
      data_buffer_32_mask_shuff              = (unsigned int *) pOverride->pMaskShuff;
      data_buffer_32_mask_non_shuff          = (unsigned int *) pOverride->pMaskNonShuff;
      data_buffer_32_shuff_override_val      = (unsigned int *) pOverride->pMaskShuffOverrideValue;
      data_buffer_32_non_shuff_override_val  = (unsigned int *) pOverride->pMaskNonShuffOverrideValue;
   
      for (chunk = 0; chunk < chunk_count; chunk ++)
      {
         //  host_printf ("testing chunk %08x  size %08x\n", ps, MAX_INT_PER_CHUNK );
     
         ps_flush = (unsigned) ps;
     
         for (inx = 0; inx < MAX_INT_PER_CHUNK; inx ++, ps++ )
         {
            src = *ps;
            expected = tinymt32_generate_uint32(ptinymt);
      
            shuffled = (((unsigned int) ps >>8)&1 ) ^ (((unsigned int) ps>>7)&1);
            if (shuffled)
            {
               expected &=  ~data_buffer_32_mask_shuff[inx & 0x7]; 
               //expected |=  (data_buffer_32_mask_shuff[inx & 0x7] & data_buffer_32_shuff_override_val[inx & 0x7]); 
               src &=  ~data_buffer_32_mask_shuff[inx & 0x7];
            }
            else
            {
               expected &=  ~data_buffer_32_mask_non_shuff[inx & 0x7];  
               //expected |=  (data_buffer_32_mask_non_shuff[inx & 0x7] & data_buffer_32_non_shuff_override_val[inx & 0x7]);
               src      &=  ~data_buffer_32_mask_non_shuff[inx & 0x7];  
            }
   
            if (src == expected)
               continue;

            // test failed
            // check if all DQ has failed at least once - if so, testing is over
            cum_err_t = (src ^ expected);
#if PHY_WIDTH == 32
            cum_err |= fdq_binary (ps, cum_err_t);
#else
            cum_err |= fdq_binary_16 ((unsigned)ps, cum_err_t);
#endif
            
            if (cum_err !=0)
            {
               if (error_mask == (error_mask & cum_err))
               {
                  cum_err = MT_ALL_FAILED;
                  test_over = 1;
                  break;
               }
            }
         }
         // done with one chunk, flush D cache
         flush_data_cache (ps_flush, MAX_BYTES_PER_CHUNK, 0);
         if (test_over)
            break;
      }
   }
   EXIT("mips_test_memory");
   return cum_err;
}

#if RDI_USE_MCP

extern unsigned host_get_elapsed_msec(void);
extern void generate_random_seed(unsigned int *);
// dual read eye test memory loop
// returns the combined (AND'ed) P and N results
// note this function modifies the results_array
static inline unsigned mips_test_memory_dual_eye (MEMC_INFO *this_memc,
                                                  unsigned source,
                                                  unsigned size,
                                                  tinymt32_t *ptinymt,
                                                  unsigned vref,
                                                  unsigned vdl,
                                                  OverrideInfo_t * pOverride)
{
#define RDBUF_ISIZE INTS_PER_CHUNK
#define RDBUF_BSIZE BYTES_PER_CHUNK

   unsigned inx, mtest_result = 0;
   unsigned src; 

   unsigned chunk_count = size / MAX_INT_PER_CHUNK;
   unsigned chunk;
//   unsigned ps_flush;  /// pointer for flushing (points to the top of each chunk)
   unsigned ps;        /// pointer for memcpy (points to the top of each chunk)
   unsigned *pt;       /// pointer for testing memory (points to readbuf)

   unsigned cum_err[SIZE_NUM_EYES] = {0, 0};
   unsigned dq_err[SIZE_NUM_EYES];
   unsigned cum_err_t = 0;
   unsigned test_over = 0;
   unsigned int expected;
//   unsigned int shuffled;
   unsigned readbuf [RDBUF_ISIZE];

//   unsigned int * data_buffer_32_mask_shuff;
//   unsigned int * data_buffer_32_mask_non_shuff;
//   unsigned int * data_buffer_32_shuff_override_val;
//   unsigned int * data_buffer_32_non_shuff_override_val;
   unsigned int error_mask;

   // optimal VREF related
   unsigned result_marker;
   unsigned start_ms = 0;
   unsigned stop_ms;

   ps = source;
   
   error_mask = ~pOverride->uDqOverride;
   
   // override off
   if (pOverride->uDqOverride == 0xFFFFFFFF)   {
	 start_ms = host_get_elapsed_msec ();
	 for (chunk = 0; chunk < chunk_count; chunk ++)      {
	   /// debug only
	   //// host_printf ("testing chunk %08x  isize %08x, bsize %08x\n", ps, MAX_INT_PER_CHUNK, BYTES_PER_CHUNK );
	   // copy one chunk of data
	   memcpy ( readbuf, (void *)ps, RDBUF_BSIZE );
	   this_memc->total_bytes_read += RDBUF_BSIZE;

	   // readbuf has the entire chunk of data to test
	   // test the memory now
	   for (inx = 0, pt = &readbuf [0]; inx < MAX_INT_PER_CHUNK; inx ++ ) {
		 // Generate the expected data
		 expected = tinymt32_generate_uint32(ptinymt);
		 src = *pt++;
		 if ( src == expected) continue;
		 // test failed
		 // check if all DQ has failed at least once - if so, testing is over
		 cum_err_t = (src ^ expected);
#if MEMC_MODE == 32
		 fdq_binary_dual_cycle ((unsigned)((ps + inx) << 2), cum_err_t, dq_err, 1);
#else
		 fdq_binary_dual_cycle ((unsigned)((ps + inx) << 2), cum_err_t, dq_err, 0);
#endif
		 cum_err[0] |= dq_err[0];
		 cum_err[1] |= dq_err[1];
            
		 if (cum_err[0] == MT_ALL_FAILED && cum_err[1] == MT_ALL_FAILED) {
		   test_over = 1;
		   break;
		 }
	   }
	   // done with one chunk, flush D cache
	   // ps is still pointing to the top of this chunk
	   flush_data_cache (ps, MAX_BYTES_PER_CHUNK, 0);
	   // update ps for next chunk
	   ps += BYTES_PER_CHUNK;
	   if (test_over) break;
	 }
   }
   // overrride on
   else   {
	 //// 120912 CKD: NO SUPPORT FOR OVERRIDE DUAL EYE for now
   }
   /// Done with read test
   stop_ms = host_get_elapsed_msec ();
   this_memc-> total_time_in_ms += (stop_ms - start_ms);

   mtest_result = (cum_err[0] & cum_err[1]);
    
   if ( mtest_result == MT_ALL_FAILED )
      result_marker = 'f';
   else
   {
      if (mtest_result == 0) result_marker = '0';
      else result_marker = ' ';
   }
   if ((vref < this_memc->vref_top ) || (vref >this_memc->vref_bot ))
   {
      mtest_result = MT_ALL_FAILED;
      result_marker = 'r';
   }
   
   this_memc->send_char (result_marker);

   // For time lapse support, the test_result is OR'ed with the
   // record. In the case of a very bad eye to start with, then
   // no subsequent run can change 1 to 0. But in that case, the
   // eye will be bad for the entire run with multiple loops.
   this_memc->results_array[this_memc->memc_id][0][64*vref + vdl] |= cum_err[0];
   this_memc->results_array[this_memc->memc_id][1][64*vref + vdl] |= cum_err[1];
   
   return (mtest_result);
}

#else    //// use direct pointer
// dual read eye test memory loop
// returns the combined (AND'ed) P and N results
// note this function modifies the results_array
static inline unsigned mips_test_memory_dual_eye (MEMC_INFO *this_memc,
                                                  unsigned source,
                                                  unsigned size,
                                                  tinymt32_t *ptinymt,
                                                  unsigned vref,
                                                  unsigned vdl,
                                                  OverrideInfo_t * pOverride)
{
   unsigned inx, mtest_result = 0;
   unsigned src; 

   unsigned chunk_count = size / MAX_INT_PER_CHUNK;
   unsigned chunk;
   unsigned *ps,  ps_flush;
   unsigned cum_err[SIZE_NUM_EYES] = {0, 0};
   unsigned dq_err[SIZE_NUM_EYES];
   unsigned cum_err_t = 0;
   unsigned test_over = 0;
   unsigned int expected;
   unsigned int shuffled;
  
   unsigned int * data_buffer_32_mask_shuff;
   unsigned int * data_buffer_32_mask_non_shuff;
   unsigned int * data_buffer_32_shuff_override_val;
   unsigned int * data_buffer_32_non_shuff_override_val;
   unsigned int error_mask;

   // optimal VREF related
   unsigned result_marker;
   unsigned start_ms;
   unsigned stop_ms;
   
   ps = (unsigned*) source;
  
   error_mask = ~pOverride->uDqOverride;

   start_ms = host_get_elapsed_msec ();
   // override off
   if (pOverride->uDqOverride == 0xFFFFFFFF)
   {
	 
      for (chunk = 0; chunk < chunk_count; chunk ++)
      {
         // host_printf ("testing chunk %08x  size %08x\n", ps, MAX_INT_PER_CHUNK );
   
         ps_flush = (unsigned) ps;
   
         for (inx = 0; inx < MAX_INT_PER_CHUNK; inx ++, ps++ )
         {
            src = *ps;
            expected = tinymt32_generate_uint32(ptinymt);

// 7231 doesn't have L2, always go through the full error path
// to ensure fdq_binary_dual_cycle() is in cache.
// Need to think of a better way, as this impacts performance.
#if CHIP_HAS_L2
            if (src == expected) continue;
#endif
            // test failed
            // check if all DQ has failed at least once - if so, testing is over
            cum_err_t = (src ^ expected);
#if MEMC_MODE == 32
            fdq_binary_dual_cycle (ps, cum_err_t, dq_err, 1);
#else
            fdq_binary_dual_cycle (ps, cum_err_t, dq_err, 0);
#endif
            cum_err[0] |= dq_err[0];
            cum_err[1] |= dq_err[1];
            
            if (cum_err[0] == MT_ALL_FAILED && cum_err[1] == MT_ALL_FAILED)
            {
               test_over = 1;
               break;
            }
         }
         // done with one chunk, flush D cache
         flush_data_cache (ps_flush, MAX_BYTES_PER_CHUNK, 0);
		 this_memc->total_bytes_read += MAX_BYTES_PER_CHUNK;
         if (test_over)
            break;
      }
   }
   // overrride on
   else
   {
	 //// 120912 CKD: NO SUPPORT FOR OVERRIDE DUAL EYE for now
   }
   stop_ms = host_get_elapsed_msec ();
   this_memc-> total_time_in_ms += (stop_ms - start_ms);

   mtest_result = (cum_err[0] & cum_err[1]);
    
   if ( mtest_result == MT_ALL_FAILED )
      result_marker = 'f';
   else
   {
      if (mtest_result == 0) result_marker = '0';
      else result_marker = ' ';
   }
   if ((vref < this_memc->vref_top ) || (vref >this_memc->vref_bot ))
   {
      mtest_result = MT_ALL_FAILED;
      result_marker = 'r';
   }
   
   this_memc->send_char (result_marker);

   // For time lapse support, the test_result is OR'ed with the
   // record. In the case of a very bad eye to start with, then
   // no subsequent run can change 1 to 0. But in that case, the
   // eye will be bad for the entire run with multiple loops.
   this_memc->results_array[this_memc->memc_id][0][64*vref + vdl] |= cum_err[0];
   this_memc->results_array[this_memc->memc_id][1][64*vref + vdl] |= cum_err[1];
         
   return (mtest_result);
}
#endif


static inline void one_shmoo_addr_rd_eye_mips ( MEMC_INFO *this_memc, 
                                                unsigned start_vdl,
                                                unsigned start_vref,
                                                unsigned clear_result,
                                                unsigned do_vref,
                                                unsigned seed,
                                                OverrideInfo_t * pOverride)
{
   unsigned memc_base, phy_base, mt_start, mt_size, wl;
   unsigned vdl, vref;
   unsigned dac1_setting = 32;
   unsigned test_result;
   unsigned /*reg,*/ val, bits, vref_reg_base;
   unsigned p_wl0_byte0_bit_r_p;

   unsigned p_wl0_byte0_bit_r_n;
   unsigned p_wl0_byte1_bit_r_p;
   unsigned p_wl0_byte1_bit_r_n;

   unsigned p_wl1_byte0_bit_r_p;
   unsigned p_wl1_byte0_bit_r_n;
   unsigned p_wl1_byte1_bit_r_p;
   unsigned p_wl1_byte1_bit_r_n;
   unsigned mt_source, mt_dest, mt_test_size;//, *ps, *pd, data;
   unsigned /*inx, num,*/ end_vref = SIZE_VREF;
  // optimal VREF related
  unsigned result_marker;

   tinymt32_t tinymt;
//   char critical_line1 [DIAG_SIZE_16];
//   char critical_line2 [DIAG_SIZE_16];
//   char critical_line3 [DIAG_SIZE_16];



   ENTER("one_shmoo_addr_rd_eye_mips");
   memc_base = this_memc->memc_base;
   phy_base  = this_memc->phy_base;
   mt_start  = this_memc->sdram_top;
   mt_size   = this_memc->sdram_size;
   wl = this_memc->data_width;
   host_printf ("\nRDI mode = %d, WL = %d, do_vref = %d\n", this_memc->mode, wl, this_memc->do_vref );
   mt_source = mt_start;
   mt_dest   = mt_start + (mt_size/ 2);

   mt_test_size = (mt_size) / 4; /// full size, in unit of INTS
   // BW calculation support
   this_memc->total_bytes_read = 0;
   this_memc->total_time_in_ms = 0;

   this_memc->printf ("\nOptimal VREF setting = (%d:%d)\n", this_memc->vref_top, this_memc->vref_bot);
   this_memc->printf ("\nShowing RAW read-eye result\n");
   this_memc->printf ("\t'r' = optimal VREF in effect - no memory test\n");
   this_memc->printf ("\t'f' = All DQ lines showed failure\n");
   this_memc->printf ("\t'0' = All DQ lines passed with no error\n");
   this_memc->printf ("\t' ' = Some DQ lines passed with no error\n\n");

   for (vdl = start_vdl; vdl < SIZE_VDL; vdl ++ )
   {
      this_memc->printf ("VDL %02d ", vdl);

      p_wl0_byte0_bit_r_p = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P;
      p_wl0_byte0_bit_r_n = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_N;
      p_wl0_byte1_bit_r_p = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_P;
      p_wl0_byte1_bit_r_n = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_N;
      if (wl == 32) {
         p_wl1_byte0_bit_r_p = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P + WL_OFFSET;
         p_wl1_byte0_bit_r_n = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_N + WL_OFFSET;
         p_wl1_byte1_bit_r_p = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_P + WL_OFFSET;
         p_wl1_byte1_bit_r_n = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_N + WL_OFFSET;
      }

      val = 0x30000 | vdl;
   
      for (bits = 0; bits < 8; bits ++ )
      {
         this_memc->reg_writ (p_wl0_byte0_bit_r_p, val);
		 host_wait_not_busy (p_wl0_byte0_bit_r_p);

         this_memc->reg_writ (p_wl0_byte0_bit_r_n, val);
         host_wait_not_busy (p_wl0_byte0_bit_r_n);

         this_memc->reg_writ (p_wl0_byte1_bit_r_p, val);
         host_wait_not_busy (p_wl0_byte1_bit_r_p);

         this_memc->reg_writ (p_wl0_byte1_bit_r_n, val);
         host_wait_not_busy (p_wl0_byte1_bit_r_n);
         /////
         // update pointers 
         p_wl0_byte0_bit_r_p += 8;
         p_wl0_byte0_bit_r_n += 8;
         p_wl0_byte1_bit_r_p += 8;
         p_wl0_byte1_bit_r_n += 8;
         if (wl == 32)
         {
            this_memc->reg_writ (p_wl1_byte0_bit_r_p, val);
            this_memc->reg_writ (p_wl1_byte0_bit_r_n, val);
            this_memc->reg_writ (p_wl1_byte1_bit_r_p, val);
            this_memc->reg_writ (p_wl1_byte1_bit_r_n, val);
            p_wl1_byte0_bit_r_p += 8;
            p_wl1_byte0_bit_r_n += 8;
            p_wl1_byte1_bit_r_p += 8;
            p_wl1_byte1_bit_r_n += 8;
         }
      }

      timeout_ns (0x2000);

      for (vref = start_vref; vref < end_vref; vref ++ )
      {
         // DDR40_PHY_CONTROL_REGS_1.VREF_DAC_CONTROL = lshift(dac1_setting, 6) OR vref
         val = (dac1_setting << 6) | vref;
         vref_reg_base = memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0;
         this_memc->reg_writ ( vref_reg_base, val);

         timeout_ns (0x2000);

         //    results_array [64*vref + vdl] = dis_per_bit_test_mem_both_mips ( this_memc);

         tinymt.mat1 = 0; tinymt.mat2 = 0; tinymt.tmat = 0;
         tinymt32_init(&tinymt, seed);
     
         //     if ( (test_result = mips_test_memory (mt_source, mt_dest, mt_test_size, 0)) == MT_ALL_FAILED )
         //     if ( (test_result = mips_test_memory (mt_source, mt_dest, mt_test_size, seed)) == MT_ALL_FAILED )

         if (this_memc->mode == 0)
         {
            if ( (test_result = mips_test_memory (mt_source, mt_test_size, &tinymt, seed, pOverride)) == MT_ALL_FAILED )
               result_marker = 'f';
            else
            {
               if (test_result == 0)
                  result_marker = '0';
               else
                  result_marker = ' ';
            }
            if ((vref < this_memc->vref_top ) || (vref >this_memc->vref_bot ))
            {
               test_result = MT_ALL_FAILED;
               result_marker = 'r';
            }
            
            this_memc->send_char (result_marker);

            // For time lapse support, the test_result is OR'ed with the
            // record. In the case of a very bad eye to start with, then
            // no subsequent run can change 1 to 0. But in that case, the
            // eye will be bad for the entire run with multiple loops.
            this_memc->results_array[this_memc->memc_id][0][64*vref + vdl] |=
               test_result;
         }
         else
            test_result = mips_test_memory_dual_eye (this_memc,
                                                     mt_source,
                                                     mt_test_size,
                                                     &tinymt,
                                                     vref,
                                                     vdl,
                                                     pOverride);
         
         if (this_memc->do_vref == 0) break; // to speed up
      }
      this_memc->printf ("\n");
   }
   EXIT("one_shmoo_addr_rd_eye_mips");
}


////

//unsigned results_array[SIZE_RESULT_ARRAY];

//   all_shmoo_rd_eye_DIS_memc1(write_data_array)
void all_shmoo_rd_eye_mips (MEMC_INFO* this_memc,
            
                unsigned start_vdl,
                            unsigned start_vref,
                            unsigned clear_result,
                            unsigned do_vref,
                            OverrideInfo_t * pOverride)
{
   unsigned memc_base, phy_base, mt_start, mt_size;//, wl, phy = 0;
   unsigned init_setting [SIZE_INIT_SETTING];
   unsigned mt_source,  mt_test_size, /**ps, *pd, data,*/ seed;
   tinymt32_t tinymt;  
   int inx, iny;
   unsigned org_dac, reg_addr;//, val;
   unsigned org_vdl_byte_bit [VDL_BYTE_BIT_REGS_COUNT];
  
   unsigned int data_buffer_32_mask_non_shuff[ 8 ];
   unsigned int data_buffer_32_mask_shuff    [ 8 ];
   unsigned int data_buffer_32_non_shuff_override_val  [ 8 ];
   unsigned int data_buffer_32_shuff_override_val  [ 8 ];
   unsigned net_time;
   unsigned net_mb;

   ENTER("all_shmoo_rd_eye_mips");
   if (clear_result)
   {  
      for (iny = 0; iny < SIZE_NUM_EYES; iny++)
      {
         for (inx = 0; inx < SIZE_RESULT_ARRAY; inx ++ )
         {
            this_memc->results_array[this_memc->memc_id][iny][inx] = 0;
         }
      }
   }
  
   for (inx = 0; inx < SIZE_INIT_SETTING; inx ++ )
      init_setting [inx] = 0;

   /// prepare the memory block first
   memc_base = this_memc->memc_base;
   phy_base  = this_memc->phy_base;
   mt_start  = this_memc->sdram_top;
   mt_size   = this_memc->sdram_size;
  
   mt_source = mt_start;
   mt_test_size = (mt_size) / 4; /// half of the size, in unit of INTS


   /*
	 if clear_result == 1, then this is the first real run
	*/
   if (clear_result == 1) {
	 this_memc->seed = 0;
   } 
   generate_random_seed ( &this_memc->seed );

   seed = this_memc->seed;
   host_printf ("\n\tCurrent random seed = %08x\n", seed);

   if (this_memc->print_info)
   {
      this_memc->printf ("MIPS method to test memory from %08x size %08x\n", mt_start, mt_size);
      this_memc->printf ("mt_source %08x mt_test_size %08x\n", mt_source,  mt_test_size );
   }
   //  save_vdl_settings (&org_dac, org_vdl_byte_bit);
#if 0
   reg_addr = memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0 + RBUS_BASE;
   val  = *(unsigned volatile*)reg_addr;
   org_dac  = val;
#else
   org_dac = host_reg_read (memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0);
#endif
   //  this_memc->printf ("DAC_CONTROL @%08x = %08x\n", reg_addr, val);

   for (inx = 0, reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P;
        inx < 16; inx ++, reg_addr += 4 )
   {
	 //      val = *(unsigned volatile*)reg_addr;
	 // this_memc->printf ("VDL_OVRIDE WL0 %08x saved as %08x (%d)\n", reg_addr, val, inx);
	 org_vdl_byte_bit [inx] = host_reg_read ( reg_addr);
   }

   // inx = 16
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_P;
         inx < 32; inx ++, reg_addr += 4 )
   {
	 //      val = *(unsigned volatile*)reg_addr;
	 // this_memc->printf ("VDL_OVRIDE WL0 %08x saved as %08x (%d)\n", reg_addr, val, inx);
      org_vdl_byte_bit [inx] = host_reg_read ( reg_addr);
   }

   if ( this_memc->data_width == 32) {
	 for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P  + WL_OFFSET;
		   inx < 48; inx ++, reg_addr += 4 )   {
	   //	   this_memc->printf ("VDL_OVRIDE WL1 %08x saved as %08x (%d)\n", reg_addr, val, inx);
	   org_vdl_byte_bit [inx] = host_reg_read ( reg_addr);
	 }

	 for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_P + WL_OFFSET;
		   inx < 64; inx ++, reg_addr += 4 )   {
	   //	   this_memc->printf ("VDL_OVRIDE WL1 %08x saved as %08x (%d)\n", reg_addr, val, inx);
	   org_vdl_byte_bit [inx] = host_reg_read ( reg_addr);
	 }
   }
  
   pOverride->pMaskNonShuff               =   &data_buffer_32_mask_non_shuff[0];
   pOverride->pMaskShuff                  =   &data_buffer_32_mask_shuff[0];
   pOverride->pMaskShuffOverrideValue     =   &data_buffer_32_shuff_override_val[0];
   pOverride->pMaskNonShuffOverrideValue  =   &data_buffer_32_non_shuff_override_val[0];
   
   generate_dq_masks((unsigned char *) pOverride->pMaskNonShuff, (unsigned char *)pOverride->pMaskShuff, 
                     (unsigned char *)pOverride->pMaskNonShuffOverrideValue, (unsigned char *)pOverride->pMaskShuffOverrideValue,
                     pOverride->uDqOverride, pOverride->uDqOverrideVal);

   if (this_memc->print_info)
      this_memc->printf ("\nTINY initialized with seed = %08x\n", seed);
   tinymt.mat1 = 0; tinymt.mat2 = 0; tinymt.tmat = 0;
   tinymt32_init(&tinymt, seed);

   if (this_memc->print_info)
      this_memc->printf ("Pre-fill memory region @%08x size %08x bytes random seed %08x\n", 
                         mt_source, mt_size, seed );
   srdi_fill_memory (mt_source, (mt_size ), seed, pOverride );

   one_shmoo_addr_rd_eye_mips (this_memc, start_vdl, start_vref, clear_result, do_vref, seed, pOverride);
   //  restore_vdl_settings (&org_dac, org_vdl_byte_bit);
   reg_addr = memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0;
   //   *(unsigned volatile*)reg_addr = org_dac;
   host_reg_write( reg_addr, org_dac);
   //  this_memc->printf ("DAC_CONTROL @%08x restored to %08x\n", reg_addr, org_dac);

   for (inx = 0, reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P;
        inx < 16; inx ++, reg_addr += 4 )
   {
	 //    *(unsigned volatile*)reg_addr = (org_vdl_byte_bit [inx] | (1<<17)); // set OVR_FORCE bit to write back
	 host_reg_write( reg_addr, (org_vdl_byte_bit [inx] | (1<<17))); // set OVR_FORCE bit to write back
	 // this_memc->printf ("VDL_OVRIDE WL0 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
   }
   // inx = 16
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_P;
         inx < 32; inx ++, reg_addr += 4 )
   {
	 //      *(unsigned volatile*)reg_addr = (org_vdl_byte_bit [inx] | (1<<17));
	 host_reg_write( reg_addr, (org_vdl_byte_bit [inx] | (1<<17)));
	 // this_memc->printf ("VDL_OVRIDE WL0 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
   }
   if ( this_memc->data_width == 32 ) {
	 for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P + WL_OFFSET;
		   inx < 48; inx ++, reg_addr += 4 )   {
	   host_reg_write( reg_addr , (org_vdl_byte_bit [inx] | (1<<17)));
	   //	   this_memc->printf ("VDL_OVRIDE WL1 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
	 }
	 // inx = 16
	 for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_P + WL_OFFSET;
         inx < 64; inx ++, reg_addr += 4 )   {
	   host_reg_write( reg_addr, (org_vdl_byte_bit [inx] | (1<<17)));
	   //	   this_memc->printf ("VDL_OVRIDE WL1 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
	 }
   }
#ifdef NORTHSTAR
#if 0 // no forcing
   if (do_vref == 0) {
	 host_printf ("\nNS dry run - force to plot\n");
	 this_memc->ok_to_plot = 1;
   }
#endif
#endif

   prt_rd_eye_shmoo_result_166(this_memc,
                               org_vdl_byte_bit,
                               this_memc->memc_id,
                               this_memc->ok_to_plot,
                               pOverride->uDqOverride);

   net_time = this_memc-> total_time_in_ms/1000;
   net_mb   = this_memc -> total_bytes_read/ONE_MB;
   host_printf ("\n** %dMB read in %d sec ", net_mb, net_time);

   if ( net_time > 0 ) {
	 host_printf ("BW = %dMB/s **\n", (net_mb / net_time));
   } else {
	 host_printf ("\n");
   }
   //  optimal_prt_rd_eye_shmoo_result (results_array, init_setting, phy, do_vref);

   EXIT("all_shmoo_rd_eye_mips");
}


/////

unsigned print_calib_steps (unsigned memc_base, unsigned do_vref)
{
  unsigned ticks_per_clock = 0;
  unsigned phy_reg = 0, val;
  
  ENTER("print_calib_steps");
  //ticks_per_clock = DDR40_PHY_CONTROL_REGS_0.VDL_CALIB_STATUS.calib_total
  // prints out: Ticks per clock cycle = 132
  // 
  val = host_reg_read (memc_base + PHY_BASE_TYPE_0 + BCHP_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUS) ;
  //  phy_reg = (memc_base + PHY_BASE_TYPE_0 + BCHP_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUS) ;
  //val = *(unsigned*)phy_reg;
  if (do_vref) host_printf ("read VDL_CALIB_STATUS register at %08x (val = %08x)\n", phy_reg, val);
  ticks_per_clock = GET_FIELD(val, BCHP_DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, calib_total);
  //  ticks_per_clock = LOCAL_GET_FIELD(val, CALIB_TOTAL_MASK, CALIB_TOTAL_SHIFT);
  EXIT("print_calib_steps");
  return ticks_per_clock;
}

void do_mips_rd_eye_shmoo (MEMC_INFO *this_memc,
                           unsigned start_vdl,
                           unsigned start_vref,
                           unsigned clear_result,
                           unsigned do_vref,
                           OverrideInfo_t *  pOverride)
{
   unsigned memc_base, phy_base, mt_start, mt_size, wl;
   unsigned vdl_status_ticks_per_clock;
  
   ENTER("do_mips_rd_eye_shmoo");
   memc_base = this_memc->memc_base;
   phy_base  = this_memc->phy_base;
   mt_start  = this_memc->sdram_top;
   mt_size   = this_memc->sdram_size;
   wl = this_memc->data_width;

   vdl_status_ticks_per_clock = print_calib_steps (memc_base, this_memc->print_info );
   // enforce caching

   no_reg_writ (0, 0);
   no_send_uart (0);
   no_printf ("");

   if (do_vref)
   {
      this_memc->reg_writ = host_reg_write;
      if (this_memc->print_info)
      {
         this_memc->printf ("\nRD eye plot version: %d.%d Date: %08x\n\n",
                            RDI_VER_MAJ, RDI_VER_MIN, RDI_DATE_CODE);
         this_memc->printf ("MIPS to test memory start vdl %d start vref\n", start_vdl, start_vref);
         //  tinymt32_init(&tinymt, seed);
         this_memc->printf ("\nTicks per clock cycle = %d\n", vdl_status_ticks_per_clock);
         this_memc->printf ("do_mips_rd_eye_shmoo optimal result (%d %d)\n", this_memc->vref_top, this_memc->vref_bot );
      }
   } else
   {
      this_memc->printf ("\n\nPre-read-eye set up\nPlease wait...\n\n");
      this_memc->reg_writ = no_reg_writ;
   }

   all_shmoo_rd_eye_mips (this_memc, start_vdl, start_vref, clear_result, do_vref, pOverride);
   EXIT("do_mips_rd_eye_shmoo");
}

////
unsigned set_vref_test_memory ( MEMC_INFO *cfg )
{
   unsigned dac1_setting = 32;
   unsigned original_vref;
   unsigned vref_now = cfg->vref_start;
   unsigned test_result;
   unsigned mt_source;
   unsigned mt_dest;
   unsigned mt_test_size;
//   unsigned *ps, *pd;
   unsigned val;
   tinymt32_t tinymt;
   OverrideInfo_t sOverrride;
  
   ENTER("set_vref_test_memory");
   sOverrride.uDqOverride = 0xFFFFFFFF; // don't override

   mt_source    = cfg->sdram_top;
   mt_dest      = cfg->sdram_top + (cfg->sdram_size/ 2);
   mt_test_size = (cfg->sdram_size)/4; // full size need size in INTS

   original_vref = host_reg_read ( cfg->memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0);

   // SVTM_DBGP ("test memory region 1 @%08x region 2 @%08x size %08x bytes\n", mt_source, mt_dest, mt_test_size );
   // SVTM_DBGP ("Original VREF = %08x\n", original_vref);

   while (1)
   {
      // set vref
      // SVTM_DBGP ("VREF %d\n", vref_now );
      val = (dac1_setting << 6) | vref_now;
      cfg->reg_writ ( ( cfg->memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0), val);
      timeout_ns (0x2000);
      // test memory

      tinymt.mat1 = 0; tinymt.mat2 = 0; tinymt.tmat = 0;
      tinymt32_init(&tinymt, cfg->seed);
      test_result =  mips_test_memory (mt_source, mt_test_size, &tinymt, cfg->seed, &sOverrride);

      if (cfg->exit_condition == RUN_ONCE_AND_EXIT)
      {
         //    SVTM_DBGP ("run once and exit now\n");
         break;
      }

      if ( test_result == MT_ALL_FAILED )
      {
         if ( cfg->exit_condition == ALL_FAILED_THEN_EXIT)
         {
            //    SVTM_DBGP ("ALL_FAILED_THEN_EXIT now\n");
            break;
         }
      }
      else
      { // memory test has some passing
         if (cfg->exit_condition == SOME_PASSED_THEN_EXIT)
         {
            SVTM_DBGP ("SOME_PASSED_THEN_EXIT now\n");
            break;
         }
      }
      // exit condition
      if (vref_now == cfg->vref_end)
      {
         SVTM_DBGP ("set_vref_test_memory exit due to VREF reaching limit (%d)\n", vref_now );
         SVTM_DBGP ("Cannot determine - return default VREF %d\n", vref_now );
         break;
      }
      // set up for next ref
      vref_now += cfg->vref_incr; // vref_incr can be 1, or -1
   }
   /// restore original vref (the entire register)
   cfg->reg_writ ( ( cfg->memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0), original_vref );

   /// if expected result is not found, return default for extreme case
   EXIT("set_vref_test_memory");
   return vref_now;
}

unsigned get_optimal_vref_settings (MEMC_INFO *test_cfg)
{
   unsigned svtm_result;
   
   unsigned top = 0;
   unsigned bot = 63;
   unsigned old_do_vref =  test_cfg->do_vref ;
#ifdef VREF_OPTIMAL_MARGIN
   unsigned optimal_margin = VREF_OPTIMAL_MARGIN;
#else
   unsigned optimal_margin = DEFAULT_OPTIMAL_MARGIN;
#endif
   ENTER("get_optimal_vref_settings");
   //  SVTM_DBGP ("\nget_optimal_vref_setting\n\n");

   test_cfg->vref_start     = VREF_MID_POINT;
   test_cfg->exit_condition = RUN_ONCE_AND_EXIT;
   test_cfg->vref_incr      = DECREMENT_VREF; // This is a dont-care
   test_cfg->do_vref        = 0; 


   set_vref_test_memory(test_cfg);


   test_cfg->do_vref        = 1; 
   //  show_vrfmt (test_cfg);
  
   svtm_result = set_vref_test_memory(test_cfg);

   if (svtm_result == MT_ALL_FAILED)
   {
      /// The top can be either above or below the mid point
      host_printf ("The top can be either above or below the mid poin\n");
      test_cfg->vref_start     = VREF_MID_POINT;
      test_cfg->vref_end       = VREF_LOW_LIMIT; 
      test_cfg->vref_incr      = DECREMENT_VREF;
      test_cfg->exit_condition = SOME_PASSED_THEN_EXIT;

      svtm_result = set_vref_test_memory(test_cfg);
      /// if the result was not at 0, then we have found BOT
      if (svtm_result != 0)
         bot = svtm_result;
      else
      {
         /// The last result was at 0, the top is below midpoint
         test_cfg->vref_start     = VREF_MID_POINT;
         test_cfg->vref_end       = VREF_HIGH_LIMIT; 
         test_cfg->vref_incr      = INCREMENT_VREF;
         test_cfg->exit_condition = SOME_PASSED_THEN_EXIT;
         svtm_result = set_vref_test_memory(test_cfg);
         if (svtm_result != VREF_HIGH_LIMIT)
         {
            // we have found the top
            top = svtm_result;
            // proceed to find the bot
         } 
         // otherwise, there is no optimal rectange here - sweep the entire rectangle
         // use the default (0, 63) and we are done
      }
   }
   else
   {
      /// The top is to the north of mid point
      test_cfg->vref_start     = VREF_MID_POINT;
      test_cfg->vref_end       = VREF_LOW_LIMIT; 
      test_cfg->vref_incr      = DECREMENT_VREF;
      test_cfg->exit_condition = ALL_FAILED_THEN_EXIT;
      top = set_vref_test_memory(test_cfg);
      // SVTM_DBGP ("\tvref_top = %02x (%02d)\n", test_cfg->vref_top, test_cfg->vref_top );

      test_cfg->vref_start     = VREF_MID_POINT;
      test_cfg->vref_end       = VREF_HIGH_LIMIT; 
      test_cfg->vref_incr      = INCREMENT_VREF;
      test_cfg->exit_condition = ALL_FAILED_THEN_EXIT;
      bot = set_vref_test_memory(test_cfg);
      // SVTM_DBGP ("\tvref_bot = %02x (%02d)\n", test_cfg->vref_bot, test_cfg->vref_bot );
   }
   // increase the margin on both side to avoid cropping

   if (top  > optimal_margin )   {
	 // host_printf ("get_optimal_vref: top %d extended to ", top);
	 top -= optimal_margin;
	 // host_printf ("%d\n", top);
   }
   if (bot  < (VREF_HIGH_LIMIT - optimal_margin ))    {
	 // host_printf ("get_optimal_vref: bot %d extended to ", bot);
	 bot += optimal_margin;
	 // host_printf ("%d\n", bot);
   }
   test_cfg->vref_top = top;
   test_cfg->vref_bot = bot;
   test_cfg->do_vref  = old_do_vref;


   EXIT("get_optimal_vref_settings");
   return 1;
}

unsigned mips_rd_eye_plot (char *line)
{
   char name [DIAG_SIZE_32];
   unsigned /*seed,*/ memc_base, mt_start, mt_size, wl, dis_start, dis_size;
   unsigned /*time_start, time_done,*/ start_vref, start_vdl, plot_flag, clear_result, do_vref, sta_vref, octal_eye_memc_id;
   MEMC_INFO this_memc_info;

   int tokens;
  
   unsigned int uDqOverride;
   OverrideInfo_t sOverride;
   uint32_t mode;
  
   ENTER("mips_rd_eye_plot ");
   tokens = mdeCountTokens(line);
  
   // rdi <seed>
  
   // if override and override val are not provided disable mask
   if (tokens >= 16)
   {
      mdeLineScanf (line, "%s%d%x%x%x%x%x%d%d%d%d%d%d%d%d%x%x",
                name, &mode, &memc_base, &dis_start, &dis_size,
                &mt_start, &mt_size, &wl, &start_vdl, &start_vref,
                &plot_flag, &clear_result, &do_vref, &sta_vref,
                &octal_eye_memc_id, &uDqOverride, &sOverride.uDqOverrideVal);
   }
   else
   {
      mdeLineScanf (line, "%s%d%x%x%x%x%x%d%d%d%d%d%d%d%d",
                name, &mode, &memc_base, &dis_start, &dis_size,
                &mt_start, &mt_size, &wl, &start_vdl, &start_vref,
                &plot_flag, &clear_result, &do_vref, &sta_vref, &octal_eye_memc_id);
         
      uDqOverride = DQ_OVERRIDE_OFF;
      sOverride.uDqOverrideVal =0;
   
   }

   host_printf ("mrdi (%d %08x %08x %08x %08x %08x %d %d %d %d %d %d %d %d %x %x)\n",
				mode, memc_base, dis_start, dis_size,
				mt_start, mt_size, wl, start_vdl, start_vref,
				plot_flag, clear_result, do_vref, sta_vref, octal_eye_memc_id,
				uDqOverride, sOverride.uDqOverrideVal );
   if (do_vref)
   {
      this_memc_info.reg_writ = host_reg_write;
      if (plot_flag)
      {
         host_printf ("mrdi (%d %08x %08x %08x %08x %08x %d %d %d %d %d %d %d %d %x %x)\n",
                     mode, memc_base, dis_start, dis_size,
                     mt_start, mt_size, wl, start_vdl, start_vref,
                     plot_flag, clear_result, do_vref, sta_vref,
                     octal_eye_memc_id, uDqOverride, sOverride.uDqOverrideVal );
         this_memc_info.ok_to_plot = 1;
         this_memc_info.print_info = 1;
         this_memc_info.send_char  = host_console_write;
		 this_memc_info.printf     = host_printf;
      }
      else
      {
         this_memc_info.ok_to_plot = 0;
         this_memc_info.print_info = 0;
         this_memc_info.send_char  = no_send_uart;
         this_memc_info.printf     = no_printf;
      }
   }
   else
   {
      this_memc_info.reg_writ = no_reg_writ;
      this_memc_info.ok_to_plot = 0;
      this_memc_info.print_info = 0;
      this_memc_info.send_char  = no_send_uart;
      this_memc_info.printf     = no_printf;
   }

   this_memc_info.do_vref = do_vref;

   // The first seed is for get_optimal_vref_setting
   // use current time
   this_memc_info.seed = 0;
   generate_random_seed ( &this_memc_info.seed );

   // 
   this_memc_info.memc_base  = memc_base;
   this_memc_info.phy_base   = memc_base + PHY_BASE_TYPE_0;
   this_memc_info.sdram_top  = mt_start;
   this_memc_info.sdram_size = mt_size;
   this_memc_info.dis_top    = dis_start;
   this_memc_info.dis_size   = dis_size;
   this_memc_info.data_width = wl;  
   this_memc_info.vref_top   = 0;
   this_memc_info.vref_bot   = 0;
   this_memc_info.mode       = mode;
   this_memc_info.memc_id    = MEMC0;
   this_memc_info.sta_vref   = sta_vref;

#ifndef BCHP_SID_BIGRAM_REG_START
   if ( mode == 2 )
   {
	 host_printf ("There is no SID_BIGRAM for this chip, cannot run mode 2\n");
	 return 0;
   }
#endif

#ifdef MEMC_1_DDR_ENABLE
   // determine which MEMC is being tested
   if ( memc_base !=  ATRI_MEMC0_BASE )
      this_memc_info.memc_id = MEMC1;
#endif

   sOverride.uDqOverride =  0xFFFFFFFF;
   // The first srdi_fill_memory is for get_optimal_vref_setting
   srdi_fill_memory (mt_start, mt_size,  this_memc_info.seed, &sOverride );

   sOverride.uDqOverride =  uDqOverride;

   get_optimal_vref_settings ( &this_memc_info );

   host_printf ("MEMC%d result array in use - clear flag = %d",
               this_memc_info.memc_id, clear_result );

   do_mips_rd_eye_shmoo ( &this_memc_info, start_vdl, start_vref, clear_result, do_vref, &sOverride);
   EXIT("mips_rd_eye_plot");
   return 0;
}

unsigned mde_rd_eye_plot (sMde_t *mde )
{
  
  return mips_rd_eye_plot ( mde->user_line );
}





int find_dq_inverse(unsigned int byte_addx, int DQ) {

//  unsigned int scb_addx;
  unsigned int lower_byte_addx;
  int shuffled, is_even, BL;
  int bit_pos;

  // swizzle to go from CPU to SCB byte ordering.
  byte_addx ^= 0x1C;

  // obtain lower 5 addx bits.  
  lower_byte_addx = byte_addx & 0x1F;

  // addx shuffled flag
  shuffled = ((byte_addx>>8)&1 ) ^ ((byte_addx>>7)&1);

  // pick Byte Lane the user supplied byte_addx lies in.
  is_even = (lower_byte_addx & 1) == 0;

  if ( lower_byte_addx >= 16 ) {
    if (is_even)   BL = 0;
    else           BL = 1;
  }
  else {
    if (is_even)   BL = 2;
    else           BL = 3;
  }

  if (shuffled) BL ^= 2;

  if ( (DQ>>3) == BL ) bit_pos = DQ&7;
  else bit_pos = -1;

  return bit_pos;

}


// PW change starts here


void generate_dq_masks(char *ptr_mask_non_shuff, char * ptr_mask_shuff, char * ptr_non_shuff_override_value, char * ptr_shuff_override_value, unsigned int dq_override, unsigned int dq_override_val)
{

  unsigned int DQ;
  unsigned int addx;
  int bitpos;
  
  
  // Zero masks
	for(addx=0;addx<32;addx++) { 
	   ptr_mask_non_shuff[addx]             = 0;   
		 ptr_mask_shuff[addx]                 = 0;    
		 ptr_shuff_override_value[addx]       = 0;  
		 ptr_non_shuff_override_value[addx]   = 0;
	}
	
	// generate Non Shuffled mask once. Mask is repetitive over 32Byte
	for(DQ=0;DQ<32;DQ++) {
	   if ( dq_override & (1<<DQ) ) {
	     for(addx=0;addx<32;addx++) {
	        bitpos = find_dq_inverse(addx, DQ ); 
	        if (bitpos >=0)
					{
					   ptr_mask_non_shuff[addx] |= (1<< bitpos);
					   ptr_non_shuff_override_value[addx] |=  ( ((dq_override_val>> DQ) & 1) << bitpos);
					}      
	     }
	   }
	}
	
	// generate Shuffled mask once. Mask is repetitive over 32Byte
	for(DQ=0;DQ<32;DQ++) {
	   if ( dq_override & (1<<DQ) ) {
	     for(addx=0;addx<32;addx++) {
	        bitpos = find_dq_inverse( ((1<<8) | addx), DQ ); 
	        if (bitpos >=0) 
					{
					   ptr_mask_shuff[addx] |= (1<< bitpos); 
						 ptr_shuff_override_value[addx] |=  ( ((dq_override_val>> DQ) &1) << bitpos);       
	        }
	     }
	   }
	}
	
	if (is_big_endian())
	{
	  //dbg_printf ("DQ bigendian\n");
	  for(addx=0;addx<32;addx+=4) { 
	    endian_swap(ptr_mask_non_shuff+addx);             
		  endian_swap(ptr_mask_shuff+addx);   
		  endian_swap(ptr_shuff_override_value+addx);  
		  endian_swap(ptr_non_shuff_override_value+addx);
	  }
	}
	
	#if 0
	// Dump masks
	for(addx=0;addx<32;addx++) { 
	   dbg_printf("** %d %x %x %x %x\n", addx,  ptr_mask_non_shuff[addx], ptr_mask_shuff[addx] , ptr_non_shuff_override_value[addx], ptr_shuff_override_value[addx]);   
	}
	#endif

}

// This is for BUSY bit in the VDL setting
//DDR-EYE-SHMOO: remove static
/*static*/ void  host_wait_not_busy ( unsigned offset ) {
#define BIT_32 0x80000000
#define TICK_LIMIT  0x1000000

  unsigned val;
  unsigned ticks = 0;

  while ( ticks++ < TICK_LIMIT ) {
	val = host_reg_read ( offset);
	if ((val & BIT_31) == 0) return;
  }
  host_printf ("\n***** FATAL error VDL busy bit stay high when timeout expired (register offset %08x)\n",
			   offset );
}

