/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003  Wolfgang Denk <wd@denx.de>
 *  Copyright 2004, 2005 Cavium Networks.
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


#define COP0_HWRENA_REG $7,0
#define COP0_COUNT_REG $9,0
#define COP0_CVMCOUNT_REG $9,6
#define COP0_CVMCTL_REG $9,7
#define COP0_COMPARE_REG $11,0
#define COP0_CVMMEMCTL_REG $11,7
#define COP0_STATUS_REG $12,0
#define COP0_CAUSE_REG $13,0
#define COP0_PAGEGRAIN_REG $5,1
#define COP0_ENTRYLO0_REG $2,0
#define COP0_ENTRYLO1_REG $3,0
#define COP0_PAGEMASK_REG $5,0
#define COP0_ENTRYHI_REG $10,0
#define COP0_INDEX_REG $0,0
#define COP0_CONFIG1_REG $16,1
#define COP0_CONTEXT_REG $4,0
#define COP0_XCONTEXT_REG $20,0
#define COP0_WIRED_REG $6,0
#define COP0_EPC_REG $14,0
#define COP0_ERROREPC_REG $30,0
#define COP0_BADVADDR_REG $8,0
#define COP0_DEPC_REG $24,0
#define COP0_DEBUG_REG $23,0
#define COP0_MDEBUG_REG $22,0
#define COP0_DESAVE_REG $31,0
#define COP0_PERF_CNT0 $25,1
#define COP0_PERF_CNT1 $25,3
#define COP0_EBASE $15,1
#define COP0_PROC_ID $15,0

#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>

#include "octeon_mem_map.h"
#define  OCTEON_GPIO_RX_DAT                                     0x8001070000000880

#define RVECENT(f,n) \
   b f; nop
#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev

        .set noreorder
    
#ifdef CONFIG_OCTEON
#undef mips64
#endif

        .globl _start
        .text
_start:
        RVECENT(reset,0)        /* U-boot entry point */
        RVECENT(reset,1)        /* software reboot */
#if defined(CONFIG_INCA_IP)
        .word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
        .word 0x00000000           /* phase of the flash                    */
#elif defined(CONFIG_PURPLE)
        .word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
        .word INFINEON_EBU_BOOTCFG /* EBU init code, fetched during booting */
#else
        RVECENT(romReserved,2)
