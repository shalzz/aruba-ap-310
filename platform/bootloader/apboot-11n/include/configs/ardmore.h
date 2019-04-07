/*
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * QorIQ RDB boards configuration file
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef CONFIG_36BIT
#define CONFIG_PHYS_64BIT
#endif

#if defined(CONFIG_P1020MBG)
#define CONFIG_BOARDNAME "P1020MBG"
#define CONFIG_P1020
#define CONFIG_VSC7385_ENET
#define CONFIG_SLIC
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0xe4
#define __SW_BOOT_SD		0x54
#endif

#if defined(CONFIG_P1020UTM)
#define CONFIG_BOARDNAME "P1020UTM"
#define CONFIG_P1020
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0xe0
#define __SW_BOOT_SD		0x50
#endif

#if defined(CONFIG_P1020RDB)
#define CONFIG_BOARDNAME "P1020RDB"
#define CONFIG_NAND_FSL_ELBC
#define CONFIG_P1020
#define CONFIG_SPI_FLASH
#define CONFIG_VSC7385_ENET
#define CONFIG_SLIC
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0x5c
#define __SW_BOOT_SPI		0x1c
#define __SW_BOOT_SD		0x9c
#define __SW_BOOT_NAND		0xec
#define __SW_BOOT_PCIE		0x6c
#endif

#if defined(CONFIG_P1020WLAN)
#define CONFIG_BOARDNAME "P1020WLAN"
#undef CONFIG_NAND_FSL_ELBC
#define CONFIG_P1020
#undef CONFIG_SPI_FLASH
#undef CONFIG_VSC7385_ENET
#undef CONFIG_SLIC
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0x5c
#define __SW_BOOT_SPI		0x1c
#define __SW_BOOT_SD		0x9c
#define __SW_BOOT_NAND		0xec
#define __SW_BOOT_PCIE		0x6c
#endif

#if defined(CONFIG_ARDMORE)
#define CONFIG_BOARDNAME "ARDMORE"
#undef CONFIG_NAND_FSL_ELBC
#define CONFIG_P1020
#undef CONFIG_SPI_FLASH
#undef CONFIG_VSC7385_ENET
#undef CONFIG_SLIC
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0x5c
#define __SW_BOOT_SPI		0x1c
#define __SW_BOOT_SD		0x9c
#define __SW_BOOT_NAND		0xec
#define __SW_BOOT_PCIE		0x6c
#endif

#if defined(CONFIG_P1021RDB)
#define CONFIG_BOARDNAME "P1021RDB"
#define CONFIG_NAND_FSL_ELBC
#define CONFIG_P1021
#define CONFIG_QE
#define CONFIG_SPI_FLASH
#define CONFIG_VSC7385_ENET
#define CONFIG_SYS_LBC_LBCR	0x00080000	/* Implement conversion of
						addresses in the LBC */
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0x5c
#define __SW_BOOT_SPI		0x1c
#define __SW_BOOT_SD		0x9c
#define __SW_BOOT_NAND		0xec
#define __SW_BOOT_PCIE		0x6c
#endif

#if defined(CONFIG_P1024RDB)
#define CONFIG_BOARDNAME "P1024RDB"
#define CONFIG_NAND_FSL_ELBC
#define CONFIG_P1024
#define CONFIG_SLIC
#define CONFIG_SPI_FLASH
#define __SW_BOOT_MASK		0xf3
#define __SW_BOOT_NOR		0x00
#define __SW_BOOT_SPI		0x08
#define __SW_BOOT_SD		0x04
#define __SW_BOOT_NAND		0x0c
#endif

#if defined(CONFIG_P1025RDB)
#define CONFIG_BOARDNAME "P1025RDB"
#define CONFIG_NAND_FSL_ELBC
#define CONFIG_P1025
#define CONFIG_QE
#define CONFIG_SLIC
#define CONFIG_SPI_FLASH

#define CONFIG_SYS_LBC_LBCR	0x00080000	/* Implement conversion of
						addresses in the LBC */
#define __SW_BOOT_MASK		0xf3
#define __SW_BOOT_NOR		0x00
#define __SW_BOOT_SPI		0x08
#define __SW_BOOT_SD		0x04
#define __SW_BOOT_NAND		0x0c
#endif

#if defined(CONFIG_P2020RDB)
#define CONFIG_BOARDNAME "P2020RDB"
#define CONFIG_NAND_FSL_ELBC
#define CONFIG_P2020
#define CONFIG_SPI_FLASH
#define CONFIG_VSC7385_ENET
#define __SW_BOOT_MASK		0x03
#define __SW_BOOT_NOR		0xc8
#define __SW_BOOT_SPI		0x28
#define __SW_BOOT_SD		0x68 /* or 0x18 */
#define __SW_BOOT_NAND		0xe8
#define __SW_BOOT_PCIE		0xa8
#endif

#ifdef CONFIG_SDCARD
#define CONFIG_RAMBOOT_SDCARD
#define CONFIG_SYS_RAMBOOT
#define CONFIG_SYS_EXTRA_ENV_RELOC
#define CONFIG_SYS_TEXT_BASE		0x11000000
#define CONFIG_RESET_VECTOR_ADDRESS	0x1107fffc
#endif

#ifdef CONFIG_SPIFLASH
#define CONFIG_RAMBOOT_SPIFLASH
#define CONFIG_SYS_RAMBOOT
#define CONFIG_SYS_EXTRA_ENV_RELOC
#define CONFIG_SYS_TEXT_BASE		0x11000000
#define CONFIG_RESET_VECTOR_ADDRESS	0x1107fffc
#endif

#if defined(CONFIG_NAND) && defined(CONFIG_NAND_FSL_ELBC)
#define CONFIG_NAND_U_BOOT
#define CONFIG_SYS_EXTRA_ENV_RELOC
#define CONFIG_SYS_RAMBOOT
#define CONFIG_SYS_TEXT_BASE_SPL	0xff800000
#ifdef CONFIG_NAND_SPL
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE_SPL
#else
#define CONFIG_SYS_TEXT_BASE		0x11001000
#endif /* CONFIG_NAND_SPL */
#endif

