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
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQENTIAL DAMAGES
 *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *     GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *     OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *     TORT (INCLUDING NEGLIGENCE OR OTHERWISE), EVEN IF ADVISED OF
 *     THE POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************
 *  Broadcom Memory Diagnostics Environment (MDE)
 *********************************************************************
 *  Filename: cde_wr_eye.c
 *
 *  Function: plot write eye shmoo
 *
 *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
 *
 */

//DDR-EYE-SHMOO: chang efrom cde to mde
#include "mde_common_headers.h"


#include "tinymt32.h"
#include "ddr40_phy_control_regs.h"
#include "cde_local_memc_ddr_0.h"
#include "cde_local_memc_gen_0.h"
#include "local_memc_arb_0.h"
#include "ddr40_phy_word_lane_0.h"
//DDR-EYE-SHMOO: comment out
//#include "bchp_uarta.h"
//DDR-EYE-SHMOO: change from cde_plat.h to mde_plat.h
#include "mde_plat.h"

//DDR-EYE-SHMOO: add these definition to avoid compiling errors and enable some printf
#define SIZE_NUM_DUAL_EYES 1
#define BCHP_CHIP 53012
#define MEMC0_DIS0_START 0
#define MEMC0_DIS0_END 1
#define MEMC0_DIS1_START 0
#define MEMC0_DIS1_END 1
#define MEMC1_DIS0_START 0
#define MEMC1_DIS0_END 1
#define MEMC1_DIS1_START 0
#define MEMC1_DIS1_END 1
#define RBUS_BASE NORTHSTAR_REG_BASE
#define dbg_printf printf
#define dbg_sprintf(format, arg...) do {} while (0)
#define cdeScanf mdeLineScanf
#define dis_test_trigger_memc(a) do {} while (0)
#define dis_stop_test_memc(a) do {} while (0)
#define dis_special_setup(a, b, c, d, e, f) do {} while (0)
#define send_uart putc

extern void host_wait_not_busy ( unsigned offset );
extern void flush_data_cache (unsigned addr, unsigned bytes, unsigned verbose);
extern void generate_random_seed( unsigned int *g_current_seed );
extern void mdeLineScanf ( char *line, char *ctl, ...);
extern unsigned current_time_in_seconds (void);
extern int fdq_binary_dual_cycle (unsigned int addx,
                             unsigned int logical_err_map,
                             unsigned int *dq_error,
                             int is_32b_phy );
extern void srdi_fill_memory (unsigned from, unsigned size_bytes, unsigned seed, OverrideInfo_t * pOverride);

void clear_system_time(void)
{
	//none
} 



#undef DBGP
#define DBGP(a) dbg_printf ("step %08x\n", a);
extern unsigned  mips_test_memory ( unsigned source, unsigned size, tinymt32_t *tinymt, unsigned seed, OverrideInfo_t * pOverride);

///////////

#define MAX_BYTES_PER_CHUNK 0x8000
#define MAX_INT_PER_CHUNK  (MAX_BYTES_PER_CHUNK/4)

#define BYTES_PER_CHUNK    (MAX_BYTES_PER_CHUNK)
#define INTS_PER_CHUNK     (MAX_BYTES_PER_CHUNK/4)

#define OUT4(a, b, c, d) send_uart(a);send_uart(b);send_uart(c);send_uart(d)
#define OUTCRLF() send_uart(0xa);send_uart(0xd)
#define OUTHEX(a) send_hex(a); send_uart (0x20)
#define OUTONE(a) send_uart(a)

#define checkit(addr) OUTHEX(addr); send_uart ('='); val=*(unsigned*)addr; OUTHEX(val); OUTCRLF()
#define outcode(code) OUTHEX(code); OUTCRLF()

//#define SDBGP dbg_printf
#define SDBGP(format, arg...) do {} while(0)
//#define DSVTWP dbg_printf 
#define DSVTWP(format, arg...) do {} while(0)
//#define DVWSP dbg_printf
#define DVWSP(format, arg...) do {} while(0)

#define BITS_IN_UNSIGNED 32
#define BITS_PER_NIBBLE  4
#define NIBBLE           0xF
#define NIBBLE_COUNT     (BITS_IN_UNSIGNED/BITS_PER_NIBBLE) // 8
#define MS_NIBBLE_POS    (BITS_IN_UNSIGNED-BITS_PER_NIBBLE)  // 28 : we need to print from MS bits to LS
#define A_F_BASE         ('A' - 10) // 0x37

// WRI related 
#if BCHP_CHIP == 7344
#define TAWRI_MEMC0_BASE     0x00A00000
#else
//DDR-EYE-SHMOO:
#define TAWRI_MEMC0_BASE     0x0
//#define TAWRI_MEMC0_BASE     0x003b0000
#endif
//DDR-EYE-SHMOO: change memory test base addr from 0xd0000000 to 0x01000000
//#define TAWRI_MEMC0_MT_TOP   0xd0000000
#define TAWRI_MEMC0_MT_TOP   0x01000000
#define TAWRI_MEMC0_MT_SIZE  0x00100000

#ifdef MEMC_1_DDR_ENABLE
#define TAWRI_MEMC1_BASE     0x003c0000
#define TAWRI_MEMC1_MT_TOP   0xf0000000
#define TAWRI_MEMC1_MT_SIZE  0x00400000
#endif

