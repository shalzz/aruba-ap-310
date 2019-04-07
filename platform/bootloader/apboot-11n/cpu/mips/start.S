/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
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


#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>

#include "nebbiolo/asm.h"
#include "nebbiolo/bridge.h"
#include "nebbiolo/on_chip.h"
#include "nebbiolo/mem_ctrl.h"
#include "nebbiolo/cache.h"
#include "nebbiolo/uart.h"
#include "nebbiolo/gpio.h"
#include "nebbiolo/shared_structs_offsets.h"

#define BOOT1_5_LINE_SIZE	0x800      
#define KSEG0_IMG_LOC		0xffffffff8c000000
#define MSGRNG_MSG_STATUS_REG $2
#define MSGRNG_CODE_BOOT_WAKEUP 200
#define CPU_BITMAP 0x00

#define MSGRNG_TX_BUF_REG $0
#define MSGRNG_RX_BUF_REG $1

#define CPU_BITMAP 0x00

    .option pic2
.macro phoenix_cpu_id id, temp0
    /* Read the cpu id from the cp0 config register
     * cpuid[9:4], thrid[3: 0]
     */
    /* get the cpu id */
    mfc0   \id, CP0_CONFIG, 7
    srl    \id, \id, 4 
    andi   \id, \id, 0x3f 
    sll    \id, \id, 2
    /* get the thread id */
    mfc0   \temp0, CP0_CONFIG, 7 
    andi   \temp0, 0x0f
    /* calculate the linear cpu id */
    addu   \id, \id, \temp0
.endm
    
#define RVECENT(f,n) \
   b f; nop
#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev

	.set noreorder
    .align 2

	.globl _start
	.text
_start:
	RVECENT(reset,0)	/* U-boot entry point */
	RVECENT(reset,1)	/* software reboot */
	RVECENT(romReserved,2)
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
	XVECENT(romExcHandle,0x200)	/* bfc00200: R4000 tlbmiss vector */
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
	XVECENT(romExcHandle,0x280)	/* bfc00280: R4000 xtlbmiss vector */
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
	XVECENT(romExcHandle,0x300)	/* bfc00300: R4000 cache vector */
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
	XVECENT(romExcHandle,0x380)	/* bfc00380: R4000 general vector */
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

	/* We hope there are no more reserved vectors!
	 * 128 * 8 == 1024 == 0x400
	 * so this is address R_VEC+0x400 == 0xbfc00400
	 */
	.align 4
reset:
#if 0  // Something here fucks with the cache and stack-on-cache fails later on...
	/* Clear watch registers.
	 */
	mtc0	zero, CP0_WATCHLO
	mtc0	zero, CP0_WATCHHI

	/* STATUS register */
#ifdef  CONFIG_TB0229
	li	k0, ST0_CU0
#else
	mfc0	k0, CP0_STATUS
#endif
	li	k1, ~ST0_IE
	and	k0, k1
	mtc0	k0, CP0_STATUS

	/* CAUSE register */
	mtc0	zero, CP0_CAUSE

	/* Init Timer */
	mtc0	zero, CP0_COUNT
	mtc0	zero, CP0_COMPARE

	/* CONFIG0 register */
	li	t0, CONF_CM_UNCACHED
	mtc0	t0, CP0_CONFIG
#endif // 0

	/* 
     * Initialize GOT pointer.
	 */
	bal     1f
	nop
	.word   _GLOBAL_OFFSET_TABLE_
	1:
	move    gp, ra
	lw      t1, 0(ra)
	move	gp, t1
     
	/* Read the cpu id from the cp0 config register
	 * cpuid[9:4], thrid[3:	0]
	 */	
	mfc0	a0, CP0_CONFIG, 7
	srl	a1, a0, 4
	andi	a1, a1, 0x3f
	andi	a0, a0, 0xf	

	/* calculate linear cpuid */
	sll	t0, a1, 2
	addu	a2, t0, a0

	/* Initialize Status register */
	mfc0	t0, CP0_STATUS
	li	t1, 0xffa7ffff
	nor	t1, t1, t1
	and	t0, t0, t1	//	li	t1, 0x500000e0
	li	t1, 0x500000c0
	or	t0, t0, t1
	mtc0	t0, CP0_STATUS
    
	/* If not master cpu, wait for a message */
	beqz	a2, master_cpu
	nop