#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE		0xeff80000
#endif

#ifndef CONFIG_RESET_VECTOR_ADDRESS
#define CONFIG_RESET_VECTOR_ADDRESS	0xeffffffc
#endif

#ifndef CONFIG_SYS_MONITOR_BASE
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE	/* start of monitor */
#endif

/* High Level Configuration Options */
#define CONFIG_BOOKE
#define CONFIG_E500
#define CONFIG_MPC85xx

#define CONFIG_MP

#define CONFIG_FSL_ELBC
#define CONFIG_PCI
#define CONFIG_PCIE1	/* PCIE controler 1 (slot 1) */
#define CONFIG_PCIE2	/* PCIE controler 2 (slot 2) */
#define CONFIG_FSL_PCI_INIT	/* Use common FSL init code */
#define CONFIG_FSL_PCIE_RESET	/* need PCIe reset errata */
#define CONFIG_SYS_PCI_64BIT	/* enable 64-bit PCI resources */

#define CONFIG_FSL_LAW
#define CONFIG_TSEC_ENET	/* tsec ethernet support */
#define CONFIG_ENV_OVERWRITE

#undef CONFIG_CMD_SATA
#undef CONFIG_SATA_SIL3114
#undef CONFIG_SYS_SATA_MAX_DEVICE	//	2
#undef CONFIG_LIBATA
#undef CONFIG_LBA48

#if defined(CONFIG_P2020RDB)
#define CONFIG_SYS_CLK_FREQ	100000000
#else
#define CONFIG_SYS_CLK_FREQ	66666666
#endif
#define CONFIG_DDR_CLK_FREQ	66666666

#define CONFIG_HWCONFIG
/*
 * These can be toggled for performance analysis, otherwise use default.
 */
#define CONFIG_L2_CACHE
#define CONFIG_BTB

#define CONFIG_BOARD_EARLY_INIT_F	/* Call board_pre_init */
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_ENABLE_36BIT_PHYS
#endif

#ifdef CONFIG_PHYS_64BIT
#define CONFIG_ADDR_MAP			1
#define CONFIG_SYS_NUM_ADDR_MAP		16	/* number of TLB1 entries */
#endif

#define CONFIG_SYS_MEMTEST_START	0x00200000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x1fffffff
//#define CONFIG_PANIC_HANG	/* do not reset board on panic */

/*
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 */
#define CONFIG_SYS_CCSRBAR	0xffe00000	/* relocated CCSRBAR */
#define CONFIG_SYS_CCSRBAR_DEFAULT	0xff700000	/* CCSRBAR Default */

/* CCSRBAR PHYSICAL Address */
/* IN case of NAND bootloader relocate CCSRBAR in RAMboot code not in the 4k
       SPL code*/
#if defined(CONFIG_NAND_U_BOOT) && defined(CONFIG_NAND_SPL)
#define CONFIG_SYS_CCSRBAR_PHYS	CONFIG_SYS_CCSRBAR_DEFAULT
#else
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_CCSRBAR_PHYS	0xfffe00000ull
#else
#define CONFIG_SYS_CCSRBAR_PHYS	CONFIG_SYS_CCSRBAR	/* physical addr of */
	/* CCSRBAR */
#endif
#endif
#define CONFIG_SYS_IMMR	CONFIG_SYS_CCSRBAR	/* PQII uses */
	/* CONFIG_SYS_IMMR */

/* DDR Setup */
#define CONFIG_FSL_DDR3
#undef CONFIG_DDR_RAW_TIMING
#undef CONFIG_DDR_SPD
#undef CONFIG_SYS_SPD_BUS_NUM //1
#undef SPD_EEPROM_ADDRESS // 0x52
#define CONFIG_FSL_DDR_INTERACTIVE

#ifdef CONFIG_ARDMORE

#if (__ARDMORE_DRAM__ == 256)
#define CONFIG_SYS_SDRAM_SIZE_LAW	LAW_SIZE_256M
#elif (__ARDMORE_DRAM__ == 512)
#define CONFIG_SYS_SDRAM_SIZE_LAW	LAW_SIZE_512M
#else
#error "Unknown Ardmore DRAM size"
#endif

#define CONFIG_CHIP_SELECTS_PER_CTRL	1
#else
#define CONFIG_SYS_SDRAM_SIZE_LAW	LAW_SIZE_1G
#define CONFIG_CHIP_SELECTS_PER_CTRL	1
#endif
#define CONFIG_SYS_SDRAM_SIZE		(1u << (CONFIG_SYS_SDRAM_SIZE_LAW - 19))
#define CONFIG_SYS_DDR_SDRAM_BASE	0x00000000
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

#define CONFIG_NUM_DDR_CONTROLLERS	1
#define CONFIG_DIMM_SLOTS_PER_CTLR	1

#if 0
/* Default settings for DDR3 */
#if defined(CONFIG_P2020RDB)
#define CONFIG_SYS_DDR_CS0_BNDS		0x0000003f
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80014202
#define CONFIG_SYS_DDR_CS0_CONFIG_2	0x00000000

#define CONFIG_SYS_DDR_DATA_INIT	0xdeadbeef
#define CONFIG_SYS_DDR_INIT_ADDR	0x00000000
#define CONFIG_SYS_DDR_INIT_EXT_ADDR	0x00000000
#define CONFIG_SYS_DDR_MODE_CONTROL	0x00000000

#define CONFIG_SYS_DDR_ZQ_CONTROL	0x89080600
#define CONFIG_SYS_DDR_WRLVL_CONTROL	0x8645F607
#define CONFIG_SYS_DDR_SR_CNTR		0x00000000
#define CONFIG_SYS_DDR_RCW_1		0x00000000
#define CONFIG_SYS_DDR_RCW_2		0x00000000
#define CONFIG_SYS_DDR_CONTROL		0xC7000000	/* Type = DDR3	*/
#define CONFIG_SYS_DDR_CONTROL_2	0x24401000
#define CONFIG_SYS_DDR_TIMING_4		0x00220001
#define CONFIG_SYS_DDR_TIMING_5		0x02401400

