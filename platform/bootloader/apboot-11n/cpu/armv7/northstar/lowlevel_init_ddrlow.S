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
 #define PRIMARY_CPU  0

#if 0
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
	cmp r11, #2 /* Check for SKU */
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
pcie_wa_done:
#endif

/* Initialize LCPLL to required clocks for DDR */        
.globl lcpll_init
lcpll_init:
#if 0
	ldr	r0,=CRU_CLKSET_KEY_OFFSET
	ldr	r1,=0xea68
	str	r1,[r0]

	ldr	r3,=ChipcommonA_ChipID
	ldr	r4,[r3]
	lsr r4,r4,#ChipcommonA_ChipID__PackageOption_R
	and	r4,r4,#0xf

	cmp	r4,#1
	beq	1f
	cmp	r4,#2
	beq	2f
	/*Default : High SKU, DDR-1600 */
	ldr	r0,=CRU_LCPLL_CONTROL1_OFFSET
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x08000000
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=CRU_LCPLL_CONTROL2_OFFSET
	ldr	r1,=0x2010081a /* MDIV 0/1/2/3 */
	str	r1,[r0]
	b	en_change
1:  /* Low SKU, DDR-800 */
	ldr	r0,=CRU_LCPLL_CONTROL1_OFFSET
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x07800000 /* NDIV 120 */
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=CRU_LCPLL_CONTROL2_OFFSET
	ldr	r1,=0x1e0f0f19 /* MDIV 0/1/2/3 */
	str	r1,[r0]
	b	en_change
2: /* Mid SKU, DDR-1066*/
	ldr	r0,=CRU_LCPLL_CONTROL1_OFFSET
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x08000000 /* NDIV 128 */
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=CRU_LCPLL_CONTROL2_OFFSET
	ldr	r1,=0x20100c1a /* MDIV 0/1/2/3 */
	str	r1,[r0]
	b	en_change
en_change:	
	ldr	r0,=CRU_LCPLL_CONTROL0_OFFSET
	ldr	r1,[r0]
	orr	r1,r1,#0x7
	str	r1,[r0]
	and	r1,r1,#0xfffffff8
	str	r1,[r0]
	ldr	r0,=CRU_CLKSET_KEY_OFFSET
	ldr	r1,=0x0
	str	r1,[r0]
#endif
        		
/* DDR initialization */
ddr_init:
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
        LDR r9, =0x18010000
    	MOV	r8,r9		/* save r9 */

        LDR     r1, =0x18108800
        LDR     r2, =0x00000000
        STR     r2, [r1]
        LDR     r1, =0x18109800
        STR     r2, [r1]
        
        LDR     r1, =0x18108408
        LDR     r2, [r1]
        LDR     r3, =0xf000ffff
        AND     r2, r2, r3
        ORR     r2, r2, #0x01900000
        STR     r2, [r1]

        LDR     r9, =0x18010800
        LDR     r1, =0x0
        LDR     r3, =0x00019000

wait_for_ddr_phy_up:
        CMP     r3, r1
        BEQ     ddr_phy_rdy
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r9]
        CMP     r2, r1
        BNE     ddr_phy_rdy
        B       wait_for_ddr_phy_up
ddr_phy_rdy:


        LDR     r1, =0x18010814
        LDR     r2, =0x00000010
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r4, =0x18010810
        LDR     r1, =0x1
        LDR     r3, =0x00001400

wait_for_ddr_phy_pll_lock:
        CMP     r3, r1
        BEQ     ddr_phy_pll_lock_done 
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r4]
        AND     r2, r2, #0x1
        CMP     r2, r1
        BEQ     ddr_phy_pll_lock_done
        B       wait_for_ddr_phy_pll_lock
ddr_phy_pll_lock_done:

        LDR     r1, =0x18010b60
        LDR     r2, =0x00000002
        STR     r2, [r1]
        LDR     r3, [r9]


	/* Write 2 if ddr2, 3 if ddr3 */
        LDR     r1, =0x18108500
        LDR     r3, [r1]
        LDR     r2, =0x00000001
        AND     r3, r2, r3
        ORR     r2, r3, #0x2
        LDR     r1, =0x18010bac
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x1801083c
        LDR     r2, =0x00100000
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010848
        LDR     r2, =0x00000101
        STR     r2, [r1]
        LDR     r3, [r9]
        
        
        LDR     r4, =0x1801084C
        LDR     r1, =0x1
        LDR     r3, =0x00001400


wait_for_ddr_phy_calib_lock:
        CMP     r3, r1
        BEQ     ddr_phy_calib_lock_done 
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r4]
        AND     r2, r2, #0x1
        CMP     r2, r1
        BEQ     ddr_phy_calib_lock_done
        B       wait_for_ddr_phy_calib_lock
