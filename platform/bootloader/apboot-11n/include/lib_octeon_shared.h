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


#ifndef __LIB_OCTEON_SHARED_H__
#define __LIB_OCTEON_SHARED_H__

#define CALC_SCALER 10000

/* Structure for bootloader pci IO buffers */
typedef struct
{
    uint32_t owner;
    uint32_t len;
    char data[0];
} octeon_pci_io_buf_t;
#define BOOTLOADER_PCI_READ_BUFFER_STR_LEN  (BOOTLOADER_PCI_READ_BUFFER_SIZE - 8)
#define BOOTLOADER_PCI_WRITE_BUFFER_STR_LEN  (BOOTLOADER_PCI_WRITE_BUFFER_SIZE - 8)

#define BOOTLOADER_PCI_READ_BUFFER_OWNER_ADDR   (BOOTLOADER_PCI_READ_BUFFER_BASE + 0)
#define BOOTLOADER_PCI_READ_BUFFER_LEN_ADDR   (BOOTLOADER_PCI_READ_BUFFER_BASE + 4)
#define BOOTLOADER_PCI_READ_BUFFER_DATA_ADDR   (BOOTLOADER_PCI_READ_BUFFER_BASE + 8)

enum octeon_pci_io_buf_owner
{
    OCTEON_PCI_IO_BUF_OWNER_INVALID = 0,  /* Must be zero, set when memory cleared */
    OCTEON_PCI_IO_BUF_OWNER_OCTEON = 1,
    OCTEON_PCI_IO_BUF_OWNER_HOST = 2,
};

/* data field addresses in the DDR2 SPD eeprom */
typedef enum ddr2_spd_addrs {
    DDR2_SPD_BYTES_PROGRAMMED	= 0,
    DDR2_SPD_TOTAL_BYTES	= 1,
    DDR2_SPD_MEM_TYPE		= 2,
    DDR2_SPD_NUM_ROW_BITS	= 3,
    DDR2_SPD_NUM_COL_BITS	= 4,
    DDR2_SPD_NUM_RANKS		= 5,
    DDR2_SPD_CYCLE_CLX		= 9,
    DDR2_SPD_CONFIG_TYPE	= 11,
    DDR2_SPD_REFRESH		= 12,
    DDR2_SPD_SDRAM_WIDTH	= 13,
    DDR2_SPD_BURST_LENGTH	= 16,
    DDR2_SPD_NUM_BANKS		= 17,
    DDR2_SPD_CAS_LATENCY	= 18,
    DDR2_SPD_DIMM_TYPE		= 20,
    DDR2_SPD_CYCLE_CLX1		= 23,
    DDR2_SPD_CYCLE_CLX2		= 25,
    DDR2_SPD_TRP		= 27,
    DDR2_SPD_TRRD 		= 28,
    DDR2_SPD_TRCD 		= 29,
    DDR2_SPD_TRAS 		= 30,
    DDR2_SPD_TWR 		= 36,
    DDR2_SPD_TWTR 		= 37,
    DDR2_SPD_TRFC_EXT		= 40,
    DDR2_SPD_TRFC 		= 42,
    DDR2_SPD_MFR_ID		= 64,
} ddr2_spd_addr_t;


/*
** DRAM Module Organization
**
** Octeon:
** Octeon can be configured to use two pairs of DIMM's, lower and
** upper, providing a 128/144-bit interface or one to four DIMM's
** providing a 64/72-bit interface.  This structure contains the TWSI
** addresses used to access the DIMM's Serial Presence Detect (SPD)
** EPROMS and it also implies which DIMM socket organization is used
** on the board.  Software uses this to detect the presence of DIMM's
** plugged into the sockets, compute the total memory capacity, and
** configure DRAM controller.  All DIMM's must be identical.
**
** CN31XX:
** Octeon CN31XX can be configured to use one to four DIMM's providing
** a 64/72-bit interface.  This structure contains the TWSI addresses
** used to access the DIMM's Serial Presence Detect (SPD) EPROMS and
** it also implies which DIMM socket organization is used on the
** board.  Software uses this to detect the presence of DIMM's plugged
** into the sockets, compute the total memory capacity, and configure
** DRAM controller.  All DIMM's must be identical.
*/
typedef struct {
    unsigned int lower_spd_addr;
    unsigned int upper_spd_addr; /* Only used on 38XX */
} dimm_config_t;


typedef struct {
    unsigned int odt_ena;
    unsigned int odt_mask;
    unsigned int odt_mask1;
    unsigned int qs_dic;
    unsigned int rodt_ctl;
    unsigned int dic;
} dimm_odt_config_t;

