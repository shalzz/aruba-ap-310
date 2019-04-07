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
/* CONFIG_OCTEON_EBT3000 set by Makefile in include/config.h */ 

#if 0 /* kihbingo */
/* Memory defines are separated out for sharing with pci host utilities */
#include "octeon_ebt3000_shared.h"
#endif

#if !defined(__U_BOOT_HOST__) && !defined(__BYTE_ORDER)
/* Set endian macros for simple exec includes, but not for host utilities */
#define __BYTE_ORDER __BIG_ENDIAN
#endif

/* Defaults to use if bootloader cannot autodetect settings */
#define DEFAULT_ECLK_FREQ_MHZ			400  /* Used if multiplier read fails, and for DRAM refresh rates*/
/* Set this for 533 memory, as this config will work with 400 Mhz memory,
** albeit with poor performance.  This at least prevents a dead board.*/
#define EBT3000_REV1_DEF_DRAM_FREQ  266

#define CONFIG_OCTEON_LLM_WORKAROUND
#define CONFIG_OCTEON_PCIX_WORKAROUND

/* Used to control conditional compilation for shared code between simple
** exec and u-boot */
#define CONFIG_OCTEON_U_BOOT


/* let the eth address be writeable */
#define CONFIG_ENV_OVERWRITE 1


/* bootloader usable memory size in bytes for ebt3000, hard code until autodetect */
#define OCTEON_MEM_SIZE (1*1024*1024*1024ULL)


/* Addresses for various things on boot bus.  These addresses
** are used to configure the boot bus mappings. */


#define CONFIG_BOOT_BUS_BOARD 1  /* Use board specific boot bus init */
#define NS0216_FLASH1_BASE_ADDR     0xB0000000 /* 8bit, 64MB */
#define NS0216_FLASH2_BASE_ADDR     0xB4000000 /* 8bit, 64MB */
#define NS0216_SPIMAC1_BASE_ADDR    0xB8000000 /* 16bit */
#define NS0216_SPIMAC2_BASE_ADDR    0xB8100000 /* 16Bit */
#define NS0216_DEBMAC1_BASE_ADDR    0xB8200000 /* 16Bit */
#define NS0216_DEBMAC2_BASE_ADDR    0xB8300000 /* 16Bit */
#define NS0216_RTC_BASE_ADDR        0xB8400000 /* 8bit */



#define CFG_64BIT_VSPRINTF  1
#define CFG_64BIT_STRTOUL   1



/* Set bootdelay to 0 for immediate boot */
#define CONFIG_BOOTDELAY	0	/* autoboot after X seconds	*/

#define CONFIG_BAUDRATE		115200
#define CONFIG_DOWNLOAD_BAUDRATE		115200

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800 }

#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */
#undef	CONFIG_BOOTARGS


#define CFG_LOAD_ADDR       0x100000
#define CFG_BOOTOCT_ADDR    0x100000

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



/*-----------------------------------------------------------------------
 * U-boot Commands Configuration
 */
#define CONFIG_COMMANDS (\
                CFG_CMD_OCTEON | CFG_CMD_ELF | CFG_CMD_LOADB | CFG_CMD_FLASH | \
                CFG_CMD_NET | CFG_CMD_PING | CFG_CMD_DHCP |\
                CFG_CMD_MEMORY | \
                CFG_CMD_ENV | CFG_CMD_RUN | CFG_CMD_ASKENV)
#include <cmd_confdefs.h>




#define CONFIG_NET_RETRY_COUNT 5

#define CONFIG_AUTO_COMPLETE 1
#define CFG_CONSOLE_INFO_QUIET 1


/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP			/* undef to save memory      */
#define U_BOOT_BUILD_NUMBER_STR "- 2.0"
#define CFG_PROMPT    "lanbird# " /* Monitor Command Prompt    */


#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		64		/* max number of command args*/

#define CFG_MALLOC_LEN		64*1024

#define CFG_BOOTPARAMS_LEN	16*1024

#define CFG_HZ			500000000ull      /* FIXME causes overflow in net.c */

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */


#define CFG_MEMTEST_START	(CFG_SDRAM_BASE + 0x100000)
#define CFG_MEMTEST_END		(CFG_SDRAM_BASE + 0xffffff)



#define CONFIG_EXTRA_ENV_SETTINGS         \
  "clearenv=protect off 2:511;erase 2:511;protect on 2:511\0" \
  "linux_save=tftp 21000000 vmlinuz;erase b0000000 b04fffff;cp.b 21000000 b0000000 500000\0" \
  "linux=cp.b b0000000 21000000 500000;bootoctlinux 21000000 numcores=16\0" \
  "linux_smallfs=cp.b b0000000 21000000 500000;bootoctlinux 21000000 numcores=16 endbootargs smallfs\0" \
  ""

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_FLASH_SIZE	(128*1024*1024)	/* Flash size (bytes) */
#define CFG_MAX_FLASH_BANKS 2 /* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(512)	/* max number of sectors on one chip */
#define CFG_FLASH_BASE1   NS0216_FLASH1_BASE_ADDR
#define CFG_FLASH_BASE2   NS0216_FLASH2_BASE_ADDR
#define CFG_FLASH_BASE    CFG_FLASH_BASE1
#define CFG_FLASH_BANKS_LIST { CFG_FLASH_BASE1, CFG_FLASH_BASE2 }
#define CFG_NO_BOOT_BUS_INIT   /* Don't to standard boot bus init */

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(192 << 10)

#define CFG_INIT_SP_OFFSET	0x400000


#define CFG_FLASH_CFI   1
#define CFG_FLASH_CFI_DRIVER   1

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Write */

#define CFG_ENV_IS_IN_FLASH 1

/* Address and size of Primary Environment Sector	*/
#define CFG_ENV_SIZE            (128*1024)
#define CFG_ENV_ADDR		(CFG_FLASH_BASE + CFG_FLASH_SIZE - CFG_ENV_SIZE)

#define CONFIG_FLASH_8BIT

#define CONFIG_NR_DRAM_BANKS	2

#define CONFIG_MEMSIZE_IN_BYTES

#if 1 /* kihbingo */
#define CONFIG_DRIVER_CS8900  1
#define CS8900_BUS16          1
#define CS8900_BASE           NS0216_DEBMAC1_BASE_ADDR
#endif

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		( 8 * 1024)
#define CFG_ICACHE_SIZE		(32 * 1024)
#define CFG_CACHELINE_SIZE	128

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

#define DRAM_SOCKET_CONFIGURATION { 0x50, 0x52 }


/* Board delay in picoseconds */
#define DDR_BOARD_DELAY		4815

#endif	/* __CONFIG_H */
