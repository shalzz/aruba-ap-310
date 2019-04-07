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

#ifndef _IPROC_NAND_H_
#define _IPROC_NAND_H_

#include <config.h>
#include <linux/types.h>
#include <asm/arch/iproc_regs.h>
#if defined(CONFIG_NORTHSTAR) || defined (CONFIG_NS_PLUS)
#define IPROC_R_STRAPS_CONTROL_ADDR           (CRU_STRAPS_CONTROL_OFFSET) 
#else
#define IPROC_R_STRAPS_CONTROL_ADDR           (DMU_PCU_IPROC_STRAPS_CAPTURED) 
#endif
#define IPROC_R_IDM_IDM_IO_CONTROL_DIRECT     (NAND_IDM_IDM_IO_CONTROL_DIRECT)
#define IPROC_R_IDM_IDM_RESET_CONTROL_ADDR    (NAND_IDM_IDM_RESET_CONTROL)
#define IPROC_R_NAND_BASE_ADDR                (NAND_nand_flash_REVISION)
#define IPROC_R_NAND_REVISION_ADDR            (NAND_nand_flash_REVISION)
#define IPROC_R_NAND_ONFI_STATUS_ADDR         (NAND_nand_flash_ONFI_STATUS)
#define IPROC_R_NAND_ONFI_DEBUG_DATA          (NAND_nand_flash_ONFI_DEBUG_DATA)
#define IPROC_R_NAND_ACC_CONTROL_CS0_ADDR     (NAND_nand_flash_ACC_CONTROL_CS0) 
#define IPROC_R_NAND_ACC_CONTROL_CS1_ADDR     (NAND_nand_flash_ACC_CONTROL_CS1) 
#define IPROC_R_NAND_CMD_ADDRESS_ADDR         (NAND_nand_flash_CMD_ADDRESS)
#define IPROC_R_NAND_CMD_ADDRESS_END_ADDR     (NAND_nand_flash_CMD_END_ADDRESS)
#define IPROC_R_NAND_CMD_EXT_ADDRESS_ADDR     (NAND_nand_flash_CMD_EXT_ADDRESS)
#define IPROC_R_NAND_CMD_START_ADDR           (NAND_nand_flash_CMD_START)
#define IPROC_R_NAND_CONFIG_CS0_ADDR          (NAND_nand_flash_CONFIG_CS0)
#define IPROC_R_NAND_CONFIG_CS1_ADDR          (NAND_nand_flash_CONFIG_CS1)
#define IPROC_R_NAND_CS_NAND_SELECT_ADDR      (NAND_nand_flash_CS_NAND_SELECT)
#define IPROC_R_NAND_FLASH_CACHE_0_ADDR       (NAND_nand_flash_FLASH_CACHE0)
#define IPROC_R_NAND_FLASH_DEVICE_ID_ADDR     (NAND_nand_flash_FLASH_DEVICE_ID)
#define IPROC_R_NAND_FLASH_DEVICE_ID_EXT_ADDR (NAND_nand_flash_FLASH_DEVICE_ID_EXT)
#define IPROC_R_NAND_INIT_STATUS_ADDR         (NAND_nand_flash_INIT_STATUS)
#define IPROC_R_NAND_INTFC_STATUS_ADDR        (NAND_nand_flash_INTFC_STATUS)
#define IPROC_R_NAND_RO_CTRL_READY_ADDR       (NAND_ro_ctlr_ready)
#define IPROC_R_NAND_UNCORR_ERROR_COUNT_ADDR  (NAND_nand_flash_UNCORR_ERROR_COUNT)
#define IPROC_R_NAND_UNCORR_ERROR_ADDRESS_ADDR (NAND_nand_flash_ECC_UNC_ADDR)
#define IPROC_R_NAND_CORR_ERROR_COUNT_ADDR    (NAND_nand_flash_CORR_ERROR_COUNT)
#define IPROC_R_NAND_CORR_ERROR_ADDRESS_ADDR  (NAND_nand_flash_ECC_CORR_ADDR)
#define IPROC_R_NAND_XOR_ADDR                 (NAND_nand_flash_CS_NAND_XOR)
#define IPROC_R_NAND_SPARE_AREA_READ_OFS_0    (NAND_nand_flash_SPARE_AREA_READ_OFS_0)
#define IPROC_R_NAND_SPARE_AREA_WRITE_OFS_0   (NAND_nand_flash_SPARE_AREA_WRITE_OFS_0)