typedef struct {
    unsigned int lmc_delay_clk;
    unsigned int lmc_delay_cmd;
    unsigned int lmc_delay_dq; 
} ddr_tweak_t;

typedef struct {
    uint32_t ddr_board_delay;
    dimm_config_t *dimm_config_table;
    dimm_odt_config_t *odt_1rank_config;
    dimm_odt_config_t *odt_2rank_config;
} ddr_configuration_t;

extern const dimm_odt_config_t disable_odt_config[];
extern const dimm_odt_config_t single_rank_odt_config[];
extern const dimm_odt_config_t dual_rank_odt_config[];


int init_octeon_dram_interface(uint32_t cpu_id,
                               ddr_configuration_t *ddr_configuration,
                               uint32_t ddr_hertz,
                               uint32_t cpu_hertz,
                               uint32_t ddr_ref_hertz,
                               int board_type,
                               int board_rev_maj,
                               int board_rev_min
                               );
int initialize_ddr_clock(uint32_t cpu_id,
                         uint32_t cpu_hertz,
                         uint32_t ddr_hertz,
                         uint32_t ddr_ref_hertz
                         );
uint32_t measure_octeon_ddr_clock(uint32_t cpu_id,
                                  uint32_t cpu_hertz,
                                  uint32_t ddr_hertz,
                                  uint32_t ddr_ref_hertz
                                  );

int twsii_mcu_read(uint8_t twsii_addr);
int octeon_twsi_read(uint8_t dev_addr, uint8_t twsii_addr);

int twsii_mcu_read(uint8_t twsii_addr);
int octeon_twsi_read16(uint8_t dev_addr, uint16_t addr);
int octeon_twsi_write16(uint8_t dev_addr, uint16_t addr, uint16_t data);
int octeon_twsi_read16_cur_addr(uint8_t dev_addr);
int octeon_twsi_read8(uint8_t dev_addr, uint16_t addr);
int octeon_twsi_write8(uint8_t dev_addr, uint16_t addr, uint8_t data);
int octeon_twsi_read8_cur_addr(uint8_t dev_addr);
int octeon_twsi_set_addr16(uint8_t dev_addr, uint16_t addr);
int octeon_twsi_set_addr8(uint8_t dev_addr, uint16_t addr);

int octeon_tlv_get_tuple_addr(uint8_t dev_addr, uint16_t type, uint16_t version, uint8_t *eeprom_buf, uint32_t buf_len);

int  octeon_tlv_eeprom_get_next_tuple(uint8_t dev_addr, uint16_t addr, uint8_t *buf_ptr, uint32_t buf_len);

#ifdef ENABLE_BOARD_DEBUG
void gpio_status(int data);
#endif

#define CN31XX_DRAM_ODT_1RANK_CONFIGURATION \
    /* DIMMS   ODT_ENA  WODT_CTL0     WODT_CTL1   QS_DIC RODT_CTL DIC */ \
    /* =====   ======= ============ ============= ====== ======== === */ \
    /*   1 */ {   0,    0x00000100,   0x00000000,    1,   0x0000,  0  },  \
    /*   2 */ {   0,    0x01000400,   0x00000000,    1,   0x0000,  0  },  \
    /*   3 */ {   0,    0x01000400,   0x00000400,    2,   0x0000,  0  },  \
    /*   4 */ {   0,    0x01000400,   0x04000400,    2,   0x0000,  0  }

#define CN31XX_DRAM_ODT_2RANK_CONFIGURATION \
    /* DIMMS   ODT_ENA  WODT_CTL0     WODT_CTL1   QS_DIC RODT_CTL DIC */ \
    /* =====   ======= ============ ============= ====== ======== === */ \
    /*   1 */ {   0,    0x00000101,   0x00000000,    1,   0x0000,  0  },  \
    /*   2 */ {   0,    0x01010404,   0x00000000,    1,   0x0000,  0  },  \
    /*   3 */ {   0,    0x01010404,   0x00000404,    2,   0x0000,  0  },  \
    /*   4 */ {   0,    0x01010404,   0x04040404,    2,   0x0000,  0  }

/* CN30xx is the same as CN31xx */
#define CN30XX_DRAM_ODT_1RANK_CONFIGURATION CN31XX_DRAM_ODT_1RANK_CONFIGURATION
#define CN30XX_DRAM_ODT_2RANK_CONFIGURATION CN31XX_DRAM_ODT_2RANK_CONFIGURATION

