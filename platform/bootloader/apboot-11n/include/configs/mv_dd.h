/*
 * (C) Copyright 2001
 * Josh Huber <huber@mclx.com>, Mission Critical Linux, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
 
/*
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "../../board/mv_feroceon/mv_dd/mvSysHwConfig.h"

/************/
/* VERSIONS */
/************/
#ifdef MV632X
#define BUILD_TAG  "1.0.2"
#ifdef DUAL_OS_78200
#define CONFIG_IDENT_STRING	" Marvell version: 1.0.2 - Dual CPU"
#else
#define CONFIG_IDENT_STRING	" Marvell version: 1.0.2"
#endif

/* version number passing when loading Kernel */
#define VER_NUM 0x01000200           /* 1.0.2 */
#else
#define BUILD_TAG  "3.3.26"
#ifdef DUAL_OS_78200
#define CONFIG_IDENT_STRING	" Marvell version: 3.3.26 - Dual CPU"
#else
#define CONFIG_IDENT_STRING	" Marvell version: 3.3.26"
#endif

/* version number passing when loading Kernel */
#define VER_NUM 0x03031a00           /* 3.3.26 */
#endif
/* magic word pass when booting Kernel */
#define MV_UBOOT_MAGIC 0xa0b1c2d3

/********************/
/* MV DEV SUPPORTS  */
/********************/	
#define CONFIG_PCI           /* pci support               */

/**********************************/
/* Marvell Monitor Extension      */
/**********************************/
#define enaMonExt() ( /*(!getenv("enaMonExt")) ||\ */ \
		     ( getenv("enaMonExt") && \
                       ((!strcmp(getenv("enaMonExt"),"yes")) ||\
		       (!strcmp(getenv("enaMonExt"),"Yes"))) \
		     )\
		    )
#define enaMP() ( /*(!getenv("enaMP")) ||\ */ \
		     ( getenv("enaMP") && \
                       ((!strcmp(getenv("enaMP"),"yes")) ||\
		       (!strcmp(getenv("enaMP"),"Yes"))) \
		     )\
		    )
/*Dual CPU support*/
#define MASTER_CPU	0
#define SLAVE_CPU	1

/********/
/* CLKs */
/********/
#ifndef __ASSEMBLY__
extern unsigned int mvSysClkGet(void);
extern unsigned int mvTclkGet(void);
extern unsigned int mvMclkGet(void);
#define UBOOT_CNTR		0		/* counter to use for uboot timer */
#define CFG_HZ			1000
#define CFG_TCLK                mvTclkGet()
#define CFG_BUS_HZ              mvSysClkGet()
#define CFG_BUS_CLK             CFG_BUS_HZ
#endif

/********************/
/* PT settings */
/********************/
#define CFG_MV_PT

#ifdef CFG_MV_PT
#define TOTAL_PAGE_TABLE (8<<20) /* 8M */
#else 
#define TOTAL_PAGE_TABLE 0
#endif


/*************************************/
/* High Level Configuration Options  */
/* (easy to change)		     */
/*************************************/
#define CONFIG_MARVELL

/* commands */

#define CONFIG_BOOTP_MASK	(CONFIG_BOOTP_DEFAULT | \
				 CONFIG_BOOTP_BOOTFILESIZE)
#if defined(MV_TINY_IMAGE)
#define CONFIG_COMMANDS	((CONFIG_CMD_DFL \
			 | CFG_CMD_PCI \
			 | CFG_CMD_FLASH\
			 | CFG_CMD_ENV	\
			 | CFG_CMD_NET	\
			 | CFG_CMD_BOOTD \
			 | CFG_CMD_NAND) \
			 & ~CFG_CMD_MISC \
			 & ~CFG_CMD_RCVR)
#elif defined(DB_MV78XX0) || defined(DB_MV88F632X)
#define CONFIG_COMMANDS	((CONFIG_CMD_DFL \
			 | CFG_CMD_I2C \
			 | CFG_CMD_EEPROM \
			 | CFG_CMD_DATE \
			 | CFG_CMD_PCI \
			 | CFG_CMD_NET \
			 | CFG_CMD_PING \
             | CFG_CMD_JFFS2 \
			 | CFG_CMD_BSP \
			 | CFG_CMD_EXT2 \
			 | CFG_CMD_IDE \
			 | CFG_CMD_MISC \
			 | CFG_CMD_NAND \
              | CFG_CMD_USB \
              | CFG_CMD_FAT) \
			 & ~CFG_CMD_RCVR)
