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
    *  Filename: mde_rd_eye.h
    *
    *  Function: defines all mde_rd_eye related symbols
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
    * 
    * $Id::                                                       $:
    * $Rev::file =  : Global SVN Revision = 2057                  $:
    * 
 */
#ifndef MDE_RD_EYE_H
#define MDE_RD_EYE_H


#define MSA_SHMOO 2 // defined in memc_0_shmoo_inc.bss
#define VSSO_DATA_SIZE (16*8)

/*
  120109 CKD:
  Derive PHY register base from MEMC base
  Current known MEMC locations list:

  CHIP        MEMC offset        PHY offset

  7231        memc0: 003B0000    00006000
  7344        memc0: 00A00000    00006000
  7425        memc0: 003B0000    00006000
              memc1: 003C0000    00006000


 */
/*
  Define GET_FIELD and SET_FIELD without using BCHP header file
 */
//#define LOCAL_GET_FIELD(val,mask,shift) ((val & mask)>>shift)
//#define LOCAL_SET_FIELD(val,mask,shift,new) val=(((val&~mask)|(new<<shift)&mask))
#if defined(GET_FIELD) || defined(SET_FIELD)

#undef BRCM_ALIGN
#undef BRCM_BITS
#undef BRCM_MASK
#undef BRCM_SHIFT
#undef GET_FIELD
#undef SET_FIELD

#endif // if defined(GET_FIELD) || defined(SET_FIELD)


#define BRCM_ALIGN(c,r,f)   c##_##r##_##f##_ALIGN
#define BRCM_BITS(c,r,f)    c##_##r##_##f##_BITS
#define BRCM_MASK(c,r,f)    c##_##r##_##f##_MASK
#define BRCM_SHIFT(c,r,f)   c##_##r##_##f##_SHIFT

#define GET_FIELD(m,c,r,f) \
        ((((m) & BRCM_MASK(c,r,f)) >> BRCM_SHIFT(c,r,f)))

#define SET_FIELD(m,c,r,f,d) \
  ((m) = (((m) & ~BRCM_MASK(c,r,f)) | (((d) << BRCM_SHIFT(c,r,f)) & BRCM_MASK(c,r,f)))) 

#define MEMC_CFG_TYPE_0 0x003B0000
#define MEMC_CFG_TYPE_1 0x00A00000

#define HARD_CODED_SUN_TOP_PID 0x00404000

#define DBG_PRINT dbg_printf
#define BBS_PRINT dbg_printf

/*
  Revision History

  Version    Date     Reason

  5.0        20120717 Major upgrade: supports mode 1, 2, 3

  4.1        20120510 clean up
  4.0        20120509 Fixed get_optimal vref with pre-read-eye setup
  3.2        20120425 JIRA 52 - time elapse with mixed MEMC0 and MEMC1 plots
  3.1        20120423 JIRA 166 add P,N or X to mark R_VDL_P and R_VDL_N picked by SHMOO
  3.0        20120419 Integration with read-eye masked packaged
  2.2        20120418 Updated time elapsed version for 7425 and other chips
  2.1        20120416 Time elapsed version with interval suppport
  2.0        20120411 Major upgrade to support MEMC0 MIPS based read-eye
  1.1        20120118 First release

 */
#define RDI_VER_MAJ   05
#define RDI_VER_MIN   00
#define RDI_DATE_CODE 0x20120717



#define SIZE_VREF 64
#define SIZE_VDL  64
#define SIZE_RESULT_ARRAY  (SIZE_VREF*SIZE_VDL)

#define SIZE_INIT_SETTING  4

#define DBG_PRINT dbg_printf
#define BBS_PRINT dbg_printf

#define RDI_METHOD_DIS  1
#define RDI_METHOD_MIPS 2


#define DIS_START1_ADDR 0x20000000
#define DIS_END1_ADDR   0x50000000





#define BUFZONE         0x10000

#define LOW_LIMIT 8
#define HIGH_LIMIT (64-8)

///// MEMC_INFO default setting



/////

#define VREF_LOW_LIMIT        0
#define VREF_MID_POINT        32
#define VREF_HIGH_LIMIT       63

