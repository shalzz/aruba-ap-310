/*************************************************************************
Copyright (c) 2004-2005 Cavium Networks (support@cavium.com). All rights
reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Cavium Networks' name may not be used to endorse or promote products
derived from this software without specific prior written permission.

This Software, including technical data, may be subject to U.S. export
control laws, including the U.S. Export Administration Act and its
associated regulations, and may be subject to export or import
regulations in other countries. You warrant that You will comply
strictly in all respects with all such regulations and acknowledge that
you have the responsibility to obtain licenses to export, re-export or
import the Software.

TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
AND WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR
WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY)
WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE
RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.

*************************************************************************/

/**
 *
 * $Id: lib_octeon.c,v 1.35 2006/12/13 01:15:38 rfranz Exp $
 *
 */


#include <common.h>
#include <command.h>
#include <exports.h>
#include <linux/ctype.h>
#include "octeon_boot.h"
#include "octeon_hal.h"
#include <octeon_eeprom_types.h>
#include <lib_octeon_shared.h>
#include <flash.h>







#ifndef CONFIG_OCTEON_SIM
/******************  Begin u-boot eeprom hooks ******************************/
/* support for u-boot i2c functions is limited to the standard serial eeprom on the board.
** The do not support reading either the MCU or the DIMM eeproms
*/


/**
 * Reads bytes from eeprom and copies to DRAM.
 * Only supports address size of 2 (16 bit internal address.)
 * 
 * @param chip   chip address
 * @param addr   internal address (both 8 and 16 bit offsets supported for reads)
 * @param alen   address length, must be 2
 * @param buffer memory buffer pointer
 * @param len    number of bytes to read
 * 
 * @return 0 on Success
 *         1 on Failure
 */
int  i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{


//    if (alen != 2 || !buffer || !len)
    if (!buffer || !len)
        return(1);
    if (alen == 2) {
        if (octeon_twsi_set_addr16(chip, addr) < 0)
            return(1);
    } else {
        if (octeon_twsi_set_addr8(chip, addr) < 0)
            return(1);
    }

    while (len--)
    {
        int tmp;
        tmp = octeon_twsi_read8_cur_addr(chip);
        if (tmp < 0)
            return(1);
        *buffer++ = (uchar)(tmp & 0xff);
    }

    return(0);

}

/**
 * Reads bytes from memory and copies to eeprom.
 * Only supports address size of 1 (8 bit internal address.)
 * 
 * We can only write two bytes at a time to the eeprom, so in some cases
 * we need to to a read/modify/write.
 * 
 * Note: can't do single byte write to last address in EEPROM
 * 
 * @param chip   chip address
 * @param addr   internal address (only 8 bit addresses supported)
 * @param alen   address length, must be 1
 * @param buffer memory buffer pointer
 * @param len    number of bytes to write
 * 
 * @return 0 on Success
 *         1 on Failure
 */
 
/* NOTE!!!  Aruba currently is NOT using a device which requires 16bit
 *          offsets, so the code here has been modified to only support 
 *          8bit (1 byte) offsets.  If needed, the time can be spent to
 *          support the alen argument (2 byte offsets) properly.
 */
int  i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{

//    if (alen != 2 || !buffer || !len)
    if (!buffer || !len)
        return(1);
//    if (alen == 2) {
//        if (octeon_twsi_set_addr16(chip, addr) < 0)
//            return(1);
//    } else {
//        if (octeon_twsi_set_addr8(chip, addr) < 0)
//            return(1);
//    }

    while (len > 0)
    {
        if (octeon_twsi_write8(chip, addr, *((uint8_t *)buffer)) < 0)
            return(1);
        len -= 1;
        addr +=1;
        buffer += 1;
    }


#if 0
    /* Handle single (or last) byte case */
    if (len == 1)
    {
        int tmp;

        /* Read 16 bits at addr */
        tmp = octeon_twsi_read16(chip, addr);
        if (tmp < 0)
            return(1);

        tmp &= 0xff; 
        tmp |= (*buffer) << 8;

        if (octeon_twsi_write16(chip, addr, (uint16_t)tmp) < 0)
            return(1);
    }
#endif
    return(0);

}