#endif
        RVECENT(romReserved,3)
        RVECENT(romReserved,4)
        RVECENT(romReserved,5)
        RVECENT(romReserved,6)
        RVECENT(romReserved,7)
        RVECENT(romReserved,8)
        RVECENT(romReserved,9)
        RVECENT(romReserved,10)
        RVECENT(romReserved,11)
        RVECENT(romReserved,12)
        RVECENT(romReserved,13)
        RVECENT(romReserved,14)
        RVECENT(romReserved,15)
        RVECENT(romReserved,16)
        RVECENT(romReserved,17)
        RVECENT(romReserved,18)
        RVECENT(romReserved,19)
        RVECENT(romReserved,20)
        RVECENT(romReserved,21)
        RVECENT(romReserved,22)
        RVECENT(romReserved,23)
        RVECENT(romReserved,24)
        RVECENT(romReserved,25)
        RVECENT(romReserved,26)
        RVECENT(romReserved,27)
        RVECENT(romReserved,28)
        RVECENT(romReserved,29)
        RVECENT(romReserved,30)
        RVECENT(romReserved,31)
        RVECENT(romReserved,32)
        RVECENT(romReserved,33)
        RVECENT(romReserved,34)
        RVECENT(romReserved,35)
        RVECENT(romReserved,36)
        RVECENT(romReserved,37)
        RVECENT(romReserved,38)
        RVECENT(romReserved,39)
        RVECENT(romReserved,40)
        RVECENT(romReserved,41)
        RVECENT(romReserved,42)
        RVECENT(romReserved,43)
        RVECENT(romReserved,44)
        RVECENT(romReserved,45)
        RVECENT(romReserved,46)
        RVECENT(romReserved,47)
        RVECENT(romReserved,48)
        RVECENT(romReserved,49)
        RVECENT(romReserved,50)
        RVECENT(romReserved,51)
        RVECENT(romReserved,52)
        RVECENT(romReserved,53)
        RVECENT(romReserved,54)
        RVECENT(romReserved,55)
        RVECENT(romReserved,56)
        RVECENT(romReserved,57)
        RVECENT(romReserved,58)
        RVECENT(romReserved,59)
        RVECENT(romReserved,60)
        RVECENT(romReserved,61)
        RVECENT(romReserved,62)
        RVECENT(romReserved,63)
        XVECENT(romExcHandle,0x200)     /* bfc00200: R4000 tlbmiss vector */
        RVECENT(romReserved,65)
        RVECENT(romReserved,66)
        RVECENT(romReserved,67)
        RVECENT(romReserved,68)
        RVECENT(romReserved,69)
        RVECENT(romReserved,70)
        RVECENT(romReserved,71)
        RVECENT(romReserved,72)
        RVECENT(romReserved,73)
        RVECENT(romReserved,74)
        RVECENT(romReserved,75)
        RVECENT(romReserved,76)
        RVECENT(romReserved,77)
        RVECENT(romReserved,78)
        RVECENT(romReserved,79)
        XVECENT(romExcHandle,0x280)     /* bfc00280: R4000 xtlbmiss vector */
        RVECENT(romReserved,81)
        RVECENT(romReserved,82)
        RVECENT(romReserved,83)
        RVECENT(romReserved,84)
        RVECENT(romReserved,85)
        RVECENT(romReserved,86)
        RVECENT(romReserved,87)
        RVECENT(romReserved,88)
        RVECENT(romReserved,89)
        RVECENT(romReserved,90)
        RVECENT(romReserved,91)
        RVECENT(romReserved,92)
        RVECENT(romReserved,93)
        RVECENT(romReserved,94)
        RVECENT(romReserved,95)
        XVECENT(romExcHandle,0x300)     /* bfc00300: R4000 cache vector */
        RVECENT(romReserved,97)
        RVECENT(romReserved,98)
        RVECENT(romReserved,99)
        RVECENT(romReserved,100)
        RVECENT(romReserved,101)
        RVECENT(romReserved,102)
        RVECENT(romReserved,103)
        RVECENT(romReserved,104)
        RVECENT(romReserved,105)
        RVECENT(romReserved,106)
        RVECENT(romReserved,107)
        RVECENT(romReserved,108)
        RVECENT(romReserved,109)
        RVECENT(romReserved,110)
        RVECENT(romReserved,111)
        XVECENT(romExcHandle,0x380)     /* bfc00380: R4000 general vector */
        RVECENT(romReserved,113)
        RVECENT(romReserved,114)
        RVECENT(romReserved,115)
        RVECENT(romReserved,116)
        RVECENT(romReserved,116)
        RVECENT(romReserved,118)
        RVECENT(romReserved,119)
        RVECENT(romReserved,120)
        RVECENT(romReserved,121)
        RVECENT(romReserved,122)
        RVECENT(romReserved,123)
        RVECENT(romReserved,124)
        RVECENT(romReserved,125)
        RVECENT(romReserved,126)
        RVECENT(romReserved,127)
        XVECENT(romExcHandle,0x400)     /* bfc00400: */
        RVECENT(romReserved,129)
        RVECENT(romReserved,130)
        RVECENT(romReserved,131)
        RVECENT(romReserved,132)
        RVECENT(romReserved,133)
        RVECENT(romReserved,134)
        RVECENT(romReserved,135)
        RVECENT(romReserved,136)
        RVECENT(romReserved,137)
        RVECENT(romReserved,138)
        RVECENT(romReserved,139)
        RVECENT(romReserved,140)
        RVECENT(romReserved,141)
        RVECENT(romReserved,142)
        RVECENT(romReserved,143)
        RVECENT(debugHandler,0x480)     /* bfc00480:  Debug vector*/
        RVECENT(romReserved,145)
        RVECENT(romReserved,146)
        RVECENT(romReserved,147)
        RVECENT(romReserved,148)
        RVECENT(romReserved,149)
        RVECENT(romReserved,150)
        RVECENT(romReserved,151)
        RVECENT(romReserved,152)
        RVECENT(romReserved,153)
        RVECENT(romReserved,154)
        RVECENT(romReserved,155)
        RVECENT(romReserved,156)
        RVECENT(romReserved,157)
        RVECENT(romReserved,158)
        RVECENT(romReserved,159)
        /* Reserve extra space so that when we use the boot bus local memory
        ** segment to remap the debug exception vector we don't overwrite
        ** anything useful */
    
    
        .align 8
