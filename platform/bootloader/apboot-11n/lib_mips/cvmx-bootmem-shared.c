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
 * Simple allocate only memory allocator.  Used by the bootloader and applications
 *
 * Note: Simple executive applications should only use the cvmx_bootmem_* functions,
 * as the functions in this file are not meant to be used directly.
 *
 * $Id: cvmx-bootmem-shared.c,v 1.38 2006/12/15 00:04:39 rfranz Exp $
 *
 */


/* Note:  This file is also #included by the bootloader, as the memory alloc routine is
shared between programs.
*/

#if !defined(CONFIG_OCTEON_U_BOOT) && !defined(__KERNEL__)
#include "cvmx-config.h"
#include <cvmx.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#ifndef cvmx_dprintf
#define cvmx_dprintf printf
#endif

#include "cvmx-bootmem-shared.h"

#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define ALIGN_ADDR_UP(addr, align)     (((addr) + (~(align))) & (align))


void octeon_write64(uint64_t addr, uint64_t val);
uint64_t octeon_read64(uint64_t addr);
void octeon_phy_mem_set_size(uint64_t addr, uint64_t size);
void octeon_phy_mem_set_next(uint64_t addr, uint64_t next);
uint64_t octeon_phy_mem_get_size(uint64_t addr);
uint64_t octeon_phy_mem_get_next(uint64_t addr);


/**
 * Allocates a block of physical memory from the free list, at (optional) requested address and alignment.
 *
 * @param bootmem_desc_ptr   Pointer to the CVMX bootmem descriptor.
 * @param req_size      size of region to allocate.  All requests are rounded up to 8 byte size
 * @param address_min   Minimum address that block can occupy.
 * @param address_max   Specifies the maximum address_min (inclusive) that the allocation can use.
 * @param alignment Requested alignment of the block.  If this alignment cannot be met, the allocation fails.
 *                  This must be a power of 2.
 *                  (Note: Alignment of 8 bytes is required, and internally enforced.  Requested alignments of
 *                  less than 8 are set to 8.)
 *
 * @return address_min of block allocated, or 0 on failure
 */