#define CONFIG_SYS_DDR_TIMING_3		0x00020000
#define CONFIG_SYS_DDR_TIMING_0		0x00330104
#define CONFIG_SYS_DDR_TIMING_1		0x6f6B4644
#define CONFIG_SYS_DDR_TIMING_2		0x0FA88CCF
#define CONFIG_SYS_DDR_CLK_CTRL		0x02000000
#define CONFIG_SYS_DDR_MODE_1		0x00421422
#define CONFIG_SYS_DDR_MODE_2		0x04000000
#define CONFIG_SYS_DDR_INTERVAL		0x0C300100
#else
#define CONFIG_SYS_DDR_CS0_BNDS		0x0000003f
#define CONFIG_SYS_DDR_CS0_CONFIG	0x80014302
#define CONFIG_SYS_DDR_CS0_CONFIG_2	0x00000000
#define CONFIG_SYS_DDR_CS1_BNDS		0x0040007f
#define CONFIG_SYS_DDR_CS1_CONFIG	0x80014302
#define CONFIG_SYS_DDR_CS1_CONFIG_2	0x00000000

#define CONFIG_SYS_DDR_DATA_INIT	0xdeadbeef
#define CONFIG_SYS_DDR_INIT_ADDR	0x00000000
#define CONFIG_SYS_DDR_INIT_EXT_ADDR	0x00000000
#define CONFIG_SYS_DDR_MODE_CONTROL	0x00000000

#define CONFIG_SYS_DDR_ZQ_CONTROL	0x89080600
#define CONFIG_SYS_DDR_WRLVL_CONTROL	0x8655A608
#define CONFIG_SYS_DDR_SR_CNTR		0x00000000
#define CONFIG_SYS_DDR_RCW_1		0x00000000
#define CONFIG_SYS_DDR_RCW_2		0x00000000
#define CONFIG_SYS_DDR_CONTROL		0xC70C0000	/* Type = DDR3	*/
#define CONFIG_SYS_DDR_CONTROL_2	0x04401050
#define CONFIG_SYS_DDR_TIMING_4		0x00220001
#define CONFIG_SYS_DDR_TIMING_5		0x03402400

#define CONFIG_SYS_DDR_TIMING_3		0x00020000
#define CONFIG_SYS_DDR_TIMING_0		0x00330004
#define CONFIG_SYS_DDR_TIMING_1		0x6f6B4846
#define CONFIG_SYS_DDR_TIMING_2		0x0FA8C8CF
#define CONFIG_SYS_DDR_CLK_CTRL		0x03000000
#define CONFIG_SYS_DDR_MODE_1		0x40461520
#define CONFIG_SYS_DDR_MODE_2		0x8000c000
#define CONFIG_SYS_DDR_INTERVAL		0x0C300000
#endif
#endif	/* 0 */

#undef CONFIG_CLOCKS_IN_MHZ

/*
 * Memory map
 *
 * 0x0000_0000 0x7fff_ffff	DDR	Up to 2GB cacheable
 * 0x8000_0000 0xdfff_ffff	PCI Express Mem	1.5G non-cacheable(PCIe * 3)
 * 0xffc0_0000 0xffc3_ffff	PCI IO range	256k non-cacheable
 *
 * Localbus cacheable (TBD)
 * 0xXXXX_XXXX 0xXXXX_XXXX	SRAM	YZ M Cacheable
 *
 * Localbus non-cacheable
 * 0xec00_0000 0xefff_ffff	FLASH	Up to 64M non-cacheable
 * 0xff80_0000 0xff8f_ffff	NAND flash	1M non-cacheable
 * 0xff90_0000 0xff97_ffff	L2 SDRAM(REV.)  512K cacheable(optional)
 * 0xffd0_0000 0xffd0_3fff	L1 for stack	16K Cacheable TLB0
 * 0xffe0_0000 0xffef_ffff	CCSR	1M non-cacheable
 */


/*
 * Local Bus Definitions
 */
#if defined(CONFIG_P1020MBG) || defined(CONFIG_P1020WLAN)
#define CONFIG_SYS_MAX_FLASH_SECT	512	/* 64M */
#define CONFIG_SYS_FLASH_BASE		0xec000000
#elif defined(CONFIG_P1020UTM) || defined(CONFIG_ARDMORE)
#define CONFIG_SYS_MAX_FLASH_SECT	256	/* 32M */
#define CONFIG_SYS_FLASH_BASE		0xee000000
#else
#define CONFIG_SYS_MAX_FLASH_SECT	128	/* 16M */
#define CONFIG_SYS_FLASH_BASE		0xef000000
#endif


#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_FLASH_BASE_PHYS	0xf00000000ull | CONFIG_SYS_FLASH_BASE
#else
#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE
#endif

#define CONFIG_FLASH_BR_PRELIM (BR_PHYS_ADDR((CONFIG_SYS_FLASH_BASE_PHYS)) \
	| BR_PS_16 | BR_V)

#define CONFIG_FLASH_OR_PRELIM	0xfc000ff7

#define CONFIG_SYS_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE_PHYS}
#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS	45	/* count down from 45/5: 9..1 */

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* number of banks */

#undef CONFIG_SYS_FLASH_CHECKSUM
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_EMPTY_INFO

/* Nand Flash */
#ifdef CONFIG_NAND_FSL_ELBC
#define CONFIG_SYS_NAND_BASE		0xff800000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_NAND_BASE_PHYS	0xfff800000ull
#else
#define CONFIG_SYS_NAND_BASE_PHYS	CONFIG_SYS_NAND_BASE
#endif

#define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND_BASE }
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_MTD_NAND_VERIFY_WRITE
#define CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_BLOCK_SIZE	(16 * 1024)