reset:
        nop

        /* In PCI the code before InitTLBStart is only executed on core 0.
        ** All other cores branch from the boot bus local memory section
        ** to the InitTLBStart in the relocated u-boot image.
        */
        mfc0    t1, COP0_STATUS_REG
        li      v0,0xE0  // enable 64 bit mode for CSR access
        or      v0, v0, t1
        mtc0    v0, COP0_STATUS_REG


#if CONFIG_OCTEON

#if (CONFIG_OCTEON_FAILSAFE)
        /* Check to see if we should boot the failsafe bootloader image
        ** or the normal one.  The failsafe image is loaded at the reset vector
        ** and the normal one at CFG_NORMAL_BOOTLOADER_BASE  Which image to boot to is
        ** based on GPIO bit 0.
        ** The failsafe image is only intended to be used for recovering from
        ** a failed bootloader update.
        */

        /* Branch to normal bootlaoder image at CFG_NORMAL_BOOTLOADER_BASE if GPIO bit 0 is 0, otherwise  boot this image */
        dli     t0, OCTEON_GPIO_RX_DAT
        ld      t1, 0(t0)
        andi    t1, 0x1
        bne     zero, t1, boot_cont
        nop
        li      t1, CFG_NORMAL_BOOTLOADER_BASE  # jump to normal bootloader image
        j       t1
        nop
boot_cont:
#endif




/* Start of Octeon setup */

#if !(CONFIG_OCTEON_SIM)


        /* Workaround for LLM bug where resetting
        ** the chip during LLM activity causes problems.
        ** Fix is to do another reset.  Note that this workaround
        ** does not handle the case where the user application
        ** does a soft reset during LLM activity.
        */
        mfc0    a0, COP0_EBASE
        andi    a0, 0xFF   // get core
        bne     a0, zero, llm_workaround_end
        nop

        /* Check chip revision, and skip PCIX workaround if we are not on pass 1 */
        mfc0    t0, COP0_PROC_ID
        andi    t0, 0xff
        bnez    t0, pcix_workaround_end
        nop

        /* Only core 0 executes this, non-zero cores
        ** skip to llm_workaround_end */
#if defined(CONFIG_OCTEON_PCIX_WORKAROUND)
        /* Set valid value for PCI_CTL_STATUS_2[TSR_HWM] */
        dli     t1, 0x80011F0000001188 
        dli     t0, (1<<4)
        sw      t0, 0(t1)
#endif
pcix_workaround_end:

#if defined(CONFIG_OCTEON_LLM_WORKAROUND) && !(CONFIG_RAM_RESIDENT)
        /* Check to see if this is a soft reset, if
        ** not reset again to make sure LLM interface is
        ** quiet during reset. */
        mfc0    v0, COP0_STATUS_REG
        srl     v0, 20
        andi    v0, 1
        bne     v0, zero, llm_workaround_end  /* soft reset, so just continue */
        nop
        /* This was a hard reset, so we will do a soft reset here */
        dli     t0, 0x8001070000000740
        li      t1, 1
        sd      t1, 0(t0)

        /* Loop forever after issuing soft reset */ 
llm_workaround_loop:
        j       llm_workaround_loop
        nop
#endif

llm_workaround_end:    
#endif

#ifdef CONFIG_ARUBA_OCTEON
#if 0
	//
	// disable any residual watchdog
	// interrupts until we get settled
	//
	//
        dli     t0, 0x8001070000000500
        li      t1, 0
        sd      t1, 0(t0)
#endif
	//
	// Check to see if this is an NMI, if
	// so, soft reset again to avoid weirdness
	//
        mfc0    v0, COP0_STATUS_REG
        srl     v0, 19
        andi    v0, 1
        beqz    v0, not_nmi
        nop

        dli     t0, 0x8001070000000740
        li      t1, 1
        sd      t1, 0(t0)

	// we don't get here
not_nmi:
#endif

    /* Check what core we are - if core 0, branch to init tlb
    ** loop in flash.  Otherwise, look up address of init tlb
    ** loop that was saved in the boot vector block.
    */
        mfc0    a0, COP0_EBASE
        andi    a0, 0xFF   // get core
#if 0
        /* This code is used to generate the stub that is loaded
        ** by the oct-pci-boot program */
        li      t1, 0x100000
        bnez     a0, later_cores_ram_boot
        nop
        j       t1
        nop
later_cores_ram_boot:         
#else
        beqz    a0, InitTLBStart_local