#define MEMC1_MT_BASE 0xF0000000

#define USE_CURRENT_TIME_FOR_RANDOM_SEED 0

#define W_VDL_BYTE_BIT_REGS_COUNT (16*2) // two WORD LANES, 16 register per word lane

//DDR-EYE-SHMOO: 16 bit only
#define WRI_ALL_FAILED 0xFFFF
//#define WRI_ALL_FAILED 0xFFFFFFFF
#define WRI_ALL_PASSED 0x00000000

//DDR-EYE-SHMOO: defined in mde_rd_eye.h, not used in this file
//#define VREF_LO_LIMIT 5
//#define VREF_HI_LIMIT 54


#define VDL_W_MASK         0xFF // 8 bit value
#define WR_EYE_INVALID     '-'
#define WR_EYE_VALID       '+'
#define SHMOO_PICKED_W_VDL 'X'
//DDR-EYE-SHMOO: 16 bit only
//#define BITS_TO_PLOT       32
#define BITS_TO_PLOT       16

typedef struct {
   // properties

   // MEMC and memory test information
   unsigned chip_id;
   unsigned memc_base;
   unsigned mt_addr;
   unsigned mt_size;
   unsigned seed;
   unsigned phy_id;
   unsigned dis0_start;
   unsigned dis0_end;
   unsigned dis1_start;
   unsigned dis1_end;

   // for accumulating test results
   unsigned clear_result;
   unsigned results_array    [PLAT_MAX_NUM_MEMC][SIZE_NUM_DUAL_EYES][SIZE_RESULT_ARRAY];

   // for saving and restoring original DAC and WR VDL
   unsigned org_dac;
   unsigned org_vdl_byte_bit [W_VDL_BYTE_BIT_REGS_COUNT];

   // for VREF and VDL control
   unsigned vref_start;
   unsigned vref_end;
   unsigned vdl_start;
   unsigned vdl_end;

   unsigned mode; // test mode 0=single eye, 1=dual eye

   // mask related
   OverrideInfo_t sOverride;

   // for printing result
   unsigned time_interval_to_print;
   unsigned print_info;
   unsigned plot_flag;

   // methods
   void (*send_char)  ( const char c);
   int  (*printf )    ( const char *text, ...);
} WRI_INFO;

//DDR-EYE-SHMOO: comment out
//extern void send_uart  (unsigned val);

void send_char_to_bucket (const char c)
{
   // This is a NOP
}

int send_text_to_bucket ( const char *text, ...)
{
   // This is a NOP
   return 0;
}


// Helper function: first time setup of WRI_INFO structure
WRI_INFO * wri_setup_new ( WRI_INFO *wri_info,
                           unsigned phy_id,
                           unsigned mode,
                           unsigned memc_base,
                           unsigned mt_addr,
                           unsigned mt_size,
                           unsigned seed )
{
   wri_info->chip_id        = BCHP_CHIP;
   wri_info->mode           = mode;
   wri_info->phy_id         = phy_id;
   wri_info->memc_base      = memc_base;
   wri_info->mt_addr        = mt_addr;
   wri_info->mt_size        = mt_size;
   wri_info->seed           = seed;
   wri_info->clear_result   = 1;
   wri_info->plot_flag      = 1;
   wri_info->send_char      = send_uart;
   wri_info->printf         = dbg_printf;
   wri_info->sOverride.uDqOverride = DQ_OVERRIDE_OFF;
   wri_info->sOverride.uDqOverrideVal = 0;
   if (phy_id == MEMC0)
   {
      wri_info->dis0_start  = MEMC0_DIS0_START;
      wri_info->dis0_end    = MEMC0_DIS0_END;
      wri_info->dis1_start  = MEMC0_DIS1_START;
      wri_info->dis1_end    = MEMC0_DIS1_END;
   }
   else
   {
      wri_info->dis0_start  = MEMC1_DIS0_START;
      wri_info->dis0_end    = MEMC1_DIS0_END;
      wri_info->dis1_start  = MEMC1_DIS1_START;
      wri_info->dis1_end    = MEMC1_DIS1_END;
   }
   
   return wri_info;
}

void wri_show_info (WRI_INFO *local_info)
{
   local_info->printf ("WRI_INFO at %08x (size %d)\n", local_info, sizeof(WRI_INFO) );
   local_info->printf ("    chip_id    = %08x\n", local_info -> chip_id );
   local_info->printf ("    memc_base  = %08x\n", local_info -> memc_base );
   local_info->printf ("    mt_size    = %08x\n", local_info -> mt_size   );
   local_info->printf ("    seed       = %08x\n", local_info -> seed   );
   local_info->printf ("    phy_id     = %08x\n", local_info -> phy_id );
   local_info->printf ("    dis0_start = %08x\n", local_info -> dis0_start );
   local_info->printf ("    dis0_end   = %08x\n", local_info -> dis0_end );
   local_info->printf ("    dis1_start = %08x\n", local_info -> dis1_start );
   local_info->printf ("    dis1_end   = %08x\n", local_info -> dis1_end );
}

