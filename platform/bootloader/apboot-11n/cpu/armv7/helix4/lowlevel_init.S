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

//#define EMUL_DDR3_32BIT 1
 #define PRIMARY_CPU  0
 
.globl DDR_PHY_CFG_BASE
DDR_PHY_CFG_BASE:
	.word	0x18010320
.globl DDR_PHY_CONFIG
DDR_PHY_CONFIG:
	.word	0x18010400	

.globl DDR_PHY_STRP_STAT
DDR_PHY_STRP_STAT:
	.word	0x180108b8	

.globl lowlevel_init
lowlevel_init:
		MRC     p15, 0, r0, c0, c0, 5      /* Identify current CPU */
        AND     r0, r0, #0xf
        CMP     r0, #PRIMARY_CPU
#ifdef CONFIG_HELIX4_EMULATION        
        BEQ     ddr_init      
#else 
        BEQ     ddr_init_done		
#endif                 
		WFE
        B       coreo_init_done		
/* DDR initialization */
ddr_init:
        LDR     r1, =DDR_S1_IDM_RESET_CONTROL
        LDR     r2, =0x00000000
        STR     r2, [r1]
        LDR     r1, =DDR_S2_IDM_RESET_CONTROL
        STR     r2, [r1]
        
        LDR     r1, =DDR_S1_IDM_IO_CONTROL_DIRECT
        LDR     r2, [r1]
        LDR     r3, =0xf000ffff
        AND     r2, r2, r3
        ORR     r2, r2, #0x01900000
        STR     r2, [r1]

        LDR     r9, =DDR_PHY_CONTROL_REGS_REVISION
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

        LDR     r1, =DDR_PHY_CONTROL_REGS_STRAP_CONTROL
#if EMUL_DDR3_32BIT        
        LDR     r2, =0x0190c4d7
#else
		/* 16 bit DDR */
        LDR     r2, =0x0190d4d7        
#endif        
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_STRAP_CONTROL2
        LDR     r2, =0x00360286
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_PLL_CONFIG
        LDR     r2, =0x00000010
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r4, =DDR_PHY_CONTROL_REGS_PLL_STATUS
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

        LDR     r1, =DDR_PHY_WORD_LANE_0_READ_DATA_DLY
        LDR     r2, =0x00000002
        STR     r2, [r1]
        LDR     r3, [r9]
#if EMUL_DDR3_32BIT
        LDR     r1, =DDR_PHY_WORD_LANE_1_READ_DATA_DLY
        LDR     r2, =0x00000002
        STR     r2, [r1]
        LDR     r3, [r9]
#endif

/* Write 2 if ddr2, 3 if ddr3 */
        LDR     r1, =DDR_S1_IDM_IO_STATUS
        LDR     r3, [r1]
        LDR     r2, =0x00000001
        AND     r3, r2, r3
        ORR     r2, r3, #0x2
        LDR     r1, =DDR_PHY_WORD_LANE_0_WR_PREAMBLE_MODE
        STR     r2, [r1]
        LDR     r3, [r9]
#if EMUL_DDR3_32BIT
        LDR     r1, =DDR_PHY_WORD_LANE_1_WR_PREAMBLE_MODE
        STR     r2, [r1]
        LDR     r3, [r9]
#endif
        LDR     r1, =DDR_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTL
        LDR     r2, =0x00100000
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_VDL_CALIBRATE
        LDR     r2, =0x00000101
        STR     r2, [r1]
        LDR     r3, [r9]
        
        
        LDR     r4, =DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS
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

        
        LDR     r4, =DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS
        LDR     r1, =0x2
        LDR     r2, [r4]
        AND     r2, r2, #0x2
        CMP     r2, r1
        BEQ     ddr_cntrl_prog


calib_override:
        LDR     r1, =DDR_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTL
        LDR     r2, =0x0001003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]
#if EMUL_DDR3_32BIT
        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]
#endif