/* NAND boot: 4K NAND loader config */
#define CONFIG_SYS_NAND_SPL_SIZE	0x1000
#define CONFIG_SYS_NAND_U_BOOT_SIZE	((512 << 10) + CONFIG_SYS_NAND_SPL_SIZE)
#define CONFIG_SYS_NAND_U_BOOT_DST	(0x11000000 - CONFIG_SYS_NAND_SPL_SIZE)
#define CONFIG_SYS_NAND_U_BOOT_START	0x11000000
#define CONFIG_SYS_NAND_U_BOOT_OFFS	(0)
#define CONFIG_SYS_NAND_U_BOOT_RELOC	0x00010000
#define CONFIG_SYS_NAND_U_BOOT_RELOC_SP	(CONFIG_SYS_NAND_U_BOOT_RELOC + 0x10000)

#define CONFIG_NAND_BR_PRELIM	(BR_PHYS_ADDR((CONFIG_SYS_NAND_BASE_PHYS)) \
	| (2<<BR_DECC_SHIFT)	/* Use HW ECC */ \
	| BR_PS_8	/* Port Size = 8 bit */ \
	| BR_MS_FCM	/* MSEL = FCM */ \
	| BR_V)	/* valid */
#define CONFIG_NAND_OR_PRELIM	(OR_AM_32KB	/* small page */ \
	| OR_FCM_CSCT \
	| OR_FCM_CST \
	| OR_FCM_CHT \
	| OR_FCM_SCY_1 \
	| OR_FCM_TRLX \
	| OR_FCM_EHTR)
#endif /* CONFIG_NAND_FSL_ELBC */

#define CONFIG_BOARD_EARLY_INIT_R	/* call board_early_init_r function */

#define CONFIG_SYS_INIT_RAM_LOCK
#define CONFIG_SYS_INIT_RAM_ADDR	0xffd00000 /* stack in RAM */
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS_HIGH 0xf
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS_LOW CONFIG_SYS_INIT_RAM_ADDR
/* The assembler doesn't like typecast */
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS \
	((CONFIG_SYS_INIT_RAM_ADDR_PHYS_HIGH * 1ull << 32) | \
	  CONFIG_SYS_INIT_RAM_ADDR_PHYS_LOW)
#else
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS	CONFIG_SYS_INIT_RAM_ADDR /* Initial L1 address */
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS_HIGH 0
#define CONFIG_SYS_INIT_RAM_ADDR_PHYS_LOW CONFIG_SYS_INIT_RAM_ADDR_PHYS
#endif
#define CONFIG_SYS_INIT_RAM_SIZE	0x00004000 /* Size of used area in RAM */

#define CONFIG_SYS_GBL_DATA_OFFSET	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_OFFSET	CONFIG_SYS_GBL_DATA_OFFSET

#define CONFIG_SYS_MONITOR_LEN	(256 * 1024)/* Reserve 256 kB for Mon */
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)/* Reserved for malloc */

#if 0
#define CONFIG_SYS_CPLD_BASE	0xffa00000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_CPLD_BASE_PHYS	0xfffa00000ull
#else
#define CONFIG_SYS_CPLD_BASE_PHYS	CONFIG_SYS_CPLD_BASE
#endif
/* CPLD config size: 1Mb */
#define CONFIG_CPLD_BR_PRELIM	(BR_PHYS_ADDR(CONFIG_SYS_CPLD_BASE_PHYS) | \
					BR_PS_8 | BR_V)
#define CONFIG_CPLD_OR_PRELIM	(0xfff009f7)
#endif

#define CONFIG_SYS_PMC_BASE	0xff980000
#define CONFIG_SYS_PMC_BASE_PHYS	CONFIG_SYS_PMC_BASE
#define CONFIG_PMC_BR_PRELIM	(BR_PHYS_ADDR(CONFIG_SYS_PMC_BASE_PHYS) | \
					BR_PS_8 | BR_V)
#define CONFIG_PMC_OR_PRELIM	(OR_AM_64KB | OR_GPCM_CSNT | OR_GPCM_XACS | \
				 OR_GPCM_SCY | OR_GPCM_TRLX | OR_GPCM_EHTR | \
				 OR_GPCM_EAD)

#ifdef CONFIG_NAND_U_BOOT
#define CONFIG_SYS_BR0_PRELIM	CONFIG_NAND_BR_PRELIM	/* NAND Base Address */
#define CONFIG_SYS_OR0_PRELIM	CONFIG_NAND_OR_PRELIM	/* NAND Options */
#define CONFIG_SYS_BR1_PRELIM	CONFIG_FLASH_BR_PRELIM	/* NOR Base Address */
#define CONFIG_SYS_OR1_PRELIM	CONFIG_FLASH_OR_PRELIM	/* NOR Options */
#else
#define CONFIG_SYS_BR0_PRELIM	CONFIG_FLASH_BR_PRELIM	/* NOR Base Address */
#define CONFIG_SYS_OR0_PRELIM	CONFIG_FLASH_OR_PRELIM	/* NOR Options */
#ifdef CONFIG_NAND_FSL_ELBC
#define CONFIG_SYS_BR1_PRELIM	CONFIG_NAND_BR_PRELIM	/* NAND Base Address */
#define CONFIG_SYS_OR1_PRELIM	CONFIG_NAND_OR_PRELIM	/* NAND Options */
#endif
#endif
#if 0
#define CONFIG_SYS_BR3_PRELIM	CONFIG_CPLD_BR_PRELIM	/* CPLD Base Address */
#define CONFIG_SYS_OR3_PRELIM	CONFIG_CPLD_OR_PRELIM	/* CPLD Options */
#endif


/* Vsc7385 switch */
#ifdef CONFIG_VSC7385_ENET
#define CONFIG_SYS_VSC7385_BASE		0xffb00000

#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_VSC7385_BASE_PHYS	0xfffb00000ull
#else
#define CONFIG_SYS_VSC7385_BASE_PHYS	CONFIG_SYS_VSC7385_BASE
#endif

