/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 * Copyright 2004,2005 Cavium Networks
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
 * This file contains the configuration parameters for the octeon simulator
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1  /* MIPS32 CPU core	*/
#define CONFIG_OCTEON		1
#define CONFIG_OCTEON_38XX	1
/* CONFIG_OCTEON_TRANTOR set by Makefile in include/config.h */


#if !defined(__U_BOOT_HOST__) && !defined(__BYTE_ORDER)
/* Set endian macros for simple exec includes, but not for host utilities */
#define __BYTE_ORDER __BIG_ENDIAN
#endif

/* Defaults to use if bootloader cannot autodetect settings */
#define DEFAULT_ECLK_FREQ_MHZ			400  /* Used if multiplier read fails, and for DRAM refresh rates*/
/* Set this for 533 memory, as this config will work with 400 Mhz memory,
** albeit with poor performance.  This at least prevents a dead board.*/

/* Used to control conditional compilation for shared code between simple
** exec and u-boot */
#define CONFIG_OCTEON_U_BOOT

/* let the eth address be writeable */
#define CONFIG_ENV_OVERWRITE 1

/* Addresses for various things on boot bus.  These addresses
** are used to configure the boot bus mappings. */
#define OCTEON_CF_COMMON_BASE_ADDR  0
#define OCTEON_CF_ATTRIB_BASE_ADDR  0

#define CFG_64BIT_VSPRINTF  1
#define CFG_64BIT_STRTOUL   1


/* Set bootdelay to 0 for immediate boot */
#define CONFIG_BOOTDELAY	0	/* autoboot after X seconds	*/

#define CONFIG_BAUDRATE		115200
#define CONFIG_DOWNLOAD_BAUDRATE		460800

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800 }

#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */
#undef	CONFIG_BOOTARGS


#define CFG_LOAD_ADDR       0x100000
#define CFG_BOOTOCT_ADDR    0x100000
#define CFG_BOOTOCT_LINUX_ADDR    0x1f480000

/* This is the address that the normal bootloader (not the failsafe image)
** will be loaded at.  This may vary from board to board depending on the
** sector size of the flash used.
*/
#define CFG_NORMAL_BOOTLOADER_BASE  0xbfc40000

/*
** Define CONFIG_OCTEON_PCI_HOST = 1 to map the pci devices on the
** bus.  Define CONFIG_OCTEON_PCI_HOST = 0 for target mode when the
** host system performs the pci bus mapping instead.  Note that pci
** commands are enabled to allow access to configuration space for
** both modes.
*/
#ifndef CONFIG_OCTEON_PCI_HOST
#define CONFIG_OCTEON_PCI_HOST	0
#endif
/*
** Define CONFIG_PCI only if the system is known to provide a PCI
** clock to Octeon.  A bus error exception will occur if the inactive
** Octeon PCI core is accessed.  U-boot is not currently configured to
** recover when a exception occurs.
*/
#undef CONFIG_PCI
/*-----------------------------------------------------------------------
 * PCI Configuration
 */
#if defined(CONFIG_PCI)

#define PCI_CONFIG_COMMANDS (CFG_CMD_PCI)



#if (CONFIG_OCTEON_PCI_HOST)
#define CONFIG_PCI_PNP
#endif /* CONFIG_OCTEON_PCI_HOST */
#else  /* CONFIG_PCI */
#define PCI_CONFIG_COMMANDS (0)
#endif /* CONFIG_PCI */

/*
** The Trantor does not ?? use the internal arbiter in Octeon.
** Enable this for boards that do.
*/
/* #define USE_OCTEON_INTERNAL_ARBITER */

/* Define this to enable built-in octeon ethernet support */
#define OCTEON_RGMII_ENET

/* Enable Octeon built-in networking if either SPI or RGMII support is enabled */
#if defined(OCTEON_RGMII_ENET)
#define OCTEON_INTERNAL_ENET
#endif