void save_vdl_wr_settings (unsigned memc_base, unsigned *p_org_dac, unsigned org_vdl_byte_bit [])
{
   unsigned reg_addr;
   unsigned val;
   int inx;

   reg_addr = memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0 + RBUS_BASE;
   val  = *(unsigned volatile*)reg_addr;
   *p_org_dac  = val;

   SDBGP ("DAC_CONTROL @%08x = %08x\n", reg_addr, val);

   for (inx = 0, 
           reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + RBUS_BASE ; 
        inx < 8; inx ++, reg_addr += 4 ) {

      val = *(unsigned volatile*)reg_addr;
      SDBGP ("VDL_OVRIDE WL0 %08x saved as %08x (%d)\n", reg_addr, val, inx);
      org_vdl_byte_bit [inx] = val;
   }

   // inx = 8
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + RBUS_BASE; 
         inx < 16; inx ++, reg_addr += 4 ) {
      val = *(unsigned volatile*)reg_addr;
      SDBGP ("VDL_OVRIDE WL0 %08x saved as %08x (%d)\n", reg_addr, val, inx);
      org_vdl_byte_bit [inx] = val;
   }

#if MEMC_MODE == 32
   // inx = 16
   for (reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + RBUS_BASE + WL_OFFSET; 
        inx < 24; inx ++, reg_addr += 4 ) {

      val = *(unsigned volatile*)reg_addr;
      SDBGP ("VDL_OVRIDE WL1 %08x saved as %08x (%d)\n", reg_addr, val, inx);
      org_vdl_byte_bit [inx] = val;
   }

   // inx = 24
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + RBUS_BASE + WL_OFFSET ; 
         inx < 32; inx ++, reg_addr += 4 ) {
      val = *(unsigned volatile*)reg_addr;
      SDBGP ("VDL_OVRIDE WL1 %08x saved as %08x (%d)\n", reg_addr, val, inx);
      org_vdl_byte_bit [inx] = val;
   }
#endif

}  


////

void restore_vdl_wr_settings (unsigned memc_base, unsigned *p_org_dac, unsigned org_vdl_byte_bit [])
{
   unsigned reg_addr;
   int inx;

   reg_addr = memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0 + RBUS_BASE;
   *(unsigned volatile*)reg_addr = *p_org_dac;

   SDBGP ("DAC_CONTROL @%08x restored to %08x\n", reg_addr, *p_org_dac);

   for (inx = 0, reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + RBUS_BASE ; 
        inx < 8; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = (org_vdl_byte_bit [inx] | (1<<17)); // set OVR_FORCE bit to write back
      SDBGP ("VDL_OVRIDE WL0 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
 //DDR-EYE-SHMOO:wait till previous VDL setting done
      host_wait_not_busy (BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + (inx<<2));     
   }
   // inx = 8
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + RBUS_BASE; 
         inx < 16; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = (org_vdl_byte_bit [inx] | (1<<17));
      SDBGP ("VDL_OVRIDE WL0 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
//DDR-EYE-SHMOO:wait till previous VDL setting done
      host_wait_not_busy (BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + ((inx-8)<<2) );
   }
#if MEMC_MODE == 32
   for (reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + RBUS_BASE + WL_OFFSET; 
        inx < 24; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = (org_vdl_byte_bit [inx] | (1<<17)); // set OVR_FORCE bit to write back
      SDBGP ("VDL_OVRIDE WL1 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
   }
   // inx = 24
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + RBUS_BASE + WL_OFFSET; 
         inx < 32; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = (org_vdl_byte_bit [inx] | (1<<17));
      SDBGP ("VDL_OVRIDE WL1 %08x restored as %08x (%d)\n", reg_addr, org_vdl_byte_bit [inx], inx);
   }
#endif
}

//DDR-EYE-SHMOO: added
static void wait_loop( unsigned ticks )
{
  while ( ticks-- > 0 ) /* nothing */ ;
}

void do_vdl_wr_settings (unsigned memc_base, unsigned vref, unsigned vdl_val)
{
   unsigned reg_addr;
   unsigned reg_val;
   unsigned dac_val;
   unsigned vdl_set_val = vdl_val | 0x30000;

   int inx;

   dac_val = vref << BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_SHIFT;
#if 0
   dbg_printf ("MEMC %08x -> set VREF (%d:%x -> shift %d) to %x, WR VDL to %d (%08x)\n", memc_base, vref, vref, 
               BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_SHIFT,
               dac_val, vdl_val, vdl_set_val);
#endif
   reg_addr = memc_base + BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL + PHY_BASE_TYPE_0 + RBUS_BASE;

   reg_val  = *(unsigned volatile*)reg_addr & (~(BCHP_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROL_DAC1_MASK));

   DVWSP (" -> reg orginal = %08x (masked)\n", reg_val);
   dac_val |= reg_val;
   DVWSP (" -> reg now changed to %08x\n", dac_val );

   DVWSP ("DAC_CONTROL @%08x set to %08x\n", reg_addr, dac_val);
   *(unsigned volatile*)reg_addr = dac_val;
   
//DDR-EYE-SHMOO:add some delay
wait_loop(0x10000);

   for (inx = 0, reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + RBUS_BASE ; 
        inx < 8; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr =  vdl_set_val; // set OVR_FORCE bit to write back
      DVWSP ("VDL_OVRIDE WL0 %08x set as %08x (%d)\n", reg_addr, vdl_set_val);
         
//DDR-EYE-SHMOO:wait till previous VDL setting done
      host_wait_not_busy (BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + (inx<<2));
   }
   // inx = 8
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + RBUS_BASE; 
         inx < 16; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = vdl_set_val;
      DVWSP ("VDL_OVRIDE WL0 %08x set as %08x (%d)\n", reg_addr, vdl_set_val);
//DDR-EYE-SHMOO:wait till previous VDL setting done
      host_wait_not_busy (BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + ((inx-8)<<2) );
   }
     
#if MEMC_MODE == 32
   for (reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W + RBUS_BASE + WL_OFFSET; 
        inx < 24; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = vdl_set_val; // set OVR_FORCE bit to write back
      DVWSP ("VDL_OVRIDE WL1 %08x set as %08x (%d)\n", reg_addr, vdl_set_val);
   }
   // inx = 24
   for ( reg_addr = memc_base + PHY_OFFSET + BCHP_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W + RBUS_BASE + WL_OFFSET; 
         inx < 32; inx ++, reg_addr += 4 ) {
      *(unsigned volatile*)reg_addr = vdl_set_val;
      DVWSP ("VDL_OVRIDE WL1 %08x set as %08x (%d)\n", reg_addr, vdl_set_val);
   }
#endif
}