#define CONFIG_SYS_VSC7385_BR_PRELIM	\
	(BR_PHYS_ADDR(CONFIG_SYS_VSC7385_BASE_PHYS) | BR_PS_8 | BR_V)
#define CONFIG_SYS_VSC7385_OR_PRELIM	(OR_AM_128KB | OR_GPCM_CSNT | \
			OR_GPCM_XACS |  OR_GPCM_SCY_15 | OR_GPCM_SETA | \
			OR_GPCM_TRLX |  OR_GPCM_EHTR | OR_GPCM_EAD)

#define CONFIG_SYS_BR2_PRELIM	CONFIG_SYS_VSC7385_BR_PRELIM
#define CONFIG_SYS_OR2_PRELIM	CONFIG_SYS_VSC7385_OR_PRELIM

/* The size of the VSC7385 firmware image */
#define CONFIG_VSC7385_IMAGE_SIZE	8192
#endif

/* Serial Port - controlled on board with jumper J8
 * open - index 2
 * shorted - index 1
 */
#define CONFIG_CONS_INDEX		1
#undef CONFIG_SERIAL_SOFTWARE_FIFO
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#define CONFIG_SYS_NS16550_CLK		get_bus_freq(0)
#ifdef CONFIG_NAND_SPL
#define CONFIG_NS16550_MIN_FUNCTIONS
#endif

