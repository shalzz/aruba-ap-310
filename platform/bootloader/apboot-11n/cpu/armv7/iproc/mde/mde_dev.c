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

#include <mde_common_headers.h>

extern MDE_CMD *host_setup_local_cmds ( unsigned *uCommandCount );

void setupNewMdeWithCommonInfo ( sMde_t *newMde )
{
  int inx;

  // setup common MDE properties

  newMde -> version = MDE_VERSION;
  newMde -> date    = MDE_DATE_CODE;

  strcpy ( &newMde->name [0], MDE_NAME );

  newMde -> cli_history_depth = HISTORY_DEPTH;
  for (inx = 0; inx < HISTORY_DEPTH; inx ++) {
	newMde -> history [inx][0] = 0; // terminate history buffer lines
  }
  // setup MDE command commands
  newMde -> common.cmds = getMdeCommonCommands  ( &newMde -> common.cmd_count );
#ifdef HOST_CMD_INCLUDED
  newMde -> local.cmds  = host_setup_local_cmds ( &newMde -> local.cmd_count  );
#endif
  //  newMde -> = ;
}


// Compute the DDR PLL frequency in KHz.
// specific for ANA_DPLL_40G, at x4 mode.
// (joon lee)
// 
// inputs:
//    mult    =  PLL multipler  (P)
//    div_int =  PLL divider interger (N)
//    div_fraq=  PLL divider fraqtional (N fraq)
//    freq_mhz=  external reference clock in MHz
//    post_div=  post divider.
//
// output:
//    PLL freq in KHz.
//
int ana_dpll_40g_pll_khz(int mult, 
                         int div_int, 
                         int div_fraq, 
                         int fref_mhz, 
                         int post_div)
{

  int vco_freq, pll_freq;
  int internal_scaler = 20;
  int ndiv_scaler  = (1<<internal_scaler);
  int dram_freq;
//  int left;
  int xtra_scaling = 1;
  int fraq;

  // apply range masks to input
  mult     &= 7;      // 3 bits
  div_int  &= 0xFF;   // 8 bits
  div_fraq &= 0xFFFFF;// 20 bits
  div_fraq >>= (20-internal_scaler);

  vco_freq = xtra_scaling*(div_int*ndiv_scaler + div_fraq) ;
  vco_freq /= mult;
  vco_freq /= post_div;
  vco_freq *= fref_mhz;

  pll_freq = vco_freq/ndiv_scaler;
  
  dram_freq = pll_freq * 2; // x4 mode

  // fractional
  fraq = 1000*(vco_freq % ndiv_scaler);
  fraq /= ndiv_scaler;
  fraq *= 2;

  dram_freq = dram_freq*1000 + fraq;
  return (dram_freq);
}

#define EXTERNAL_REF_CLOCK_MHZ 54

unsigned mde_do_ana_pll_calc ( sMde_t *mde )
{
  //
  // 833, 866 and 900 are derived from 933 adjusting downward
  //
  // 966, 1000, 1033 are derived from 1067 adjusting downward
  // 1107, 1134 are derived from 1067 adjusting upward
  //
  // The reported DDR frequency is calcuated based on the following two registers
  //

  unsigned pll6ch    = 0x2451C71D; // need to read register
  unsigned pll_mdiv0 = 0x00001004; // need to read register

  unsigned q, r;
  unsigned calc_val, /*tenth, compliment = 0,*/ round_up;
  unsigned freq_mhz = EXTERNAL_REF_CLOCK_MHZ;

  unsigned mult     = (pll_mdiv0 & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_SYS_PLL_MDIV0_CHAN_MDIV_MASK);
  unsigned ndiv_int = (pll6ch & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_INT_MASK) >> \
	BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_INT_SHIFT;

  unsigned ndiv_fra = (pll6ch & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_FRAC_MASK) >> \
	BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_NDIV_FRAC_SHIFT;
  unsigned post_div = ( pll6ch & BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_PDIV_MASK ) >> \
	BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_PLL6CH_PNDIV_SYS_PDIV_SHIFT;

  calc_val = ana_dpll_40g_pll_khz ( mult, ndiv_int, ndiv_fra, freq_mhz, post_div );

  // The returned value is MHz X 1000 and it can be more or less than the target frequency
  q = calc_val / 1000;
  r = calc_val % 1000;

  calc_val = calc_val + ( 1000 - r);
  round_up = calc_val / 1000;
  //  host_printf ("\nDDR speed = %d.%dMHz rounded to %d\n",  q, r, round_up);
  return round_up; 
}

void mdeInit (sMde_t *mde)
{

  host_printf ("\nMDE for %s\n", mde->chip_name );
  host_printf ("size of MDE %d bytes\n", sizeof( sMde_t) );
  // clear the current command point to start
  mde -> current_command = NULL;

  // setup find_dq handling according to PHY data width
  // unless specified, default is 32
#if 0
  host_printf ("Skipping set up chip specific init for now\n");
#else
  host_printf ("set up find_dq handlers for %d-bit PHY\n", PHY_WIDTH);
#if PHY_WIDTH == 16
  mde->fdq_log_error    = find_all_errors_all_16;
  mde->fdq_show_result  = find_dq_show_result_all_16;
#else

  mde->fdq_log_error    = find_all_errors_all;
  mde->fdq_show_result  = find_dq_show_result_all;
#endif


  mde->fdq_init = find_dq_init_all;

  // DDR frequency calcuation
  mde->ddr_frequency = mde_do_ana_pll_calc ( mde );
#endif
}