//DDR-EYE-SHMOO: change arguments format
//unsigned test_memory_wr_eye (char *line)
unsigned test_memory_wr_eye (unsigned source, unsigned size, unsigned seed)
{
//DDR-EYE-SHMOO: comment out
//   char name [DIAG_SIZE_32];
//   unsigned source, size, seed, xor_result, verbose = 0;
   unsigned xor_result;//, verbose = 0;

   tinymt32_t tinymt;  

   OverrideInfo_t sOverride;

   sOverride.uDqOverride    = 0xFFFFFFFF;
//DDR-EYE-SHMOO: comment out
//   cdeScanf (line, "%s%x%x%x", name, &source, &size, &seed);
   tinymt.mat1 = 0; tinymt.mat2 = 0; tinymt.tmat = 0;
   tinymt32_init(&tinymt, seed);


   xor_result =  mips_test_memory (source, size, &tinymt, seed, &sOverride);

   return xor_result;
}

// dual write eye test memory loop
// returns the combined (AND'ed) P and N results
// note this function modifies the results_array
unsigned mips_test_memory_dual_write_eye (WRI_INFO *this_wri,
                                          unsigned source,
                                          unsigned size,
                                          tinymt32_t *ptinymt,
                                          unsigned vref,
                                          unsigned vdl)
{
   unsigned inx, mtest_result = 0;
   unsigned src; 

   unsigned chunk_count = size / MAX_INT_PER_CHUNK;
   unsigned chunk;
   unsigned *ps,  ps_flush;
   unsigned cum_err[SIZE_NUM_DUAL_EYES] = {/*0,*/ 0};
   unsigned dq_err[SIZE_NUM_DUAL_EYES];
   unsigned cum_err_t = 0;
   unsigned test_over = 0;
   unsigned int expected;
//   unsigned int shuffled;
  
   unsigned int error_mask;

   // optimal VREF related
   unsigned result_marker;
   
   ps = (unsigned*) source;
  
   error_mask = ~(this_wri->sOverride.uDqOverride);


   for (chunk = 0; chunk < chunk_count; chunk ++)
   {
      // dbg_printf ("testing chunk %08x  size %08x\n", ps, MAX_INT_PER_CHUNK );
      
      ps_flush = (unsigned) ps;
      
      for (inx = 0; inx < MAX_INT_PER_CHUNK; inx ++, ps++ )
      {
         src = *ps;
         expected = tinymt32_generate_uint32(ptinymt);
         
         if (src == expected) continue;
         // test failed
         // check if all DQ has failed at least once - if so, testing is over
         cum_err_t = (src ^ expected);
#if MEMC_MODE == 32
         fdq_binary_dual_cycle (ps, cum_err_t, dq_err, 1);
#else
         fdq_binary_dual_cycle ((unsigned)ps, cum_err_t, dq_err, 0);
#endif
         cum_err[0] |= dq_err[0];
#if (SIZE_NUM_DUAL_EYES > 1)
         cum_err[1] |= dq_err[1];
#endif
         
#if (SIZE_NUM_DUAL_EYES > 1)
         if (cum_err[0] == MT_ALL_FAILED && cum_err[1] == MT_ALL_FAILED)
#else
         if (cum_err[0] == MT_ALL_FAILED)
#endif
         {
            test_over = 1;
            break;
         }
      }
      // done with one chunk, flush D cache
      flush_data_cache (ps_flush, MAX_BYTES_PER_CHUNK, 0);
      
      if (test_over)
         break;
   }
   
#if (SIZE_NUM_DUAL_EYES > 1)
   mtest_result = (cum_err[0] & cum_err[1]);
#else
   mtest_result = (cum_err[0]);
#endif
    
   if ( mtest_result == WRI_ALL_FAILED )
      result_marker = 'f';
   else
   {
      if (mtest_result == 0) result_marker = '0';
      else result_marker = ' ';
   }
   
   this_wri->send_char (result_marker);

   // For time lapse support, the test_result is OR'ed with the
   // record. In the case of a very bad eye to start with, then
   // no subsequent run can change 1 to 0. But in that case, the
   // eye will be bad for the entire run with multiple loops.
   this_wri->results_array[this_wri->phy_id][0][64*vref + vdl] |= cum_err[0];
#if (SIZE_NUM_DUAL_EYES > 1)
   this_wri->results_array[this_wri->phy_id][1][64*vref + vdl] |= cum_err[1];
#endif
         
   return (mtest_result);
}

