//#define DEBUG
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


/**
 *
 * $Id: octeon_boot.c,v 1.117 2007/01/03 15:25:43 fhooker Exp $
 * 
 */
 
 
#include <common.h>
#include <command.h>
#include <exports.h>
#include <watchdog.h>
#include <linux/ctype.h>
#include <lib_octeon.h>
#include <octeon_eeprom_types.h>
#include <lib_octeon_shared.h>
#include <environment.h>
#include "octeon_boot.h"
#include "cvmx-bootmem-shared.h"
#include "cvmx-app-init.h"

/************************************************************************/
/******* Global variable definitions ************************************/
/************************************************************************/
uint32_t cur_exception_base = EXCEPTION_BASE_INCR;
octeon_boot_descriptor_t boot_desc[16];
cvmx_bootinfo_t cvmx_bootinfo_array[16];

boot_info_block_t boot_info_block_array[16];
cvmx_bootmem_desc_t *glob_phy_desc_ptr;
uint32_t coremask_to_run;
uint64_t boot_cycle_adjustment;
/************************************************************************/
/************************************************************************/


//#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif


/************************************************************************/
uint32_t get_except_base_reg(void)
{
    uint32_t tmp;

    asm volatile (
    "    .set push              \n"
    "    .set mips64            \n"
    "    .set noreorder         \n"
    "    mfc0   %[tmp], $15, 1  \n"
    "    .set pop               \n"
     : [tmp] "=&r" (tmp) : );

    return(tmp);
}
void set_except_base_addr(uint32_t addr)
{

    asm volatile (
          "  .set push                  \n"
          "  .set mips64                \n"
          "  .set noreorder             \n"
          "  mtc0   %[addr], $15, 1     \n"
          "  .set pop                   \n"
         : :[addr] "r" (addr) );
}
uint64_t get_cop0_cvmctl_reg(void)
{
    uint32_t tmp_low, tmp_hi;

    asm volatile (
               "   .set push               \n"
               "   .set mips64                  \n"
               "   .set noreorder               \n"
               "   dmfc0 %[tmpl], $9, 7         \n"
               "   dadd   %[tmph], %[tmpl], $0   \n"
               "   dsrl  %[tmph], 32            \n"
               "   dsll  %[tmpl], 32            \n"
               "   dsrl  %[tmpl], 32            \n"
               "   .set pop                 \n"
                  : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );

    return(((uint64_t)tmp_hi << 32) + tmp_low);
}
void set_cop0_cvmctl_reg(uint64_t value)
{
    uint32_t val_low  = value & 0xffffffff;
    uint32_t val_high = value  >> 32;

    asm volatile (
        "  .set push                         \n"
        "  .set mips64                       \n"
        "  .set noreorder                    \n"
        /* Standard twin 32 bit -> 64 bit construction */
        "  dsll  %[valh], 32                 \n"
        "  dsll  %[vall], 32          \n"
        "  dsrl  %[vall], 32          \n"
        "  daddu %[valh], %[valh], %[vall]   \n"
        /* Combined value is in valh */
        "  dmtc0 %[valh],$9, 7               \n"
        "  .set pop                          \n"
         :: [valh] "r" (val_high), [vall] "r" (val_low) );


}
uint64_t get_cop0_cvmmemctl_reg(void)
{
    uint32_t tmp_low, tmp_hi;

    asm volatile (
               "   .set push                  \n"
               "   .set mips64                  \n"
               "   .set noreorder               \n"
               "   dmfc0 %[tmpl], $11, 7         \n"
               "   dadd   %[tmph], %[tmpl], $0   \n"
               "   dsrl  %[tmph], 32            \n"
               "   dsll  %[tmpl], 32            \n"
               "   dsrl  %[tmpl], 32            \n"
               "   .set pop                     \n"
                  : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );

    return(((uint64_t)tmp_hi << 32) + tmp_low);
}
void set_cop0_cvmmemctl_reg(uint64_t value)
{
    uint32_t val_low  = value & 0xffffffff;
    uint32_t val_high = value  >> 32;
    

    asm volatile (
        "  .set push                         \n"
        "  .set mips64                       \n"
        "  .set noreorder                    \n"
        /* Standard twin 32 bit -> 64 bit construction */
        "  dsll  %[valh], 32                 \n"
        "  dsll  %[vall], 32          \n"
        "  dsrl  %[vall], 32          \n"
        "  daddu %[valh], %[valh], %[vall]   \n"
        /* Combined value is in valh */
        "  dmtc0 %[valh],$11, 7               \n"
        "  .set pop                      \n"
         :: [valh] "r" (val_high), [vall] "r" (val_low) );


}

uint32_t get_core_num(void)
{

    return(0x3FF & get_except_base_reg());
}

uint32_t get_except_base_addr(void)
{

    return(~0x3FF & get_except_base_reg());
}

void copy_default_except_handlers(uint32_t addr_arg)
{
    uint8_t *addr = (void *)addr_arg;
    uint32_t nop_loop[2] = {0x1000ffff, 0x0};
    /* Set up some dummy loops for exception handlers to help with debug */
    memset(addr, 0, 0x800);
    memcpy(addr + 0x00, nop_loop, 8);
    memcpy(addr + 0x80, nop_loop, 8);
    memcpy(addr + 0x100, nop_loop, 8);
    memcpy(addr + 0x180, nop_loop, 8);
    memcpy(addr + 0x200, nop_loop, 8);
    memcpy(addr + 0x280, nop_loop, 8);
    memcpy(addr + 0x300, nop_loop, 8);
    memcpy(addr + 0x380, nop_loop, 8);
    memcpy(addr + 0x400, nop_loop, 8);
    memcpy(addr + 0x480, nop_loop, 8);
    memcpy(addr + 0x500, nop_loop, 8);
    memcpy(addr + 0x580, nop_loop, 8);
    memcpy(addr + 0x600, nop_loop, 8);
    memcpy(addr + 0x680, nop_loop, 8);


}
/**
 * Provide current cycle counter as a return value
 * 
 * @return current cycle counter
 */
uint64_t octeon_get_cycles(void)
{
    uint32_t tmp_low, tmp_hi;

    asm volatile (
               "   .set push                  \n"
               "   .set mips64                  \n"
               "   .set noreorder               \n"
               "   dmfc0 %[tmpl], $9, 6         \n"
               "   dadd   %[tmph], %[tmpl], $0   \n"
               "   dsrl  %[tmph], 32            \n"
               "   dsll  %[tmpl], 32            \n"
               "   dsrl  %[tmpl], 32            \n"
               "   .set pop                 \n"
                  : [tmpl] "=&r" (tmp_low), [tmph] "=&r" (tmp_hi) : );

    return(((uint64_t)tmp_hi << 32) + tmp_low);
}

void octeon_delay_cycles(uint64_t cycles)
{
    uint64_t start = octeon_get_cycles();
    while (start + cycles > octeon_get_cycles())
        ;
}


uint32_t octeon_get_cop0_status(void)
{
    uint32_t tmp;

    asm volatile (
               "   .set push                    \n"
               "   .set mips64                  \n"
               "   .set noreorder               \n"
               "   mfc0 %[tmp], $12, 0         \n"
               "   .set pop                     \n"
                  : [tmp] "=&r" (tmp) : );

    return tmp;
}
void cvmx_set_cycle(uint64_t cycle)
{
    uint32_t val_low  = cycle & 0xffffffff;
    uint32_t val_high = cycle  >> 32;

    asm volatile (
        "  .set push                         \n"
        "  .set mips64                       \n"
        "  .set noreorder                    \n"
        /* Standard twin 32 bit -> 64 bit construction */
        "  dsll  %[valh], 32                 \n"
        "  dsll  %[vall], 32          \n"
        "  dsrl  %[vall], 32          \n"
        "  daddu %[valh], %[valh], %[vall]   \n"
        /* Combined value is in valh */
        "  dmtc0 %[valh],$9, 6               \n"
        "  .set pop                          \n"
         :: [valh] "r" (val_high), [vall] "r" (val_low) );

}