/*-----------------------------------------------------------------------
 * Probe to see if a chip is present using single byte read.  Also good for checking for the
 * completion of EEPROM writes since the chip stops responding until
 * the write completes (typically 10mSec).
 */
int i2c_probe(uchar addr)
{
    if (octeon_twsi_read8(addr, 0) < 0)
        return(1);
    else
        return(0); /* probed OK */
}

int i2c_reg_write(uchar devid, uint addr,  uchar val) {
    int ret;
    
    ret = i2c_write(devid, addr, 1, &val, 1);
    return ret;
}

uchar i2c_reg_read(uchar devid, uint addr) {
    uchar   val;
    
    i2c_read(devid, addr, 1, &val, 1);
    return val;
}

/******************  End u-boot eeprom hooks ******************************/
#endif


#if CONFIG_OCTEON_EBT3000
int octeon_ebt3000_get_board_major_rev(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return(gd->board_desc.rev_major);
}
int octeon_ebt3000_get_board_minor_rev(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return(gd->board_desc.rev_minor);
}
#endif

int octeon_show_info(void)
{
#ifdef CONFIG_ARUBA_OCTEON
    // GPIO 1 on palomino/moscato is used for something else
    return 1;
#else
    return (!!(octeon_read_gpio() & OCTEON_GPIO_SHOW_FREQ));
#endif
}


#ifdef OCTEON_CHAR_LED_BASE_ADDR
void octeon_led_str_write_std(const char *str)
{
    DECLARE_GLOBAL_DATA_PTR;
    if ((gd->board_desc.board_type == CVMX_BOARD_TYPE_EBT3000) && (gd->board_desc.rev_major == 1))
    {
        char *ptr = (char *)(OCTEON_CHAR_LED_BASE_ADDR + 4);
        int i;
        for (i=0; i<4; i++)
        {
            if (*str)
                ptr[3 - i] = *str++;
            else
                ptr[3 - i] = ' ';
        }
    }
    else
    {
        /* rev 2 ebt3000 or kodama board */
        char *ptr = (char *)(OCTEON_CHAR_LED_BASE_ADDR | 0xf8);
        int i;
        for (i=0; i<8; i++)
        {
            if (*str)
                ptr[i] = *str++;
            else
                ptr[i] = ' ';
        }
    }
}
#else
void octeon_led_str_write_std(const char *str)
{
    /* empty function for when there is no LED */
}
#endif


