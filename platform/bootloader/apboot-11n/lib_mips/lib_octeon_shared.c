// vim:set ts=4 sw=4:
//#define DEBUG
//#define PRINT_LMC_REGS
/***********************license start************************************
 * Copyright (c) 2004-2007  Cavium Networks (support@cavium.com). 
 * All rights reserved.
 * 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of Cavium Networks nor the names of
 *       its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.  
 * 
 * This Software, including technical data, may be subject to U.S.  export 
 * control laws, including the U.S.  Export Administration Act and its 
 * associated regulations, and may be subject to export or import regulations 
 * in other countries.  You warrant that You will comply strictly in all 
 * respects with all such regulations and acknowledge that you have the 
 * responsibility to obtain licenses to export, re-export or import the 
 * Software.  
 * 
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS 
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH 
 * RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY 
 * REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT 
 * DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR 
 * PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET 
 * POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT 
 * OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
 * 
 ***********************license end**************************************/

#ifdef __U_BOOT__
#include <common.h>
#include <octeon_boot.h>
#include <watchdog.h>
#else
/* Building on host for PCI boot
** Always use the pass 2 definitions for this module.
** Assume that pass 2 is a superset of pass 1 and
** determine actual chip revision at run time. */
#undef  OCTEON_MODEL
#define OCTEON_MODEL OCTEON_CN38XX_PASS2

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cvmx.h"


#define simple_strtoul strtoul
#endif

#include "lib_octeon_shared.h"
#include "configs/octeon_ebt3000_shared.h"
#include "configs/octeon_thunder_shared.h"
#include "octeon_eeprom_types.h"



/* Make a define similar to OCTEON_IS_MODEL that will work in this context.  This is runtime only.
** We can't do the normal runtime here since this code can run on the host.
** All functions using this must have a cpu_id variable set to the correct value */
#define octeon_is_cpuid(x)   (__OCTEON_IS_MODEL_COMPILE__(x, cpu_id))

#if (!CONFIG_OCTEON_SIM)

#ifdef DEBUG
#define DEBUG_DEFINED (1)
const char *ddr2_spd_strings[];
#define debug_print  printf
#else
#define DEBUG_DEFINED (0)
#define debug_print(...)
#endif

#if 0
#ifdef PRINT_LMC_REGS
static void 
octeon_print_lmc_regs(void)
{
    cvmx_lmc_comp_ctl_t lmc_comp_ctl;
    cvmx_lmc_ctl_t lmc_ctl;
    cvmx_lmc_ddr2_ctl_t ddr2_ctl;
    cvmx_lmc_mem_cfg0_t mem_cfg0;
    cvmx_lmc_mem_cfg1_t mem_cfg1;
    cvmx_lmc_delay_cfg_t lmc_delay;

    lmc_delay.u64 = octeon_read64(CVMX_LMC_DELAY_CFG);
    printf("delay = %Lx\n", lmc_delay.u64);

    mem_cfg0.u64 = octeon_read64(CVMX_LMC_MEM_CFG0);
    printf("mem_cfg0 = %Lx\n", mem_cfg0.u64);

    mem_cfg1.u64 = octeon_read64(CVMX_LMC_MEM_CFG1);
    printf("mem_cfg1 = %Lx\n", mem_cfg1.u64);

    lmc_ctl.u64 = octeon_read64(CVMX_LMC_CTL);
    printf("lmc_ctl = %Lx\n", lmc_ctl.u64);

    ddr2_ctl.u64 = octeon_read64(CVMX_LMC_DDR2_CTL);
    printf("ddr2_ctl = %Lx\n", ddr2_ctl.u64);

    lmc_comp_ctl.u64 = octeon_read64(CVMX_LMC_COMP_CTL);
    printf("comp_ctl = %Lx\n", lmc_comp_ctl.u64);

    lmc_comp_ctl.u64 = octeon_read64(CVMX_LMC_RODT_CTL);
    printf("rodt_ctl = %Lx\n", lmc_comp_ctl.u64);

    lmc_comp_ctl.u64 = octeon_read64(CVMX_LMC_WODT_CTL0);
    printf("wodt_ctl0 = %Lx\n", lmc_comp_ctl.u64);

    lmc_comp_ctl.u64 = octeon_read64(CVMX_LMC_WODT_CTL1);
    printf("wodt_ctl1 = %Lx\n", lmc_comp_ctl.u64);
}
#endif
#endif

#ifdef __U_BOOT__
static inline int ddr_verbose(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return (!!(gd->flags & GD_FLG_DDR_VERBOSE));
}
#else
static int ddr_verbose(void)
{
    return(getenv("ddr_verbose") != NULL);
}
#endif

