/*
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "asm/arch/socregs.h" 
#include "config.h"

 #define PRIMARY_CPU  0
 

.globl lowlevel_init
lowlevel_init:
		MRC     p15, 0, r0, c0, c0, 5      /* Identify current CPU */
        AND     r0, r0, #0xf
        CMP     r0, #PRIMARY_CPU
        BEQ     core0_init
		WFE
        B       coreo_init_done		
core0_init:
	/* Disable interrupts */
	mrs	r0, cpsr
	orr	r0, r0, #0xc0
	msr	cpsr,r0
	mov r1, #0
	/* Initialize cp15 registers */
    MCR	p15,0,r1,c5,c0,0   /* DFSR */
    MCR	p15,0,r1,c5,c0,1   /* IFSR */
    MCR	p15,0,r1,c6,c0,0   /* DFAR */
    MCR	p15,0,r1,c6,c0,1   /* WFAR */
	
	@ Disable MMU
	MRC p15, 0, r1, c1, c0, 0 @ Read Control Register configuration data
	BIC r1, r1, #0x1
	MCR p15, 0, r1, c1, c0, 0 @ Write Control Register configuration data
	@ Disable L1 Caches
	MRC p15, 0, r1, c1, c0, 0 @ Read Control Register configuration data
	BIC r1, r1, #(0x1 << 12) @ Disable I Cache
	BIC r1, r1, #(0x1 << 2) @ Disable D Cache
	MCR p15, 0, r1, c1, c0, 0 @ Write Control Register configuration data
	@ Invalidate L1 Caches
	@ Invalidate Instruction cache
	MOV r1, #0
	MCR p15, 0, r1, c7, c5, 0

    @ Invalidate BTAC
	MCR p15, 0, r0, c7, c5, 6 	
    mcr p15, 0, r1, c7, c10, 4 /* DSB */
    mcr p15, 0, r0, c7, c5, 4 /* ISB */ 

    @ Enable I cache
    MRC     p15, 0, r0, c1, c0, 0   @ Read Control Register configuration data
    ORR     r0, r0, #(0x3 << 11)    @ Enable I Cache and Branch prediction
    MCR     p15, 0, r0, c1, c0, 0   @ Write Control Register configuration data

    @  Invalidate Data cache
    @ To make the code general purpose, we calculate the cache size first and loop through each set + way
    MOV  r0, #0                  
    MCR  p15 ,2, r0, c0, c0, 0
    mcr p15, 0, r0, c7, c5, 4 /* ISB */ 
    MRC  p15, 1, r0, c0, c0, 0   @  Read Cache Size ID
    LDR  r4, =0x1ff
    AND  r4, r4, r0, LSR #3      @
    ADD  r4, r4, #1              @  r4 = no: of ways
    LDR  r3, =0x7fff
    AND  r0, r3, r0, LSR #13     @  r0 = no. of sets/lines - 1
    ADD  r0, r0, #1              @  r0 = no: of ways
    MOV  r1, #0                  @  r1 = way counter way_loop
way_loop:
    MOV  r3, #0                  @  r3 = set counter set_loop
set_loop:
    MOV  r2, r1, LSL #30 
    ORR  r2, r3, LSL #5          @  r2 = set/way cache operation format
    MCR  p15, 0, r2, c7, c6, 2   @  Invalidate line described by r2
    ADD  r3, r3, #1              @  Increment set counter
    CMP  r0, r3                  @  Last set reached yet?
    BNE  set_loop                @  if not, iterate set_loop
    ADD  r1, r1, #1              @  else, next
    CMP  r1, r4                  @  Last way reached yet?
    BNE  way_loop                @  if not, iterate way_loop

	@ Enable D Cache
    MCR  p15, 0, r1, c7, c10, 4 /* DSB */
    MRC  p15, 0, r0, c1, c0, 0  @ read CP15 register 1 into r0
    ORR  r0, r0, #0x4           @ enable D Cache
    MCR  p15, 0, r0, c1, c0, 0  @ write CP15 register 1
    MCR  p15, 0, r0, c7, c5, 4 /* ISB */ 

	@ Invalidate TLB
	MCR p15, 0, r1, c8, c7, 0	


#if 1 /* Bump up the CPU clock */
set_cru_policy7:
	ldr	r0,=IHOST_PROC_CLK_WR_ACCESS
	ldr	r1,=0xa5a501
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_PLLARMA
	ldr	r1,=0x1005001
setdiv:
	str	r1,[r0]
	ldr	r2,=0x0
	
	ldr	r4,=0x100000
poll_plllock:
	cmp	r4,r2
	beq	poll_plllock_done
	sub	r4,r4,#1
	ldr	r1,[r0]
	and	r3,r1,#(1 << IHOST_PROC_CLK_PLLARMA__pllarm_lock)
	cmp	r3,r2
	beq	poll_plllock
poll_plllock_done:
	orr	r1,r1,#(1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb)
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_POLICY_FREQ
	ldr	r1,=0x87070707
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_CORE0_CLKGATE
	ldr	r1,=0x00000303 /* Disable clock gating */
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_CORE1_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_ARM_SWITCH_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_ARM_PERIPH_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_APB0_CLKGATE
	add r1,r1,#2 /* 0x303 */
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_POLICY_CTL
	ldr	r2,=(1 << IHOST_PROC_CLK_POLICY_CTL__GO)
	ldr	r3,=(1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC)
	orr	r3,r3,r2
	str	r3,[r0]
	ldr	r3,=0x0
	ldr	r4,=0x100000
poll_ccu:
	cmp	r4,r3
	beq	poll_ccu_done
	sub	r4,r4,#1
	ldr	r1,[r0]
	and	r1,r1,r2
	cmp	r1,r3
	bne	poll_ccu
poll_ccu_done:
#endif /* bump up the CPU clock */

#if 1
/* Setup CCA UART clock divider to 2*/
        LDR     r1, =ChipcommonA_ClkDiv
        LDR     r2, [r1]
        AND     r2, r2, #0xFFFFFF00
        ORR     r2, r2, #0x2 
        STR     r2, [r1]
#else 
/* Setup CCA UART clock to APB clock  */       
        LDR     r1, =ChipcommonA_CoreCtrl
        LDR     r2, [r1]
        AND     r2, r2, #0xFFFFFFF7 /* Disable UART clock */        
        STR     r2, [r1]        
        ORR     r2, r2, #0x9 /* Enable and set it to APB clock(bit 0) */ 
        STR     r2, [r1]
#endif


coreo_init_done:
		mov	pc, lr