//// NEW METHOD

void prt_wr_eye_shmoo_header_2 (WRI_INFO *this_wri)
{
   char result_str [100];
   int i;
   char prefix_str [] = "wr_eye:             ";

   result_str [0] = 0;
   this_wri->printf ("\n%s", prefix_str);
   for (i = 0; i < 64; i ++ ) {
      this_wri->send_char ('0' + i/10);
   }
   this_wri->printf ("\n%s", prefix_str);
   for (i = 0; i < 64; i ++ ) {
      this_wri->send_char('0' + (i % 10));
   }
   this_wri->printf ("\n");
}

void prt_wr_eye_shmoo_result_2 (WRI_INFO *this_wri)
{
   //unsigned unsigned *p_init_setting, unsigned phy, unsigned print_flag, unsigned int override)

   char result_str [100];
//   char prefix_str [5];
   int bit;
   unsigned bit_masked_result;
//   unsigned upper_limit;
   unsigned vref, vdl, mV;
   unsigned marker;
   unsigned shmoo_picked_w_vdl;
//   unsigned int ignore_bits = 0; /// need to fill in real ignore_bits according to override

   for (bit = 0, marker = 1; bit < BITS_TO_PLOT ; bit ++, marker = marker << 1 ) {
      // if ( marker & ignore_bits ) continue; // skip print if override bit is set
      shmoo_picked_w_vdl = this_wri->org_vdl_byte_bit [bit] & VDL_W_MASK ;
      this_wri->printf ("\nwr_eye:  ********************  bit = %d (shmoo picked vdl = %d)\n", bit, shmoo_picked_w_vdl );
      prt_wr_eye_shmoo_header_2(this_wri);
//DDR-EYE-SHMOO: plot less info
      for (vref = 0+15; vref < SIZE_VREF-15; vref ++ ) {
         //    mV = (1500 * vref) / 64 ; // 'ddr3 supply voltage = 1500 mV
         mV = VREF_TO_MV(vref); // 'ddr3 supply voltage = 1500 mV
         for (vdl = 0; vdl < SIZE_VDL; vdl ++ ) {
            bit_masked_result = this_wri->results_array[this_wri->phy_id][0][(64*vref)+vdl] & marker;
            if (bit_masked_result != 0) result_str [vdl] = WR_EYE_INVALID;
            else result_str [vdl] = WR_EYE_VALID;
            // at mid point of vref (32) print the orginal shmoo picked W VDL value
            if (vref == 32) {
               if (vdl == shmoo_picked_w_vdl) result_str [vdl] = SHMOO_PICKED_W_VDL;
            }
         }
         result_str [vdl] = 0; /// terminate the string
         if (this_wri->mode == 0)
            this_wri->printf ("wr_eye_%d_%02d: %4dmV %s\n", this_wri->phy_id, bit, mV, result_str);
         else
            this_wri->printf ("wr_eye_%d_%02d_p: %4dmV %s\n", this_wri->phy_id, bit, mV, result_str);
      }

      if (this_wri->mode > 0)
      {
         this_wri->printf ("\n");

         for (vref = 0; vref < SIZE_VREF; vref ++ ) {
            //    mV = (1500 * vref) / 64 ; // 'ddr3 supply voltage = 1500 mV
            mV = VREF_TO_MV(vref); // 'ddr3 supply voltage = 1500 mV
            for (vdl = 0; vdl < SIZE_VDL; vdl ++ ) {
               bit_masked_result = this_wri->results_array[this_wri->phy_id][1][(64*vref)+vdl] & marker;
               if (bit_masked_result != 0) result_str [vdl] = WR_EYE_INVALID;
               else result_str [vdl] = WR_EYE_VALID;
               // at mid point of vref (32) print the orginal shmoo picked W VDL value
               if (vref == 32) {
                  if (vdl == shmoo_picked_w_vdl) result_str [vdl] = SHMOO_PICKED_W_VDL;
               }
            }
            result_str [vdl] = 0; /// terminate the string
            this_wri->printf ("wr_eye_%d_%02d_n: %4dmV %s\n", this_wri->phy_id, bit, mV, result_str);
         }
      }
   }
}

///// 

