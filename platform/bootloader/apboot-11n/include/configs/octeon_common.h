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



/* This file sets configuration options that are common to all Octeon
** bootloaders, and is included by each board's config header file. */

#ifndef __OCTEON_COMMON_H__
#define __OCTEON_COMMON_H__

#define CONFIG_MIPS32		1  /* MIPS32 CPU core	*/
#define CONFIG_OCTEON		1

#if !defined(__U_BOOT_HOST__) && !defined(__BYTE_ORDER)
/* Set endian macros for simple exec includes, but not for host utilities */
#define __BYTE_ORDER __BIG_ENDIAN
#endif


/* Used to control conditional compilation for shared code between simple exec and u-boot */
#define CONFIG_OCTEON_U_BOOT

/* let the eth address be writeable */
#define CONFIG_ENV_OVERWRITE 1

#define CONFIG_BOOTDELAY	0	/* autoboot after X seconds	*/

#define CONFIG_BAUDRATE		    115200
#define CONFIG_DOWNLOAD_BAUDRATE    115200

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800 }

#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */
#undef	CONFIG_BOOTARGS


#define CFG_LOAD_ADDR       0x100000
#define CFG_BOOTOCT_ADDR    0x100000


#define CFG_64BIT_VSPRINTF  1
#define CFG_64BIT_STRTOUL   1

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


#define	CFG_LONGHELP			/* undef to save memory      */



#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		64		/* max number of command args*/

#define CFG_MALLOC_LEN		64*1024

#define CFG_BOOTPARAMS_LEN	16*1024


#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */


#define CFG_MEMTEST_START	(CFG_SDRAM_BASE + 0x100000)
#define CFG_MEMTEST_END		(CFG_SDRAM_BASE + 0xffffff)

#define CFG_INIT_SP_OFFSET	0x400000

#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(256 << 10)

#define CFG_FLASH_BASE		PHYS_FLASH_1
#define CFG_FLASH_PROTECT_LEN   0x80000  /* protect low 512K, for both normal and failsafe images */

#define CFG_FLASH_CFI   1
#define CFG_FLASH_CFI_DRIVER   1
#define CONFIG_FLASH_8BIT

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Write */


/* CONFIG_OCTEON_FAILSAFE and CONFIG_RAM_RESIDENT are set in config.h, so
** we can check them here */
#if CONFIG_OCTEON_FAILSAFE || CONFIG_RAM_RESIDENT
#define	CFG_ENV_IS_NOWHERE	1
#else
#define	CFG_ENV_IS_IN_FLASH	1
#endif

#define CONFIG_NET_RETRY_COUNT 5

/*-----------------------------------------------------------------------
 * PCI Configuration
 */
#if defined(CONFIG_PCI)

#define PCI_CONFIG_COMMANDS (CFG_CMD_PCI)

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


#if (CONFIG_OCTEON_PCI_HOST)
#define CONFIG_PCI_PNP
#endif /* CONFIG_OCTEON_PCI_HOST */
#else  /* CONFIG_PCI */
#define PCI_CONFIG_COMMANDS (0)
#endif /* CONFIG_PCI */
/*
 * End (PCI Configuration)
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------
 * U-boot command Configuration
 */

/* Disable networking for failsafe builds.  Failsafe builds
** have size constraint of 192K that must be satisfied. */
#ifdef CONFIG_OCTEON_FAILSAFE
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

#define CONFIG_COMMANDS		((CONFIG_CMD_DFL | CFG_CMD_ELF | CFG_CMD_OCTEON | CFG_CMD_LOADB | CFG_CMD_FLASH \
                                  | CFG_CMD_ENV | CFG_CMD_FLASH | CFG_CMD_IDE | CFG_CMD_FAT | CFG_CMD_RUN \
                                  | EXTRA_INCLUDE_COMMANDS | CFG_CMD_ASKENV)\
                                  & ~(EXTRA_EXCLUDE_COMMANDS | CFG_CMD_FPGA | CFG_CMD_BDI \
                                   | CFG_CMD_BEDBUG | CFG_CMD_BOOTD | CFG_CMD_LOADS | CFG_CMD_NFS))
#include <cmd_confdefs.h>

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
/*
 * End (U-boot command Configuration
 *-----------------------------------------------------------------------*/


#endif  /* __OCTEON_COMMON_H__ */