#endif        
        // other cores look up addr from dram, and will jump to run
        // out of DRAM rather than flash from this point
        li      a1, BOOT_VECTOR_NUM_WORDS
        sll     a1, 2   // convert to word addresses
        mul     a0, a0, a1
        li      a1, BOOT_VECTOR_BASE
        add     a1, a1, a0
        
    /* Get execution address and separate stack from boot vector.
    ** The address will point to the init TLB loop below, but
    ** will be in DRAM rather than flash.
    */
        lw      sp, 0(a1)
        lw      a0, 4(a1)
        j       a0
        nop
        
        .globl InitTLBStart
InitTLBStart:
InitTLBStart_local:
        /* This code run on all cores - core 0 from flash,
    ** the rest from DRAM.  When booting from PCI, non-zero cores
    ** come directly here from the boot vector - no earlier code in this
    ** file is executed.
    */

    /* Some generic initialization is done here as well, as we need this done on
    ** all cores even when booting from PCI
    */
        /* Clear watch registers.
         */
        mtc0    zero, CP0_WATCHLO
        mtc0    zero, CP0_WATCHHI

        /* STATUS register */
        mfc0    k0, CP0_STATUS
        li      k1, ~ST0_IE
        and     k0, k1
        mtc0    k0, CP0_STATUS

        /* CAUSE register */
        mtc0    zero, CP0_CAUSE

        /* Init Timer */
        dmtc0   zero, CP0_COUNT
        dmtc0   zero, CP0_COMPARE


        mfc0    t1, COP0_STATUS_REG
        li      v0,0xE0  // enable 64 bit mode for CSR access
        or      v0, v0, t1
        mtc0    v0, COP0_STATUS_REG

    
        dli     v0, 1 << 29  /* Enable large physical address support in TLB */
        mtc0    v0, COP0_PAGEGRAIN_REG

InitTLB:
        dmtc0   zero, COP0_ENTRYLO0_REG
        mfc0    a0, COP0_CONFIG1_REG
        dmtc0   zero, COP0_ENTRYLO1_REG
        srl     a0, a0, 25
        mtc0    zero, COP0_PAGEMASK_REG
        dli     v0, 0xFFFFFFFF90000000 // Use an offset into kseg0 so we won't conflict with Mips1 legacy TLB clearing
        and     a0, a0, 0x3F // a0 now has the max mmu entry index
        dmtc0   zero, COP0_CONTEXT_REG
        dmtc0   zero, COP0_XCONTEXT_REG
        mtc0    zero, COP0_WIRED_REG
InitTLBloop:
        dmtc0   v0, COP0_ENTRYHI_REG
        nop
        nop
        tlbp
        nop
        nop
        mfc0    v1, COP0_INDEX_REG
        daddiu  v0, v0, 1<<13
        bgez    v1, InitTLBloop
        nop
        nop

        mtc0    a0, COP0_INDEX_REG
        nop
        nop
        tlbwi
        nop
        nop
        bne     a0, zero, InitTLBloop
        addiu   a0, -1

        mthi    zero
        mtlo    zero

        /* Set up status register */        
        mfc0    v0, COP0_STATUS_REG
        li      t0, 1 << 28  // enable cop0 access
        or      v0, t0
        li      t0, 1 << 30  // enable cop2 access
        or      v0, t0

        /* Must leave BEV set here, as DRAM is not configured for core 0.
        ** Also, BEV must be 1 later on when the exception base address is set. */

        li      t0, ~0xff00 // mask all interrupts
        and     v0, t0
        li      t0, ~0xff 
        and     v0, t0
        ori     v0, 0xE5  // enable 64 bit, disable interrupts 
        mtc0    v0, COP0_STATUS_REG


        dli     v0,0xC000000F // enable all readhw locations
        mtc0    v0, COP0_HWRENA_REG
        
        dmfc0   v0, COP0_CVMCTL_REG
        mfc0    t0, COP0_PROC_ID
        andi    t0, 0xff
        bnez    t0, skip_icachetch_disable
        nop
        ori     v0, 1<<13   // disable icache prefectch - errata core 8 (pass1 only)
