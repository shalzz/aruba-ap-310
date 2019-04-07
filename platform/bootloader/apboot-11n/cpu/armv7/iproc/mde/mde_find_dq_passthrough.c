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
    *  Filename: mde_main.c
    *
    *  Function:
    *    this version is for "pass-thrugh" memc (non Andover MEMC)
    *
    *  Author:  Chung Kwong (C.K.) Der , Joon Lee
    *
    * 
    * $Id::                                                       $:
    * $Rev:: : Global SVN Revision = 1950                         $:
    * 
 */

#include <mde_common_headers.h>


extern int map_err_bit_to_dq  ( unsigned int addx, int bit_pos);
extern int map_err_bit_to_cyc ( unsigned int addx, int bit_pos);
extern int map_err_bit_to_dq_16( unsigned int addx, int bit_pos);
extern int map_err_bit_to_cyc_16( unsigned int addx, int bit_pos);

#define SCALE_FACTOR (1000*1000) // (1024*1024) 

void find_dq_init_all ( sMde_t *mde , int which)
{
  int II,JJ;

  // clear out values
  for (II=0;II<32;II++) 
    for(JJ=0;JJ<16;JJ++) 
	  mde->memtester [which].result [II][JJ] = 0;
}



inline int find_all_errors_all (sMde_t *mde , int which) 
{
  unsigned int addx = mde->memtester [which].addx;
  unsigned int mask=1;
  unsigned int diff;
  int II, /*BL, shuffled,*/ dq, cyc,err_cnt=0;


  // find all faling bits
  diff = mde->memtester [which].act_data ^ mde->memtester [which].exp_data;
  for (II=0;II<32;II++) {
    if ( (diff & mask) != 0 ) {
      dq  = map_err_bit_to_dq (addx,   II );
      cyc = map_err_bit_to_cyc(addx,   II ) ;
      mde->memtester [which].result [dq][cyc]++;
      err_cnt++;
    }
    mask<<=1;
  }

  return (err_cnt);

}

extern unsigned mde_do_ana_pll_calc ( sMde_t *mde );

int find_dq_show_result_all (sMde_t *mde , int which) 
{
  int II,JJ;//, line=0;
  int /*dbg=0,*/ saturate=0;
  unsigned int /*err_cnt=1,*/ temp, force1=1;
//  unsigned int addx, exp_data, act_data;
  unsigned bit_err_per_cyc;
  unsigned g_i_bcount_mb, total_mbits;
  unsigned loop_cnt = mde->memtester_loop;

  if (loop_cnt==0) {
	if (force1) loop_cnt = 1; 
  }

  host_printf ("\nfind_dq_show_result for MEMC%d CHIP ID %d DDR speed %dMHz WIDTH %d\n", which,
			   mde->chip_id,  mde_do_ana_pll_calc ( mde) , mde-> phy_data_width);

  // determine total Byte Count
  //  g_i_bcount    =  mde->memtester[which].test_bsize * loop_cnt; /// likely to overflow
  g_i_bcount_mb = (mde->memtester[which].test_bsize/SCALE_FACTOR) * loop_cnt; // unlikely to overflow


  host_printf("Found %d Loops.  Total Bytes CPU Read (%08x MB) \n", 
			  loop_cnt, g_i_bcount_mb );

  //  host_printf("User supplied Start Addx: %08X.  End Addx: %08X\n", saddx, eaddx);
  //  host_printf("Test Log file: %s\n\n", argv[1]);

  host_printf("options:  saturate: %d\n", saturate);

  if (saturate)  host_printf("Error Count Map (values in hex. Satuated to 0xFFF) \n");
  else host_printf("Error Count Map \n");

  // print total error count
  host_printf("        ");
  for(II=31;II>=0;II--) {
    if (saturate) host_printf ("DQ%02d ",  II);
    else          host_printf ("DQ%02d      ",  II);
  }
  host_printf("\n");
  for(JJ=0;JJ<8;JJ++) {
    host_printf("[%02d] : ", JJ);
    for(II=31;II>=0;II--) {
      temp = mde->memtester[which].result [II][JJ];
      if(saturate==0) {
        host_printf (" %08X ",  temp);
      }
      else {
        if (temp> 0xFFF) temp = 0xFFF;
        host_printf (" %03X ",  temp);
      }
    }
    host_printf("\n");
  }
  host_printf("\n");

  // print  error rate

  if (g_i_bcount_mb == 0) {
    host_printf("The Loop Count form the logfile is 0.  Please run test longer to obtain %% Bit Error Rate \n");
    return 0;
  }

  total_mbits = g_i_bcount_mb * 8;
  host_printf ("\ng_i_bcount_mb %08x total_mbits %08x\n", g_i_bcount_mb, total_mbits);

  //  host_printf("Bit Error Rate Percent.  Unit:  %% x PPB (total M bits %d)\n", total_mbits);
  host_printf("Bit Error Rate.  Unit:  PPB (total M bits %d)\n", total_mbits);
  host_printf("       ");
  for(II=31;II>=0;II--) {
    host_printf (" %5d ",  II);
  }
  host_printf("\n");
  for(JJ=0;JJ<8;JJ++) {
    host_printf("[%02d] : ", JJ);
	//// calculate the percentage -> error /(total 
    for(II=31;II>=0;II--) {      
	  // g_i_bcount_mb is in MB, total bits is then g_i_bcount_mb * 8
      bit_err_per_cyc = mde->memtester[which].result [II][JJ] * 1000; /// for PPB
	  if (bit_err_per_cyc > 0) {
		//		host_printf ("  bit_err_per_cyc => %d\n",bit_err_per_cyc);
	  }
	  mde_fdiv (bit_err_per_cyc, total_mbits, 3, mde);
    }
	host_printf ("\n");
  }  
  return 1; /// OK
}

