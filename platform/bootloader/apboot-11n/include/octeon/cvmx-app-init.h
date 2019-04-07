/***********************license start***************
 * Copyright (c) 2003-2008  Cavium Networks (support@cavium.com). All rights
 * reserved.
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
 *
 * For any questions regarding licensing please contact marketing@caviumnetworks.com
 *
 ***********************license end**************************************/





/**
 * @file
 * Header file for simple executive application initialization.  This defines
 * part of the ABI between the bootloader and the application.
 * <hr>$Revision: 33837 $<hr>
 *
 */

#ifndef __CVMX_APP_INIT_H__
#define __CVMX_APP_INIT_H__

#ifdef	__cplusplus
extern "C" {
#endif


/* Current major and minor versions of the CVMX bootinfo block that is passed
** from the bootloader to the application.  This is versioned so that applications
** can properly handle multiple bootloader versions. */
#define CVMX_BOOTINFO_MAJ_VER 1
#define CVMX_BOOTINFO_MIN_VER 2


#if (CVMX_BOOTINFO_MAJ_VER == 1)
#define CVMX_BOOTINFO_OCTEON_SERIAL_LEN 20
/* This structure is populated by the bootloader.  For binary
** compatibility the only changes that should be made are
** adding members to the end of the structure, and the minor
** version should be incremented at that time.
** If an incompatible change is made, the major version
** must be incremented, and the minor version should be reset
** to 0.
*/
typedef struct
{
    uint32_t major_version;
    uint32_t minor_version;

    uint64_t stack_top;
    uint64_t heap_base;
    uint64_t heap_end;
    uint64_t desc_vaddr;

    uint32_t exception_base_addr;
    uint32_t stack_size;
    uint32_t flags;
    uint32_t core_mask;
    uint32_t dram_size;  /**< DRAM size in megabytes */
    uint32_t phy_mem_desc_addr;  /**< physical address of free memory descriptor block*/
    uint32_t debugger_flags_base_addr;  /**< used to pass flags from app to debugger */
    uint32_t eclock_hz;  /**< CPU clock speed, in hz */
    uint32_t dclock_hz;  /**< DRAM clock speed, in hz */
    uint32_t reserved0;
    uint16_t board_type;
    uint8_t board_rev_major;
    uint8_t board_rev_minor;
    uint16_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    char board_serial_number[CVMX_BOOTINFO_OCTEON_SERIAL_LEN];
    uint8_t mac_addr_base[6];
    uint8_t mac_addr_count;
#if (CVMX_BOOTINFO_MIN_VER >= 1)
    /* Several boards support compact flash on the Octeon boot bus.  The CF
    ** memory spaces may be mapped to different addresses on different boards.
    ** These are the physical addresses, so care must be taken to use the correct
    ** XKPHYS/KSEG0 addressing depending on the application's ABI.
    ** These values will be 0 if CF is not present */
    uint64_t compact_flash_common_base_addr;
    uint64_t compact_flash_attribute_base_addr;
    /* Base address of the LED display (as on EBT3000 board)
    ** This will be 0 if LED display not present. */
    uint64_t led_display_base_addr;
#endif
#if (CVMX_BOOTINFO_MIN_VER >= 2)
    uint32_t dfa_ref_clock_hz;  /**< DFA reference clock in hz (if applicable)*/
    uint32_t config_flags;  /**< flags indicating various configuration options.  These flags supercede
                            ** the 'flags' variable and should be used instead if available */
#endif


} cvmx_bootinfo_t;

#define CVMX_BOOTINFO_CFG_FLAG_PCI_HOST			(1ull << 0)
#define CVMX_BOOTINFO_CFG_FLAG_PCI_TARGET		(1ull << 1)
#define CVMX_BOOTINFO_CFG_FLAG_DEBUG			(1ull << 2)
#define CVMX_BOOTINFO_CFG_FLAG_NO_MAGIC			(1ull << 3)
/* This flag is set if the TLB mappings are not contained in the
** 0x10000000 - 0x20000000 boot bus region. */
#define CVMX_BOOTINFO_CFG_FLAG_OVERSIZE_TLB_MAPPING     (1ull << 4)
#define CVMX_BOOTINFO_CFG_FLAG_BREAK			(1ull << 5)

#endif /*   (CVMX_BOOTINFO_MAJ_VER == 1) */


/* Type defines for board and chip types */
enum cvmx_board_types_enum {
    CVMX_BOARD_TYPE_NULL = 0,
    CVMX_BOARD_TYPE_SIM,
    CVMX_BOARD_TYPE_EBT3000,
    CVMX_BOARD_TYPE_KODAMA,
    CVMX_BOARD_TYPE_NIAGARA,
    CVMX_BOARD_TYPE_NAC38,	/* formerly NAO38 */
    CVMX_BOARD_TYPE_THUNDER,
    CVMX_BOARD_TYPE_TRANTOR,
    CVMX_BOARD_TYPE_EBH3000,
    CVMX_BOARD_TYPE_EBH3100,
    CVMX_BOARD_TYPE_HIKARI,
    CVMX_BOARD_TYPE_CN3010_EVB_HS5,
    CVMX_BOARD_TYPE_CN3005_EVB_HS5,
    CVMX_BOARD_TYPE_KBP,
    CVMX_BOARD_TYPE_CN3020_EVB_HS5,  /* Deprecated, CVMX_BOARD_TYPE_CN3010_EVB_HS5 supports the CN3020 */
    CVMX_BOARD_TYPE_EBT5800,
    CVMX_BOARD_TYPE_NICPRO2,
    CVMX_BOARD_TYPE_EBH5600,
    CVMX_BOARD_TYPE_EBH5601,
    CVMX_BOARD_TYPE_EBH5200,
    CVMX_BOARD_TYPE_BBGW_REF,
    CVMX_BOARD_TYPE_MAX,

    /* The range from CVMX_BOARD_TYPE_MAX to CVMX_BOARD_TYPE_CUST_DEFINED_MIN is reserved
    ** for future SDK use. */

    /* Set aside a range for customer boards.  These numbers are managed
    ** by Cavium.
    */
    CVMX_BOARD_TYPE_CUST_DEFINED_MIN = 10000,
    CVMX_BOARD_TYPE_CUST_WSX16       = 10001,
    CVMX_BOARD_TYPE_CUST_NS0216      = 10002,
    CVMX_BOARD_TYPE_CUST_NB5         = 10003,
    CVMX_BOARD_TYPE_CUST_WMR500      = 10004,
    CVMX_BOARD_TYPE_CUST_DEFINED_MAX = 20000,

    /* Set aside a range for customer private use.  The SDK won't
    ** use any numbers in this range. */
    CVMX_BOARD_TYPE_CUST_PRIVATE_MIN = 20001,
    CVMX_BOARD_TYPE_CUST_PRIVATE_MAX = 30000,

    /* The remaining range is reserved for future use. */
};
enum cvmx_chip_types_enum {
    CVMX_CHIP_TYPE_NULL = 0,
    CVMX_CHIP_SIM_TYPE_DEPRECATED = 1,
    CVMX_CHIP_TYPE_OCTEON_SAMPLE = 2,
    CVMX_CHIP_TYPE_MAX,
};

/* Compatability alias for NAC38 name change, planned to be removed from SDK 1.7 */
#define CVMX_BOARD_TYPE_NAO38	CVMX_BOARD_TYPE_NAC38

/* Functions to return string based on type */
#define ENUM_BRD_TYPE_CASE(x)   case x: return(#x + 16);   /* Skip CVMX_BOARD_TYPE_ */
static inline const char *cvmx_board_type_to_string(enum cvmx_board_types_enum type)
{
    switch (type)
    {
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_NULL)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_SIM)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBT3000)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_KODAMA)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_NIAGARA)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_NAC38)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_THUNDER)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_TRANTOR)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBH3000)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBH3100)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_HIKARI)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CN3010_EVB_HS5)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CN3005_EVB_HS5)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_KBP)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CN3020_EVB_HS5)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBT5800)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_NICPRO2)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBH5600)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBH5601)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_EBH5200)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_BBGW_REF)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_MAX)

        /* Customer boards listed here */
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_DEFINED_MIN)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_WSX16)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_NS0216)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_NB5)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_WMR500)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_DEFINED_MAX)

        /* Customer private range */
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_PRIVATE_MIN)
        ENUM_BRD_TYPE_CASE(CVMX_BOARD_TYPE_CUST_PRIVATE_MAX)
    }
    return "Unsupported Board";
}

#define ENUM_CHIP_TYPE_CASE(x)   case x: return(#x + 15);   /* Skip CVMX_CHIP_TYPE */
static inline const char *cvmx_chip_type_to_string(enum cvmx_chip_types_enum type)
{
    switch (type)
    {
        ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_NULL)
        ENUM_CHIP_TYPE_CASE(CVMX_CHIP_SIM_TYPE_DEPRECATED)
        ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_OCTEON_SAMPLE)
        ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_MAX)
    }
    return "Unsupported Chip";
}






#ifdef	__cplusplus
}
#endif

#endif /* __CVMX_APP_INIT_H__ */
