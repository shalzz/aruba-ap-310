/*
 * (C) Copyright 2004,2005
 * Cavium Networks
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
#define OCTEON_NUM_CORES    16

/* Debugger related defines */
#define DEBUG_STACK_SIZE        1024    /* Size of the debugger stack */
#define DEBUG_NUMREGS           512    /* Total number of (64 bit)registers stored. Must match debug_handler.S */


#define EXCEPTION_BASE_BASE     0          /* must be 4k aligned */
#define EXCEPTION_BASE_INCR     (4*1024)   /* Increment size for exception base addresses (4k minimum) */
#define OCTEON_EXCEPTION_VECTOR_BLOCK_SIZE  (OCTEON_NUM_CORES*EXCEPTION_BASE_INCR) /* 16 4k blocks */

#define BOOTLOADER_DEBUG_REG_SAVE_SIZE  (OCTEON_NUM_CORES * DEBUG_NUMREGS * 8)
#define BOOTLOADER_DEBUG_STACK_SIZE  (OCTEON_NUM_CORES * DEBUG_STACK_SIZE)
#define BOOTLOADER_PCI_READ_BUFFER_SIZE     (256)
#define BOOTLOADER_PCI_WRITE_BUFFER_SIZE     (256)
/* Bootloader debugger stub register save area follows exception vector space */
#define BOOTLOADER_DEBUG_REG_SAVE_BASE  (EXCEPTION_BASE_BASE + OCTEON_EXCEPTION_VECTOR_BLOCK_SIZE)
/* debugger stack follows reg save area */
#define BOOTLOADER_DEBUG_STACK_BASE  (BOOTLOADER_DEBUG_REG_SAVE_BASE + BOOTLOADER_DEBUG_REG_SAVE_SIZE)
/* pci communication blocks.  Read/write are from bootloader perspective*/
#define BOOTLOADER_PCI_READ_BUFFER_BASE   (BOOTLOADER_DEBUG_STACK_BASE + BOOTLOADER_DEBUG_STACK_SIZE)
#define BOOTLOADER_PCI_WRITE_BUFFER_BASE   (BOOTLOADER_PCI_READ_BUFFER_BASE + BOOTLOADER_PCI_READ_BUFFER_SIZE)
#define BOOTLOADER_END_RESERVED_SPACE   (BOOTLOADER_PCI_WRITE_BUFFER_BASE + BOOTLOADER_PCI_WRITE_BUFFER_SIZE)




/* Use the range EXCEPTION_BASE_BASE + 0x800 - 0x1000 (2k-4k) for bootloader private data
** structures that need fixed addresses
*/                                        

#define BOOTLOADER_PRIV_DATA_BASE       (EXCEPTION_BASE_BASE + 0x800)
#define BOOTLOADER_BOOT_VECTOR          (BOOTLOADER_PRIV_DATA_BASE)
#define BOOTLOADER_DEBUG_TRAMPOLINE     (BOOTLOADER_BOOT_VECTOR + BOOT_VECTOR_SIZE)   /* WORD */
#define BOOTLOADER_DEBUG_GP_REG         (BOOTLOADER_DEBUG_TRAMPOLINE + 4)   /* WORD */
#define BOOTLOADER_DEBUG_HANDLER_LIST   (BOOTLOADER_DEBUG_GP_REG + 4)   /* WORD */
#define BOOTLOADER_DEBUG_FLAGS_BASE     (BOOTLOADER_DEBUG_HANDLER_LIST + 4*8)  /* WORD * NUM_CORES */
#define BOOTLOADER_DEBUG_KO_REG_SAVE    (BOOTLOADER_DEBUG_FLAGS_BASE + 4*OCTEON_NUM_CORES)
#define BOOTLOADER_NEXT_AVAIL_ADDR      (BOOTLOADER_DEBUG_KO_REG_SAVE + 4)

/* Debug flag bit definitions in cvmx-bootloader.h */

/* Address used for boot vectors for non-zero cores */
#define BOOT_VECTOR_BASE    (0x80000000 | BOOTLOADER_BOOT_VECTOR)
#define BOOT_VECTOR_NUM_WORDS           (8)
#define BOOT_VECTOR_SIZE                ((OCTEON_NUM_CORES*4)*BOOT_VECTOR_NUM_WORDS)


/* Real physical addresses of memory regions */
#define OCTEON_DDR0_BASE    (0x0ULL)
#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_APBOOT)
#define OCTEON_DDR0_SIZE    (0x004000000ULL)
#else
#define OCTEON_DDR0_SIZE    (0x010000000ULL)
#endif
#define OCTEON_DDR1_BASE    (0x410000000ULL)
#define OCTEON_DDR1_SIZE    (0x010000000ULL)
#define OCTEON_DDR2_BASE    (0x020000000ULL)
#define OCTEON_DDR2_SIZE    (0x3e0000000ULL)
#define OCTEON_MAX_PHY_MEM_SIZE (16*1024*1024*1024ULL)



/* Reserved DRAM for exception vectors, bootinfo structures, bootloader, etc */
#ifdef CONFIG_STAGE1
#define OCTEON_RESERVED_LOW_MEM_SIZE    (5*1024*1024)
#else
#define OCTEON_RESERVED_LOW_MEM_SIZE    (1*1024*1024)
#endif


#define OCTEON_BOOTLOADER_NAMED_BLOCK_TMP_PREFIX "__tmp"
/* DRAM reserved for bootloader use.  This is freed just
** before the application starts, but is used by the bootloader for
** loading ELF images, etc */

#ifdef CONFIG_STAGE1
#define OCTEON_BOOTLOADER_RESERVED_MEM_BASE    (5*1024*1024)
#else
#define OCTEON_BOOTLOADER_RESERVED_MEM_BASE    (1*1024*1024)
#endif
#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_APBOOT)
#define OCTEON_BOOTLOADER_RESERVED_MEM_SIZE    (0*1024*1024)
#else
#define OCTEON_BOOTLOADER_RESERVED_MEM_SIZE    (15*1024*1024)
#endif
#define OCTEON_BOOTLOADER_RESERVED_MEM_NAME    "__tmp_reserved_bootloader"
/* DRAM section reserved for the Linux kernel (we want this to be contiguous with
** the bootloader reserved area to that they can be merged on free.
*/
#if defined(CONFIG_ARUBA_OCTEON) || defined(CONFIG_APBOOT)
#ifdef CONFIG_STAGE1
// stage 1 can't boot Linux for now
#define OCTEON_LINUX_RESERVED_MEM_BASE    (0*1024*1024)
#define OCTEON_LINUX_RESERVED_MEM_SIZE    (0*1024*1024)
#else
#define OCTEON_LINUX_RESERVED_MEM_BASE    (2*1024*1024)
#define OCTEON_LINUX_RESERVED_MEM_SIZE    (16*1024*1024)
#endif
#else
#define OCTEON_LINUX_RESERVED_MEM_BASE    (16*1024*1024)
#define OCTEON_LINUX_RESERVED_MEM_SIZE    (128*1024*1024)
#endif
#define OCTEON_LINUX_RESERVED_MEM_NAME    "__tmp_reserved_linux"


#define OCTEON_ENV_MEM_NAME    "__bootloader_env"