ddr_phy_calib_lock_done:

        
        LDR     r4, =0x1801084C
        LDR     r1, =0x2
        LDR     r2, [r4]
        AND     r2, r2, #0x2
        CMP     r2, r1
        BEQ     ddr_cntrl_prog


calib_override:
        LDR     r1, =0x18010834
        LDR     r2, =0x0001003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a04
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a10
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a14
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a18
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a1c
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a20
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a24
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a28
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a2c
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a30
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010aa4
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ab0
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ab4
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ab8
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010abc
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ac0
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ac4
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ac8
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010acc
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ad0
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a08
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a0c
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010aa8
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010aac
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

ddr_cntrl_prog:
        mov	r9,r8		/* restore r9 */

	ldr	r6,=0x0 /* No sdram params */
	ldr	r5,=0x0
init_regs:
	mov	fp,lr			/* save lr across calls */
	adrl	r0,ddr_init_tab
	bl	ddr_init_regs
	mov	lr,fp			/* restore lr */
	
	/* ddr2 or ddr3 */
#if 0
	ldr	r0,[r8,#AI_IOSTATUS]
#else
	ldr	r8,=0x18108500
	ldr	r0,[r8]
#endif
	ldr	r2,=DDR_STAT_DDR3
	and	r0,r0,r2
	cmp	r0,r2
	beq	ddr3_init

ddr2_init:
	mov	fp,lr			/* save lr across calls */
	adrl	r0,ddr2_init_tab
	bl	ddr_init_regs
	mov	lr,fp			/* restore lr */
	b	chparams

ddr3_init:
	mov	fp,lr			/* save lr across calls */
	adrl	r0,ddr3_init_tab
	bl	ddr_init_regs
	mov	lr,fp			/* restore lr */

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
    ldr r9, =0x18010000
    ldr r1, [r9, #DDRC_CONTROL87]
    ldr	r2,=0x00000100
    orr	r1,r1,r2
    str	r1,[r9,#DDRC_CONTROL87]

ch8banks:
	ldr	r1,=0x40
	and	r1,r1,r6
	cmp	r1,r0
	beq	do4banks
do8banks:
        ldr     r1,[r9,#DDRC_CONTROL82]
        ldr	r2,=~0x00000300
        and	r1,r1,r2
        str	r1,[r9,#DDRC_CONTROL82]
	b	docols
do4banks:
        ldr     r1,[r9,#DDRC_CONTROL82]
        ldr	r2,=~0x00000300
        and	r1,r1,r2
        ldr	r2,=0x00000100
        orr	r1,r1,r2
        str	r1,[r9,#DDRC_CONTROL82]

docols:	
        ldr     r1,[r9,#DDRC_CONTROL82]
        ldr	r2,=~0x07070000
        and	r1,r1,r2
        ldr	r2,=0x700
        and	r2,r2,r6
        lsl	r2,r2,#16
        orr	r1,r1,r2
        str	r1,[r9,#DDRC_CONTROL82]

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

turnon:
	ldr	r0,=DDRC00_START
	ldr	r1,[r9,#DDRC_CONTROL00]
	orr	r1,r1,r0
        str     r1,[r9,#DDRC_CONTROL00]

poll_ddr_ctrl:
        ldr     r0,[r9,#DDRC_CONTROL89]
        ldr     r2,=DDR_INT_INIT_DONE
        and     r0,r2,r0
        cmp     r0,r2
        bne     poll_ddr_ctrl
		
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


	.align 4
ddr_init_tab:
	.word	14,	0x05051001
	.word	15,	0x000a0a05
	.word	36,	0x01000000
	.word	37,	0x10000000
	.word	38,	0x00100400
	.word	39,	0x00000400
	.word	40,	0x00000100
	.word	42,	0x00000001
	.word	61,	0x00010100
	.word	78,	0x01000200
	.word	79,	0x02000040
	.word	80,	0x00400100
	.word	81,	0x00000200
	.word	83,	0x01ffff0a
	.word	84,	0x01010101
	.word	85,	0x03010101
	.word	86,	0x01000003
	.word	87,	0x0000010c
	.word	88,	0x00010000
	.word	112,	0x00000200
	.word	116,	0x19000000
	.word	117,	0x00000028
	.word	119,	0x00010001
	.word	120,	0x00010001
	.word	121,	0x00010001
	.word	122,	0x00010001
	.word	123,	0x00010001
	.word	130,	0x00000001
	.word	139,	0x00000001
	.word	148,	0xffff0001
	.word	149,	0x00ffff00
	.word	150,	0x0000ffff
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
	.word	168,	0x000a0a00
	.word	170,	0x02000200
	.word	171,	0x02000200
	.word	175,	0x02000200
	.word	176,	0x02000200
	.word	180,	0x80000100
	.word	181,	0x04070303
	.word	182,	0x0000000a
	.word	185,	0x0010ffff
	.word	187,	0x0000000f
	.word	194,	0x00000204
	.word	205,	0x00000040
	.word	DDR_TABLE_END

	.align 4
ddr2_init_tab:
	.word	0,	0x00000400
	.word	3,	0x00000043
	.word	4,	0x000000a7
	.word	5,	0x0a040a02
	.word	6,	0x04020404
	.word	7,	0x05030e14
	.word	8,	0x030e1404
	.word	9,	0x04020305
	.word	10,	0x03005b26
	.word	11,	0x04020303
	.word	12,	0x03005b26
	.word	13,	0x01000003
	.word	16,	0x030000c8
	.word	17,	0x00860000
	.word	18,	0x05050086
	.word	20,	0x00002301
	.word	21,	0x00230a24
	.word	22,	0x00050a24
	.word	23,	0x00000200
	.word	24,	0x000a0002
	.word	25,	0x0002000a
	.word	26,	0x00020008
	.word	27,	0x00c80008
	.word	28,	0x00c80027
	.word	29,	0x00000027
	.word	30,	0x03000001
	.word	31,	0x00030303
	.word	44,	0x00085300
	.word	45,	0x00000004
	.word	46,	0x00040853
	.word	48,	0x08530000
	.word	49,	0x00000004
	.word	50,	0x00040853
	.word	52,	0x08530000
	.word	53,	0x00000004
	.word	54,	0x00040853
	.word	56,	0x08530000
	.word	57,	0x00000004
	.word	58,	0x00040853
	.word	82,	0x01030101
	.word	108,	0x02020101
	.word	109,	0x08080404
	.word	110,	0x03020200
	.word	111,	0x01000202
	.word	128,	0x001d1d00
	.word	129,	0x1d1d0001
	.word	133,	0x00011d1d
	.word	134,	0x00011d1d
	.word	137,	0x001d1d00
	.word	138,	0x1d1d0001
	.word	142,	0x00011d1d
	.word	143,	0x00011d1d
	.word	146,	0x001d1d00
	.word	147,	0x1d1d0001
	.word	169,	0x0a240000
	.word	172,	0x00000a24
	.word	173,	0x000032b4
	.word	174,	0x0a240404
	.word	177,	0x00000a24
	.word	178,	0x000032b4
	.word	179,	0x02020404
	.word	186,	0x15070303
	.word	202,	0x0001046b
	.word	203,	0x0001046b
	.word	206,	0x01030301
	.word	207,	0x00000001
	.word	DDR_TABLE_END

	.align 4
ddr3_init_tab:
	.word	0,	0x00000600
	.word	3,	0x0001046b
	.word	4,	0x00028b0b
	.word	5,	0x0c050c00
	.word	6,	0x04040405
	.word	7,	0x05040e14
	.word	8,	0x040e1404
	.word	9,	0x0c040405
	.word	10,	0x03005b68
	.word	11,	0x0c040404
	.word	12,	0x03005b68
	.word	13,	0x01000004
	.word	16,	0x03000200
	.word	17,	0x00000f0f
	.word	18,	0x05050000
	.word	20,	0x00007801
	.word	21,	0x00780a20
	.word	22,	0x00050a20
	.word	23,	0x00000300
	.word	24,	0x000a0003
	.word	25,	0x0000000a
	.word	27,	0x02000000
	.word	28,	0x0200005a
	.word	29,	0x0000005a
	.word	30,	0x05000001
	.word	31,	0x00050505
	.word	44,	0x00022000
	.word	45,	0x00000046
	.word	46,	0x00460210
	.word	48,	0x02100000
	.word	49,	0x00000046
	.word	50,	0x00460210
	.word	52,	0x02100000
	.word	53,	0x00000046
	.word	54,	0x00460210
	.word	56,	0x02100000
	.word	57,	0x00000046
	.word	58,	0x00460210
	.word	82,	0x01010001
	.word	108,	0x02040108
	.word	109,	0x08010402
	.word	110,	0x02020202
	.word	111,	0x01000201
	.word	128,	0x00212100
	.word	129,	0x21210001
	.word	133,	0x00012121
	.word	134,	0x00012121
	.word	137,	0x00212100
	.word	138,	0x21210001
	.word	142,	0x00012121
	.word	143,	0x00012121
	.word	146,	0x00212100
	.word	147,	0x21210001
	.word	169,	0x0a200000
	.word	172,	0x00000a20
	.word	173,	0x000032a0
	.word	174,	0x0a200505
	.word	177,	0x00000a20
	.word	178,	0x000032a0
	.word	179,	0x02020505
	.word	186,	0x16070303
	.word	202,	0x00000004
	.word	203,	0x00000004
	.word	206,	0x01040401
	.word	207,	0x00000001
	.word	DDR_TABLE_END