#elif defined(RD_MV78XX0_AMC)
#define CONFIG_COMMANDS	((CONFIG_CMD_DFL \
			 | CFG_CMD_I2C \
			 | CFG_CMD_EEPROM \
			 | CFG_CMD_DATE \
			 | CFG_CMD_PCI \
			 | CFG_CMD_NET \
			 | CFG_CMD_PING \
             | CFG_CMD_JFFS2 \
			 | CFG_CMD_BSP \
			 | CFG_CMD_EXT2 \
			 | CFG_CMD_IDE \
			 | CFG_CMD_MISC \
			 | CFG_CMD_NAND) \
			 & ~CFG_CMD_RCVR)
#elif defined(RD_MV78XX0_H3C) || defined(RD_MV78XX0_PCAC)
#define CONFIG_COMMANDS	(((CONFIG_CMD_DFL \
			 | CFG_CMD_I2C \
			 | CFG_CMD_EEPROM \
			 | CFG_CMD_DATE \
			 | CFG_CMD_PCI \
			 | CFG_CMD_NET \
			 | CFG_CMD_PING \
			 | CFG_CMD_BSP \
             | CFG_CMD_EXT2 \
			 | CFG_CMD_IDE) \
			 | CFG_CMD_FLASH) \
			 & ~CFG_CMD_RCVR \
			 & ~CFG_CMD_IMLS \
			 & ~CFG_CMD_NAND)
#elif defined(RD_MV78XX0_MASA)
#define CONFIG_COMMANDS	((CONFIG_CMD_DFL \
			 | CFG_CMD_I2C \
			 | CFG_CMD_EEPROM \
			 | CFG_CMD_DATE \
			 | CFG_CMD_PCI \
			 | CFG_CMD_NET \
			 | CFG_CMD_PING \
			 | CFG_CMD_BSP \
			 | CFG_CMD_EXT2 \
			 | CFG_CMD_IDE \
			 | CFG_CMD_NAND) \
			 & ~CFG_CMD_RCVR \
			 & ~CFG_CMD_IMLS \
			 & ~CFG_CMD_FLASH)
#endif
/* This needs to be added to the defines above if need USB support
#if defined(MV_INCLUDE_USB)
             CFG_CMD_USB 
            CFG_CMD_FAT
#endif
*/
/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define	CFG_MAXARGS	16		/* max number of command args	*/

/* which initialization functions to call for this board */
#define CONFIG_MISC_INIT_F	/* before relloc */
#define CONFIG_MISC_INIT_R      /* after relloc initialization*/
#define CONFIG_BOARD_EARLY_INIT_F /* first c function, will initialize the board */
#define CONFIG_BOARD_EARLY_INIT_R
#define CFG_BOARD_ASM_INIT   1  /* init in asm before moving to c code*/
#undef CONFIG_DISPLAY_MEMMAP    /* at the end of the bootprocess show the memory map*/

#define CONFIG_ENV_OVERWRITE  /*allow to change env parameters */

#undef	CONFIG_WATCHDOG			/* watchdog disabled		*/
#undef	CONFIG_ALTIVEC                  /* undef to disable             */

/* Boot Flags*/
#define	BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH */
#define BOOTFLAG_WARM	0x02		/* Software reboot		    */

/* L2 Cache */
/* see also env parameter enaL2 */
#define CFG_L2
#define L2_INIT 0
#define L2_ENABLE	(L2_INIT | L2CR_L2E)
#define CFG_CACHELINE_SIZE	32	/* For all CPUs */

/* global definetions. */
#define	CFG_SDRAM_BASE		0x00000000
#define CFG_FLASH_BASE		BOOTDEV_CS_BASE
#define CFG_FLASH_SIZE      	BOOTDEV_CS_SIZE
#define CFG_RESET_ADDRESS	0xffff0000

#if defined(MV_TINY_IMAGE)
#define CFG_MONITOR_LEN		(252 << 10)	/* Reserve 252 kB for Monitor */
#elif defined(MV_SEC_128K)
#define	CFG_MONITOR_LEN		(512 << 10)	/* Reserve 512 kB for Monitor */
#elif defined(MV_SEC_256K)
#define	CFG_MONITOR_LEN		(512 << 10)	/* Reserve 512 kB for Monitor */
#elif defined(MV_SEC_64K)
#define	CFG_MONITOR_LEN		(512 << 10)	/* Reserve 512 kB for Monitor */
#endif

