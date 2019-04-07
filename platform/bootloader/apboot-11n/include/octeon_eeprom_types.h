/*************************************************************************
Copyright (c) 2005 Cavium Networks (support@cavium.com). All rights
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

#ifndef __OCTEON_EEPROM_TYPES_H__
#define __OCTEON_EEPROM_TYPES_H__

#include <cvmx-app-init.h>


enum eeprom_types_enum {
    EEPROM_NULL_TYPE = 0,
    EEPROM_CLOCK_DESC_TYPE,
    EEPROM_BOARD_DESC_TYPE,
    EEPROM_CHIP_CAPABILITY_TYPE,
    EEPROM_MAC_ADDR_TYPE,
    EEPROM_VOLT_MULT_TYPE,
    EEPROM_NIC_XL_DESC_TYPE,
    EEPROM_MAX_TYPE,
    EEPROM_CUSTOMER_RESERVED_START = 0xf000,  /* Start of range (inclusive) for customer use */
    EEPROM_CUSTOMER_RESERVED_END = 0xff00,    /* End of range (inclusive) for customer use */
    EEPROM_END_TYPE = 0xffff
};

#define ENUM_TYPE_CASE(x)   case x: return(#x + 7);   /* Skip EEPROM_ */
static inline char *eeprom_type_to_string(enum eeprom_types_enum type)
{
    if (type >= EEPROM_CUSTOMER_RESERVED_START && type <= EEPROM_CUSTOMER_RESERVED_END)
        return("Unknown customer tuple type");
    if (type >= EEPROM_MAX_TYPE)
        return("Unknown tuple type");

    switch (type)
    {
        ENUM_TYPE_CASE(EEPROM_NULL_TYPE)
        ENUM_TYPE_CASE(EEPROM_CLOCK_DESC_TYPE)
        ENUM_TYPE_CASE(EEPROM_BOARD_DESC_TYPE)
        ENUM_TYPE_CASE(EEPROM_CHIP_CAPABILITY_TYPE)
        ENUM_TYPE_CASE(EEPROM_MAC_ADDR_TYPE)
        ENUM_TYPE_CASE(EEPROM_VOLT_MULT_TYPE)
        ENUM_TYPE_CASE(EEPROM_NIC_XL_DESC_TYPE)
        ENUM_TYPE_CASE(EEPROM_MAX_TYPE)
        ENUM_TYPE_CASE(EEPROM_CUSTOMER_RESERVED_START)
        ENUM_TYPE_CASE(EEPROM_CUSTOMER_RESERVED_END)
        ENUM_TYPE_CASE(EEPROM_END_TYPE)

    }
    return("Unknown tuple type");
}

/* Header used by all blocks in the EEPROM.  Checksum covers
** the header (excluding the checksum) and all payload bytes.
*/
typedef  struct {
    uint16_t type;
    uint16_t length;
    uint16_t version;
    uint16_t checksum;
} octeon_eeprom_header_t;


/* Maximum length allowed for a tuple.  This is used for simple
** header validity checks.
*/
#define OCTEON_EEPROM_MAX_TUPLE_LENGTH   256


/*
** NOTE: The clock descriptor is used by the MCU, so any changes made to the
** structure/type definitions must be made there as well.
*/

struct octeon_eeprom_clock_desc_v1 {
    octeon_eeprom_header_t header;
    uint16_t    ddr_clock_mhz;
    uint16_t    cpu_ref_clock_mhz_x_8;   /* Ref clock Mhz * 8 */
    uint16_t    spi_clock_mhz;
};

/* Unused fields should be set to 0 */
struct octeon_eeprom_clock_desc_v2 {
    octeon_eeprom_header_t header;
    uint16_t    ddr_clock_mhz;
    uint16_t    cpu_ref_clock_mhz_x_8;  /* Ref clock Mhz * 8 */
    uint16_t    dfa_ref_clock_mhz_x_8;  /* Ref clock Mhz * 8 */
    uint16_t    spi_clock_mhz;
};

/* We'll need to change this based on chip type, as Octeon uses version 1, and newer 
** chips use version 2 */
/* This define always corresponds to the latest structure version, and the 
** latest structure version always has the generic name */
#define OCTEON_EEPROM_CLOCK_DESC_VER    2
typedef struct octeon_eeprom_clock_desc_v2 octeon_eeprom_clock_desc_t;

struct octeon_eeprom_cpu_mul_vol_desc_v1 {
     octeon_eeprom_header_t header;
     uint16_t    cpu_voltage_millivolts; /* voltage in millivolts */
     uint16_t    cpu_multiplier;         /* Raw value */
};
#define OCTEON_EEPROM_CPU_MUL_VOL_DESC_VER    1
typedef struct octeon_eeprom_cpu_mul_vol_desc_v1 octeon_eeprom_cpu_mul_vol_desc_t;





#ifndef OCTEON_SERIAL_LEN
#define OCTEON_SERIAL_LEN 20
#endif
struct octeon_eeprom_board_desc_v1 {
    octeon_eeprom_header_t header;
    uint16_t    board_type;
    uint8_t     rev_major;
    uint8_t     rev_minor;
    uint16_t    chip_type;  /* Deprecated */
    uint8_t     chip_rev_major;  /* Deprecated */
    uint8_t     chip_rev_minor;  /* Deprecated */
    uint8_t     serial_str[OCTEON_SERIAL_LEN];  /* Must be nul terminated */
};
#define OCTEON_EEPROM_BOARD_DESC_VER    1
typedef struct octeon_eeprom_board_desc_v1 octeon_eeprom_board_desc_t;




struct octeon_eeprom_chip_capability_v1 {
    octeon_eeprom_header_t header;
    uint16_t coremask;
    uint16_t voltage_x100;
    uint16_t cpu_freq_mhz;
};
#define OCTEON_EEPROM_CHIP_CAPABILITY_VER    1
typedef struct octeon_eeprom_chip_capability_v1 octeon_eeprom_chip_capability_t;


struct octeon_eeprom_mac_addr_v1 {
    octeon_eeprom_header_t header;
    uint8_t mac_addr_base[6];
    uint8_t count;
};
#define OCTEON_EEPROM_MAC_ADDR_VER    1
typedef struct octeon_eeprom_mac_addr_v1 octeon_eeprom_mac_addr_t;

struct octeon_eeprom_voltage_multiplier_v1 {
    octeon_eeprom_header_t header;
    uint16_t voltage_millivolts;
    uint16_t cpu_multiplier;
};
#define OCTEON_EEPROM_VOLT_MUTL_VER    1
typedef struct octeon_eeprom_voltage_multiplier_v1 octeon_eeprom_voltage_multiplier_t;

struct octeon_eeprom_nic_xl_desc_v1 {
    octeon_eeprom_header_t header;
    uint16_t feature_bitmask;
    uint16_t llm_port_0_mbytes;
    uint16_t llm_port_1_mbytes;
};
#define OCTEON_EEPROM_NIC_XL_DESC_VER    1
typedef struct octeon_eeprom_nic_xl_desc_v1 octeon_eeprom_nic_xl_desc_t;

/* Bit definitions for Octeon NIC XL feature bitmask */
#define OCTEON_EEPROM_NIC_XL_FEATURE_LAN_BYPASS (1 << 0)
#define OCTEON_EEPROM_NIC_XL_FEATURE_CF_SLOT (1 << 1)

#endif /* __OCTEON_EEPROM_TYPES_H__ */