#if CONFIG_OCTEON_SIM
/* This address on the boot bus is used by the oct-sim script to pass
** flags to the bootloader.
*/
#define OCTEON_BOOTLOADER_FLAGS_ADDR    (0x9ffffff0ull)  /* 32 bit uint */
#endif


int octeon_setup_boot_desc_block(uint32_t core_mask, int argc, char **argv, uint64_t entry_point, uint32_t stack_size, uint32_t heap_size, uint32_t boot_flags, uint64_t stack_heep_base_addr, uint32_t image_flags, uint32_t config_flags) 
{
#ifdef CONFIG_APBOOT
    extern char version_string[];
    extern char build_num[];
    char tbuf[128];
#endif
    /* Set up application descriptors for all cores running this application.
    ** These are only used during early boot - the simple exec init code copies any information
    ** of interest before the application starts.  This is stored in the top of the heap,
    ** and is guaranteed to be looked at before malloc is called.
    */

    DECLARE_GLOBAL_DATA_PTR;
    /* Set up space for passing arguments to application */
    int argv_size = 0;
    int i;

#ifdef CONFIG_APBOOT
    argv[argc] = tbuf;
    argc++;
    sprintf(tbuf, "bootver=%s/%s", version_string, build_num);
#endif
    
    for (i = 0; i < argc; i++)
        argv_size += strlen(argv[i]) + 1;

    /* Allocate permanent storage for the argv strings. */
    uint64_t argv_data_addr = octeon_phy_mem_block_alloc(glob_phy_desc_ptr, argv_size + 64, 0, 0x7fffffff, 0);

    char *argv_data = (char *)(uint32_t)argv_data_addr;
    
    char *argv_array[OCTEON_ARGV_MAX_ARGS];
    for (i = 0; i  < argc; i++)
    {
        argv_array[i] = argv_data;
        argv_data = strcpy(argv_data, argv[i]) + strlen(argv[i]) + 1;
        argv_size += strlen(argv[i]) + 1;
    }

#if OCTEON_APP_INIT_H_VERSION >= 1  /* The UART1 flag is new */
    /* If console is on uart 1 pass this to executive */
    {
        DECLARE_GLOBAL_DATA_PTR;
        if (gd->console_uart == 1)
            boot_flags |= OCTEON_BL_FLAG_CONSOLE_UART1;
    }
#endif

    /* Bootloader boot_flags only used in simulation environment, default
    ** to no magic as required by hardware */
    boot_flags |= OCTEON_BL_FLAG_NO_MAGIC;

#if CONFIG_OCTEON_SIM && !CONFIG_OCTEON_NO_BOOT_BUS
    /* Get debug and no_magic flags from oct-sim script if running on simulator */
    boot_flags &= ~(OCTEON_BL_FLAG_DEBUG | OCTEON_BL_FLAG_NO_MAGIC);
    boot_flags |= *(uint32_t *)OCTEON_BOOTLOADER_FLAGS_ADDR & (OCTEON_BL_FLAG_DEBUG | OCTEON_BL_FLAG_NO_MAGIC);
#endif

    int core;
    for (core = coremask_iter_init(core_mask); core >= 0; core = coremask_iter_next())
    {

#if (OCTEON_CURRENT_DESC_VERSION != 6) && (OCTEON_CURRENT_DESC_VERSION != 7)
#error Incorrect boot descriptor version for this bootloader release, check toolchain version!
#endif


        if (getenv("icache_prefetch_disable"))
            boot_info_block_array[core].flags |= BOOT_INFO_FLAG_DISABLE_ICACHE_PREFETCH;
        boot_info_block_array[core].entry_point = entry_point;
        boot_info_block_array[core].exception_base = cur_exception_base;

        /* Align initial stack value to 16 byte alignment */
        boot_info_block_array[core].stack_top =  (stack_heep_base_addr + stack_size) & (~0xFull);

        if (image_flags & OCTEON_BOOT_DESC_IMAGE_LINUX)
        {
            boot_info_block_array[core].boot_desc_addr = octeon_phy_mem_block_alloc(glob_phy_desc_ptr, sizeof(boot_desc[0]), 0, 0x7fffffff, 0);
            if (!boot_info_block_array[core].boot_desc_addr)
            {
                printf("ERROR allocating memory for boot descriptor block\n");
                return(-1);
            }
            boot_desc[core].cvmx_desc_vaddr = octeon_phy_mem_block_alloc(glob_phy_desc_ptr, sizeof(cvmx_bootinfo_t), 0, 0x7fffffff, 0);
            if (!boot_desc[core].cvmx_desc_vaddr)
            {
                printf("ERROR allocating memory for cvmx bootinfo block\n");
                return(-1);
            }
        }
        else
        {
            /* For simple exec we put these in Heap mapping */
            boot_desc[core].heap_base =  stack_heep_base_addr + stack_size;
            boot_desc[core].heap_end =  stack_heep_base_addr + stack_size + heap_size;
            /* Put boot descriptor at top of heap, align address */
            boot_info_block_array[core].boot_desc_addr = (boot_desc[core].heap_end - sizeof(boot_desc[0])) & ~0x7ULL;
            /* Put boot descriptor below boot descriptor near top of heap, align address */
            boot_desc[core].cvmx_desc_vaddr = (boot_info_block_array[core].boot_desc_addr - sizeof(cvmx_bootinfo_t)) & (~0x7ull);
        }


        /* The boot descriptor will be copied into the top of the heap for the core.  The
        ** application init code is responsible for copying the parts of the descriptor block
        ** that the application is interested in, as space is not reserved in the heap for
        ** this data.
        ** Note that the argv strings themselves are put into permanently allocated storage.
        */

        /* Most fields in the boot (app) descriptor are depricated, and have been
        ** moved to the cvmx_bootinfo structure.  They are set here to ease tranistion,
        ** but may be removed in the future.
        */

        boot_desc[core].argc = argc;
        memcpy(boot_desc[core].argv, argv_array, sizeof(argv_array));
        boot_desc[core].desc_version = OCTEON_CURRENT_DESC_VERSION;
        boot_desc[core].desc_size = sizeof(boot_desc[0]);
        /* boot_flags set later, copied from bootinfo block */
        boot_desc[core].debugger_flags_base_addr = BOOTLOADER_DEBUG_FLAGS_BASE;

        /* Set 'master' core for application.  This is the only core that will do certain
        ** init code such as copying the exception vectors for the app, etc
        ** NOTE: This is deprecated, and the application should use the coremask
        ** to choose a core to to setup with.
        */
        if (core == coremask_iter_get_first_core())
            cvmx_bootinfo_array[core].flags |= BOOT_FLAG_INIT_CORE;
        /* end of valid fields in boot descriptor */

#if (CVMX_BOOTINFO_MAJ_VER != 1)
#error Incorrect cvmx descriptor version for this bootloader release, check simple executive version!
#endif

        cvmx_bootinfo_array[core].flags |= boot_flags;
        cvmx_bootinfo_array[core].major_version = CVMX_BOOTINFO_MAJ_VER;
        cvmx_bootinfo_array[core].minor_version = CVMX_BOOTINFO_MIN_VER;
        cvmx_bootinfo_array[core].core_mask = core_mask;
        cvmx_bootinfo_array[core].dram_size = gd->ram_size/(1024 * 1024);  /* Convert from bytes to Megabytes */
        cvmx_bootinfo_array[core].phy_mem_desc_addr = (uint32_t)glob_phy_desc_ptr;
        cvmx_bootinfo_array[core].exception_base_addr = cur_exception_base;
        cvmx_bootinfo_array[core].dclock_hz = gd->ddr_clock_mhz * 1000000;
        cvmx_bootinfo_array[core].eclock_hz = gd->cpu_clock_mhz * 1000000;
#if 0
        cvmx_bootinfo_array[core].spi_clock_hz = (uint64_t)gd->clock_desc.spi_clock_mhz * 1000000;
#else
        cvmx_bootinfo_array[core].reserved0 = (uint64_t)gd->clock_desc.spi_clock_mhz * 1000000;
#endif
        cvmx_bootinfo_array[core].board_type = gd->board_desc.board_type;
        cvmx_bootinfo_array[core].board_rev_major = gd->board_desc.rev_major;
        cvmx_bootinfo_array[core].board_rev_minor = gd->board_desc.rev_minor;
#if 0
        cvmx_bootinfo_array[core].chip_type = gd->board_desc.chip_type;
        cvmx_bootinfo_array[core].chip_rev_major = gd->board_desc.chip_rev_major;
        cvmx_bootinfo_array[core].chip_rev_minor = gd->board_desc.chip_rev_minor;
#else
        cvmx_bootinfo_array[core].reserved1 = gd->board_desc.chip_type;
        cvmx_bootinfo_array[core].reserved2 = gd->board_desc.chip_rev_major;
        cvmx_bootinfo_array[core].reserved3 = gd->board_desc.chip_rev_minor;
#endif
        cvmx_bootinfo_array[core].mac_addr_count = gd->mac_desc.count;
        memcpy(cvmx_bootinfo_array[core].mac_addr_base, (void *)gd->mac_desc.mac_addr_base, sizeof(cvmx_bootinfo_array[core].mac_addr_base));
        strncpy(cvmx_bootinfo_array[core].board_serial_number, (char *)(gd->board_desc.serial_str),CVMX_BOOTINFO_OCTEON_SERIAL_LEN);

        dprintf("board type is: %d, %s\n", cvmx_bootinfo_array[core].board_type, cvmx_board_type_to_string(cvmx_bootinfo_array[core].board_type));
#if (CVMX_BOOTINFO_MIN_VER >= 1)

#ifdef OCTEON_CF_COMMON_BASE_ADDR
        cvmx_bootinfo_array[core].compact_flash_common_base_addr = OCTEON_CF_COMMON_BASE_ADDR;
#else
        cvmx_bootinfo_array[core].compact_flash_common_base_addr = 0;
#endif
#ifdef OCTEON_CF_ATTRIB_BASE_ADDR
        cvmx_bootinfo_array[core].compact_flash_attribute_base_addr = OCTEON_CF_ATTRIB_BASE_ADDR;
#else
        cvmx_bootinfo_array[core].compact_flash_attribute_base_addr = 0;
#endif
#if CONFIG_OCTEON_EBT3000
        /* Rev 1 boards use 4 segment display at slightly different address */
        if (gd->board_desc.rev_major == 1)
            cvmx_bootinfo_array[core].led_display_base_addr  = OCTEON_CHAR_LED_BASE_ADDR;
        else
            cvmx_bootinfo_array[core].led_display_base_addr  = OCTEON_CHAR_LED_BASE_ADDR + 0xf8;
#else
#ifdef OCTEON_CHAR_LED_BASE_ADDR
        if (OCTEON_CHAR_LED_BASE_ADDR)
            cvmx_bootinfo_array[core].led_display_base_addr = OCTEON_CHAR_LED_BASE_ADDR + 0xf8;
        else
#endif
            cvmx_bootinfo_array[core].led_display_base_addr = 0;
#endif

#endif  /* (CVMX_BOOTINFO_MAJ_VER >= 1) */

#if (CVMX_BOOTINFO_MIN_VER >= 2)
        /* Set DFA reference if available */
        cvmx_bootinfo_array[core].dfa_ref_clock_hz = gd->clock_desc.dfa_ref_clock_mhz_x_8/8*1000000;

        /* Copy boot_flags settings to config flags */
        if (boot_flags & OCTEON_BL_FLAG_DEBUG)
            cvmx_bootinfo_array[core].config_flags |= CVMX_BOOTINFO_CFG_FLAG_DEBUG;
        if (boot_flags & OCTEON_BL_FLAG_NO_MAGIC)
            cvmx_bootinfo_array[core].config_flags |= CVMX_BOOTINFO_CFG_FLAG_NO_MAGIC;
        /* Set config flags */
        if (CONFIG_OCTEON_PCI_HOST)
            cvmx_bootinfo_array[core].config_flags |= CVMX_BOOTINFO_CFG_FLAG_PCI_HOST;
#ifdef CONFIG_OCTEON_PCI_TARGET
        else if (CONFIG_OCTEON_PCI_TARGET)
            cvmx_bootinfo_array[core].config_flags |= CVMX_BOOTINFO_CFG_FLAG_PCI_TARGET;
#endif

        cvmx_bootinfo_array[core].config_flags |= config_flags;
#endif  /* (CVMX_BOOTINFO_MAJ_VER >= 2) */

        /* Copy deprecated fields from cvmx_bootinfo array to boot descriptor for 
        ** compatibility
        */
        boot_desc[core].core_mask       = cvmx_bootinfo_array[core].core_mask;       
        boot_desc[core].flags       = cvmx_bootinfo_array[core].flags;       
//        boot_desc[core].exception_base_addr       = cvmx_bootinfo_array[core].exception_base_addr;       
        boot_desc[core].eclock_hz       = cvmx_bootinfo_array[core].eclock_hz;       
//        boot_desc[core].dclock_hz       = cvmx_bootinfo_array[core].dclock_hz;       
//        boot_desc[core].spi_clock_hz    = cvmx_bootinfo_array[core].spi_clock_hz;    
//        boot_desc[core].board_type      = cvmx_bootinfo_array[core].board_type;      
//        boot_desc[core].board_rev_major = cvmx_bootinfo_array[core].board_rev_major; 
//        boot_desc[core].board_rev_minor = cvmx_bootinfo_array[core].board_rev_minor;
//        boot_desc[core].chip_type       = cvmx_bootinfo_array[core].chip_type;       
//        boot_desc[core].chip_rev_major  = cvmx_bootinfo_array[core].chip_rev_major;  
//        boot_desc[core].chip_rev_minor  = cvmx_bootinfo_array[core].chip_rev_minor;  
//        boot_desc[core].mac_addr_count  = cvmx_bootinfo_array[core].mac_addr_count;  
        boot_desc[core].dram_size       = cvmx_bootinfo_array[core].dram_size;       
        boot_desc[core].phy_mem_desc_addr = cvmx_bootinfo_array[core].phy_mem_desc_addr;
        /* Linux needs the exception base in the boot descriptor for now */
//        boot_desc[core].exception_base_addr = cur_exception_base;


//        memcpy(boot_desc[core].mac_addr_base, cvmx_bootinfo_array[core].mac_addr_base, 6);
//        strncpy(boot_desc[core].board_serial_number, cvmx_bootinfo_array[core].board_serial_number, OCTEON_SERIAL_LEN);
        /* End deprecated boot descriptor fields */


        if (image_flags & OCTEON_BOOT_DESC_IMAGE_LINUX)
        {
            // copy boot desc for use by app
            memcpy((void *)(uint32_t)boot_info_block_array[core].boot_desc_addr, (void *)&boot_desc[core], boot_desc[core].desc_size);
            memcpy((void *)(uint32_t)boot_desc[core].cvmx_desc_vaddr, (void *)&cvmx_bootinfo_array[core], sizeof(cvmx_bootinfo_t));
        }
        else
        {
            /* Copy boot descriptor and cmvx_bootinfo structures to correct virtual address
            ** for the application to read them.         */
            mem_copy_tlb(boot_info_block_array[core].tlb_entries, boot_info_block_array[core].boot_desc_addr, cvmx_ptr_to_phys(&boot_desc[core]), boot_desc[core].desc_size);
            mem_copy_tlb(boot_info_block_array[core].tlb_entries, boot_desc[core].cvmx_desc_vaddr, cvmx_ptr_to_phys(&cvmx_bootinfo_array[core]), sizeof(cvmx_bootinfo_t));
        }

    }
    copy_default_except_handlers(cur_exception_base);
    cur_exception_base += EXCEPTION_BASE_INCR;

    dprintf("stack expected: 0x%Lx, actual: 0x%Lx\n", stack_heep_base_addr + stack_size, boot_desc[0].stack_top);
    dprintf("heap_base expected: 0x%Lx, actual: 0x%Lx\n", stack_heep_base_addr + stack_size, boot_desc[0].heap_base);
    dprintf("heap_top expected: 0x%Lx, actual: 0x%Lx\n", stack_heep_base_addr + stack_size + heap_size, boot_desc[0].heap_end);
    dprintf("Entry point (virt): 0x%Lx\n", entry_point);

    return(0);
}