slave_cpu:
    la      t9, enable_l1_icache
    jalr    t9
	nop
	la      t9, enable_l1_dcache
    jalr    t9
	nop
	la      t9, enable_cpu_l2_cache
    jalr    t9
	nop
	sync
	la      t9, enable_all_threads
    jalr    t9
	nop
	
	/* update the cpu online map */
	la      t9, boot2_update_cpu_online_map
    jalr    t9
	nop	
	
    la  t9, flush_l1_caches
    //li      t0, -0x20000000
    //add     t9, t9, t0
    jalr    t9
	nop

	/* wait for a message in bucket# == thrid */
	mfc0	a0, CP0_CONFIG, 7
	andi	a0, a0, 0x0f
    
    la      t9, slave_in_kseg0
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr    t9

slave_in_kseg0:	
	li	t1, 1
	sllv	t1, t1, a0
1:	msgwait	t1
	msgld	a0
	/* wait for pending to clear */
2:	mfc2	t2, MSGRNG_MSG_STATUS_REG, 0
	andi	t3, t2, 0x08
	bnez	t3, 2b
	nop
	/* check for empty bucket and pending fail */
	andi	t3, t2, 0x30
	bnez	t3, 1b
	nop
	/* Check for SW code */
	srl	t3, t2, 8
	andi	t3, 0xff
	li	t4, MSGRNG_CODE_BOOT_WAKEUP
	bne	t3, t4, 1b
	nop
	/* check for size */
	srl	t3, t2, 6
	andi	t3, 0x03
	li	t4, 3
	bne	t3, t4, 1b
	nop
    
	/* msg0=sp, msg1=gp, msg2=fn msg3=args */
	dmfc2	sp, MSGRNG_RX_BUF_REG, 0
	dmfc2	gp, MSGRNG_RX_BUF_REG, 1
	dmfc2	a0, MSGRNG_RX_BUF_REG, 2
	dmfc2	a1, MSGRNG_RX_BUF_REG, 3
 
        /* Clear BEV */
        mfc0    t0, CP0_STATUS
        li      t1, 0x00400000
        nor     t1, t1, t1
        and     t0, t0, t1
        mtc0    t0, CP0_STATUS
 
	move   t9, a0
	jr	a0
	nop
	/* should never come here */
	la	t0, halt
	jalr	t0
	nop
	
  
master_cpu:
	/* Enable AERR collection */
    la      t9, aerr_log
    jalr    t9
	nop
	
	/* Initialize any external memory.
	 */
    la      t9, uart_init
    jalr    t9
    nop
    PRINT("\nU-Boot - Starting up...\n")
	//la      t9, lowlevel_init
	//jalr    t9
	//nop

	/* Get the BIST Stats */
	jal	gpio_dump_bist	
	nop

	//PRINT("Initialize caches...\n")
	/* Initialize caches...
	 */
	//PRINT("Enabling L1I Cache...\n") ; 
    la      t9, enable_l1_icache
    jalr    t9
	nop

	//PRINT("Enabling L1D Cache...\n")  
    la      t9, enable_l1_dcache
    jalr    t9
	nop

    //PRINT("Enabling L2 Caches... without ecc\n")
    la      t9, enable_l2_cache_no_ecc
    jalr    t9
    nop

    //PRINT("Enabling L2 allocate...\n")  
    la      t9, enable_cpu_l2_cache
    jalr    t9
    nop
    sync

	/* Dummy BAR to cover address space locked in L2 */
    li      t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
    li      t1, ((0x0c<<16)|(0x0<<4)|(0<<1)|0x01)	
    sw      t1, (BRIDGE_DRAM_7_BAR << 2)(t0)

	//PRINT("Locking some cache lines into L2...\n")
	/* locking temporary stack space */
	li	s6, BOOT1_5_LINE_SIZE
 	dli 	s7, KSEG0_IMG_LOC
    
	//PRINTF("Lines locked into L2 at address: %x\n", s7)
	//PRINTF("# of lines: %x\n", s6)

lock_l2_loop2:

    la      t9, gpio_get_procid
    jalr    t9
    nop
    
	move 	a0, s7	
	la 	    t9, WRITE_L2_LDCBITS	/* Locking cache line a0 in l2 */
	jalr 	t9
	nop        
	li 	t0, 32			/* Cache line size is 32 bytes */ 
	daddu  	s7, s7, t0		
	addi    s6, -1			
	blez    s6, load_boot1_5
	nop
	b 	lock_l2_loop2
	nop

load_boot1_5:	
	li	s6, BOOT1_5_LINE_SIZE
	dli     s7, KSEG0_IMG_LOC
    