ddr_cntrl_prog:
        LDR     r1, =0x18010004
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801000c
        LDR     r2, =0x000000a0
        STR     r2, [r1]

        LDR     r1, =0x18010010
        LDR     r2, =0x00000190
        STR     r2, [r1]

        LDR     r1, =0x18010014
        LDR     r2, =0x16081600
        STR     r2, [r1]

        LDR     r1, =0x18010018
        LDR     r2, =0x06040408
        STR     r2, [r1]

        LDR     r1, =0x1801001c
        LDR     r2, =0x0b061c27
        STR     r2, [r1]

        LDR     r1, =0x18010020
        LDR     r2, =0x061c2706
        STR     r2, [r1]

        LDR     r1, =0x18010024
        LDR     r2, =0x0c04060b
        STR     r2, [r1]

        LDR     r1, =0x18010028
        LDR     r2, =0x0400db60
        STR     r2, [r1]

        LDR     r1, =0x1801002c
        LDR     r2, =0x0c040604
        STR     r2, [r1]

        LDR     r1, =0x18010030
        LDR     r2, =0x0400db60
        STR     r2, [r1]

        LDR     r1, =0x18010034
        LDR     r2, =0x01010004
        STR     r2, [r1]

        LDR     r1, =0x18010038
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x1801003c
        LDR     r2, =0x00171700
        STR     r2, [r1]

        LDR     r1, =0x18010040
        LDR     r2, =0x03000200
        STR     r2, [r1]

        LDR     r1, =0x18010044
        LDR     r2, =0x00002020
        STR     r2, [r1]

        LDR     r1, =0x18010048
        LDR     r2, =0x0b0b0000
        STR     r2, [r1]

        LDR     r1, =0x1801004c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010050
        LDR     r2, =0x00011801
        STR     r2, [r1]

        LDR     r1, =0x18010054
        LDR     r2, =0x01181858
        STR     r2, [r1]

        LDR     r1, =0x18010058
        LDR     r2, =0x00051858
        STR     r2, [r1]

        LDR     r1, =0x1801005c
        LDR     r2, =0x00000500
        STR     r2, [r1]

        LDR     r1, =0x18010060
        LDR     r2, =0x00140005
        STR     r2, [r1]

        LDR     r1, =0x18010064
        LDR     r2, =0x00000014
        STR     r2, [r1]

        LDR     r1, =0x18010068
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801006c
        LDR     r2, =0x02000000
        STR     r2, [r1]

        LDR     r1, =0x18010070
        LDR     r2, =0x02000120
        STR     r2, [r1]

        LDR     r1, =0x18010074
        LDR     r2, =0x00000120
        STR     r2, [r1]

        LDR     r1, =0x18010078
        LDR     r2, =0x08000001
        STR     r2, [r1]

        LDR     r1, =0x1801007c
        LDR     r2, =0x00080808
        STR     r2, [r1]

        LDR     r1, =0x18010080
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801008c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010090
        LDR     r2, =0x01000000
        STR     r2, [r1]

        LDR     r1, =0x18010094
        LDR     r2, =0x10000000
        STR     r2, [r1]

        LDR     r1, =0x18010098
        LDR     r2, =0x00100400
        STR     r2, [r1]

        LDR     r1, =0x1801009c
        LDR     r2, =0x00000400
        STR     r2, [r1]

        LDR     r1, =0x180100a0
        LDR     r2, =0x00000100
        STR     r2, [r1]

        LDR     r1, =0x180100a4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100a8
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x180100ac
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100b0
        LDR     r2, =0x000c7000
        STR     r2, [r1]

        LDR     r1, =0x180100b4
        LDR     r2, =0x00180046
        STR     r2, [r1]

        LDR     r1, =0x180100b8
        LDR     r2, =0x00460c70
        STR     r2, [r1]

        LDR     r1, =0x180100bc
        LDR     r2, =0x00000018
        STR     r2, [r1]

        LDR     r1, =0x180100c0
        LDR     r2, =0x0c700000
        STR     r2, [r1]

        LDR     r1, =0x180100c4
        LDR     r2, =0x00180046
        STR     r2, [r1]

        LDR     r1, =0x180100c8
        LDR     r2, =0x00460c70
        STR     r2, [r1]

        LDR     r1, =0x180100cc
        LDR     r2, =0x00000018
        STR     r2, [r1]

        LDR     r1, =0x180100d0
        LDR     r2, =0x0c700000
        STR     r2, [r1]

        LDR     r1, =0x180100d4
        LDR     r2, =0x00180046
        STR     r2, [r1]

        LDR     r1, =0x180100d8
        LDR     r2, =0x00460c70
        STR     r2, [r1]

        LDR     r1, =0x180100dc
        LDR     r2, =0x00000018
        STR     r2, [r1]

        LDR     r1, =0x180100e0
        LDR     r2, =0x0c700000
        STR     r2, [r1]

        LDR     r1, =0x180100e4
        LDR     r2, =0x00180046
        STR     r2, [r1]

        LDR     r1, =0x180100e8
        LDR     r2, =0x00460c70
        STR     r2, [r1]

        LDR     r1, =0x180100ec
        LDR     r2, =0x00000018
        STR     r2, [r1]

        LDR     r1, =0x180100f0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100f4
        LDR     r2, =0x00010100
        STR     r2, [r1]

        LDR     r1, =0x180100f8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100fc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010100
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010104
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010108
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801010c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010110
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010114
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010118
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801011c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010120
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010124
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010128
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801012c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010130
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010134
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010138
        LDR     r2, =0x01000200
        STR     r2, [r1]

        LDR     r1, =0x1801013c
        LDR     r2, =0x02000040
        STR     r2, [r1]

        LDR     r1, =0x18010140
        LDR     r2, =0x00400100
        STR     r2, [r1]

        LDR     r1, =0x18010144
        LDR     r2, =0x00000200
        STR     r2, [r1]

        LDR     r1, =0x18010148
        LDR     r2, =0x01000001
        STR     r2, [r1]

        LDR     r1, =0x1801014c
        LDR     r2, =0x01ffff0a
        STR     r2, [r1]

        LDR     r1, =0x18010150
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =0x18010154
#if EMUL_DDR3_32BIT
        LDR     r2, =0x03010101