//
inline int find_all_errors_all_16 ( sMde_t *mde , int which )
{
  unsigned int addx = mde->memtester [which].addx;
  unsigned int mask=1;
  unsigned int diff;
  int II, /*BL, shuffled,*/ dq, cyc,err_cnt=0;


  // find all faling bits
  diff = mde->memtester [which].act_data ^ mde->memtester [which].exp_data;
  for (II=0;II<32;II++) {
    if ( (diff & mask) != 0 ) {
      dq  = map_err_bit_to_dq_16 (addx,   II );
      cyc = map_err_bit_to_cyc_16(addx,   II ) ;
      mde->memtester [which].result [dq][cyc]++;
      err_cnt++;
    }
    mask<<=1;
  }
  return (err_cnt);
}

int find_dq_show_result_all_16 ( sMde_t *mde , int which) 
{
  // 
  //int which_memc, unsigned int result[16][16], int loop_cnt, unsigned saddx, unsigned eaddx)
  int loop_cnt = mde->memtester_loop;
  int II,JJ;//, line=0;
  //  unsigned int bit_err[32];
  int /*dbg=0,*/ saturate=0;
  unsigned int /*err_cnt=1,*/ temp, force1=1;
//  unsigned int addx, exp_data, act_data;
  unsigned bit_err_per_cyc;
  //  unsigned g_i_bcount, g_i_bcount_mb, total_mbits;
  unsigned  g_i_bcount_mb, total_mbits;


  if (loop_cnt==0) {
	if (force1) loop_cnt = 1; 
  }

  //  printf ("\nfind_dq_show_result for MEMC%d CHIP ID %d DDR speed %dMHz WIDTH %d\n", which_memc, 
  //		  BCHP_CHIP, MEMC_0_DDR_FREQ, MEMC_MODE, saddx);

  // determine total Byte Count
  //  g_i_bcount    = mde->memtester[which].test_bsize * loop_cnt; /// likely to overflow
  g_i_bcount_mb = mde->memtester[which].test_bsize/SCALE_FACTOR * loop_cnt; // unlikely to overflow


  host_printf("Found %d Loops.  Total Bytes CPU Read %08x MB\n", loop_cnt, g_i_bcount_mb );

  //  printf("User supplied Start Addx: %08X.  End Addx: %08X\n", saddx, eaddx);
  //  printf("Test Log file: %s\n\n", argv[1]);

  host_printf("options:  saturate: %d\n", saturate);

  if (saturate)  host_printf("Error Count Map (values in hex. Satuated to 0xFFF) \n");
  else host_printf("Error Count Map \n");

  // print total error count
  host_printf("        ");
  for(II=15;II>=0;II--) {
    if (saturate) host_printf ("DQ%02d ",  II);
    else          host_printf ("DQ%02d      ",  II);
  }
  host_printf("\n");
  for(JJ=0;JJ<16;JJ++) {
    host_printf("[%02d] : ", JJ);
    for(II=15;II>=0;II--) {
      temp = mde->memtester[which].result [II][JJ];
      if(saturate==0) {
        host_printf (" %08X ",  temp);
      }
      else {
        if (temp> 0xFFF) temp = 0xFFF;
        host_printf (" %03X ",  temp);
      }
    }
    host_printf("\n");
  }
  host_printf("\n");

  // print  error rate

  if (g_i_bcount_mb == 0) {
    host_printf("The Loop Count form the logfile is 0.  Please run test longer to obtain %% Bit Error Rate \n");
    return 0;
  }

  total_mbits = g_i_bcount_mb * 8;
  //  host_printf ("\ng_i_bcount %08x, g_i_bcount_mb %08x total_mbits %08x\n", g_i_bcount, g_i_bcount_mb, total_mbits);
  host_printf ("\ng_i_bcount_mb %08x total_mbits %08x\n", g_i_bcount_mb, total_mbits);

  //  host_printf("Bit Error Rate Percent.  Unit:  %% x PPB (total M bits %d)\n", total_mbits);
  host_printf("Bit Error Rate.  Unit:  PPB (total M bits %d)\n", total_mbits);
  host_printf("       ");
  for(II=15;II>=0;II--) {
    host_printf (" %5d ",  II);
  }
  host_printf("\n");
  for(JJ=0;JJ<15;JJ++) {
    host_printf("[%02d] : ", JJ);
	//// calculate the percentage -> error /(total 
    for(II=15;II>=0;II--) {      
	  // g_i_bcount_mb is in MB, total bits is then g_i_bcount_mb * 8
      bit_err_per_cyc = mde->memtester[which].result [II][JJ] * 1000; /// for PPB
	  if (bit_err_per_cyc > 0) {
		//		host_printf ("  bit_err_per_cyc => %d\n",bit_err_per_cyc);
	  }
	  mde_fdiv (bit_err_per_cyc, total_mbits, 3, mde );
    }
	host_printf ("\n");
  }  
  return 1; /// OK
}

