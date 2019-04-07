/*****************************************************************************
* Copyright 2012 Broadcom Corporation.  All rights reserved.
* 
* This program is the proprietary software of Broadcom Corporation and/or its
* licensors, and may only be used, duplicated, modified or distributed pursuant
* to the terms and conditions of a separate, written license agreement executed
* between you and Broadcom (an "Authorized License").  
* Except as set forth in an Authorized License, Broadcom grants no license 
* (express or implied), right to use, or waiver of any kind with respect to the
* Software, and Broadcom expressly reserves all rights in and to the Software 
* and all intellectual property rights therein.  
* IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE 
* IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF 
* THE SOFTWARE.  
*  
* Except as expressly set forth in the Authorized License,
*   
*  1. This program, including its structure, sequence and organization, 
*     constitutes the valuable trade secrets of Broadcom, and you shall use all
*     reasonable efforts to protect the confidentiality thereof, and to use this 
*     information only in connection with your use of Broadcom integrated circuit 
*     products.
*   
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*     AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*     WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT 
*     TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED 
*     WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A 
*     PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET 
*     ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME 
*     THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
* 
*  3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*     LICENSORS BE LIABLE FOR 
* 
*     (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES 
*         WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR 
*         INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*         THE POSSIBILITY OF SUCH DAMAGES; 
* 
*     OR 
* 
*     (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE 
*         ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL 
*         APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED 
*         REMEDY.
* 
*****************************************************************************/

/******************************************************************************
*  @file    iproc_nand.c
*
*  @brief   One NAND driver for bcm53010
*
*  @note
*
*   These routines provide basic NAND functionality. 
*   Intended for use with u-boot .
*****************************************************************************/

#include <asm/errno.h>

#include <config.h>
#include <common.h>
#include <nand.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>

#include <asm/arch/iproc_regs.h>
#include <asm/iproc/reg_utils.h>
#include <asm/arch/bcmutils.h>
#include <asm/iproc/iproc_nand.h>

#define REG_ACC_CONTROL(cs)    (cs ? IPROC_R_NAND_ACC_CONTROL_CS1_ADDR : IPROC_R_NAND_ACC_CONTROL_CS0_ADDR)
#define REG_CONFIG(cs)         (cs ? IPROC_R_NAND_CONFIG_CS1_ADDR : IPROC_R_NAND_CONFIG_CS0_ADDR)

static const uint32_t page_sizes[]   = {512, 2048, 4096, 8192 };
static const uint32_t block_sizes[]  = {8, 16, 128, 256, 512, 1024, 2048};
static const uint32_t device_sizes[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 
                                            4096, 8192, 16384, 32768, 65536, 131072};
struct iproc_nand_chips {
    const char     *name;
    uint8_t        id[7];
    int            idlen; /* usable */
    unsigned int   chipsize; /* MB */
    unsigned int   writesize; /* B */
    unsigned int   erasesize; /* B */
    unsigned int   oobsize; /* B per page */
    int            chipoptions;
    int            badblockpos;
};
#define IPROC_NAND_CHIP_LIST_COUNT   14

static struct iproc_nand_chips iproc_chip_list[IPROC_NAND_CHIP_LIST_COUNT] = {
    {"Micron MT29F2G08ABAEA",  
		{0x2C, 0xDA, 0x90, 0x95, 0x64, 0x00, 0x00}, 
		5, 0x00100, 2048, 0x020000, 64},                                                          
    {"Micron MT29F8G08ABACA",  
		{0x2C, 0xD3, 0x90, 0xA6, 0x64, 0x00, 0x00}, 
		5, 0x00400, 4096, 0x040000, 224},                                                          
    {"Micron MT29F16G08ABABA", 
		{0x2C, 0x48, 0x00, 0x26, 0x89, 0x00, 0x00}, 
		5, 0x00800, 4096, 0x080000, 224},                                                          
    {"Micron MT29F16G08CBABA", 
		{0x2C, 0x48, 0x04, 0x46, 0x85, 0x00, 0x00}, 
		5, 0x00800, 4096, 0x100000, 224},                                                          
    {"Micron MT29F16G08CBACA", 
		{0x2C, 0x48, 0x04, 0x4A, 0xA5, 0x00, 0x00}, 
		5, 0x00800, 4096, 0x100000, 224},                                                          
    {"Micron MT29F16G08MAA",   
		{0x2C, 0xD5, 0x94, 0x3E, 0x74, 0x00, 0x00}, 
		5, 0x00800, 4096, 0x080000, 218},                                                          
    {"Micron MT29F32G08CBACA", 
		{0x2C, 0x68, 0x04, 0x4A, 0xA9, 0x00, 0x00}, 
		5, 0x01000, 4096, 0x100000, 224},                                                          
    {"Micron MT29F64G08CBAAA", 
		{0x2C, 0x88, 0x04, 0x4B, 0xA9, 0x00, 0x00}, 
		5, 0x02000, 8192, 0x200000, 448},                                                          
    {"Micron MT29F256G08CJAAA",
		{0x2C, 0xA8, 0x05, 0xCB, 0xA9, 0x00, 0x00}, 
		5, 0x08000, 8192, 0x200000, 448},     
    {"Micron MT29F1G08ABADA",
		{0x2C, 0xF1, 0x80, 0x95, 0x02, 0x00, 0x00}, 
		4, 0x00400, 2048, 0x020000, 224},                                                          
    {"Micron MT29F1G08ABBDA",
		{0x2C, 0xA1, 0x80, 0x15, 0x02, 0x00, 0x00}, 
		4, 0x00400, 2048, 0x020000, 224},                                                          
    {"Micron MT29F1G16ABBDA",
		{0x2C, 0xB1, 0x80, 0x55, 0x02, 0x00, 0x00}, 
		4, 0x00400, 2048, 0x020000, 224},     
    {"Micron MT29F128G08AJAAA",
		{0x2C, 0x88, 0x01, 0xA7, 0xA9, 0x00, 0x00}, 
		5, 0x02000, 8192, 0x100000, 448},
	{NULL,}
};

/* ECC bytes required per 512B */
static const uint8_t nand_iproc_ecc_bytes[] = {
    0, 2, 4, 6, 7, 9, 11, 13, 14, 16, 18, 20, 21, 23, 25,
    27 /* or 3 if SPARE_AREA_SIZE == 16 and SECTOR_SIZE_1K == 0*/
};

struct nand_strap_type_t {
    uint8_t     sector_1k;
    uint8_t     ecclevel;
    uint16_t    spare_size;
};

static const struct nand_strap_type_t iproc_nand_strap_types[] = {
    { 0,  0, 16 },
    { 0, 15, 16 },
    { 0,  4, 16 },
    { 0,  8, 16 },
    { 0,  8, 27 },
    { 0, 12, 27 },
    { 1, 12, 27 },
    { 1, 15, 27 },
    { 1, 20, 45 },
};

static const uint32_t iproc_nand_strap_page_sizes[] = { 
    2048, 2048, 4096, 8192 
};

#define CHIPSELECT_MAX_COUNT    2

#define IPROC_INIT_MAGIC        0xbeefdead

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define NAND_TIMEOUT 5 

#define IPROC_NAND_WARNING
#define SIMULATE_MTD_CALLS
#undef IPROC_NAND_DEBUG_CALLS

typedef struct {
    uint32_t initialized;
    uint32_t onfi_status;
    uint32_t onfi_param;
    uint32_t device_id; 
    uint32_t block_size; 
    uint32_t page_size;
    uint32_t device_size; 
    uint32_t device_width; 
    uint32_t spare_area_bytes;
    uint32_t col_addr_bytes;
    uint32_t blk_addr_bytes;
    uint32_t full_addr_bytes;
    uint32_t sector_size_1k;
    struct iproc_nand_chips *chip_ptr;
} INAND;

