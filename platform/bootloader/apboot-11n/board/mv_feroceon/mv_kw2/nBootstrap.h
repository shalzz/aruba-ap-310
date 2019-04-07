/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/


#ifndef __INCnBootstraph
#define __INCnBootstraph

/* includes */

/* defines  */
#define _DDR2

#define SDRAM_CONFIG_REG_DV                 0x03154400     /* 1400 */
#define SDRAM_DUNIT_CTRL_REG_DV             0x04041040      /* 1404 */
#define SDRAM_TIMING_CTRL_LOW_REG_DVAL      0x11712220      /* 1408 */
#define SDRAM_TIMING_CTRL_HIGH_REG_DVAL     0x00000104      /* 140C */
#define SDRAM_ADDR_CTRL_REG_DV              0x00000020      /* 1410 */
#define SDRAM_OPEN_PAGE_CTRL_REG_DV         0x00000000      /* 1414 */
#define SDRAM_MODE_REG_DV                   0x00000432      /* 141C */
#define SDRAM_EXTENDED_MODE_REG_DV          0x00000440      /* 1420 */
#define SDRAM_FTDLL_CONFIG_REG_DV           0x00f95000      /* 1484 */
#define DDR2_SDRAM_ODT_CTRL_LOW_REG_DV      0x84210000      /* 1494 */
#define DDR2_SDRAM_ODT_CTRL_HIGH_REG_DV     0x00000000      /* 1498 */
#define DDR2_DUNIT_ODT_CTRL_REG_DV          0x0000780f      /* 149C */
#define SDRAM_SIZE_REG_DV                   0x07ff0001  /* 128MB */

/* NAND Flash access */
#define NAND_CMD_PORT       (0x1 << (NFLASH_DEV_WIDTH >> 4))
#define NAND_ADDR_PORT      (0x2 << (NFLASH_DEV_WIDTH >> 4))

/* NAND Flash Chip Capability */
#ifdef MV_LARGE_PAGE
#define NUM_BLOCKS		2048
#define PAGES_PER_BLOCK     	64
//alior #define PAGE_SIZE      		2048	 /* Bytes */
#define SPARE_SIZE        	64
#define CFG_NAND_PAGE_SIZE	(2048)		/* NAND chip page size		*/
#define CFG_NAND_BLOCK_SIZE	(128 << 10)	/* NAND chip block size		*/
#define CFG_NAND_PAGE_COUNT	(64)		/* NAND chip page count		*/
#define CFG_NAND_BAD_BLOCK_POS	(0)		/* Location of bad block marker	*/

#define CFG_NAND_U_BOOT_OFFS	CONFIG_SYS_MONITOR_BASE	/* Offset to U-Boot image	*/
#define CFG_NAND_U_BOOT_SIZE	CONFIG_SYS_MONITOR_LEN	/* Size of RAM U-Boot image	*/
#define CFG_NAND_U_BOOT_DST	MV_MONITOR_IMAGE_DST	/* Load NUB to this addr	*/
#define CFG_NAND_U_BOOT_START	CFG_NAND_U_BOOT_DST /* Start NUB from this addr	*/

#else /* ! LARGE PAGE NAND */
/* NAND Flash Chip Capability */
#define NUM_BLOCKS		2048
#define PAGES_PER_BLOCK     	32
#define PAGE_SIZE      		512	 /* Bytes */
#define SPARE_SIZE        	16
#define CFG_NAND_PAGE_SIZE	(512)		/* NAND chip page size		*/
#define CFG_NAND_BLOCK_SIZE	(16 << 10)	/* NAND chip block size		*/
#define CFG_NAND_PAGE_COUNT	(32)		/* NAND chip page count		*/
#define CFG_NAND_BAD_BLOCK_POS	(5)		/* Location of bad block marker	*/

#define CFG_NAND_U_BOOT_OFFS	CONFIG_SYS_MONITOR_BASE	/* Offset to U-Boot image	*/
#define CFG_NAND_U_BOOT_SIZE	CONFIG_SYS_MONITOR_LEN	/* Size of RAM U-Boot image	*/
#define CFG_NAND_U_BOOT_DST	MV_MONITOR_IMAGE_DST	/* Load NUB to this addr	*/
#define CFG_NAND_U_BOOT_START	CFG_NAND_U_BOOT_DST /* Start NUB from this addr	*/