// given an err bit pos, determine the DQ position
// * 32bit ver *
int map_err_bit_to_dq(unsigned int addx,  int bit_pos) {

  return ( bit_pos );

}
// given an err bit pos, determine the relative Cycle position (0 to 7)
// * 32bit ver *
int map_err_bit_to_cyc(unsigned int addx,  int bit_pos) {

  int cyc = (addx>>2)&7;
  return cyc;

}

// 16bit version of map_scb_bit_to_dq
//  16bit referrs to the PHY data bus width.  
//
//  input :    addx :  failg addx alinged to 32bit
//             err bit pos  0-31
//          
//  return:  0 - 15 : which represents the DQ position
//         
//
int map_err_bit_to_dq_16( unsigned int addx,  int bit_pos ){

  if (bit_pos>15) bit_pos-=16;
  return ( bit_pos );

}

// 16b Version of "map_scb_bit_to_cyc"
//  16b refers to the PHY data bus width.
// given an SCB bit pos, determine the relative Cycle position (0 to 15)
//
//  input :   addx:     failing addx aligned to 32bit
//            bit_pos:  error bit pos 0-31
//
//  output:   cycle pos   ( 0 to 15 )
//
int map_err_bit_to_cyc_16( unsigned int addx,  int bit_pos) {

  addx &= 0x1F;
  addx >>= 1;
  if (bit_pos>=16) addx++;
  return (addx ^ 1);

}

// Given a 32bit CPU data, and it's address associated,
// return the physical DQ mapped 32bit.
// 
// return DQ error map.
// input:   logical err map (xor of exp and actual), and 32bit aligned address
//
unsigned int fdq_binary(unsigned int addx, unsigned int logical_err_map) {

  return logical_err_map;

}


unsigned int fdq_binary_16(unsigned int addx, unsigned int logical_err_map) {

  return ( ((logical_err_map>>16)&0xFFFF) | ((logical_err_map)&0xFFFF) );

}

#if PHY_WIDTH == 32
// ** 32b PHY ONLY **
// Given a 32bit CPU data, and it's address associated,
// return the physical DQ mapped 32bit.
// 
// return   dual DQ error map for Even and Odd cycles (Even = P-vdl, Odd = N-vdl)
//            dq_error[0] = even
//            dq_error[1] = odd
//          -1 error
//           0 pass
//
// input:   logical err map (xor of exp and actual), and 32bit aligned address
//
//inline
int fdq_binary_dual_cycle(unsigned int addx, unsigned int logical_err_map,
                                        unsigned int *dq_error,
                                        int is_32b_phy ) 
{

  unsigned int lower_byte_addx, error;
  int shuffled, is_upper_16b ;
  int bit_pos;

  if (is_32b_phy==0) return -1;
 
  // swizzle to go from CPU to SCB byte ordering.
  addx ^= 0x1C; // cpu to scb

  // obtain lower 5 addx bits.  
  lower_byte_addx = addx & 0x1F;

  // addx shuffled flag
  shuffled = ((addx>>8)&1 ) ^ ((addx>>7)&1);

  // determine if we're on lower or upper 16 bits
  is_upper_16b = ((lower_byte_addx&0x10)==0) ? 1 : 0;
  is_upper_16b ^= shuffled;

  // Extract Even (P VDL ) and Odd (N VDL) errors
  dq_error[0] = (logical_err_map>>16);    // Even cycles (P) 
  dq_error[1] = (logical_err_map&0xFFFF); // Odd  cycles (N)
  if ( is_upper_16b)  {
    dq_error[0] <<= 16;
    dq_error[1] <<= 16;
  }

  return 0;
}
#endif

#if PHY_WIDTH == 16
// ** 32b PHY ONLY **
// Given a 32bit CPU data, and it's address associated,
// return the physical DQ mapped 32bit.
// 
// return   dual DQ error map for Even and Odd cycles (Even = P-vdl, Odd = N-vdl)
//            dq_error[0] = even
//            dq_error[1] = odd
//          -1 error
//           0 pass
//
// input:   logical err map (xor of exp and actual), and 32bit aligned address
//
//inline
int fdq_binary_dual_cycle (unsigned int addx, 
                             unsigned int logical_err_map,
                             unsigned int *dq_error,
                             int is_32b_phy ) 
{
  
//  unsigned int lower_byte_addx, error;
//  int shuffled, is_upper_16b ;
//  int bit_pos;
  
  if (is_32b_phy) return -1;
 
  // P
  dq_error[0] = (logical_err_map&0xFFFF); // even (P)
  // N
  dq_error[1] = (logical_err_map>>16); // Odd  cycles (N)


  return 0;
}
#endif