struct mtd_local {
    uint32_t last_cmd;
    uint32_t last_byte;
    uint32_t last_word;
    uint32_t last_addr;
};

struct mtd_local *mlocal;

static struct nand_ecclayout iproc_nand_oob_layout;

/* global NAND structure */
INAND *inand = NULL;

/* ======================== Static Functions ============================== */

/***************************************************************************
 * Functions to read and write IPROC registers
 *******************************i******************************************/
static uint32_t readl(uint32_t a) 
{
    volatile uint32_t *reg = (uint32_t *)a; 
    return (*reg); 
}

static uint32_t writel(uint32_t a, uint32_t b) 
{
    volatile uint32_t *reg = (uint32_t *)a; 
    *reg = b;
    return *reg; 
}

/***************************************************************************
 * iproc_nand_ecc_status_cmd
 *******************************i******************************************/
static void iproc_nand_ecc_status_cmd (
    uint32_t *error) 
{

    uint32_t reg_data;

    reg_data = readl(IPROC_R_NAND_UNCORR_ERROR_COUNT_ADDR);

    if(reg_data != 0){
        *error = reg_data;
        /* reset un-correctable error counter */
        writel(IPROC_R_NAND_UNCORR_ERROR_COUNT_ADDR, 0);
    }
    return;
}

#if 0
/***************************************************************************
 * iproc_nand_spare_area_read
 *******************************i******************************************/
static void iproc_nand_spare_area_read (
    int      bytes,
    uint32_t *dst) 
{
    int byte_count = bytes & ~0x03;
    int n = 0;

    while ((n*4 < byte_count) && (n < SPARE_AREA_WORDS))  
        *dst = readl(IPROC_R_NAND_SPARE_AREA_READ_OFS_0 + n++);

    return;
}
#endif

/***************************************************************************
 * iproc_check_nand_done
 *******************************i******************************************/
static int iproc_check_nand_done(
    ulong timeout,
	uint32_t bits)
{
    ulong start_time;
    ulong current_time;
    volatile uint32_t reg_data;
    uint32_t mask;

    start_time = get_timer(0);
    
    mask =  IPROC_NAND_FLASH_READY_BIT | IPROC_NAND_CONTROL_READY_BIT | bits;
    reg_data = readl(IPROC_R_NAND_INTFC_STATUS_ADDR);
    while ((reg_data & mask) != mask) {
        __udelay(1);
		/* remove the bits that are done from the mask */
		mask = mask & ~(reg_data & mask);
        current_time = get_timer(0);
		reg_data = readl(IPROC_R_NAND_INTFC_STATUS_ADDR);
        /* Check if the operation has timed out */
        if ((current_time - start_time) >= timeout){
#ifdef IPROC_NAND_WARNING
            printf("NAND command timeout INTFC status %x\n", reg_data);
#endif
            return NAND_TIMEOUT_ERROR;
        }
		/* Check if the operation has failed */
        if (reg_data & IPROC_NAND_INTFC_STATUS_READY_BIT) {
            if (reg_data & IPROC_NAND_INTFC_STATUS_FAIL_BIT) {
#ifdef IPROC_NAND_WARNING
                printf("NAND command INTFC operation failed %x\n", reg_data);
#endif
                return NAND_OPERATION_FAIL;
            }
        }
    }

	/* Wait for the interrupts for an operation complete 
	 * This should not be needed but 
	 * Omitting it results in partial data reads and writes 
	 */
    reg_data = readl(IPROC_R_NAND_RO_CTRL_READY_ADDR);
    while (!(reg_data & 1)) { 
        __udelay(1);
        reg_data = readl(IPROC_R_NAND_RO_CTRL_READY_ADDR);
        current_time = get_timer(0);

        if ((current_time - start_time) >= timeout){
#ifdef IPROC_NAND_WARNING
           printf("NAND command timeout RO_CTRL status %x\n", reg_data);
#endif
           return NAND_TIMEOUT_ERROR;
        }
    }

    /* clear NAND_RO_CRTL_READY bit */
    writel(IPROC_R_NAND_RO_CTRL_READY_ADDR, 1);
    
    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_onfi_parameters_cmd
 *
 * This handles "PARAMETER PAGE" Commands
 *******************************i******************************************/
static int iproc_nand_onfi_parameters_cmd(
    uint8_t state)
{
    if (!state) {
        /* get first two pages of ONFI parameter pages */
        writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, 0x00000000);
        writel(IPROC_R_NAND_CMD_START_ADDR, PARAMETER_READ);

        /* give the flash some time to execute "PARAMETER PAGE" */
        return iproc_check_nand_done(NAND_TIMEOUT, 0);
    }
    else {
        /* get the third page */
        writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, 0x00000200);
        writel(IPROC_R_NAND_CMD_START_ADDR, PARAMETER_CHANGE_COL);

        /* give the flash some time to execute "PARAMETER PAGE" */
        return iproc_check_nand_done(NAND_TIMEOUT, 0);
    }
    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_endian_swap
 *******************************i******************************************/
static void iproc_endian_swap (
    uint32_t *param_page , 
    uint8_t size) 
{
    uint32_t page_data;
    int i;

    for ( i = 0 ; i < size ; i++) {
        page_data = param_page[i];

        param_page[i] = (((page_data & 0x000000FF) << 24) | 
                         ((page_data & 0x0000FF00) << 8)  |
                         ((page_data & 0x00FF0000) >> 8)  | 
                         ((page_data & 0xFF000000) >> 24));
    }

    return;
}

/***************************************************************************
 * iproc_nand_config
 *******************************i******************************************/
static int iproc_nand_config (
    int cs) 
{
    volatile uint32_t reg_data;
	uint32_t ecc_level;
	uint8_t steps;
    uint32_t strap_type;
    uint32_t strap_page;
	uint8_t eccbytes;
    uint32_t full_addr;
	int i;

	/* 
	 * Do not remove the printf below
	 * It is needed because timers are not yet configured 
	 */
    printf(" ");

	if (cs >= CHIPSELECT_MAX_COUNT)
		return NAND_CONFIG_PARAM;

    if (inand && (inand[cs].initialized == IPROC_INIT_MAGIC))
        return NAND_STATUS_OK;

	/* Do a flash reset */
    writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, cs << 16);
    writel(IPROC_R_NAND_CMD_START_ADDR, FLASH_RESET);

    /* Is the NAND initialization a success */
    reg_data = readl(IPROC_R_NAND_INIT_STATUS_ADDR);
    if (!(reg_data & IPROC_NAND_INIT_SUCCESS_BIT))
        return NAND_AUTOCONFIG_FAIL;

    /* read device ID */
	reg_data = readl(IPROC_R_NAND_FLASH_DEVICE_ID_ADDR);
	if (!reg_data)
        return NAND_AUTOCONFIG_FAIL;
	inand[cs].device_id = reg_data;
#ifdef NAND_INFO
	printf("cs %d dev_id word 0 %x\n", cs, reg_data);