#define CFG_MONITOR_BASE	(0xFFFFFFFF - CFG_MONITOR_LEN + 1)

/* change memory map, the U-boot will sit in 7M */
/* the malloc area will be  1M			*/
#define CFG_UBOOT_TOP  		(8 << 20) 	/* 8M */

#define	CFG_MALLOC_LEN		(1 << 20)	/* (default) Reserve 1MB for malloc*/
#define CFG_MALLOC_BASE		(TEXT_BASE + (1 << 20)) /* 7M */

#define CFG_GUNZIP_LEN		(1 << 20)
#undef CONFIG_VERY_BIG_RAM 	

/*
 * When locking data in cache you should point the CFG_INIT_RAM_ADDRESS
 * To an unused memory region. The stack will remain in cache until RAM
 * is initialized 
*/
#undef CFG_INIT_RAM_LOCK
#define CFG_INIT_RAM_ADDR       0x42000000 /* Internal SRAM */

#define CFG_INIT_RAM_END	0x1000
#define CFG_GBL_DATA_SIZE	128  /* size in bytes reserved for init data */
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)

#define isync() __asm__ __volatile__ ("isync")

/********/
/* DRAM */
/********/
/* 	we don't use the global CONFIG_ECC, since in the global ecc we initialize
	the DRAM for ECC in the phase we are relocating to it, which isn't so 
	sufficient.
	so we will define our ECC CONFIG and initilize the DRAM for ECC in the DRAM 
	initialization phase, see sdram_init.c   */
#undef CONFIG_ECC  	 /* enable ECC support */
#define CONFIG_MV_ECC

/* this defines whether we want to use the lowest CAL or the highest CAL available,*/
/* we also check for the env parameter CASset.					  */
#define MV_MIN_CAL

#undef CFG_DISPLAY_DIMM_SPD_CONTENT	/* show SPD content during boot */

/* default values for mtest : 4 ... 12 MB in DRAM */
#define CFG_MEMTEST_START     0x00800000          
#define CFG_MEMTEST_END       0x00C00000         

/********/
/* RTC  */
/********/
#if (CONFIG_COMMANDS & CFG_CMD_DATE)
#define CFG_NVRAM_SIZE  0x00 /* dummy */
#define CFG_NVRAM_BASE_ADDR DEVICE_CS1_BASE /* dummy */
#define CONFIG_RTC_DS1338_DS1339 
#endif

/********************/
/* Serial + parser  */
/********************/
/*
 * The following defines let you select what serial you want to use
 * for your console driver.
 *
 */
#define CONFIG_BAUDRATE         115200   /* console baudrate = 115000    */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400 }

#if defined(RD_MV78XX0_H3C)
#define CFG_DUART_CHAN		1		/* channel 1 to use for console */
#else
#define CFG_DUART_CHAN		0		/* channel 0 to use for console */
#endif
#define CFG_INIT_CHAN1
#define CFG_INIT_CHAN2

#define CONFIG_LOADS_ECHO       0       /* echo off for serial download */
#define CFG_LOADS_BAUD_CHANGE           /* allow baudrate changes       */

#define CFG_CONSOLE_INFO_QUIET  /* don't print In/Out/Err console assignment. */

/* parser */
/* don't chang the parser if you want to load Linux(if you cahnge it to HUSH the cmdline will
	not pass to the kernel correctlly???) */
/*#define CFG_HUSH_PARSER */
#undef CFG_HUSH_PARSER 
#define CONFIG_AUTO_COMPLETE

#define CFG_PROMPT_HUSH_PS2	"> "

#define	CFG_LONGHELP			/* undef to save memory		*/
#define	CFG_PROMPT	"Marvell>> "		/* Monitor Command Prompt	*/
#define	CFG_CBSIZE	1024		/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */

/************/
/* ETHERNET */
/************/
/* to change the default ethernet port, use this define (options: 0, 1, 2) */
#define CONFIG_NET_MULTI
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2

#define ENV_ETH_PRIME			"egiga0"
#if defined(RD_MV78XX0_H3C)
#define YUK_ETHADDR         "00:00:00:EE:51:81"
#undef ENV_ETH_PRIME
#define ENV_ETH_PRIME			"SK98#0"
#endif

#define CONFIG_IPADDR		10.4.50.165
#define CONFIG_SERVERIP		10.4.50.5
#define CONFIG_NETMASK		255.255.255.0

