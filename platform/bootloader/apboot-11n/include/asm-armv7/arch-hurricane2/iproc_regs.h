/*****************************************************************************
* Copyright 2011 Broadcom Corporation.  All rights reserved.
*
* Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2, available at
* http://www.broadcom.com/licenses/GPLv2.php (the "GPL"). 
*
* Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a
* license other than the GPL, without Broadcom's express prior written
* consent.
*****************************************************************************/


#ifndef __IPROC_REGS_H
#define __IPROC_REGS_H __FILE__
#include <linux/types.h>
#include "socregs.h"

#define IPROC_NUM_CPUS			(2)
#define IPROC_NUM_IRQS	(256) /* Number of interrupts supported */
#define IPROC_CPU0_MIN_INT_PRIORITY	(0)
#define IPROC_CPU1_MIN_INT_PRIORITY	(0)

#define IPROC_DDR_MEM_BASE1		(0x0)
#define IPROC_DDR_MEM_BASE2		(0x60000000)

#define IPROC_QSPI_MEM_BASE		(0x1c000000)
#define IPROC_NAND_MEM_BASE		(0x28000000)
#define IPROC_NOR_MEM_BASE		(0x20000000)

#define IPROC_CCA_REG_BASE			(ChipcommonA_ChipID) //(0x18000000)
#define IPROC_CCB_GPIO_REG_BASE		(ChipcommonB_GP_DATA_IN) //(0x18001000)
#define IPROC_CCB_PWM_REG_BASE		(ChipcommonB_PWMCTL) //(0x18002000)
#define IPROC_CCB_MDIO_REG_BASE		(ChipcommonB_MII_Management_Control )//(0x18003000)
#define IPROC_CCB_RNG_REG_BASE		(ChipcommonB_rng_CTRL) //(0x18004000)
#define IPROC_CCB_TIM0_REG_BASE		(ChipcommonB_tim0_TIM_TIMER1Load) //(0x18005000)
#define IPROC_CCB_TIM1_REG_BASE		(ChipcommonB_tim1_TIM_TIMER1Load) //(0x18006000)
#define IPROC_CCB_SRAU_REG_BASE		(0x18007000) //(0x18007000)
#define IPROC_CCB_UART0_REG_BASE	(ChipcommonB_UART0_RBR_THR_DLL) //(0x18008000)

#define IPROC_DDRC_REG_BASE			(DDR_DENALI_CTL_00) //(0x18010000)
#define IPROC_DMAC_REG_BASE			(DMAC_pl330_non_DS) //(0x18011000)
#define IPROC_PCIE_AXIB0_REG_BASE	(PAXB_0_PCIE_CONTROL) //(0x18012000)
#define IPROC_PCIE_AXIB1_REG_BASE	(PAXB_1_PCIE_CONTROL) //(0x18013000)
#define IPROC_PCIE_AXIB2_REG_BASE	(PAXB_2_PCIE_CONTROL) //(0x18014000)

#define IPROC_SDIO3_REG_BASE		(SDIO_eMMCSDXC_SYSADDR) //(0x18020000)
#define IPROC_USB20_REG_BASE		(0x18021000)
#define IPROC_USB30_REG_BASE		(0x18022000)
#define IPROC_USB20_PHY_REG_BASE	(0x18023000)
#define IPROC_GMAC0_REG_BASE		(GMAC0_DEVCONTROL) //(0x18024000)
#define IPROC_GMAC1_REG_BASE		(GMAC1_DEVCONTROL) //(0x18025000)
#define IPROC_GMAC2_REG_BASE		(GMAC2_DEVCONTROL) //(0x18026000)
#define IPROC_GMAC3_REG_BASE		(GMAC3_DEVCONTROL) //(0x18027000)

/* ARM9 Private memory region */
#define IPROC_PERIPH_BASE				(0x19020000) //(IHOST_A9MP_scu_CONTROL)
#define IPROC_PERIPH_SCU_REG_BASE		(IPROC_PERIPH_BASE)			/* Snoop Control Unit */
#define IPROC_PERIPH_INT_CTRL_REG_BASE	(IPROC_PERIPH_BASE + 0x100) /* Interrupt control */
#define IPROC_PERIPH_GLB_TIM_REG_BASE	(IPROC_PERIPH_BASE + 0x200) /* Global timer */
#define IPROC_PERIPH_PVT_TIM_REG_BASE	(IPROC_PERIPH_BASE + 0x600) /* Private timer */
#define IPROC_PERIPH_PVT_WDT_REG_BASE	(IPROC_PERIPH_BASE + 0x620) /* Watchdog timer */
#define IPROC_PERIPH_INT_DISTR_REG_BASE (IPROC_PERIPH_BASE + 0x1000) /* Interrupt Distributor */
#define IPROC_L2CC_REG_BASE				(IPROC_PERIPH_BASE + 0x2000) /* L2 Cache controller */