check_l2_tag2:
    jal gpio_get_procid
    nop
    
	move    a0, s7
	la      t9, CHECK_LOCK		/* Checking cache line a0 is locked in l2 */
	jalr    t9
	nop
	li 	t0, 32			/* Cache line size is 32 bytes */
	daddu  	s7, s7, t0		
	addi    s6, -1
 	blez    s6, exec_spd
	nop
	b 	check_l2_tag2
	nop  
    
exec_spd:
	/* halting if check_lock fails */
    //beqz   v0, 1f
	//nop

	//PRINT("boot2_update_psb_info\n")
	jal	boot2_update_psb_info
	nop

    li t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
    sw s0, (BRIDGE_SCRATCH0 << 2)(t0)
                
	//PRINT("Set up temporary stack.\n")
	/* Set up temporary stack.
	 */
	//li	   a0, CFG_INIT_SP_OFFSET

	//li	t0, CFG_SDRAM_BASE + CFG_INIT_SP_OFFSET
	//la	sp, 0(t0)
	li	t0, 0x8c010000
	la	sp, 0(t0)

	//PRINT("Jump to C code in Flash.\n")
 	la	t9, board_init_f
	jalr	t9
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
	.globl	relocate_code
	.ent	relocate_code
relocate_code:
	la	sp, 0(a0)		/* Set new stack pointer		*/
    move    s7, a2
    move    s6, a1
    move    s5, a0
    
/*******************************************************/    
	//PRINT("returned to flash...\n")
	//PRINT("boot2_set_bev\n")
	la	t9, boot2_set_bev
	jalr	t9
	nop
	//PRINT("flush_l1_caches\n")
	la	t9, flush_l1_caches
	jalr	t9
	nop
	//PRINT("boot2_clear_bev\n")
	la	t9, boot2_clear_bev
	jalr	t9
	nop
	//PRINT("invalidate_l2_cache\n")
	la	t9, invalidate_l2_cache
    li      t0, -0x20000000
    add     t9, t9, t0
	jalr	t9
	nop
	//PRINT("Erasing dummy DRAM BAR, activating actual ones\n");
	la	t9, dram_bar_postprocessing
    li      t0, -0x20000000
    add     t9, t9, t0
	jalr	t9
	nop
	nop	 
	//PRINT("Size-based DRAM scrub prior to ECC enabling\n") 
	la	t9, dram_scrub
    li      t0, -0x20000000
    add     t9, t9, t0
	jalr	t9
	nop
	//PRINT("dram_ecc_enable\n") 
	la	t9, dram_ecc_enable
    li      t0, -0x20000000
    add     t9, t9, t0
	jalr	t9
	nop
        
    li      a1, 0x00400000  // range
    li      a0, 0x80000000  // starting address
	la	    t9, dram_mats
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
    nop
	
	la	    t9, dram_scrub
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
    nop

	/* enabling ecc for l2 cache */
	la	    t9, enable_l2_cache
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
	nop
		
	/* initialize and update cpu online map */
	la	    t9, boot2_update_cpu_online_map
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
	nop
	
	/* slave cpus will be in reset till this point */
	la	    t9, gpio_wakeup_slave_cpus
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
	nop
    
	/* enable all threads in this core */
	la	    t9, enable_all_threads
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
	nop
   
	la	    t9, boot2_update_psb_info
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr t9
	nop
    
    li  v1, 0x0
    li  t0, BOOT1_INFO
    sd  v1, boot1_info_board_version_off(t0)
    
	/* Major Board version */
    li  v1, 0x4
	li	t0, BOOT1_INFO
	sd	v1, boot1_info_board_major_version_off(t0)

	//PRINTF("Nebbiolo Board Major Version:	 %X\n", v1)
	
	/* Minor Board version */
    li  v1, 0x0
	li	t0, BOOT1_INFO
	sd	v1, boot1_info_board_minor_version_off(t0)

	//PRINTF("Nebbiolo Board Minor Version:	 %X\n", v1)

	/* Board manf version char0*/
    li  v1, 0x0
	li	t0, BOOT1_INFO
	sb	v1, boot1_info_board_manf_revision_off(t0)

	//PRINTF("Nebbiolo Board Manf Revision, char0:  %c\n", v1)
	
	/* Board manf version char1*/
    li  v1, 0x0
	li	t0, BOOT1_INFO
	sb	v1, (boot1_info_board_manf_revision_off+1)(t0)

	//PRINTF("Nebbiolo Board Manf Revision, char1:  %c\n", v1)
	
	/* Board serial num hi*/
    li  v1, 0x0
	li	t0, BOOT1_INFO
	sb	v1, boot1_info_board_serial_number_off(t0)

	//PRINTF("Nebbiolo Board Serial Num, char0:  %X\n", v1)
	
	/* Board manf version char1*/
    li  v1, 0x0
	li	t0, BOOT1_INFO
	sb	v1, (boot1_info_board_manf_revision_off+1)(t0)

	//PRINTF("Nebbiolo Board Manf Revision, char1:  %X\n", v1)
    
    li  t0, 0xbfe00000      /* load address of CPLD/XPLD rev reg    */
    lw  t1, (t0)            /* read 32bits from CPLD rev reg        */
    li  t0, 0x00ff0000      /* mask for  checking whether this 512 or 1G 
							 * memory parts      */
	and t0, t0, t1
	beqz t0, neb_mac_def
	nop
     /* MAC Address */ 
	li  v1, 0x0000000b
	li	t0, BOOT1_INFO
	sw	v1, boot1_info_mac_addr_off(t0)
    li  v1, 0x86123457      /* load a phony MAC address for Nebbiolo bringup */
	li	t0, BOOT1_INFO
	sw	v1, (boot1_info_mac_addr_off+4)(t0)
    la  t9, neb_mac_done
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr    t9
	//b neb_mac_done
	nop