static void ddr_print(const char *format, ...)
{
    if (ddr_verbose() || (DEBUG_DEFINED))
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

#define error_print  printf


extern uint64_t octeon_read64(uint64_t address);
extern void octeon_write64(uint64_t address, uint64_t val);
extern void octeon_delay_cycles(uint64_t cycles);

/* Return the revision field from the NPI_CTL_STATUS register.  This
** is used to distinguish pass1/pass2.
** Compile time checks should only be used as a last resort.
** pass1 == 0
** pass2 == 1
*/
static inline uint32_t octeon_get_chip_rev(void)
{
    cvmx_npi_ctl_status_t npi_ctl_status;
    npi_ctl_status.u64 = octeon_read64(CVMX_NPI_CTL_STATUS);
    return(npi_ctl_status.s.chip_rev);
}

#undef min
#define min(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x < __y) ? __x : __y; })

#undef max
#define max(X, Y)				\
	({ typeof (X) __x = (X), __y = (Y);	\
		(__x > __y) ? __x : __y; })

#undef round_divide
#define round_divide(X, Y)                              /* ***  Example  *** */ \
        ({   typeof (X) __x = (X), __y = (Y), __z;      /* 8/5 = 1.6 ==> 2   */ \
             __z = __x * 10 / __y;                      /* 8 * 10 / 5 = 16   */ \
             __x = __z / 10;                            /* 16 / 10 = 1       */ \
             __z = __z - (__x * 10);                    /* 16 - (1 * 10) = 6 */ \
             (__x + (typeof (__x))(__z >= 5));})        /* 1 + 1 = 2         */


/*
**
** +---------+----+----------------------+---------------+--------+------+-----+
** |  Dimm   |Rank|         Row          |      Col      |  Bank  |  Col | Bus |
** +---------+----+----------------------+---------------+--------+------+-----+
**          |   |                       |                    |
**          |   |                       |                    |
**          |  1 bit           LMC_MEM_CFG0[ROW_LSB]         |
**          |                                                |
**   LMC_MEM_CFG0[PBANK_LSB]                       (2 + LMC_DDR2_CTL[BANK8]) bits
**
**    Bus     = Selects the byte on the 72/144-bit DDR2 bus
**         Octeon: Bus = (3 + LMC_CTL[MODE128b]) bits
**         CN31XX: Bus = (3 - LMC_CTL[MODE32b]) bits
**         CN30XX: Bus = (1 + LMC_CTL[MODE32b]) bits
**    Col     = Column Address for the DDR2 part
**    Bank    = Bank Address for the DDR2 part
**    Row     = Row Address for the DDR2 part
**    Rank    = Optional Rank Address for dual-rank DIMM's
**              (present when LMC_CTL[BUNK_ENABLE] is set)
**    DIMM    = Optional DIMM address - 0, 1,or 2 bits
*/





#ifdef STATIC_DRAM_CONFIG_TABLE
extern const unsigned char STATIC_DRAM_CONFIG_TABLE [];
#endif


#ifndef CONFIG_ARUBA_OCTEON
static int read_spd(uint8_t dev_addr, uint8_t twsii_addr)
{
/*
** For memory configurations that don't incorporate Serial Presence
** Detect (SPD) the SPD EEPROM can be simulated using a static table
** in memory.  This capability is enabled by defining
** STATIC_DRAM_CONFIG_TABLE to point to the array that contains the
** simulated SPD data.  In this case the DRAM_SOCKET_CONFIGURATION
** table takes on a slightly different function.  Instead of providing
** the TWSII address of each SPD EEPROM a non-zero value indicates
** that the STATIC_DRAM_CONFIG_TABLE should be applied for that DIMM
** interface.  A zero value indicates that no DRAM is present on that
** DIMM interface.
*/
#ifdef STATIC_DRAM_CONFIG_TABLE
    if (dev_addr == 0) return 0;
    //printf("read_spd(%d) = 0x%02x\n", twsii_addr, STATIC_DRAM_CONFIG_TABLE[twsii_addr]);
    return STATIC_DRAM_CONFIG_TABLE[twsii_addr];
#else
    return octeon_twsi_read(dev_addr, twsii_addr);
#endif
}

static int validate_dimm(unsigned int spd_addr, int dimm)
{
    if (0 > read_spd(spd_addr, DDR2_SPD_NUM_ROW_BITS)
        && 0 > read_spd(spd_addr, DDR2_SPD_NUM_COL_BITS))
    {
        return(0);              /* Silently check for DIMM presence */
    }

    if ((read_spd(spd_addr, DDR2_SPD_MEM_TYPE) & 0xff) != 0x8) {
        return(0);              /* Silently check for DIMM presence */
    }

#ifdef STATIC_DRAM_CONFIG_TABLE
    ddr_print("Reading simulated SPD %p\n", STATIC_DRAM_CONFIG_TABLE);
#endif

    int spd_ecc   = ((read_spd(spd_addr, DDR2_SPD_CONFIG_TYPE) & 0x3f) == 2);
    int spd_rdimm = !!(read_spd(spd_addr, DDR2_SPD_DIMM_TYPE) & 0x11);

    ddr_print("DIMM %d: DDR2 %s, %s", dimm,
              (spd_rdimm ? "Registered" : "Unbuffered"), (spd_ecc ? "ECC" : "non-ECC"));

    if (ddr_verbose())
    {
        char part_number[90-73+2] = {0}; /* Initialize string terminator */

        int i;
        for (i = 0; i < (90-73+1); ++i) {
            part_number[i] = read_spd(spd_addr, i+73);
            //debug_print("spd[%d]: 0x%02x\n", i+73, part_number[i]);
        }
        ddr_print("  %s\n", part_number);
    }

#ifdef DEBUG
    debug_print("SPD dump for DIMM at TWSI address: 0x%x\n", spd_addr);
    int spd_byte_count = read_spd(spd_addr, DDR2_SPD_BYTES_PROGRAMMED);
    int spd_byte;
    for ( spd_byte = 0 ; spd_byte < min(spd_byte_count, 0x80); ++ spd_byte) {
        debug_print("%4d %-62s 0x%02x\n", spd_byte, ddr2_spd_strings[spd_byte],
               read_spd(spd_addr, spd_byte));
    }
#endif

    return(1);
}
#endif



static int divide_roundup(long dividend, long divisor)
{
    int quotent = dividend/divisor;
    return (dividend == (quotent * divisor)) ? quotent : quotent+1;
}



#ifndef CONFIG_ARUBA_OCTEON
static int lookup_cycle_time_psec (int spd_byte)
{
    static const char _subfield_b[] = { 00, 10, 20, 30, 40, 50, 60, 70,
                                        80, 90, 25, 33, 66, 75, 00, 00 };

    return (((spd_byte>>4)&0xf) * 1000) + _subfield_b[spd_byte&0xf] * 10;
}



static int lookup_delay_psec (int spd_byte)
{
    static const char _subfield_b[] =  { 0, 25, 50, 75 };

    return (((spd_byte>>2)&0x3f) * 1000) + _subfield_b[spd_byte&0x3] * 10;
}
#endif

static int lookup_refresh_rate_nsec (int spd_byte)
{
#define BASE_REFRESH 15625
    static const int _refresh[] =
        {
            (BASE_REFRESH),     /* 15.625 us */
            (BASE_REFRESH/4),   /* Reduced (.25x)..3.9us */
            (BASE_REFRESH/2),   /* Reduced (.5x)...7.8us */
            (BASE_REFRESH*2),   /* Extended (2x)...31.3us */
            (BASE_REFRESH*4),   /* Extended (4x)...62.5us */
            (BASE_REFRESH*8),   /* Extended (8x)...125us */
        };

    if ((spd_byte&0x7f) > 5) {
        error_print("Unsupported refresh rate: %#04x\n", spd_byte);
        return (-1);
    }

    return (_refresh[spd_byte&0x7f]);
}



#ifndef CONFIG_ARUBA_OCTEON
static int lookup_rfc_psec (int spd_trfc, int spd_trfc_ext)
{
    static const char _subfield_b[] =  { 0, 25, 33, 50, 66, 75 };
    int trfc = (spd_trfc * 1000) + _subfield_b[(spd_trfc_ext>>1)&0x7] * 10;
    return ((spd_trfc_ext&1) ? (trfc + (256*1000)) : trfc);
}


static int select_cas_latency (ulong tclk_psecs, int spd_cas_latency, unsigned *cycle_clx)
{
    int i;
    int max_cl = 7;

    while ( (((spd_cas_latency>>max_cl)&1) == 0) && (max_cl > 0) )
        --max_cl;

    if (max_cl == 0) {
        error_print("CAS Latency was not specified by SPD: %#04x\n", spd_cas_latency);
        return (-1);
    }

    /* Pick that fastest CL that is within spec. */
    for (i = 2; i >= 0; --i) {
        if ((spd_cas_latency>>(max_cl-i))&1) {
            ddr_print("CL%d Minimum Clock Rate                        : %6d ps\n", max_cl-i, cycle_clx[i]);

            if (tclk_psecs >= cycle_clx[i])
                return max_cl-i;
        }
    }

    error_print("WARNING!!!!!!: DDR Clock Rate (tCLK) exceeds DIMM specifications!!!!!!!!\n");
    return max_cl;
}
#endif

/*
** Calculate the board delay in quarter-cycles.  That value is used as
** an index into the delay parameter table.
*/
static ulong compute_delay_params(ulong board_skew, ulong tclk_psecs, int silo_qc_unsupported)
{
    int idx;
    static const ulong _params[] = {
        /* idx   board delay (cycles) tskw  silo_hc  silo_qc */
        /* ====  ==================== ====  =======  ======= */
        /* [ 0]     0.00 - 0.25 */     0 |   1<<8 |   0<<16,
        /* [ 1]     0.25 - 0.50 */     0 |   1<<8 |   1<<16,
        /* [ 2]     0.50 - 0.75 */     1 |   0<<8 |   0<<16,
        /* [ 3]     0.75 - 1.00 */     1 |   0<<8 |   1<<16,
        /* [ 4]     1.00 - 1.25 */     1 |   1<<8 |   0<<16,
        /* [ 5]     1.25 - 1.50 */     1 |   1<<8 |   1<<16,
        /* [ 6]     1.50 - 1.75 */     2 |   0<<8 |   0<<16,
        /* [ 7]     1.75 - 2.00 */     2 |   0<<8 |   1<<16,
        /* [ 8]     2.00 - 2.25 */     2 |   1<<8 |   0<<16,
        /* [ 9]     2.25 - 2.50 */     2 |   1<<8 |   1<<16,
        /* [10]     2.50 - 2.75 */     3 |   0<<8 |   0<<16,
        /* [11]     2.75 - 3.00 */     3 |   0<<8 |   1<<16,
        /* [12]     3.00 - 3.25 */     3 |   1<<8 |   0<<16,
        /* [13]     3.25 - 3.50 */     3 |   1<<8 |   1<<16,
        /* [14]     3.50 - 3.75 */     4 |   0<<8 |   0<<16,
    };


    /*
    ** Octeon pass 1 did not support the quarter-cycle delay setting.
    ** In that case round to the settings for the nearest half-cycle.
    ** This is effectively done by rounding up to the next
    ** quarter-cycle setting and ignoring the silo_qc bit when the
    ** quarter-cycle setting is not supported.
    */

    idx = ((board_skew*4) + (tclk_psecs*silo_qc_unsupported)) / tclk_psecs;
    return _params[min(idx, 14)];
}


static int encode_row_lsb(uint32_t cpu_id, int row_lsb, int ddr_interface_wide)
{
    int encoded_row_lsb;
    static const int _params[] = {
        /* 110: row_lsb = mem_adr[12] */ 6,
        /* 111: row_lsb = mem_adr[13] */ 7,
        /* 000: row_lsb = mem_adr[14] */ 0,
        /* 001: row_lsb = mem_adr[15] */ 1,
        /* 010: row_lsb = mem_adr[16] */ 2,
        /* 011: row_lsb = mem_adr[17] */ 3,
        /* 100: row_lsb = mem_adr[18] */ 4
        /* 101: RESERVED              */
    };

    int row_lsb_start = 12;
    if (octeon_is_cpuid(OCTEON_CN30XX))
        row_lsb_start = 12;
    else if (octeon_is_cpuid(OCTEON_CN31XX))
        row_lsb_start = 12;
    else if (octeon_is_cpuid(OCTEON_CN50XX))
        row_lsb_start = 12;
    else if (octeon_is_cpuid(OCTEON_CN38XX))
        row_lsb_start = 14;
    else if (octeon_is_cpuid(OCTEON_CN58XX))
        row_lsb_start = 14;
    else 
        printf("ERROR: Unsupported Octeon model: 0x%x\n", cpu_id);

    if (octeon_is_cpuid(OCTEON_CN50XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN31XX))
    {
        encoded_row_lsb      = _params[row_lsb  + ddr_interface_wide - row_lsb_start];
        debug_print("_params[%d] = %d\n", (row_lsb  + ddr_interface_wide - row_lsb_start), encoded_row_lsb);
    }
    else
    {
        encoded_row_lsb      = row_lsb + ddr_interface_wide - row_lsb_start ;
    }
    return encoded_row_lsb;
}


static int encode_pbank_lsb(int cpu_id, int pbank_lsb, int ddr_interface_wide)
{
    int encoded_pbank_lsb;

    static const int _params_cn31xx[] = {
        14, /* 1110:pbank[1:0] = mem_adr[26:25]    / rank = mem_adr[24] (if bunk_ena) */
        15, /* 1111:pbank[1:0] = mem_adr[27:26]    / rank = mem_adr[25] (if bunk_ena) */
         0, /* 0000:pbank[1:0] = mem_adr[28:27]    / rank = mem_adr[26] (if bunk_ena) */
         1, /* 0001:pbank[1:0] = mem_adr[29:28]    / rank = mem_adr[27]      "        */
         2, /* 0010:pbank[1:0] = mem_adr[30:29]    / rank = mem_adr[28]      "        */
         3, /* 0011:pbank[1:0] = mem_adr[31:30]    / rank = mem_adr[29]      "        */
         4, /* 0100:pbank[1:0] = mem_adr[32:31]    / rank = mem_adr[30]      "        */
         5, /* 0101:pbank[1:0] = mem_adr[33:32]    / rank = mem_adr[31]      "        */
         6, /* 0110:pbank[1:0] ={1'b0,mem_adr[33]} / rank = mem_adr[32]      "        */
         7, /* 0111:pbank[1:0] ={2'b0}             / rank = mem_adr[33]      "        */
            /* 1000-1101: RESERVED                                                    */
    };

    int pbank_lsb_start = 0;
    if (octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
        pbank_lsb_start = 25;
    else if (octeon_is_cpuid(OCTEON_CN31XX))
        pbank_lsb_start = 25;
    else if (octeon_is_cpuid(OCTEON_CN38XX))
        pbank_lsb_start = 27;
    else if (octeon_is_cpuid(OCTEON_CN58XX))
        pbank_lsb_start = 27;
    else 
        printf("ERROR: Unsupported Octeon model: 0x%x\n", cpu_id);
    if (octeon_is_cpuid(OCTEON_CN50XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN31XX))
    {
        encoded_pbank_lsb      = _params_cn31xx[pbank_lsb + ddr_interface_wide - pbank_lsb_start];
        debug_print("_params[%d] = %d\n", (pbank_lsb + ddr_interface_wide - pbank_lsb_start), encoded_pbank_lsb);
    }
    else
    {
        encoded_pbank_lsb      = pbank_lsb + ddr_interface_wide - pbank_lsb_start;
    }
    return encoded_pbank_lsb;
}

//#define PRINT_LMC_REGS
#ifdef PRINT_LMC_REGS
void print_addr_name(uint64_t addr, char *name)
{
    printf("%20s(0x%Lx): 0x%Lx\n", name, (unsigned long long)addr, (unsigned long long)octeon_read64(addr));
}
#define PRINT_REG(X)     print_addr_name(X,#X)
static void octeon_print_lmc_regs(uint32_t cpu_id)
{
    PRINT_REG(CVMX_LMC_CTL);
    PRINT_REG(CVMX_LMC_CTL1);
    PRINT_REG(CVMX_LMC_DDR2_CTL);
    PRINT_REG(CVMX_LMC_RODT_CTL);

    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        PRINT_REG(CVMX_LMC_WODT_CTL0);
        PRINT_REG(CVMX_LMC_WODT_CTL1);
    }
    else
    {
        PRINT_REG(CVMX_LMC_WODT_CTL);
    }
    PRINT_REG(CVMX_LMC_DELAY_CFG);
    PRINT_REG(CVMX_LMC_MEM_CFG0);
    PRINT_REG(CVMX_LMC_MEM_CFG1);
    if (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        PRINT_REG(CVMX_LMC_RODT_COMP_CTL);
        PRINT_REG(CVMX_LMC_PLL_CTL);
    }
}
#endif

#ifdef CONFIG_ARUBA_OCTEON
/* Default ODT config must disable ODT */
/* Must be const (read only) so that the structure is in flash */
const dimm_odt_config_t disable_odt_config[] = {
    /* DIMMS   ODT_ENA ODT_MASK ODT_MASK1 QS_DIC RODT_CTL DIC */ \
    /* =====   ======= ======== ========= ====== ======== === */ \
    /*   1 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
    /*   2 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
    /*   3 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
    /*   4 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
};
int init_octeon_dram_interface(uint32_t cpu_id,
							   ddr_configuration_t *ddr_configuration,
                               uint32_t ddr_hertz,
                               uint32_t cpu_hertz,
                               uint32_t ddr_ref_hertz,
                               int board_type,
                               int board_rev_maj,
                               int board_rev_min
                               )
{
    uint32_t board_delay = ddr_configuration->ddr_board_delay; /* Get default board delay */
//    const dimm_odt_config_t *odt_1rank_config = ddr_configuration->odt_1rank_config;
//   const dimm_odt_config_t *odt_2rank_config = ddr_configuration->odt_2rank_config;
//    dimm_config_t *dimm_config_table = ddr_configuration->dimm_config_table;

	int refresh;
    int fatal_error = 0;        /* Accumulate and report all the errors before giving up */
    int row_bits = 13;
    int col_bits = __DDR_NCOLS;
    int num_banks = 4;
    int num_ranks = 1;
    int dram_width = 16;
    int dimm_count = 1;	// not really, but calculations below will be correct
//    int spd_rdimm       = 0;
//    int spd_burst_length = 0xc;
    int safe_ddr_flag = 0;
    int ctl_fprch2 = 1;
    int ctl_qs_dic = __DDR_QS_DIC;
    int ctl_dic = 0;
    int ctl_odt_ena = 0;
	int trrd = __DDR_TRRD;	// changed to 10ns per Jerry
	int cas_latency = 4;	// for 533Mhz, per Jerry
	int tmrd = 2;			// for 533, per Jerry
	int trfc = __DDR_TRFC;	// ps XXX
	int trp = 15000;
	int twtr = 7500;
	int trcd = 15000;
	int twr = 15000;
	int tras = __DDR_TRAS;
	int ddr_interface_wide = 1;	// 32-bit
	int ctl_silo_qc;
	int ctl_silo_hc;
    uint32_t mem_size_mbytes = 0;
    unsigned int ctl_odt_mask = __DDR_CTL_ODT_MASK, ctl_rodt_ctl = 0;
    unsigned int ctl_odt_mask1 = 0;
    ulong tclk_psecs = (ulong) (1000*1000*1000)/(ddr_hertz/1000); /* Clock in psecs */
    ulong eclk_psecs = (ulong) (1000*1000*1000)/(cpu_hertz/1000); /* Clock in psecs */
	int ctl_tskw;

    cvmx_lmc_comp_ctl_t lmc_comp_ctl;
    cvmx_lmc_ctl_t lmc_ctl;
    cvmx_lmc_ddr2_ctl_t ddr2_ctl;
    cvmx_lmc_mem_cfg0_t mem_cfg0;
    cvmx_lmc_mem_cfg1_t mem_cfg1;
    cvmx_lmc_delay_cfg_t lmc_delay;
//    cvmx_lmc_rodt_comp_ctl_t lmc_rodt_comp_ctl;

    initialize_ddr_clock(cpu_id, cpu_hertz, ddr_hertz, ddr_ref_hertz);

ddr_print("cpu_id %x, cpu_hertz %lu, ddr_hertz %lu, ddr_ref_hertz %lu\n", cpu_id, cpu_hertz, ddr_hertz, ddr_ref_hertz);
ddr_print("TCLK (ps) %u, ECLK (ps) %u\n", tclk_psecs, eclk_psecs);
	if (octeon_gpio_value(GPIO_NOT_3005) == 0
	    && !__octeon_is_model_runtime__(OCTEON_CN50XX)) {
		// use the GPIO pin to determine if this is a 3010/3005
		ddr_interface_wide = 0;
		col_bits = 10;
	}

	if (__octeon_is_model_runtime__(OCTEON_CN50XX)) {
		// override for 34D, per Jerry
		board_delay = DDR_BOARD_DELAY_5010;
		tmrd = 3;
		cas_latency = 5;
		if (ARUBA_5010_DDR_MHZ == 266) {
			// new settings for 533Mhz DRAM on Moscato
			tmrd = 3;
			cas_latency = 4;
		}
	}

	ddr_print("Board delay                                   : %6d ps\n", board_delay);
	if (board_delay == 0) {
		error_print("Error!!! Board delay is not set\n");
		++fatal_error;
	}

    /*
    ** Check that values are within some theoretical limits.
    ** col_bits(min) = row_lsb(min) - bank_bits(max) - bus_bits(max) = 14 - 3 - 4 = 7
    ** col_bits(max) = row_lsb(max) - bank_bits(min) - bus_bits(min) = 18 - 2 - 3 = 13
    */
    if ((col_bits > 13) || (col_bits < 7)) {
        error_print("Unsupported number of Col Bits: %d\n", col_bits);
        ++fatal_error;
    }

    /*
    ** Check that values are within some theoretical limits.
    ** row_bits(min) = pbank_lsb(min) - row_lsb(max) - rank_bits = 26 - 18 - 1 = 7
    ** row_bits(max) = pbank_lsb(max) - row_lsb(min) - rank_bits = 33 - 14 - 1 = 18
    */
    if ((row_bits > 18) || (row_bits < 7)) {
        error_print("Unsupported number of Row Bits: %d\n", row_bits);
        ++fatal_error;
    }

    int bank_bits = 0;
    if (num_banks == 8)
        bank_bits = 3;
    else if (num_banks == 4)
        bank_bits = 2;


    int bunk_enable = (num_ranks > 1);

    int column_bits_start = 1;
    if (octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
       column_bits_start = 1;
    else if (octeon_is_cpuid(OCTEON_CN31XX))
       column_bits_start = 2;
    else if (octeon_is_cpuid(OCTEON_CN38XX))
       column_bits_start = 3;
    else if (octeon_is_cpuid(OCTEON_CN58XX))
       column_bits_start = 3;
    else 
       printf("ERROR: Unsupported Octeon model: 0x%x\n", cpu_id);

    int row_lsb = column_bits_start + col_bits + bank_bits;
    debug_print("row_lsb = column_bits_start + col_bits + bank_bits = %d\n", row_lsb);


    int pbank_lsb = row_lsb + row_bits + bunk_enable;
    debug_print("pbank_lsb = row_lsb + row_bits + bunk_enable = %d\n", pbank_lsb);


	/* Interface width is not a function of dimm_count so add it in
	   for this calculation. */
	mem_size_mbytes =  dimm_count * ((1ull << (pbank_lsb+ddr_interface_wide)) >> 20);

    ddr_print("row bits: %d, col bits: %d, banks: %d, ranks: %d, dram width: %d, size: %d MB\n",
              row_bits, col_bits, num_banks, num_ranks, dram_width, mem_size_mbytes);

    ulong delay_params = compute_delay_params(board_delay, tclk_psecs, (cpu_id == OCTEON_CN38XX_PASS1));

#if CONFIG_RAM_RESIDENT
    /* If we are booting from RAM, the DRAM controller is already set up.  Just return the
    ** memory size */
    return mem_size_mbytes;
#endif
    if (octeon_is_cpuid(OCTEON_CN58XX)) {
        /* These registers are in the CN58XX and they must be zero.
           Read ODT is enabled differently for CN58XX */
        ctl_odt_ena	= 0;    /* Must be zero */
        ctl_rodt_ctl	= 0;    /* Must be zero */
    }

    ctl_tskw    = (delay_params>>0) & 0xff;
    ctl_silo_hc = (delay_params>>8) & 1;
    ctl_silo_qc = (delay_params>>16) & 1;

#ifdef __DDR_SILO_QC
	ctl_silo_qc = __DDR_SILO_QC;
#endif
/* ----------------------------------------------------------------------------- */

    /*
     * DRAM Controller Initialization
     * The reference-clock inputs to the LMC (DDR2_REF_CLK_*) should be stable
     * when DCOK asserts (refer to Section 26.3). DDR_CK_* should be stable from that
     * point, which is more than 200 us before software can bring up the main memory
     * DRAM interface. The generated DDR_CK_* frequency is four times the
     * DDR2_REF_CLK_* frequency.
     * To initialize the main memory and controller, software must perform the following
     * steps in this order:
     *
     * 1. Write LMC_CTL with [DRESET] = 1, [PLL_BYPASS] = user_value, and
     * [PLL_DIV2] = user_value.
     */

    lmc_ctl.u64               = 0;

    lmc_ctl.s.dic               = ctl_dic;
    lmc_ctl.s.qs_dic            = ctl_qs_dic;
    lmc_ctl.s.tskw              = ctl_tskw;	// XXX
    lmc_ctl.s.sil_lat           = safe_ddr_flag ? 2 : 1;
    lmc_ctl.s.bprch             = ctl_silo_hc & ctl_silo_qc;
    lmc_ctl.s.fprch2            = ctl_fprch2;
    lmc_ctl.cn30xx.mode32b      = ddr_interface_wide; /* 32-bit == Wide */
    lmc_ctl.s.inorder_mrf       = 0;
    lmc_ctl.s.inorder_mwf       = 0;
    lmc_ctl.s.r2r_slot          = safe_ddr_flag ? 1 : 0;
    lmc_ctl.s.rdimm_ena         = 0;
    lmc_ctl.s.max_write_batch   = 0xf;
    lmc_ctl.s.xor_bank          = 1;
    lmc_ctl.s.ddr__pctl         = 0;
    lmc_ctl.s.ddr__nctl         = 0;

#if CONFIG_ARUBA_OCTEON
    lmc_ctl.cn31xx.pll_div2          = 0;
	if (!__octeon_is_model_runtime__(OCTEON_CN50XX)) {
		lmc_ctl.s.pll_div2          = 1;
	}
#endif
    lmc_ctl.cn31xx.dreset            = 1;

    octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);

    /* 2. Read L2D_BST0 and wait for the result. */
    octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

    /* 3. Wait 10 us (LMC_CTL[PLL_BYPASS] and LMC_CTL[PLL_DIV2] must not
     * transition after this) */
    octeon_delay_cycles(6000);  /* Wait 10 us */

    /* ---------------------------------------------------------------------------- */

    int ctl_twr = divide_roundup(twr, tclk_psecs) - 1;
    ctl_twr = min(ctl_twr, 5);
    ctl_twr = max(ctl_twr, 1);

    int ctl_ddr_eof;
    if (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
        int ctl_ddr_eof_ratio;
        ctl_ddr_eof_ratio = tclk_psecs*10/eclk_psecs;
            ctl_ddr_eof = 3;
        if (ctl_ddr_eof_ratio >= 15)
            ctl_ddr_eof = 2;
        if (ctl_ddr_eof_ratio >= 20)
            ctl_ddr_eof = 1;
        if (ctl_ddr_eof_ratio >= 30)
            ctl_ddr_eof = 0;
    }
    else {
        ctl_ddr_eof = min( 3, max( 1, divide_roundup(tclk_psecs, eclk_psecs)));
    }
    ddr_print("DDR EOF                                   : %6d\n", ctl_ddr_eof);

    ddr2_ctl.u64 = 0;
    ddr2_ctl.s.ddr2       = 1;
    ddr2_ctl.s.dll90_byp  = 0;
    if (octeon_is_cpuid(OCTEON_CN38XX))
        ddr2_ctl.cn58xx.rdqs       = 0;
    if (octeon_is_cpuid(OCTEON_CN58XX))
        ddr2_ctl.cn58xx.rdqs       = 0;
    if (octeon_is_cpuid(OCTEON_CN50XX))
        ddr2_ctl.cn50xx.rdqs       = 0;
    ddr2_ctl.s.dll90_vlu  = 0;
    ddr2_ctl.s.qdll_ena   = 0;
    ddr2_ctl.s.crip_mode  = 0;

    ddr2_ctl.s.ddr2t      = __DDR2T; // safe_ddr_flag ? 1 : (spd_rdimm == 0);

    ddr2_ctl.s.ddr_eof    = ctl_ddr_eof;
    ddr2_ctl.s.silo_hc    = ctl_silo_hc;//XXX
    ddr2_ctl.s.twr        = 3;//ctl_twr; XXX
    ddr2_ctl.s.bwcnt      = 0;
	ddr2_ctl.s.pocas      = 0;
	ddr2_ctl.s.addlat     = 0;

    ddr2_ctl.s.odt_ena    = ctl_odt_ena;
    ddr2_ctl.s.burst8     = 1;

    ddr2_ctl.s.bank8      = 0;


    /* 4. Write LMC_DDR2_CTL[QDLL_ENA] = 1. */ /* Is it OK to write 0 first? */

    octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);
    octeon_read64(CVMX_LMC_DDR2_CTL);
    ddr2_ctl.s.qdll_ena   = 1;
    octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);
    octeon_read64(CVMX_LMC_DDR2_CTL);


    octeon_delay_cycles(2000);   // must be 200 dclocks

    /* 5. Read L2D_BST0 and wait for the result. */
    octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

    /* 6. Wait 10 us (LMC_DDR2_CTL[QDLL_ENA] must not transition after this) */
    octeon_delay_cycles(2000);  /* Wait 10 us */

    /* ---------------------------------------------------------------------------- */

    /*
     * 7. Write LMC_CTL[DRESET] = 0 (at this point, the DCLK is running and the
     * memory controller is out of reset)
     */

    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))	// SCA; added 50xx
    {
        lmc_ctl.cn31xx.dreset            = 0;
        octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);
        octeon_delay_cycles(2000);   // must be 200 dclocks
    }

    /* ----------------------------------------------------------------------------- */
    /*
     * Next, boot software must re-initialize the LMC_MEM_CFG1, LMC_CTL, and
     * LMC_DDR2_CTL CSRs, and also the LMC_WODT_CTL and LMC_RODT_CTL
     * CSRs. Refer to Sections 2.3.4, 2.3.5, and 2.3.7 regarding these CSRs (and
     * LMC_MEM_CFG0).
     */

    /* Configure ODT */
    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        octeon_write64(CVMX_LMC_WODT_CTL0, ctl_odt_mask);
        octeon_write64(CVMX_LMC_WODT_CTL1, ctl_odt_mask1);
    }
    else
        octeon_write64(CVMX_LMC_WODT_CTL, ctl_odt_mask);


    octeon_write64(CVMX_LMC_RODT_CTL, ctl_rodt_ctl);

    octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);

    /* ----------------------------------------------------------------------------- */

    if (octeon_is_cpuid(OCTEON_CN30XX) || (cpu_id == OCTEON_CN38XX_PASS3) || octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        lmc_delay.u64 = 0;

#ifdef LMC_DELAY_CLK  /* Defined per board */
        lmc_delay.s.clk  = LMC_DELAY_CLK;
        lmc_delay.s.cmd  = LMC_DELAY_CMD;
        lmc_delay.s.dq   = LMC_DELAY_DQ;
#endif
#ifdef LMC_DELAY_CLK_3010  /* Defined per board */
		if (!octeon_is_cpuid(OCTEON_CN50XX)) {
			lmc_delay.s.clk  = LMC_DELAY_CLK_3010;
			lmc_delay.s.cmd  = LMC_DELAY_CMD_3010;
			lmc_delay.s.dq   = LMC_DELAY_DQ_3010;
		}
#endif

        octeon_write64(CVMX_LMC_DELAY_CFG, lmc_delay.u64);

        ddr_print("delay_clk                                     : %6d\n", lmc_delay.s.clk);
        ddr_print("delay_cmd                                     : %6d\n", lmc_delay.s.cmd);
        ddr_print("delay_dq                                      : %6d\n", lmc_delay.s.dq);
    }

    if (1 /* ddr_verbose() */ && (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)))
    {
        cvmx_lmc_ctl1_t lmc_ctl1;
        cvmx_lmc_pll_ctl_t pll_ctl;
        uint64_t clkf, clkr, pll_MHz, calculated_ddr_hertz;
        int _en = 0;

        lmc_ctl1.u64 = octeon_read64(CVMX_LMC_CTL1);
        ddr_print("sil_mode                                      : %6d\n", lmc_ctl1.cn58xx.sil_mode);

        pll_ctl.u64 = octeon_read64(CVMX_LMC_PLL_CTL);
        clkf = pll_ctl.cn58xx.clkf;
        clkr = pll_ctl.cn58xx.clkr;

        ddr_print("DDR Fixed Reference Clock Hertz               : %8d\n", ddr_ref_hertz);
        ddr_print("clkf                                          : %6d\n", clkf);
        ddr_print("clkr                                          : %6d\n", clkr);
        ddr_print("fasten_n                                      : %6d\n", pll_ctl.cn50xx.fasten_n);

        if (pll_ctl.cn58xx.en2 == 1) {
            ddr_print("EN2                                           : %6d\n", 1); _en = 2;
        }
        if (pll_ctl.cn58xx.en4 == 1) {
            ddr_print("EN4                                           : %6d\n", 1); _en = 4;
        }
        if (pll_ctl.cn58xx.en6 == 1) {
            ddr_print("EN6                                           : %6d\n", 1); _en = 6;
        }
        if (pll_ctl.cn58xx.en8 == 1) {
            ddr_print("EN8                                           : %6d\n", 1); _en = 8;
        }
        if (pll_ctl.cn58xx.en12 == 1) {
            ddr_print("EN12                                          : %6d\n", 1); _en = 12;
        }
        if (pll_ctl.cn58xx.en16 == 1) {
            ddr_print("EN16                                          : %6d\n", 1); _en = 16;
        }

        pll_MHz = ddr_ref_hertz * (clkf+1) / (clkr+1) / 1000000;

        ddr_print("LMC PLL Frequency                             : %6d MHz\n", pll_MHz);

        calculated_ddr_hertz = ddr_ref_hertz * (clkf + 1) / ((clkr + 1) * _en);
        ddr_print("Calculated DClk Frequency                     : %8d Hz\n", calculated_ddr_hertz);
    }

    /* ----------------------------------------------------------------------------- */

    /* ---------------------------------------------------------------------------- */

    int cfg1_tras;
    cfg1_tras =  divide_roundup(tras, tclk_psecs);

    int cfg1_trcd = divide_roundup(trcd, tclk_psecs);

    int cfg1_twtr = divide_roundup(twtr, tclk_psecs);

    /* Plus 1 for 8-bank parts */
    int cfg1_trp = divide_roundup(trp, tclk_psecs) + (num_banks == 8);

    int cfg1_trfc =  divide_roundup(trfc, (4*tclk_psecs));

    int cfg1_tmrd = tmrd; /* Always at least 2. 3 supports higher speeds */

    int cfg1_trrd = divide_roundup(trrd, tclk_psecs);

    mem_cfg1.u64 = 0;
    mem_cfg1.s.tras     = cfg1_tras;
    /* In 2T mode, make this register TRCD-1, not going below 2. */
    mem_cfg1.s.trcd     = max(2, (ddr2_ctl.s.ddr2t ? (cfg1_trcd - 1) : cfg1_trcd));
    mem_cfg1.s.twtr     = cfg1_twtr;
    mem_cfg1.s.trp      = cfg1_trp;
    mem_cfg1.s.trfc     = cfg1_trfc;
    mem_cfg1.s.tmrd     = cfg1_tmrd;
    mem_cfg1.s.caslat   = cas_latency;
    mem_cfg1.s.trrd     = cfg1_trrd;

    octeon_write64(CVMX_LMC_MEM_CFG1, mem_cfg1.u64);

    /* ---------------------------------------------------------------------------- */

    lmc_comp_ctl.u64 = 0;
    lmc_comp_ctl.s.pctl_dat = 0x0;
    lmc_comp_ctl.s.pctl_clk = 0x0;
    lmc_comp_ctl.s.pctl_csr = 0xf;
    lmc_comp_ctl.s.nctl_dat = 0x0;
    lmc_comp_ctl.s.nctl_clk = 0x0;
    lmc_comp_ctl.s.nctl_csr = 0xf;

    octeon_write64(CVMX_LMC_COMP_CTL, lmc_comp_ctl.u64);

    /* ---------------------------------------------------------------------------- */

    /*
     * Finally, software must write the LMC_MEM_CFG0 register with
     * LMC_MEM_CFG0[INIT_START] = 1. At that point, CN31XX hardware initiates
     * the standard DDR2 initialization sequence shown in Figure 2.
     */

	refresh = lookup_refresh_rate_nsec(0x82);

	ddr_print("refresh calculated to be %u\n", refresh);

    int ref_int = (refresh*1000) / (tclk_psecs*512);
	ddr_print("ref_int calculated to be %u\n", ref_int);

    ref_int = __DDR_REFINT;	// XXX; override

    mem_cfg0.u64 = 0;

    mem_cfg0.s.init_start   = 0;
    mem_cfg0.s.ecc_ena      = 0;

    mem_cfg0.s.row_lsb      = encode_row_lsb(cpu_id, row_lsb, ddr_interface_wide);
    mem_cfg0.s.bunk_ena     = 0; //bunk_enable;

	mem_cfg0.s.silo_qc      = ctl_silo_qc;

    mem_cfg0.s.pbank_lsb    = encode_pbank_lsb(cpu_id, pbank_lsb, ddr_interface_wide);
    mem_cfg0.s.ref_int      = ref_int;
	mem_cfg0.s.tcl          = 0; /* Has no effect on the controller's behavior */
    mem_cfg0.s.intr_sec_ena = 0;
    mem_cfg0.s.intr_ded_ena = 0;
    mem_cfg0.s.sec_err      = 0;
    mem_cfg0.s.ded_err      = 0;
    mem_cfg0.s.reset        = 0;

    ddr_print("bunk_enable                                   : %6d\n", mem_cfg0.s.bunk_ena);
    ddr_print("burst8                                        : %6d\n", ddr2_ctl.s.burst8);
	ddr_print("ddr2t                                         : %6d\n", ddr2_ctl.s.ddr2t);

    ddr_print("tskw                                          : %6d\n", lmc_ctl.s.tskw);
	ddr_print("refint                                        : %6d\n", mem_cfg0.s.ref_int);
    ddr_print("silo_hc                                       : %6d\n", ddr2_ctl.s.silo_hc);
    ddr_print("pocas                                         : %6d\n", ddr2_ctl.s.pocas);
    ddr_print("addlat                                        : %6d\n", ddr2_ctl.s.addlat);

	ddr_print("silo_qc                                       : %6d\n", mem_cfg0.s.silo_qc);

    ddr_print("sil_lat                                       : %6d\n", lmc_ctl.s.sil_lat);
    ddr_print("r2r_slot                                      : %6d\n", lmc_ctl.s.r2r_slot);
	ddr_print("qs_dic                                        : %6d\n", lmc_ctl.s.qs_dic);
	ddr_print("dic                                           : %6d\n", lmc_ctl.s.dic);

	ddr_print("odt_ena                                       : %6d\n", ddr2_ctl.s.odt_ena);
	ddr_print("twr                                           : %6d\n", ddr2_ctl.s.twr);

	ddr_print("ctl_odt_mask                                  : %08x\n", ctl_odt_mask);
	WATCHDOG_RESET();

	ddr_print("tras                                          : %6d\n", mem_cfg1.s.tras);
	ddr_print("trcd                                          : %6d\n", mem_cfg1.s.trcd);
	ddr_print("twtr                                          : %6d\n", mem_cfg1.s.twtr);
	ddr_print("trp                                           : %6d\n", mem_cfg1.s.trp);
	if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX))
		ddr_print("ctl_odt_mask1                                 : %08x\n", ctl_odt_mask1);
	ddr_print("ctl_rodt_ctl                                  : %08x\n", ctl_rodt_ctl);
