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
 
 #include "ddr_core.h"
 #include "asm/arch/socregs.h"
 #include "config.h"
 #define PRIMARY_CPU  0

#ifdef CONFIG_DDR775
#define CONFIG_DDR3_CLOCK       775
#else
#define CONFIG_DDR3_CLOCK       800
#endif

.globl lowlevel_init
lowlevel_init:
	MRC     p15, 0, r0, c0, c0, 5      /* Identify current CPU */
    AND     r0, r0, #0xf
    CMP     r0, #PRIMARY_CPU
    BEQ     core0_init       
	WFE
    B       coreo_init_done

core0_init:
	ldr r0,=ROM_S0_IDM_IO_STATUS  /* get the SKU id */
	ldr	r11,[r0]
	lsr r11,r11, #2 /* Bits [3:2] - strap_sku_vect, 0/3 - HIGH SKU, 1 - Low SKU, 2 - Medium SKU */
	and r11,r11, #3

/* PCIE workaround for MID SKU */
/* Issue: PCIE slave access is blocked by NIC in Medium SKU */
/* Workaround is to change the address mapping of interconnect to HIGH SKU */

	cmp r11, #2 /* Check for SKU */	/* IS MID SKU */
	bne pcie_wa_done 


	/* Medium SKU */
	eor r1,r1,r1 /* r1=0 */
	ldr r0, =AXIIC_REMAP
	str r1, [r0]
	ldr r0, =SDROM_REMAP_SELECT
	str r1, [r0]
	orr r1, r1, #1
	ldr r0, =SDROM_REMAP_SELECT_ENABLE
	str r1, [r0]

	/* DDR conf paths */
	/* branch to mid sku ...ddr initialization file -  */
	B  lcpll_init


pcie_wa_done:

	/* High & LOW  SKU */


	/* LOW  SKU */
	/* branch to low sku ...ddr initialization file  */
	cmp r11, #1 /* Check for SKU */
	beq lcpll_init

	/* High  SKU */
	/* fall through for DDR3 */
        /* Workaround to configure DDR3 voltage to 1.5v */
        LDR r0, =DDR_S1_IDM_IO_STATUS
        LDR r1, [r0]
	    ANDS r1,r1, #1
	    BEQ ddr3_wa_done 
	    /* DDR3 */
        LDR     r0, =PCU_1V8_1V5_VREGCNTL_OFFSET
        LDR     r1, =0x00000200
        STR     r1, [r0]
        NOP
ddr3_wa_done:	    

set_ddr_clock:
    ldr r5,=CONFIG_DDR3_CLOCK
	ldr	r4,=0xfff
	and	r5,r5,r4
	ldr	r3,=DDR_TABLE_END
	adrl	r4,ddr_clk_tab
1:	ldr	r2,[r4]
	cmp	r2,r3
	beq	init_ddrphy
	cmp	r2,r5
	beq	chg_ddr_clock
	add	r4,r4,#12
	b	1b
        