skip_icachetch_disable:
        ori     v0, 1<<14   // enable fixup of unaligned mem access
        dmtc0   v0, COP0_CVMCTL_REG

        /* Setup scratch memory.  This is also done in
        ** cvmx_user_app_init, and this code will be removed
        ** from the bootloader in the near future.
        */
        dmfc0   v0, COP0_CVMMEMCTL_REG
        ori     v0, 0x104   // setup 4 lines of scratch
        dmtc0   v0, COP0_CVMMEMCTL_REG
        

        // clear these to avoid immediate interrupt in noperf mode
        dmtc0   zero, COP0_COMPARE_REG            // clear timer interrupt
        dmtc0   zero, COP0_COUNT_REG            // clear timer interrupt
        dmtc0   zero, COP0_PERF_CNT0              // clear perfCnt0
        dmtc0   zero, COP0_PERF_CNT1              // clear perfCnt1

        /* Check if we are core 0, if we are not then we need
        ** to vector to code in DRAM to do application setup, and
        ** skip the rest of the bootloader.  Only core 0 runs the bootloader
        ** and sets up the tables that the other cores will use for configuration
        */
        mfc0    a0, COP0_EBASE
        andi    a0, 0xFF   // get core
        beqz    a0, core_0_cont1
        nop
        // other cores look up addr from dram
        // DRAM controller already set up by first core
        li      a1, BOOT_VECTOR_NUM_WORDS
        sll     a1, 2   // convert to word addresses
        mul     a0, a0, a1
        li      a1, BOOT_VECTOR_BASE
        add     a1, a1, a0

        /* No cache init required */
        lw      t9, 8(a1)
        lw      k0, 12(a1)
        j       t9
        nop
core_0_cont1:
#endif        


    /* From here on, only core 0 runs, other cores have branched
    ** away.
    */

#if (CONFIG_OCTEON && !CONFIG_NO_RELOCATION)
        /* If we don't have working memory yet configure a bunch of
	** scratch memory, and set the stack pointer to the top
        ** of it.  This allows us to go to C code without having
	** memory set up
        */
#define STACK_LINES 0x36   /* MAX is 0x36 */
        dmfc0   v0, COP0_CVMMEMCTL_REG
        srl     v0, 9
        sll     v0, 9
        ori     v0, 0x100 | STACK_LINES   // setup STACK_LINES scratch lines of scratch
        dmtc0   v0, COP0_CVMMEMCTL_REG
        li      sp, 0xffff8000 + (STACK_LINES * 128)   // set stack to top of scratch memory
#else
        li      t0, CFG_SDRAM_BASE + CFG_INIT_SP_OFFSET
        la      sp, 0(t0)
#endif



        /* Initialize GOT pointer.
        ** Can't resolve any symbols before this is done.  Note that this
        ** points to the GOT that is in flash(or wherever the bootloader
        ** was first loaded.  A 'relocated' gp must be used to reference symbols
        ** in the code relocated to DRAM.
         */
       bal     1f
       nop
       .extern _GLOBAL_OFFSET_TABLE_
       .word   _GLOBAL_OFFSET_TABLE_
       .word   . - 4
1:
       move    gp, ra
       lw      t1, 0(ra)
       lw      t2, 4(ra)
       sub     t1, t2
       add     gp, t1  # calculate current location of offset table


        
        
#ifndef CONFIG_OCTEON
        /* must not use .cprestore here, don't know how to avoid warning */
        /* Initialize any external memory.
         */
        jal     memsetup
        nop
        /* Initialize caches...
         */
        sync
        cache 0, 0($0)
        sync
        cache 9, 0($0)
        sync
        
        jal     mips_cache_reset
        nop

        /* ... and enable them.
         */
        li      t0, CONF_CM_CACHABLE_NONCOHERENT
        mtc0    t0, CP0_CONFIG


        /* Set up temporary stack.
         */
        li      a0, CFG_INIT_SP_OFFSET
        jal     mips_cache_lock
        nop
#endif

        
    
        la      t9, board_init_f  /* doesn't return... */
        j       t9
        nop


/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 * a0 = addr_sp
 * a1 = gd
 * a2 = destination address
 */
        .globl  relocate_code
        .ent    relocate_code
relocate_code:
        move    sp, a0          /* Set new stack pointer                */

        li      t0, CFG_MONITOR_BASE /* Text base, 0xBFC00000 */
        la      t3, in_ram
        lw      t2, -12(t3)     /* t2 <-- uboot_end_data        */
        move    t1, a2

        /*
         * Fix GOT pointer:
         *
         * New GOT-PTR = (old GOT-PTR - CFG_MONITOR_BASE) + Destination Address
         */
        move    t6, gp
        sub     gp, CFG_MONITOR_BASE  
        add     gp, a2                  /* gp now adjusted              */
        sub     t6, gp, t6              /* t6 <-- relocation offset     */

        /*
         * t0 = source address
         * t1 = target address
         * t2 = source end address
         */
        /* On the purple board we copy the code earlier in a special way
         * in order to solve flash problems
         */
