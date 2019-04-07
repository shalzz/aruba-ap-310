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

/**
 * @file
 * Simple allocate only memory allocator.  Used to allocate memory at application
 * start time.
 * This file is used by both the bootloader and the simple executive.
 *
 * $Id: cvmx-bootmem-shared.h,v 1.18 2006/04/01 02:11:20 kreese Exp $
 *
 */
#ifndef __CVMX_BOOTMEM_SHARED_H__
#define __CVMX_BOOTMEM_SHARED_H__

#ifdef	__cplusplus
extern "C" {
#endif

#define CVMX_BOOTMEM_NAME_LEN 128   /* Must be multiple of 8, changing breaks ABI */
#define CVMX_BOOTMEM_NUM_NAMED_BLOCKS 64  /* Can change without breaking ABI */

#define CVMX_BOOTMEM_ALIGNMENT_SIZE     (16ull)  /* minimum alignment of bootmem alloced blocks */


/* First bytes of each free physical block of memory contain this structure,
 * which is used to maintain the free memory list.  Since the bootloader is
 * only 32 bits, there is a union providing 64 and 32 bit versions.  The
 * application init code converts addresses to 64 bit addresses before the
 * application starts.
 */
typedef struct 
{
    /* Note: these are referenced from assembly routines in the bootloader, so this structure
    ** should not be changed without changing those routines as well. */
    uint64_t next_block_addr;
    uint64_t size;
 
} cvmx_bootmem_block_header_t;


/* Structure for named memory blocks
** Number of descriptors
** available can be changed without affecting compatiblity,
** but name length changes require a bump in the bootmem
** descriptor version
** Note: This structure must be naturally 64 bit aligned, as a single
** memory image will be used by both 32 and 64 bit programs.
*/
typedef struct
{
    uint64_t base_addr;     /**< Base address of named block */
    uint64_t size;          /**< Size actually allocated for named block (may differ from requested) */
    char name[CVMX_BOOTMEM_NAME_LEN];   /**< name of named block */
} cvmx_bootmem_named_block_desc_t;



/* Current descriptor versions */
#define CVMX_BOOTMEM_DESC_MAJ_VER   3   /* CVMX bootmem descriptor major version */
#define CVMX_BOOTMEM_DESC_MIN_VER   0   /* CVMX bootmem descriptor minor version */

/* First three members of cvmx_bootmem_desc_t are left in original
** positions for backwards compatibility.
*/
typedef struct
{
    uint32_t    lock;       /**< spinlock to control access to list */
    uint32_t    flags;      /**< flags for indicating various conditions */
    uint64_t    head_addr; 

    uint32_t    major_version;  /**< incremented changed when incompatible changes made */
    uint32_t    minor_version;  /**< incremented changed when compatible changes made, reset to zero when major incremented */
    uint64_t    app_data_addr;
    uint64_t    app_data_size;
    
    uint32_t    named_block_num_blocks;  /**< number of elements in named blocks array */
    uint32_t    named_block_name_len;    /**< length of name array in bootmem blocks */
    uint64_t    named_block_array_addr;  /**< address of named memory block descriptors */
 
} cvmx_bootmem_desc_t;



static inline uint64_t octeon_xkphys(uint64_t address)
{
    return address | (1ull<<63);
}

#if !defined(CONFIG_OCTEON_U_BOOT)
static inline void octeon_lock(uint64_t lock_address)
{

    if (sizeof(void*) == 8)
    {
        uint32_t tmp;
        __asm__ __volatile__(
        ".set noreorder             \n"
#ifdef __OCTEON__
        "   syncw                   \n"
#else
        "   sync                    \n"
#endif
        "1: ll   %[tmp], 0(%[addr]) \n"
        "   bnez %[tmp], 1b         \n"
        "   li   %[tmp], 1          \n"
        "   sc   %[tmp], 0(%[addr]) \n"
        "   beqz %[tmp], 1b         \n"
        "   nop                     \n"
        ".set reorder               \n"
        : [tmp] "=&r" (tmp)
        : [addr] "r" (lock_address)
        : "memory");
    }
    else
    {
        uint32_t addrh = lock_address >> 32;
        uint32_t addrl = lock_address;
        uint32_t tmph;
        uint32_t tmpl;
        __asm__ __volatile__(
        ".set noreorder                         \n"
#ifdef __OCTEON__
        "   syncw                   \n"
#else
        "   sync                    \n"
#endif
        "   dsll %[tmpl], %[addrl], 32          \n"
        "   dsll %[tmph], %[addrh], 32          \n"
        "   dsrl %[tmpl], %[tmpl], 32           \n"
        "   or   %[tmph], %[tmph], %[tmpl]      \n"
        "1: ll   %[tmpl], 0(%[tmph])            \n"
        "   bnez %[tmpl], 1b                    \n"
        "   li   %[tmpl], 1                     \n"
        "   sc   %[tmpl], 0(%[tmph])            \n"
        "   beqz %[tmpl], 1b                    \n"
        "   nop                                 \n"
        ".set reorder                           \n"
        : [tmpl] "=&r" (tmpl), [tmph] "=&r" (tmph)
        : [addrh] "r" (addrh), [addrl] "r" (addrl)
        : "memory");
    }
}

static inline void octeon_unlock(uint64_t lock_address)
{

    if (sizeof(void*) == 8)
    {
        __asm__ __volatile__(
        ".set noreorder             \n"
#ifdef __OCTEON__
        "   syncw                   \n"
        "   sw   $0, 0(%[addr])     \n"
        "   syncw                   \n"
#else
        "   sync                    \n"
        "   sw   $0, 0(%[addr])     \n"
        "   sync                    \n"
#endif
        ".set reorder               \n"
        :
        : [addr] "r" (lock_address)
        : "memory");
    }
    else
    {
        uint32_t addrh = lock_address >> 32;
        uint32_t addrl = lock_address;
        uint32_t tmph;
        uint32_t tmpl;
        __asm__ __volatile__(
        ".set noreorder                         \n"
#ifdef __OCTEON__
        "   syncw                   \n"
#else
        "   sync                                \n"
#endif
        "   dsll %[tmpl], %[addrl], 32          \n"
        "   dsll %[tmph], %[addrh], 32          \n"
        "   dsrl %[tmpl], %[tmpl], 32           \n"
        "   or   %[tmph], %[tmph], %[tmpl]      \n"
        "   sw   $0, 0(%[tmph])                 \n"
#ifdef __OCTEON__
        "   syncw                               \n"
#else
        "   sync                                \n"
#endif
        ".set reorder                           \n"
        : [tmph] "=&r" (tmph), [tmpl] "=&r" (tmpl)
        : [addrh] "r" (addrh), [addrl] "r" (addrl)
        : "memory");
    }
}
#endif



/* Prototypes for the functions shared between the bootloader and
** the simple exec.  These must NOT be called directly from applications
*/
uint64_t octeon_phy_mem_block_alloc(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t req_size, uint64_t address_min, uint64_t address_max, uint64_t alignment);

uint64_t octeon_phy_mem_named_block_alloc(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t size, uint64_t min_addr, uint64_t max_addr, uint64_t alignment, char *name);
cvmx_bootmem_named_block_desc_t * octeon_phy_mem_named_block_find(cvmx_bootmem_desc_t *bootmem_desc_ptr, char *name);

void octeon_phy_mem_list_print(cvmx_bootmem_desc_t *bootmem_desc_ptr);
int octeon_phy_mem_block_free(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t phy_addr, uint64_t size);
void octeon_phy_mem_named_block_print(cvmx_bootmem_desc_t *bootmem_desc_ptr);
int octeon_phy_mem_named_block_free(cvmx_bootmem_desc_t *bootmem_desc_ptr, char *name);
uint64_t octeon_phy_mem_list_available_mem(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t min_block_size);

#ifdef	__cplusplus
}
#endif

#endif /* __CVMX_BOOTMEM_SHARED_H__ */