#endif

/*  NAND Flash Command. This appears to be generic across all NAND flash chips */
#define CMD_READ            0x00        /*  Read */
#define CMD_READ1           0x01        /*  Read1 */
#define CMD_READ2           0x50        /*  Read2 */
#define CMD_START_READ      0x30        /*  Read command after write addr */
#define CMD_READID          0x90        /*  ReadID */
#define CMD_READID2         0x91        /*  Read extended ID */
#define CMD_WRITE           0x80        /*  Write phase 1 */
#define CMD_WRITE2          0x10        /*  Write phase 2 */
#define CMD_ERASE           0x60        /*  Erase phase 1 */
#define CMD_ERASE2          0xd0        /*  Erase phase 2 */
#define CMD_STATUS          0x70        /*  Status read */
#define CMD_RESET           0xff        /*  Reset */

/*  Status bit pattern */
#define STATUS_READY        0x40        /*  Ready */
#define STATUS_ERROR        0x01        /*  Error */


#define NFLASH_DEV_WIDTH    8
#ifdef MV_LARGE_PAGE
#define BOOTER_PAGE_NUM     2
#define BOOTER_BASE         0x00020000 + PAGE_SIZE
#else
#define BOOTER_PAGE_NUM     5
#define BOOTER_BASE         0x00020000 + (3 * PAGE_SIZE)
#endif /* MV_LARGE_PAGE */
#define BOOTER_END          (BOOTER_BASE + (BOOTER_PAGE_NUM * PAGE_SIZE))

#undef  INTER_REGS_BASE
#define INTER_REGS_BASE		0xd0000000

           
#if defined(MV_BOOTROM)
#if defined(MV_88F6082)
#define NAND_FLASH_BASE		0xD8000000
#endif
#if defined(MV_88F5182)
#define NAND_FLASH_BASE		0xf0000000
#endif
#else
#define NAND_FLASH_BASE		0xffff0000
#endif /* defined(MV_BOOTROM) */

#if 0
#define NBOOT_UART_CHAN     0
#define NBOOT_BAUDRATE      115200
#define NBOOT_TIMER_NUM     0


/* CPU config register (0x20100) bit[15:8] value for CPU to DDR clock ratio */ 
#define CPU_2_MBUSL_DDR_CLK     0x0000   /* clock ratio 1x2 */
/* #define CPU_2_MBUSL_DDR_CLK     0x2100  *//*  clock ratio 1x3 */
/* #define CPU_2_MBUSL_DDR_CLK     0x2200  *//* clock ratio 1x4 */
           
/* Load General Purpose Register (GPR) with 32-bit constant value */
#define GPR_LOAD(reg, val)                     \
        mov     reg, $(val & 0xFF)             ;\
        orr     reg, reg, $(val & 0xFF00)      ;\
        orr     reg, reg, $(val & 0xFF0000)    ;\
        orr     reg, reg, $(val & 0xFF000000)

/* Register Read/Write */
#define MV_REG_READ_ASM(toReg, baseReg, regOffs)         \
        ldr     toReg, [baseReg, $(regOffs & 0xFFF)]
        
#define MV_REG_WRITE_ASM(fromReg, baseReg, regOffs)      \
        str     fromReg, [baseReg, $(regOffs & 0xFFF)]
        

/* 32bit byte swap. For example 0x11223344 -> 0x44332211                    */
#define MV_BYTE_SWAP_32BIT(X) ((((X)&0xff)<<24) |                       \
                               (((X)&0xff00)<<8) |                      \
                               (((X)&0xff0000)>>8) |                    \
                               (((X)&0xff000000)>>24))
/* Endianess macros.                                                        */
#if defined(MV_CPU_LE)
    #define MV_32BIT_LE(X)  (X)
    #define MV_32BIT_BE(X)  MV_BYTE_SWAP_32BIT(X)
#elif defined(MV_CPU_BE)
    #define MV_32BIT_LE(X)  MV_BYTE_SWAP_32BIT(X)
    #define MV_32BIT_BE(X)  (X)
#else
    #error "CPU endianess isn't defined!\n"
#endif
#endif
 
#endif /* __INCnBootstraph */