/* Disable networking for failsafe builds.  Failsafe builds
** have size constraint of 192K that must be satisfied. */
#ifdef CONFIG_OCTEON_FAILSAFE
#undef OCTEON_RGMII_ENET
#undef OCTEON_INTERNAL_ENET
#endif

#if CONFIG_OCTEON_PCI_HOST
#define EXTRA_INCLUDE_COMMANDS   (CFG_CMD_NET | CFG_CMD_DHCP | CFG_CMD_PING | PCI_CONFIG_COMMANDS )
#define EXTRA_EXCLUDE_COMMANDS   (0)
#elif CONFIG_OCTEON_FAILSAFE
#define EXTRA_INCLUDE_COMMANDS   (0)
#define EXTRA_EXCLUDE_COMMANDS   (CFG_CMD_NET | CFG_CMD_DHCP | CFG_CMD_PING | PCI_CONFIG_COMMANDS )
#elif  defined(OCTEON_INTERNAL_ENET)
#define EXTRA_INCLUDE_COMMANDS   (CFG_CMD_NET | CFG_CMD_DHCP | CFG_CMD_MII | CFG_CMD_PING)
#define EXTRA_EXCLUDE_COMMANDS   (0)
#else
#define EXTRA_INCLUDE_COMMANDS   (0)
#define EXTRA_EXCLUDE_COMMANDS   (CFG_CMD_NET | CFG_CMD_DHCP | CFG_CMD_MII | CFG_CMD_PING | PCI_CONFIG_COMMANDS )
#endif


/*-----------------------------------------------------------------------
 * U-boot Commands Configuration
 */
/* Srecord loading seems to be busted - checking for ctrl-c eats bytes */
#define CONFIG_COMMANDS		((CONFIG_CMD_DFL | CFG_CMD_ELF | CFG_CMD_OCTEON | CFG_CMD_LOADB | CFG_CMD_FLASH \
                                  | CFG_CMD_ENV | CFG_CMD_FLASH | CFG_CMD_RUN \
                                  | EXTRA_INCLUDE_COMMANDS | CFG_CMD_ASKENV)\
                                  & ~(EXTRA_EXCLUDE_COMMANDS | CFG_CMD_FPGA | CFG_CMD_BDI \
                                   | CFG_CMD_BEDBUG | CFG_CMD_BOOTD | CFG_CMD_LOADS | CFG_CMD_NFS))
#include <cmd_confdefs.h>


/*-----------------------------------------------------------------------
 * Networking Configuration
 */
#if (CONFIG_COMMANDS & CFG_CMD_NET)
#define CONFIG_NET_MULTI

#ifdef CONFIG_PCI_PNP
/*
** Enable PCI networking devices if PCI is auto configured by u-boot.
*/
#define CONFIG_NATSEMI
#endif /* CONFIG_PCI_PNP */

#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_HOSTNAME	 | \
					CONFIG_BOOTP_BOOTPATH)

#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT
#endif /* CONFIG_COMMANDS & CFG_CMD_NET */


#define CONFIG_NET_RETRY_COUNT 5

#define CONFIG_AUTO_COMPLETE 1
#define CFG_CONSOLE_INFO_QUIET 1

#define CONFIG_DOS_PARTITION 1

#define CFG_IDE_MAXBUS 1
#define CFG_IDE_MAXDEVICE 1

/* Base address of Common memory for Compact flash */
#define CFG_ATA_BASE_ADDR  (OCTEON_CF_COMMON_BASE_ADDR)

/* Offset from base at which data is repeated so that it can be
** read as a block */
#define CFG_ATA_DATA_OFFSET     0x400

/* Not sure what this does, probably offset from base
** of the command registers */
#define CFG_ATA_REG_OFFSET      0

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP			/* undef to save memory      */
#ifdef CONFIG_OCTEON_FAILSAFE
#define	CFG_PROMPT		"Octeon Trantor Failsafe bootloader # "	/* Monitor Command Prompt    */
#else
#if CONFIG_RAM_RESIDENT
#define	CFG_PROMPT		"Octeon Trantor (ram)# "	/* Monitor Command Prompt    */
#else
#define	CFG_PROMPT		"Octeon Trantor # "	/* Monitor Command Prompt    */
#endif
#endif
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		64		/* max number of command args*/

