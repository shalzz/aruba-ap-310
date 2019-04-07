/*
 *  armboot - Startup Code for ARM926EJS CPU-core
 *
 *  Copyright (c) 2003  Texas Instruments
 *
 *  ----- Adapted for OMAP1610 from ARM925t code ------
 *
 *  Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <gj@denx.de>
 *  Copyright (c) 2003	Richard Woodruff <r-woodruff2@ti.com>
 *  Copyright (c) 2003	Kshitij <kshitij@ti.com>
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

#if defined(CONFIG_OMAP1610)
#include <./configs/omap1510.h>
#elif defined(CONFIG_OMAP730)
#include <./configs/omap730.h>
#endif

/*
 *************************************************************************
 *
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */


.globl _start
_start:
	b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq

	.balignl 16,0xdeadbeef


/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************
 */
.globl _TEXT_BASE
_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif


/*
 * the actual reset code
 */

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0
#if defined(MV_88F6183)
    /* Start of SPI errata change */

        /*
         * set the cpu to SVC32 mode, I and F disabled.
         */
        mov     r1, #0xd3 
        msr     cpsr,r1 

/* Add for load code into I cache */
	mvn	r5, #0xff
	and	r4, r4, r5

	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
    bic     r0, r0, #0x00002300     /* clear bits 13, 9:8 (--V- --RS) */
	bic	r0, r0, #0x00000007	/* 2:0 (CAM) */
	orr	r0, r0, #0x00000002	/* set bit 2 (A) Align */
	orr	r0, r0, #0x00001000	/* Enabled I-cache */	
	mcr	p15, 0, r0, c1, c0, 0

	/* Add nop commands for cache flush operations */
	nop
	nop
	nop
	/* here. MUST BE IN THE SAME CACHE LINE */

	/* lock I-Cache */
	mrc   p15, 0, r8, c9, c0, 1
	orr   r8, r8, #0xf
	mcr   p15, 0, r8, c9, c0, 1

	/* Load source code from 0xfff90000-0xfff94000 */
        mov   r8, #0
        mov   r0, #0xff		/* U-boot base address on flash */
    	orr   r8, r8, r0, LSL #24
        mov   r0, #0xf9		/* U-boot base address on flash */
    	orr   r8, r8, r0, LSL #16
        mov   	r2, #0x5000	/* U-boot size of code in reset vector */

	/* Load code into I Cache */
load_loop1:
	mcr   p15, 0, r8, c7, c13, 1
        add   r8, r8, #32 	/* 8 dwords * 4 bytes */
        sub   r2, r2, #32 	/* 8 dwords * 4 bytes */
        cmp   r2, #0 /* check if we have read a full Page */
        bne   load_loop1
/* End of code load */
    /*End of SPI errata change */
#endif /*MV_88F6183*/

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
#endif

#ifndef CONFIG_SKIP_RELOCATE_UBOOT
#if defined(CONFIG_MARVELL) && defined(MV78200) && !defined(DUAL_OS_78200)
	mov	r0, #0
	mrc	p15, 1, r0, c15, c1, 0
        /* Check if we are CPU0 or CPU1 */
	and	r0, r0, #0x4000
	cmp	r0, #0x4000
        beq     slave_cpu_relocate
#endif

relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc during debug         */
	beq     stack_setup

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot            */
	add	r2, r0, r2		/* r2 <- source end address         */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop
#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

	/* Set up the stack						    */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
#ifndef CONFIG_MARVELL
	sub	r0, r0, #CFG_MALLOC_LEN	/* malloc area                      */
#endif
	sub	r0, r0, #CFG_GBL_DATA_SIZE /* bdinfo                        */
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #16		/* leave 4 words for abort-stack    */
	bic     sp, sp, #7              /* 8-byte alignment for ABI compliance */

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov 	r2, #0x00000000		/* clear                            */

clbss_l:str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l

	ldr	pc, _start_armboot

#if defined(CONFIG_MARVELL) && defined(MV78200) && !defined(DUAL_OS_78200)
/* Dummy relocate code for slave cpu
   Create new stack
*/
slave_cpu_relocate:
	/* Set up the stack						    */
	ldr	r0, _TEXT_BASE			/* stack from uboot base (6MB) - first CPU (2MB)*/
	sub	r0, r0, #CONFIG_STACKSIZE
	sub	r0, r0, #CFG_GBL_DATA_SIZE 	/* bdinfo */
	sub	sp, r0, #12			/* leave 3 words for abort-stack */
	bic     sp, sp, #7              /* 8-byte alignment for ABI compliance */

	ldr	pc, _start_armboot
#endif

_start_armboot:
	.word start_armboot


/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */


cpu_init_crit:

        /* Check if we are 926 or 946 */
        mrc	p15, 0, r0, c0, c0, 0
        mov    r0, r0, LSR #4
        ldr    r1, =0xfff
        and    r0, r0, r1
        ldr    r1, =0x946
        cmp    r0, r1
        beq     cpu_946

cpu_926:
#if !defined(MV78XX0) && !defined(MV_88F6183)
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */
#endif
         b cpu_continue


cpu_946:
	
         /*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate v4 I-cache */
	mcr	p15, 0, r0, c7, c6, 0	/* flush D-Cache */
         

         


cpu_continue:

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00000007	/* 2:0 (CAM) */
	orr	r0, r0, #0x00000002	/* set bit 2 (A) Align */
#if !defined(MV78XX0)
    orr r0, r0, #0x00001000 /* set bit 12 (I-cache) */
#endif
	mcr	p15, 0, r0, c1, c0, 0