/*-----------------------------------------------------------------------
 * IDE/ATA stuff (Supports IDE harddisk on PCMCIA Adapter)
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_8xx_PCCARD		/* Use IDE with PC Card	Adapter	*/

#undef	CONFIG_IDE_8xx_DIRECT		/* Direct IDE    not supported	*/
#undef	CONFIG_IDE_LED			/* LED   for ide not supported	*/
#undef	CONFIG_IDE_RESET		/* reset for ide not supported	*/

#define CFG_IDE_MAXBUS		4	/* max. 1 IDE bus		*/
#define CFG_IDE_MAXDEVICE	CFG_IDE_MAXBUS * 8	/* max. 1 drive per IDE bus	*/

#define CFG_ATA_IDE0_OFFSET	0x0000

#undef CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION
#define CONFIG_LBA48

/***************************************/
/* LINUX BOOT and other ENV PARAMETERS */
/***************************************/
#define CFG_BOOTARGS_END ":::DB78xx0:eth0:none"
#define RCVR_IP_ADDR "169.254.100.100"
#define	RCVR_LOAD_ADDR	"0x00800000"

#define CONFIG_ZERO_BOOTDELAY_CHECK

#define	CFG_LOAD_ADDR		0x00400000	/* default load address	*/

#undef	CONFIG_BOOTARGS

/* auto boot*/
#define CONFIG_BOOTDELAY	3		/* by default no autoboot */

#if (CONFIG_BOOTDELAY >= 0)
#define CONFIG_ROOTPATH	/mnt/ARM_FS/

#if defined(DUAL_OS_SHARED_MEM_78200)
#define CONFIG_BOOTCOMMAND      "lockSem 1;tftpboot 0x2000000 $(image_name);\
setenv bootargs $(console) $(bootargs_root) nfsroot=$(serverip):$(rootpath) \
cpu0=$(cpu0_res) cpu1=$(cpu1_res) ip=$(ipaddr):$(serverip)$(bootargs_end);  bootm 0x2000000; "
#else
#define CONFIG_BOOTCOMMAND      "tftpboot 0x2000000 $(image_name);\
 setenv bootargs $(console) $(bootargs_root) nfsroot=$(serverip):$(rootpath) \
cpu0=$(cpu0_res) cpu1=$(cpu1_res) ip=$(ipaddr):$(serverip)$(bootargs_end);  bootm 0x2000000; "
#endif                                                                                                             
#define CONFIG_BOOTARGS "console=ttyS0,115200 mtdparts=physmapped-flash.0:32m(root)"
#endif

#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define	CFG_BOOTMAPSZ		(8<<20)	/* Initial Memory map for Linux */

#define BRIDGE_REG_BASE_BOOTM 	0xfbe00000 /* this paramaters are used when booting the linux kernel */ 

/********/
/* USB  */
/********/
#if defined(RD_MV78XX0_AMC) || defined(RD_MV78XX0_H3C)
#define ENV_USB0_MODE	"device"
#define ENV_USB1_MODE	"host"
#define ENV_USB2_MODE	"device"
#elif RD_MV78XX0_MASA
#define ENV_USB0_MODE	"host"
#define ENV_USB1_MODE	"host"
#define ENV_USB2_MODE	"host"
#else
#define ENV_USB0_MODE	"host"
#define ENV_USB1_MODE	"host"
#define ENV_USB2_MODE	"device"
#endif

#if defined(MV_INCLUDE_USB)
#define CONFIG_USB_EHCI
#define CONFIG_USB_STORAGE
#endif
/********/
/* I2C  */
/********/
#define CFG_I2C_EEPROM_ADDR_LEN 1
#define CFG_I2C_MULTI_EEPROMS
#define CFG_I2C_SPEED   100000		/* I2C speed default */

/* I2C addresses for the two DIMM SPD chips */
#define DIMM0_I2C_ADDR	0x56
#define DIMM1_I2C_ADDR	0x54

/* CPU I2C settings */
#define CPU_I2C  
#define I2C_CPU0_EEPROM_ADDR    0x51


/********/
/* PCI  */
/********/
#ifdef CONFIG_PCI
 #define CONFIG_PCI_HOST PCI_HOST_FORCE  /* select pci host function     */
 #define CONFIG_PCI_PNP          /* do pci plug-and-play         */
#ifndef MV_TINY_IMAGE
 #define CONFIG_EEPRO100	 /* Support for Intel 82557/82559/82559ER chips */