#ifndef CONFIG_PURPLE

#ifndef CONFIG_OCTEON
1:
        lw      t3, 0(t0)
        sw      t3, 0(t1)
        addu    t0, 4
        ble     t0, t2, 1b
        addu    t1, 4                   /* delay slot                   */
#else
/* Use 64 bit copies to relocate code - much faster on simulator 
** src/dest are 8 byte aligned.  might copy 4 bytes too much, but we don't care  */
/* don't relocate if dest/src already equal.  This condition is set by board_init_f
** to avoid relocation when loaded directly into RAM.  */
        beq     t0, t1, copyDone
1:
        ld      t3, 0(t0)
        sd      t3, 0(t1)
        addu    t0, 8
        ble     t0, t2, 1b
        addu    t1, 8                   /* delay slot                   */
#endif
    
#endif

        /* If caches were enabled, we would have to flush them here.
         */
copyDone:
        /* Jump to where we've relocated ourselves.
         */
        addi    t0, a2, in_ram - _start
    	synci 	0($0)  /* Flush icache (only required when relocating from ddr->ddr, as in pci boot */
        j       t0
        nop

        .word   uboot_end_data
        .word   uboot_end
        .word   num_got_entries

in_ram:
        /* Now we want to update GOT.
         */
        lw      t3, -4(t0)      /* t3 <-- num_got_entries       */
        addi    t4, gp, 8       /* Skipping first two entries.  */
        li      t2, 2
1:
        lw      t1, 0(t4)
        beqz    t1, 2f
        add     t1, t6
        sw      t1, 0(t4)
2:
        addi    t2, 1
        blt     t2, t3, 1b
        addi    t4, 4           /* delay slot                   */

        /* Clear BSS.
         */
        lw      t1, -12(t0)     /* t1 <-- uboot_end_data        */
        lw      t2, -8(t0)      /* t2 <-- uboot_end             */
        add     t1, t6          /* adjust pointers              */
        add     t2, t6

        sub     t1, 4
1:      addi    t1, 4
        bltl    t1, t2, 1b
        sw      zero, 0(t1)     /* delay slot                   */



        dmfc0   a0, COP0_CVMMEMCTL_REG
        srl     a0, 9
        sll     a0, 9
        ori     a0, 0x104   // setup 4 scratch lines of scratch
        dmtc0   a0, COP0_CVMMEMCTL_REG


        move    a0, a1
        la      t9, board_init_r  /* doesn't return, runs main_loop() */
        j       t9
        move    a1, a2          /* delay slot                   */

        .end    relocate_code


        /* Exception handlers.
         */
romReserved:
        b romReserved
        nop

romExcHandle:

#if CONFIG_OCTEON_SIM
        break
#endif
#if CONFIG_OCTEON_KODAMA
#define LED_REG_VAL     0x80001a00
#define LED_BASE_ADDR   0x9a000000
#else
#define LED_REG_VAL     0x80001d02
#define LED_BASE_ADDR   0x9d020000
#endif

#ifndef CONFIG_ARUBA_OCTEON
        /* Print "TRAP" on rev 2 and later boards */

    /* Setup region for 4 char LED display */
        /* Map LED display */
        dli t0, 0x8001180000000020
        dli  t1, LED_REG_VAL
        sd  t1, 0(t0); 

        /* Write string */
        li t0, LED_BASE_ADDR   /* Base address of LED */
        li t1, 'T'
        sb t1, 0xf8(t0)
        li t1, 'R'
        sb t1, 0xf9(t0)
        li t1, 'A'
        sb t1, 0xfa(t0)
        li t1, 'P'
        sb t1, 0xfb(t0)
#endif

rmExcLoop:
        b rmExcLoop
        nop
    
#define COP0_DESAVE_REG $31,0
debugHandler:
        .globl debugHandler_entrypoint
        .align 8  /* Copied 8 bytes at a time, so make sure aligned */
debugHandler_entrypoint:	
        dmtc0   k0, COP0_DESAVE_REG
        li      k0, 0x80000000
        lw      k0, BOOTLOADER_DEBUG_TRAMPOLINE(k0)
        j       k0
        nop
    
        .globl OcteonBreak
OcteonBreak:
        break
        break