#define IPROC_NAND_IDM_IO_CONTROL_APB_LE_MODE_BIT       0x01000000
#define IPROC_NAND_BLOCK_SIZE_MASK                      0x70000000
#define IPROC_NAND_BLOCK_SIZE_SHIFT                     28
#define IPROC_NAND_DEVICE_SIZE_MASK                     0x0f000000
#define IPROC_NAND_DEVICE_SIZE_SHIFT                    24
#define IPROC_NAND_DEVICE_WIDTH_MASK                    0x00800000
#define IPROC_NAND_DEVICE_WIDTH_SHIFT                   23
#define IPROC_NAND_BLOCK_ADDR_BYTE_SHIFT                8
#define	IPROC_NAND_COLUMN_ADDR_BYTE_SHIFT               12
#define IPROC_NAND_FULL_ADDR_BYTE_SHIFT                 16
#define IPROC_NAND_PAGE_SIZE_MASK                       0x00300000
#define IPROC_NAND_PAGE_SIZE_SHIFT                      20
#define IPROC_NAND_ECC_LEVEL_MASK                       0x001f0000
#define IPROC_NAND_ECC_LEVEL_SHIFT                      16 
#define IPROC_NAND_CONTROL_READY_BIT                    0x80000000
#define IPROC_NAND_FLASH_READY_BIT                      0x40000000
#define IPROC_NAND_CACHE_VALID_BIT                      0x20000000
#define IPROC_NAND_INTFC_STATUS_MASK                    0x000000FF
#define IPROC_NAND_INTFC_STATUS_READY_BIT               0x00000040
#define IPROC_NAND_INTFC_STATUS_FAIL_BIT                0x00000001
#define IPROC_NAND_XOR_CS_MASK                          0x000000FF
#define IPROC_NAND_DIRECT_ACCESS_CS_MASK                0x000000FF
#define IPROC_NAND_BLOCK_SIZE_MASK                      0x70000000
#define IPROC_NAND_BLOCK_SIZE_SHIFT                     28
#define IPROC_NAND_DEVICE_WIDTH_BIT                     0x00800000
#define IPROC_NAND_PAGE_SIZE_MASK                       0x00300000
#define IPROC_NAND_PAGE_SIZE_SHIFT                      20
#define IPROC_NAND_SELECT_AUTO_DEVID_CONFIG_BIT         0x40000000
#define IPROC_NAND_ONFI_STRING_DETECTED                 0x08000000
#define IPROC_NAND_SELECT_AUTO_DEVICE_ID_BIT            0x40000000
#define IPROC_NAND_PREFETCH_EN                          0x00800000
#define IPROC_NAND_PAGE_HIT_EN                          0x01000000
#define IPROC_NAND_ECC_READ_EN                          0x80000000
#define IPROC_NAND_ECC_WRITE_EN                         0x40000000
#define IPROC_NAND_1K_SECTOR                            0x00000080
#define IPROC_NAND_WRITE_PROCTECT_BIT                   0x20000000
#define IPROC_NAND_STATUS_PROTECT_BIT                   0x00000080
#if defined(CONFIG_NORTHSTAR) || defined (CONFIG_NS_PLUS)
#define IPROC_NAND_STRAP_TYPE_MASK                      0x0000f000
#define IPROC_NAND_STRAP_TYPE_SHIFT                     12
#define IPROC_NAND_STRAP_PAGE_MASK                      0x00000c00
#define IPROC_NAND_STRAP_PAGE_SHIFT                     10
#else
#define IPROC_NAND_STRAP_TYPE_MASK	        (0xf << DMU_PCU_IPROC_STRAPS_CAPTURED__strap_nand_type_R)
#define IPROC_NAND_STRAP_TYPE_SHIFT         (DMU_PCU_IPROC_STRAPS_CAPTURED__strap_nand_type_R)
#define IPROC_NAND_STRAP_PAGE_MASK	        (0x3 << DMU_PCU_IPROC_STRAPS_CAPTURED__strap_nand_page_R)
#define IPROC_NAND_STRAP_PAGE_SHIFT         (DMU_PCU_IPROC_STRAPS_CAPTURED__strap_nand_page_R)
#endif
#define IPROC_NAND_8bit_BAD_BLOCK_MASK					0xFF000000
#define IPROC_NAND_16bit_BAD_BLOCK_MASK					0x00FF0000