#endif
	/* Identify the chip */

    for (i = 0; i < IPROC_NAND_CHIP_LIST_COUNT; i++) {
		uint32_t devid;
        struct iproc_nand_chips *ch;

		ch = &iproc_chip_list[i];
		devid = ch->id[0] << 24 | ch->id[1] << 16 | ch->id[2] << 8 | ch->id[3];
		if (devid == inand[cs].device_id) {
		    inand[cs].chip_ptr = &iproc_chip_list[i];
		    break;
		}
	}
	/* have we identified the chip? */
	if (!inand[cs].chip_ptr) {
	    /* 
		 * Do not remove the printf below
	     * It is needed because timers are not yet configured 
	     */
		printf ("(ONFI), ");
	} else {
		printf ("%s, ", iproc_chip_list[i].name);
	}

    reg_data = readl(IPROC_R_NAND_INIT_STATUS_ADDR);
    if (!(reg_data & IPROC_NAND_INIT_SUCCESS_BIT))
        return NAND_AUTOCONFIG_FAIL;

    /* Check for ONFI comaptibility */
    if (reg_data & IPROC_NAND_ONFI_INIT_BIT) {
    
        uint32_t block_count;
        uint32_t block_mask = 0;
        uint32_t device_mask = 0;
        uint32_t page_mask = 0;
        uint32_t block_addr;
        uint32_t col_addr;
        uint32_t oobsize;
		int n;

        uint8_t param_page[256 * 4];
        struct nand_onfi_params *params = (struct nand_onfi_params *)param_page;

        /* check if ONFI flash */
        reg_data = readl(IPROC_R_NAND_ONFI_STATUS_ADDR);
        if (!(reg_data & IPROC_NAND_ONFI_STRING_DETECTED)) {
			/* should not be here */
            return NAND_AUTOCONFIG_FAIL;
		}
	    inand[cs].onfi_status = TRUE;
        
		/* read the ONFI device parameters */
        n = iproc_nand_onfi_parameter_pages(cs, (uint32_t *)param_page);
        if (n != NAND_STATUS_OK) {
            printf ("ONFI: failed to read parameter pages!");
            return NAND_AUTOCONFIG_FAIL;
        }
        
        /* Get device model name from ONFI */
        if (!inand[cs].chip_ptr) {
            char name[21];
            memcpy(name, params->model, 20);
            name[20] = 0;
            printf("%s, ", name);
        }
        
        /* Calculate device config based on the ONFI parameters */
        inand[cs].page_size = params->byte_per_page;
        inand[cs].block_size = params->pages_per_block * inand[cs].page_size;
        block_count = params->blocks_per_lun * params->lun_count;
        inand[cs].device_size = 
            block_count * (inand[cs].block_size / 1024) / 1024;
        inand[cs].device_width = (params->features & 1)? 16 : 8;
        oobsize = params->spare_bytes_per_page;
        inand[cs].spare_area_bytes = 
            oobsize / (inand[cs].page_size / IPROC_FLASH_CACHE_SIZE);
        block_addr = params->addr_cycles & 0x0f;
        col_addr = (params->addr_cycles >> 4) & 0x0f;
            
#ifdef NAND_INFO
        printf("\nONFI info: device %dMB, page %dB, oobsize %dB, block %dKB, \n"
                 "           col-addr %d, row-addr %d, %s\n", 
               inand[cs].device_size,
               inand[cs].page_size, 
               oobsize, 
               inand[cs].block_size / 1024, 
               col_addr, block_addr,
               inand[cs].device_width == 8? "8-bit" : "16-bit");
#endif

		/* Adjust the page size, device size and block size accoring to ONFI */
        for (n = 0; n < ARRAY_SIZE(block_sizes); n++) {
			if (inand[cs].block_size == 1024 * block_sizes[n]) { 
                block_mask = n << IPROC_NAND_BLOCK_SIZE_SHIFT;
			    break;
			}
		}

        for (n = 0; n < ARRAY_SIZE(device_sizes); n++) {
			if (inand[cs].device_size == device_sizes[n]) {
                device_mask = n << IPROC_NAND_DEVICE_SIZE_SHIFT;
			    break;
			}
		}

        for (n = 0; n < ARRAY_SIZE(page_sizes); n++) {
			if (inand[cs].page_size == page_sizes[n]) { 
                page_mask = n << IPROC_NAND_PAGE_SIZE_SHIFT;
			    break;
			}
		}
        
		reg_data = 0;
		reg_data |= page_mask | device_mask | block_mask ;
		full_addr =  block_addr + col_addr;
		reg_data |= full_addr << IPROC_NAND_FULL_ADDR_BYTE_SHIFT;
		reg_data |= block_addr << IPROC_NAND_BLOCK_ADDR_BYTE_SHIFT;
		reg_data |= col_addr << IPROC_NAND_COLUMN_ADDR_BYTE_SHIFT;

	    if (inand[cs].device_width == 16) 
            reg_data |= IPROC_NAND_DEVICE_WIDTH_MASK;

        writel(REG_CONFIG(cs), reg_data);

    } else { 
		/* Fail if not ONFI */
        return NAND_AUTOCONFIG_FAIL;
	}
    
	/* Read NAND strap settings */
	reg_data = readl(IPROC_R_STRAPS_CONTROL_ADDR);
	strap_type = (reg_data & IPROC_NAND_STRAP_TYPE_MASK) >> IPROC_NAND_STRAP_TYPE_SHIFT;
    strap_page = (reg_data & IPROC_NAND_STRAP_PAGE_MASK) >> IPROC_NAND_STRAP_PAGE_SHIFT;
    
    /* Validate strap settings */
#ifdef NAND_INFO
    printf("Found strap type 0x%x strap page 0x%x\n", strap_type, strap_page);
#endif
    if (strap_type == 0 || 
        strap_type >= sizeof(iproc_nand_strap_types) / sizeof(struct nand_strap_type_t) ||
        iproc_nand_strap_types[strap_type].spare_size >= inand[cs].spare_area_bytes ||
        iproc_nand_strap_page_sizes[strap_page] != inand[cs].page_size) {
        
        /* Strap options are invalid */
        printf("\n*ERROR* Invalid strap options for this NAND: page=%d type=%d\n",
            strap_page, strap_type);
            
        /* Trying to fit with available strap options */
        if (inand[cs].spare_area_bytes >= 27) {
            if (inand[cs].page_size >= 2048)
                strap_type = 6;
            else 
                strap_type = 5;
        } else if (inand[cs].spare_area_bytes > 16) {
            strap_type = 3;
        } else {
            strap_type = 2;
        }
        
        printf("Overriding invalid strap options: strap_type=%d\n", strap_type);
    }

	/* Calculate the REG_ACC_CONTROL */
	reg_data = IPROC_NAND_PAGE_HIT_EN;
	reg_data |= IPROC_NAND_ECC_READ_EN;
	reg_data |= IPROC_NAND_ECC_WRITE_EN;

    ecc_level = iproc_nand_strap_types[strap_type].ecclevel;
 	inand[cs].sector_size_1k = iproc_nand_strap_types[strap_type].sector_1k;
	inand[cs].spare_area_bytes = iproc_nand_strap_types[strap_type].spare_size;
    steps = inand[cs].page_size/IPROC_FLASH_CACHE_SIZE >> inand[cs].sector_size_1k;
    eccbytes = nand_iproc_ecc_bytes[ecc_level] << inand[cs].sector_size_1k;
    iproc_nand_oob_layout.eccbytes = eccbytes * steps;
	if (inand[cs].sector_size_1k)
        reg_data |= IPROC_NAND_1K_SECTOR;

	reg_data |= ecc_level << IPROC_NAND_ECC_LEVEL_SHIFT; 
	reg_data |= inand[cs].spare_area_bytes;
    writel(REG_ACC_CONTROL(cs), reg_data);
#ifdef NAND_INFO
	printf("REG_ACC_CONTROL %x spare_area_bytes %x ecc_level %x eccbytes %x steps %x\n", 
			reg_data, inand[cs].spare_area_bytes, ecc_level, eccbytes, steps);
#endif	

    reg_data = readl(REG_CONFIG(cs));
#ifdef NAND_INFO
	printf("REG_CONFIG %x\n", reg_data);