#define CONFIG_SYS_BAUDRATE_TABLE	\
	{300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

#define CONFIG_SYS_NS16550_COM1	(CONFIG_SYS_CCSRBAR+0x4500)
#define CONFIG_SYS_NS16550_COM2	(CONFIG_SYS_CCSRBAR+0x4600)

#if 0
/* Use the HUSH parser */
#define CONFIG_SYS_HUSH_PARSER
#ifdef CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 "> "
#endif
#endif

/*
 * Pass open firmware flat tree
 */
#define CONFIG_OF_LIBFDT
#define CONFIG_OF_BOARD_SETUP
#define CONFIG_OF_STDOUT_VIA_ALIAS

#define CONFIG_SYS_64BIT_VSPRINTF
#define CONFIG_SYS_64BIT_STRTOUL
#define CFG_64BIT_VSPRINTF
#define CFG_64BIT_STRTOUL

#if 1
/* new uImage format support */
#define CONFIG_FIT
#define CONFIG_FIT_VERBOSE	/* enable fit_format_{error,warning}() */
#endif

/* I2C */
#define CONFIG_FSL_I2C			/* Use FSL common I2C driver */
#define CONFIG_HARD_I2C			/* I2C with hardware support */
#undef CONFIG_SOFT_I2C			/* I2C bit-banged */
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_I2C_CMD_TREE
#define CONFIG_SYS_I2C_SPEED		400000	/* I2C spd and slave address */
#define CONFIG_SYS_I2C_EEPROM_ADDR	0x52
#define CONFIG_SYS_I2C_SLAVE		0x7F
#define CONFIG_SYS_I2C_NOPROBES		{{0, 0x29}}/* Don't probe these addrs */
#define CONFIG_SYS_I2C_OFFSET		0x3000
#define CONFIG_SYS_I2C2_OFFSET		0x3100
#define CONFIG_SYS_SPD_BUS_NUM		1 /* For displaying rom_loc and flash bank */

#if 0
/*
 * I2C2 EEPROM
 */
#undef CONFIG_ID_EEPROM

#define CONFIG_RTC_PT7C4338
#define CONFIG_SYS_I2C_RTC_ADDR		0x68
#define CONFIG_SYS_I2C_PCA9557_ADDR	0x18

/* enable read and write access to EEPROM */
#define CONFIG_CMD_EEPROM
#define CONFIG_SYS_I2C_MULTI_EEPROMS
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN 1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS 3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS 5
#endif

/* LM75 / AD7416 */
#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS  2
#define CONFIG_DTT_LM75
#define CONFIG_DTT_SENSORS { 0x48, 0x49, 0x4b }
//#define LM75_I2C_CHANNEL 0

#if 0
/*
 * eSPI - Enhanced SPI
 */
#define CONFIG_HARD_SPI
#define CONFIG_FSL_ESPI

#if defined(CONFIG_SPI_FLASH)
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_CMD_SF
#define CONFIG_SF_DEFAULT_SPEED	10000000
#define CONFIG_SF_DEFAULT_MODE	0
#endif
#endif

#if defined(CONFIG_PCI)
/*
 * General PCI
 * Memory space is mapped 1-1, but I/O space must start from 0.
 */

/* controller 2, direct to uli, tgtid 2, Base address 9000 */
#define CONFIG_SYS_PCIE2_NAME		"PCIe2"
#define CONFIG_SYS_PCIE2_MEM_VIRT	0xa0000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE2_MEM_BUS	0xc0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS	0xc20000000ull
#else
#define CONFIG_SYS_PCIE2_MEM_BUS	0xa0000000
#define CONFIG_SYS_PCIE2_MEM_PHYS	0xa0000000
#endif
#define CONFIG_SYS_PCIE2_MEM_SIZE	0x20000000	/* 512M */
#define CONFIG_SYS_PCIE2_IO_VIRT	0xffc10000
#define CONFIG_SYS_PCIE2_IO_BUS		0x00000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE2_IO_PHYS	0xfffc10000ull
#else
#define CONFIG_SYS_PCIE2_IO_PHYS	0xffc10000
#endif
#define CONFIG_SYS_PCIE2_IO_SIZE	0x00010000	/* 64k */

/* controller 1, Slot 2, tgtid 1, Base address a000 */
#define CONFIG_SYS_PCIE1_NAME		"PCIe1"
#define CONFIG_SYS_PCIE1_MEM_VIRT	0x80000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE1_MEM_BUS	0x80000000
#define CONFIG_SYS_PCIE1_MEM_PHYS	0xc00000000ull
#else
#define CONFIG_SYS_PCIE1_MEM_BUS	0x80000000
#define CONFIG_SYS_PCIE1_MEM_PHYS	0x80000000
#endif
#define CONFIG_SYS_PCIE1_MEM_SIZE	0x20000000	/* 512M */
#define CONFIG_SYS_PCIE1_IO_VIRT	0xffc00000
#define CONFIG_SYS_PCIE1_IO_BUS		0x00000000
#ifdef CONFIG_PHYS_64BIT
#define CONFIG_SYS_PCIE1_IO_PHYS	0xfffc00000ull
#else
#define CONFIG_SYS_PCIE1_IO_PHYS	0xffc00000
#endif
#define CONFIG_SYS_PCIE1_IO_SIZE	0x00010000	/* 64k */


/*PCIE video card used*/
#define VIDEO_IO_OFFSET		CONFIG_SYS_PCIE2_IO_VIRT

/* video */
#undef CONFIG_VIDEO
#ifdef CONFIG_VIDEO
#define CONFIG_BIOSEMU
#define CONFIG_CFB_CONSOLE
#define CONFIG_VIDEO_SW_CURSOR
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_ATI_RADEON_FB
#define CONFIG_VIDEO_LOGO
#define CONFIG_SYS_ISA_IO_BASE_ADDRESS	VIDEO_IO_OFFSET
#endif

#define CONFIG_NET_MULTI
#define CONFIG_PCI_PNP	/* do pci plug-and-play */
#undef CONFIG_E1000	/* Defind e1000 pci Ethernet card*/
#define CONFIG_CMD_PCI
#define CONFIG_CMD_NET

#define CONFIG_PCI_SCAN_SHOW	/* show pci devices on startup */
#define CONFIG_DOS_PARTITION
#endif /* CONFIG_PCI */

#if defined(CONFIG_TSEC_ENET)

#ifndef CONFIG_NET_MULTI
#define CONFIG_NET_MULTI
#endif

#define CONFIG_MII		/* MII PHY management */
#if 1 // XXX
#define CONFIG_TSEC1
#define CONFIG_TSEC1_NAME	"eth2"
#endif
#define CONFIG_TSEC2
#define CONFIG_TSEC2_NAME	"eth0"
#define CONFIG_TSEC3
#define CONFIG_TSEC3_NAME	"eth1"

#define TSEC1_PHY_ADDR	3
#define TSEC2_PHY_ADDR	0
#define TSEC3_PHY_ADDR	1

#define TSEC_HIDE_ETH2

#if 1
#define TSEC1_FLAGS	(TSEC_GIGABIT | TSEC_REDUCED)
#endif
#define TSEC2_FLAGS	(TSEC_GIGABIT | TSEC_SGMII)
#define TSEC3_FLAGS	(TSEC_GIGABIT | TSEC_SGMII)

#if 0
#define TSEC1_PHYIDX	0
#define TSEC2_PHYIDX	0
#define TSEC3_PHYIDX	1
#endif

/* per Freescale FAE; use these for SGMII */
#define CONFIG_SYS_TBIANA_SETTINGS ( 0x4001 )

#define CONFIG_ETHPRIME	"eth0"

#define CONFIG_PHY_GIGE	1	/* Include GbE speed/duplex detection */

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#undef CONFIG_HAS_ETH2
#endif /* CONFIG_TSEC_ENET */

#ifdef CONFIG_QE
/* QE microcode/firmware address */
#define CONFIG_SYS_QE_FW_ADDR		0xefec0000
#define CONFIG_SYS_QE_FW_LENGTH		0x10000
#endif /* CONFIG_QE */

#ifdef CONFIG_P1025RDB
/*
 * QE UEC ethernet configuration
 */
#define CONFIG_MIIM_ADDRESS	(CONFIG_SYS_CCSRBAR + 0x82120)

#undef CONFIG_UEC_ETH
#define CONFIG_PHY_MODE_NEED_CHANGE

#define CONFIG_UEC_ETH1	/* ETH1 */
#define CONFIG_HAS_ETH0

#ifdef CONFIG_UEC_ETH1
#define CONFIG_SYS_UEC1_UCC_NUM	0	/* UCC1 */
#define CONFIG_SYS_UEC1_RX_CLK	QE_CLK12 /* CLK12 for MII */
#define CONFIG_SYS_UEC1_TX_CLK	QE_CLK9 /* CLK9 for MII */
#define CONFIG_SYS_UEC1_ETH_TYPE	FAST_ETH
#define CONFIG_SYS_UEC1_PHY_ADDR	0x0	/* 0x0 for MII */
#define CONFIG_SYS_UEC1_INTERFACE_TYPE RMII
#define CONFIG_SYS_UEC1_INTERFACE_SPEED	100
#endif /* CONFIG_UEC_ETH1 */

#define CONFIG_UEC_ETH5	/* ETH5 */
#define CONFIG_HAS_ETH1

#ifdef CONFIG_UEC_ETH5
#define CONFIG_SYS_UEC5_UCC_NUM	4	/* UCC5 */
#define CONFIG_SYS_UEC5_RX_CLK	QE_CLK_NONE
#define CONFIG_SYS_UEC5_TX_CLK	QE_CLK13 /* CLK 13 for RMII */
#define CONFIG_SYS_UEC5_ETH_TYPE	FAST_ETH
#define CONFIG_SYS_UEC5_PHY_ADDR	0x3	/* 0x3 for RMII */
#define CONFIG_SYS_UEC5_INTERFACE_TYPE RMII
#define CONFIG_SYS_UEC5_INTERFACE_SPEED	100
#endif /* CONFIG_UEC_ETH5 */
#endif /* CONFIG_P1025RDB */

/*
 * Environment
 */
#ifdef CONFIG_SYS_RAMBOOT
#ifdef CONFIG_RAMBOOT_SPIFLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SPI_BUS	0
#define CONFIG_ENV_SPI_CS	0
#define CONFIG_ENV_SPI_MAX_HZ	10000000
#define CONFIG_ENV_SPI_MODE	0
#define CONFIG_ENV_SIZE		0x2000	/* 8KB */
#define CONFIG_ENV_OFFSET	0x100000	/* 1MB */
#define CONFIG_ENV_SECT_SIZE	0x10000
#elif defined(CONFIG_RAMBOOT_SDCARD)
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_SIZE		0x2000
#define CONFIG_SYS_MMC_ENV_DEV	0
#elif defined(CONFIG_NAND_U_BOOT)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_OFFSET	((512 * 1024) + CONFIG_SYS_NAND_BLOCK_SIZE)
#define CONFIG_ENV_RANGE	(3 * CONFIG_ENV_SIZE)
#else
#define CONFIG_ENV_IS_NOWHERE	/* Store ENV in memory only */
#define CONFIG_ENV_ADDR		(CONFIG_SYS_MONITOR_BASE - 0x1000)
#define CONFIG_ENV_SIZE		0x2000
#endif
#else
#define CONFIG_ENV_IS_IN_FLASH
#define CFG_ENV_IS_IN_FLASH
#if CONFIG_SYS_MONITOR_BASE > 0xfff80000
#define CONFIG_ENV_ADDR	0xfff80000
#else
#define CONFIG_ENV_ADDR	(CONFIG_SYS_MONITOR_BASE - CONFIG_ENV_SECT_SIZE)
#endif
#define CONFIG_ENV_SIZE		0x20000
#define CONFIG_ENV_SECT_SIZE	0x20000 /* 128K (one sector) */
#endif

#if 0
#define CONFIG_LOADS_ECHO		/* echo on for serial download */
#define CONFIG_SYS_LOADS_BAUD_CHANGE	/* allow baudrate change */
#endif

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_IRQ
#define CONFIG_CMD_PING
#define CONFIG_CMD_I2C
#define CONFIG_CMD_MII
#define CONFIG_CMD_DATE
#define CONFIG_CMD_ELF
#define CONFIG_CMD_SETEXPR
#define CONFIG_CMD_REGINFO
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE

/*
 * USB
 */
#define CONFIG_HAS_FSL_DR_USB
#define USB_PHY_TYPE "ulpi"
#define ENV_HWCONFIG "hwconfig=usb1:dr_mode=host,phy_type=ulpi"

#if defined(CONFIG_HAS_FSL_DR_USB)
#define CONFIG_USB_EHCI

#ifdef CONFIG_USB_EHCI
#define CONFIG_CMD_USB
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_USB_EHCI_FSL
#define CONFIG_USB_STORAGE
#endif
#endif

#undef CONFIG_MMC

#ifdef CONFIG_MMC
#define CONFIG_FSL_ESDHC
#define CONFIG_SYS_FSL_ESDHC_ADDR	CONFIG_SYS_MPC85xx_ESDHC_ADDR
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#endif

#if defined(CONFIG_MMC) || defined(CONFIG_USB_EHCI) \
		 || defined(CONFIG_FSL_SATA)
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#endif

#undef CONFIG_WATCHDOG	/* watchdog disabled */

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP			/* undef to save memory */
#define CONFIG_CMDLINE_EDITING			/* Command-line editing */
#define CONFIG_SYS_LOAD_ADDR	0x2000000	/* default load address */
#define CONFIG_SYS_PROMPT	"=> "		/* Monitor Command Prompt */
#if defined(CONFIG_CMD_KGDB)
#define CONFIG_SYS_CBSIZE	1024		/* Console I/O Buffer Size */
#else
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */
#endif
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE/* Boot Argument Buffer Size */
#define CONFIG_SYS_HZ		1000	/* decrementer freq: 1ms tick */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 * (this used to be 16MB, but was increased by git commit
 * e8b63761554aca641bd9020447d487bfd85111bf)
 */