chg_ddr_clock:
	ldr	r0,=0x1800c180 /* CRU_CLKSET_KEY */
	ldr	r1,=0xea68
	str	r1,[r0]

	ldr	r0,=0x1800c104 /* LCPLL_NDIV_INT */
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,[r4,#4]
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=0x1800c108 /* LCPLL_CHX_MDIV */
	ldr	r1,[r0]
	ldr	r2,=0x000000ff
	and	r1,r1,r2
	ldr	r2,[r4,#8]
	orr	r1,r1,r2
	str	r1,[r0]

en_change:	
	ldr	r0,=0x1800c100 /* LCPLL_LOAD_EN_CH */
	ldr	r1,[r0]
	orr	r1,r1,#0x7
	str	r1,[r0]
	and	r1,r1,#0xfffffff8
	str	r1,[r0]
	ldr	r0,=0x1800c180 /* CRU_CLKSET_KEY */
	ldr	r1,=0x0
	str	r1,[r0]

init_ddrphy:
    LDR r9,=0x18010000
	mov	r8,r9		/* save r9 */

	ldr	r1,=0x18108800
	ldr	r2,=0x00000000
	str	r2,[r1]
	ldr	r1,=0x18109800
	str	r2,[r1]

	ldr	r1,=0x18108408
	ldr	r2,[r1]
	ldr	r3,=0xf000ffff
	and	r2,r2,r3
	orr	r2,r2,#0x01900000
	str	r2,[r1]

	ldr	r9,=0x18010800
	ldr	r1,=0x0
	ldr	r3,=0x00019000

wait_for_ddr_phy_up:
	cmp	r3,r1
	beq	ddr_phy_rdy
	sub	r3,r3,#0x1
	ldr	r2,=0x0
	ldr	r2,[r9]
	cmp	r2,r1
	bne	ddr_phy_rdy
	b	wait_for_ddr_phy_up
ddr_phy_rdy:

	/* Change PLL divider values inside PHY */
	ldr	r1,=0x1801081c
	ldr	r2,=0x00000c10
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r1,=0x18010814
	ldr	r2,=0x00000010
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r4,=0x18010810
	ldr	r1,=0x1
	ldr	r3,=0x00001400

wait_for_ddr_phy_pll_lock:
	cmp	r3,r1
	beq	ddr_phy_pll_lock_done 
	sub	r3,r3,#0x1
	ldr	r2,=0x0
	ldr	r2,[r4]
	and	r2,r2,#0x1
	cmp	r2,r1
	beq	ddr_phy_pll_lock_done
	b	wait_for_ddr_phy_pll_lock
ddr_phy_pll_lock_done:

	ldr	r1,=0x18010b60
	ldr	r2,=0x00000003
	str	r2,[r1]
	ldr	r3,[r9]

	/* Write 2 if ddr2, 3 if ddr3 */
	ldr	r1,=0x18108500
	ldr	r3,[r1]
	ldr	r2,=0x00000001
	and	r3,r2,r3
	orr	r2,r3,#0x2
	ldr	r1,=0x18010bac
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r1,=0x1801083c
	ldr	r2,=0x00100000
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r1,=0x18010848
	ldr	r2,=0x08000101
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r4,=0x1801084C
	ldr	r1,=0x1
	ldr	r3,=0x00001400

wait_for_ddr_phy_calib_lock:
	cmp	r3,r1
	beq	ddr_phy_calib_lock_done 
	sub	r3,r3,#0x1
	ldr	r2,=0x0
	ldr	r2,[r4]
	and	r2,r2,#0x1
	cmp	r2,r1
	beq	ddr_phy_calib_lock_done
	b	wait_for_ddr_phy_calib_lock
ddr_phy_calib_lock_done:

	ldr	r4,=0x1801084C
	ldr	r1,=0x2
	ldr	r2,[r4]
	and	r2,r2,#0x2
	cmp	r2,r1
	beq	ddr_cntrl_prog

calib_override:
	ldr	r1,=0x18010834
	ldr	r2,=0x0001003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a04
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a10
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a14
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a18
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a1c
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a20
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a24
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a28
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a2c
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a30
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010aa4
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ab0
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ab4
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ab8
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010abc
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ac0
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ac4
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ac8
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010acc
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010ad0
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a08
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a0c
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010aa8
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010aac
	ldr	r2,=0x0003003f
	str	r2,[r1]
	ldr	r3,[r9]

ddr_cntrl_prog:

	/* correct Vtt voltage */
	ldr	r1,=0x18010864
	ldr	r2,=0x01d7ffff
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r1,=0x18010868
	str	r2,[r1]
	ldr	r3,[r9]

	mov	r9,r8		/* restore r9 */

init_regs:

	/* ddr2 or ddr3 */
	ldr	r8,=0x18108500
	ldr	r0,[r8]
	ldr	r2,=DDR_STAT_DDR3
	and	r0,r0,r2
	cmp	r0,r2
	beq	ddr3_init

ddr2_init:
	mov	r11,lr			/* save lr across calls */
	adrl	r0,ddr2_init_tab
	bl	ddr_init_regs
	mov	lr,r11			/* restore lr */
	b	chparams

ddr3_init:
	mov	r11,lr			/* save lr across calls */
	adrl	r0,ddr3_init_tab
	bl	ddr_init_regs
	mov	lr,r11			/* restore lr */

chparams:
	ldr	r0,=0x0
	cmp	r6,r0
	beq	turnon			/* No sdram params, use default values */

chhalf:
	ldr	r1,=0x80
	and	r1,r1,r6
	cmp	r1,r0
	beq	ch8banks
setreduc:
	ldr	r1,[r9,#DDRC_CONTROL87]
	ldr	r2,=0x00000100
	orr	r1,r1,r2
	str	r1,[r9,#DDRC_CONTROL87]

ch8banks:
	ldr	r1,=0x40
	and	r1,r1,r6
	cmp	r1,r0
	beq	do4banks
do8banks:
	ldr	r1,[r9,#DDRC_CONTROL82]
	ldr	r2,=~0x00000300
	and	r1,r1,r2
	str	r1,[r9,#DDRC_CONTROL82]
	b	docols
do4banks:
	ldr	r1,[r9,#DDRC_CONTROL82]
	ldr	r2,=~0x00000300
	and	r1,r1,r2
	ldr	r2,=0x00000100
	orr	r1,r1,r2
	str	r1,[r9,#DDRC_CONTROL82]

docols:	
	ldr	r1,[r9,#DDRC_CONTROL82]
	ldr	r2,=~0x07070000
	and	r1,r1,r2
	ldr	r2,=0x700
	and	r2,r2,r6
	lsl	r2,r2,#16
	orr	r1,r1,r2
	str	r1,[r9,#DDRC_CONTROL82]        
#if 0
docaslat:
	ldr	r0,=0x1f
	and	r0,r0,r6		/* cas latency */
	sub	r1,r0,#1		/* wrlat */
	ldr	r2,[r9,#DDRC_CONTROL05]
	ldr	r3,=~0x3f1f3f00
	and	r2,r2,r3
	lsl	r3,r0,#9		/* cas << 9 */
	orr	r2,r2,r3	
	lsl	r3,r0,#25		/* cas << 25 */
	orr	r2,r2,r3
	lsl	r3,r1,#16		/* wrlat << 16 */
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL05]
	ldr	r2,[r9,#DDRC_CONTROL06]
	ldr	r3,=~0x0000001f
	and	r2,r2,r3
	orr	r2,r2,r1
	str	r2,[r9,#DDRC_CONTROL06]
	
	ldr	r2,[r9,#DDRC_CONTROL174]
	ldr	r3,=~0x00001f3f
	and	r2,r2,r3
	lsl	r3,r1,#8
	orr	r2,r2,r3
	orr	r2,r2,r1
	str	r2,[r9,#DDRC_CONTROL174]

	ldr	r2,[r9,#DDRC_CONTROL44]
	ldr	r3,=~0x00007000
	and	r2,r2,r3
	mov	r3,r0
	lsl	r3,r3,#12
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL44]

	ldr	r2,[r9,#DDRC_CONTROL186]
	ldr	r3,=~0xff000000
	and	r2,r2,r3
	add	r3,r0,#16
	lsl	r3,r3,#24
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL186]

	ldr	r8,=0x18108500
	ldr	r3,[r8]
	ldr	r2,=DDR_STAT_DDR3
	and	r3,r3,r2
	cmp	r3,r2
	bne	turnon

	ldr	r2,[r9,#DDRC_CONTROL44]
	ldr	r3,=~0x00007000
	and	r2,r2,r3
	sub	r3,r0,#4
	lsl	r3,r3,#12
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL44]
	ldr	r2,[r9,#DDRC_CONTROL45]
	ldr	r3,=~0x00380000
	and	r2,r2,r3
	sub	r3,r0,#6
	lsl	r3,r3,#19
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL45]

	ldr	r2,[r9,#DDRC_CONTROL206]
	ldr	r3,=~0x00001f00
	and	r2,r2,r3
	sub	r3,r0,#2
	lsl	r3,r3,#8
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL206]
#endif

turnon:
	ldr	r0,=DDRC00_START
	ldr	r1,[r9,#DDRC_CONTROL00]
	orr	r1,r1,r0
	str	r1,[r9,#DDRC_CONTROL00]

poll_ddr_ctrl:
	ldr	r0,[r9,#DDRC_CONTROL89]
	ldr	r2,=DDR_INT_INIT_DONE
	and	r0,r2,r0
	cmp	r0,r2
	bne	poll_ddr_ctrl

	ldr	r1,=0x18010b60
	ldr	r2,=0x00000003
	str	r2,[r1]
	ldr	r3,[r9]
	
	ldr	r1,=0x18010a00
	ldr	r2,=0x00010000
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r1,=0x18010a74
	ldr	r2,=0x00010028
	str	r2,[r1]
	ldr	r3,[r9]

	ldr	r1,=0x18010b14
	ldr	r2,=0x00010023
	str	r2,[r1]
	ldr	r3,[r9]
        
ddr_init_done:

#if 1 /* Bump up the CPU clock */
set_cru_policy7:
	ldr r0,=0x1810D500 /* get the SKU id */
	ldr	r3,[r0]
	lsr r3,r3, #2
	and r3,r3, #3
	lsr r2,r3, #1
	and r3,r3, #1
	eor r3,r2,r3
	ldr	r0,=IHOST_PROC_CLK_WR_ACCESS
	ldr	r1,=0xa5a501
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_PLLARMA
	ldr	r1,=0x1004001
	eor r2,r2,r2
	cmp r3,r2 
	bne setdiv
	orr r1,r1,#0x1000 /* r1 = 0x1006001 */
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
	ldr	r1,=0x00000301
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_CORE1_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_ARM_SWITCH_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_ARM_PERIPH_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_APB0_CLKGATE
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

/* Setup CCA UART clock divider to 2*/
        LDR     r1, =ChipcommonA_ClkDiv
        LDR     r2, [r1]
        AND     r2, r2, #0xFFFFFF00
        ORR     r2, r2, #0x2 
        STR     r2, [r1]
coreo_init_done:		
		mov	pc, lr
        .ltorg

	.align 4
ddr_clk_tab:
	.word	333,	0x07800000,	0x1e0f1200
	.word	400,	0x08000000,	0x20101000
	.word	533,	0x08000000,	0x20100c00
	.word	666,	0x08000000,	0x1e0f0900
	.word	775,	0x07c00000,	0x1f100800
	.word	800,	0x08000000,	0x20100800
	.word	DDR_TABLE_END
	
	.align 4
ddr2_init_tab:
	.word	0,	0x00000400
	.word	1,	0x00000000
	.word	3,	0x00000050
	.word	4,	0x000000c8
	.word	5,	0x0c050c02
	.word	6,	0x04020405
	.word	7,	0x05031015
	.word	8,	0x03101504
	.word	9,	0x05020305
	.word	10,	0x03006d60
	.word	11,	0x05020303
	.word	12,	0x03006d60
	.word	13,	0x01000003
	.word	14,	0x05061001
	.word	15,	0x000b0b06
	.word	16,	0x030000c8
	.word	17,	0x00a01212
	.word	18,	0x060600a0
	.word	19,	0x00000000
	.word	20,	0x00003001
	.word	21,	0x00300c2d
	.word	22,	0x00050c2d
	.word	23,	0x00000200
	.word	24,	0x000a0002
	.word	25,	0x0002000a
	.word	26,	0x00020008
	.word	27,	0x00c80008
	.word	28,	0x00c80037
	.word	29,	0x00000037
	.word	30,	0x03000001
	.word	31,	0x00030303
	.word	36,	0x01000000
	.word	37,	0x10000000
	.word	38,	0x00100400
	.word	39,	0x00000400
	.word	40,	0x00000100
	.word	41,	0x00000000
	.word	42,	0x00000001
	.word	43,	0x00000000
	.word	44,	0x000a6300
	.word	45,	0x00000004
	.word	46,	0x00040a63
	.word	47,	0x00000000
	.word	48,	0x0a630000
	.word	49,	0x00000004
	.word	50,	0x00040a63
	.word	51,	0x00000000
	.word	52,	0x0a630000
	.word	53,	0x00000004
	.word	54,	0x00040a63
	.word	55,	0x00000000
	.word	56,	0x0a630000
	.word	57,	0x00000004
	.word	58,	0x00040a63
	.word	59,	0x00000000
	.word	60,	0x00000000
	.word	61,	0x00010100
	.word	78,	0x01000200
	.word	79,	0x02000040
	.word	80,	0x00400100
	.word	81,	0x00000200
	.word	82,	0x01030001
	.word	83,	0x01ffff0a
	.word	84,	0x01010101
	.word	85,	0x03010101
	.word	86,	0x01000003
	.word	87,	0x0000010c
	.word	88,	0x00010000
	.word	108,	0x02020101
	.word	109,	0x08080404
	.word	110,	0x03020200
	.word	111,	0x01000202
	.word	112,	0x00000200
	.word	116,	0x19000000
	.word	117,	0x00000028
	.word	118,	0x00000000
	.word	119,	0x00010001
	.word	120,	0x00010001
	.word	121,	0x00010001
	.word	122,	0x00010001
	.word	123,	0x00010001
	.word	128,	0x001c1c00
	.word	129,	0x1c1c0001
	.word	130,	0x00000001
	.word	133,	0x00011c1c
	.word	134,	0x00011c1c
	.word	137,	0x001c1c00
	.word	138,	0x1c1c0001
	.word	139,	0x00000001
	.word	142,	0x00011c1c
	.word	143,	0x00011c1c
	.word	144,	0x00000000
	.word	145,	0x00000000
	.word	146,	0x001c1c00
	.word	147,	0x1c1c0001
	.word	148,	0xffff0001
	.word	149,	0x00ffff00
	.word	150,	0x0000ffff
	.word	151,	0x00000000
	.word	152,	0x03030303
	.word	153,	0x03030303
	.word	156,	0x02006400
	.word	157,	0x02020202
	.word	158,	0x02020202
	.word	160,	0x01020202
	.word	161,	0x01010064
	.word	162,	0x01010101
	.word	163,	0x01010101
	.word	165,	0x00020101
	.word	166,	0x00000064
	.word	167,	0x00000000
	.word	168,	0x000a0a00
	.word	169,	0x0c2d0000
	.word	170,	0x02000200
	.word	171,	0x02000200
	.word	172,	0x00000c2d
	.word	173,	0x00003ce1
	.word	174,	0x0c2d0505
	.word	175,	0x02000200
	.word	176,	0x02000200
	.word	177,	0x00000c2d
	.word	178,	0x00003ce1
	.word	179,	0x02020505
	.word	180,	0x80000100
	.word	181,	0x04070303
	.word	182,	0x0000000a
	.word	185,	0x0010ffff
	.word	186,	0x16070303
	.word	187,	0x0000000f
	.word	194,	0x00000204
	.word	202,	0x00000050
	.word	203,	0x00000050
	.word	204,	0x00000000
	.word	205,	0x00000040
	.word	206,	0x01030301
	.word	207,	0x00000001
	.word	DDR_TABLE_END

	.align 4
ddr3_init_tab:
	.word	0,	0x00000600
	.word	1,	0x00000000 
	.word	3,	0x000000a0 
	.word	4,	0x00000190 
	.word	5,	0x16081600 
	.word	6,	0x06040408 
	.word	7,	0x0b061c27
	.word	8,	0x061c2706
	.word	9,	0x0c04060b 
	.word	10,	0x0400db60 
	.word	11,	0x0c040604 
	.word	12,	0x0400db60 
	.word	13,	0x01000004 
	.word	14,	0x0b0c1001 
	.word	15,	0x0017170c 
	.word	16,	0x03000200 
	.word	17,	0x00002020 
	.word	18,	0x0b0b0000 
	.word	19,	0x00000000 
	.word	20,	0x00011801 
	.word	21,	0x01181858 
	.word	22,	0x00051858 
	.word	23,	0x00000500 
	.word	24,	0x00140005 
	.word	25,	0x00000014 
	.word	26,	0x00000000 
	.word	27,	0x02000000 
	.word	28,	0x02000120 
	.word	29,	0x00000120 
	.word	30,	0x08000001 
	.word	31,	0x00080808 
	.word	32,	0x00000000 
	.word	35,	0x00000000 
	.word	36,	0x01000000 
	.word	37,	0x10000000 
	.word	38,	0x00100400 
	.word	39,	0x00000400 
	.word	40,	0x00000100 
	.word	41,	0x00000000 
	.word	42,	0x00000001 
	.word	43,	0x00000000 
	.word	44,	0x000c7000 
	.word	45,	0x00180046 
	.word	46,	0x00460c70 
	.word	47,	0x00000018 
	.word	48,	0x0c700000 
	.word	49,	0x00180046 
	.word	50,	0x00460c70 
	.word	51,	0x00000018 
	.word	52,	0x0c700000 
	.word	53,	0x00180046 
	.word	54,	0x00460c70 
	.word	55,	0x00000018 
	.word	56,	0x0c700000 
	.word	57,	0x00180046 
	.word	58,	0x00460c70 
	.word	59,	0x00000018 
	.word	60,	0x00000000 
	.word	61,	0x00010100 
	.word	62,	0x00000000 
	.word	63,	0x00000000 
	.word	64,	0x00000000 
	.word	65,	0x00000000 
	.word	66,	0x00000000 
	.word	67,	0x00000000 
	.word	68,	0x00000000 
	.word	69,	0x00000000 
	.word	70,	0x00000000 
	.word	71,	0x00000000 
	.word	72,	0x00000000 
	.word	73,	0x00000000 
	.word	74,	0x00000000 
	.word	75,	0x00000000 
	.word	76,	0x00000000 
	.word	77,	0x00000000 
	.word	78,	0x01000200 
	.word	79,	0x02000040 
	.word	80,	0x00400100 
	.word	81,	0x00000200 
	.word	82,	0x01000001 
	.word	83,	0x01ffff0a 
	.word	84,	0x01010101 
	.word	85,	0x03010101 
	.word	86,	0x01000003 
	.word	87,	0x0000010c 
	.word	88,	0x00010000 
	.word	89,	0x00000000 
	.word	90,	0x00000000 
	.word	91,	0x00000000 
	.word	92,	0x00000000 
	.word	93,	0x00000000 
	.word	94,	0x00000000 
	.word	95,	0x00000000 
	.word	96,	0x00000000 
	.word	97,	0x00000000 
	.word	98,	0x00000000 
	.word	99,	0x00000000 
	.word	100,	0x00000000 
	.word	101,	0x00000000 
	.word	102,	0x00000000 
	.word	103,	0x00000000 
	.word	104,	0x00000000 
	.word	105,	0x00000000 
	.word	106,	0x00000000 
	.word	107,	0x00000000 
	.word	108,	0x02040108 
	.word	109,	0x08010402 
	.word	110,	0x02020202 
	.word	111,	0x01000201 
	.word	112,	0x00000200 
	.word	113,	0x00000000 
	.word	114,	0x00000000 
	.word	115,	0x00000000 
	.word	116,	0x19000000 
	.word	117,	0x00000028 
	.word	118,	0x00000000 
	.word	119,	0x00010001 
	.word	120,	0x00010001 
	.word	121,	0x00010001 
	.word	122,	0x00010001 
	.word	123,	0x00010001 
	.word	124,	0x00000000 
	.word	125,	0x00000000 
	.word	126,	0x00000000 
	.word	127,	0x00000000 
	.word	128,	0x00232300 
	.word	129,	0x23230001 
	.word	130,	0x00000001 
	.word	131,	0x00000000 
	.word	132,	0x00000000 
	.word	133,	0x00012323 
	.word	134,	0x00012323 
	.word	135,	0x00000000 
	.word	136,	0x00000000 
	.word	137,	0x00232300 
	.word	138,	0x23230001 
	.word	139,	0x00000001 
	.word	140,	0x00000000 
	.word	141,	0x00000000 
	.word	142,	0x00012323 
	.word	143,	0x00012323 
	.word	144,	0x00000000 
	.word	145,	0x00000000 
	.word	146,	0x00232300 
	.word	147,	0x23230001 
	.word	148,	0xffff0001 
	.word	149,	0x00ffff00 
	.word	150,	0x0000ffff 
	.word	151,	0x00000000 
	.word	152,	0x03030303 
	.word	153,	0x03030303 
	.word	156,	0x02006400 
	.word	157,	0x02020202 
	.word	158,	0x02020202 
	.word	160,	0x01020202 
	.word	161,	0x01010064 
	.word	162,	0x01010101 
	.word	163,	0x01010101 
	.word	165,	0x00020101 
	.word	166,	0x00000064 
	.word	167,	0x00000000 
	.word	168,	0x000b0b00 
	.word	169,	0x18580000 
	.word	170,	0x02000200 
	.word	171,	0x02000200 
	.word	172,	0x00001858 
	.word	173,	0x000079b8 
	.word	174,	0x1858080a 
	.word	175,	0x02000200 
	.word	176,	0x02000200 
	.word	177,	0x00001858 
	.word	178,	0x000079b8 
	.word	179,	0x0202080a 
	.word	180,	0x80000100 
	.word	181,	0x04070303 
	.word	182,	0x0000000a 
	.word	183,	0x00000000 
	.word	184,	0x00000000 
	.word	185,	0x0010ffff 
	.word	186,	0x1c070303 
	.word	187,	0x0000000f 
	.word	188,	0x00000000 
	.word	189,	0x00000000 
	.word	190,	0x00000000 
	.word	191,	0x00000000 
	.word	192,	0x00000000 
	.word	193,	0x00000000 
	.word	194,	0x00000204 
	.word	195,	0x00000000 
	.word	196,	0x00000000 
	.word	197,	0x00000000 
	.word	198,	0x00000000 
	.word	199,	0x00000000 
	.word	200,	0x00000000 
	.word	201,	0x00000000 
	.word	202,	0x00000008 
	.word	203,	0x00000008 
	.word	204,	0x00000000 
	.word	205,	0x00000040 
	.word	206,	0x00070701 
	.word	207,	0x00000000
	.word	DDR_TABLE_END
		
ddr_init_regs:
	mov	r2,r0
	ldr	r3,=DDR_TABLE_END
1:	ldr	r4,[r2]
	cmp	r4,r3
	beq	2f
	ldr	r5,[r2,#4]
	lsl	r4,r4,#2
	add	r1,r9,r4
	str	r5,[r1]
	add	r2,r2,#8
	b	1b
2:
	mov	pc,lr