typedef struct iproc_clk_struct_t {
	uint32_t arm_clk; /* A9 core clock */
	uint32_t arm_periph_clk;
	uint32_t axi_clk;
	uint32_t apb_clk;
} iproc_clk_struct;


/* Structures and bit definitions */
/* SCU Control register */
#define IPROC_SCU_CTRL_SCU_EN		(0x00000001)
#define IPROC_SCU_CTRL_ADRFLT_EN	(0x00000002)
#define IPROC_SCU_CTRL_PARITY_EN	(0x00000004)
#define IPROC_SCU_CTRL_SPEC_LNFL_EN	(0x00000008)
#define IPROC_SCU_CTRL_FRC2P0_EN	(0x00000010)
#define IPROC_SCU_CTRL_SCU_STNDBY_EN	(0x00000020)
#define IPROC_SCU_CTRL_IC_STNDBY_EN	(0x00000040)

typedef volatile struct scu_reg_struct_t {
	uint32_t control;
	uint32_t config;
	uint32_t cpupwrstatus;
	uint32_t invalidate;
	uint32_t rsvd1[4];
	uint32_t rsvd2[4];
	uint32_t rsvd3[4];
	uint32_t filtstart;
	uint32_t filtend;
	uint32_t rsvd4[2];
	uint32_t sac;
	uint32_t snsac;
} scu_reg_struct, * scu_reg_struct_ptr;

/* ARM A9 Private Timer */
#define IPROC_PVT_TIM_CTRL_TIM_EN		(0x00000001)
#define IPROC_PVT_TIM_CTRL_AUTO_RELD	(0x00000002)
#define IPROC_PVT_TIM_CTRL_INT_EN		(0x00000004)
#define IPROC_PVT_TIM_CTRL_PRESC_MASK	(0x0000FF00)
#define IPROC_PVT_TIM_INT_STATUS_SET	(0x00000001)

typedef volatile struct pvt_tim_reg_struct_t {
	uint32_t load;
	uint32_t counter;
	uint32_t control;
	uint32_t intstatus;
} pvt_tim_reg_struct, * pvt_tim_reg_struct_ptr;

/* Global timer */
#define IPROC_GLB_TIM_CTRL_TIM_EN		(0x00000001)
#define IPROC_GLB_TIM_CTRL_COMP_EN		(0x00000002)
#define IPROC_GLB_TIM_CTRL_INT_EN		(0x00000004)
#define IPROC_GLB_TIM_CTRL_AUTO_INC		(0x00000008)
#define IPROC_GLB_TIM_CTRL_PRESC_MASK	(0x0000FF00)
#define IPROC_GLB_TIM_INT_STATUS_SET	(0x00000001)

typedef volatile struct glb_tim_reg_struct_t {
	uint32_t counter_l;
	uint32_t counter_h;
	uint32_t control;
	uint32_t intstatus;
	uint32_t cmp_l;
	uint32_t cmp_h;
	uint32_t reload;
} glb_tim_reg_struct, * glb_tim_reg_struct_ptr;

/* GIC(Generic Interrupt controller) definitions */

typedef volatile struct intr_data_struct_t {
	uint32_t cpuid;
	uint32_t intid;
	void * data;
} intr_data_struct, * intr_data_struct_ptr;