#define CONFIG_SYS_BOOTMAPSZ	(64 << 20)	/* Initial Memory map for Linux*/
#define CONFIG_SYS_BOOTM_LEN	(64 << 20)	/* Increase max gunzip size */

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

#if 0
/* default location for tftp and bootm */
#define CONFIG_LOADADDR	1000000
#endif

#define CONFIG_BOOTDELAY 2	/* -1 disables auto-boot */
#if 0
#define CONFIG_BOOTARGS	/* the boot command will set bootargs */
#endif

#define CONFIG_BAUDRATE	9600

#ifdef __SW_BOOT_NOR
#define __NOR_RST_CMD	\
norboot=i2c dev 1; i2c mw 18 1 __SW_BOOT_NOR 1; \
i2c mw 18 3 __SW_BOOT_MASK 1; reset
#endif
#ifdef __SW_BOOT_SPI
#define __SPI_RST_CMD	\
spiboot=i2c dev 1; i2c mw 18 1 __SW_BOOT_SPI 1; \
i2c mw 18 3 __SW_BOOT_MASK 1; reset
#endif
#ifdef __SW_BOOT_SD
#define __SD_RST_CMD	\
sdboot=i2c dev 1; i2c mw 18 1 __SW_BOOT_SD 1; \
i2c mw 18 3 __SW_BOOT_MASK 1; reset
#endif
#ifdef __SW_BOOT_NAND
#define __NAND_RST_CMD	\
nandboot=i2c dev 1; i2c mw 18 1 __SW_BOOT_NAND 1; \
i2c mw 18 3 __SW_BOOT_MASK 1; reset
#endif
#ifdef __SW_BOOT_PCIE
#define __PCIE_RST_CMD	\
pciboot=i2c dev 1; i2c mw 18 1 __SW_BOOT_PCIE 1; \
i2c mw 18 3 __SW_BOOT_MASK 1; reset
#endif

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Ardmore\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=e500.ari\0"		        \
	""