#if 0
    if (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
        lmc_rodt_comp_ctl.u64 = 0;

        if (odt_config[odt_idx].odt_ena == 1) { /* Weak Read ODT */
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 3;
            lmc_rodt_comp_ctl.s.nctl = 1;
        }

        if (odt_config[odt_idx].odt_ena == 2) { /* Strong Read ODT */
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 7;
            lmc_rodt_comp_ctl.s.nctl = 2;
        }

        /* Weak Read ODT */
        if (ddr_verbose() && (((s = getenv("ddr_rodt")) != NULL) && (strcmp(s,"weak") == 0))) {
            error_print("Parameter found in environment. ddr_rodt = %s\n", s);
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 3;
            lmc_rodt_comp_ctl.s.nctl = 1;
        }

        /* Strong Read ODT */
        if (ddr_verbose() && (((s = getenv("ddr_rodt")) != NULL) && (strcmp(s,"strong") == 0))) {
            error_print("Parameter found in environment. ddr_rodt = %s\n", s);
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 7;
            lmc_rodt_comp_ctl.s.nctl = 2;
        }

        octeon_write64(CVMX_LMC_RODT_COMP_CTL, lmc_rodt_comp_ctl.u64);

        ddr_print("RODT enable                                   : %6d\n", lmc_rodt_comp_ctl.s.enable);
        ddr_print("RODT pctl                                     : %6d\n", lmc_rodt_comp_ctl.s.pctl);
        ddr_print("RODT nctl                                     : %6d\n", lmc_rodt_comp_ctl.s.nctl);

        if (octeon_is_cpuid(OCTEON_CN56XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
            /* CN56XX has a 5-bit copy of the compensation values */
            cvmx_lmcx_pll_status_t pll_status;
            pll_status.u64 = cvmx_read_csr(CVMX_LMC_PLL_STATUS);
            ddr_print("DDR PMOS control                              : %6d\n", pll_status.s.ddr__pctl);
            ddr_print("DDR NMOS control                              : %6d\n", pll_status.s.ddr__nctl);
        } else {
            cvmx_lmc_ctl_t tmp_ctl;
            tmp_ctl.u64 = cvmx_read_csr(CVMX_LMC_CTL);
            ddr_print("DDR PMOS control                              : %6d\n", tmp_ctl.s.ddr__pctl);
            ddr_print("DDR NMOS control                              : %6d\n", tmp_ctl.s.ddr__nctl);
        }
    }
#endif
    octeon_write64(CVMX_LMC_MEM_CFG0, mem_cfg0.u64);
    mem_cfg0.u64 = octeon_read64(CVMX_LMC_MEM_CFG0);

    mem_cfg0.s.init_start   = 1;
    octeon_write64(CVMX_LMC_MEM_CFG0, mem_cfg0.u64);
    octeon_read64(CVMX_LMC_MEM_CFG0);

    /* ---------------------------------------------------------------------------- */

#ifdef PRINT_LMC_REGS
    printf("##################################################################\n");
    printf("New lmc config:\n");
    octeon_print_lmc_regs(cpu_id);
#endif

    return(mem_size_mbytes);
}
#else
/* Default ODT config must disable ODT */
/* Must be const (read only) so that the structure is in flash */
const dimm_odt_config_t disable_odt_config[] = {
    /* DIMMS   ODT_ENA ODT_MASK ODT_MASK1 QS_DIC RODT_CTL DIC */ \
    /* =====   ======= ======== ========= ====== ======== === */ \
    /*   1 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
    /*   2 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
    /*   3 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
    /*   4 */ {   0,    0x0000,   0x0000,    0,   0x0000,  0  },  \
};
/* Memory controller setup function */
int init_octeon_dram_interface(uint32_t cpu_id,
                               ddr_configuration_t *ddr_configuration,
                               uint32_t ddr_hertz,
                               uint32_t cpu_hertz,
                               uint32_t ddr_ref_hertz,
                               int board_type,
                               int board_rev_maj,
                               int board_rev_min
                               )
{
#if defined(__U_BOOT__) || defined(unix)
    char *s;
#endif

    uint32_t board_delay = ddr_configuration->ddr_board_delay; /* Get default board delay */
    const dimm_odt_config_t *odt_1rank_config = ddr_configuration->odt_1rank_config;
    const dimm_odt_config_t *odt_2rank_config = ddr_configuration->odt_2rank_config;
    dimm_config_t *dimm_config_table = ddr_configuration->dimm_config_table;

    /*
    ** Compute clock rates in picoseconds.
    */
    ulong tclk_psecs = (ulong) (1000*1000*1000)/(ddr_hertz/1000); /* Clock in psecs */
    ulong eclk_psecs = (ulong) (1000*1000*1000)/(cpu_hertz/1000); /* Clock in psecs */

    cvmx_lmc_comp_ctl_t lmc_comp_ctl;
    cvmx_lmc_ctl_t lmc_ctl;
    cvmx_lmc_ddr2_ctl_t ddr2_ctl;
    cvmx_lmc_mem_cfg0_t mem_cfg0;
    cvmx_lmc_mem_cfg1_t mem_cfg1;
    cvmx_lmc_delay_cfg_t lmc_delay;
    cvmx_lmc_rodt_comp_ctl_t lmc_rodt_comp_ctl;

    int row_bits, col_bits, num_banks, num_ranks, dram_width;
    int dimm_count = 0;
    int fatal_error = 0;        /* Accumulate and report all the errors before giving up */

    int safe_ddr_flag = 0; /* Flag that indicates safe DDR settings should be used */
    int ddr_interface_wide = 0;
    uint32_t mem_size_mbytes = 0;

    initialize_ddr_clock(cpu_id, cpu_hertz, ddr_hertz, ddr_ref_hertz);

    /* Disable ODT for Octeon pass 1 */
    if ((cpu_id == OCTEON_CN38XX_PASS1)) {
        odt_1rank_config = disable_odt_config;
        odt_2rank_config = disable_odt_config;
    }

    if (!odt_1rank_config)
        odt_1rank_config = disable_odt_config;
    if (!odt_2rank_config)
        odt_2rank_config = disable_odt_config;

    if (((s = getenv("ddr_safe")) != NULL) && (strcmp(s,"yes") == 0)) {
        safe_ddr_flag = 1;
        error_print("Parameter found in environment. ddr_safe = %d\n", safe_ddr_flag);
    }


    if (dimm_config_table[0].lower_spd_addr == 0) {
        error_print("ERROR: No dimms specified in the dimm_config_table.\n");
        return (-1);
    }

    /*
    ** Walk the DRAM Socket Configuration Table to see what is installed.
    */
    unsigned int didx;
    for (didx = 0; didx < 8; ++didx)
    {
        if (dimm_config_table[didx].lower_spd_addr == 0) break;

        /* Check for lower DIMM socket populated */
        if (validate_dimm(dimm_config_table[didx].lower_spd_addr, dimm_count)) {
            ++dimm_count;
        } else { break; }       /* Finished when there is no lower DIMM */

        if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
        {
            ddr_interface_wide = 1;
            continue;               /* Wide means 1 DIMM width for O2P */
        }
        else
        {
            /* Is there an upper DIMM socket? */
            if (dimm_config_table[didx].upper_spd_addr == 0) continue;

            /* Check for upper DIMM socket populated */
            if (validate_dimm(dimm_config_table[didx].upper_spd_addr, dimm_count)) {
                ddr_interface_wide = 1;
                ++dimm_count;
            }

            /* Check for odd number of DIMMs when 128-bit expected */
            if ((ddr_interface_wide)  && (dimm_count&1)) {
                error_print("ERROR: Install DIMMs in pairs for 128-bit interface\n");
                ++fatal_error;
            }
        }
    }

    if (dimm_count == 0) {
        error_print("ERROR: DIMM 0 not detected.\n");
        return(-1);
    }

    /* Force narrow DDR for chips that do not support wide */
    if (octeon_is_cpuid(OCTEON_CN3020) || octeon_is_cpuid(OCTEON_CN3005))
        ddr_interface_wide = 0;

    if (ddr_interface_wide && ((s = getenv("ddr_narrow")) != NULL) && (strcmp(s,"yes") == 0) && !octeon_is_cpuid(OCTEON_CN50XX)) {
        error_print("Parameter found in environment: ddr_narrow, forcing narrow ddr interface\n");
        ddr_interface_wide = 0;
    }

    /* Temporary to handle mismarked 3020 parts */
    if (board_type == CVMX_BOARD_TYPE_CN3020_EVB_HS5 && octeon_is_cpuid(OCTEON_CN31XX))
        ddr_interface_wide = 0;


    row_bits = read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_NUM_ROW_BITS);
    col_bits = read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_NUM_COL_BITS);
    num_banks = read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_NUM_BANKS);
    num_ranks = 1 + (0x7 & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_NUM_RANKS));
    dram_width = read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_SDRAM_WIDTH);



    /*
    ** Check that values are within some theoretical limits.
    ** col_bits(min) = row_lsb(min) - bank_bits(max) - bus_bits(max) = 14 - 3 - 4 = 7
    ** col_bits(max) = row_lsb(max) - bank_bits(min) - bus_bits(min) = 18 - 2 - 3 = 13
    */
    if ((col_bits > 13) || (col_bits < 7)) {
        error_print("Unsupported number of Col Bits: %d\n", col_bits);
        ++fatal_error;
    }

    /*
    ** Check that values are within some theoretical limits.
    ** row_bits(min) = pbank_lsb(min) - row_lsb(max) - rank_bits = 26 - 18 - 1 = 7
    ** row_bits(max) = pbank_lsb(max) - row_lsb(min) - rank_bits = 33 - 14 - 1 = 18
    */
    if ((row_bits > 18) || (row_bits < 7)) {
        error_print("Unsupported number of Row Bits: %d\n", row_bits);
        ++fatal_error;
    }

    int bank_bits = 0;
    if (num_banks == 8)
        bank_bits = 3;
    else if (num_banks == 4)
        bank_bits = 2;


    int bunk_enable = (num_ranks > 1);