/* GIC(Generic Interrupt controller) CPU interface registers */
#define IPROC_GIC_CI_CTRL_EN			(0x00000001)
#define IPROC_GIC_CI_PMR_PRIO_MASK		(0x000000FF)
#define IPROC_GIC_CI_BPR_BP_MASK		(0x00000003)
#define IPROC_GIC_CI_IAR_INTID_MASK		(0x000003FF)
#define IPROC_GIC_CI_IAR_CPUID_MASK		(0x00001C00)
#define IPROC_GIC_CI_IAR_CPUID_OFFSET	(10)
#define IPROC_GIC_CI_EOIR_INTID_MASK	(0x000003FF)
#define IPROC_GIC_CI_EOIR_CPUID_MASK	(0x00001C00)
#define IPROC_GIC_CI_EOIR_CPUID_OFFSET	(10)
#define IPROC_GIC_CI_RPR_PRIO_MASK		(0x000000FF)
#define IPROC_GIC_CI_HPIR_PENDID_MASK	(0x000003FF)
#define IPROC_GIC_CI_HPIR_CPUID_MASK	(0x00001C00)
#define IPROC_GIC_CI_HPIR_CPUID_OFFSET	(10)
#define IPROC_GIC_CI_ABPR_BP_MASK		(0x00000003)



typedef volatile struct gic_ci_reg_struct_t {
	uint32_t control;	/* Control reg */
	uint32_t pmr;		/* Interrupt Priority mask reg */
	uint32_t bpr;		/* Binary point reg */
	uint32_t iar;		/* interrupt acknowledge reg */
	uint32_t eoir;		/* End of interrupt reg */
	uint32_t rpr;		/* running priority register */
	uint32_t hpir;		/* Highest pending interrupt register */
	uint32_t abpr;		/* Aliased Non-Secure Binary point register */
	uint32_t rsvd[55];
	uint32_t idr;		/* CPU Interface Implementer Identification register */
} gic_ci_reg_struct, * gic_ci_reg_struct_ptr;

#define IPROC_GIC_DIST_CTRL_S_EN_S			(0x00000001)
#define IPROC_GIC_DIST_CTRL_S_EN_NS			(0x00000002)
#define IPROC_GIC_DIST_CTRL_NS_EN_NS		(0x00000001)

#define IPROC_GIC_DIST_ISR_BIT_SIZE			(1)
#define IPROC_GIC_DIST_ISER_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ICER_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ISPR_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ISPR_SECURE			(1)
#define IPROC_GIC_DIST_ISPR_NON_SECURE		(0)
#define IPROC_GIC_DIST_ICPR_BIT_SIZE		(1)
#define IPROC_GIC_DIST_IPR_BIT_SIZE			(8)
#define IPROC_GIC_DIST_IPTR_BIT_SIZE		(8)
#define IPROC_GIC_DIST_IPTR_CPU0			(0x01)
#define IPROC_GIC_DIST_IPTR_CPU1			(0x02)
#define IPROC_GIC_DIST_SGIR_ID_MASK			(0xF)
#define IPROC_GIC_DIST_SGIR_TR_LIST_MASK	(0x00FF0000)
#define IPROC_GIC_DIST_SGIR_TR_LIST_BOFFSET	(16)
#define IPROC_GIC_DIST_SGIR_TR_FILT_MASK	(0x03000000)
#define IPROC_GIC_DIST_SGIR_TR_FILT_BOFFSET	(24)
#define IPROC_GIC_DIST_SGIR_TR_FILT_FW_LIST			(0)
#define IPROC_GIC_DIST_SGIR_TR_FILT_FW_ALL_EX_ME	(0x01)
#define IPROC_GIC_DIST_SGIR_TR_FILT_FW_ME_ONLY		(0x02)


#define IPROC_INTR_LEVEL_SENSITIVE			(1)
#define IPROC_INTR_EDGE_TRIGGERED			(2)



typedef volatile struct gic_dist_reg_struct_t {
	uint32_t control;	/* Control reg */
	uint32_t ictr;		/* Interrupt controller type reg */
	uint32_t idr;		/* Distributor Implementer Identification register */
	uint32_t rsvd1[1];
	uint32_t rsvd2[28];
	uint32_t isr[8];	/* interrupt security reg */
	uint32_t rsvd3[24];
	uint32_t iser[8];	/* interrupt set-enable reg */
	uint32_t rsvd4[24];
	uint32_t icer[8];	/* interrupt clear-enable reg */
	uint32_t rsvd5[24];
	uint32_t ispr[8];	/* interrupt set-pending reg */
	uint32_t rsvd6[24];
	uint32_t icpr[8];	/* interrupt clear-pending reg */
	uint32_t rsvd7[24];
	uint32_t abr[8];	/* Active bit reg */
	uint32_t rsvd8[24];
	uint32_t rsvd9[32];
	uint32_t ipr[64];	/* Interrupt priority reg */
	uint32_t rsvd10[192];
	uint32_t iptr[64];	/* Interrupt processor targets reg */
	uint32_t rsvd11[192];
	uint32_t icfr[16];	/* Interrupt configuration reg */
	uint32_t rsvd12[48];
	uint32_t ppi_status; /* PPI status register */
	uint32_t spi_status[7]; /* SPI status register */
	uint32_t rsvd13[24];
	uint32_t rsvd14[32];
	uint32_t rsvd15[64]; 
	uint32_t sgir;		/* Software generated interrupt reg */
	uint32_t rsvd16[51];
	uint32_t periph_id[8];	
	uint32_t component_id[4];	
} gic_dist_reg_struct, * gic_dist_reg_struct_ptr;