#endif	

    inand[cs].initialized = IPROC_INIT_MAGIC;

	printf("%u KiB blocks, %u KiB pages, %uB OOB, %u-bit\n", 
		inand[cs].block_size/1024, 
		inand[cs].page_size/1024, inand[cs].spare_area_bytes, 
	    inand[cs].device_width);
        
    /* clear NAND_RO_CRTL_READY bit */
    writel(IPROC_R_NAND_RO_CTRL_READY_ADDR, 1);

    /* Create oobfree for storing user OOB data */
    if (inand[cs].spare_area_bytes > eccbytes) {
        struct nand_oobfree *free = iproc_nand_oob_layout.oobfree;
        unsigned spare_size;
        uint8_t i, count;

        /* Special case: using Hamming code when ecc_level == 15 */
        if (ecc_level == 15) {
            if (inand[cs].spare_area_bytes == 16 && inand[cs].sector_size_1k) {
                eccbytes = 3;
            }
        }
        
        spare_size = inand[cs].spare_area_bytes;

        if (steps > MTD_MAX_OOBFREE_ENTRIES) {
            steps = MTD_MAX_OOBFREE_ENTRIES;
        }
        for(i = 0, count = 0; i < steps && count < MTD_MAX_OOBFREE_ENTRIES; i++) {
            
            if (eccbytes == 3) {
                /* Hamming code: ECC bytes are 6~8; First part here. */
                free->offset = i * spare_size;
                free->length = 6;
                
            } else {
            
                /* BCH: ECC bytes at the bottom */
                free->offset = i * spare_size;
                free->length = spare_size - eccbytes;
            }
            
            /* Reserve the first two bytes of the page */
            if (i == 0) {
                if (free->length <= 2) {
                    /* Don't claim this entry if less than 2 bytes */
                    continue;
                }
                free->offset += 2;
                free->length -= 2;
            }
            
            if (eccbytes == 3) {
                /* Hamming code: the 2nd free part */
                free++;
                count++;
                if (count < MTD_MAX_OOBFREE_ENTRIES) {
                    free->offset = i * spare_size + 9;
                    free->length = 7;
                } else {
                    /* The structure limits us. */
                    break;
                }
            }
            
            free++;
            count++;
        }
        if (count < MTD_MAX_OOBFREE_ENTRIES) {
            /* Terminator */
            free->length = 0;
        }
            
        /* Print out oob space information */
        free = iproc_nand_oob_layout.oobfree;
        if (free->length) {
            spare_size = 0;
            while(free->length) {
                spare_size += free->length;
                free++;
            }
#ifdef NAND_INFO
        printf("NAND:    user oob per page: %u bytes (%u steps)\n", spare_size, steps);
#endif		
        }

    }

	printf("NAND:   chipsize ");  /* will be filled in by u-boot */

    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_one_page_read
 *******************************i******************************************/
static int _iproc_nand_one_page_read(
    int cs,
    uint64_t page_addr,
    uint8_t *dst_addr,
    uint32_t data_len)
{
    int i , j;
    uint32_t *dst;
    uint32_t ecc_errors;
    uint32_t num_of_buffers;
    uint32_t page_size; 
    uint32_t reg_data; 
    NAND_STATUS status;

    if (!inand || (inand[cs].initialized != IPROC_INIT_MAGIC))
        return NAND_UNINITIALIZED;

    dst = (uint32_t*) dst_addr;
    page_size = inand[cs].page_size;

    num_of_buffers = page_size / IPROC_FLASH_CACHE_SIZE;

    /* see if data len (required) is less than a complete page */
    if ((data_len < page_size) && (page_size > IPROC_FLASH_CACHE_SIZE)) {
        for (i = 1 ; i < num_of_buffers ; i++) {
            if (data_len <= (i * IPROC_FLASH_CACHE_SIZE)) {
                num_of_buffers = i;
                break;
            }
        }
    }

	if (page_addr & IPROC_FLASH_CACHE_SIZE) {
#ifdef IPROC_NAND_WARNING
		printf("page_read %llx not %x aligned\n", page_addr, IPROC_FLASH_CACHE_SIZE);
#endif	
		return NAND_PAGE_ALIGNMENT;
	}

    /* get data from device */
    for (i = 0 ; i < num_of_buffers; i++) {
        /*write the address for read */
        writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, (cs << 16) | ((page_addr >> 32) & 0xFFFF));
        writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, (uint32_t)page_addr);
        /* write the opcode in cmd start reg */
        writel(IPROC_R_NAND_CMD_START_ADDR, PAGE_READ);

        /* give the flash some time to execute page read */
        status =  iproc_check_nand_done(NAND_TIMEOUT, IPROC_NAND_CACHE_VALID_BIT);
        if (status != NAND_STATUS_OK)
            return status;

		/* Change Little Endian mode before reading data */
		reg_data = readl (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT);
		writel (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT, reg_data | IPROC_NAND_IDM_IO_CONTROL_APB_LE_MODE_BIT);

        /* flash is ready: read word by word from the cache */
        for ( j = 0 ; j < (IPROC_FLASH_CACHE_SIZE / 4) ; j++) {
            *(dst++) = readl((IPROC_R_NAND_FLASH_CACHE_0_ADDR + j*4));
        }
		/* Revert Little Endian bit */
		reg_data = readl (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT);
		writel (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT, reg_data & ~IPROC_NAND_IDM_IO_CONTROL_APB_LE_MODE_BIT);

        page_addr += IPROC_FLASH_CACHE_SIZE;
    }

    ecc_errors = 0;
    iproc_nand_ecc_status_cmd (&ecc_errors);
	if (ecc_errors) {
        reg_data = readl(IPROC_R_NAND_UNCORR_ERROR_ADDRESS_ADDR);
#ifdef IPROC_NAND_WARNING
		printf("%d ecc_errors after reading %llx:%x\n", ecc_errors, page_addr - page_size, reg_data);
#endif	
	}

	if (ecc_errors)
        return NAND_UNCOR_ECC_ERROR;

    return NAND_STATUS_OK;;
}

/***************************************************************************
 * iproc_nand_page_read
 *******************************i******************************************/
static int _iproc_nand_page_read(
    int cs,
    uint64_t page_addr,
    uint8_t *dst_addr,
    uint32_t data_len)
{
	int rv = 0;
	uint32_t read_len = 0;
	uint32_t data_read = 0;

#if 0	
    if (data_len > inand[cs].page_size)
        iproc_nand_ecc_set_config(cs, IPROC_NAND_PREFETCH_EN);
#endif	
	while (data_read < data_len) {
		/* read a full page by default */
		read_len = inand[cs].page_size;
		if ((data_len - data_read) < read_len) {
			read_len = data_len - data_read;
		}
        rv = _iproc_nand_one_page_read(cs, page_addr, dst_addr, read_len);
	    if (rv) {
			return rv;
		}
		data_read += inand[cs].page_size;
		dst_addr += inand[cs].page_size;
		page_addr += inand[cs].page_size;
	}
#if 0	
    if (data_len > inand[cs].page_size)
        iproc_nand_ecc_set_config(cs, 0); 
#endif	
	return rv;
}

/***************************************************************************
 * iproc_nand_page_program
 *******************************i******************************************/