#if defined(__U_BOOT__)
    if (octeon_is_cpuid(OCTEON_CN31XX))
    {
        /*
        ** For EBH3100 pass 1 we have screwy chip select mappings.  As a
        ** result only the first rank of each DIMM is selectable.
        ** Furthermore, the first rank of the second DIMM appears as the
        ** second rank of the first DIMM.  Therefore, when dual-rank DIMMs
        ** are present report half the memory size.  When single-rank
        ** DIMMs are present report them as one dual-rank DIMM.  The ODT
        ** masks will be adjusted appropriately.
        */
        {
            DECLARE_GLOBAL_DATA_PTR;
            if (gd->board_desc.board_type == CVMX_BOARD_TYPE_EBH3100
                && gd->board_desc.rev_major == 1
                && gd->board_desc.rev_minor == 0)
            {
                bunk_enable = (dimm_count > 1) || (num_ranks > 1);
            }
        }
    }
#endif
    int column_bits_start = 1;
    if (octeon_is_cpuid(OCTEON_CN30XX) | octeon_is_cpuid(OCTEON_CN50XX))
        column_bits_start = 1;
    else if (octeon_is_cpuid(OCTEON_CN31XX))
        column_bits_start = 2;
    else if (octeon_is_cpuid(OCTEON_CN38XX))
        column_bits_start = 3;
    else if (octeon_is_cpuid(OCTEON_CN58XX))
        column_bits_start = 3;
    else 
        printf("ERROR: Unsupported Octeon model: 0x%x\n", cpu_id);

    int row_lsb = column_bits_start + col_bits + bank_bits;
    debug_print("row_lsb = column_bits_start + col_bits + bank_bits = %d\n", row_lsb);

    int pbank_lsb = row_lsb + row_bits + bunk_enable;
    debug_print("pbank_lsb = row_lsb + row_bits + bunk_enable = %d\n", pbank_lsb);


    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        /* Interface width is not a function of dimm_count so add it in
           for this calculation. */
        mem_size_mbytes =  dimm_count * ((1ull << (pbank_lsb+ddr_interface_wide)) >> 20);
#if defined(__U_BOOT__)
        {
            DECLARE_GLOBAL_DATA_PTR;
            if (gd->board_desc.board_type == CVMX_BOARD_TYPE_EBH3100
                && gd->board_desc.rev_major == 1
                && gd->board_desc.rev_minor == 0
                && bunk_enable)
            {
                /* Reduce memory size by half on rev 1.0 ebh3100s */
                printf("NOTICE: Memory size reduced by half on 2 rank memory configs.\n");
                mem_size_mbytes /= 2;
            }
        }
#endif
    }
    else
        mem_size_mbytes =  dimm_count * ((1ull << pbank_lsb) >> 20);


    ddr_print("row bits: %d, col bits: %d, banks: %d, ranks: %d, dram width: %d, size: %d MB\n",
              row_bits, col_bits, num_banks, num_ranks, dram_width, mem_size_mbytes);

#if CONFIG_RAM_RESIDENT
    /* If we are booting from RAM, the DRAM controller is already set up.  Just return the
    ** memory size */
    return mem_size_mbytes;
#endif


    int spd_cycle_clx   = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_CYCLE_CLX);
    int spd_refresh     = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_REFRESH);
    int spd_cas_latency = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_CAS_LATENCY);
    int spd_cycle_clx1  = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_CYCLE_CLX1);
    int spd_cycle_clx2  = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_CYCLE_CLX2);
    int spd_trp         = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TRP);
    int spd_trrd        = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TRRD);
    int spd_trcd        = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TRCD);
    int spd_tras        = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TRAS);
    int spd_twr         = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TWR);
    int spd_twtr        = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TWTR);
    int spd_trfc_ext    = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TRFC_EXT);
    int spd_trfc        = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_TRFC);
    int spd_ecc         = ((read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_CONFIG_TYPE) & 0x3f) == 2);
    int spd_rdimm       = !!(read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_DIMM_TYPE) & 0x11);
    int spd_burst_lngth = 0xff & read_spd(dimm_config_table[0].lower_spd_addr, DDR2_SPD_BURST_LENGTH);


    int use_ecc = 1;
    if (octeon_is_cpuid(OCTEON_CN3020) || octeon_is_cpuid(OCTEON_CN3005))
        use_ecc = 0;

    /* Temporary to handle mismarked 3020 parts */
    if (board_type == CVMX_BOARD_TYPE_CN3020_EVB_HS5)
        use_ecc = 0;

    use_ecc = use_ecc && spd_ecc;

    if (octeon_is_cpuid(OCTEON_CN31XX))
        ddr_print("CN31XX DRAM Interface width: %d bits %s\n", ddr_interface_wide ? 64 : 32, use_ecc ? "+ECC" : "");
    else if (octeon_is_cpuid(OCTEON_CN50XX))
        ddr_print("CN50XX DRAM Interface width: %d bits %s\n", ddr_interface_wide ? 32 : 16, use_ecc ? "+ECC" : "");
    else if (octeon_is_cpuid(OCTEON_CN30XX))
        ddr_print("CN30XX DRAM Interface width: %d bits %s\n", ddr_interface_wide ? 32 : 16, use_ecc ? "+ECC" : "");
    else if (octeon_is_cpuid(OCTEON_CN58XX))
        ddr_print("CN58XX DRAM Interface width: %d bits %s\n", ddr_interface_wide ? 128 : 64, use_ecc ? "+ECC" : "");
    else
        ddr_print("CN38XX DRAM Interface width: %d bits %s\n", ddr_interface_wide ? 128 : 64, use_ecc ? "+ECC" : "");



    debug_print("spd_cycle_clx   : %#04x\n", spd_cycle_clx);
    debug_print("spd_refresh     : %#04x\n", spd_refresh);
    debug_print("spd_cas_latency : %#04x\n", spd_cas_latency);
    debug_print("spd_cycle_clx1  : %#04x\n", spd_cycle_clx1);
    debug_print("spd_cycle_clx2  : %#04x\n", spd_cycle_clx2);
    debug_print("spd_trp         : %#04x\n", spd_trp);
    debug_print("spd_trrd        : %#04x\n", spd_trrd);
    debug_print("spd_trcd        : %#04x\n", spd_trcd);
    debug_print("spd_tras        : %#04x\n", spd_tras);
    debug_print("spd_twr         : %#04x\n", spd_twr);
    debug_print("spd_twtr        : %#04x\n", spd_twtr);
    debug_print("spd_trfc_ext    : %#04x\n", spd_trfc_ext);
    debug_print("spd_trfc        : %#04x\n", spd_trfc);

    unsigned cycle_clx[3]    =    {
        lookup_cycle_time_psec (spd_cycle_clx),
        lookup_cycle_time_psec (spd_cycle_clx1),
        lookup_cycle_time_psec (spd_cycle_clx2)
    };
    int cas_latency     = select_cas_latency (tclk_psecs, spd_cas_latency, cycle_clx);

    int refresh         = lookup_refresh_rate_nsec (spd_refresh);
    int trp             = lookup_delay_psec (spd_trp);
    int trrd            = lookup_delay_psec (spd_trrd);
    int trcd            = lookup_delay_psec (spd_trcd);
    int tras            = spd_tras * 1000;
    int twr             = lookup_delay_psec (spd_twr);
    int twtr            = lookup_delay_psec (spd_twtr);
    int trfc            = lookup_rfc_psec (spd_trfc, spd_trfc_ext);

    ddr_print("DDR Clock Rate (tCLK)                         : %6d ps\n", tclk_psecs);
    ddr_print("CAS Latency                                   : %6d\n",    cas_latency);
    ddr_print("Refresh Rate (tREFI)                          : %6d ns\n", refresh);
    ddr_print("Minimum Row Precharge Time (tRP)              : %6d ps\n", trp);
    ddr_print("Minimum Row Active to Row Active delay (tRRD) : %6d ps\n", trrd);
    ddr_print("Minimum RAS to CAS delay (tRCD)               : %6d ps\n", trcd);
    ddr_print("Minimum Active to Precharge Time (tRAS)       : %6d ps\n", tras);
    ddr_print("Write Recovery Time (tWR)                     : %6d ps\n", twr);
    ddr_print("Internal write to read command delay (tWTR)   : %6d ps\n", twtr);
    ddr_print("Device Min Auto-refresh Active/Command (tRFC) : %6d ps\n", trfc);


    if ((num_banks != 4) && (num_banks != 8))
    {
        error_print("Unsupported number of banks %d. Must be 4 or 8.\n", num_banks);
        ++fatal_error;
    }

    if ((num_ranks < 1) || (num_ranks > 2))
    {
        error_print("Unsupported number of ranks: %d\n", num_ranks);
        ++fatal_error;
    }

    if ((dram_width != 8) && (dram_width != 16))
    {
        error_print("Unsupported SDRAM Width, %d.  Must be 8 or 16.\n", dram_width);
        ++fatal_error;
    }




    int ctl_silo_hc, ctl_silo_qc, ctl_tskw, ctl_fprch2;
    int ctl_qs_dic, ctl_dic, ctl_odt_ena;
    unsigned int ctl_odt_mask, ctl_rodt_ctl;
    unsigned int ctl_odt_mask1 = 0;