#define CFG_MALLOC_LEN		64*1024

#define CFG_BOOTPARAMS_LEN	16*1024

#define CFG_HZ			500000000ull      /* FIXME causes overflow in net.c */

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */


#define CFG_MEMTEST_START	(CFG_SDRAM_BASE + 0x100000)
#define CFG_MEMTEST_END		(CFG_SDRAM_BASE + 0xffffff)



#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"bootloader_flash_update=protect off 0x1ec40000 0x1ec7ffff;erase 0x1ec40000 0x1ec7ffff;cp.b 0x100000 0x1ec40000 0x40000\0"				\
	"linux_cf=fatload ide 0 21000000 vmlinux.64;bootoctlinux 21000000\0"				\
	"burn_app=erase 1ec80000 +$(filesize);cp.b 100000 1ec80000 $(filesize)\0"				\
	"ls=fatls ide 0\0"				\
	"bf=bootoct 1ec80000 forceboot numcores=$(numcores)\0"				\
	"nuke_env=protect off  0x1fbe0000 0x1fbffff; erase  0x1fbe0000 0x1fbffff\0"				\
	"autoload=n\0"					\
	"ddr_verbose=yes\0"					\
	""

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CFG_FLASH_SIZE	(16*1024*1024)	/* Flash size (bytes) */

#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

/* RBF - doesn't work if we use 0xbfc00000, this is strange */
#define PHYS_FLASH_1		(0x1fc00000 - CFG_FLASH_SIZE) /* Flash Bank #1 */

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(192 << 10)


#define CFG_INIT_SP_OFFSET	0x400000

#define CFG_FLASH_BASE		PHYS_FLASH_1
#define CFG_FLASH_PROTECT_LEN   0x80000  /* protect low 512K */

#define CFG_FLASH_CFI   1
#define CFG_FLASH_CFI_DRIVER   1

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Write */

#if CONFIG_OCTEON_FAILSAFE || CONFIG_RAM_RESIDENT
#define	CFG_ENV_IS_NOWHERE	1
#else
#define	CFG_ENV_IS_IN_FLASH	1
#endif

/* Address and size of Primary Environment Sector	*/
/* The size is a minimum of 8K, but if the flash blocks are larger,
 * we use all of a single flash block. */
#if (CFG_FLASH_SIZE / CFG_MAX_FLASH_SECT) < (8 *1024)
#define CFG_ENV_SIZE		(8*1024)
#else
#define CFG_ENV_SIZE		(CFG_FLASH_SIZE / CFG_MAX_FLASH_SECT)
#endif

/* The last block in the flash is the environment sector */
#define CFG_ENV_ADDR		(0x1fc00000 - CFG_ENV_SIZE)

#define CONFIG_FLASH_8BIT

#define CONFIG_NR_DRAM_BANKS	2

#define CONFIG_MEMSIZE_IN_BYTES


#define OCTEON_CHAR_LED_BASE_ADDR   0


/*-----------------------------------------------------------------------
 * DRAM Module Organization
 *
 * Octeon can be configured to use two pairs of DIMM's, lower and
 * upper, providing a 128/144-bit interface or one to four DIMM's
 * providing a 64/72-bit interface.  This structure contains the TWSI
 * addresses used to access the DIMM's Serial Presence Detect (SPD)
 * EPROMS and it also implies which DIMM socket organization is used
 * on the board.  Software uses this to detect the presence of DIMM's
 * plugged into the sockets, compute the total memory capacity, and
 * configure DRAM controller.  All DIMM's must be identical.
 */

#define DRAM_SOCKET_CONFIGURATION \
	{0x54, 0x56}

/* Board skew in picoseconds */
#define DDR_BOARD_DELAY 5500
#endif	/* __CONFIG_H */
