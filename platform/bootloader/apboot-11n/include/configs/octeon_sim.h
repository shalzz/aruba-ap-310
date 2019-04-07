/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
/*************************************************************************
Copyright (c) 2004-2005 Cavium Networks (support@cavium.com). All rights
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
/*
 * This file contains the configuration parameters for the octeon simulator
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1  /* MIPS32 CPU core	*/
#define CONFIG_OCTEON		1
#define CONFIG_OCTEON_38XX	1
/* CONFIG_OCTEON_SIM set by Makefile in include/config.h */ 

#ifndef __U_BOOT_HOST__
/* Set endian macros for simple exec includes, but not for host utilities */
#define __BYTE_ORDER __BIG_ENDIAN
#endif

#define DEFAULT_ECLK_FREQ_MHZ			600
#define DCLK_FREQ_MHZ			667	/* 1/2 Data Rate */


/* Used to control conditional compilation for shared code between simple
** exec and u-boot, used for bootmem allocator in cvmx-bootmem-shared.c */
#define CONFIG_OCTEON_U_BOOT

/* Set this to disable all accesses to the boot bus in the bootloader */
/* You also need to change the TEXT_BASE defined board/octeon_sim/config.mk
** and rebuild all files to load the bootloader at a different address.
** usually set during config by top level makefile */
/* #define CONFIG_OCTEON_NO_BOOT_BUS  1 */

#if CONFIG_OCTEON_NO_BOOT_BUS
/* Hard code memory size if no boot bus accesses */
#define OCTEON_MEM_SIZE (128*1024*1024)
#endif


#define CFG_64BIT_VSPRINTF  1
#define CFG_64BIT_STRTOUL   1
#define CFG_CONSOLE_INFO_QUIET 1

#define CONFIG_ETHADDR		DE:AD:BE:EF:01:01    /* Ethernet address */

/* Set bootdelay to 0 for immediate boot */
#define CONFIG_BOOTDELAY	0	/* autoboot after X seconds	*/

#define CONFIG_BAUDRATE		115200
#define CONFIG_DOWNLOAD_BAUDRATE		460800

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800 }

#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */
#undef	CONFIG_BOOTARGS

#define	CONFIG_EXTRA_ENV_SETTINGS					\
	"addmisc=setenv bootargs $(bootargs) "				\
		"console=ttyS0,$(baudrate) "				\
		"panic=1\0"						\
	"bootfile=/tftpboot/vmlinux.srec\0"				\
	"load=tftp 80500000 $(u-boot)\0"				\
	""

/* Set default boot command */
#if (!CONFIG_OCTEON_NO_BOOT_BUS)
#define CONFIG_BOOTCOMMAND	"bootoct 0x10000000"
#else
/* Expect ELF image at 0x4000000 in DRAM if not using boot bus */
#define CONFIG_BOOTCOMMAND	"bootoct 0x4000000"
#endif

#define OCTEON_INTERNAL_ENET
#define CONFIG_NET_MULTI
#define OCTEON_RGMII_ENET

#define CFG_BOOTOCT_ADDR    0x10000000

/* Srecord loading seems to be busted - checking for ctrl-c eats bytes */
#define CONFIG_COMMANDS		((CONFIG_CMD_DFL | \
				  CFG_CMD_ELF | CFG_CMD_OCTEON | CFG_CMD_NET | CFG_CMD_DHCP | CFG_CMD_PING) & \
 ~(CFG_CMD_ENV | CFG_CMD_FAT | CFG_CMD_FLASH | CFG_CMD_FPGA | \
   CFG_CMD_RUN | \
   CFG_CMD_BDI | CFG_CMD_BEDBUG | CFG_CMD_NFS | CFG_CMD_LOADB | CFG_CMD_BOOTD | CFG_CMD_LOADS))
#include <cmd_confdefs.h>

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory      */
#define	CFG_PROMPT		"Octeon sim # "	/* Monitor Command Prompt    */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		64		/* max number of command args*/

#define CFG_MALLOC_LEN		64*1024

#define CFG_BOOTPARAMS_LEN	16*1024

#define CFG_HZ			600000000ull      /* FIXME causes overflow in net.c */

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */

#define	CFG_LOAD_ADDR		0x10000000     /* default load address	*/

#define CFG_MEMTEST_START	0x80100000
#define CFG_MEMTEST_END		0x80800000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xbfc00000 /* Flash Bank #1 */
#define CFG_FLASH_BASE		0xbfc00000

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	TEXT_BASE
#define	CFG_MONITOR_LEN		(192 << 10)

#define CFG_INIT_SP_OFFSET	0x400000


/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2 * CFG_HZ) /* Timeout for Flash Write */

#define	CFG_ENV_IS_NOWHERE	1

/* Address and size of Primary Environment Sector	*/
#define CFG_ENV_ADDR		0xB0030000
#define CFG_ENV_SIZE		(4*1024)

#define CONFIG_FLASH_8BIT

#define CONFIG_NR_DRAM_BANKS	2


#define CONFIG_MEMSIZE_IN_BYTES

#ifndef CONFIG_OCTEON_PCI_HOST
#define CONFIG_OCTEON_PCI_HOST	0
#endif

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		( 8 * 1024)
#define CFG_ICACHE_SIZE		(32 * 1024)
#define CFG_CACHELINE_SIZE	128


#endif	/* __CONFIG_H */