uint64_t octeon_phy_mem_block_alloc(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t req_size, uint64_t address_min, uint64_t address_max, uint64_t alignment)
{

    uint64_t head_addr = bootmem_desc_ptr->head_addr;
    uint64_t ent_addr;
    uint64_t prev_addr = 0;  /* points to previous list entry, NULL current entry is head of list */
    uint64_t new_ent_addr = 0;
    uint64_t desired_min_addr;
    uint64_t alignment_mask = ~(alignment - 1);

#ifdef DEBUG
    cvmx_dprintf("block alloc called: req_size: 0x%Lx, min_addr: 0x%Lx, max_addr: 0x%Lx, align: 0x%Lx\n",
           (unsigned long long)req_size, (unsigned long long)address_min, (unsigned long long)address_max, (unsigned long long)alignment);
#endif

    if (bootmem_desc_ptr->major_version > 3)
    {
        cvmx_dprintf("ERROR: Incompatible bootmem descriptor version: %d.%d\n", (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
        return(0);
    }

    /* Do a variety of checks to validate the arguments.  The allocator code will later assume
    ** that these checks have been made.  We validate that the requested constraints are not
    ** self-contradictory before we look through the list of available memory
    */

    /* 0 is not a valid req_size for this allocator */
    if (!req_size)
        return(0);

    /* Round req_size up to mult of minimum alignment bytes */
    req_size = (req_size + (CVMX_BOOTMEM_ALIGNMENT_SIZE - 1)) & ~(CVMX_BOOTMEM_ALIGNMENT_SIZE - 1);

    /* Convert !0 address_min and 0 address_max to special case of range that specifies an exact
    ** memory block to allocate.  Do this before other checks and adjustments so that this tranformation will be validated */
    if (address_min && !address_max)
        address_max = address_min + req_size;
    else if (!address_min && !address_max)
        address_max = ~0ull;   /* If no limits given, use max limits */


#if defined(__linux__) && defined(CVMX_ABI_N32) && !defined(CONFIG_OCTEON_U_BOOT)
    extern uint64_t linux_mem32_min;
    extern uint64_t linux_mem32_max;
    cvmx_dprintf("min: 0x%llx, max: 0x%llx\n", linux_mem32_min, linux_mem32_max);
    /* For 32 bit Linux apps, we need to restrict the allocations to the range
    ** of memory configured for access from userspace.  Also, we need to add mappings
    ** for the data structures that we access.*/

    /* Reject specific location requests that are not fully within bounds */
    if (req_size == address_max - address_min && ((address_min > linux_mem32_max || address_min < linux_mem32_min)))
        return(0);

    /* Narrow range requests to be bounded by the 32 bit limits.  octeon_phy_mem_block_alloc()
    ** will reject inconsistent req_size/range requests, so we don't repeat those checks here.
    ** If max unspecified, set to 32 bit maximum. */
    address_min = MIN(MAX(address_min, linux_mem32_min), linux_mem32_max);
    if (!address_max)
        address_max = linux_mem32_max;
    else
        address_max = MAX(MIN(address_max, linux_mem32_max), linux_mem32_min);
#endif


    /* Verify that requested minimum address meets minimum alignment requirements */
    if (address_min & (CVMX_BOOTMEM_ALIGNMENT_SIZE - 1))
        return(0);

    /* Adjust address minimum based on requested alignment (round up to meet alignment).  Do this here so we can
    ** reject impossible requests up front. (NOP for address_min == 0) */
    if (alignment)
        address_min = (address_min + (alignment - 1)) & ~(alignment - 1);


    /* Reject inconsistent args.  We have adjusted these, so this may fail due to our internal changes
    ** even if this check would pass for the values the user supplied. */
    if (req_size > address_max - address_min)
        return(0);


    /* Enforce minimum alignment (this also keeps the minimum free block
    ** req_size the same as the alignment req_size */
    if (alignment < CVMX_BOOTMEM_ALIGNMENT_SIZE)
    {
        alignment = CVMX_BOOTMEM_ALIGNMENT_SIZE;
    }
    alignment_mask = ~(alignment - 1);


    /* Walk through the list entries - first fit found is returned */
    ent_addr = head_addr;
    while (ent_addr)
    {
        uint64_t usable_base, usable_max;
        uint64_t ent_size = octeon_phy_mem_get_size(ent_addr);

        if (octeon_phy_mem_get_next(ent_addr) && ent_addr > octeon_phy_mem_get_next(ent_addr))
        {
            cvmx_dprintf("Internal bootmem_alloc() error: ent: 0x%llx, next: 0x%llx\n",
                   (unsigned long long)ent_addr, (unsigned long long)octeon_phy_mem_get_next(ent_addr));
            return(0);
        }

        /* Determine if this is an entry that can satisify the request */
        /* Check to make sure entry is large enough to satisfy request */
        usable_base = ALIGN_ADDR_UP(MAX(address_min, ent_addr), alignment_mask);
        usable_max = MIN(address_max, ent_addr + ent_size);
        /* We should be able to allocate block at address usable_base */

        desired_min_addr = usable_base;

        /* Determine if request can be satisfied from the current entry */
        if ((((ent_addr + ent_size) > usable_base && ent_addr < address_max))
            && req_size <= usable_max - usable_base)
        {
            /* We have found an entry for the request, so allocate from the entry as
            ** required.
            */

            /* Match at start of entry */
            if (desired_min_addr == ent_addr)
            {
                if (req_size < ent_size)
                {
                    /* big enough to create a new block from top portion of block */
                    new_ent_addr = ent_addr + req_size;
                    octeon_phy_mem_set_next(new_ent_addr, octeon_phy_mem_get_next(ent_addr));
                    octeon_phy_mem_set_size(new_ent_addr, ent_size - req_size);

                    /* Adjust next pointer as following code uses this */
                    octeon_phy_mem_set_next(ent_addr, new_ent_addr);
                }

                /* adjust prev ptr or head to remove this entry from list */
                if (prev_addr)
                {
                    octeon_phy_mem_set_next(prev_addr, octeon_phy_mem_get_next(ent_addr));
                }
                else
                {
                    /* head of list being returned, so update head ptr */
                    bootmem_desc_ptr->head_addr = octeon_phy_mem_get_next(ent_addr);
                }
                return(desired_min_addr);
            }


            /* block returned doesn't start at beginning of entry, so we know
            ** that we will be splitting a block off the front of this one.  Create a new block
            ** from the beginning, add to list, and go to top of loop again.
            **
            ** create new block from high portion of block, so that top block
            ** starts at desired addr
            **/
            new_ent_addr = desired_min_addr;
            octeon_phy_mem_set_next(new_ent_addr, octeon_phy_mem_get_next(ent_addr));
            octeon_phy_mem_set_size(new_ent_addr, octeon_phy_mem_get_size(ent_addr) - (desired_min_addr - ent_addr));
            octeon_phy_mem_set_size(ent_addr, desired_min_addr - ent_addr);
            octeon_phy_mem_set_next(ent_addr, new_ent_addr);
            /* Loop again to handle actual alloc from new block */
        }

        prev_addr = ent_addr;
        ent_addr = octeon_phy_mem_get_next(ent_addr);
    }

    /* We didn't find anything, so return error */
    return(0);
}


/**
 * Frees a block to the bootmem allocator list.  This must
 * be used with care, as the size provided must match the size
 * of the block that was allocated, or the list will become
 * corrupted.
 * NOTE:  This allocator is not intended to be a generic replacement
 * for malloc.
 * @param bootmem_desc_ptr pointer to the bootmem descriptor block
 * @param phy_addr   physical address of block
 * @param size   size of block in bytes.
 *
 * @return 1 on success,
 *         0 on failure
 */
int octeon_phy_mem_block_free(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t phy_addr, uint64_t size)
{
    uint64_t cur_addr = bootmem_desc_ptr->head_addr;
    uint64_t prev_addr = 0;  /* zero is invalid */

#ifdef DEBUG
    cvmx_dprintf("octeon_phy_mem_block_free addr: 0x%Lx, size: 0x%Lx\n", phy_addr, size);
#endif
    if (bootmem_desc_ptr->major_version > 3)
    {
        cvmx_dprintf("ERROR: Incompatible bootmem descriptor version: %d.%d\n", (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
        return(0);
    }

    /* 0 is not a valid size for this allocator */
    if (!size)
        return(0);


    if (cur_addr == 0 || phy_addr < cur_addr)
    {
        /* add at front of list - special case with changing head ptr */
        if (cur_addr && phy_addr + size > cur_addr)
            return(0); /* error, overlapping section */
        else if (phy_addr + size == cur_addr)
        {
            /* Add to front of existing first block */
            octeon_phy_mem_set_next(phy_addr, octeon_phy_mem_get_next(cur_addr));
            octeon_phy_mem_set_size(phy_addr, octeon_phy_mem_get_size(cur_addr) + size);
            bootmem_desc_ptr->head_addr = phy_addr;

        }
        else
        {
            /* New block before first block */
            octeon_phy_mem_set_next(phy_addr, cur_addr);  /* OK if cur_addr is 0 */
            octeon_phy_mem_set_size(phy_addr, size);
            bootmem_desc_ptr->head_addr = phy_addr;
        }
        return(1);
    }

    /* Find place in list to add block */
    while (cur_addr && phy_addr > cur_addr)
    {
        prev_addr = cur_addr;
        cur_addr = octeon_phy_mem_get_next(cur_addr);
    }

    if (!cur_addr)
    {
        /* We have reached the end of the list, add on to end, checking
        ** to see if we need to combine with last block
        **/
        if (prev_addr +  octeon_phy_mem_get_size(prev_addr) == phy_addr)
        {
            octeon_phy_mem_set_size(prev_addr, octeon_phy_mem_get_size(prev_addr) + size);
        }
        else
        {
            octeon_phy_mem_set_next(prev_addr, phy_addr);
            octeon_phy_mem_set_size(phy_addr, size);
            octeon_phy_mem_set_next(phy_addr, 0);
        }
        return(1);
    }
    else
    {
        /* insert between prev and cur nodes, checking for merge with either/both */

        if (prev_addr +  octeon_phy_mem_get_size(prev_addr) == phy_addr)
        {
            /* Merge with previous */
            octeon_phy_mem_set_size(prev_addr, octeon_phy_mem_get_size(prev_addr) + size);
            if (phy_addr + size == cur_addr)
            {
                /* Also merge with current */
                octeon_phy_mem_set_size(prev_addr, octeon_phy_mem_get_size(cur_addr) + octeon_phy_mem_get_size(prev_addr));
                octeon_phy_mem_set_next(prev_addr, octeon_phy_mem_get_next(cur_addr));
            }
            return(1);
        }
        else if (phy_addr + size == cur_addr)
        {
            /* Merge with current */
            octeon_phy_mem_set_size(phy_addr, octeon_phy_mem_get_size(cur_addr) + size);
            octeon_phy_mem_set_next(phy_addr, octeon_phy_mem_get_next(cur_addr));
            octeon_phy_mem_set_next(prev_addr, phy_addr);
            return(1);
        }

        /* It is a standalone block, add in between prev and cur */
        octeon_phy_mem_set_size(phy_addr, size);
        octeon_phy_mem_set_next(phy_addr, cur_addr);
        octeon_phy_mem_set_next(prev_addr, phy_addr);


    }
    return(1);

}

/**
 * Prints the list of available memory.
 *
 * @param bootmem_desc_ptr
 *               pointer to bootmem descriptor block
 */
void octeon_phy_mem_list_print(cvmx_bootmem_desc_t *bootmem_desc_ptr)
{
    uint64_t addr;

    addr = bootmem_desc_ptr->head_addr;
    printf("\n\n\nPrinting bootmem block list, descriptor: %p,  head is 0x%Lx\n",
           bootmem_desc_ptr, (unsigned long long)addr);
    printf("Descriptor version: %d.%d\n", (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
    if (bootmem_desc_ptr->major_version > 3)
    {
        printf("Warning: Bootmem descriptor version is newer than expected\n");
    }
    if (!addr)
    {
        printf("mem list is empty!\n");
    }
    while (addr)
    {
        printf("Block address: 0x%08qx, size: 0x%08qx, next: 0x%08qx\n",
               (unsigned long long)addr,
               (unsigned long long)octeon_phy_mem_get_size(addr),
               (unsigned long long)octeon_phy_mem_get_next(addr));
        addr = octeon_phy_mem_get_next(addr);
    }
    printf("\n\n");

}

/**
 * Returns the size of available memory in bytes, only
 * counting blocks that are at least as big as the minimum block
 * size.
 *
 * @param bootmem_desc_ptr
 *               pointer to bootmem descriptor block
 * @param min_block_size
 *               Minimum block size to count in total.
 *
 * @return Number of bytes available for allocation that meet the block size requirement
 */
uint64_t octeon_phy_mem_list_available_mem(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t min_block_size)
{
    uint64_t addr;

    uint64_t available_mem = 0;
    addr = bootmem_desc_ptr->head_addr;
    while (addr)
    {
        if (octeon_phy_mem_get_size(addr) >= min_block_size)
            available_mem += octeon_phy_mem_get_size(addr);
        addr = octeon_phy_mem_get_next(addr);
    }
    return(available_mem);

}

/**
 * Internal version of named block find - Finds a named memory block by name.
 *
 * @param bootmem_desc_ptr
 *               Pointer to bootmem memory descriptor block (cvmx_bootmem_desc_t).
 * @param name   Name of memory block to find.
 *               If NULL pointer given, then finds unused descriptor, if available.
 *
 * @return Pointer to memory block descriptor, NULL if not found.
 *         If NULL returned when name parameter is NULL, then no memory
 *         block descriptors are available.
 */
static cvmx_bootmem_named_block_desc_t * octeon_phy_mem_named_block_find_internal(cvmx_bootmem_desc_t *bootmem_desc_ptr, char *name)
{
    unsigned int i;

#if defined(__linux__) && !defined(CONFIG_OCTEON_U_BOOT)
#ifdef CVMX_ABI_N32
    /* Need to use mmapped named block pointer in 32 bit linux apps */
extern cvmx_bootmem_named_block_desc_t *linux32_named_block_array_ptr;
    cvmx_bootmem_named_block_desc_t *named_block_array_ptr = linux32_named_block_array_ptr;
#else
    /* Use XKPHYS for 64 bit linux */
    cvmx_bootmem_named_block_desc_t *named_block_array_ptr = (cvmx_bootmem_named_block_desc_t *)cvmx_phys_to_ptr(bootmem_desc_ptr->named_block_array_addr);
#endif
#else
    /* Simple executive case. (and u-boot)
    ** This could be in the low 1 meg of memory that is not 1-1 mapped, so we need use XKPHYS/KSEG0 addressing for it */
    cvmx_bootmem_named_block_desc_t *named_block_array_ptr = CASTPTR(cvmx_bootmem_named_block_desc_t, CVMX_ADD_SEG32(CVMX_MIPS32_SPACE_KSEG0,bootmem_desc_ptr->named_block_array_addr));
#endif

    if (bootmem_desc_ptr->major_version == 3)
    {
        for (i = 0; i < bootmem_desc_ptr->named_block_num_blocks; i++)
        {
            if ((name && named_block_array_ptr[i].size && !strncmp(name, named_block_array_ptr[i].name, bootmem_desc_ptr->named_block_name_len - 1))
                || (!name && !named_block_array_ptr[i].size))
            {

                return(&(named_block_array_ptr[i]));
            }
        }
    }
    else
    {
        cvmx_dprintf("ERROR: Incompatible bootmem descriptor version: %d.%d\n",
               (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
    }
    return(NULL);


}
/**
 * Finds a named memory block by name.
 *
 * @param bootmem_desc_ptr
 *               Pointer to bootmem memory descriptor block (cvmx_bootmem_desc_t).
 * @param name   Name of memory block to find.
 *               If NULL pointer given, then finds unused descriptor, if available.
 *
 * @return Pointer to memory block descriptor, NULL if not found.
 *         If NULL returned when name parameter is NULL, then no memory
 *         block descriptors are available.
 */
cvmx_bootmem_named_block_desc_t * octeon_phy_mem_named_block_find(cvmx_bootmem_desc_t *bootmem_desc_ptr, char *name)
{
    cvmx_bootmem_named_block_desc_t *block_ptr;

    /* Lock the structure to make sure that it is not being changed while we are
    ** examining it.
    */
    octeon_lock(CAST64(&(bootmem_desc_ptr->lock)));
    block_ptr = octeon_phy_mem_named_block_find_internal(bootmem_desc_ptr, name);
    octeon_unlock(CAST64(&(bootmem_desc_ptr->lock)));
    return(block_ptr);

}
int octeon_phy_mem_named_block_free(cvmx_bootmem_desc_t *bootmem_desc_ptr, char *name)
{
    cvmx_bootmem_named_block_desc_t *named_block_ptr;


    if (bootmem_desc_ptr->major_version != 3)
    {
        cvmx_dprintf("ERROR: Incompatible bootmem descriptor version: %d.%d\n",
               (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
        return(0);
    }
    /* Take lock here, as name lookup/block free/name free need to be atomic */
    octeon_lock(CAST64(&(bootmem_desc_ptr->lock)));
    named_block_ptr = octeon_phy_mem_named_block_find_internal(bootmem_desc_ptr, name);
    if (named_block_ptr)
    {
        octeon_phy_mem_block_free(bootmem_desc_ptr, named_block_ptr->base_addr, named_block_ptr->size);
        named_block_ptr->size = 0;
        /* Set size to zero to indicate block not used. */


    }
    octeon_unlock(CAST64(&(bootmem_desc_ptr->lock)));
    return(!!named_block_ptr);  /* 0 on failure, 1 on success */

}
void octeon_phy_mem_named_block_print(cvmx_bootmem_desc_t *bootmem_desc_ptr)
{
    unsigned int i;
    int printed = 0;

#if defined(__linux__) && !defined(CONFIG_OCTEON_U_BOOT)
#ifdef CVMX_ABI_N32
    /* Need to use mmapped named block pointer in 32 bit linux apps */
extern cvmx_bootmem_named_block_desc_t *linux32_named_block_array_ptr;
    cvmx_bootmem_named_block_desc_t *named_block_array_ptr = linux32_named_block_array_ptr;
#else
    /* Use XKPHYS for 64 bit linux */
    cvmx_bootmem_named_block_desc_t *named_block_array_ptr = (cvmx_bootmem_named_block_desc_t *)cvmx_phys_to_ptr(bootmem_desc_ptr->named_block_array_addr);
#endif
#else
    /* Simple executive case. (and u-boot)
    ** This could be in the low 1 meg of memory that is not 1-1 mapped, so we need use XKPHYS/KSEG0 addressing for it */
    cvmx_bootmem_named_block_desc_t *named_block_array_ptr = CASTPTR(cvmx_bootmem_named_block_desc_t, CVMX_ADD_SEG32(CVMX_MIPS32_SPACE_KSEG0,bootmem_desc_ptr->named_block_array_addr));
#endif

    if (bootmem_desc_ptr->major_version != 3)
    {
        cvmx_dprintf("ERROR: Incompatible bootmem descriptor version: %d.%d\n",
               (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
        return;
    }
    printf("List of currently allocated named bootmem blocks:\n");
    for (i = 0; i < bootmem_desc_ptr->named_block_num_blocks; i++)
    {
        if (named_block_array_ptr[i].size)
        {
            printed++;
            printf("Name: %s, address: 0x%08qx, size: 0x%08qx, index: %d\n",
                   named_block_array_ptr[i].name,
                   (unsigned long long)named_block_array_ptr[i].base_addr,
                   (unsigned long long)named_block_array_ptr[i].size,
                   i);

        }
    }
    if (!printed)
    {
        printf("No named bootmem blocks exist.\n");
    }

}
uint64_t octeon_phy_mem_named_block_alloc(cvmx_bootmem_desc_t *bootmem_desc_ptr, uint64_t size, uint64_t min_addr, uint64_t max_addr, uint64_t alignment, char *name)
{
    uint64_t addr_allocated;
    cvmx_bootmem_named_block_desc_t *named_block_desc_ptr;

    if (bootmem_desc_ptr->major_version != 3)
    {
        cvmx_dprintf("ERROR: Incompatible bootmem descriptor version: %d.%d\n",
               (int)bootmem_desc_ptr->major_version, (int)bootmem_desc_ptr->minor_version);
        return(0);
    }


    /* Take lock here, as name lookup/block alloc/name add need to be atomic */
    octeon_lock(CAST64(&(bootmem_desc_ptr->lock)));


    /* Get pointer to first available named block descriptor */
    named_block_desc_ptr = octeon_phy_mem_named_block_find_internal(bootmem_desc_ptr, NULL);

    /* Check to see if name already in use, return error if name
    ** not available or no more room for blocks.
    */
    if (octeon_phy_mem_named_block_find_internal(bootmem_desc_ptr, name) || !named_block_desc_ptr)
    {
        octeon_unlock(CAST64(&(bootmem_desc_ptr->lock)));
        return(0);
    }


    /* Round size up to mult of minimum alignment bytes
    ** We need the actual size allocated to allow for blocks to be coallesced
    ** when they are freed.  The alloc routine does the same rounding up
    ** on all allocations. */
    size = (size + (CVMX_BOOTMEM_ALIGNMENT_SIZE - 1)) & ~(CVMX_BOOTMEM_ALIGNMENT_SIZE - 1);

    addr_allocated = octeon_phy_mem_block_alloc(bootmem_desc_ptr, size, min_addr, max_addr, alignment);
    if (addr_allocated)
    {
        named_block_desc_ptr->base_addr = addr_allocated;
        named_block_desc_ptr->size = size;
        strncpy(named_block_desc_ptr->name, name, bootmem_desc_ptr->named_block_name_len);
        named_block_desc_ptr->name[bootmem_desc_ptr->named_block_name_len - 1] = 0;
    }
    else
    {
        cvmx_dprintf("octeon_phy_mem_named_block_alloc: alloc failed!\n");
    }


    octeon_unlock(CAST64(&(bootmem_desc_ptr->lock)));
    return(addr_allocated);

}