#endif
#endif


#if defined(RD_MV78XX0_AMC)
/* Disable PCI-E scan over PCI-E switch */
#define PCI_DIS_INTERFACE 8
#elif defined(RD_MV78XX0_H3C) 
#define PCI_DIS_INTERFACE 3
#elif defined (RD_MV78XX0_MASA) || defined(RD_MV78XX0_PCAC)
#define PCI_DIS_INTERFACE 8
#elif defined (DB_MV78XX0)
#define PCI_DIS_INTERFACE 2
#endif
#define PCI_HOST_ADAPTER 0              /* configure as pci adapter     */
#define PCI_HOST_FORCE  1               /* configure as pci host        */
#define PCI_HOST_AUTO   2               /* detected via arbiter enable  */
#define CFG_PCI_IDSEL 0x30

/*-----------------------------------------------------------------------
 * NAND-FLASH stuff
 *-----------------------------------------------------------------------*/
/* Use the new NAND code. */

#undef CFG_NAND_LEGACY
#define CFG_MAX_NAND_DEVICE     1       /* Max number of NAND devices */
#define NAND_MAX_CHIPS          CFG_MAX_NAND_DEVICE
#define __mem_pci(x) x

/* Boot from NAND settings */
/* Duplicate defines from nBootstrap.h */
#ifdef MV_NAND_BOOT
#define CFG_NAND_BOOT
#define CFG_ENV_IS_IN_NAND	1

#if defined(MV_LARGE_PAGE)
#define CFG_ENV_OFFSET				(128 << 10)	/* environment starts here  */
#undef CFG_ENV_SECT_SIZE
#define CFG_ENV_SECT_SIZE			(128 << 10)	/* environment take 1 block */
#undef CFG_ENV_SIZE		
#define CFG_ENV_SIZE				CFG_ENV_SECT_SIZE /* environment take 1 block */	
#define CFG_NBOOT_BASE				0	
#define CFG_NBOOT_LEN				(4 << 10)	/* Reserved 4KB for boot strap */
#undef	CFG_MONITOR_LEN
#define	CFG_MONITOR_LEN				(640 << 10)	/* Reserve 4 * 128KB + ENV = 640KB for Monitor */
#undef CFG_MONITOR_BASE	
#define CFG_MONITOR_BASE			(CFG_ENV_OFFSET)
#undef	CFG_ENV_IS_IN_FLASH		
#undef CFG_MONITOR_IMAGE_OFFSET		
#define CFG_MONITOR_IMAGE_OFFSET		CFG_ENV_SECT_SIZE /* offset of the monitor from the u-boot image */
#define CFG_MONITOR_IMAGE_DST			TEXT_BASE - CFG_ENV_SECT_SIZE	/* Load NUB to this addr	*/
#undef CFG_ENV_ADDR
#define CFG_ENV_ADDR    			CFG_MONITOR_IMAGE_DST

#else /* ! LARGE PAGE NAND */

#define CFG_ENV_OFFSET				(16 << 10)    /* environment starts here  */
#undef CFG_ENV_SECT_SIZE
#define CFG_ENV_SECT_SIZE			(128 << 10)
#undef CFG_ENV_SIZE		
#define CFG_ENV_SIZE				CFG_ENV_SECT_SIZE /* environment take 1 block */	
#define CFG_NBOOT_BASE				0	
#define CFG_NBOOT_LEN				(4 << 10)	/* Reserved 16kB for boot strap */
#undef	CFG_MONITOR_LEN
#define	CFG_MONITOR_LEN				(640 << 10)	/* Reserve 4 * 128KB + ENV = 640KB for Monitor */
#undef CFG_MONITOR_BASE	
#define CFG_MONITOR_BASE			(CFG_ENV_OFFSET)
#undef	CFG_ENV_IS_IN_FLASH		
#undef CFG_MONITOR_IMAGE_OFFSET		
#define CFG_MONITOR_IMAGE_OFFSET		CFG_ENV_SECT_SIZE /* offset of the monitor from the u-boot image */
#define CFG_MONITOR_IMAGE_DST			TEXT_BASE - CFG_ENV_SECT_SIZE	/* Load NUB to this addr	*/
#undef CFG_ENV_ADDR
#define CFG_ENV_ADDR    			CFG_MONITOR_IMAGE_DST
#endif /* defined(MV_LARGE_PAGE) */
#else  /* NOT BOOT FROM NAND */