#if (CONFIG_OCTEON_EBT5800 || !defined(__U_BOOT__))
    if (board_type == CVMX_BOARD_TYPE_EBT5800) {
        if (octeon_is_cpuid(OCTEON_CN58XX)) {
            board_delay = spd_rdimm ? OCTEON_CN58XX_EBT5800_DDR_BOARD_DELAY : OCTEON_CN58XX_EBT5800_UNB_DDR_BOARD_DELAY;
        }
        if (octeon_is_cpuid(OCTEON_CN38XX)){
            board_delay = spd_rdimm ? OCTEON_CN38XX_EBT5800_DDR_BOARD_DELAY : OCTEON_CN38XX_EBT5800_UNB_DDR_BOARD_DELAY;
        }
    }
#endif

#if (CONFIG_OCTEON_EBT3000 || !defined(__U_BOOT__))
    if (board_type == CVMX_BOARD_TYPE_EBT3000) {
        board_delay = spd_rdimm ? OCTEON_EBT3000_DDR_BOARD_DELAY : OCTEON_EBT3000_UNB_DDR_BOARD_DELAY;
    }
#endif

#if (CONFIG_OCTEON_NAC38  && defined(__U_BOOT__))
    if (octeon_is_cpuid(OCTEON_CN58XX)) {
        board_delay = spd_rdimm ? OCTEON_CN58XX_NAC38_DDR_BOARD_DELAY : OCTEON_CN58XX_NAC38_UNB_DDR_BOARD_DELAY;
    }
    if (octeon_is_cpuid(OCTEON_CN38XX)){
        board_delay = spd_rdimm ? OCTEON_CN38XX_NAC38_DDR_BOARD_DELAY : OCTEON_CN38XX_NAC38_UNB_DDR_BOARD_DELAY;
    }