neb_mac_def:
	li  v1, 0x0000000b
	li	t0, BOOT1_INFO
	sw	v1, boot1_info_mac_addr_off(t0)
    li  v1, 0x86123456      /* load a phony MAC address for Nebbiolo bringup */
	li	t0, BOOT1_INFO
	sw	v1, (boot1_info_mac_addr_off+4)(t0)

neb_mac_done:
	//PRINTF("Nebbiolo phony bringup MAC: %X\n", v1)
    nop

end_i2c_bypass:    
    la  t9, flush_l1_caches
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr    t9
	nop
    
	//PRINT("boot2_clear_bev\n")
    la  t9, boot2_clear_bev
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr    t9
    nop

/*******************************************************/

    move    a2, s7
    move    a1, s6
    move    a0, s5
    
	li	t0, CFG_MONITOR_BASE
	la	t3, in_ram
	lw	t2, -12(t3)	/* t2 <-- uboot_end_data	*/
	move	t1, a2

	/*
	 * Fix GOT pointer:
	 *
	 * New GOT-PTR = (old GOT-PTR - CFG_MONITOR_BASE) + Destination Address
	 */
	move	t6, gp
	sub	gp, CFG_MONITOR_BASE
	add	gp, a2			/* gp now adjusted		*/
	sub	t6, gp, t6		/* t6 <-- relocation offset	*/

	/*
	 * t0 = source address
	 * t1 = target address
	 * t2 = source end address
	 */
1:
	lw	t3, 0(t0)
	sw	t3, 0(t1)
	addu	t0, 4
	ble	t0, t2, 1b
	addu	t1, 4			/* delay slot			*/

	/* If caches were enabled, we would have to flush them here.
	 */
    la  t9, flush_l1_caches
    li      t0, -0x20000000
    add     t9, t9, t0
    jalr    t9
	nop

	/* Jump to where we've relocated ourselves.
	 */
	addi	t0, a2, in_ram - _start
	j	t0
	nop

	.word	uboot_end_data
	.word	uboot_end
	.word	num_got_entries

in_ram:
	/* Now we want to update GOT.
	 */
	lw	t3, -4(t0)	/* t3 <-- num_got_entries	*/
	addi	t4, gp, 8	/* Skipping first two entries.	*/
	li	t2, 2
1:
	lw	t1, 0(t4)
	beqz	t1, 2f
	add	t1, t6
	sw	t1, 0(t4)
2:
	addi	t2, 1
	blt	t2, t3, 1b
	addi	t4, 4		/* delay slot			*/

	/* Clear BSS.
	 */
	lw	t1, -12(t0)	/* t1 <-- uboot_end_data	*/
	lw	t2, -8(t0)	/* t2 <-- uboot_end		*/
	add	t1, t6		/* adjust pointers		*/
	add	t2, t6
    

	sub	t1, 4
1:	addi	t1, 4
	bltl	t1, t2, 1b
	sw	zero, 0(t1)	/* delay slot			*/

   	move	a0, s6
	move	a1, s7	
	move	a2, gp
	la	t9, board_init_r
	j	t9
    nop

	.end	relocate_code


	/* Exception handlers.
	 */
romReserved:
	b romReserved