#define CFG_MONITOR_BASE CONFIG_SYS_MONITOR_BASE
#define CFG_ENV_SIZE	CONFIG_ENV_SIZE
#define CFG_ENV_ADDR	CONFIG_ENV_ADDR
#define CFG_MAX_FLASH_SECT CONFIG_SYS_MAX_FLASH_SECT
#define CFG_MAX_FLASH_BANKS CONFIG_SYS_MAX_FLASH_BANKS
#define CFG_FLASH_BASE CONFIG_SYS_FLASH_BASE
#define	CFG_FLASH_SIZE (32 * 1024 * 1024)
#define	CFG_MAXARGS	CONFIG_SYS_MAXARGS
#define	CFG_LONGHELP
#define	CFG_MEMTEST_START CONFIG_SYS_MEMTEST_START
#define	CFG_MEMTEST_END CONFIG_SYS_MEMTEST_END
#define CFG_HZ CONFIG_SYS_HZ
#define TOTAL_MALLOC_LEN CONFIG_SYS_MALLOC_LEN
#define NAND_MAX_CHIPS 1
#define CFG_NS16550_COM1 CONFIG_SYS_NS16550_COM1
#define CFG_NS16550_COM2 CONFIG_SYS_NS16550_COM2
#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE	1
#define CFG_NS16550_CLK          get_bus_freq(0)


#define CONFIG_MAX_CPUS         2

#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "
#define CONFIG_APBOOT
#define CFG_LONGHELP
#define	CFG_CBSIZE	1024		/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_I2C_SPEED CONFIG_SYS_I2C_SPEED
#define CFG_I2C_SLAVE CONFIG_SYS_I2C_SLAVE
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/
#define	CFG_BOOTMAPSZ		CONFIG_SYS_BOOTMAPSZ

#define CONFIG_COMMANDS	(  CFG_CMD_DFL \
			 | CFG_CMD_DHCP	\
                         | CFG_CMD_I2C \
                         | CFG_CMD_PCI \
                         | CFG_CMD_NET \
                         | CFG_CMD_PING \
                         | CFG_CMD_BSP	\
			 | CFG_CMD_FLASH	\
			 | CFG_CMD_MEMORY	\
             		 | CFG_CMD_IMI \
			 | CFG_CMD_ENV	\
			 | CFG_CMD_BOOTD	\
			 | CFG_CMD_RUN	\
			 | CFG_CMD_MII	\
			 | CFG_CMD_DTT \
			 )

#include <cmd_confdefs.h>

#define	nvram_manuf_t	std_manuf_t

#define ARUBA_BOARD_TYPE	"Ardmore"
#define __MODEL_STR__		"AP-22x"

#define	APBOOT_IMAGE	0xeff80000
#define	APBOOT_SIZE	(512 * 1024)

#define	AP_PRODUCTION_IMAGE		0xee000000
#define	AP_PRODUCTION_IMAGE_SIZE	(14 * 1024 * 1024)
#define	AP_PROVISIONING_IMAGE		0xeee00000
#define	AP_PROVISIONING_IMAGE_SIZE	(14 * 1024 * 1024)

#define	__OS_IMAGE_TYPE	ARUBA_IMAGE_ARDMORE
#define	__BOOT_IMAGE_TYPE ARUBA_IMAGE_ARDMORE
#define ARUBA_HEADER_SIZE 512

/* reset if no command received in two minutes */
#define CONFIG_BOOT_RETRY_TIME 120 	/* seconds */
#define CONFIG_RESET_TO_RETRY 1

#define CFG_TFTP_STAGING_AREA	(128 * 1024 * 1024)
#define AP_SLOW_FLASH_STAGING_AREA CFG_TFTP_STAGING_AREA

#define	ARUBA_FREESCALE
#define CFG_64BIT_VSPRINTF
#define CONFIG_SHOW_BOOT_PROGRESS

#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800 }

#define	CFG_LOAD_ADDR	CFG_TFTP_STAGING_AREA

#define CONFIG_TIMESTAMP
#define CFG_FLASH_CFI

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT

#define CFG_CONSOLE_INFO_QUIET
#define CONFIG_AUTO_COMPLETE 1

#define CONFIG_AR5416
#define CFG_LATE_BOARD_INIT

#define CFG_MANUF_BASE      0xeff40000
#define CFG_MFG_SIZE	    0x20000
#define __TLV_INVENTORY__	1
#define INVENT_MAX_INVENTORIES  1
#define __TLV_INVENTORY_BUF_SIZE__	1024
#define __CARD_WITH_VARIANT__   2
#define CFG_TPM_CERT 	    0xefec0000

#define CONFIG_NEEDS_MANUAL_RELOC
#define CONFIG_BOOTM_LINUX
#define CONFIG_BZIP2
#define CONFIG_LZMA
#define CONFIG_HW_WATCHDOG
#define CONFIG_SYS_WATCHDOG_FREQ CFG_HZ
#define CONFIG_HW_WATCHDOG_INTERNAL

#define CONFIG_POST (CONFIG_SYS_POST_MEMORY)
#define CONFIG_PHY_MV154x
#define __BOARD_HAS_POWER_INFO__

/* GPIO outputs */
#define GPIO_EXT_WDOG_POKE	1
#define GPIO_RED_STATUS_LED	5
#define GPIO_GREEN_STATUS_LED	6
#define GPIO_PCI_RST		15

/* planned */
#define GPIO_TPM_RST_L		11

/* maybe */
#define GPIO_TPM_CLK            11
#define GPIO_TPM_DATA           12

#define	GPIO_2G_RED_LED		7
#define	GPIO_2G_GREEN_LED	8
#define	GPIO_5G_RED_LED		9
#define	GPIO_5G_GREEN_LED	10

#define GPIO_SMART_ANT_CLOCK	2
#define GPIO_SMART_ANT_ENABLE	3
#define GPIO_SMART_ANT_DATA	4

/* GPIO inputs */
#define	GPIO_POWER_POE_PLUS	12
#define	GPIO_POWER_DC		13
#define	GPIO_POWER_AT		14

#define	GPIO_CONFIG_CLEAR	0

#define ARDMORE_REV_A_PHY_WAR
#define CONFIG_RESET_BUTTON 

#define DISALLOW_USB_BASED_ON_POWER

#define TPM_DIAG

#endif /* __CONFIG_H */