static int bist_failures = 0;
void displayErrorReg_1(const char *reg_name, uint64_t addr, uint64_t expected, uint64_t mask)
{
    uint64_t bist_val;
    bist_val = octeon_read64(addr);
    bist_val = (bist_val & mask) ^ expected;
    if (bist_val)
    {
        bist_failures++;
        printf("BIST FAILURE: %s, error bits: 0x%Lx\n", reg_name, bist_val);
    }
}
#define COP0_CVMMEMCTL_REG $11,7
void displayErrorRegC0_cvmmem(const char *reg_name, uint64_t expected, uint64_t mask)
{
    uint64_t bist_val;

    {
        uint32_t tmp_low, tmp_hi;
    
        asm volatile (
                   "   .set push                    \n"
                   "   .set mips64                  \n"
                   "   .set noreorder               \n"
                   "   dmfc0 %[tmpl], $11, 7         \n"
                   "   dadd   %[tmph], %[tmpl], $0   \n"
                   "   dsrl  %[tmph], 32            \n"
                   "   dsll  %[tmpl], 32            \n"
                   "   dsrl  %[tmpl], 32            \n"
                   "   .set pop                 \n"
                      : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );
    
        bist_val = (((uint64_t)tmp_hi << 32) | tmp_low);
    }
    bist_val = (bist_val & mask) ^ expected;
    if (bist_val)
    {
        bist_failures++;
        printf("BIST FAILURE: %s, error bits: 0x%Lx\n", reg_name, bist_val);
    }
}
void displayErrorRegC0_cache(const char *reg_name, uint64_t expected, uint64_t mask)
{
    uint64_t bist_val;
    {
        uint32_t tmp_low, tmp_hi;
    
        asm volatile (
                   "   .set push                    \n"
                   "   .set mips64                  \n"
                   "   .set noreorder               \n"
                   "   dmfc0 %[tmpl], $27, 0         \n"
                   "   dadd   %[tmph], %[tmpl], $0   \n"
                   "   dsrl  %[tmph], 32            \n"
                   "   dsll  %[tmpl], 32            \n"
                   "   dsrl  %[tmpl], 32            \n"
                   "   .set pop                     \n"
                      : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );
    
        bist_val = (((uint64_t)tmp_hi << 32) | tmp_low);
    }
    bist_val = (bist_val & mask) ^ expected;
    if (bist_val)
    {
        bist_failures++;
        printf("BIST FAILURE: %s, error bits: 0x%Lx\n", reg_name, bist_val);
    }
}
int octeon_bist()
{

	displayErrorRegC0_cvmmem( "COP0_CVMMEMCTL_REG",       0ull,			0xfc00000000000000ull );
	displayErrorRegC0_cache( "COP0_CACHEERR_REG",      0x0ull,	0x1ull <<32 | 0x1ull <<33 );
	/*                    NAME                   REGISTER              EXPECTED            MASK   */
	displayErrorReg_1  ( "GMX0_BIST",           CVMX_GMXX_BIST(0),        0ull,			0xffffffffffffffffull );
        if (!octeon_is_model(OCTEON_CN31XX) && !octeon_is_model(OCTEON_CN30XX) && !octeon_is_model(OCTEON_CN50XX))
            displayErrorReg_1  ( "GMX1_BIST",           CVMX_GMXX_BIST(1),        0ull,			0xffffffffffffffffull );
	displayErrorReg_1  ( "IPD_BIST_STATUS",     CVMX_IPD_BIST_STATUS,     0ull,			0xffffffffffffffffull );
        if (!octeon_is_model(OCTEON_CN31XX) && !octeon_is_model(OCTEON_CN30XX) && !octeon_is_model(OCTEON_CN50XX))
            displayErrorReg_1  ( "KEY_BIST_REG",        CVMX_KEY_BIST_REG,        0ull,			0xffffffffffffffffull );
	displayErrorReg_1  ( "L2D_BST0",            CVMX_L2D_BST0,            0ull,		    1ull<<34 );
	displayErrorReg_1  ( "L2C_BST0",            CVMX_L2C_BST0,            0,			0x1full );
	displayErrorReg_1  ( "L2C_BST1",            CVMX_L2C_BST1,            0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "L2C_BST2",            CVMX_L2C_BST2,            0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "CIU_BIST",            CVMX_CIU_BIST,            0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "NPI_BIST_STATUS",     CVMX_NPI_BIST_STATUS,     0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "PIP_BIST_STATUS",     CVMX_PIP_BIST_STATUS,     0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "PKO_REG_BIST_RESULT", CVMX_PKO_REG_BIST_RESULT, 0,			0xffffffffffffffffull );
        /* Mask POW BIST_STAT with coremask so we don't report errors on known bad cores  */
        uint32_t val = (uint32_t)octeon_read64(CVMX_POW_BIST_STAT);
        char *cm_str = getenv("coremask_override");
        if (cm_str)
        {
            uint32_t cm_override = simple_strtoul(cm_str, NULL, 0);
            /* Shift coremask override to match up with core BIST bits in register */
            cm_override = cm_override << 16;
            val &= cm_override;
        }
        if (val)
        {
            printf("BIST FAILURE: POW_BIST_STAT: 0x%08x\n", val);
            bist_failures++;
        }

	displayErrorReg_1  ( "RNM_BIST_STATUS",     CVMX_RNM_BIST_STATUS,     0,			0xffffffffffffffffull );
        if (!octeon_is_model(OCTEON_CN31XX) && !octeon_is_model(OCTEON_CN30XX) && !octeon_is_model(OCTEON_CN50XX))
        {
            displayErrorReg_1  ( "SPX0_BIST_STAT",      CVMX_SPXX_BIST_STAT(0),   0,			0xffffffffffffffffull );
            displayErrorReg_1  ( "SPX1_BIST_STAT",      CVMX_SPXX_BIST_STAT(1),   0,			0xffffffffffffffffull );
        }
	displayErrorReg_1  ( "TIM_REG_BIST_RESULT", CVMX_TIM_REG_BIST_RESULT, 0,			0xffffffffffffffffull );
        if (!octeon_is_model(OCTEON_CN30XX) && !octeon_is_model(OCTEON_CN50XX))
            displayErrorReg_1  ( "TRA_BIST_STATUS",     CVMX_TRA_BIST_STATUS,     0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "MIO_BOOT_BIST_STAT",  CVMX_MIO_BOOT_BIST_STAT,  0,			0xffffffffffffffffull );
	displayErrorReg_1  ( "IOB_BIST_STATUS",     CVMX_IOB_BIST_STATUS,     0,			0xffffffffffffffffull );
        if (!octeon_is_model(OCTEON_CN30XX) && !octeon_is_model(OCTEON_CN50XX))
        {
            displayErrorReg_1  ( "DFA_BST0",            CVMX_DFA_BST0,            0,			0xffffffffffffffffull );
            displayErrorReg_1  ( "DFA_BST1",            CVMX_DFA_BST1,            0,			0xffffffffffffffffull );
        }
	displayErrorReg_1  ( "FPA_BIST_STATUS",     CVMX_FPA_BIST_STATUS,     0,			0xffffffffffffffffull );
        if (!octeon_is_model(OCTEON_CN30XX) && !octeon_is_model(OCTEON_CN50XX))
            displayErrorReg_1  ( "ZIP_CMD_BIST_RESULT", CVMX_ZIP_CMD_BIST_RESULT, 0,			0xffffffffffffffffull );

    if (bist_failures)
    {
        printf("BIST:  failed; ");
        printf("'1' bits above indicate unexpected BIST status.\n");
    }
    else
    {
        printf("BIST:  passed\n");
    }
    return(0);
}