#if !defined(MV78XX0)
    /*
     * Initialize Dcache lockdown register for KW A0 only
     */
    ldr r1, =(0xd0000000 + 0x40008)
    ldr r6, [r1]
#if defined(MV_CPU_BE)
    eor temp, r6, r6, ROR #16 ;     /*temp = A^C,B^D,C^A,D^B  */\
    bic r1, r1, #0xFF0000 ;     /*temp = A^C,0  ,C^A,D^B  */\
    mov r6, r6, ROR #8 ;            /*sr   = D  ,A  ,B  ,C    */\
    eor r6, r6, r1, LSR #8        /*sr   = D  ,C  ,B  ,A    */
#endif
    ldr   r1, =0x3              /* MV_88F6XXX_A1_REV */
    and r6, r6, #0xff
    cmp r1, r6
    bne kw_a1
	mov	r0, #0
    mcr	p15, 0, r0, c9, c0, 0
kw_a1:
#endif

	/*
	 * Go setup Memory and board specific bits prior to relocation.
	 */
	mov	ip, lr		/* perserve link reg across call */
	bl	lowlevel_init	/* go setup pll,mux,memory */
	mov	lr, ip		/* restore link */
	mov	pc, lr		/* back to my caller */
/*
 *************************************************************************
 *
 * Flush DCache
 *
 *************************************************************************
 */

.globl _dcache_index_max
_dcache_index_max:
	.word 0x0

.globl _dcache_index_inc
_dcache_index_inc:
	.word 0x0

.globl _dcache_set_max
_dcache_set_max:
	.word 0x0

.globl _dcache_set_index
_dcache_set_index:         
         .word 0x0


#define s_max   r0
#define s_inc   r1
#define i_max   r2
#define i_inc   r3

.globl cpu_dcache_flush_all         
cpu_dcache_flush_all:
        stmdb	sp!, {r0-r3,ip}

        ldr i_max, _dcache_index_max
        ldr i_inc, _dcache_index_inc
        ldr s_max, _dcache_set_max
        ldr s_inc, _dcache_set_index
        
Lnext_set_inv:
        orr     ip, s_max, i_max
Lnext_index_inv:
        mcr     p15, 0, ip, c7, c14, 2  /* Purge D cache SE with Set/Index */
        sub     ip, ip, i_inc
        tst     ip, i_max               /* Index 0 is last one */
        bne     Lnext_index_inv         /* Next index */
        mcr     p15, 0, ip, c7, c14, 2  /* Purge D cache SE with Set/Index */
        subs    s_max, s_max, s_inc
        bpl     Lnext_set_inv           /* Next set */
        ldmia	sp!, {r0-r3,ip}
        
        mov	pc, lr   /* back to my caller */

.globl cpu_icache_flush_invalidate_all         
cpu_icache_flush_invalidate_all:
        stmdb	sp!, {r0}
        
        ldr     r0,=0
        mcr     p15, 0, r0, c7, c5, 0   /* Flush Invalidate I caches */
        ldmia	sp!, {r0}
        
        mov	pc, lr   /* back to my caller */

/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */

@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	@ carve out a frame on current user stack
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}	@ Save user registers (now in svc mode) r0-r12

	ldr	r2, _armboot_start
#ifndef CONFIG_MARVELL
 	sub	r2, r2, #(CONFIG_STACKSIZE+CFG_MALLOC_LEN)
#else
	sub	r2, r2, #CONFIG_STACKSIZE
#endif
	sub	r2, r2, #(CFG_GBL_DATA_SIZE+8)  @ set base 2 words into abort stack
	@ get values for "aborted" pc and cpsr (into parm regs)
	ldmia	r2, {r2 - r3}
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack
	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}	@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp		@ save current stack into r0 (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	@ !!!! R8 NEEDS to be saved !!!! a reserved stack spot would be good.
	add	r8, sp, #S_PC
	stmdb	r8, {sp, lr}^		@ Calling SP, LR
	str	lr, [r8, #0]		@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]		@ Save CPSR
	str	r0, [r8, #8]		@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4		@ return & move spsr_svc into cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack
#ifndef CONFIG_MARVELL
	sub	r13, r13, #(CONFIG_STACKSIZE+CFG_MALLOC_LEN) 
#else
	sub	r13, r13, #CONFIG_STACKSIZE
#endif
	sub	r13, r13, #(CFG_GBL_DATA_SIZE+8) @ reserved a couple spots in abort stack

	str	lr, [r13]	@ save caller lr in position 0 of saved stack
	mrs	lr, spsr	@ get the spsr
	str	lr, [r13, #4]	@ save spsr in position 1 of saved stack
	mov	r13, #MODE_SVC	@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13	@ switch modes, make sure moves will execute
#ifdef CONFIG_MARVELL
	ldr	r13, _TEXT_BASE		
#endif
	mov	lr, pc		@ capture return pc
	movs	pc, lr		@ jump to next instruction & switch modes.
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align  5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#ifdef CONFIG_USE_IRQ

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl 	do_irq
	irq_restore_user_regs

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	bl 	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif

#ifdef CONFIG_INTEGRATOR

	/* Satisfied by Integrator routine (AP or CP) */

#else

	.align	5

#ifndef CONFIG_MARVELL
.globl reset_cpu
reset_cpu:
	ldr	r1, rstctl1	/* get clkm1 reset ctl */
	mov	r3, #0x0
	strh	r3, [r1]	/* clear it */
	mov	r3, #0x8
	strh	r3, [r1]	/* force dsp+arm reset */
_loop_forever:
	b	_loop_forever

rstctl1:
	.word	0xfffece10
#endif /* CONFIG_MARVELL */
#endif /* #ifdef CONFIG_INTEGRATOR */