static int _iproc_nand_page_program(
    int cs,
    uint64_t page_addr,
    uint8_t* src_addr,
    uint32_t data_len)
{
    NAND_STATUS status;
    int i , j;
    uint32_t num_of_buffers;
    uint32_t *src = (uint32_t*)src_addr;
	uint32_t reg_data;

    if (!inand || (inand[cs].initialized != IPROC_INIT_MAGIC))
        return NAND_UNINITIALIZED;
        
    /*
     * Don't write this page if it contains only FFs (to avoid generating ECC)
     * since we consider it as an empty page (data could be written later).
     */ 
    if (src_addr && data_len == inand[cs].page_size) {
        uint8_t *p = src_addr;
        for(i=0; i<data_len; i++, p++) {
            if (*p != 0xFF) {
                break;
            }
        }
        if (i == data_len) {
            return NAND_STATUS_OK;
        }
    }

    num_of_buffers = data_len / IPROC_FLASH_CACHE_SIZE;

	if (page_addr & IPROC_FLASH_CACHE_SIZE) {
#ifdef IPROC_NAND_WARNING
		printf("page_program %llx not %x aligned\n", page_addr, IPROC_FLASH_CACHE_SIZE);
#endif	
		return NAND_PAGE_ALIGNMENT;
	}

    for (i = 0 ; i < num_of_buffers; i++) {
        /* set address before writing data to cache */
        writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, (cs << 16) | ((page_addr >> 32) & 0xFFFF));
        writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, (uint32_t)page_addr);

		/* Change Little Endian mode before writing data */
		reg_data = readl (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT);
		writel (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT, reg_data | IPROC_NAND_IDM_IO_CONTROL_APB_LE_MODE_BIT);

        /* write the flash cache word by word */
        for (j = 0 ; j <  (IPROC_FLASH_CACHE_SIZE / 4) ; j++) {
            writel((IPROC_R_NAND_FLASH_CACHE_0_ADDR + j*4), *(src + j));
        }
		/* Revert Little Endian bit */
		reg_data = readl (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT);
		writel (IPROC_R_IDM_IDM_IO_CONTROL_DIRECT, reg_data & ~IPROC_NAND_IDM_IO_CONTROL_APB_LE_MODE_BIT);
        
        /* Clear WP (Write Protect) bit */
        reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
        writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data & ~IPROC_NAND_WRITE_PROCTECT_BIT);

        /* write it the flash write command and wait for it to execute */
        writel(IPROC_R_NAND_CMD_START_ADDR, PROGRAM_PAGE);

        /* give the flash some time to execute "Page Program" */
        status = iproc_check_nand_done(NAND_TIMEOUT, 0);

        /* Set back WP (Write Protect) bit */
        reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
        writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data | IPROC_NAND_WRITE_PROCTECT_BIT);
        
        if (status) {
#ifdef IPROC_NAND_WARNING
	        printf("%s returned %d\n", __func__, status);
#endif
            return status;
        }

        page_addr += IPROC_FLASH_CACHE_SIZE;
        src += (IPROC_FLASH_CACHE_SIZE / 4);
    }

    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_block_erase
 *******************************i******************************************/
static int _iproc_nand_block_erase(
    int cs,
    uint64_t block_address) 
{
    int status;
	uint32_t reg_data;

    if (!inand || (inand[cs].initialized != IPROC_INIT_MAGIC))
        return NAND_UNINITIALIZED;

    block_address &= ~(inand[cs].block_size - 1);

    writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, (cs << 16) | ((block_address >> 32) & 0xFFFF));
    writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, (uint32_t)block_address);
    
    /* Clear WP (Write Protect) bit */
    reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
    writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data & ~IPROC_NAND_WRITE_PROCTECT_BIT);

    writel(IPROC_R_NAND_CMD_START_ADDR, BLOCK_ERASE);

    /* Give the flash some time to execute */
    status = iproc_check_nand_done(20*NAND_TIMEOUT, 0);
    
    /* Set back WP (Write Protect) bit */
    reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
    writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data | IPROC_NAND_WRITE_PROCTECT_BIT);
	
#ifdef IPROC_NAND_WARNING
	if (status) {
		printf("%s returned %d\n", __func__, status);
	}
#endif
    return status;
}

static int _iproc_nand_dev_ready(void)
{
    ulong start_time;
    ulong current_time;
    volatile uint32_t reg_data;

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called\n", __func__);
#endif
    start_time = get_timer(0);
    
    reg_data = readl(IPROC_R_NAND_INTFC_STATUS_ADDR);
    while (!(reg_data & (IPROC_NAND_FLASH_READY_BIT))) {
        __udelay(1);
        reg_data = readl(IPROC_R_NAND_INTFC_STATUS_ADDR);
        current_time = get_timer(0);

        if ((current_time - start_time) >= NAND_TIMEOUT){
#ifdef IPROC_NAND_WARNING
           printf("NAND command timeout INTFC %x\n", reg_data);
#endif
           return NAND_TIMEOUT_ERROR;
        }
    }
	return 0;
}

/* ======================== Interface to MTD  =============================== */

static uint8_t iproc_nand_read_byte(struct mtd_info *mtd);

static void iproc_nand_read_buf(struct mtd_info *mtd, u_char * buf, int len)
{
    int i;

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called add %x buf %x len %d\n", __func__, mlocal->last_addr, (unsigned)buf, len);
#endif

    for(i=0; i<len; i++, buf++)
        *buf = iproc_nand_read_byte(mtd);
}

static int 
iproc_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called\n", __func__); 
#endif
	return 0;
}

static int iproc_nand_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
    uint32_t reg_data;
    uint32_t mask;
    uint64_t block = (uint64_t)ofs;
    int cs = (int)((struct nand_chip *)mtd->priv)->priv;
	int status;
    
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called block %llx \n", __func__, block); 
#endif
    writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, (cs << 16) | ((block >> 32) & 0xFFFF));
    writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, (uint32_t)block);
	writel(IPROC_R_NAND_CMD_START_ADDR, PAGE_READ);
	mask = IPROC_NAND_8bit_BAD_BLOCK_MASK;
    if (inand[0].device_width == 16) {
        mask = IPROC_NAND_16bit_BAD_BLOCK_MASK;
	}
    status = iproc_check_nand_done(NAND_TIMEOUT, IPROC_NAND_CACHE_VALID_BIT);
    if (status != NAND_STATUS_OK) { 
#ifdef IPROC_NAND_WARNING
        printf("iproc_check_nand_done error %d\n", status);
#endif
	}

    reg_data = readl(IPROC_R_NAND_SPARE_AREA_READ_OFS_0);
	if ((reg_data & mask) != mask) {
#ifdef NAND_INFO
		printf("NAND%d bad block 0x%llx\n", cs, block);
#endif
		return 1;
	}
	return 0;
}

static int iproc_nand_mark_bad(struct mtd_info *mtd, loff_t ofs)
{
    uint32_t reg_data;
    uint32_t block = (unsigned long)ofs;
    int status;

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called block %x \n", __func__, block); 
#endif

    writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, block);

    /* Clear WP (Write Protect) bit */
    reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
    writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data & ~IPROC_NAND_WRITE_PROCTECT_BIT);

    writel(IPROC_R_NAND_SPARE_AREA_WRITE_OFS_0, 0);

    writel(IPROC_R_NAND_CMD_START_ADDR, PROGRAM_SPARE_AREA);

    /* Give the flash some time to execute */
    status = iproc_check_nand_done(20*NAND_TIMEOUT, 0);
    if (status != NAND_STATUS_OK) { 
#ifdef IPROC_NAND_WARNING
        printf("iproc_check_nand_done error %d\n", status);
#endif
	}
    
    /* Set back WP (Write Protect) bit */
    reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
    writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data | IPROC_NAND_WRITE_PROCTECT_BIT);

	return 0;
}

/***************************************************************************
* Reads every block of the chip and marks those that are bad
* Use with caution, takes long time to complete
***************************************************************************/
int iproc_nand_scan_bad_blocks(int cs)
{
    uint64_t block;
	uint64_t total_size;
    uint32_t page_addr;
    uint32_t start;
	uint8_t *buf;
	int rv;

	buf = (uint8_t *)malloc(inand[cs].page_size);
	if (!buf)
		return -1;

	total_size = inand[cs].device_size * 1024 * 1024; 
    for (block = 0; block < total_size; block += inand[cs].block_size) {
		writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, (cs << 16) | ((block >> 32) & 0xFFFF));
		start = (uint32_t)block;
		for (page_addr = start; page_addr < start + inand[cs].block_size; page_addr += inand[cs].page_size) { 
			rv = _iproc_nand_one_page_read(cs, page_addr, buf, inand[cs].page_size);
			if (rv) {
				printf("NAND%d bad block 0x%llx\n", cs, block);
				iproc_nand_mark_bad(NULL, (loff_t) block);
			}
		}
	}
    free (buf);
	return 0;
}