#define IPROC_NAND_ONFI_PARAMETER_CRC_ERR_PAGE0_BIT     0x001
#define IPROC_NAND_ONFI_PARAMETER_CRC_ERR_PAGE1_BIT     0x002
#define IPROC_NAND_ONFI_PARAMETER_CRC_ERR_PAGE2_BIT     0x004
#define IPROC_NAND_ONFI_PARAMETER_BAD_IDENT_PAGE0_BIT   0x008
#define IPROC_NAND_ONFI_PARAMETER_BAD_IDENT_PAGE1_BIT   0x010
#define IPROC_NAND_ONFI_PARAMETER_BAD_IDENT_PAGE2_BIT   0x020
#define IPROC_NAND_ONFI_PARAMETER_PAGE_MASK             0xfff
#define ONFI_PAGE_SIZE_READ_MASK                        0x28000000
#define ONFI_BLOCK_COUNT_READ_MASK                      0x48000000
#define ONFI_PAGES_PER_BLOCK_READ_MASK                  0x58000000

#define IPROC_IDM_SKU_MASK                              0x0000000C
#define IPROC_IDM_SKU_SHIFT                             2
#define IDM_SKU_BCM953010                               1
#define IDM_SKU_BCM953011                               2
#define IDM_SKU_BCM953012                               3

#define PAGE_READ                                       0x01000000
#define SPARE_AREA_READ                                 0x02000000
#define STATUS_READ                                     0x03000000
#define PROGRAM_PAGE                                    0x04000000
#define PROGRAM_SPARE_AREA                              0x05000000
#define DEVICE_ID_READ                                  0x07000000
#define BLOCK_ERASE                                     0x08000000
#define FLASH_RESET                                     0x09000000
#define PARAMETER_READ                                  0x0e000000
#define PARAMETER_CHANGE_COL                            0x0f000000
#define PAGE_READ_MULTI                                 0x11000000
#define PROGRAM_PAGE_MULTI                              0x13000000
#define BLOCK_ERASE_MULTI                               0x15000000

#define IPROC_NAND_CMD_EXT_CS_SELECT_CS0                0x00000000
#define IPROC_NAND_CMD_EXT_CS_SELECT_CS1                0x00010000

#define IPROC_NAND_INIT_SUCCESS_BIT                     0x20000000
#define IPROC_NAND_DEVICE_ID_INIT_BIT                   0x40000000
#define IPROC_NAND_ONFI_INIT_BIT                        0x80000000


#define IPROC_FLASH_CACHE_SIZE                          512
#define IPROC_FLASH_CACHE_WORDS                         128 
#define SPARE_AREA_WORDS                                64
#define IPROC_NAND_BADBLOCK_POS		                    5

#define FC_SHIFT                                        9
#define FC_BYTES                                        512
#define FC_WORDS                                        (FC_BYTES/4)

/* Status conditions */
typedef enum 
{
    NAND_STATUS_OK       = 0,
    NAND_TIMEOUT_ERROR   = 1,
    NAND_UNCOR_ECC_ERROR = 2,
    NAND_AUTOCONFIG_FAIL = 3,
    NAND_UNINITIALIZED   = 4,
    NAND_CONFIG_PARAM    = 5,
    NAND_CONFIG_UNKNOWN  = 6,
    NAND_CACHE_INVALID   = 7,
    NAND_ALLOC           = 8,
	NAND_NOT_SUPPORTED   = 9,
    NAND_PAGE_ALIGNMENT  = 10,
	NAND_OPERATION_FAIL  = 11,
    NAND_TEST_ERROR      = 12,
} NAND_STATUS;

int iproc_nand_init(uint32_t cs);
int iproc_nand_onfi_parameter_pages(int cs, uint32_t *param_page);
uint32_t iproc_nand_onfi_ecc_update (int cs, uint32_t ecc_corr_bits);
int iproc_nand_ecc_set_config(uint32_t cs, uint32_t ecc_level);
void iproc_nand_device_param(uint8_t mnfc_id, uint8_t dev_id, uint32_t *device_size);
int iproc_nand_get_config(int cs); 
int iproc_nand_set_cs_select(int cs);
int iproc_nand_ecc_get_config(uint32_t cs, uint32_t *ecc_level);

int iproc_nand_scan_bad_blocks(int cs);
int iproc_nand_block_erase(int cs, uint64_t block_address);
int iproc_nand_page_read(int cs, uint64_t page_addr, uint8_t* dst_addr, uint32_t data_len);
int iproc_nand_page_program(int cs, uint64_t page_addr, uint8_t* src_addr, uint32_t data_size);

#endif /* _IPROC_NAND_H_*/