#define INCREMENT_VREF        1
#define DECREMENT_VREF        0xFFFFFFFF

#define RUN_ONCE_AND_EXIT     1
#define ALL_FAILED_THEN_EXIT  2
#define SOME_PASSED_THEN_EXIT 3

#define DEFAULT_OPTIMAL_MARGIN        3

////

#define MEMC0_RDI_MT_TOP     0xC0000000
#define MEMC0_RDI_MT_SIZE    0x00400000

#define MEMC1_RDI_MT_TOP     0xE0000000
#define MEMC1_RDI_MT_SIZE    0x00400000

#define MEMC0_RDI_DIS_TOP    0x30000000
#define MEMC0_RDI_DIS_SIZE   0x10000000
#define MEMC1_RDI_DIS_TOP    0x90000000
#define MEMC1_RDI_DIS_SIZE   0x10000000


#define CFG0_SDRAM_TOP  MEMC0_RDI_MT_TOP
#define CFG0_SDRAM_SIZE MEMC0_RDI_MT_SIZE
#define CFG0_DIS_TOP    MEMC0_RDI_DIS_TOP
#define CFG0_DIS_SIZE   MEMC0_RDI_DIS_SIZE

#ifdef MEMC_1_DDR_ENABLE

#define CFG1_SDRAM_TOP  MEMC1_RDI_MT_TOP
#define CFG1_SDRAM_SIZE MEMC1_RDI_MT_SIZE
#define CFG1_DIS_TOP    MEMC1_RDI_DIS_TOP
#define CFG1_DIS_SIZE   MEMC1_RDI_DIS_SIZE
#endif

#if PHY_WIDTH == 32
#define MT_ALL_FAILED 0xFFFFFFFF
#endif
#if PHY_WIDTH == 16
#define MT_ALL_FAILED 0x0000FFFF
#endif


// two WORD LANES, 32 register per word lane
#define VDL_BYTE_BIT_REGS_COUNT (16*2*2)

#define DIS0_TIMEOUT 0x2000000

#ifdef MEMC_1_DDR_ENABLE
#define ATRI_MEMC1_BASE 0x003C0000
#define ATRI_MEMC1_TOP  0xF0000000
#define ATRI_DIS1_TOP   0x90000000
#endif
#define ATRI_PROBE_MT_SIZE  0x00010000
#define ATRI_RD_EYE_MT_SIZE 0x00400000

#define ATRI_MEMC0_TOP 0xD0000000
#define ATRI_DIS0_TOP  0x30000000

#if BCHP_CHIP == 7344
#define ATRI_MEMC0_BASE 0x00A00000
#else
#define ATRI_MEMC0_BASE 0x003B0000
#endif

#define ATRI_DEFAULT_INTERVAL 300
#define ATRI_DEFAULT_LOOP     6
#define MAX_TIME              0x10000000

#define SVTM_DBGP(format, arg...) do {} while(0)

#define SIZE_NUM_EYES  2
#define K_P_EYE  0
#define K_N_EYE  1

// VREF index (0-63) mapped to VREF in mV
#define VREF_TO_MV(vref) ((1500 * (vref)) / 64)

// VREF in mV mapped to VREF index 
#define MV_TO_VREF(mv) ((64 * (mv) + 750) / 1500)

// STA offset for read eye ALL_RDI_VDL structure in SIDRAM
#define RDI_STA_AREA_START_OFFSET   0x1000

// STA invalid vdl
#define RDI_STA_VDL_INVALID   0xff

// STA area for ALL_RDI_VDL structure in SIDRAM
#ifdef BCHP_SID_BIGRAM_REG_START
#define RDI_STA_AREA_START (BCHP_SID_BIGRAM_REG_START + RDI_STA_AREA_START_OFFSET)
#endif

#define RDI_STA_FLASH_START     0x00100000
#define RDI_STA_SPIFLASH_START  0x000b8000