static int iproc_nand_read_page(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int page)
{
	int rv;
	uint64_t page_addr = (uint64_t)page * inand[0].page_size;
    
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called page %x page_addr %llx\n", __func__, (unsigned)page, page_addr);
#endif
	rv = iproc_nand_page_read(0, page_addr, (uint8_t*)buf, inand[0].page_size);
	if (rv) {
#ifdef IPROC_NAND_WARNING
		printf("%s returned %d\n", __func__, rv);
#endif
		return -EINVAL;
	}

	return 0;
}

static void iproc_nand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf)
{
	uint64_t page_addr = (uint64_t)mlocal->last_word * inand[0].page_size;
	int rv;
    
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called page_addr %llx\n", __func__, page_addr);
#endif
	rv = iproc_nand_page_program(0, page_addr, (uint8_t*)buf, inand[0].page_size);
	if (rv) {
#ifdef IPROC_NAND_WARNING
		printf("%s returned %d\n", __func__, rv);
#endif
	}
}

static int iproc_nand_write_page(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int page, int cached, int raw)
{
	uint64_t page_addr = (uint64_t)page * inand[0].page_size;
	int rv;
    
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called page %x page_addr %llx cached %x raw %x\n", __func__, (unsigned)page, page_addr, cached, raw);
#endif
	rv = iproc_nand_page_program(0, page_addr, (uint8_t*)buf, inand[0].page_size);
	if (rv) {
		printf("%s returned %d\n", __func__, rv);
		return 1;
	}
	return 0;
}

static void iproc_nand_command(struct mtd_info *mtd, unsigned cmd, int column, int page_addr)
{
    int iproc_cmd = 0;
    uint32_t addr = column;
	int rv;
    int cs = (int)((struct nand_chip *)mtd->priv)->priv;

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called cmd %x, addr %x page %x\n", __func__, cmd, addr, page_addr);
#endif
    switch (cmd) {
    case NAND_CMD_RESET:
        iproc_cmd = FLASH_RESET;
        break;
    case NAND_CMD_STATUS:
        iproc_cmd = STATUS_READ;
        break;
    case NAND_CMD_READID:
        iproc_cmd = DEVICE_ID_READ;
        break;
    case NAND_CMD_PARAM:
        iproc_cmd = PARAMETER_READ;
        break;
    case NAND_CMD_READOOB:
        iproc_cmd = SPARE_AREA_READ;
        break;
    case NAND_CMD_ERASE1:
        iproc_cmd = BLOCK_ERASE;
        break;
	case NAND_CMD_SEQIN:
		/* just capture the page address */
        mlocal->last_word = page_addr;
        break;
	}

    mlocal->last_cmd = iproc_cmd;
    mlocal->last_byte = 0;
    mlocal->last_addr = addr;

	/* Issue the command to the chip */
	/* should call the following function but only after the timer has been initialized
	 * timer initialization is performed later than NAND initialization 
     iproc_nand_dev_ready();
	 */
     
	switch(iproc_cmd) {
	case DEVICE_ID_READ:
        writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, cs << 16);
        writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, column);
		writel(IPROC_R_NAND_CMD_START_ADDR, iproc_cmd);
        iproc_check_nand_done(NAND_TIMEOUT, 0);
	    mlocal->last_word = readl(IPROC_R_NAND_FLASH_DEVICE_ID_ADDR);
	    break; 
	case PARAMETER_READ:
        writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, cs << 16);
        writel(IPROC_R_NAND_CMD_ADDRESS_ADDR, column);
		writel(IPROC_R_NAND_CMD_START_ADDR, iproc_cmd);
        iproc_check_nand_done(NAND_TIMEOUT, 0);
	    break; 
    case STATUS_READ:
		/* just read the current status */
        writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, cs << 16);
		writel(IPROC_R_NAND_CMD_START_ADDR, iproc_cmd);
        iproc_check_nand_done(NAND_TIMEOUT, 0);
	    mlocal->last_word = readl(IPROC_R_NAND_INTFC_STATUS_ADDR);
		mlocal->last_word |= 0x80;
	    break; 
    case BLOCK_ERASE:
		/* perform block erase */
        rv = iproc_nand_block_erase(0, (uint64_t)page_addr * inand[0].page_size);
		if (rv) {
			printf("erase block %llx failed %x\n", (uint64_t)page_addr*inand[0].page_size, rv);
		}
	    break; 
	case FLASH_RESET:
		/* we just made a reset during initialization */
		/* don't want to do another because timers may not yet be working */
		break;
	}
}	

static uint8_t iproc_nand_read_byte(struct mtd_info *mtd)
{
    uint32_t val = 0;
	uint32_t offset;

    switch (mlocal->last_cmd) {
    case DEVICE_ID_READ:
        switch (mlocal->last_byte) {
		case 0:
			mlocal->last_word = readl(IPROC_R_NAND_FLASH_DEVICE_ID_ADDR);
            val = (mlocal->last_word & 0xFF000000) >> 24;
			break;
		case 1:
            val = (mlocal->last_word & 0xFF0000) >> 16;
			break;
		case 2:
            val = (mlocal->last_word & 0xFF00) >> 8;
			break;
		case 3:
            val = mlocal->last_word & 0xFF;
			break;
		case 4:
			mlocal->last_word = readl(IPROC_R_NAND_FLASH_DEVICE_ID_EXT_ADDR);
            val = (mlocal->last_word & 0xFF000000) >> 24;
			break;
		case 5:
            val = (mlocal->last_word & 0xFF0000) >> 16;
			break;
		case 6:
            val = (mlocal->last_word & 0xFF00) >> 8;
			break;
		case 7:
            val = mlocal->last_word & 0xFF;
			break;
		}
        mlocal->last_byte++;
        break;

    case SPARE_AREA_READ:
        offset  = mlocal->last_byte;
        /* read the spare area at this offset */
        val = readl(IPROC_R_NAND_SPARE_AREA_READ_OFS_0 + (offset & ~0x03));
        val >>= (24 - ((offset & 0x03) << 3));
        break;

    case STATUS_READ:
        val = mlocal->last_word & IPROC_NAND_INTFC_STATUS_MASK;
        break;

    case PARAMETER_READ:
        if (mlocal->last_byte < IPROC_FLASH_CACHE_SIZE) {
            offset = mlocal->last_byte;
            val = readl(IPROC_R_NAND_FLASH_CACHE_0_ADDR + (offset & ~0x03));
            val >>= (24 - ((offset & 0x03) << 3));
        } else {
            val = 0xFF;
        }
        mlocal->last_byte++;
        break;
    }

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: value 0x%02x\n", __func__, val & 0xFF);
#endif
    return (uint8_t)val;
}

static int iproc_nand_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called buf %x len %x\n", __func__, (unsigned)buf, len);
#endif
	return 0;
}

static void iproc_nand_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called cmd %x ctrl %x\n", __func__, cmd, ctrl);
#endif
	/* deliberately left as NOOP */
}

static int nand_dev_ready(struct mtd_info *mtd)
{
	return _iproc_nand_dev_ready();
}