#define CN38XX_DRAM_ODT_1RANK_CONFIGURATION \
    /* DIMMS   ODT_ENA LMC_ODT_CTL  reserved  QS_DIC RODT_CTL DIC */ \
    /* =====   ======= ============ ========= ====== ======== === */ \
    /*   1 */ {   0,    0x00000001,   0x0000,    1,   0x0000,  0  },  \
    /*   2 */ {   0,    0x00010001,   0x0000,    2,   0x0000,  0  },  \
    /*   3 */ {   0,    0x01040104,   0x0000,    2,   0x0000,  0  },  \
    /*   4 */ {   0,    0x01040104,   0x0000,    2,   0x0000,  0  }

#define CN38XX_DRAM_ODT_2RANK_CONFIGURATION \
    /* DIMMS   ODT_ENA LMC_ODT_CTL  reserved  QS_DIC RODT_CTL DIC */ \
    /* =====   ======= ============ ========= ====== ======== === */ \
    /*   1 */ {   0,    0x00000011,   0x0000,    1,   0x0000,  0  },  \
    /*   2 */ {   0,    0x00110011,   0x0000,    2,   0x0000,  0  },  \
    /*   3 */ {   0,    0x11441144,   0x0000,    3,   0x0000,  0  },  \
    /*   4 */ {   0,    0x11441144,   0x0000,    3,   0x0000,  0  }

/* Note: CN58XX RODT_ENA 0 = disabled, 1 = Weak Read ODT, 2 = Strong Read ODT */
#define CN58XX_DRAM_ODT_1RANK_CONFIGURATION \
    /* DIMMS   RODT_ENA LMC_ODT_CTL  reserved  QS_DIC RODT_CTL DIC */ \
    /* =====   ======== ============ ========= ====== ======== === */ \
    /*   1 */ {   2,    0x00000001,   0x0000,    1,   0x0000,  0  },  \
    /*   2 */ {   2,    0x00010001,   0x0000,    2,   0x0000,  0  },  \
    /*   3 */ {   2,    0x01040104,   0x0000,    2,   0x0000,  0  },  \
    /*   4 */ {   2,    0x01040104,   0x0000,    2,   0x0000,  0  }

/* Note: CN58XX RODT_ENA 0 = disabled, 1 = Weak Read ODT, 2 = Strong Read ODT */
#define CN58XX_DRAM_ODT_2RANK_CONFIGURATION \
    /* DIMMS   RODT_ENA LMC_ODT_CTL  reserved  QS_DIC RODT_CTL DIC */ \
    /* =====   ======== ============ ========= ====== ======== === */ \
    /*   1 */ {   2,    0x00000011,   0x0000,    1,   0x0000,  0  },  \
    /*   2 */ {   2,    0x00110011,   0x0000,    2,   0x0000,  0  },  \
    /*   3 */ {   2,    0x11441144,   0x0000,    3,   0x0000,  0  },  \
    /*   4 */ {   2,    0x11441144,   0x0000,    3,   0x0000,  0  }

/* Note: CN50XX RODT_ENA 0 = disabled, 1 = Weak Read ODT, 2 = Strong Read ODT */
#define CN50XX_DRAM_ODT_1RANK_CONFIGURATION \
    /* DIMMS   RODT_ENA LMC_ODT_CTL  reserved  QS_DIC RODT_CTL DIC */ \
    /* =====   ======== ============ ========= ====== ======== === */ \
    /*   1 */ {   2,    0x00000001,   0x0000,    1,   0x0000,  0  },  \
    /*   2 */ {   2,    0x00010001,   0x0000,    2,   0x0000,  0  },  \
    /*   3 */ {   2,    0x01040104,   0x0000,    2,   0x0000,  0  },  \
    /*   4 */ {   2,    0x01040104,   0x0000,    2,   0x0000,  0  }

/* Note: CN50XX RODT_ENA 0 = disabled, 1 = Weak Read ODT, 2 = Strong Read ODT */
#define CN50XX_DRAM_ODT_2RANK_CONFIGURATION \
    /* DIMMS   RODT_ENA LMC_ODT_CTL  reserved  QS_DIC RODT_CTL DIC */ \
    /* =====   ======== ============ ========= ====== ======== === */ \
    /*   1 */ {   2,    0x00000011,   0x0000,    1,   0x0000,  0  },  \
    /*   2 */ {   2,    0x00110011,   0x0000,    2,   0x0000,  0  },  \
    /*   3 */ {   2,    0x11441144,   0x0000,    3,   0x0000,  0  },  \
    /*   4 */ {   2,    0x11441144,   0x0000,    3,   0x0000,  0  }

#endif  /*  __LIB_OCTEON_SHARED_H__  */