#endif

    if ((s = getenv("ddr_board_delay")) != NULL) {
        unsigned env_delay;
        env_delay = (unsigned) simple_strtoul(s, NULL, 0);
        if ((board_delay) && (board_delay != env_delay)) {
            error_print("Overriding internal board delay (%d ps).\n", board_delay);
            error_print("Parameter found in environment. ddr_board_delay = %d ps\n",
                        env_delay);
            board_delay = env_delay;
        }
    }

    ddr_print("Board delay                                   : %6d ps\n", board_delay);
    if (board_delay == 0) {
        error_print("Error!!!  Board delay was not specified!!!\n");
        ++fatal_error;
    }

    /*
    ** Bail out here if things are not copasetic.
    */
    if (fatal_error)
        return(-1);


    ulong delay_params = compute_delay_params(board_delay, tclk_psecs, (cpu_id == OCTEON_CN38XX_PASS1));

    ctl_tskw    = (delay_params>>0) & 0xff;
    ctl_silo_hc = (delay_params>>8) & 1;
    ctl_silo_qc = (delay_params>>16) & 1;


    if (ddr_verbose())
    {
        if ((s = getenv("ddr_tskw")) != NULL) {
            ctl_tskw    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_tskw = %d\n", ctl_tskw);
        }

        if ((s = getenv("ddr_silo_hc")) != NULL) {
            ctl_silo_hc    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_silo_hc = %d\n", ctl_silo_hc);
        }

        if ((s = getenv("ddr_silo_qc")) != NULL) {
            ctl_silo_qc    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_silo_qc = %d\n", ctl_silo_qc);
        }
    }


    ctl_fprch2  = 1;

    int odt_idx = min(dimm_count - 1, 3);
    const dimm_odt_config_t *odt_config = bunk_enable ? odt_2rank_config : odt_1rank_config;

    ctl_qs_dic		= odt_config[odt_idx].qs_dic;
    /* Note: We don't use OCTEON_IS_MODEL here since we must use the cpu_id variable passed us - we
    ** may be running on a PCI host and not be able to ready the CPU ID directly */
    if ((octeon_is_cpuid(OCTEON_CN31XX) || (octeon_is_cpuid(OCTEON_CN30XX)))
        && (tclk_psecs <= 3750)
        && (num_ranks > 1)) {
        /* O2P requires 50 ohm ODT for dual-rank 533 MHz and higher  */
        ctl_qs_dic  = 3;
    }

    ctl_dic		= odt_config[odt_idx].dic;
    ctl_odt_ena		= odt_config[odt_idx].odt_ena;
    ctl_odt_mask	= odt_config[odt_idx].odt_mask;
    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
        ctl_odt_mask1	= odt_config[odt_idx].odt_mask1;
    ctl_rodt_ctl	= odt_config[odt_idx].rodt_ctl;

    if (ddr_verbose() && (cpu_id != OCTEON_CN38XX_PASS1)) {
        if ((s = getenv("ddr_dic")) != NULL) {
            ctl_dic    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_dic = %d\n", ctl_dic);
        }

        if ((s = getenv("ddr_qs_dic")) != NULL) {
            ctl_qs_dic    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_qs_dic = %d\n", ctl_qs_dic);
        }

        if ((s = getenv("ddr_odt_ena")) != NULL) {
            ctl_odt_ena    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_odt_ena = %d\n", ctl_odt_ena);
        }

        if ((s = getenv("ddr_odt_mask")) != NULL) {
            ctl_odt_mask    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_odt_mask = 0x%x\n", ctl_odt_mask);
        }

        if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN30XX))
        {
            if ((s = getenv("ddr_odt_mask1")) != NULL) {
                ctl_odt_mask1    = simple_strtoul(s, NULL, 0);
                error_print("Parameter found in environment. ddr_odt_mask1 = 0x%x\n", ctl_odt_mask1);
            }
        }

        if ((s = getenv("ddr_rodt_ctl")) != NULL) {
            ctl_rodt_ctl    = simple_strtoul(s, NULL, 0);
            error_print("Parameter found in environment. ddr_rodt_ctl = 0x%x\n", ctl_rodt_ctl);
        }
    }

    if (octeon_is_cpuid(OCTEON_CN58XX)) {
        /* These registers are in the CN58XX and they must be zero.
           Read ODT is enabled differently for CN58XX */
        ctl_odt_ena	= 0;    /* Must be zero */
        ctl_rodt_ctl	= 0;    /* Must be zero */
    }


    if (board_type == CVMX_BOARD_TYPE_EBT3000 && board_rev_maj == 1)
    {
        /* Hack to support old rev 1 ebt3000 boards.  These boards
        ** are not supported by the pci utils */
        ctl_fprch2  = 0;
        ctl_qs_dic  = 2;  /* 0 may also work */
    }
    /* ----------------------------------------------------------------------------- */

    /*
     * DRAM Controller Initialization
     * The reference-clock inputs to the LMC (DDR2_REF_CLK_*) should be stable
     * when DCOK asserts (refer to Section 26.3). DDR_CK_* should be stable from that
     * point, which is more than 200 us before software can bring up the main memory
     * DRAM interface. The generated DDR_CK_* frequency is four times the
     * DDR2_REF_CLK_* frequency.
     * To initialize the main memory and controller, software must perform the following
     * steps in this order:
     *
     * 1. Write LMC_CTL with [DRESET] = 1, [PLL_BYPASS] = user_value, and
     * [PLL_DIV2] = user_value.
     */

    lmc_ctl.u64               = 0;

    lmc_ctl.s.dic               = ctl_dic;
    lmc_ctl.s.qs_dic            = ctl_qs_dic;
    lmc_ctl.s.tskw              = ctl_tskw;
    lmc_ctl.s.sil_lat           = safe_ddr_flag ? 2 : 1;
    lmc_ctl.s.bprch             = ctl_silo_hc & ctl_silo_qc;
    lmc_ctl.s.fprch2            = ctl_fprch2;
    if (octeon_is_cpuid(OCTEON_CN31XX))
        lmc_ctl.cn31xx.mode32b           = ! ddr_interface_wide; /* 32-bit == Not Wide */
    else if (octeon_is_cpuid(OCTEON_CN50XX))
        lmc_ctl.cn30xx.mode32b           = ddr_interface_wide; /* 32-bit == Wide */
    else if (octeon_is_cpuid(OCTEON_CN30XX))
        lmc_ctl.cn30xx.mode32b           = ddr_interface_wide; /* 32-bit == Wide */
    else
        lmc_ctl.cn38xx.mode128b          = ddr_interface_wide; /* 128-bit == Wide */
    lmc_ctl.s.inorder_mrf       = 0;
    lmc_ctl.s.inorder_mwf       = 0;
    lmc_ctl.s.r2r_slot          = safe_ddr_flag ? 1 : 0;
    lmc_ctl.s.rdimm_ena         = spd_rdimm;
    lmc_ctl.s.max_write_batch   = 0xf;
    lmc_ctl.s.xor_bank          = 1;
    lmc_ctl.s.ddr__pctl         = 0;
    lmc_ctl.s.ddr__nctl         = 0;
    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX))
    {
#if CONFIG_OCTEON_HIKARI
        lmc_ctl.cn31xx.pll_div2          = 1;
#endif
        lmc_ctl.cn31xx.dreset            = 1;
    }



    octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);



    /* 2. Read L2D_BST0 and wait for the result. */
    octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

    /* 3. Wait 10 us (LMC_CTL[PLL_BYPASS] and LMC_CTL[PLL_DIV2] must not
     * transition after this) */
    octeon_delay_cycles(6000);  /* Wait 10 us */


    /* ----------------------------------------------------------------------------- */

    int ctl_twr = divide_roundup(twr, tclk_psecs) - 1;
    ctl_twr = min(ctl_twr, 5);
    ctl_twr = max(ctl_twr, 1);

    int ctl_ddr_eof;
    if (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
        int ctl_ddr_eof_ratio;
        ctl_ddr_eof_ratio = tclk_psecs*10/eclk_psecs;
            ctl_ddr_eof = 3;
        if (ctl_ddr_eof_ratio >= 15)
            ctl_ddr_eof = 2;
        if (ctl_ddr_eof_ratio >= 20)
            ctl_ddr_eof = 1;
        if (ctl_ddr_eof_ratio >= 30)
            ctl_ddr_eof = 0;
    }
    else {
        ctl_ddr_eof = min( 3, max( 1, divide_roundup(tclk_psecs, eclk_psecs)));
    }
    ddr_print("DDR EOF                                   : %6d\n", ctl_ddr_eof);
    ddr2_ctl.u64 = 0;
    ddr2_ctl.s.ddr2       = 1;
    if (octeon_is_cpuid(OCTEON_CN38XX))
        ddr2_ctl.cn38xx.rdqs       = 0;
    if (octeon_is_cpuid(OCTEON_CN58XX))
        ddr2_ctl.cn58xx.rdqs       = 0;
    if (octeon_is_cpuid(OCTEON_CN50XX))
        ddr2_ctl.cn50xx.rdqs       = 0;
    ddr2_ctl.s.dll90_byp  = 0;
    ddr2_ctl.s.dll90_vlu  = 0;
    ddr2_ctl.s.qdll_ena   = 0;
    ddr2_ctl.s.crip_mode  = 0;

    if (cpu_id != OCTEON_CN38XX_PASS1) /* Octeon Pass 1 chip? */
        ddr2_ctl.s.ddr2t  = safe_ddr_flag ? 1 : (spd_rdimm == 0);

    if (ddr_verbose() && (cpu_id != OCTEON_CN38XX_PASS1) && ((s = getenv("ddr_2t")) != NULL)) {
        ddr2_ctl.s.ddr2t  = simple_strtoul(s, NULL, 0);
        error_print("Parameter found in environment. ddr_2t = %d\n", ddr2_ctl.s.ddr2t);
    }


    ddr2_ctl.s.ddr_eof    = ctl_ddr_eof;
    ddr2_ctl.s.silo_hc    = ctl_silo_hc;
    ddr2_ctl.s.twr        = ctl_twr;
    ddr2_ctl.s.bwcnt      = 0;
    ddr2_ctl.s.pocas      = 0;
    ddr2_ctl.s.addlat     = 0;
    if ((cpu_id == OCTEON_CN38XX_PASS1) || (cpu_id == OCTEON_CN38XX_PASS2)) {
        ddr2_ctl.s.odt_ena    = 0; /* Signal aliased to cripple_mode. */
    }
    else {
        ddr2_ctl.s.odt_ena    = ctl_odt_ena;
    }
    ddr2_ctl.s.burst8     = ((cpu_id != OCTEON_CN38XX_PASS1) && (spd_burst_lngth & 8)); /* Disable burst8 for pass 1 */


    if (ddr_verbose() && (s = getenv("ddr_burst8")) != NULL) {
        ddr2_ctl.s.burst8  = simple_strtoul(s, NULL, 0);
        error_print("Parameter found in environment. ddr_burst8 = %d\n", ddr2_ctl.s.burst8);
    }


    ddr2_ctl.s.bank8      = (num_banks == 8);


    /* 4. Write LMC_DDR2_CTL[QDLL_ENA] = 1. */ /* Is it OK to write 0 first? */

    octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);
    octeon_read64(CVMX_LMC_DDR2_CTL);
    ddr2_ctl.s.qdll_ena   = 1;
    octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);
    octeon_read64(CVMX_LMC_DDR2_CTL);


    octeon_delay_cycles(2000);   // must be 200 dclocks

    /* 5. Read L2D_BST0 and wait for the result. */
    octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

    /* 6. Wait 10 us (LMC_DDR2_CTL[QDLL_ENA] must not transition after this) */
    octeon_delay_cycles(2000);  /* Wait 10 us */


    /* ----------------------------------------------------------------------------- */

    /*
     * 7. Write LMC_CTL[DRESET] = 0 (at this point, the DCLK is running and the
     * memory controller is out of reset)
     */

    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX))
    {
        lmc_ctl.cn31xx.dreset            = 0;
        octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);
        octeon_delay_cycles(2000);   // must be 200 dclocks
    }

    /* ----------------------------------------------------------------------------- */
    /*
     * Next, boot software must re-initialize the LMC_MEM_CFG1, LMC_CTL, and
     * LMC_DDR2_CTL CSRs, and also the LMC_WODT_CTL and LMC_RODT_CTL
     * CSRs. Refer to Sections 2.3.4, 2.3.5, and 2.3.7 regarding these CSRs (and
     * LMC_MEM_CFG0).
     */

    /* Configure ODT */
    if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        octeon_write64(CVMX_LMC_WODT_CTL0, ctl_odt_mask);
        octeon_write64(CVMX_LMC_WODT_CTL1, ctl_odt_mask1);
    }
    else
        octeon_write64(CVMX_LMC_WODT_CTL, ctl_odt_mask);


    octeon_write64(CVMX_LMC_RODT_CTL, ctl_rodt_ctl);

    octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);

    /* ----------------------------------------------------------------------------- */

    if (octeon_is_cpuid(OCTEON_CN30XX) || (cpu_id == OCTEON_CN38XX_PASS3) || octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX))
    {
        lmc_delay.u64 = 0;

#ifdef LMC_DELAY_CLK  /* Defined per board */
        lmc_delay.s.clk  = LMC_DELAY_CLK;
        lmc_delay.s.cmd  = LMC_DELAY_CMD;
        lmc_delay.s.dq   = LMC_DELAY_DQ;
#endif

#if (CONFIG_OCTEON_EBT5800 || !defined(__U_BOOT__))
        if (board_type == CVMX_BOARD_TYPE_EBT5800) {
            if (octeon_is_cpuid(OCTEON_CN58XX)  && (spd_rdimm == 1)) {
                lmc_delay.s.clk  = OCTEON_CN58XX_EBT5800_LMC_DELAY_CLK;
                lmc_delay.s.cmd  = OCTEON_CN58XX_EBT5800_LMC_DELAY_CMD;
                lmc_delay.s.dq   = OCTEON_CN58XX_EBT5800_LMC_DELAY_DQ;
            }
            if (octeon_is_cpuid(OCTEON_CN58XX)  && (spd_rdimm == 0)) {
                lmc_delay.s.clk  = OCTEON_CN58XX_EBT5800_UNB_LMC_DELAY_CLK;
                lmc_delay.s.cmd  = OCTEON_CN58XX_EBT5800_UNB_LMC_DELAY_CMD;
                lmc_delay.s.dq   = OCTEON_CN58XX_EBT5800_UNB_LMC_DELAY_DQ;
            }
            if (octeon_is_cpuid(OCTEON_CN38XX)  && (spd_rdimm == 1)) {
                lmc_delay.s.clk  = OCTEON_CN38XX_EBT5800_LMC_DELAY_CLK;
                lmc_delay.s.cmd  = OCTEON_CN38XX_EBT5800_LMC_DELAY_CMD;
                lmc_delay.s.dq   = OCTEON_CN38XX_EBT5800_LMC_DELAY_DQ;
            }
            if (octeon_is_cpuid(OCTEON_CN38XX)  && (spd_rdimm == 0)) {
                lmc_delay.s.clk  = OCTEON_CN38XX_EBT5800_UNB_LMC_DELAY_CLK;
                lmc_delay.s.cmd  = OCTEON_CN38XX_EBT5800_UNB_LMC_DELAY_CMD;
                lmc_delay.s.dq   = OCTEON_CN38XX_EBT5800_UNB_LMC_DELAY_DQ;
            }
        }
#endif

#if (CONFIG_OCTEON_EBT3000 || !defined(__U_BOOT__))
        if (board_type == CVMX_BOARD_TYPE_EBT3000) {
            if (spd_rdimm == 1) {
                lmc_delay.s.clk  = OCTEON_EBT3000_LMC_DELAY_CLK;
                lmc_delay.s.cmd  = OCTEON_EBT3000_LMC_DELAY_CMD;
                lmc_delay.s.dq   = OCTEON_EBT3000_LMC_DELAY_DQ;
            }
            if (spd_rdimm == 0) {
                lmc_delay.s.clk  = OCTEON_EBT3000_UNB_LMC_DELAY_CLK;
                lmc_delay.s.cmd  = OCTEON_EBT3000_UNB_LMC_DELAY_CMD;
                lmc_delay.s.dq   = OCTEON_EBT3000_UNB_LMC_DELAY_DQ;
            }
        }
#endif

#if (CONFIG_OCTEON_NAC38  && defined(__U_BOOT__))
        if (octeon_is_cpuid(OCTEON_CN58XX)  && (spd_rdimm == 1)) {
            lmc_delay.s.clk  = OCTEON_CN58XX_NAC38_LMC_DELAY_CLK;
            lmc_delay.s.cmd  = OCTEON_CN58XX_NAC38_LMC_DELAY_CMD;
            lmc_delay.s.dq   = OCTEON_CN58XX_NAC38_LMC_DELAY_DQ;
        }
        if (octeon_is_cpuid(OCTEON_CN58XX)  && (spd_rdimm == 0)) {
            lmc_delay.s.clk  = OCTEON_CN58XX_NAC38_UNB_LMC_DELAY_CLK;
            lmc_delay.s.cmd  = OCTEON_CN58XX_NAC38_UNB_LMC_DELAY_CMD;
            lmc_delay.s.dq   = OCTEON_CN58XX_NAC38_UNB_LMC_DELAY_DQ;
        }
        if (octeon_is_cpuid(OCTEON_CN38XX)  && (spd_rdimm == 1)) {
            lmc_delay.s.clk  = OCTEON_CN38XX_NAC38_LMC_DELAY_CLK;
            lmc_delay.s.cmd  = OCTEON_CN38XX_NAC38_LMC_DELAY_CMD;
            lmc_delay.s.dq   = OCTEON_CN38XX_NAC38_LMC_DELAY_DQ;
        }
        if (octeon_is_cpuid(OCTEON_CN38XX)  && (spd_rdimm == 0)) {
            lmc_delay.s.clk  = OCTEON_CN38XX_NAC38_UNB_LMC_DELAY_CLK;
            lmc_delay.s.cmd  = OCTEON_CN38XX_NAC38_UNB_LMC_DELAY_CMD;
            lmc_delay.s.dq   = OCTEON_CN38XX_NAC38_UNB_LMC_DELAY_DQ;
        }
#endif

        if (ddr_verbose())
        {
            if ((s = getenv("ddr_delay_clk")) != NULL) {
                lmc_delay.s.clk  = simple_strtoul(s, NULL, 0);
                error_print("Parameter found in environment. ddr_delay_clk = %d\n", lmc_delay.s.clk);
            }

            if ((s = getenv("ddr_delay_cmd")) != NULL) {
                lmc_delay.s.cmd  = simple_strtoul(s, NULL, 0);
                error_print("Parameter found in environment. ddr_delay_cmd = %d\n", lmc_delay.s.cmd);
            }

            if ((s = getenv("ddr_delay_dq")) != NULL) {
                lmc_delay.s.dq  = simple_strtoul(s, NULL, 0);
                error_print("Parameter found in environment. ddr_delay_dq = %d\n", lmc_delay.s.dq);
            }
        }

        octeon_write64(CVMX_LMC_DELAY_CFG, lmc_delay.u64);

        ddr_print("delay_clk                                     : %6d\n", lmc_delay.s.clk);
        ddr_print("delay_cmd                                     : %6d\n", lmc_delay.s.cmd);
        ddr_print("delay_dq                                      : %6d\n", lmc_delay.s.dq);
    }

    /* ----------------------------------------------------------------------------- */

    if (ddr_verbose() && (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)))
    {
        cvmx_lmc_ctl1_t lmc_ctl1;
        cvmx_lmc_pll_ctl_t pll_ctl;
        uint64_t clkf, clkr, pll_MHz, calculated_ddr_hertz;
        int _en = 0;

        lmc_ctl1.u64 = octeon_read64(CVMX_LMC_CTL1);
        ddr_print("sil_mode                                      : %6d\n", lmc_ctl1.cn58xx.sil_mode);

        pll_ctl.u64 = octeon_read64(CVMX_LMC_PLL_CTL);
        clkf = pll_ctl.cn58xx.clkf;
        clkr = pll_ctl.cn58xx.clkr;

        ddr_print("DDR Fixed Reference Clock Hertz               : %8d\n", ddr_ref_hertz);
        ddr_print("clkf                                          : %6d\n", clkf);
        ddr_print("clkr                                          : %6d\n", clkr);

        if (pll_ctl.cn58xx.en2 == 1) {
            ddr_print("EN2                                           : %6d\n", 1); _en = 2;
        }
        if (pll_ctl.cn58xx.en4 == 1) {
            ddr_print("EN4                                           : %6d\n", 1); _en = 4;
        }
        if (pll_ctl.cn58xx.en6 == 1) {
            ddr_print("EN6                                           : %6d\n", 1); _en = 6;
        }
        if (pll_ctl.cn58xx.en8 == 1) {
            ddr_print("EN8                                           : %6d\n", 1); _en = 8;
        }
        if (pll_ctl.cn58xx.en12 == 1) {
            ddr_print("EN12                                          : %6d\n", 1); _en = 12;
        }
        if (pll_ctl.cn58xx.en16 == 1) {
            ddr_print("EN16                                          : %6d\n", 1); _en = 16;
        }

        pll_MHz = ddr_ref_hertz * (clkf+1) / (clkr+1) / 1000000;

        ddr_print("LMC PLL Frequency                             : %6d MHz\n", pll_MHz);

        calculated_ddr_hertz = ddr_ref_hertz * (clkf + 1) / ((clkr + 1) * _en);
        ddr_print("Calculated DClk Frequency                     : %8d Hz\n", calculated_ddr_hertz);
    }

    /* ----------------------------------------------------------------------------- */

    int cfg1_tras;
    cfg1_tras =  divide_roundup(tras, tclk_psecs);

    int cfg1_trcd = divide_roundup(trcd, tclk_psecs);

    int cfg1_twtr = divide_roundup(twtr, tclk_psecs);

    /* Plus 1 for 8-bank parts */
    int cfg1_trp = divide_roundup(trp, tclk_psecs) + (num_banks == 8);

    int cfg1_trfc =  divide_roundup(trfc, (4*tclk_psecs));

    int cfg1_tmrd = 3; /* Always at least 2. 3 supports higher speeds */

    int cfg1_trrd = divide_roundup(trrd, tclk_psecs);



    mem_cfg1.u64 = 0;
    mem_cfg1.s.tras     = cfg1_tras;
    /* In 2T mode, make this register TRCD-1, not going below 2. */
    mem_cfg1.s.trcd     = max(2, (ddr2_ctl.s.ddr2t ? (cfg1_trcd - 1) : cfg1_trcd));
    mem_cfg1.s.twtr     = cfg1_twtr;
    mem_cfg1.s.trp      = cfg1_trp;
    mem_cfg1.s.trfc     = cfg1_trfc;
    mem_cfg1.s.tmrd     = cfg1_tmrd;
    mem_cfg1.s.caslat   = cas_latency;
    mem_cfg1.s.trrd     = cfg1_trrd;



    octeon_write64(CVMX_LMC_MEM_CFG1, mem_cfg1.u64);

    /* ----------------------------------------------------------------------------- */

    lmc_comp_ctl.u64 = 0;
    lmc_comp_ctl.s.pctl_clk = 0x0;
    lmc_comp_ctl.s.pctl_csr = 0xf;
    lmc_comp_ctl.s.nctl_dat = 0x0;
    lmc_comp_ctl.s.nctl_clk = 0x0;
    lmc_comp_ctl.s.nctl_csr = 0xf;


    if (board_type == CVMX_BOARD_TYPE_EBT3000 && board_rev_maj == 1)
    {
        /* Hack to support old rev 1 ebt3000 boards.  These boards
        ** are not supported by the pci utils */
        cvmx_lmc_ctl_t tmp_ctl;

#define NCTL_CMD_STRENGTH       14                      /* Strength setting */
#define PCTL_CMD_STRENGTH       14                      /* Strength setting */
#define NCTL_CLK_STRENGTH       2                       /* Strength setting */
#define PCTL_CLK_STRENGTH       1                       /* Strength setting */

        tmp_ctl.u64 = octeon_read64(CVMX_LMC_CTL);

        lmc_comp_ctl.cn38xx.pctl_cmd =  (((PCTL_CMD_STRENGTH) + 16) - tmp_ctl.s.ddr__pctl) & 0xf;
        lmc_comp_ctl.s.nctl_cmd =  (((NCTL_CMD_STRENGTH) + 16) - tmp_ctl.s.ddr__nctl) & 0xf;

        if (ddr_hertz > 250000000)
        {
            lmc_comp_ctl.s.pctl_clk =  (((PCTL_CLK_STRENGTH) + 16) - tmp_ctl.s.ddr__pctl) & 0xf;
            lmc_comp_ctl.s.nctl_clk =  (((NCTL_CLK_STRENGTH) + 16) - tmp_ctl.s.ddr__nctl) & 0xf;
        }
    }


    octeon_write64(CVMX_LMC_COMP_CTL, lmc_comp_ctl.u64);

    /* ----------------------------------------------------------------------------- */

    /*
     * Finally, software must write the LMC_MEM_CFG0 register with
     * LMC_MEM_CFG0[INIT_START] = 1. At that point, CN31XX hardware initiates
     * the standard DDR2 initialization sequence shown in Figure 2.
     */

    int ref_int = (refresh*1000) / (tclk_psecs*512);

    int cfg0_tcl = cas_latency;

    mem_cfg0.u64 = 0;


    mem_cfg0.s.init_start   = 0;
    mem_cfg0.s.ecc_ena      = use_ecc && spd_ecc;
    mem_cfg0.s.row_lsb      = encode_row_lsb(cpu_id, row_lsb, ddr_interface_wide);
    mem_cfg0.s.bunk_ena     = bunk_enable;

    if (cpu_id != OCTEON_CN38XX_PASS1) /* Octeon Pass 1 chip? */
        mem_cfg0.s.silo_qc  = ctl_silo_qc;

    mem_cfg0.s.pbank_lsb    = encode_pbank_lsb(cpu_id, pbank_lsb, ddr_interface_wide);
    mem_cfg0.s.ref_int      = ref_int;
    if (cpu_id == OCTEON_CN38XX_PASS1)           /* Octeon Pass 1 chip? */
        mem_cfg0.s.tcl          = cfg0_tcl; /* Use only for Octeon Pass 1 */
    else
        mem_cfg0.s.tcl          = 0; /* Has no effect on the controller's behavior */
    mem_cfg0.s.intr_sec_ena = 0;
    mem_cfg0.s.intr_ded_ena = 0;
    mem_cfg0.s.sec_err      = ~0;
    mem_cfg0.s.ded_err      = ~0;
    mem_cfg0.s.reset        = 0;



    ddr_print("bunk_enable                                   : %6d\n", mem_cfg0.s.bunk_ena);
    ddr_print("burst8                                        : %6d\n", ddr2_ctl.s.burst8);
    if (cpu_id != OCTEON_CN38XX_PASS1) /* Octeon Pass 1 chip? */
        ddr_print("ddr2t                                         : %6d\n", ddr2_ctl.s.ddr2t);

    ddr_print("tskw                                          : %6d\n", lmc_ctl.s.tskw);
    ddr_print("silo_hc                                       : %6d\n", ddr2_ctl.s.silo_hc);

    if (cpu_id != OCTEON_CN38XX_PASS1) /* Octeon Pass 1 chip? */
        ddr_print("silo_qc                                       : %6d\n", mem_cfg0.s.silo_qc);

    ddr_print("sil_lat                                       : %6d\n", lmc_ctl.s.sil_lat);
    ddr_print("r2r_slot                                      : %6d\n", lmc_ctl.s.r2r_slot);

    if ((cpu_id != OCTEON_CN38XX_PASS1)) {
        ddr_print("odt_ena                                       : %6d\n", ddr2_ctl.s.odt_ena);
        ddr_print("qs_dic                                        : %6d\n", lmc_ctl.s.qs_dic);
        ddr_print("dic                                           : %6d\n", lmc_ctl.s.dic);
        ddr_print("ctl_odt_mask                                  : %08x\n", ctl_odt_mask);
        if (octeon_is_cpuid(OCTEON_CN31XX) || octeon_is_cpuid(OCTEON_CN30XX) || octeon_is_cpuid(OCTEON_CN50XX))
            ddr_print("ctl_odt_mask1                                 : %08x\n", ctl_odt_mask1);
        ddr_print("ctl_rodt_ctl                                  : %08x\n", ctl_rodt_ctl);
    }

    if (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
        lmc_rodt_comp_ctl.u64 = 0;

        if (odt_config[odt_idx].odt_ena == 1) { /* Weak Read ODT */
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 3;
            lmc_rodt_comp_ctl.s.nctl = 1;
        }

        if (odt_config[odt_idx].odt_ena == 2) { /* Strong Read ODT */
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 7;
            lmc_rodt_comp_ctl.s.nctl = 2;
        }

        /* Weak Read ODT */
        if (ddr_verbose() && (((s = getenv("ddr_rodt")) != NULL) && (strcmp(s,"weak") == 0))) {
            error_print("Parameter found in environment. ddr_rodt = %s\n", s);
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 3;
            lmc_rodt_comp_ctl.s.nctl = 1;
        }

        /* Strong Read ODT */
        if (ddr_verbose() && (((s = getenv("ddr_rodt")) != NULL) && (strcmp(s,"strong") == 0))) {
            error_print("Parameter found in environment. ddr_rodt = %s\n", s);
            lmc_rodt_comp_ctl.s.enable = 1;
            lmc_rodt_comp_ctl.s.pctl = 7;
            lmc_rodt_comp_ctl.s.nctl = 2;
        }

        octeon_write64(CVMX_LMC_RODT_COMP_CTL, lmc_rodt_comp_ctl.u64);

        ddr_print("RODT enable                                   : %6d\n", lmc_rodt_comp_ctl.s.enable);
        ddr_print("RODT pctl                                     : %6d\n", lmc_rodt_comp_ctl.s.pctl);
        ddr_print("RODT nctl                                     : %6d\n", lmc_rodt_comp_ctl.s.nctl);

        if (octeon_is_cpuid(OCTEON_CN56XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
            /* CN56XX has a 5-bit copy of the compensation values */
            cvmx_lmcx_pll_status_t pll_status;
            pll_status.u64 = cvmx_read_csr(CVMX_LMC_PLL_STATUS);
            ddr_print("DDR PMOS control                              : %6d\n", pll_status.s.ddr__pctl);
            ddr_print("DDR NMOS control                              : %6d\n", pll_status.s.ddr__nctl);
        } else {
            cvmx_lmc_ctl_t tmp_ctl;
            tmp_ctl.u64 = cvmx_read_csr(CVMX_LMC_CTL);
            ddr_print("DDR PMOS control                              : %6d\n", tmp_ctl.s.ddr__pctl);
            ddr_print("DDR NMOS control                              : %6d\n", tmp_ctl.s.ddr__nctl);
        }
    }

    octeon_write64(CVMX_LMC_MEM_CFG0, mem_cfg0.u64);
    mem_cfg0.u64 = octeon_read64(CVMX_LMC_MEM_CFG0);

    mem_cfg0.s.init_start   = 1;
    octeon_write64(CVMX_LMC_MEM_CFG0, mem_cfg0.u64);
    octeon_read64(CVMX_LMC_MEM_CFG0);
    /* ----------------------------------------------------------------------------- */


#ifdef PRINT_LMC_REGS
    printf("##################################################################\n");
    printf("LMC register dump:\n");
    octeon_print_lmc_regs(cpu_id);
    printf("##################################################################\n");
#endif

    return(mem_size_mbytes);
}
#endif