romExcHandle:
	b romExcHandle
    
    .globl  halt
    .ent    halt
halt:		
	phoenix_cpu_id k0, k1
	
	PRINTF("CPU_%x HALTED\n", k0)	
1:	wait
	b       1b
	nop
    
    .end    halt

    .globl  dram_scrub
    .ent    dram_scrub
dram_scrub:

/* t0 - BAR address */
/* t1 - xkphys base virtual address */
/* t2 - contents of the BAR */
/* t3 - temporary register */
/* t4 - extracted mask bits, number of cache lines */		
/* t5 - extracted virtual address */
/* t6 - BAR read loop counter, BAR offset */
	
	/* Initialize variables */
	dli     t1, 0x9800000000000000	/* CONSTANT - move this later */
	li	t6, 0x20	        /* Start with BAR7 */

	/* Enable 64-bit addressing by activating KX */
	mfc0    t0, CP0_STATUS
	ori     t0, 0x80
	mtc0    t0, CP0_STATUS
	//PRINTF("dram_ecc_scrub - CP0status:%x\n",t0) 
		
1:	/* BAR read loop begin */
	dli     t1, 0x9800000000000000	/* CONSTANT - move this later */
	addi	t6, -4          /* Point to the next BAR */
	bltz	t6, 3f          /* If no more BARs left, exit the loop */
	nop

    li      t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
	addu	t0, t0, t6	/* Calculate the address of the BAR */
	
    lw      t2, 0(t0)      /* Read the BAR */
	dli     t3, 0x0000000000000001
	and     t3, t2, t3     /* Extract the BAR enable bit */
	beqz	t3, 1b	       /* If BAR is not enabled, skip to the next one */
	nop
	dli	t3, 0x00000000ffff0000	
	and	t3, t2, t3     /* Extract the BAR base address field */
	dli	t4, 0x000000000000fff0	
	and	t4, t2, t4     /* Extract the BAR mask bits */
	
	dsll    t5, t4, 20     /* Make shifted copy of mask bits, aligned from 35 to 24 */
	nor	t5, t5, zero   /* Invert the shifted mask */
	dsll    t3, t3, 8      /* Move base address to bits 39 to 24 */	
	and     t5, t5, t3     /* Mask out unwanted base address bits */
	or	t5, t5, t1     /* Convert address to xkphys cached virtual address */
	
	dsrl	t4, t4, 4      /* Move mask to bits 11 to 0 */
	daddiu  t4, t4, 0x1    /* Add 1 */
	dsll    t4, t4, 24     /* Move results to bits 36 to 24 */ 
	dsrl    t4, t4, 5      /* Divide by 32 to get number of cache lines */

	//PRINTF("Scrub region base virtual address  %x \n", t5)	
	//PRINTF("Scrub region size in cache lines   %x \n", t4)

2:	/* Scrub loop begin */
	beqz	t4, 1b         /* If # of unscrubbed cache lines = 0, skip the scrubbing process */
	nop
	pref	Pref_Prep_For_Store, 0(t5)
	
	sd	zero, 0(t5)
	sd	zero, 8(t5)
	sd	zero, 16(t5)
	sd	zero, 24(t5)

	daddiu	t5, 32
	addiu	t4, -1
	b	2b
	nop
	/* Scrub loop end */
        /* BAR read loop end */
	
3:	/* Disable 64-bit addressing by clearing KX */
	//PRINT("dram_ecc_scrub - disable 64bit\n") 
	li	t1, 0xffffff7f
	mfc0    t0, CP0_STATUS
	and     t0, t0, t1
	mtc0    t0, CP0_STATUS

	jalr ra
	nop
	
    .end    dram_scrub

    .globl  dram_ecc_enable
    .ent    dram_ecc_enable
dram_ecc_enable:
	/* enable ecc for dram channels */
	/* ------------------------------------------------------------- */
        /* Note: this function uses the bridge scratch register 1 as an  */
	/* input and assumes that it has been programmed by the SPD code */ 
	/* (not to be confused with OS scratch register 0 )              */
	/* Format: [0]: Set if channel 0 supports ECC                    */
	/*         [1]: Set if channel 1 supports ECC                    */
	/*         [2]: Set if channel 2 supports ECC                    */
	/*         [3]: Set if channel 3 supports ECC                    */
        /* ------------------------------------------------------------- */
		
	/* PRINT("Activating ECC0\n") */
        li      t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
        lw      t1, (BRIDGE_SCRATCH1 << 2)(t0)
	andi    t1, t1, 0x1
	beqz    t1, 1f      /* skip this channel if it does not support ECC */
	nop
	li	t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_DDR2_CHN0_OFFSET)
	lw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)
	ori	t1, 0x20
	sw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)