void octeon_flush_l2_cache(void)
{
    uint64_t assoc, set;
    cvmx_l2c_dbg_t l2cdbg;

    int l2_sets = 0;
    int l2_assoc = 0;
    if (octeon_is_model(OCTEON_CN30XX))
    {
        l2_sets  = 256;
        l2_assoc = 4;
    }
    else if (octeon_is_model(OCTEON_CN31XX))
    {
        l2_sets  = 512;
        l2_assoc = 4;
    }
    else if (octeon_is_model(OCTEON_CN38XX))
    {
        l2_sets  = 1024;
        l2_assoc = 8;
    }
    else if (octeon_is_model(OCTEON_CN58XX))
    {
        l2_sets  = 2048;
        l2_assoc = 8;
    }
    else if (octeon_is_model(OCTEON_CN50XX))
    {
        l2_sets  = 128;
        l2_assoc = 8;
    }
    else
    {
        printf("ERROR: unsupported Octeon model\n");
    }


    l2cdbg.u64 = 0;
    l2cdbg.s.ppnum = get_core_num();
    l2cdbg.s.finv = 1;
    for(set=0; set < l2_sets; set++)
    {
        for(assoc = 0; assoc < l2_assoc; assoc++)
        {
            l2cdbg.s.set = assoc;
            /* Enter debug mode, and make sure all other writes complete before we
            ** enter debug mode */
            OCTEON_SYNC;
            cvmx_write_csr(CVMX_L2C_DBG, l2cdbg.u64);
            cvmx_read_csr(CVMX_L2C_DBG);

            CVMX_PREPARE_FOR_STORE (1ULL << 63 | set*CVMX_CACHE_LINE_SIZE, 0);
            /* Exit debug mode */
            OCTEON_SYNC;
            cvmx_write_csr(CVMX_L2C_DBG, 0);
            cvmx_read_csr(CVMX_L2C_DBG);
        }
    }

}