/*
  JIRA http://jira.broadcom.com/browse/SWMEMSYS-307

  This structure holds the start and end of the P and N of a DQ
  These values are used for the calculation of corrections to the
  original setting generated by SHMOO
  The values are filled in during the plotting of the eye.
  The last set of values collected will be used for the calculation.

*/
typedef struct {
  unsigned p_start;
  unsigned p_end;
  unsigned n_start;
  unsigned n_end;
} VDL_WINDOW;

#define TOTAL_DQ_COUNT 32

typedef struct {
  unsigned data_valid_flag;
  unsigned sta_vref;
  unsigned filler [62];
  // MEMC0 data starts at offset 0x100
  VDL_WINDOW all_dqs [PLAT_MAX_NUM_MEMC][TOTAL_DQ_COUNT];
} ALL_RDI_VDL;




typedef struct {
  unsigned char p_correction;
  unsigned char n_correction;
} DQ_CORRECTION;

#define DQ_BLK_FILLER (0x100 - (sizeof(DQ_CORRECTION)*TOTAL_DQ_COUNT))
#define DQ_BLK_FILLER_INTS (DQ_BLK_FILLER/4)

typedef struct {
  DQ_CORRECTION all_dq_corrections [TOTAL_DQ_COUNT];
  unsigned fillers [DQ_BLK_FILLER_INTS];

} DQ_CORRECTION_DATA_BLOCK;

typedef struct {
  unsigned char las_p_start;
  unsigned char las_p_end;
  unsigned char mre_p_start;
  unsigned char mre_p_end;
  unsigned char las_n_start;
  unsigned char las_n_end;
  unsigned char mre_n_start;
  unsigned char mre_n_end;
} DQ_DBG_INFO;

typedef struct {
  unsigned dq_correction_valid;
  unsigned memc0_ddr_freq;
  unsigned memc1_ddr_freq;
  unsigned odt;
  unsigned odv;

  unsigned filler [63-4];
  // Maker sure that all_dq_corrections start at offset 0x100
  //DQ_CORRECTION all_dq_corrections [PLAT_MAX_NUM_MEMC][TOTAL_DQ_COUNT];
  DQ_CORRECTION_DATA_BLOCK all_memc [PLAT_MAX_NUM_MEMC];
  DQ_DBG_INFO memc_dq_dbg_info [PLAT_MAX_NUM_MEMC][TOTAL_DQ_COUNT];
} ALL_DQ_CORRECTIONS;

typedef struct {
  unsigned chip_id;    // derived from SUN_TOP PID
  unsigned memc_id;    // 0, or 1 (valid only if chip has two MEMC
  unsigned memc_base;  // data base lookup
  unsigned phy_base;   // = memc_base + 0x6000
  unsigned sdram_top;  // data base lookup
  unsigned sdram_size; // in total bytes
  unsigned dis_top;
  unsigned dis_size;
  unsigned data_width; // either 32 or 16
  // extra info
  unsigned seed;
  unsigned start_vdl;
  unsigned end_vdl;
  unsigned start_vref;
  unsigned end_vref;
  unsigned clear_result;
  unsigned do_vref;
  unsigned qualify;
  unsigned show_raw_plot;
  // VREF optimal info

  unsigned vref_start;
  unsigned vref_end;
  unsigned vref_incr;
  unsigned exit_condition;

  unsigned vref_top;
  unsigned vref_bot;

  unsigned sta_vref;       // vref specified for STA collection
	
  // time lapsed support
  unsigned ok_to_plot;
  unsigned print_info;
   
  unsigned mode; // test mode 0=single eye, 1=dual eye

  unsigned results_array[PLAT_MAX_NUM_MEMC][SIZE_NUM_EYES][SIZE_RESULT_ARRAY];
  unsigned test_array[SIZE_NUM_EYES];

  unsigned time_interval_to_print;

  // BW calculation support
  unsigned total_bytes_read;
  unsigned total_time_in_ms;
  //
  void (*reg_writ) (unsigned addr, unsigned val);
  void (*send_char)(const char);
  int  (*printf)   (const char *templat,...);
} MEMC_INFO;

#define MEMC0 0
#define MEMC1 1
////
#endif //// MDE_RD_EYE_H