#define CFG_MONITOR_IMAGE_OFFSET		CFG_ENV_SECT_SIZE /* offset of the monitor from the u-boot image */
#endif /* MV_NAND_BOOT */

/***************************/
/* CFI FLASH organization  */
/***************************/
#ifndef CFG_NO_FLASH
#define CFG_FLASH_CFI_DRIVER
#define CFG_FLASH_CFI		1
#define CFG_FLASH_USE_BUFFER_WRITE
#define CFG_FLASH_QUIET_TEST
#define CFG_FLASH_BANKS_LIST	{BOOTDEV_CS_BASE}
#if defined(__BE)
#define CFG_WRITE_SWAPPED_DATA
#endif
#endif /* CFG_NO_FLASH */

/***********************/
/* FLASH organization  */
/***********************/

/*
 * When CFG_MAX_FLASH_BANKS_DETECT is defined, the actual number of Flash
 * banks has to be determined at runtime and stored in a gloabl variable
 * mv_board_num_flash_banks. The value of CFG_MAX_FLASH_BANKS_DETECT is only
 * used instead of CFG_MAX_FLASH_BANKS to allocate the array flash_info, and
 * should be made sufficiently large to accomodate the number of banks that
 * might actually be detected.  Since most (all?) Flash related functions use
 * CFG_MAX_FLASH_BANKS as the number of actual banks on the board, it is
 * defined as mv_board_num_flash_banks.
 */
#define CFG_MAX_FLASH_BANKS_DETECT	5
#ifndef __ASSEMBLY__
extern int mv_board_num_flash_banks;
#endif
#define CFG_MAX_FLASH_BANKS (mv_board_num_flash_banks)

#define CFG_MAX_FLASH_SECT	300	/* max number of sectors on one chip */
#define CFG_FLASH_PROTECTION    1

#ifndef MV_NAND_BOOT
#define	CFG_ENV_IS_IN_FLASH	1
#define	CFG_ENV_SIZE		0x1000	/* Total Size of Environment Sector */
#if defined(MV_TINY_IMAGE)
#define CFG_ENV_SECT_SIZE	0x1000
#elif defined(MV_SEC_128K)
#define CFG_ENV_SECT_SIZE	0x20000
#elif defined(MV_SEC_256K)
#define CFG_ENV_SECT_SIZE	0x40000
#elif defined(MV_SEC_64K)
#define CFG_ENV_SECT_SIZE	0x10000
#endif
#define CFG_ENV_OFFSET		0x0
#if defined(MV_TINY_IMAGE)
				/* In tiny image we assume we have room for the env 
 				 * above the reset vector address */
#define CFG_ENV_ADDR    	(0xFFFFFFFF - CFG_ENV_SECT_SIZE + 1) 
#else
#define CFG_ENV_ADDR    	(0xFFFFFFFF - CFG_MONITOR_LEN - CFG_ENV_SECT_SIZE + 1)
#endif
#endif /* MV_NAND_BOOT */

#if (CONFIG_COMMANDS & CFG_CMD_JFFS2)
#define CONFIG_JFFS2_CMDLINE
#if (CONFIG_COMMANDS & CFG_CMD_NAND)
#define CONFIG_JFFS2_NAND
#endif
#endif

/*****************/
/* others        */
/*****************/
#define CFG_INTERNAL_RAM_ADDR	0xf2400000 /* Internal RAM */
#define CFG_DFL_MV_REGS		0xD0000000 /* boot time MV_REGS */
#define CFG_MV_REGS		0xf1000000 /* MV Registers will be mapped here */

/***************************/
/* Relevent for ARM only   */
/***************************/
#if defined(RD_MV78XX0_H3C)
#define CONFIG_SK98 
#endif
#define CONFIG_STACKSIZE	(1 << 20)	/* regular stack - up to 4M (in case of exception)*/
#define CONFIG_NR_DRAM_BANKS 	4 
#undef SYSCLK_AUTO_DETECT
#define CFG_PT_BASE(cpu)  (CFG_MALLOC_BASE - ((cpu+1) * 0x20000))

#define CONFIG_CMDLINE_TAG              1       /* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG		1	/* enable INITRD tag for ramdisk data */
#define CONFIG_SETUP_MEMORY_TAGS        1
#define CONFIG_MARVELL_TAG              1
#define ATAG_MARVELL                    0x41000403

#endif	/* __CONFIG_H */