/* Single byte address twsi read, used for reading MCU, DRAM eeproms */

int octeon_twsi_read(uint8_t dev_addr, uint8_t twsii_addr)
{
    /* 8 bit internal address */


    uint64_t val;
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8000000000000000ull | (((uint64_t)dev_addr) << 40) | twsii_addr); //write the address we want to read
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull)) {
        //      printf("twsii write failed\n");
        return -1;
    }
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8100000000000000ull | (((uint64_t)dev_addr) << 40)); // tell twsii to do the read
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull)) {
        //      printf("twsii read failed\n");
        return -1;
    }
    else {
        //    printf("twsii read address %d  val = %d\n",twsii_addr,(uint8_t)(val & 0xFF));

        return (uint8_t)(val & 0xFF);
    }
}

int twsii_mcu_read(uint8_t twsii_addr)
{
#ifdef BOARD_MCU_TWSI_ADDR
    return(octeon_twsi_read(BOARD_MCU_TWSI_ADDR, twsii_addr));
#else
    return(0);
#endif /* BOARD_MCU_TWSI_ADDR */
}



int initialize_ddr_clock(uint32_t cpu_id,
                         uint32_t cpu_hertz,
                         uint32_t ddr_hertz,
                         uint32_t ddr_ref_hertz)
{
#if defined(__U_BOOT__) || defined(unix)
    char *s;
#endif

#ifdef __U_BOOT__
    if ((getenv("ddr_verbose") != NULL))
    {
        DECLARE_GLOBAL_DATA_PTR;
        gd->flags |= GD_FLG_DDR_VERBOSE;
    }
#endif

    /* Check to see if dram controller already running, if so, just return */
    cvmx_lmc_mem_cfg0_t mem_cfg0;
    mem_cfg0.u64 = octeon_read64(CVMX_LMC_MEM_CFG0);
    if (mem_cfg0.s.init_start)
        return 0;

ddr_print("initialize_ddr_clock: %x %lu %lu %lu\n", cpu_id, cpu_hertz, ddr_hertz, ddr_ref_hertz);
    if ((octeon_is_cpuid(OCTEON_CN31XX)) || (octeon_is_cpuid(OCTEON_CN30XX))) {
ddr_print("3010\n");
        cvmx_lmc_ctl_t lmc_ctl;
        cvmx_lmc_ddr2_ctl_t ddr2_ctl;

        /*
         * DRAM Controller Initialization
         * The reference-clock inputs to the LMC (DDR2_REF_CLK_*) should be stable
         * when DCOK asserts (refer to Section 26.3). DDR_CK_* should be stable from that
         * point, which is more than 200 us before software can bring up the main memory
         * DRAM interface. The generated DDR_CK_* frequency is four times the
         * DDR2_REF_CLK_* frequency.
         * To initialize the main memory and controller, software must perform the following
         * steps in this order:
         *
         * 1. Write LMC_CTL with [DRESET] = 1, [PLL_BYPASS] = user_value, and
         * [PLL_DIV2] = user_value.
         */

        lmc_ctl.u64                 = octeon_read64(CVMX_LMC_CTL);

        lmc_ctl.s.pll_bypass        = 0;
#if defined(CONFIG_OCTEON_HIKARI) || defined(CONFIG_PALOMINO)
        if (!__octeon_is_model_runtime__(OCTEON_CN50XX)) {
			lmc_ctl.s.pll_div2          = 1;
		}
#else
        lmc_ctl.s.pll_div2          = 0;
#endif
        lmc_ctl.cn31xx.dreset            = 1;
        octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);

        /* 2. Read L2D_BST0 and wait for the result. */
        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

        /* 3. Wait 10 us (LMC_CTL[PLL_BYPASS] and LMC_CTL[PLL_DIV2] must not
         * transition after this) */
        octeon_delay_cycles(6000);  /* Wait 10 us */

        /* 4. Write LMC_DDR2_CTL[QDLL_ENA] = 1. */
        ddr2_ctl.u64 = octeon_read64(CVMX_LMC_DDR2_CTL);
        ddr2_ctl.s.qdll_ena   = 1;
        octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);
        octeon_read64(CVMX_LMC_DDR2_CTL);

        octeon_delay_cycles(2000);   // must be 200 dclocks

        /* 5. Read L2D_BST0 and wait for the result. */
        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

        /* 6. Wait 10 us (LMC_DDR2_CTL[QDLL_ENA] must not transition after this) */
        octeon_delay_cycles(6000);  /* Wait 10 us */

	/* 7. Write LMC_CTL[DRESET] = 0 (at this point, the DCLK is running and the
	 * memory controller is out of reset)
	 */
	lmc_ctl.cn31xx.dreset = 0;
	octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);
	octeon_delay_cycles(2000);	// must be 200 dclocks
    }

    if (octeon_is_cpuid(OCTEON_CN58XX) || octeon_is_cpuid(OCTEON_CN50XX)) {
        cvmx_lmc_ctl_t lmc_ctl;
        cvmx_lmc_ddr2_ctl_t ddr2_ctl;
        cvmx_lmc_pll_ctl_t pll_ctl;
        cvmx_lmc_ctl1_t lmc_ctl1;
ddr_print("5010\n");

        if (ddr_ref_hertz == 0) {
            error_print("ERROR: DDR Reference Clock not specified.\n");
            return(-1);
        }

        /*
         * DCLK Initialization Sequence
         * 
         * When the reference-clock inputs to the LMC (DDR2_REF_CLK_P/N) are
         * stable, perform the following steps to initialize the DCLK.
         * 
         * 1. Write LMC_CTL[DRESET]=1, LMC_DDR2_CTL[QDLL_ENA]=0.
         */

        lmc_ctl.u64                 = octeon_read64(CVMX_LMC_CTL);
        lmc_ctl.cn58xx.dreset       = 1;
        octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);

        ddr2_ctl.u64                 = octeon_read64(CVMX_LMC_DDR2_CTL);
        ddr2_ctl.s.qdll_ena     = 0;
        octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);

        /*
         * 2. Write LMC_PLL_CTL[CLKR, CLKF, EN*] with the appropriate values,
         *    while writing LMC_PLL_CTL[RESET_N] = 0, LMC_PLL_CTL[DIV_RESET] = 1.
         *    LMC_PLL_CTL[CLKR, CLKF, EN*] values must not change after this
         *    point without restarting the DCLK initialization sequence.
         */

        /* CLKF = (DCLK/DREF) * (CLKR+1) * EN(2, 4, 6, 8, 12, 16) - 1 */
        {
            int en_idx, save_en_idx, best_en_idx=0;
            uint64_t clkf, clkr;
            uint64_t best_clkf=0, best_clkr=0;
            uint64_t pll_MHz;
            uint64_t error;
            uint64_t best_error = ddr_hertz; /* Init to max error */
            uint64_t orig_ddr_hertz = ddr_hertz;
            uint64_t calculated_ddr_hertz = 0;
            static int _en[] = {2, 4, 6, 8, 12, 16};

            pll_ctl.u64 = 0;

            while (best_error == ddr_hertz)
            {

                for (clkr = 0; clkr < 64; ++clkr) {
                    for (en_idx=sizeof(_en)/sizeof(int)-1; en_idx>=0; --en_idx) {
                        save_en_idx = en_idx;
                        clkf = ((ddr_hertz) * (clkr+1) * _en[save_en_idx]);
                        clkf = round_divide(clkf, ddr_ref_hertz) - 1;
                        pll_MHz = ddr_ref_hertz * (clkf+1) / (clkr+1) / 1000000;
                        if (pll_MHz <= 2500)
                            break;
                    }
    
                    calculated_ddr_hertz = ddr_ref_hertz * (clkf + 1) / ((clkr + 1) * _en[save_en_idx]);
                    error = ddr_hertz - calculated_ddr_hertz;
    
                    if (pll_MHz < 1200) continue;
                    if (calculated_ddr_hertz > ddr_hertz) continue;
                    if (error >= best_error) continue;
#ifdef CONFIG_ARUBA_OCTEON
                    if (clkf > 0xff) continue; /* PLL requires clkf to be limited values less than 256, if fasten_n is set */
#else
                    if (clkf > 0x7f) continue; /* PLL requires clkf to be limited values less than 128 */
#endif
    
#if defined(__U_BOOT__)
                    ddr_print("clkr: %2lu, en: %2d, clkf: %4lu, pll_MHz: %4lu, ddr_hertz: %8lu, error: %8lu\n",
                              clkr, _en[save_en_idx], clkf, pll_MHz, calculated_ddr_hertz, error);
#else
                    ddr_print("clkr: %2llu, en: %2d, clkf: %4llu, pll_MHz: %4llu, ddr_hertz: %8llu, error: %8llu\n",
                              clkr, _en[save_en_idx], clkf, pll_MHz, calculated_ddr_hertz, error);
#endif
    
                    best_error = error;
                    best_clkr = clkr;
                    best_clkf = clkf;
                    best_en_idx = save_en_idx;
    
                    if (best_error == 0) break; /* Done... Can't do any better */
                }

                /* Try lowering the frequency if we can't get a working configuration */
                if (best_error == ddr_hertz) {
                    if (ddr_hertz < orig_ddr_hertz - 10000000)
                        break;
                    ddr_hertz -= 1000000;
                    best_error = ddr_hertz;
                }

            }
            if (best_error == ddr_hertz) {
                error_print("ERROR: Can not compute a legal DDR clock speed configuration.\n");
                return(-1);
            }

            pll_ctl.cn50xx.en2  = (best_en_idx == 0);
            pll_ctl.cn50xx.en4  = (best_en_idx == 1);
            pll_ctl.cn50xx.en6  = (best_en_idx == 2);
            pll_ctl.cn50xx.en8  = (best_en_idx == 3);
            pll_ctl.cn50xx.en12 = (best_en_idx == 4);
            pll_ctl.cn50xx.en16 = (best_en_idx == 5);

            pll_ctl.cn50xx.clkf = best_clkf;
            pll_ctl.cn50xx.clkr = best_clkr;
            pll_ctl.cn50xx.reset_n = 0;
            pll_ctl.cn50xx.div_reset = 1;

#ifdef CN5010_PLL_LOCK_WAR
			pll_ctl.cn50xx.fasten_n = 1;
#else
            if (best_clkf > 0x7f) {
                // if clkf > 127, then enable fasten_n for more range
                pll_ctl.cn50xx.fasten_n = 1;
            }
#endif
            octeon_write64(CVMX_LMC_PLL_CTL, pll_ctl.u64);
        }


        /*
         * 3. Read L2D_BST0 and wait for the result.
         */

        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

        /* 
         * 4. Wait 5 usec.
         */

        octeon_delay_cycles(10000);  /* Wait 10 us */


        /*
         * 5. Write LMC_PLL_CTL[RESET_N] = 1 while keeping LMC_PLL_CTL[DIV_RESET]
         *    = 1. LMC_PLL_CTL[RESET_N] must not change after this point without
         *    restarting the DCLK initialization sequence.
         */

        pll_ctl.u64 = octeon_read64(CVMX_LMC_PLL_CTL);
        pll_ctl.cn58xx.reset_n = 1;
        octeon_write64(CVMX_LMC_PLL_CTL, pll_ctl.u64);

        /*
         * 6. Read L2D_BST0 and wait for the result.
         */

        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

        /*
         * 7. Wait 500  (LMC_PLL_CTL[CLKR] + 1) reference-clock cycles.
         */

        octeon_delay_cycles(1000000); /* FIX: Wait 1 ms */

        /*
         * 8. Write LMC_PLL_CTL[DIV_RESET] = 0. LMC_PLL_CTL[DIV_RESET] must not
         *    change after this point without restarting the DCLK initialization
         *    sequence.
         */

        pll_ctl.u64 = octeon_read64(CVMX_LMC_PLL_CTL);
        pll_ctl.cn58xx.div_reset = 0;
        octeon_write64(CVMX_LMC_PLL_CTL, pll_ctl.u64);

        /*
         * 9. Read L2D_BST0 and wait for the result.
         * 
         * The DDR address clock frequency (DDR_CK_<5:0>_P/N) should be stable at
         * that point. Section 2.3.9 describes the DDR_CK frequencies resulting
         * from different reference-clock values and programmings.
         */

        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */


        /*
         * DRESET Initialization Sequence
         * 
         * The DRESET initialization sequence cannot start unless DCLK is stable
         * due to a prior DCLK initialization sequence. Perform the following
         * steps to initialize DRESET.
         * 
         * 1. Write LMC_CTL[DRESET] = 1 and LMC_DDR2_CTL[QDLL_ENA] = 0.
         */
 
        lmc_ctl.u64                 = octeon_read64(CVMX_LMC_CTL);
        lmc_ctl.cn58xx.dreset       = 1;
        octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);

        ddr2_ctl.u64                 = octeon_read64(CVMX_LMC_DDR2_CTL);
        ddr2_ctl.s.qdll_ena     = 0;
        octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);

        /*
         * 2. Write LMC_DDR2_CTL[QDLL_ENA] = 1. LMC_DDR2_CTL[QDLL_ENA] must not
         *    change after this point without restarting the LMC and/or DRESET
         *    initialization sequence.
         */
 
        ddr2_ctl.u64 = octeon_read64(CVMX_LMC_DDR2_CTL);
        ddr2_ctl.s.qdll_ena   = 1;
        octeon_write64(CVMX_LMC_DDR2_CTL, ddr2_ctl.u64);
        octeon_read64(CVMX_LMC_DDR2_CTL);

        /*
         * 3. Read L2D_BST0 and wait for the result.
         */

        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */
 
        /*
         * 4. Wait 10 usec.
         */

        octeon_delay_cycles(10000);  /* Wait 10 us */

 
        /*
         * 5. Write LMC_CTL[DRESET] = 0. LMC_CTL[DRESET] must not change after
         *    this point without restarting the DRAM-controller and/or DRESET
         *    initialization sequence.
         */

        lmc_ctl.u64                 = octeon_read64(CVMX_LMC_CTL);
        lmc_ctl.cn58xx.dreset       = 0;
        octeon_write64(CVMX_LMC_CTL, lmc_ctl.u64);


        /*
         * 6. Read L2D_BST0 and wait for the result.
         */

        octeon_read64(CVMX_L2D_BST0); /* Read CVMX_L2D_BST0 */

 

        /* 
         * LMC Initialization Sequence
         * 
         * The LMC initialization sequence must be preceded by a DCLK and DRESET
         * initialization sequence.
         * 
         * 1. Software must ensure there are no pending DRAM transactions.
         */

        /*
         * 2. Write LMC_CTL, LMC_CTL1, LMC_MEM_CFG1, LMC_DDR2_CTL,
         * LMC_RODT_CTL, LMC_DUAL_MEMCFG, and LMC_WODT_CTL with appropriate
         * values, if necessary. Refer to Sections 2.3.4, 2.3.5, and 2.3.7 regarding
         * these registers (and LMC_MEM_CFG0).
         */

        lmc_ctl1.u64 = octeon_read64(CVMX_LMC_CTL1);
	if (ddr_verbose() && (s = getenv("ddr_sil_mode")) != NULL) {
	    lmc_ctl1.cn58xx.sil_mode = simple_strtoul(s, NULL, 0);
	    error_print("Parameter found in environment. ddr_sil_mode = %d\n", lmc_ctl1.cn58xx.sil_mode);
	}
	else {
	    lmc_ctl1.cn58xx.sil_mode = 1;
	}
        octeon_write64(CVMX_LMC_CTL1, lmc_ctl1.u64);

        /*
         * 3. Write LMC_MEM_CFG0 with appropriate values and
         * LMC_MEM_CFG0[INIT_START] = 0.
         */

        /*
         * 4. Write LMC_MEM_CFG0 with appropriate values and
         *    LMC_MEM_CFG0[INIT_START] = 1. At that point, CN58XX hardware
         *    initiates the standard DDR2 init sequence shown in Figure 29.
         * 
         *    CN58XX activates DDR_CKE (if it has not already been
         *    activated). DDR_CKE remains activated from that point until a
         *    subsequent DRESET.
         * 
         *    CN58XX then follows with the standard DDR2 initialization sequence,
         *    not using OCD. While CN58XX performs the initialization sequence,
         *    it cannot perform other DDR2 transactions.
         * 
         *    Note that if there is not a DRESET between two LMC initialization
         *    sequences, DDR_CKE remains asserted through the second
         *    initialization sequence. The hardware initiates the same DDR2
         *    initialization sequence as the first, except that DDR_CKE does not
         *    deactivate. If DDR_CKE deactivation and reactivation is desired for
         *    a second controller reset, a DRESET sequence is required.
         */

        /*
         * 5. Read L2D_BST0 and wait for the result.
         * 
         * After this point, the LMC is fully functional.
         * LMC_MEM_CFG0[INIT_START] must not transition from 0.1 during normal
         * operation.
         */ 

    }

    /* On pass2 we can count DDR clocks, and we use this to correct
    ** the DDR clock that we are passed.
    ** We must enable the memory controller to count DDR clocks. */
    mem_cfg0.u64 = 0;
    mem_cfg0.s.init_start   = 1;
    octeon_write64(CVMX_LMC_MEM_CFG0, mem_cfg0.u64);
    octeon_read64(CVMX_LMC_MEM_CFG0);


    return 0;

}