typedef volatile struct l2cc_reg_struct_t {
	uint32_t cache_id;	
	uint32_t cache_type;
	uint32_t rsvd1[62];
	uint32_t control;	/* 0x100 */
	uint32_t aux_control;
	uint32_t tag_ram_control;
	uint32_t data_ram_control;
	uint32_t rsvd2[60];
	uint32_t ev_counter_ctrl;	/* 0x200 */
	uint32_t ev_counter1_cfg;
	uint32_t ev_counter0_cfg;
	uint32_t ev_counter1;
	uint32_t ev_counter0;
	uint32_t int_mask;
	uint32_t int_mask_status;
	uint32_t int_raw_status;
	uint32_t int_clear;
	uint32_t rsvd3[55];
	uint32_t rsvd4[64]; /* 0x300 */
	uint32_t rsvd5[64]; /* 0x400 */
	uint32_t rsvd6[64]; /* 0x500 */
	uint32_t rsvd7[64]; /* 0x600 */
	uint32_t rsvd8[12]; /* 0x700 - 0x72F */
	uint32_t cache_sync; /* 0x730 */
	uint32_t rsvd9[15];
	uint32_t inv_pa; /* 0x770 */
	uint32_t rsvd10[2];
	uint32_t inv_way; /* 0x77C */
	uint32_t rsvd11[12]; 
	uint32_t clean_pa; /* 0x7B0 */
	uint32_t rsvd12[1];
	uint32_t clean_index; /* 0x7B8 */
	uint32_t clean_way;
	uint32_t rsvd13[12]; 
	uint32_t clean_inv_pa; /* 0x7F0 */
	uint32_t rsvd14[1];
	uint32_t clean_inv_index;
	uint32_t clean_inv_way;
	uint32_t rsvd15[64]; /* 0x800 - 0x8FF*/
	uint32_t d_lockdown0; /* 0x900 */
	uint32_t i_lockdown0;
	uint32_t d_lockdown1;
	uint32_t i_lockdown1;
	uint32_t d_lockdown2;
	uint32_t i_lockdown2;
	uint32_t d_lockdown3;
	uint32_t i_lockdown3;
	uint32_t d_lockdown4;
	uint32_t i_lockdown4;
	uint32_t d_lockdown5;
	uint32_t i_lockdown5;
	uint32_t d_lockdown6;
	uint32_t i_lockdown6;
	uint32_t d_lockdown7;
	uint32_t i_lockdown7;
	uint32_t rsvd16[4]; /* 0x940 */
	uint32_t lock_line_en; /* 0x950 */
	uint32_t unlock_way;
	uint32_t rsvd17[42];
	uint32_t rsvd18[64]; /* 0xA00 */
	uint32_t rsvd19[64]; /* 0xB00 */
	uint32_t addr_filtering_start; /* 0xC00 */
	uint32_t addr_filtering_end;
	uint32_t rsvd20[62];
	uint32_t rsvd21[64]; /* 0xD00 */
	uint32_t rsvd22[64]; /* 0xE00 */
	uint32_t rsvd23[16]; /* 0xF00 - 0xF3F */
	uint32_t debug_ctrl; /* 0xF40 */
	uint32_t rsvd24[7];
	uint32_t prefetch_ctrl; /* 0xF60 */
	uint32_t rsvd25[7];
	uint32_t power_ctrl; /* 0xF80 */
} l2cc_reg_struct, * l2cc_reg_struct_ptr;

#endif /*__IPROC_REGS_H */