#else        
        LDR     r2, =0x01010101
#endif
        STR     r2, [r1]

        LDR     r1, =0x18010158
#if EMUL_DDR3_32BIT
        LDR     r2, =0x01000003
#else        
        LDR     r2, =0x03000003
#endif
        STR     r2, [r1]

        LDR     r1, =0x1801015c
#if EMUL_DDR3_32BIT
        LDR     r2, =0x0000000c
#else        
        LDR     r2, =0x0000010c
#endif        
        STR     r2, [r1]

        LDR     r1, =0x18010160
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =0x18010164
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801016c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010170
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010174
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010178
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801017c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010180
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010184
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010188
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801018c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010190
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010194
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010198
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801019c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101a0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101a4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101a8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101ac
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101b0
        LDR     r2, =0x02040108
        STR     r2, [r1]

        LDR     r1, =0x180101b4
        LDR     r2, =0x08010402
        STR     r2, [r1]

        LDR     r1, =0x180101b8
        LDR     r2, =0x02020202
        STR     r2, [r1]

        LDR     r1, =0x180101bc
        LDR     r2, =0x01000200
        STR     r2, [r1]

        LDR     r1, =0x180101c0
        LDR     r2, =0x00000200
        STR     r2, [r1]

        LDR     r1, =0x180101c4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101c8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101cc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101d0
        LDR     r2, =0x0d000000
        STR     r2, [r1]

        LDR     r1, =0x180101d4
        LDR     r2, =0x00000028
        STR     r2, [r1]

        LDR     r1, =0x180101d8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101dc
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101e0
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101e4
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101e8
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101ec
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101f0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101f4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101f8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101fc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010200
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =0x18010204
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =0x18010208
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x1801020c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010210
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010214
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x18010218
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x1801021c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010220
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010224
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =0x18010228
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =0x1801022c
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x18010230
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010234
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010238
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x1801023c
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x18010240
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010244
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010248
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =0x1801024c
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =0x18010250
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x18010254
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010258
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801025c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010260
        LDR     r2, =0x03030303
        STR     r2, [r1]

        LDR     r1, =0x18010264
        LDR     r2, =0x03030303
        STR     r2, [r1]

        LDR     r1, =0x18010270
        LDR     r2, =0x02006400
        STR     r2, [r1]

        LDR     r1, =0x18010274
        LDR     r2, =0x02020202
        STR     r2, [r1]

        LDR     r1, =0x18010278
        LDR     r2, =0x00020202
        STR     r2, [r1]

        LDR     r1, =0x18010280
        LDR     r2, =0x01000000
        STR     r2, [r1]

        LDR     r1, =0x18010284
        LDR     r2, =0x01010064
        STR     r2, [r1]

        LDR     r1, =0x18010288
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =0x1801028c
        LDR     r2, =0x00000101
        STR     r2, [r1]

        LDR     r1, =0x18010294
        LDR     r2, =0x00020000
        STR     r2, [r1]

        LDR     r1, =0x18010298
        LDR     r2, =0x00000064
        STR     r2, [r1]

        LDR     r1, =0x1801029c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102a0
        LDR     r2, =0x000c0c00
        STR     r2, [r1]

        LDR     r1, =0x180102a4
        LDR     r2, =0x18580000
        STR     r2, [r1]

        LDR     r1, =0x180102a8
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102ac
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102b0
        LDR     r2, =0x00001858
        STR     r2, [r1]

        LDR     r1, =0x180102b4
        LDR     r2, =0x0000f370
        STR     r2, [r1]

        LDR     r1, =0x180102b8
        LDR     r2, =0x1858080a
        STR     r2, [r1]

        LDR     r1, =0x180102bc
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102c0
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102c4
        LDR     r2, =0x00001858
        STR     r2, [r1]

        LDR     r1, =0x180102c8
        LDR     r2, =0x0000f370
        STR     r2, [r1]

        LDR     r1, =0x180102cc
        LDR     r2, =0x0202080b
        STR     r2, [r1]

        LDR     r1, =0x180102d0
        LDR     r2, =0x80000100
        STR     r2, [r1]

        LDR     r1, =0x180102d4
        LDR     r2, =0x04070303
        STR     r2, [r1]

        LDR     r1, =0x180102d8
        LDR     r2, =0x0000000a
        STR     r2, [r1]

        LDR     r1, =0x180102dc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102e0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102e4
        LDR     r2, =0x0010ffff
        STR     r2, [r1]

        LDR     r1, =0x180102e8
        LDR     r2, =0x00070303
        STR     r2, [r1]

        LDR     r1, =0x180102ec
        LDR     r2, =0x0000000f
        STR     r2, [r1]

        LDR     r1, =0x180102f0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102f4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102f8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102fc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010300
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010304
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010308
        LDR     r2, =0x00000204
        STR     r2, [r1]

        LDR     r1, =0x1801030c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010314
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010318
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801031c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010320
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010324
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010328
        LDR     r2, =0x00000008
        STR     r2, [r1]

        LDR     r1, =0x1801032c
        LDR     r2, =0x00000008
        STR     r2, [r1]

        LDR     r1, =0x18010330
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010338
        LDR     r2, =0x02070701
        STR     r2, [r1]

        LDR     r1, =0x1801033c
        LDR     r2, =0x00000002
        STR     r2, [r1]

        LDR     r1, =0x18010348
        LDR     r2, =0x0c0b0000
        STR     r2, [r1]

        LDR     r1, =0x1801034c
        LDR     r2, =0x00000c0b
        STR     r2, [r1]

        LDR     r1, =0x18010350
        LDR     r2, =0x00000040
        STR     r2, [r1]

        LDR     r1, =0x18010354
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010358
        LDR     r2, =0x01010606
        STR     r2, [r1]

        LDR     r1, =0x1801035c
        LDR     r2, =0x00000101
        STR     r2, [r1]

        LDR     r1, =0x18010360
        LDR     r2, =0x00002727
        STR     r2, [r1]

        LDR     r1, =0x18010364
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010000
        LDR     r2, =0x00000601
        STR     r2, [r1]


poll_ddr_ctrl_begin:
        LDR     r1, =DDR_DENALI_CTL_89
poll_ddr_ctrl:
        LDR     r0, [r1]
        LDR     r2, =0x100
        AND     r0, r2, r0
        CMP     r0, r2
        BNE     poll_ddr_ctrl

/*        ; Enable SCU

        ; BL scu_enable        

        ;SEV ;Wake up other processors, disabled for now

*/        
ddr_init_done:

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
	ldr	r1,=0x00000303
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