/* ======================== Public Functions =============================== */
int board_nand_init(struct nand_chip *nand)
{
    int rv;
    struct nand_chip *this;
    
    static int cs = 0;

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called\n", __func__);
#endif
	/* Initialize the contorller parameters */
    rv = iproc_nand_init(cs);
    if (rv) {
        printf(KERN_ERR "iProc NAND chip could not be initialized\n");
        return -EIO;
    }

    /* Get pointer to private data */
    this = (struct nand_chip *) nand;

    /* Set address of NAND IO lines , do not let mtd access */
    this->IO_ADDR_W = (void __iomem *) 0xdeadbeef; 
    this->IO_ADDR_R = (void __iomem *) 0xdeadbeef; 

    /* Assign the device ready function, if available */
    this->chip_delay = 100;

    this->dev_ready = nand_dev_ready;
    this->options = NAND_NO_SUBPAGE_WRITE | NAND_NO_AUTOINCR | NAND_SKIP_BBTSCAN;

    this->cmd_ctrl = iproc_nand_cmd_ctrl;
    this->cmdfunc = iproc_nand_command;

    this->read_buf = iproc_nand_read_buf;
    this->read_byte = iproc_nand_read_byte;

    /* for writing buffers to flash */
    this->write_page = iproc_nand_write_page;
	this->block_bad = iproc_nand_block_bad;
	this->block_markbad = iproc_nand_mark_bad;
    this->verify_buf = iproc_nand_verify_buf; 
    this->waitfunc = iproc_nand_waitfunc;

    this->ecc.mode = NAND_ECC_HW;
    this->ecc.size = 512;
    this->ecc.layout = &iproc_nand_oob_layout;
    this->ecc.bytes = iproc_nand_oob_layout.eccbytes / (inand[0].page_size/IPROC_FLASH_CACHE_SIZE);
    this->ecc.write_page = iproc_nand_write_page_hwecc;
    this->ecc.read_page = iproc_nand_read_page;
    
    /* Store chip select in priv of nand_chip */
    this->priv = (void *)cs;
    cs++;

    return 0;
}

/***************************************************************************
 * iproc_nand_set_cs_select
 **************************************************************************/
int iproc_nand_set_cs_select (int cs_select)
{
    if (cs_select < CHIPSELECT_MAX_COUNT) {
        /* set up cs chosen, either cs0 or cs1 */
        writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, cs_select);
        return NAND_STATUS_OK;
    }

    return NAND_CONFIG_PARAM;
}

/***************************************************************************
 * iproc_nand_init
 *
 * This should be the first function to be called to initialize NAND Controller 
 * and the connected flash
 *******************************i******************************************/
int iproc_nand_init(
    uint32_t cs)
{
    volatile uint32_t reg_data;
	int rv;

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called\n", __func__);
#endif
	if (cs >= CHIPSELECT_MAX_COUNT)
		return NAND_CONFIG_PARAM;

    /* Global initialization */
    if (inand == NULL) {
        
        /* read the strap information first to see if it supports NAND */
        reg_data = readl(ROM_S0_IDM_IO_STATUS);
        if (((reg_data & IPROC_IDM_SKU_MASK) >> IPROC_IDM_SKU_SHIFT) == IDM_SKU_BCM953010) {
            /* this is bcm953010 which does not have a NAND flash */
            printf("NAND not supported on this iProc board\n");
            return NAND_NOT_SUPPORTED;
        }

        inand = (INAND *)malloc (sizeof(INAND) * CHIPSELECT_MAX_COUNT);
        if (!inand) {
            printf("Error allocating memory for inand\n");
            return NAND_ALLOC;
        }
        memset(inand, 0, sizeof(INAND)* CHIPSELECT_MAX_COUNT);

        mlocal = (struct mtd_local *)malloc (sizeof(struct mtd_local));
        if (!mlocal) {
            printf("Error allocating memory for mlocal\n");
            return NAND_ALLOC;
        }
        memset(mlocal, 0, sizeof(struct mtd_local));
        
        /* Take the NAND block out of reset */
        writel(IPROC_R_IDM_IDM_RESET_CONTROL_ADDR, 0x1);
        writel(IPROC_R_IDM_IDM_RESET_CONTROL_ADDR, 0x0);
        reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
         
        /* Set the autoconfig bit */
        reg_data &= ~(IPROC_NAND_SELECT_AUTO_DEVICE_ID_BIT | IPROC_NAND_SELECT_AUTO_DEVID_CONFIG_BIT);
        writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data);
        reg_data |= IPROC_NAND_SELECT_AUTO_DEVICE_ID_BIT | IPROC_NAND_SELECT_AUTO_DEVID_CONFIG_BIT;
        writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data);
    }

    if (inand && (inand[cs].initialized == IPROC_INIT_MAGIC))
        return NAND_STATUS_OK;

    rv = iproc_nand_config(cs);

    if (cs == CONFIG_SYS_MAX_NAND_DEVICE - 1) {
        /* Clear the autoconfig bit to indiciate to indicate the NAND has been configured manually 
         * This is the sign to the Linux NAND driver that the bootloader performed NAND configuration
         */
        reg_data = readl(IPROC_R_NAND_CS_NAND_SELECT_ADDR);
        /* clear the autoconfig bit */
        reg_data &= ~IPROC_NAND_SELECT_AUTO_DEVID_CONFIG_BIT;
        writel(IPROC_R_NAND_CS_NAND_SELECT_ADDR, reg_data);
    }

	return rv;
}

/***************************************************************************
 * iproc_nand_onfi_parameters_pages
 *
 * To be called after initializing ONFI device
 *******************************i******************************************/