static void octeon_ipd_delay_cycles(uint64_t cycles)
{
    uint64_t start = octeon_read64((uint64_t)0x80014F0000000338ULL);
    while (start + cycles > octeon_read64((uint64_t)0x80014F0000000338ULL))
        ;
}

uint32_t measure_octeon_ddr_clock(uint32_t cpu_id,
                                  uint32_t cpu_hertz,
                                  uint32_t ddr_hertz,
                                  uint32_t ddr_ref_hertz)
{

    if (cpu_id == OCTEON_CN38XX_PASS1)
        return 0;

    initialize_ddr_clock(cpu_id, cpu_hertz, ddr_hertz, ddr_ref_hertz);

    /* Dynamically determine the DDR clock speed */
    uint32_t core_clocks = octeon_read64((uint64_t)0x80014F0000000338ULL);
    uint32_t ddr_clocks = octeon_read64(CVMX_LMC_DCLK_CNT_LO);  /* ignore overflow, starts counting when we enable the controller */
    octeon_ipd_delay_cycles(100000000); /* How many cpu cycles to measure over */
    core_clocks = octeon_read64((uint64_t)0x80014F0000000338ULL) - core_clocks;
    ddr_clocks = octeon_read64(CVMX_LMC_DCLK_CNT_LO) - ddr_clocks;
    uint32_t calc_ddr_hertz = ((ddr_clocks/CALC_SCALER) * (cpu_hertz/CALC_SCALER) / (core_clocks/CALC_SCALER)) * CALC_SCALER;
    ddr_print("Measured DDR clock %d\n", calc_ddr_hertz);
    return (calc_ddr_hertz);
}



int octeon_twsi_set_addr16(uint8_t dev_addr, uint16_t addr)
{

    /* 16 bit internal address ONLY */
    uint64_t val;
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8000000000000000ull | ( 0x1ull << 57) | (((uint64_t)dev_addr) << 40) | ((((uint64_t)addr) >> 8) << 32) | (addr & 0xff)); // tell twsii to do the read
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull)) {
        return -1;
    }

    return(0);
}

int octeon_twsi_set_addr8(uint8_t dev_addr, uint16_t addr)
{

    /* 16 bit internal address ONLY */
    uint64_t val;
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8000000000000000ull | ( 0x0ull << 57) | (((uint64_t)dev_addr) << 40) | (((uint64_t)addr & 0xff))); // tell twsii to do the read
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull)) {
        return -1;
    }

    return(0);
}

int octeon_twsi_read8_cur_addr(uint8_t dev_addr)
{

    uint64_t val;
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8100000000000000ull | ( 0x0ull << 57) | (((uint64_t)dev_addr) << 40) ); // tell twsii to do the read
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull))
    {
        return -1;
    }
    else
    {
        return(uint8_t)(val & 0xFF);
    }
}

int octeon_twsi_read8(uint8_t dev_addr, uint16_t addr)
{
    if (octeon_twsi_set_addr16(dev_addr, addr))
        return(-1);

    return(octeon_twsi_read8_cur_addr(dev_addr));
}

int octeon_twsi_read16_cur_addr(uint8_t dev_addr)
{
    int tmp;
    uint16_t val;
    tmp = octeon_twsi_read8_cur_addr(dev_addr);
    if (tmp < 0)
        return(-1);
    val = (tmp & 0xff) << 8;
    tmp = octeon_twsi_read8_cur_addr(dev_addr);
    if (tmp < 0)
        return(-1);
    val |= (tmp & 0xff);

    return((int)val);

}

int octeon_twsi_read16(uint8_t dev_addr, uint16_t addr)
{
    if (octeon_twsi_set_addr16(dev_addr, addr))
        return(-1);

    return(octeon_twsi_read16_cur_addr(dev_addr));
}

int octeon_twsi_write16(uint8_t dev_addr, uint16_t addr, uint16_t data)
{

    uint64_t val;
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8000000000000000ull | ( 0x8ull << 57) | (((uint64_t)dev_addr) << 40) | (addr << 16) | data ); // tell twsii to do the read
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull)) {
        //      printf("twsii write (4byte) failed\n");
        return -1;
    }

    /* or poll for read to not fail */
    octeon_delay_cycles(40*1000000);
    return(0);
}

int octeon_twsi_write8(uint8_t dev_addr, uint16_t addr, uint8_t data)
{

    uint64_t val;
    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8000000000000000ull | ( 0x1ull << 57) | (((uint64_t)dev_addr) << 40) | (((uint64_t)addr) << 32) | ((uint64_t)data));
//    octeon_write64(CVMX_MIO_TWS_SW_TWSI,0x8000000012345678ull | ( 0x1ull << 57) | (((uint64_t)dev_addr) << 40) | (((uint64_t)addr) << 32));
    while (octeon_read64(CVMX_MIO_TWS_SW_TWSI)&0x8000000000000000ull);
    val = octeon_read64(CVMX_MIO_TWS_SW_TWSI);
    if (!(val & 0x0100000000000000ull)) {
              printf("octeon_twsi_write8: (1byte) failed\n");
        return -1;
    }

    /* or poll for read to not fail */
    octeon_delay_cycles(40*1000000);
    return(0);
}



#define tlv_debug_print(...) /* Careful... This could be too early to print */

/***************** EEPROM TLV support functions ******************/

int  octeon_tlv_eeprom_get_next_tuple(uint8_t dev_addr, uint16_t addr, uint8_t *buf_ptr, uint32_t buf_len)
{
    octeon_eeprom_header_t *tlv_hdr_ptr = (void *)buf_ptr;
    uint16_t checksum = 0;
    unsigned int i;


    if (buf_len < sizeof(octeon_eeprom_header_t))
    {
        tlv_debug_print("ERROR: buf_len too small: %d, must be at least %d\n", buf_len, sizeof(octeon_eeprom_header_t));
        return(-1);
    }
    if (octeon_twsi_set_addr16(dev_addr,addr))
    {
        tlv_debug_print("octeon_tlv_eeprom_get_next_tuple: unable to set address: 0x%x, device: 0x%x\n", addr, dev_addr);
        return(-1);
    }
    for (i = 0; i < sizeof(octeon_eeprom_header_t); i++)
    {
        *buf_ptr = (uint8_t)octeon_twsi_read8_cur_addr(dev_addr);
        tlv_debug_print("Read: 0x%x\n", *buf_ptr);
        checksum += *buf_ptr++;
    }

    /* Fix endian issues - data structure in EEPROM is big endian */
    tlv_hdr_ptr->length = ntohs(tlv_hdr_ptr->length);
    tlv_hdr_ptr->type = ntohs(tlv_hdr_ptr->type);
    tlv_hdr_ptr->checksum = ntohs(tlv_hdr_ptr->checksum);
    tlv_hdr_ptr->version = ntohs(tlv_hdr_ptr->version);

    if (tlv_hdr_ptr->type == EEPROM_END_TYPE)
        return -1;

    tlv_debug_print("TLV header at addr 0x%x: type: 0x%x, len: 0x%x, version: 0x%x, checksum: 0x%x\n",
           addr,tlv_hdr_ptr->type, tlv_hdr_ptr->length, tlv_hdr_ptr->version, tlv_hdr_ptr->checksum);

    /* Do basic header check to see if we should continue */

    if (tlv_hdr_ptr->length > OCTEON_EEPROM_MAX_TUPLE_LENGTH
        || tlv_hdr_ptr->length < sizeof(octeon_eeprom_header_t))
    {
        tlv_debug_print("Invalid tuple type/length: type: 0x%x, len: 0x%x\n", tlv_hdr_ptr->type, tlv_hdr_ptr->length);
        return(-1);
    }
    if (buf_len < tlv_hdr_ptr->length)
    {
        tlv_debug_print("Error: buffer length too small.\n");
        return(-1);
    }

    /* Read rest of tuple into buffer */
    for (i = 0; i < (int)tlv_hdr_ptr->length - sizeof(octeon_eeprom_header_t); i++)
    {
        *buf_ptr = (uint8_t)octeon_twsi_read8_cur_addr(dev_addr);
        checksum += *buf_ptr++;
    }

    checksum -= (tlv_hdr_ptr->checksum & 0xff) + (tlv_hdr_ptr->checksum >> 8);

    if (checksum != tlv_hdr_ptr->checksum)
    {
        tlv_debug_print("Checksum mismatch: computed 0x%x, found 0x%x\n", checksum, tlv_hdr_ptr->checksum);
        return(-1);
    }

    return(tlv_hdr_ptr->length);
}

/* find tuple based on type and version.  if not found, returns next available address with bit 31 set (negative).
** If positive value is returned the tuple was found
** if supplied version is zero, matches any version
**
*/
int octeon_tlv_get_tuple_addr(uint8_t dev_addr, uint16_t type, uint16_t version, uint8_t *eeprom_buf, uint32_t buf_len)
{

    octeon_eeprom_header_t *tlv_hdr_ptr = (void *)eeprom_buf;
    int cur_addr = 0;
#ifdef CONFIG_EEPROM_TLV_BASE_ADDRESS
    cur_addr = CONFIG_EEPROM_TLV_BASE_ADDRESS; /* Skip to the beginning of the TLV region. */
#endif

    int len;


    while ((len = octeon_tlv_eeprom_get_next_tuple(dev_addr, cur_addr, eeprom_buf, buf_len)) > 0)
    {
        /* Checksum already validated here, endian swapping of header done in  octeon_tlv_eeprom_get_next_tuple*/
        /* Check to see if we found matching */
        if (type == tlv_hdr_ptr->type && (!version || (version == tlv_hdr_ptr->version)))
        {
            return(cur_addr);
        }
        cur_addr += len;
    }
    /* Return next available address with bit 31 set */
    cur_addr |= (1 << 31);

    return(cur_addr);

}



#ifdef DEBUG
/* Serial Presence Detect (SPD) for DDR2 SDRAM - JEDEC Standard No. 21-C */
/* ===================================================================== */

const char *ddr2_spd_strings[] = {
/* Byte                                                                   */
/* Number  Function                                                       */
/* ======  ============================================================== */
/*  0     */ "Number of Serial PD Bytes written during module production",
/*  1     */ "Total number of Bytes in Serial PD device",
/*  2     */ "Fundamental Memory Type (FPM, EDO, SDRAM, DDR, DDR2)",
/*  3     */ "Number of Row Addresses on this assembly",
/*  4     */ "Number of Column Addresses on this assembly",
/*  5     */ "Number of DIMM Ranks",
/*  6     */ "Data Width of this assembly",
/*  7     */ "Reserved",
/*  8     */ "Voltage Interface Level of this assembly",
/*  9     */ "SDRAM Cycle time at Maximum Supported CAS Latency (CL), CL=X",
/* 10     */ "SDRAM Access from Clock (tAC)",
/* 11     */ "DIMM configuration type (Non-parity, Parity or ECC)",
/* 12     */ "Refresh Rate/Type (tREFI)",
/* 13     */ "Primary SDRAM Width",
/* 14     */ "Error Checking SDRAM Width",
/* 15     */ "Reserved",
/* 16     */ "SDRAM Device Attributes: Burst Lengths Supported",
/* 17     */ "SDRAM Device Attributes: Number of Banks on SDRAM Device",
/* 18     */ "SDRAM Device Attributes: CAS Latency",
/* 19     */ "Reserved",
/* 20     */ "DIMM Type Information",
/* 21     */ "SDRAM Module Attributes",
/* 22     */ "SDRAM Device Attributes: General",
/* 23     */ "Minimum Clock Cycle at CLX-1",
/* 24     */ "Maximum Data Access Time (tAC) from Clock at CLX-1",
/* 25     */ "Minimum Clock Cycle at CLX-2",
/* 26     */ "Maximum Data Access Time (tAC) from Clock at CLX-2",
/* 27     */ "Minimum Row Precharge Time (tRP)",
/* 28     */ "Minimum Row Active to Row Active delay (tRRD)",
/* 29     */ "Minimum RAS to CAS delay (tRCD)",
/* 30     */ "Minimum Active to Precharge Time (tRAS)",
/* 31     */ "Module Rank Density",
/* 32     */ "Address and Command Input Setup Time Before Clock (tIS)",
/* 33     */ "Address and Command Input Hold Time After Clock (tIH)",
/* 34     */ "Data Input Setup Time Before Clock (tDS)",
/* 35     */ "Data Input Hold Time After Clock (tDH)",
/* 36     */ "Write recovery time (tWR)",
/* 37     */ "Internal write to read command delay (tWTR)",
/* 38     */ "Internal read to precharge command delay (tRTP)",
/* 39     */ "Memory Analysis Probe Characteristics",
/* 40     */ "Extension of Byte 41 tRC and Byte 42 tRFC",
/* 41     */ "SDRAM Device Minimum Active to Active/Auto Refresh Time (tRC)",
/* 42     */ "SDRAM Min Auto-Ref to Active/Auto-Ref Command Period (tRFC)",
/* 43     */ "SDRAM Device Maximum device cycle time (tCKmax)",
/* 44     */ "SDRAM Device maximum skew between DQS and DQ signals (tDQSQ)",
/* 45     */ "SDRAM Device Maximum Read DataHold Skew Factor (tQHS)",
/* 46     */ "PLL Relock Time",
/* 47-61  */ "IDD in SPD - To be defined",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/* 62     */ "SPD Revision",
/* 63     */ "Checksum for Bytes 0-62",
/* 64-71  */ "Manufacturers JEDEC ID Code",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/* 72     */ "Module Manufacturing Location",
/* 73-90  */ "Module Part Number",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/* 91-92  */ "Module Revision Code",
/*        */ "",
/* 93-94  */ "Module Manufacturing Date",
/*        */ "",
/* 95-98  */ "Module Serial Number",
/*        */ "",
/*        */ "",
/*        */ "",
/* 99-127 */ "Manufacturers Specific Data",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
/*        */ "",
};
#endif /* DEBUG */
#endif