unsigned do_set_vref_test_write_2 (WRI_INFO *this_wri, unsigned vref )
{
//   char cmd [DIAG_SIZE_64];
   unsigned vdl = 0;
   unsigned result;
   unsigned marker;
   tinymt32_t tinymt;  

   DSVTWP ("dsvtw: memc %08x vref %d addr %08x size %08x seed %08x\n", this_wri->memc_base,vref,this_wri->mt_addr,
           this_wri->mt_size, this_wri->seed);
   // 
   dbg_sprintf  (cmd, "tmtm %x %x %x", this_wri->mt_addr, this_wri->mt_size/4, this_wri->seed);
   DSVTWP  ("memory test command -> [%s]\n", cmd);

   this_wri->printf ("\nVREF %02d VDL (0:63) -> ", vref, vdl);

   for (vdl = 0; vdl < SIZE_VDL; vdl ++ )
   {
      // fill memory
      do_vdl_wr_settings (this_wri->memc_base, vref, vdl );
      srdi_fill_memory ( this_wri->mt_addr, this_wri->mt_size, this_wri->seed, &this_wri->sOverride );
      
      if (this_wri->mode == 0)
      {
//DDR-EYE-SHMOO: change arguments      	
         //result = test_memory_wr_eye (cmd);
         result = test_memory_wr_eye (this_wri->mt_addr, this_wri->mt_size/4, this_wri->seed);
         if (result == WRI_ALL_FAILED)
            marker = 'f';
         else
         {
            if (result == WRI_ALL_PASSED) marker = '0';
            else marker = ' ';
         }
         this_wri->send_char (marker);
         this_wri->results_array [this_wri->phy_id][0][64*vref + vdl] |= result;
      }
      else
      {
         tinymt.mat1 = 0; tinymt.mat2 = 0; tinymt.tmat = 0;
         tinymt32_init(&tinymt, this_wri->seed);
         result = mips_test_memory_dual_write_eye (this_wri,
                                                   this_wri->mt_addr,
                                                   this_wri->mt_size/4,
                                                   &tinymt,
                                                   vref,
                                                   vdl);
      }
   }
   return DIAG_RET_OK;
}

unsigned do_wr_eye ( WRI_INFO *this_wri)
{
   //  unsigned memc_base, unsigned addr, unsigned size, unsigned seed, unsigned clear_result, unsigned printok )
   //  unsigned org_dac;
   //  unsigned org_vdl_byte_bit [W_VDL_BYTE_BIT_REGS_COUNT];

   int inx;
   int vref;
   //  int this_wri->phy_id = 0;

   //  if ((addr & 0xF0000000) == 0xF0000000) phy = 1;

   this_wri->printf ("\nRunning WRITE EYE for MEMC%d - test memory from %08x size %08x\n", 
                     this_wri->phy_id, this_wri->mt_addr, this_wri->mt_size);
  
   dbg_printf ("\n\tCurrent random seed = %08x\n", this_wri->seed);

   save_vdl_wr_settings (this_wri->memc_base, &this_wri->org_dac, this_wri->org_vdl_byte_bit);
  
   // clear result
   if (this_wri->clear_result)
   {
      this_wri->printf ("Clear test result array\n");
      for (vref = 0; vref < SIZE_NUM_DUAL_EYES; vref++)
      {
         for (inx = 0; inx < SIZE_RESULT_ARRAY; inx++)
            this_wri->results_array[this_wri->phy_id][vref][inx] = 0;
      }
   }
   this_wri->printf ("\n");
   for (vref = 0; vref < SIZE_VREF; vref ++ )
   {
      do_set_vref_test_write_2 (this_wri, vref);
   }

   restore_vdl_wr_settings (this_wri->memc_base, &this_wri->org_dac, this_wri->org_vdl_byte_bit);
   this_wri->printf ("\n");

   //  prt_wr_eye_shmoo_result (this_wri->results_array, this_wri->org_vdl_byte_bit, this_wri->phy_id, 1, 0);
   prt_wr_eye_shmoo_result_2 (this_wri);
   return DIAG_RET_OK;
}

// Command line for a single write eye plot
unsigned mips_wr_eye_plot (char *line)
{
   char name [DIAG_SIZE_32];
   unsigned seed, memc_base, mt_start, mt_size, clear_result, mode;

   WRI_INFO sWriInfo;
   WRI_INFO *pWriInfo;

   cdeScanf (line, "%s%d%x%x%x%x%d", name, &mode, &memc_base, &mt_start, &mt_size, &seed, &clear_result );
   dbg_printf ("\nmwri mode %d base %08x start %08x size %08x seed %08x clear %d\n",
               mode, memc_base, mt_start, mt_size, seed, clear_result );

   pWriInfo = wri_setup_new (&sWriInfo,                                      // wri_info
                             (memc_base >= MEMC1_MT_BASE) ? MEMC1 : MEMC0,   // phy_id
                             mode,                                           // mode
                             memc_base,                                      // memc_base
                             mt_start,                                       // mt_start
                             mt_size,                                        // mt_size
                             USE_CURRENT_TIME_FOR_RANDOM_SEED );             // seed

   if (seed != USE_CURRENT_TIME_FOR_RANDOM_SEED)
      pWriInfo->seed = seed;
   else
      generate_random_seed ( &pWriInfo->seed );

   // start DIS
   dis_special_setup(MEMC0, MEMC0_DIS0_START, MEMC0_DIS0_END,
                     MEMC0_DIS1_START, MEMC0_DIS1_END, DIS_LFSR-1);
   dis_test_trigger_memc (MEMC0);
#ifdef MEMC_1_DDR_ENABLE
   dis_special_setup(MEMC1, MEMC1_DIS0_START, MEMC1_DIS0_END,
                     MEMC1_DIS1_START, MEMC1_DIS1_END, DIS_LFSR-1);
   dis_test_trigger_memc (MEMC1);
#endif

   // write eye test
   do_wr_eye (pWriInfo);

   // stop DIS
   dis_stop_test_memc(MEMC0);
#ifdef MEMC_1_DDR_ENABLE
   dis_stop_test_memc(MEMC1);
#endif
   
   return DIAG_RET_OK;
}