int iproc_nand_onfi_parameter_pages(
    int cs,
    uint32_t *param_page)
{
    uint8_t i;
    uint8_t state = 0;
    NAND_STATUS status = NAND_STATUS_OK;

    /* Chip select */
    writel(IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR, cs << 16);
    
    /* get first two pages */
    status = iproc_nand_onfi_parameters_cmd(state++);
    if (status != NAND_STATUS_OK) {
        return status;
    }

    for (i = 0; i <  128; i++) {
        param_page[i] = readl((IPROC_R_NAND_FLASH_CACHE_0_ADDR + i*4));
    }

    /* get third one */
    status = iproc_nand_onfi_parameters_cmd(state);
    if (status != NAND_STATUS_OK) {
        return status;
    }

    for (i = 0; i <  64; i++) {
        param_page[128+i] = readl((IPROC_R_NAND_FLASH_CACHE_0_ADDR + i*4));
    }

    /* convert data to little endian since hardware stores it in Cache as big endian */
    iproc_endian_swap (param_page , (128+64));

    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_ecc_set_config
 *******************************i******************************************/
int iproc_nand_ecc_set_config(
    uint32_t cs, 
    uint32_t ecc_config)
{
    uint32_t reg_data;
    uint32_t acc_ctl_reg;

    if (inand[cs].initialized != IPROC_INIT_MAGIC)
        return NAND_UNINITIALIZED;

    acc_ctl_reg = REG_ACC_CONTROL(cs);
    reg_data = readl(acc_ctl_reg);
    reg_data &= 0xFFF0001F;
    reg_data |= ecc_config;

    writel(acc_ctl_reg, reg_data);

    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_onfi_ecc_update
 *******************************i******************************************/
uint32_t iproc_nand_onfi_ecc_update (
    int cs,
    uint32_t ecc_corr_bits)
{
    uint32_t ecc_config;
    uint32_t ecc_level;
    uint32_t ecc_bytes_req;
    uint32_t page_size; 

    if (inand[cs].initialized != IPROC_INIT_MAGIC)
        return NAND_UNINITIALIZED;

    page_size = inand[cs].page_size; 

    /* check spare area limits. */
    ecc_config = inand[cs].spare_area_bytes;
    if (ecc_config > 0x40) ecc_config = 0x40;
        else if (ecc_config < 0x10)
            /* controller does not handle spare area less than 16 bytes. */
            return 0;

    /* make sure spare area number is even for 16 bit devices */
    if ((ecc_config & 0x01) && (inand[cs].device_width == 16)) ecc_config += 0x01;

    if (ecc_corr_bits == 0){
        return ecc_config;
    }

    /* For 1 bit correction we have two choices depending on the */
    /* size of spare area */
    if ((ecc_corr_bits == 1) && (ecc_config == 0x10)) {
        /* use hamming code */
        ecc_level = 0x000F0000;
        ecc_config |= ecc_level;
        return ecc_config;
    }

    else if ((ecc_corr_bits == 1) && (ecc_config != 0x10)) {
        /* use BCH code */
        ecc_level = 0x00010000;
        ecc_config |= ecc_level;
        return ecc_config;
    }

    /* At this point if 'n' bit correction < 8, we roughly have */
    /* enough spare area size that can support that */
    else if (ecc_corr_bits < 8) {
        ecc_level = ecc_corr_bits;
        ecc_level <<= 16;
        ecc_config |= ecc_level;
        return ecc_config;
    }

    /* we need to check to see if we have enough spare area for */
    /* the required ECC level. */
    else if (ecc_corr_bits < 18){
        /* check spare bytes required for ECC level */
        ecc_bytes_req = (ecc_corr_bits * 14 / 8) + 2;
        if (ecc_bytes_req > inand[cs].spare_area_bytes)
            /* we can not handle the required ECC level */
            /* with spare area size we have. */
            return 0;

        ecc_level = ecc_corr_bits;
        ecc_level <<= 16;
        ecc_config |= ecc_level;
        return ecc_config;
    }

    /* if ECC level is higher than 17 we have to use bigger */
    /* sector size */
    else
    {
        if (page_size == IPROC_FLASH_CACHE_SIZE)
        {
            /* page size does not fit bigger sector */
            return 0;
        }

        else
        {
            ecc_bytes_req = ((ecc_corr_bits * 14 / 8) + 2) / 2;
            if (ecc_bytes_req > inand[cs].spare_area_bytes)
                /* we can not handle the required ECC level */
                /* with spare area size we have. */
                return 0;

            ecc_level = ((ecc_corr_bits >> 1) << 16) | (IPROC_NAND_1K_SECTOR);
            ecc_config |= ecc_level;
            return ecc_config;
        }
    }
}

/***************************************************************************
 * iproc_nand_get_config
 *******************************i******************************************/
int iproc_nand_get_config (
    int cs) 
{
    uint32_t reg_data = 0;
    uint32_t block_code = 0; 
    uint32_t page_code = 0;
    uint32_t device_code = 0;
    int i, found;

    /* read the register configuartion */
    reg_data = readl(REG_CONFIG(cs));
#ifdef NAND_INFO
    printf("cs %d REG_CONFIG %x read\n", cs, reg_data);
#endif

    memset(&inand[cs], 0, sizeof(INAND));
    block_code = ((reg_data & IPROC_NAND_BLOCK_SIZE_MASK) >> IPROC_NAND_BLOCK_SIZE_SHIFT);
#ifdef NAND_INFO
    printf("cs %d block_code %x\n", cs, block_code);
#endif

    for (i = 0, found = 0; i < ARRAY_SIZE(block_sizes); i++) {
        if (i == block_code) {
            inand[cs].block_size = block_sizes[i];
            found = 1;
            break;
        }
    }
    if (!found)
        return NAND_CONFIG_UNKNOWN;
#ifdef NAND_INFO
    printf("cs %d block size %d KB\n", cs, inand[cs].block_size);
#endif

    page_code = ((reg_data & IPROC_NAND_PAGE_SIZE_MASK) >> IPROC_NAND_PAGE_SIZE_SHIFT);
    for (i = 0, found = 0; i < ARRAY_SIZE(page_sizes); i++) {
        if (i == page_code) {
            inand[cs].page_size = page_sizes[i];
            found = 1;
            break;
        }
    }
    if (!found)
        return NAND_CONFIG_UNKNOWN;
#ifdef NAND_INFO
    printf("cs %d page size %d bytes\n", cs, inand[cs].page_size);
#endif

    device_code = ((reg_data & IPROC_NAND_DEVICE_SIZE_MASK) >> IPROC_NAND_DEVICE_SIZE_SHIFT);
    for (i = 0, found = 0; i < ARRAY_SIZE(device_sizes); i++) {
        if (i == device_code) {
            inand[cs].device_size = device_sizes[i];
            found = 1;
            break;
        }
    }
    if (!found)
        return NAND_CONFIG_UNKNOWN;
#ifdef NAND_INFO
    printf("cs %d device size %d MB\n", cs, inand[cs].device_size);
#endif

    inand[cs].device_width = 8;
    if (reg_data & IPROC_NAND_SELECT_AUTO_DEVID_CONFIG_BIT)
        inand[cs].device_width = 16;
#ifdef NAND_INFO
    printf("cs %d device width %d bit\n", cs, inand[cs].device_width);
#endif
    inand[cs].initialized = IPROC_INIT_MAGIC;

#ifdef NAND_INFO
    printf("NAND: %d MiB total, %u KiB blocks, %u KiB pages, %uB OOB, %u-bit\n", 
            inand[cs].device_size, inand[cs].block_size/1024, 
            inand[cs].page_size/1024, inand[cs].spare_area_bytes, 
            inand[cs].device_width);
#endif

    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_ecc_get_config
 *******************************i******************************************/
int iproc_nand_ecc_get_config(
    uint32_t cs, 
    uint32_t *ecc_level)
{
    uint32_t rdata;
    uint32_t acc_ctl_reg;

    acc_ctl_reg = REG_ACC_CONTROL(cs);
    rdata = readl(acc_ctl_reg);

    *ecc_level = (rdata & IPROC_NAND_ECC_LEVEL_MASK) >> IPROC_NAND_ECC_LEVEL_SHIFT;

    return NAND_STATUS_OK;
}

/***************************************************************************
 * iproc_nand_page_program
 *******************************i******************************************/
int iproc_nand_page_program(
    int cs,
    uint64_t page_addr,
    uint8_t* src_addr,
    uint32_t data_len)
{
    int rv;
	uint64_t block_addr;
	uint32_t data_written = 0;
	uint32_t len;
    
    if (!inand || (inand[cs].initialized != IPROC_INIT_MAGIC)) {
        rv = iproc_nand_config(cs);
		if (rv) {
			printf ("NAND config error %d\n", rv);
			return rv;
		}
	}

#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called page_addr %llx len %x from %x\n", __func__, page_addr, data_len, (unsigned)src_addr);
#endif

	while (data_written < data_len) {
		/* write it one block at a time */
		len = inand[cs].page_size;

		if (data_len - data_written < inand[cs].page_size) 
		    len = data_len - data_written;

		block_addr = page_addr & (~(inand[cs].block_size - 1));
		if (page_addr == block_addr) {
			iproc_nand_block_erase (cs, block_addr);
		}

        rv = _iproc_nand_page_program(cs, page_addr, src_addr, len); 
		if (rv) {
#ifdef IPROC_NAND_WARNING
			printf ("_iproc_nand_page_program returned %d\n", rv);
#endif
			return rv;
		}
		data_written += inand[cs].page_size;
		page_addr += inand[cs].page_size;
	}
    return 0; 
}

/***************************************************************************
 * iproc_nand_page_read
 *******************************i******************************************/
int iproc_nand_page_read(
    int cs,
    uint64_t page_addr,
    uint8_t *dst_addr,
    uint32_t data_len)
{
    int rv;
    
    if (!inand || (inand[cs].initialized != IPROC_INIT_MAGIC)) {
        rv = iproc_nand_config(cs);
		if (rv) {
			printf ("NAND config error %d\n", rv);
			return rv;
		}
	}

    rv =  _iproc_nand_page_read(cs, page_addr, dst_addr, data_len); 
    return rv;
}

/***************************************************************************
 * iproc_nand_block_erase
 *******************************i******************************************/
int iproc_nand_block_erase(
    int cs,
    uint64_t block_address) 
{
    int rv;
    
#ifdef IPROC_NAND_DEBUG_CALLS
    printf("%s: called block_addr %llx\n", __func__, block_address);
#endif
    if (!inand || (inand[cs].initialized != IPROC_INIT_MAGIC)) {
        rv = iproc_nand_config(cs);
		if (rv) {
			printf ("NAND config error %d\n", rv);
			return rv;
		}
	}

    return _iproc_nand_block_erase(cs, block_address); 
}