#ifndef CONFIG_OCTEON_SIM
/* Boot bus init for flash and peripheral access */
#define FLASH_RoundUP(_Dividend, _Divisor) (((_Dividend)+(_Divisor))/(_Divisor))
#undef ECLK_PERIOD
#define ECLK_PERIOD	(1000000/1000)	/* eclk period (psecs) */
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips */
int octeon_boot_bus_init(void)
{
    cvmx_mio_boot_reg_cfgx_t reg_cfg;
    cvmx_mio_boot_reg_timx_t __attribute__((unused)) reg_tim;

    /* Drive boot bus chip enables [7:4] on gpio [11:8] */
    if (octeon_is_model(OCTEON_CN31XX) || octeon_is_model(OCTEON_CN30XX) || octeon_is_model(OCTEON_CN50XX))
        octeon_write64((uint64_t)0x80010700000008A8ull, 0xf<<8);

#ifdef CFG_FLASH_SIZE
    /* Remap flash part so that it is all addressable on boot bus, with alias
    ** at 0x1fc00000 so that the data mapped at the default location (0x1fc00000) is
    ** still available at the same address */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
#if CONFIG_OCTEON_HIKARI
    reg_cfg.s.ale = 1;
#endif
#if CONFIG_OCTEON_BBGW_REF
    reg_cfg.s.ale = 1;
#endif
    reg_cfg.s.size = ((CFG_FLASH_SIZE + 0x400000) >> 16) - 1;  /* In 64k blocks, + 4MByte alias of low 4Mbytes of flash */
    reg_cfg.s.base = ((CFG_FLASH_BASE >> 16) & 0x1fff);  /* Mask to put physical address in boot bus window */
    octeon_write64(CVMX_MIO_BOOT_REG_CFG0, reg_cfg.u64);

#if !(CONFIG_OCTEON_TRANTOR)  /* Leave flash slow for Trantor */
    /* Set timing to be valid for all CPU clocks up to 600 Mhz */
    reg_tim.u64 = 0;
    reg_tim.s.pagem = 0;
    reg_tim.s.wait = 0x3f;
    reg_tim.s.adr = FLASH_RoundUP((FLASH_RoundUP(10000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.pause = 0;
    reg_tim.s.ce = FLASH_RoundUP((FLASH_RoundUP(50000ULL, ECLK_PERIOD) - 1), 4);;
    if (octeon_is_model(OCTEON_CN31XX))
        reg_tim.s.ale = 4; /* Leave ALE at 34 nS */
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(50000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(25000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.wr_hld = FLASH_RoundUP((FLASH_RoundUP(35000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = FLASH_RoundUP((FLASH_RoundUP(35000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.page = FLASH_RoundUP((FLASH_RoundUP(25000ULL, ECLK_PERIOD) - 1), 4);
    octeon_write64(CVMX_MIO_BOOT_REG_TIM0, reg_tim.u64);
#endif
#endif

    /* Set up regions for CompactFlash */
    /* Attribute memory region */
#ifdef OCTEON_CF_ATTRIB_CHIP_SEL
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
#ifdef OCTEON_CF_16_BIT_BUS
    reg_cfg.s.width = 1;
#endif
    reg_cfg.s.base = ((OCTEON_CF_ATTRIB_BASE_ADDR >> 16) & 0x1fff);  /* Mask to put physical address in boot bus window */
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(OCTEON_CF_ATTRIB_CHIP_SEL), reg_cfg.u64);

    reg_tim.u64 = 0;
    reg_tim.s.wait = 0x3f;
    reg_tim.s.page = 0x3f;
    reg_tim.s.wr_hld = FLASH_RoundUP((FLASH_RoundUP(70000ULL, ECLK_PERIOD) - 2), 4);
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(100000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = FLASH_RoundUP((FLASH_RoundUP(150000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(200000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.ce = FLASH_RoundUP((FLASH_RoundUP(30000ULL, ECLK_PERIOD) - 2), 4);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(OCTEON_CF_ATTRIB_CHIP_SEL), reg_tim.u64);
#endif

#ifdef OCTEON_CF_COMMON_CHIP_SEL
    /* Common memory region */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
#ifdef OCTEON_CF_16_BIT_BUS
    reg_cfg.s.width = 1;
#endif
    reg_cfg.s.base = ((OCTEON_CF_COMMON_BASE_ADDR >> 16) & 0x1fff);  /* Mask to put physical address in boot bus window */
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(OCTEON_CF_COMMON_CHIP_SEL), reg_cfg.u64);

    reg_tim.u64 = 0;
    reg_tim.s.wait = (FLASH_RoundUP(30000ULL, ECLK_PERIOD) - 1);
    reg_tim.s.waitm = 1;
    reg_tim.s.page = 0x3f;
    reg_tim.s.wr_hld = FLASH_RoundUP((FLASH_RoundUP(30000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.rd_hld = FLASH_RoundUP((FLASH_RoundUP(100000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.we = FLASH_RoundUP((FLASH_RoundUP(150000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.oe = FLASH_RoundUP((FLASH_RoundUP(125000ULL, ECLK_PERIOD) - 1), 4);
    reg_tim.s.ce = FLASH_RoundUP((FLASH_RoundUP(30000ULL, ECLK_PERIOD) - 2), 4);
    octeon_write64(CVMX_MIO_BOOT_REG_TIMX(OCTEON_CF_COMMON_CHIP_SEL), reg_tim.u64);
#endif

#ifdef OCTEON_CHAR_LED_CHIP_SEL
    /* Setup region for 4 char LED display */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.base = ((OCTEON_CHAR_LED_BASE_ADDR >> 16) & 0x1fff);  /* Mask to put physical address in boot bus window */
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(4), reg_cfg.u64);
#endif

#ifdef OCTEON_PAL_CHIP_SEL
    /* Setup region for PAL access */
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.base = ((OCTEON_PAL_BASE_ADDR >> 16) & 0x1fff);  /* Mask to put physical address in boot bus window */
    octeon_write64(CVMX_MIO_BOOT_REG_CFGX(OCTEON_PAL_CHIP_SEL), reg_cfg.u64);
#endif

#if CONFIG_RAM_RESIDENT
    /* Install the 2nd moveable region to the debug exception main entry
        point. This way the debugger will work even if there isn't any
        flash. */
    extern void debugHandler_entrypoint(void);
    const uint64_t *handler_code = (const uint64_t *)debugHandler_entrypoint;
    int count;
    octeon_write64(CVMX_MIO_BOOT_LOC_CFGX(1), (1<<31) | (0x1fc00480>>7<<3));
    octeon_write64(CVMX_MIO_BOOT_LOC_ADR, 0x80);
    for (count=0; count<128/8; count++)
        octeon_write64(CVMX_MIO_BOOT_LOC_DAT, *handler_code++);
#endif

    /* Slow down the TWSI clock, as some boards (Thunder) seem to need it slower
    ** than the default, especially when a spi4000 is connected. */
    cvmx_mio_tws_sw_twsi_t sw_twsi;
    sw_twsi.u64 = 0;
    sw_twsi.s.v = 1;
    sw_twsi.s.op = 0x6;
    sw_twsi.s.eop_ia = 0x3;
    sw_twsi.s.d = (0x5 << 3);  /* Set M divider to 5 (2 is default), which halves the clock */
    octeon_write64(CVMX_MIO_TWS_SW_TWSI, sw_twsi.u64);
    return(0);
}
#endif