// Auto write eye, command line support for time elapsed write eye
unsigned auto_wr_eye (char *line)
{
   char discard [DIAG_SIZE_32];
   unsigned mode = 0;
   unsigned interval = ATRI_DEFAULT_INTERVAL;
   unsigned loop     = ATRI_DEFAULT_LOOP;
   unsigned seed = USE_CURRENT_TIME_FOR_RANDOM_SEED;
   int inx/*, run, plot_flag = 1*/;
   unsigned /*time_now,*/ time_elapsed;
   unsigned print_allowed;

//DDR-EYE-SHMOO: added   
   unsigned time_start; 

   WRI_INFO sWriInfo;
   WRI_INFO *pWriInfo;

#if AUTO_READ_EYE_LOOP_COUNT
   loop     = AUTO_READ_EYE_LOOP_COUNT;
#endif
  
#if AUTO_READ_EYE_INTERVAL
   interval = AUTO_READ_EYE_INTERVAL;
#endif
  
#if AUTO_WRITE_EYE_MODE
   mode = AUTO_WRITE_EYE_MODE;
#endif

   
   if (strstr (line, "ARGS") != NULL)
   {
      // atwi ARGS <mode><loop count><time interval><seed>
      cdeScanf (line, "%s%s%d%d%d%d%x", discard, discard, &mode, &loop, &interval, &seed);
   }
   
   dbg_printf ("auto-write-eye mode %d interval %d s loop %d\n", mode, interval, loop);
   
   pWriInfo = wri_setup_new(&sWriInfo,            // wri_info
                            MEMC0,                // phy_id
                            mode,                 // mode
                            TAWRI_MEMC0_BASE,     // memc_base
                            TAWRI_MEMC0_MT_TOP,   // mt_addr
                            TAWRI_MEMC0_MT_SIZE,  // mt_size
                            seed);                // seed

   // first run
//DDR-EYE-SHMOO: comment out   
//   clear_system_time ();
//   time_now = get_system_time ();
//   time_elapsed = MAX_TIME;

   if (seed != USE_CURRENT_TIME_FOR_RANDOM_SEED)
      pWriInfo->seed = seed;
   else
      generate_random_seed ( &pWriInfo->seed );
   
   // start DIS
   dis_special_setup(MEMC0, MEMC0_DIS0_START, MEMC0_DIS0_END,
                     MEMC0_DIS1_START, MEMC0_DIS1_END, DIS_LFSR-1);
   dis_test_trigger_memc (MEMC0);
#ifdef MEMC_1_DDR_ENABLE
   dis_special_setup(MEMC1, MEMC1_DIS0_START, MEMC1_DIS0_END,
                     MEMC1_DIS1_START, MEMC1_DIS1_END, DIS_LFSR-1);
   dis_test_trigger_memc (MEMC1);
#endif
 
   // MEMC0
   //DDR-EYE-SHMOO: for maintaining plot period
   time_start = current_time_in_seconds();
   do_wr_eye (pWriInfo);
   
   //DDR-EYE-SHMOO: follow the codes in mde_rdi.c for maintaining plot period
   time_elapsed = current_time_in_seconds () - time_start;
   if (time_elapsed >= interval)
       print_allowed = 1;
   else
   	   print_allowed = 0;    
   
   // MEMC1
#ifdef MEMC_1_DDR_ENABLE
   // switch to MEMC1
   pWriInfo->phy_id     = MEMC1;
   pWriInfo->memc_base  = TAWRI_MEMC1_BASE;
   pWriInfo->mt_addr    = TAWRI_MEMC1_MT_TOP;
   pWriInfo->mt_size    = TAWRI_MEMC1_MT_SIZE;
   pWriInfo->dis0_start = MEMC1_DIS0_START;
   pWriInfo->dis0_end   = MEMC1_DIS0_END;
   pWriInfo->dis1_start = MEMC1_DIS1_START;
   pWriInfo->dis1_end   = MEMC1_DIS1_END;
   do_wr_eye (pWriInfo);
   // switch back to MEMC0
   pWriInfo->phy_id     = MEMC0;
   pWriInfo->memc_base  = TAWRI_MEMC0_BASE;
   pWriInfo->mt_addr    = TAWRI_MEMC0_MT_TOP;
   pWriInfo->mt_size    = TAWRI_MEMC0_MT_SIZE;
   pWriInfo->dis0_start = MEMC0_DIS0_START;
   pWriInfo->dis0_end   = MEMC0_DIS0_END;
   pWriInfo->dis1_start = MEMC0_DIS1_START;
   pWriInfo->dis1_end   = MEMC0_DIS1_END;
#endif

   /*
     After the first run, all subsequent runs will do these:

     clear_result should be disabled.
     no plot unless time interval expires

   */
   pWriInfo->clear_result = 0;

   /*
     We have done plotting once
     Ready to do time elapsed write-eye
   */
   
 //DDR-EYE-SHMOO: follow the codes in mde_rdi.c for maintaining plot period
   time_elapsed = 0;
   time_start = current_time_in_seconds();;
   for (inx = 1; inx < loop;  )
   {
      // generate a new seed
      generate_random_seed ( &pWriInfo->seed );
   
      if (print_allowed)
      {	
         dbg_printf ("\nPlot enabled for the next run\n");
				 dbg_printf ("\n Plot of write eye -- loop =%d \n", inx);
         print_allowed = 0; 

         pWriInfo->plot_flag = 1;
         pWriInfo->send_char = send_uart;
         pWriInfo->printf    = dbg_printf;
         do_wr_eye (pWriInfo);
          //DDR-EYE-SHMOO:for maintaining plot period
         time_elapsed = current_time_in_seconds () - time_start;
         if (time_elapsed >= interval)
             print_allowed = 1;
         else
   	         print_allowed = 0;  
         
#ifdef MEMC_1_DDR_ENABLE
         // switch to MEMC1
         pWriInfo->phy_id     = MEMC1;
         pWriInfo->memc_base  = TAWRI_MEMC1_BASE;
         pWriInfo->mt_addr    = TAWRI_MEMC1_MT_TOP;
         pWriInfo->mt_size    = TAWRI_MEMC1_MT_SIZE;
         pWriInfo->dis0_start = MEMC1_DIS0_START;
         pWriInfo->dis0_end   = MEMC1_DIS0_END;
         pWriInfo->dis1_start = MEMC1_DIS1_START;
         pWriInfo->dis1_end   = MEMC1_DIS1_END;
         do_wr_eye (pWriInfo);
         // switch back to MEMC0
         pWriInfo->phy_id     = MEMC0;
         pWriInfo->memc_base  = TAWRI_MEMC0_BASE;
         pWriInfo->mt_addr    = TAWRI_MEMC0_MT_TOP;
         pWriInfo->mt_size    = TAWRI_MEMC0_MT_SIZE;
         pWriInfo->dis0_start = MEMC0_DIS0_START;
         pWriInfo->dis0_end   = MEMC0_DIS0_END;
         pWriInfo->dis1_start = MEMC0_DIS1_START;
         pWriInfo->dis1_end   = MEMC0_DIS1_END;
#endif
         inx ++;
         //DDR-EYE-SHMOO: add this line
         time_start = current_time_in_seconds ();
         //DDR-EYE-SHMOO: comment out this line
         //clear_system_time();
      }
      else
      {
         /// plot not allowed
         //DDR-EYE-SHMOO: change time_now to time_elapsed
         dbg_printf ("\nWaiting to plot #%d (of %d). Interval %d - time elapsed %d", inx+1, loop, interval, time_elapsed);
         //DDR-EYE-SHMOO: add msg output
         dbg_printf("\nwrite eye test without ploting ......\n");
         //dbg_printf ("\n\tRunning write-eye on MEMC0 ");
         
         pWriInfo->plot_flag = 0;
         pWriInfo->send_char = send_char_to_bucket;
         pWriInfo->printf    = send_text_to_bucket;
         do_wr_eye (pWriInfo);
         
#ifdef MEMC_1_DDR_ENABLE
         dbg_printf ("\n\tRunning write-eye on MEMC1 ");
         // switch to MEMC1
         pWriInfo->phy_id     = MEMC1;
         pWriInfo->memc_base  = TAWRI_MEMC1_BASE;
         pWriInfo->mt_addr    = TAWRI_MEMC1_MT_TOP;
         pWriInfo->mt_size    = TAWRI_MEMC1_MT_SIZE;
         pWriInfo->dis0_start = MEMC1_DIS0_START;
         pWriInfo->dis0_end   = MEMC1_DIS0_END;
         pWriInfo->dis1_start = MEMC1_DIS1_START;
         pWriInfo->dis1_end   = MEMC1_DIS1_END;
         do_wr_eye (pWriInfo);
         // switch back to MEMC0
         pWriInfo->phy_id     = MEMC0;
         pWriInfo->memc_base  = TAWRI_MEMC0_BASE;
         pWriInfo->mt_addr    = TAWRI_MEMC0_MT_TOP;
         pWriInfo->mt_size    = TAWRI_MEMC0_MT_SIZE;
         pWriInfo->dis0_start = MEMC0_DIS0_START;
         pWriInfo->dis0_end   = MEMC0_DIS0_END;
         pWriInfo->dis1_start = MEMC0_DIS1_START;
         pWriInfo->dis1_end   = MEMC0_DIS1_END;
#endif
      }
      //DDR-EYE-SHMOO: change time maintenance
      //time_now = get_system_time ();
      //if (time_now >= interval)
      //   print_allowed = 1;
      time_elapsed = current_time_in_seconds () - time_start;
      if (time_elapsed >= interval)
      {
         print_allowed = 1;
		     time_start = current_time_in_seconds ();
      }
   }

   // stop DIS
   dis_stop_test_memc(MEMC0);
#ifdef MEMC_1_DDR_ENABLE
   dis_stop_test_memc(MEMC1);
#endif
   
   return DIAG_RET_OK;
}