1:	/* PRINT("Activating ECC1\n") */
        li      t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
        lw      t1, (BRIDGE_SCRATCH1 << 2)(t0)
	andi    t1, t1, 0x2
	beqz    t1, 2f      /* skip this channel if it does not support ECC */
	nop
	li	t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_DDR2_CHN1_OFFSET)
	lw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)
	ori	t1, 0x20
	sw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)

2:	/* PRINT("Activating ECC2\n") */
        li      t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
        lw      t1, (BRIDGE_SCRATCH1 << 2)(t0)
	andi    t1, t1, 0x4
	beqz    t1, 3f      /* skip this channel if it does not support ECC */
	nop
	li	t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_DDR2_CHN2_OFFSET)
	lw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)
	ori	t1, 0x20
	sw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)

3:	/* PRINT("Activating ECC3\n") */
        li      t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
        lw      t1, (BRIDGE_SCRATCH1 << 2)(t0)
	andi    t1, t1, 0x8
	beqz    t1, 4f      /* skip this channel if it does not support ECC */
	nop
	li	t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_DDR2_CHN3_OFFSET)
	lw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)
	ori	t1, 0x20
	sw	t1, (DDR2_CONFIG_REG_GLB_PARAMS <<2)(t0)	
	
4:	jr ra
	nop
	
    .end    dram_ecc_enable

    .ent dram_bar_postprocessing
dram_bar_postprocessing:
     li		t0, (BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET)
    li      t1, 0x0	
    sw      t1, (BRIDGE_DRAM_7_BAR << 2)(t0)
	jr ra
	nop
    .end dram_bar_postprocessing
    
    .ent aerr_log
aerr_log:
	/* enable logging of aerr */
	li	t0,	BRIDGE_PHNX_IO_REMAP_VADDR + PHOENIX_IO_BRIDGE_OFFSET
	li	t1,	0x1ffff
	sw	t1,	(BRIDGE_AERR_INTR_EN << 2)(t0)
	jr	ra
	nop	
	.end aerr_log	
	
    .ent gpio_dump_bist
gpio_dump_bist:
	.set push
	.set noreorder

	move	s8, ra

	li	t0, BRIDGE_PHNX_IO_RESET_VADDR + PHOENIX_IO_GPIO_OFFSET	
	lw	t1, (GPIO_BIST_ALL_GO_STATUS_REG << 2)(t0)
	//PRINTF("BIST All Go Status = 0x%x\n", t1)    FUCKME  BIST failures not displayed
	
	li	t0, BRIDGE_PHNX_IO_RESET_VADDR + PHOENIX_IO_GPIO_OFFSET	
	lw	t1, (GPIO_BIST_CPU_GO_STATUS_REG << 2)(t0)
	//PRINTF("BIST CPU Go Status = 0x%x\n", t1)
	
	li	t0, BRIDGE_PHNX_IO_RESET_VADDR + PHOENIX_IO_GPIO_OFFSET	
	lw	t1, (GPIO_BIST_DEV_GO_STATUS_REG << 2)(t0)
	//PRINTF("BIST DEVICE Go Status = 0x%x\n", t1)
	
	li	t0, BRIDGE_PHNX_IO_RESET_VADDR + PHOENIX_IO_GPIO_OFFSET	
	lw	t1, (GPIO_PWRON_RESET_CFG_REG << 2)(t0)
	//PRINTF("Power On reset config = 0x%x\n", t1)

	move	ra, s8
	jr	ra
	nop
	
	.set pop
    .end gpio_dump_bist	

    .globl  gpio_wakeup_slave_cpus
    .ent    gpio_wakeup_slave_cpus
gpio_wakeup_slave_cpus:
	.set	push
	.set	noreorder

	li	t0, BRIDGE_PHNX_IO_RESET_VADDR + PHOENIX_IO_GPIO_OFFSET

	/* pull all the slave cpus out of reset */
#ifdef CPU_BITMAP
	li	t1, CPU_BITMAP
	li	t2, 0x01
	nor	t2, t2, t2
	and	t1, t1, t2
#else
	li	t1, 0xfe
#endif
	sw	t1, (GPIO_CPU_RESET_REG << 2)(t0)
	
	jr	ra
	nop
			
	.set pop
    
    .end	gpio_wakeup_slave_cpus