uint32_t coremask_iter_core;
uint32_t coremask_iter_mask;
int coremask_iter_first_core;

int coremask_iter_init(uint32_t mask)
{
    coremask_iter_mask = mask;
    coremask_iter_core = 0;
    coremask_iter_first_core = -1;  /* Set to invalid value */
    return(coremask_iter_next());
}
int coremask_iter_next(void)
{

    while (!((1 << coremask_iter_core) & coremask_iter_mask) && coremask_iter_core < 16)
        coremask_iter_core++;
    if (coremask_iter_core > 15)
        return(-1);
    
    /* Set first core */
    if (coremask_iter_first_core < 0)
        coremask_iter_first_core = coremask_iter_core;
    
    return(coremask_iter_core++);
}
int coremask_iter_get_first_core(void)
{
    return(coremask_iter_first_core);
}



#if !CONFIG_OCTEON_SIM
int octeon_cf_present(void)
{
#ifdef OCTEON_CF_ATTRIB_CHIP_SEL
    uint32_t *ptr = (void *)(OCTEON_CF_COMMON_BASE_ADDR);
    return (*ptr != 0xffffffff);
#else
    return 0;
#endif
}
#endif


void octeon_write64(uint64_t csr_addr, uint64_t val)
{

    volatile uint32_t val_low  = val & 0xffffffff;
    volatile uint32_t val_high = val  >> 32;

    volatile uint32_t addr_low  = csr_addr & 0xffffffff;
    volatile uint32_t addr_high = csr_addr  >> 32;

    

    asm volatile (
      "  .set push                         \n"
      "  .set mips64                       \n"
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[valh], 32                 \n"
      "  dsll  %[vall], 32          \n"
      "  dsrl  %[vall], 32          \n"
      "  daddu %[valh], %[valh], %[vall]   \n"
      /* Combined value is in valh */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[addrh], 32                 \n"
      "  dsll  %[addrl], 32          \n"
      "  dsrl  %[addrl], 32          \n"
      "  daddu %[addrh], %[addrh], %[addrl]   \n"
      /* Combined value is in addrh */
      "  sd %[valh], 0(%[addrh])   \n"
      "  .set pop                          \n"
      : : [valh] "r" (val_high), [vall] "r" (val_low), [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");


}

uint64_t octeon_read64(uint64_t csr_addr)
{

    /* volatiles on variables seems to be needed.... */
    volatile uint32_t val_low;
    volatile uint32_t val_high;

    volatile uint32_t addr_low  = csr_addr & 0xffffffff;
    volatile uint32_t addr_high = csr_addr  >> 32;

    asm volatile (
                  "  .set push                         \n"
                  "  .set mips64                       \n"
                  "  .set noreorder                    \n"
                  /* Standard twin 32 bit -> 64 bit construction */
                  "  dsll  %[addrh], 32                 \n"
                  "  dsll  %[addrl], 32          \n"
                  "  dsrl  %[addrl], 32          \n"
                  "  daddu %[addrh], %[addrh], %[addrl]   \n"
                  /* Combined value is in addrh */
                  "  ld    %[valh], 0(%[addrh])   \n"
                  "  dadd   %[vall], %[valh], $0   \n"
                  "  dsrl  %[valh], 32                 \n"
                  "  dsll  %[vall], 32          \n"
                  "  dsrl  %[vall], 32          \n"
                  "  .set pop                          \n"
                  :[valh] "=&r" (val_high), [vall] "=&r" (val_low) : [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");

    return(((uint64_t)val_high << 32) | val_low);

}

/* octeon_write64_byte and octeon_read64_byte are only used by the debugger stub.
** The debugger will generate KSEG0 addresses that are not in the 64 bit compatibility
** space, so we detect that and fix it up.  This should probably be addressed in the 
** debugger itself, as this fixup makes some valid 64 bit address inaccessible.
*/
#define DO_COMPAT_ADDR_FIX
void octeon_write64_byte(uint64_t csr_addr, uint8_t val)
{

    volatile uint32_t addr_low  = csr_addr & 0xffffffff;
    volatile uint32_t addr_high = csr_addr  >> 32;
    
    if (!addr_high && (addr_low & 0x80000000))
    {
#ifdef DO_COMPAT_ADDR_FIX
        addr_high = ~0;
#endif
#if 0
        char tmp_str[500];
        sprintf(tmp_str, "fixed read64_byte at low  addr: 0x%x\n", addr_low);
        simprintf(tmp_str);
        sprintf(tmp_str, "fixed read64_byte at high addr: 0x%x\n", addr_high);
        simprintf(tmp_str);
#endif

    }

    asm volatile (
      "  .set push                         \n"
      "  .set mips64                       \n"
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[addrh], 32                 \n"
      "  dsll  %[addrl], 32          \n"
      "  dsrl  %[addrl], 32          \n"
      "  daddu %[addrh], %[addrh], %[addrl]   \n"
      /* Combined value is in addrh */
      "  sb %[val], 0(%[addrh])   \n"
      "  .set pop                          \n"
      : :[val] "r" (val), [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");


}

uint8_t octeon_read64_byte(uint64_t csr_addr)
{
    
    uint8_t val;

    uint32_t addr_low  = csr_addr & 0xffffffff;
    uint32_t addr_high = csr_addr  >> 32;

    if (!addr_high && (addr_low & 0x80000000))
    {
#ifdef DO_COMPAT_ADDR_FIX
        addr_high = ~0;
#endif
#if 0
        char tmp_str[500];
        sprintf(tmp_str, "fixed read64_byte at low  addr: 0x%x\n", addr_low);
        simprintf(tmp_str);
        sprintf(tmp_str, "fixed read64_byte at high addr: 0x%x\n", addr_high);
        simprintf(tmp_str);
#endif

    }


    asm volatile (
                  "  .set push                         \n"
                  "  .set mips64                       \n"
                  "  .set noreorder                    \n"
                  /* Standard twin 32 bit -> 64 bit construction */
                  "  dsll  %[addrh], 32                 \n"
                  "   dsll  %[addrl], 32          \n"
                  "  dsrl  %[addrl], 32          \n"
                  "  daddu %[addrh], %[addrh], %[addrl]   \n"
                  /* Combined value is in addrh */
                  "  lb    %[val], 0(%[addrh])   \n"
                  "  .set pop                          \n"
                  :[val] "=&r" (val) : [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");

    return(val);

}








void octeon_phy_mem_set_size(uint64_t addr, uint64_t size)
{
    octeon_write64(MAKE_XKPHYS(addr + 8), size);
}
void octeon_phy_mem_set_next(uint64_t addr, uint64_t next)
{
    octeon_write64(MAKE_XKPHYS(addr), next);
}
uint64_t octeon_phy_mem_get_size(uint64_t addr)
{
    return(octeon_read64(MAKE_XKPHYS(addr + 8)));
}
uint64_t octeon_phy_mem_get_next(uint64_t addr)
{
    return(octeon_read64(MAKE_XKPHYS(addr)));
}

/* Define locking primitives as NOPS for bootloader */
#define octeon_lock(x)
#define octeon_unlock(x)
/* Include boot mem alloc code from executive */
#include "cvmx-bootmem-shared.c"




uint64_t octeon_phy_mem_list_init(uint64_t mem_size, uint32_t low_reserved_bytes)
{
    uint64_t cur_block_addr;



    if (mem_size > OCTEON_MAX_PHY_MEM_SIZE)
    {
        printf("FATAL ERROR: requested memory size too large.\n");
        return(0);
    }

    if (glob_phy_desc_ptr)

        return(glob_phy_desc_ptr->head_addr);

    /* This is permanent storage (used by app), but since bootloader is left in memory
    ** this allocation is still valid */
    glob_phy_desc_ptr = (void *)(0x7fffffff &(uint32_t)malloc(sizeof(cvmx_bootmem_desc_t)));
    if (!glob_phy_desc_ptr)
    {
        printf("FATAL ERROR: unable to allocate memory for bootmem descriptor!\n");
        return(0);
    }
    /* Set up global pointer to start of list, exclude low 64k for exception vectors, space for global descriptor */
    memset(glob_phy_desc_ptr, 0x0, sizeof(cvmx_bootmem_desc_t));
    /* Set version of bootmem descriptor */
    glob_phy_desc_ptr->major_version = CVMX_BOOTMEM_DESC_MAJ_VER;
    glob_phy_desc_ptr->minor_version = CVMX_BOOTMEM_DESC_MIN_VER;

    /* Initialize the named block structure */
    glob_phy_desc_ptr->named_block_name_len = CVMX_BOOTMEM_NAME_LEN;
    glob_phy_desc_ptr->named_block_num_blocks = CVMX_BOOTMEM_NUM_NAMED_BLOCKS;
    glob_phy_desc_ptr->named_block_array_addr = (uint64_t)(uint32_t)malloc(CVMX_BOOTMEM_NUM_NAMED_BLOCKS * sizeof(cvmx_bootmem_named_block_desc_t));
    glob_phy_desc_ptr->named_block_array_addr &= 0x7fffffff;  /* Make this a physical address, NOT KSEG0 */

    if (!glob_phy_desc_ptr->named_block_array_addr)
    {
        printf("FATAL ERROR: unable to allocate memory for bootmem descriptor!\n");
        return(0);
    }
    memset((void *)(uint32_t)glob_phy_desc_ptr->named_block_array_addr, 0x0, CVMX_BOOTMEM_NUM_NAMED_BLOCKS * sizeof(cvmx_bootmem_named_block_desc_t));


    cur_block_addr = glob_phy_desc_ptr->head_addr = (OCTEON_DDR0_BASE + low_reserved_bytes);


    glob_phy_desc_ptr->head_addr = 0;

    if (mem_size <= OCTEON_DDR0_SIZE)
    {
        octeon_phy_mem_block_free(glob_phy_desc_ptr, cur_block_addr, mem_size - low_reserved_bytes);
        return(glob_phy_desc_ptr->head_addr);
    }

    octeon_phy_mem_block_free(glob_phy_desc_ptr, cur_block_addr, OCTEON_DDR0_SIZE - low_reserved_bytes);

    mem_size -= OCTEON_DDR0_SIZE;

    /* Add DDR2 block next if present */
    if (mem_size > OCTEON_DDR1_SIZE)
    {
        octeon_phy_mem_block_free(glob_phy_desc_ptr, OCTEON_DDR1_BASE, OCTEON_DDR1_SIZE);
        octeon_phy_mem_block_free(glob_phy_desc_ptr, OCTEON_DDR2_BASE, mem_size - OCTEON_DDR1_SIZE);
    }
    else
    {
        octeon_phy_mem_block_free(glob_phy_desc_ptr, OCTEON_DDR1_BASE, mem_size);

    }


    return(glob_phy_desc_ptr->head_addr);


}
/**
 * Memset in 64 bit chunks to a 64 bit addresses.  Needs
 * wrapper to be useful.
 * 
 * @param start_addr start address (must be 8 byte alligned)
 * @param val        value to write
 * @param byte_count number of bytes - must be multiple of 8.
 */
static void octeon_memset64_64only(uint64_t start_addr, uint8_t val, uint64_t byte_count)
{
    if (byte_count & 0x7)
        return;

    volatile uint32_t count_low  = byte_count & 0xffffffff;
    volatile uint32_t count_high = byte_count  >> 32;

    volatile uint32_t addr_low  = start_addr & 0xffffffff;
    volatile uint32_t addr_high = start_addr  >> 32;

    volatile uint32_t val_low = val | ((uint32_t)val << 8) | ((uint32_t)val << 16) | ((uint32_t)val << 24);
    volatile uint32_t val_high = val_low;

    asm volatile (
      "  .set push                         \n"
      "  .set mips64                       \n"
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[cnth], 32                 \n"
      "  dsll  %[cntl], 32          \n"
      "  dsrl  %[cntl], 32          \n"
      "  daddu %[cnth], %[cnth], %[cntl]   \n"
      /* Combined value is in cnth */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[valh], 32                 \n"
      "  dsll  %[vall], 32          \n"
      "  dsrl  %[vall], 32          \n"
      "  daddu %[valh], %[valh], %[vall]   \n"
      /* Combined value is in valh */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[addrh], 32                 \n"
      "  dsll  %[addrl], 32          \n"
      "  dsrl  %[addrl], 32          \n"
      "  daddu %[addrh], %[addrh], %[addrl]   \n"
      /* Combined value is in addrh */ 
        /* Now do real work..... */
      "  1:                               \n" 
        "    daddi  %[cnth], -8              \n"
      "      sd    %[valh], 0(%[addrh])      \n"
      "      bne   $0, %[cnth], 1b          \n"
      "      daddi  %[addrh], 8              \n"
      "  .set pop                           \n"
      : : [cnth] "r" (count_high), [cntl] "r" (count_low), [addrh] "r" (addr_high), [addrl] "r" (addr_low), [vall] "r" (val_low), [valh] "r" (val_high): "memory");


}

/**
 * memcpy in 64 bit chunks to a 64 bit addresses.  Needs wrapper
 * to be useful.
 * 
 * @param dest_addr destination address (must be 8 byte alligned)
 * @param src_addr destination address (must be 8 byte alligned)
 * @param byte_count number of bytes - must be multiple of 8.
 */
static void octeon_memcpy64_64only(uint64_t dest_addr, uint64_t src_addr, uint64_t byte_count)
{
    if (byte_count & 0x7)
        return;

    volatile uint32_t count_low  = byte_count & 0xffffffff;
    volatile uint32_t count_high = byte_count  >> 32;

    volatile uint32_t src_low  = src_addr & 0xffffffff;
    volatile uint32_t src_high = src_addr  >> 32;
    volatile uint32_t dest_low  = dest_addr & 0xffffffff;
    volatile uint32_t dest_high = dest_addr  >> 32;
    uint32_t tmp;


    asm volatile (
      "  .set push                         \n"
      "  .set mips64                       \n"
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[cnth], 32                 \n"
      "  dsll  %[cntl], 32          \n"
      "  dsrl  %[cntl], 32          \n"
      "  daddu %[cnth], %[cnth], %[cntl]   \n"
      /* Combined value is in cnth */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[srch], 32                 \n"
      "  dsll  %[srcl], 32          \n"
      "  dsrl  %[srcl], 32          \n"
      "  daddu %[srch], %[srch], %[srcl]   \n"
      /* Combined value is in srch */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[dsth], 32                 \n"
      "  dsll  %[dstl], 32          \n"
      "  dsrl  %[dstl], 32          \n"
      "  daddu %[dsth], %[dsth], %[dstl]   \n"
      /* Combined value is in dsth */ 
        /* Now do real work..... */
      "  1:                               \n" 
      "      ld    %[tmp], 0(%[srch])      \n"
      "      daddi  %[cnth], -8              \n"
      "      sd    %[tmp], 0(%[dsth])      \n"
      "      daddi  %[dsth], 8              \n"
      "      bne   $0, %[cnth], 1b          \n"
      "      daddi  %[srch], 8              \n"
      "  .set pop                           \n"
      : [tmp] "=&r" (tmp): [cnth] "r" (count_high), [cntl] "r" (count_low), [dsth] "r" (dest_high), [dstl] "r" (dest_low), [srcl] "r" (src_low), [srch] "r" (src_high): "memory");


}

uint64_t memcpy64(uint64_t dest_addr, uint64_t src_addr, uint64_t count)
{


    /* Convert physical address to XKPHYS or KSEG0 so we can read/write to it */
    if (!(dest_addr & 0xffffffff00000000ull) && (dest_addr & (1 << 31)))
        dest_addr |= 0xffffffff00000000ull;
    if (!(src_addr & 0xffffffff00000000ull) && (src_addr & (1 << 31)))
        src_addr |= 0xffffffff00000000ull;
    dest_addr = MAKE_XKPHYS(dest_addr);
    src_addr = MAKE_XKPHYS(src_addr);

    uint64_t to_copy = count;

    if ((src_addr & 0x7) != (dest_addr & 0x7))
    {
        while (to_copy--)
        {
            cvmx_write64_uint8(dest_addr++, cvmx_read64_uint8(src_addr++));
        }
    }
    else
    {
        while (dest_addr & 0x7)
        {
            to_copy--;
            cvmx_write64_uint8(dest_addr++, cvmx_read64_uint8(src_addr++));
        }
        if (to_copy >= 8)
        {
        octeon_memcpy64_64only(dest_addr, src_addr, to_copy & ~0x7ull);
        dest_addr += to_copy & ~0x7ull;
        src_addr += to_copy & ~0x7ull;
        to_copy &= 0x7ull;
        }
        while (to_copy--)
        {
            cvmx_write64_uint8(dest_addr++, cvmx_read64_uint8(src_addr++));
        }
    }
    return count;
}




void memset64(uint64_t start_addr, uint8_t value, uint64_t len)
{

    if (!(start_addr & 0xffffffff00000000ull) && (start_addr & (1 << 31)))
        start_addr |= 0xffffffff00000000ull;
    start_addr = MAKE_XKPHYS(start_addr);

    while ((start_addr & 0x7) && len-- > 0)
    {
        cvmx_write64_uint8(start_addr++,value);
    }
    if (len >= 8)
    {
        octeon_memset64_64only(start_addr,value, len & ~0x7ull);
        start_addr += len & ~0x7ull;
        len &= 0x7;
    }
    while (len-- > 0)
    {
        cvmx_write64_uint8(start_addr++,value);
    }

}


uint32_t mem_copy_tlb(octeon_tlb_entry_t_bl *tlb_array, uint64_t dest_virt, uint64_t src_phys, uint32_t len)
{

    uint64_t cur_src;
    uint32_t cur_len;
    uint64_t dest_phys;
    uint32_t chunk_len;
    int tlb_index;

    cur_src = src_phys;
    cur_len = len;
    dprintf("tlb_ptr: %p, dvirt: 0x%Lx, sphy: 0x%Lx, len: %d\n", tlb_array, dest_virt, src_phys, len);

    for (tlb_index = 0; tlb_index < NUM_OCTEON_TLB_ENTRIES && cur_len > 0; tlb_index++)
    {
        uint32_t page_size = TLB_PAGE_SIZE_FROM_EXP(tlb_array[tlb_index].page_size);


        /* We found a page0 mapping that contains the start of the block */
        if (dest_virt >= (tlb_array[tlb_index].virt_base) &&  dest_virt < (tlb_array[tlb_index].virt_base + page_size))
        {
            dest_phys = tlb_array[tlb_index].phy_page0 + (dest_virt - tlb_array[tlb_index].virt_base);
            chunk_len = MIN((tlb_array[tlb_index].virt_base + page_size) - dest_virt, cur_len);


            dprintf("Copying(0) 0x%x bytes from 0x%Lx to 0x%Lx\n", chunk_len, cur_src, dest_phys);
            memcpy64(dest_phys, cur_src, chunk_len);

            cur_len -= chunk_len;
            dest_virt += chunk_len;
            cur_src += chunk_len;


        }
        /* We found a page1 mapping that contains the start of the block */
        if (cur_len > 0 && dest_virt >= (tlb_array[tlb_index].virt_base + page_size) &&  dest_virt < (tlb_array[tlb_index].virt_base + 2*page_size))
        {
            dest_phys = tlb_array[tlb_index].phy_page1 + (dest_virt - (tlb_array[tlb_index].virt_base + page_size));
            chunk_len = MIN((tlb_array[tlb_index].virt_base + 2 * page_size) - dest_virt, cur_len);

            dprintf("Copying(1) 0x%x bytes from 0x%Lx to 0x%Lx\n", chunk_len, cur_src, dest_phys);
            memcpy64(dest_phys, cur_src, chunk_len);
            cur_len -= chunk_len;
            dest_virt += chunk_len;
            cur_src += chunk_len;



        }
    }
    if (cur_len != 0)
    {
        printf("ERROR copying memory using TLB mappings, cur_len: %d !\n", cur_len);
        return(~0UL);
    }
    else
    {
        return(len);
    }
    

  
}


void octeon_bzero64_pfs(uint64_t start_addr, uint64_t count)
{

    WATCHDOG_RESET();
#define OCTEON_BZERO_PFS_STRIDE (4*128ULL)
    /* 4 Cache line align start/count */
    start_addr &= ~(OCTEON_BZERO_PFS_STRIDE -1);
    count &= ~(OCTEON_BZERO_PFS_STRIDE -1);;

    volatile uint32_t count_low  = count & 0xffffffff;
    volatile uint32_t count_high = count  >> 32;

    volatile uint32_t addr_low  = start_addr & 0xffffffff;
    volatile uint32_t addr_high = start_addr  >> 32;

#if 0
    printf("octeon_bzero64_pfs: start: 0x%Lx, count: 0x%Lx\n", start_addr, count);
#endif

    

    asm volatile (
      "  .set push                   \n"
      "  .set mips64                       \n"
      "  .set noreorder                    \n"
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[cnth], 32                 \n"
      "  dsll  %[cntl], 32          \n"
      "  dsrl  %[cntl], 32          \n"
      "  daddu %[cnth], %[cnth], %[cntl]   \n"
      /* Combined value is in cnth */
      /* Standard twin 32 bit -> 64 bit construction */
      "  dsll  %[addrh], 32                 \n"
      "  dsll  %[addrl], 32          \n"
      "  dsrl  %[addrl], 32          \n"
      "  daddu %[addrh], %[addrh], %[addrl]   \n"
      /* Combined value is in addrh */ 
        /* Now do real work..... */
      " 1:                               \n" 
      "  daddi  %[cnth], -512              \n"
      "  pref 30, 0(%[addrh])      \n"
      "  pref 30, 128(%[addrh])      \n"
      "  pref 30, 256(%[addrh])      \n"
      "  pref 30, 384(%[addrh])      \n"
#if 0
      "  sd %[cnth], 0(%[addrh])      \n"
      "  sd %[cnth], 8(%[addrh])      \n"
      "  sd %[cnth], 16(%[addrh])      \n"
      "  sd %[cnth], 24(%[addrh])      \n"
      "  sd %[cnth], 32(%[addrh])      \n"
      "  sd %[cnth], 40(%[addrh])      \n"
      "  sd %[cnth], 48(%[addrh])      \n"
      "  sd %[cnth], 56(%[addrh])      \n"
      "  sd %[cnth], 64(%[addrh])      \n"
      "  sd %[cnth], 72(%[addrh])      \n"
      "  sd %[cnth], 80(%[addrh])      \n"
      "  sd %[cnth], 88(%[addrh])      \n"
      "  sd %[cnth], 96(%[addrh])      \n"
      "  sd %[cnth], 104(%[addrh])      \n"
      "  sd %[cnth], 112(%[addrh])      \n"
      "  sd %[cnth], 120(%[addrh])      \n"
      "  sd %[cnth], 128(%[addrh])      \n"
      "  sd %[cnth], 136(%[addrh])      \n"
      "  sd %[cnth], 144(%[addrh])      \n"
      "  sd %[cnth], 152(%[addrh])      \n"
      "  sd %[cnth], 160(%[addrh])      \n"
      "  sd %[cnth], 168(%[addrh])      \n"
      "  sd %[cnth], 176(%[addrh])      \n"
      "  sd %[cnth], 184(%[addrh])      \n"
      "  sd %[cnth], 192(%[addrh])      \n"
      "  sd %[cnth], 200(%[addrh])      \n"
      "  sd %[cnth], 208(%[addrh])      \n"
      "  sd %[cnth], 216(%[addrh])      \n"
      "  sd %[cnth], 224(%[addrh])      \n"
      "  sd %[cnth], 232(%[addrh])      \n"
      "  sd %[cnth], 240(%[addrh])      \n"
      "  sd %[cnth], 248(%[addrh])      \n"
      "  sd %[cnth], 256(%[addrh])      \n"
      "  sd %[cnth], 264(%[addrh])      \n"
      "  sd %[cnth], 272(%[addrh])      \n"
      "  sd %[cnth], 280(%[addrh])      \n"
      "  sd %[cnth], 288(%[addrh])      \n"
      "  sd %[cnth], 296(%[addrh])      \n"
      "  sd %[cnth], 304(%[addrh])      \n"
      "  sd %[cnth], 312(%[addrh])      \n"
      "  sd %[cnth], 320(%[addrh])      \n"
      "  sd %[cnth], 328(%[addrh])      \n"
      "  sd %[cnth], 336(%[addrh])      \n"
      "  sd %[cnth], 344(%[addrh])      \n"
      "  sd %[cnth], 352(%[addrh])      \n"
      "  sd %[cnth], 360(%[addrh])      \n"
      "  sd %[cnth], 368(%[addrh])      \n"
      "  sd %[cnth], 376(%[addrh])      \n"
      "  sd %[cnth], 384(%[addrh])      \n"
      "  sd %[cnth], 392(%[addrh])      \n"
      "  sd %[cnth], 400(%[addrh])      \n"
      "  sd %[cnth], 408(%[addrh])      \n"
      "  sd %[cnth], 416(%[addrh])      \n"
      "  sd %[cnth], 424(%[addrh])      \n"
      "  sd %[cnth], 432(%[addrh])      \n"
      "  sd %[cnth], 440(%[addrh])      \n"
      "  sd %[cnth], 448(%[addrh])      \n"
      "  sd %[cnth], 456(%[addrh])      \n"
      "  sd %[cnth], 464(%[addrh])      \n"
      "  sd %[cnth], 472(%[addrh])      \n"
      "  sd %[cnth], 480(%[addrh])      \n"
      "  sd %[cnth], 488(%[addrh])      \n"
      "  sd %[cnth], 496(%[addrh])      \n"
      "  sd %[cnth], 504(%[addrh])      \n"
#endif
      "  bne   $0, %[cnth], 1b          \n"
      "  daddi  %[addrh], 512              \n"
      "  .set pop                       \n"
      : : [cnth] "r" (count_high), [cntl] "r" (count_low), [addrh] "r" (addr_high), [addrl] "r" (addr_low): "memory");


}




#define BOOTSTRAP_STACK_SIZE    (2048)
extern void InitTLBStart(void);  /* in start.S */
extern void OcteonBreak(void);  /* in start.S */
int octeon_setup_boot_vector(uint32_t func_addr, uint32_t core_mask)
{
    DECLARE_GLOBAL_DATA_PTR;
    // setup boot vectors for other cores.....
    int core;
    boot_init_vector_t *boot_vect = (boot_init_vector_t *)(BOOT_VECTOR_BASE);

    for (core = coremask_iter_init(core_mask); core >= 0; core = coremask_iter_next())
    {
        uint32_t tmp_stack = (uint32_t)malloc(BOOTSTRAP_STACK_SIZE);
        if (!tmp_stack)
        {             
            printf("ERROR allocating stack for core: %d\n", core);
            return(-1);
        }
        boot_vect[core].stack_addr = tmp_stack  + BOOTSTRAP_STACK_SIZE;
        /* Have other cores jump directly to DRAM, rather than running out of flash.  This
        ** address is set here because is is the address in DRAM, rather than the address in FLASH
        */
        if (func_addr)
        {
            boot_vect[core].code_addr = (uint32_t)&InitTLBStart;
            boot_vect[core].app_start_func_addr = func_addr;
        }
        else
        {
            /* If fewer than all cores are run, set rest to stop, otherwise
            ** simulator runs forever */
            boot_vect[core].code_addr = (uint32_t)&OcteonBreak;
            boot_vect[core].app_start_func_addr = (uint32_t)&OcteonBreak;
        }
        boot_vect[core].k0_val = (uint32_t)gd;  /* pass global data pointer */
        boot_vect[core].boot_info_addr = (uint32_t)(&boot_info_block_array[core]);

    }

    dprintf("Address of start app: 0x%Lx\n", boot_vect[coremask_iter_get_first_core()].app_start_func_addr);

    OCTEON_SYNC;
    return(0);
}


uint32_t get_coremask_override(void)
{
    uint32_t coremask_override = 0xffff;
    char *cptr;
    cptr = getenv("coremask_override");
    if (cptr)
    {
        coremask_override = simple_strtol(cptr, NULL, 16);
    }
    return(coremask_override);

}
#define OCTEON_POP(result, input) asm ("pop %[rd],%[rs]" : [rd] "=d" (result) : [rs] "d" (input))
/**
 * Generates a coremask with a given number of cores in it given
 * the coremask_override value (defines the available cores) and the
 * number of cores requested.
 * 
 * @param num_cores number of cores requested
 * 
 * @return coremask with requested number of cores, or all that are available.
 *         Prints a warning if fewer than the requested number of cores are returned.
 */
uint32_t octeon_coremask_num_cores(int num_cores)
{
    int shift;
    uint32_t mask=1;
    int cores;

    if (!num_cores)
        return(0);

    uint32_t coremask_override = get_coremask_override();
    for (shift = 1;shift <=16; shift++)
    {
        OCTEON_POP(cores,coremask_override & mask);
        if (cores == num_cores)
            return(coremask_override & mask);
        mask = (mask << 1) | 1;
    }
    printf("####################################################################################\n");
    printf("### Warning: only %02d cores available, not running with requested number of cores ###\n", cores);
    printf("####################################################################################\n");
    return(coremask_override & mask);

}


/* Save a copy of the coremask from the eeprom so that we can print a warning
** if a user enables broken cores.
*/
uint32_t coremask_from_eeprom = 0xffff;
int32_t validate_coremask(uint32_t core_mask)
{

#if !(CONFIG_OCTEON_SIM)
    uint32_t coremask_override;
    coremask_override = get_coremask_override();

    /* Read FUSE register to determine number of cores available, bits set may not
    ** be contiguous. */
    uint32_t ciu_fuse = (uint32_t)octeon_read64(CVMX_CIU_FUSE) & 0xffff;
    int cores;
    OCTEON_POP(cores, ciu_fuse);
    uint32_t fuse_coremask = (1<<cores)-1;
    if ((core_mask & fuse_coremask) != core_mask)
    {
        printf("ERROR: Can't boot cores that don't exist! (available coremask: 0x%x)\n", fuse_coremask);
        return(-1);
    }

    if ((core_mask & coremask_override) != core_mask)
    {
        core_mask &= coremask_override;
        printf("Notice: coremask changed to 0x%x based on coremask_override of 0x%x\n", core_mask, coremask_override);
    }

    if (core_mask & ~coremask_from_eeprom)
    {
        /* The user has manually changed the coremask_override env. variable to run code
        ** on known bad cores.  Print a warning message.
        */
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING: You have changed the coremask_override and are running code on non-functional cores.\n");
        printf("WARNING: The program may crash and/or produce unreliable results.\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        printf("WARNING:\n");
        int delay = 5;
        do
        {
            printf("%d\n", delay);
            cvmx_wait(600000000ULL);  /* Wait a while to make sure warning is seen. */
        } while (delay-- > 0);
    }
#endif
    return core_mask;
}

/* Returns the available coremask either from env or fuses */
uint32_t octeon_get_available_coremask(void)
{

    if (OCTEON_CN38XX_PASS1 == octeon_get_proc_id())
    {
        /* get coremask from environment */
        return(get_coremask_override());
    }
    else if (octeon_is_model(OCTEON_CN38XX))
    {
        /* Here we only need it if no core fuses are blown and the lockdown fuse is not blown.
        ** In all other cases the cores fuses are definitive and we don't need a coremask override
        */
        if ((octeon_read64(CVMX_CIU_FUSE) & 0xffff) == 0xffff && !cvmx_octeon_fuse_locked())
        {
            /* get coremask from environment */
            return(get_coremask_override());
        }
    }

    /* Get number of cores from fuse register, convert to coremask */
    uint32_t ciu_fuse = (uint32_t)(octeon_read64(CVMX_CIU_FUSE) & 0xffff);
    uint32_t cores;
    OCTEON_POP(cores, ciu_fuse);

    return((1<<cores)-1);
}

uint32_t octeon_get_available_core_count(void)
{
    uint32_t coremask, cores;
    coremask = octeon_get_available_coremask();
    OCTEON_POP(cores, coremask);
    return(cores);
}

static void setup_env_named_block(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    int i;
    char *env_ptr = (void *)gd->env_addr;
    int env_size;

    /* Determine the actual size of the environment that is used,
    ** look for two NULs in a row */
    for (i = 0; i < ENV_SIZE - 1; i++ )
    {
        if (env_ptr[i] == 0 && env_ptr[i+1] == 0)
            break;
    }
    env_size = i + 1;

    uint64_t addr;
    /* Allocate a named block for the environment */
    addr = octeon_phy_mem_named_block_alloc(glob_phy_desc_ptr, env_size + 1, 0ull, 0x40000000ull, 0ull, OCTEON_ENV_MEM_NAME);
    char *named_ptr = (void *)(uint32_t)addr;
    if (named_ptr)
    {
        memset(named_ptr, 0x0, octeon_phy_mem_named_block_find(glob_phy_desc_ptr, OCTEON_ENV_MEM_NAME)->size);
        memcpy(named_ptr, env_ptr, env_size);
    }
}

void start_cores(uint32_t coremask_to_start)
{

    dprintf("Bringing coremask: 0x%x out of reset!\n", coremask_to_start);


    /* Set cores that are not going to be started to branch to a 'break'
    ** instruction at boot
    */
    octeon_setup_boot_vector(0, ~coremask_to_start & 0xffff);

    setup_env_named_block();
    /* Only bring cores that we are booting out of reset. */
    uint64_t ciu_fuse = octeon_read64(CVMX_CIU_FUSE);
    int cores;
    OCTEON_POP(cores, ciu_fuse);

    uint64_t reset_val = (~coremask_to_start) & ((1<<cores)-1);

//    printf("Writing 0x%Lx to reset CSR\n", reset_val);

    /* Clear cycle counter so that all cores will be close */
    dprintf("Bootloader: Starting app at cycle: %d\n", (uint32_t)boot_cycle_adjustment);

    boot_cycle_adjustment = octeon_get_cycles();
    OCTEON_SYNC;

    octeon_write64(CVMX_CIU_PP_RST, reset_val);


    if (!(coremask_to_start & 1)) {
#if CONFIG_OCTEON_SIM
        OCTEON_BREAK;  /* On simulator, we end simulation of core... */
#endif    
        /* core 0 not being run, so just loop here */
        while (1) {
            ;
        }
    }
    /* Run the app_start_func for the core - if core 0 is not being run, this will
    ** branch to a 'break' instruction to stop simulation
    */
    WATCHDOG_RESET();	// last chance for a fillup
    ((void (*)(void)) (((boot_init_vector_t *)(BOOT_VECTOR_BASE))[0].app_start_func_addr)) ();
}


uint64_t octeon_read_gpio()
{
    return(octeon_read64(CVMX_GPIO_RX_DAT));
}

void octeon_gpio_cfg_output(int bit) {
    uint64_t val = cvmx_read_csr(CVMX_GPIO_BIT_CFGX(bit));
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(bit), val | 1);
}

void octeon_gpio_xcfg_output(int bit) {
    uint64_t val = cvmx_read_csr(CVMX_GPIO_XBIT_CFGX(bit));
    cvmx_write_csr(CVMX_GPIO_XBIT_CFGX(bit), val | 1);
}

void octeon_gpio_cfg_input(int bit) {
    uint64_t val = cvmx_read_csr(CVMX_GPIO_BIT_CFGX(bit));
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(bit), val & ~1ull);
}

void octeon_gpio_set(int bit) {
    cvmx_write_csr(CVMX_GPIO_TX_SET, 1 << bit);
}

void octeon_gpio_clr(int bit) {
    cvmx_write_csr(CVMX_GPIO_TX_CLR, 1 << bit);
}

int octeon_gpio_value(int bit) {
    return (cvmx_read_csr(CVMX_GPIO_RX_DAT) >> bit) & 1;
}

#if CONFIG_OCTEON_EBT3000
int octeon_ebt3000_rev1(void)
{
    return(!(octeon_read64_byte(OCTEON_PAL_BASE_ADDR+0) == 0xa5 && octeon_read64_byte(OCTEON_PAL_BASE_ADDR+1) == 0x5a));
}
#endif

#if (CONFIG_OCTEON_EBT3000 || CONFIG_OCTEON_EBH3000 || CONFIG_OCTEON_EBH3100 || CONFIG_OCTEON_CN3010_EVB_HS5 || CONFIG_OCTEON_CN3020_EVB_HS5)
int octeon_mcu_read_cpu_ref(void)
{

    if (twsii_mcu_read(0x00)==0xa5 && twsii_mcu_read(0x01)==0x5a)
        return(((twsii_mcu_read(6)<<8)+twsii_mcu_read(7))/(8));
    else
        return(-1);

}

int octeon_mcu_read_ddr_clock(void)
{
#ifdef OCTEON_PAL_BASE_ADDR
    if (twsii_mcu_read(0x00)==0xa5 && twsii_mcu_read(0x01)==0x5a)
        return((((twsii_mcu_read(4)<<8)+twsii_mcu_read(5))/(1+octeon_read64_byte(OCTEON_PAL_BASE_ADDR+7))));
    else
        return(-1);
#else
	return -1;
#endif
}
#endif


void octeon_free_tmp_named_blocks(cvmx_bootmem_desc_t *bootmem_desc_ptr)
{
    int i;

    cvmx_bootmem_named_block_desc_t *named_block_ptr = (cvmx_bootmem_named_block_desc_t *)(uint32_t)bootmem_desc_ptr->named_block_array_addr;
    for (i = 0; i < bootmem_desc_ptr->named_block_num_blocks; i++)
    {
        if (named_block_ptr[i].size && !strncmp(OCTEON_BOOTLOADER_NAMED_BLOCK_TMP_PREFIX,named_block_ptr[i].name, strlen(OCTEON_BOOTLOADER_NAMED_BLOCK_TMP_PREFIX)))
        {
            /* Found temp block, so free */
            octeon_phy_mem_named_block_free(bootmem_desc_ptr, named_block_ptr[i].name);
        }

    }

}
