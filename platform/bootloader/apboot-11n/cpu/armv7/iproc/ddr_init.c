#include <config.h>
#include <common.h>
#if defined(CONFIG_RUN_DDR_SHMOO2) && defined(CONFIG_SHMOO_REUSE)
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include <spi_flash.h>
#include <nand.h>
#endif
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/iproc/reg_utils.h"
#include "asm/iproc/ddr40_phy_init.h"
#include "asm/iproc/shmoo_public.h"

#ifdef CONFIG_APBOOT 
#pragma GCC optimize("-O0")
#endif

extern unsigned int ddr_init_tab[];
extern unsigned int ddr2_init_tab[];
extern unsigned int ddr2_init_tab_667[];
extern unsigned int ddr2_init_tab_800[];
extern unsigned int ddr2_init_tab_1066[];
extern unsigned int ddr3_init_tab[];
extern unsigned int ddr3_init_tab_667[];
extern unsigned int ddr3_init_tab_800[];
extern unsigned int ddr3_init_tab_933[];
extern unsigned int ddr3_init_tab_1066[];
extern unsigned int ddr3_init_tab_1333[];
extern unsigned int ddr3_init_tab_1600[];
extern unsigned int ddr2_mode_reg_tab[];
extern unsigned int ddr3_mode_reg_tab[];
extern unsigned int ddr_clk_tab[];
extern unsigned int ddr_phy_ctl_ovrd_tab[];
extern unsigned int ddr_phy_wl0_tab[];
#ifdef CONFIG_APBOOT
extern void reset_cpu(ulong ignored);
#endif

#if defined(CONFIG_IPROC_DDR_ECC)
void iproc_ddr_ovrd_ecc_lane(void)
{
	uint32_t val;

#define SET_OVR_STEP(v) ( 0x30000 | ( (v) & 0x3F ) )    /* OVR_FORCE = OVR_EN = 1, OVR_STEP = v */

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_DM_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);


		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_EN);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

#if 0
		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DYN_VDL_OVRIDE_BYTE0_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DYN_VDL_OVRIDE_BYTE_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DYN_VDL_OVRIDE_BYTE0_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DYN_VDL_OVRIDE_BYTE_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DYN_VDL_OVRIDE_BYTE0_BIT_R_P);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DYN_VDL_OVRIDE_BYTE_BIT_R_P, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DYN_VDL_OVRIDE_BYTE0_BIT_R_N);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DYN_VDL_OVRIDE_BYTE_BIT_R_N, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DYN_VDL_OVRIDE_BYTE0_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DYN_VDL_OVRIDE_BYTE_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DYN_VDL_OVRIDE_BYTE0_BIT_W);
		val = SET_OVR_STEP(val & 0xff);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DYN_VDL_OVRIDE_BYTE_BIT_W, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_DATA_DLY);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_DATA_DLY, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_CONTROL);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_CONTROL, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);


		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_IDLE_PAD_CONTROL);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_IDLE_PAD_CONTROL, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DRIVE_PAD_CTL);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DRIVE_PAD_CTL, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#if 0
		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_CLOCK_PAD_DISABLE);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_CLOCK_PAD_DISABLE, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif	
		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_WR_PREAMBLE_MODE);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_WR_PREAMBLE_MODE, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#if 0
		val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_PHYBIST_VDL_ADJ);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_PHYBIST_VDL_ADJ, val);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
		__udelay(200);
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x1);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		__udelay(200);
}

uint32_t iproc_read_ecc_syndrome(void)
{
	volatile uint32_t syndrome = 0;
	/* Place uncorrectible as bits 7:0, and correctible as 15:8 */
	syndrome = ((reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) >> 3) & 0x1) |
				(((reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) >> 5) & 0x1));
	return(syndrome);
}

void iproc_clear_ecc_syndrome(void)
{
	uint32_t val;
#if 0
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_69);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_70);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_71);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_72);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_73);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_74);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_75);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_76);
	val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_77);
#endif
	/* Clear the interrupts, bits 6:3 */
	reg32_write((volatile uint32_t *)DDR_DENALI_CTL_213, (1 << 5) | (1<< 3));
	__udelay(1000);
}
#endif

#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2))
uint32_t iproc_get_ddr3_clock_mhz(uint32_t unit)
{
	uint32_t ndiv, mdiv, ddrclk;
	ndiv = reg32_read((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_4) & 0xff;
	mdiv = reg32_read((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_2) & 0xff; /* Ch1 MDIV */
	ddrclk = (25 * ndiv * 2) / mdiv;
	if(ddrclk == 466 || ddrclk == 666)
		ddrclk +=1;
	return(ddrclk);
}
#elif (defined(CONFIG_NS_PLUS))
uint32_t iproc_get_ddr3_clock_mhz(uint32_t unit)
{
	uint32_t ndiv, mdiv, ddrclk;
	/* read ndiv */
	ndiv = (reg32_read((volatile uint32_t *)CRU_LCPLL_CONTROL1_OFFSET) >> 20) & 0xff;
	/* read channel 2 mdiv */
	mdiv = (reg32_read((volatile uint32_t *)CRU_LCPLL_CONTROL2_OFFSET) >> 8) & 0xff;
	ddrclk = (25 * ndiv * 2) / mdiv;
	if(ddrclk == 466 || ddrclk == 666)
		ddrclk +=1;
	printf("%s ndiv(0x%x) mdiv(0x%x) ddrclk(0x%x)\n", __FUNCTION__, ndiv, mdiv, ddrclk);
	return(ddrclk);
}
#elif (defined(CONFIG_KATANA2))
uint32_t iproc_get_ddr3_clock_mhz(uint32_t unit)
{
	uint32_t ndiv, mdiv, ddrclk;
	
	/* read ndiv */
	ndiv = (reg32_read((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_3) >> 
		    IPROC_DDR_PLL_CTRL_REGISTER_3__NDIV_INT_R) & 
			((1 << IPROC_DDR_PLL_CTRL_REGISTER_3__NDIV_INT_WIDTH) - 1);
	
	/* read channel 0 mdiv */
	mdiv = (reg32_read((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_4) >> 
		IPROC_DDR_PLL_CTRL_REGISTER_4__CH0_MDIV_R) & 
		((1 << IPROC_DDR_PLL_CTRL_REGISTER_4__CH0_MDIV_WIDTH) - 1);
	
	
	ddrclk = (25 * ndiv * 2) / mdiv;
	if(ddrclk == 466 || ddrclk == 666)
		ddrclk +=1;
	printf("%s ndiv(0x%x) mdiv(0x%x) ddrclk(0x%x)\n", __FUNCTION__, ndiv, mdiv, ddrclk);
	return(ddrclk);
}

#endif
uint32_t change_ddr_clock(uint32_t clk)
{
	int i = 0;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2))
	uint32_t val;
#endif /* (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2)) */
#if (defined(CONFIG_NORTHSTAR) || defined(CONFIG_NS_PLUS))
	while(1) {
		if(ddr_clk_tab[i] == clk)
			break;
		if(ddr_clk_tab[i] == 0xffffffff) {
			printf("Error in change_ddr_clock: Can't find clock configuration requested\n");
			return(1);
		}
		i += 3;
	}

	/* unlock access to LCPLL registers */
	reg32_write((volatile uint32_t *)CRU_CLKSET_KEY_OFFSET, 0xea68);
	/* Program LCPLL NDIV */
	reg32_write_masked((volatile uint32_t *)CRU_LCPLL_CONTROL1_OFFSET, 0xff << 20, ddr_clk_tab[i + 1]);
	/* Program Channel dividers */
	reg32_write_masked((volatile uint32_t *)CRU_LCPLL_CONTROL2_OFFSET, 0xffffff00, ddr_clk_tab[i + 2]);
	/* Load new divider settings */
	reg32_set_bits((volatile uint32_t *)CRU_LCPLL_CONTROL0_OFFSET, 0x07);
	/* Hold new divider settings */
	reg32_clear_bits((volatile uint32_t *)CRU_LCPLL_CONTROL0_OFFSET, 0x07);
	/* Lock access to LCPLL registers */
	reg32_write((volatile uint32_t *)CRU_CLKSET_KEY_OFFSET, 0);
#elif (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2))
	while(1) {
		if(ddr_clk_tab[i] == clk)
			break;
		if(ddr_clk_tab[i] == 0xffffffff) {
			printf("Error in change_ddr_clock: Can't find clock configuration requested\n");
			return(1);
		}
		i += 8;
	}

	/* Program LCPLL NDIV */
	reg32_write_masked((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_4, 0xff, ddr_clk_tab[i + 1]);
	/* Program Channel dividers */
	reg32_write((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_0, 
		ddr_clk_tab[i + 7] << 24 | ddr_clk_tab[i + 6] << 16 | 
		ddr_clk_tab[i + 5] << 8 | ddr_clk_tab[i + 2]); /*ch 5,4,3,0 */
	reg32_write((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_2, 
		ddr_clk_tab[i + 4] << 20 | ddr_clk_tab[i + 3]); /*ch 2, 1 */

	/* Load new divider settings */
	val = ((IPROC_WRAP_IPROC_XGPLL_CTRL_1_RESETVALUE|
            (1<<IPROC_WRAP_IPROC_XGPLL_CTRL_1__SW_OVWR)) &
            (~(1<<IPROC_WRAP_IPROC_XGPLL_CTRL_1__POST_RESETB))&
            (~(1<<IPROC_WRAP_IPROC_XGPLL_CTRL_1__RESETB)));
	reg32_write((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_1, val);

	/* release the PLL reset */
	reg32_set_bits((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_1, (1<<IPROC_WRAP_IPROC_XGPLL_CTRL_1__RESETB));

   /* Wait for IPROC_XGPLL lock */
	do{
		val = reg32_read((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_STATUS);
	}while(!((val>>IPROC_WRAP_IPROC_XGPLL_STATUS__IPROC_WRAP_XGPLL_LOCK)&1));

	reg32_set_bits((volatile uint32_t *)IPROC_WRAP_IPROC_XGPLL_CTRL_1, (1<<IPROC_WRAP_IPROC_XGPLL_CTRL_1__POST_RESETB));

#elif defined(CONFIG_KATANA2)
	while(1) {
		if(ddr_clk_tab[i] == clk)
			break;
		if(ddr_clk_tab[i] == 0xffffffff) {
			printf("Error in change_ddr_clock: Can't find clock configuration requested\n");
			return(1);
		}
		i += 4;
	}

	reg32_clear_bits((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_5, 
	                 1 << IPROC_DDR_PLL_CTRL_REGISTER_5__IPROC_DDR_PLL_RESETB);	

	reg32_set_bits((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_5, 
		            (1<<IPROC_DDR_PLL_CTRL_REGISTER_5__IPROC_DDR_PLL_SW_OVWR));

	reg32_clear_bits((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_5, 
	            1 << IPROC_DDR_PLL_CTRL_REGISTER_5__IPROC_DDR_PLL_POST_RESETB);
	/* Program Ch1 (USB clock) divider */
	reg32_write_masked((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_5, 
		((1 << IPROC_DDR_PLL_CTRL_REGISTER_5__CH1_MDIV_WIDTH) - 1) << 
		IPROC_DDR_PLL_CTRL_REGISTER_5__CH1_MDIV_R, 
		ddr_clk_tab[i + 3] << IPROC_DDR_PLL_CTRL_REGISTER_5__CH1_MDIV_R);
	/* Program Ch0 (DDR clock) divider */
	reg32_write_masked((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_4, 
		((1 << IPROC_DDR_PLL_CTRL_REGISTER_4__CH0_MDIV_WIDTH) - 1) << 
		IPROC_DDR_PLL_CTRL_REGISTER_4__CH0_MDIV_R, 
		ddr_clk_tab[i + 2] << IPROC_DDR_PLL_CTRL_REGISTER_4__CH0_MDIV_R);

	/* Program ndiv */
	reg32_write_masked((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_3, 
		((1 << IPROC_DDR_PLL_CTRL_REGISTER_3__NDIV_INT_WIDTH) - 1) << 
		IPROC_DDR_PLL_CTRL_REGISTER_3__NDIV_INT_R, 
		ddr_clk_tab[i + 1] << IPROC_DDR_PLL_CTRL_REGISTER_3__NDIV_INT_R);
	
	/* release the PLL reset whih loads the new driver settings */
	reg32_set_bits((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_5, 
		            (1<<IPROC_DDR_PLL_CTRL_REGISTER_5__IPROC_DDR_PLL_RESETB));

   /* Wait for PLL lock */
	do{
		val = reg32_read((volatile uint32_t *)IPROC_DDR_PLL_STATUS);
	}while(!((val>>IPROC_DDR_PLL_STATUS__IPROC_DDR_PLL_LOCK)&1));

	reg32_set_bits((volatile uint32_t *)IPROC_DDR_PLL_CTRL_REGISTER_5, 
		       (1<<IPROC_DDR_PLL_CTRL_REGISTER_5__IPROC_DDR_PLL_POST_RESETB));
#endif

	return(0);
}

void dump_phy_regs(void)
{
#ifdef SHMOO_DEBUG
	int i;
	printf("\n PHY register dump: Control registers\n");
	for(i = 0; i <= 0xc4; i+=4)
	{
		printf("0x%03x,\t0x%08x,\n", i, *(volatile uint32_t *)(DDR_PHY_CONTROL_REGS_REVISION + i));
	}

	printf("\n PHY register dump: Wordlane0 registers\n");
	for(i = 0; i <= 0x1b0; i+=4)
	{
		printf("0x%03x,\t0x%08x,\n", i, *(volatile uint32_t *)(DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN + i));
	}

#endif	
}
 
void ddr_init_regs(unsigned int * tblptr)
{
	unsigned int offset = *tblptr;
	unsigned int *addr = (unsigned int *)DDR_DENALI_CTL_00;

	while(offset != 0xffffffff) {
		++tblptr;
		addr[offset] = *tblptr;
		++tblptr;
		offset = *tblptr;
	}
}

void ddr_phy_ctl_regs_ovrd(unsigned int * tblptr)
{
	unsigned int offset = *tblptr;
	unsigned int *addr = (unsigned int *)DDR_PHY_CONTROL_REGS_REVISION;
	unsigned int val;

	while(offset != 0xffffffff) {
		++tblptr;
		addr[offset/4] = *tblptr;
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		if (val) ;
		++tblptr;
		offset = *tblptr;
	}
}

void ddr_phy_wl_regs_ovrd(unsigned int * tblptr)
{
	unsigned int offset = *tblptr;
	unsigned int *addr = (unsigned int *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN;
	unsigned int val;

	while(offset != 0xffffffff) {
		++tblptr;
		if(offset < 0x118) {
		addr[offset/4] = *tblptr | (1 << 17);
		}
		else {
			addr[offset/4] = *tblptr;
		}
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		if (val) ;
		++tblptr;
		offset = *tblptr;
	}
}

/*DDR_SHMOO_RELATED_CHANGE*/
#if 0
uint32_t ddr_write_mode_regs(uint32_t * tblptr) {
	int i, j;

	reg32_clear_bits((volatile uint32_t *)DDR_DENALI_CTL_89, 1 << 18);
	for(i = 0; i < 4; i++) {
		reg32_write_masked((volatile uint32_t *)DDR_DENALI_CTL_47, 0xffff << 16, tblptr[i] << 16); // CS0
		reg32_write_masked((volatile uint32_t *)DDR_DENALI_CTL_51, 0xffff << 16, tblptr[i] << 16); // CS1
		reg32_write_masked((volatile uint32_t *)DDR_DENALI_CTL_55, 0xffff << 16, tblptr[i] << 16); // CS2
		reg32_write_masked((volatile uint32_t *)DDR_DENALI_CTL_59, 0xffff << 16, tblptr[i] << 16); // CS3
		/* Set mode register for single write for all chip selects */
		reg32_write((volatile uint32_t *)DDR_DENALI_CTL_43, 1 << 23 | 1 << 24 | i); 
		/* Trigger Mode Register Write(MRW) sequence */
		reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_43, 1 << 25);
		j = 100;
		do {
			if(reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) & (1 << 18))
				break;
			--j;
		}while(j);
		if (j == 0 && (reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) & (1 << 18)) == 0) {
			printf("Error: DRAM mode register write failed\n");
			return(1);
		}
	}
	return(0);
}
#endif

int ReWriteModeRegisters( void )
{
    /*DDR_SHMOO_RELATED_CHANGE: comment out calling ddr_write_mode_regs(), and add new implementation*/	
#if 0	
    unsigned int nRet = ddr_write_mode_regs( ddr3_mode_reg_tab );
    return nRet;
#endif    
    int nRet = 0;
    int j = 100;

    reg32_clear_bits( (volatile uint32_t *)DDR_DENALI_CTL_89 , 1 << 18 );

    /* Set mode register for MR0, MR1, MR2 and MR3 write for all chip selects */
    reg32_write( (volatile uint32_t *)DDR_DENALI_CTL_43 , (1 << 17) | (1 << 24) | (1 << 25) );

    /* Trigger Mode Register Write(MRW) sequence */
    reg32_set_bits( (volatile uint32_t *)DDR_DENALI_CTL_43 , 1 << 25 );

    do {
        if ( reg32_read( (volatile uint32_t *)DDR_DENALI_CTL_89) & (1 << 18) ) {
            break;
        }
        --j;
    } while( j );

    if ( j == 0 && (reg32_read( (volatile uint32_t *)DDR_DENALI_CTL_89) & (1 << 18) ) == 0 ) {
        printf("Error: DRAM mode registers write failed\n");
        nRet = 1;
    };

    return nRet;
}

extern uint32_t is_otp_programmed(uint32_t *regval);

#ifdef CONFIG_APBOOT
extern void iproc_clk_enum_2(unsigned);

char *ids[]  = { "12/16", "10/14", "11/15" };
char *revs[] = { "A", "B", "C", "D" };

int
print_cpuinfo(void)
{
	uint32_t dev_id_full;

	dev_id_full = (reg32_read((volatile uint32_t *)ChipcommonA_ChipID));
	printf("CPU:   BCM530%s, revision %s%d\n", ids[(dev_id_full >> 20) & 0x3],
		revs[(dev_id_full >> 18) & 0x3], (dev_id_full >> 16) & 0x3);

	return 0;
}
#endif

void ddr_init(void)
{
	int i;
	volatile unsigned int val;
	int ddr_type;
	uint32_t params, connect, ovride, status, otp_status = 0, sku_id, ddr_clk, dev_id;
	uint32_t wire_dly[4] = {0};

	dev_id = (reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff;
#ifndef CONFIG_APBOOT
	printf("DEV ID= %08x\n", dev_id);
#endif
	dev_id &= 0x0000ffff;

	sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 2) & 0x03;
	printf("SKU:   %d\n", sku_id);
#ifdef CONFIG_NORTHSTAR
	if (dev_id >= 0xcf19)	 {	/* 53017 */
		sku_id =0;
		printf("SKU ID (CoStar) = %d\n", sku_id);
	}

	status = is_otp_programmed(&otp_status);
	if(status) {
		printf("OTP status check failed\n");
		return;
	}

	printf("OTP:   0x%x\n", otp_status);
	/* Adjust the core volatage */
	/* Set MII clock to 125/8 MHz */
	reg32_write((volatile uint32_t *)PCU_MDIO_MGT_OFFSET, 8); // Fixme - or 0x88
	if(sku_id != 0) {
		/* not high sku, set it to 0.9V */
		reg32_write((volatile uint32_t *)PCU_MDIO_CMD_OFFSET, 1 << 30 | 1 << 28 | 4 << 23 | 3 << 18 | 2 << 16 | 0); 
	}
#endif

	ddr_type = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x1; 

#ifdef CONFIG_NORTHSTAR
	/* Workaround to configure DDR3 voltage to 1.5v */
	if(ddr_type) {
		reg32_write((volatile uint32_t *)PCU_1V8_1V5_VREGCNTL_OFFSET, 1 << 9);
	}
	/* Force DDR Power Enable, Enable LDO voltage output */
	reg32_write((volatile uint32_t *)PCU_AOPC_CONTROL_OFFSET, 0x80000000);
	reg32_write((volatile uint32_t *)PCU_AOPC_CONTROL_OFFSET, 0x80000001);


/*DDR_SHMOO_RELATED_CHANGE: force 800MHz for some 53012 with otp_status=0*/
   if ((sku_id == 0) && (otp_status == 0))
       otp_status=1;     

	if( !otp_status || (sku_id == 2)) {
		/* Mid SKU or non-OTP programmed, Data rate 1066*/
		ddr_clk = 533;
	}
	else if(sku_id == 1){
		/* Low SKU, Data rate 800 */
		ddr_clk = 400; 
	}
	else {
		/* High SKU, Data rate 1600 */
#ifdef CONFIG_DDR775
		ddr_clk = 775; 
#else
		ddr_clk = 800; 
#endif
	}
#elif CONFIG_HELIX4
#ifdef CONFIG_DDRCLK
	ddr_clk = CONFIG_DDRCLK;
#else
	ddr_clk = 800;
#endif
#elif CONFIG_HURRICANE2
#ifdef CONFIG_DDRCLK
	ddr_clk = CONFIG_DDRCLK;
#else
	ddr_clk = 533;
#endif
#else
	ddr_clk = 400; /* default value */
#endif

#ifdef CONFIG_APBOOT
	iproc_clk_enum_2(ddr_clk);
#else
	printf("DDR:   %d MHz\n", ddr_clk);
#endif
	status = change_ddr_clock(ddr_clk);
	if(status) {
		printf("CRU LCPLL configuration failed\n");
		return;
	}


	/* Get the DDR S1 and S2 out of reset */
	reg32_write((volatile uint32_t *)DDR_S1_IDM_RESET_CONTROL, 0);
	reg32_write((volatile uint32_t *)DDR_S2_IDM_RESET_CONTROL, 0);
	/* Set the ddr_ck to 400 MHz, 2x memc clock */
	reg32_write_masked((volatile uint32_t *)DDR_S1_IDM_IO_CONTROL_DIRECT, 0xfff << 16, /*ddr_clk*/ 0x190 << 16);     

	/* Wait for DDR PHY up */
	for(i=0; i < 0x19000; i++) {
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		if( val != 0)
			break; /* DDR PHY is up */
	}
	
	if(i == 0x19000) {
		printf("DDR PHY is not up\n");
		printf("REV: 0x%x\n", reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION));
		printf("DIR: 0x%x\n", reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_CONTROL_DIRECT));
		printf("IDM1: 0x%x\n", reg32_read((volatile uint32_t *)DDR_S1_IDM_RESET_CONTROL));
		printf("IDM2: 0x%x\n", reg32_read((volatile uint32_t *)DDR_S2_IDM_RESET_CONTROL));
		printf("AOPC: 0x%x\n", reg32_read((volatile uint32_t *)PCU_AOPC_CONTROL_OFFSET));
		printf("MDIC: 0x%x\n", reg32_read((volatile uint32_t *)PCU_MDIO_CMD_OFFSET));
		printf("MDIM: 0x%x\n", reg32_read((volatile uint32_t *)PCU_MDIO_MGT_OFFSET));
		printf("VREG: 0x%x\n", reg32_read((volatile uint32_t *)PCU_1V8_1V5_VREGCNTL_OFFSET));
		/* unlock access to LCPLL registers */
		printf("OFF1:  0x%x\n", reg32_read((volatile uint32_t *)CRU_LCPLL_CONTROL1_OFFSET));
		printf("OFF2:  0x%x\n", reg32_read((volatile uint32_t *)CRU_LCPLL_CONTROL2_OFFSET));
		printf("OFF0: 0x%x\n", reg32_read((volatile uint32_t *)CRU_LCPLL_CONTROL0_OFFSET));
		printf("OFF:  0x%x\n", reg32_read((volatile uint32_t *)CRU_CLKSET_KEY_OFFSET));
#if defined(CONFIG_APBOOT)
        reset_cpu(0);
#endif
		return;
	}

	if(otp_status && (sku_id == 0)) {
		/* High SKU */
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_DIVIDERS, 0x0c10);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
	}
#ifdef CONFIG_RUN_DDR_SHMOO
    // PHY init parameters
#ifdef CONFIG_DDR_LONG_PREAMBLE    
    params = DDR40_PHY_PARAM_USE_VTT |
             DDR40_PHY_PARAM_DIS_ODT |
             DDR40_PHY_PARAM_DIS_DQS_ODT |
             DDR40_PHY_PARAM_ODT_LATE |
             DDR40_PHY_PARAM_ADDR_CTL_ADJUST_0 |
             DDR40_PHY_PARAM_ADDR_CTL_ADJUST_1 |
             DDR40_PHY_PARAM_MAX_ZQ |
             DDR40_PHY_PARAM_LONG_PREAMBLE;
#else
     params = DDR40_PHY_PARAM_USE_VTT |
             DDR40_PHY_PARAM_DIS_ODT |
             DDR40_PHY_PARAM_DIS_DQS_ODT |
             DDR40_PHY_PARAM_ODT_LATE |
             DDR40_PHY_PARAM_ADDR_CTL_ADJUST_0 |
             DDR40_PHY_PARAM_ADDR_CTL_ADJUST_1 |
             DDR40_PHY_PARAM_MAX_ZQ;
#endif             
	
	if(ddr_type) {
		/* DDR3, 1.5v */
		params |= DDR40_PHY_PARAM_VDDO_VOLT_0;
	}
	else {
		/* DDR2, 1.8v */
		params |= DDR40_PHY_PARAM_VDDO_VOLT_1;
	}
    connect = 0x01CF7FFF;
    ovride = 0x00077FFF;
/*DDR_SHMOO_RELATED_CHANGE: change clock from 400 to ddr_clk*/    
	status = ddr40_phy_init(ddr_clk, params, ddr_type, wire_dly, connect, ovride, (uint32_t)DDR_PHY_CONTROL_REGS_REVISION);
	if(status != DDR40_PHY_RETURN_OK) {
		printf("Error: ddr40_phy_init failed with error 0x%x\n", status);
		return;
	}

#else

#if defined(CONFIG_HELIX4)
	/* mhz = 0x190, 32 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190c4f3);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
#if defined(CONFIG_HURRICANE2)
	/* mhz = 0x190, 16 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190d4f3);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
#if defined(CONFIG_HELIX4)
	/* ECC =1, DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0036048d);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
#if defined(CONFIG_HURRICANE2)
	/* DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0016048d);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif


	/* Set LDO output voltage control to 1.00 * VDDC, and enable PLL */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG, 1 << 4);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	/* Wait for PLL locked */
	for(i=0; i < 0x1400; i++) {
		if(reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_STATUS) & 0x1)
			break; /* PLL locked */
	}

	if(i == 0x1400) {
		printf("DDR PHY PLL lock failed\n");
		return;
	}

	/* Set Read Data delay to 2 clock cycles */
	if(otp_status && (sku_id == 0)) {
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_DATA_DLY, 3);
	}
	else {
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_DATA_DLY, 2);
	}
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#if defined(CONFIG_HELIX4)
	reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_READ_DATA_DLY, 2);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
	
	/* Write 2 if ddr2, 3 if ddr3 */
	/* Set preamble mode according to DDR type, and length of write preamble to 1.5 DQs, 0.75 DQ  */
	val = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS);
#ifdef CONFIG_DDR_LONG_PREAMBLE	
	val = (val & 1) | 0x2;
#else
	val = (val & 1);
#endif	 
	reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_WR_PREAMBLE_MODE, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#if defined(CONFIG_HELIX4)
	reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_WR_PREAMBLE_MODE, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif

	/* Initiate a PVT calibration cycle */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTL, 1 << 20);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	/* Initiate auto calibration and apply the results to VDLs */
	if(otp_status && (sku_id == 0)) {
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VDL_CALIBRATE, 0x08000101);
	}
	else {
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VDL_CALIBRATE, 0x00000101);
	}
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	/* Wait for Calibration lock done */
	for(i=0; i < 0x1400; i++) {
		if(reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS) & 0x1)
			break; /* PLL locked */
	}

	if(i == 0x1400) {
		printf("DDR PHY auto calibration timed out\n");
		return;
	}

	if(reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS) & 0x2) {
#if 0
		printf("PHY register dump after the auto calibration\n");
		dump_phy_regs();
#endif
		goto ddr_cntrl_prog;
	}
        
	/* Auto calibration failed, do the override */
calib_override:
		printf("Auto calibration failed, do the override\n");
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTL, 0x0001003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_DM_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_P, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_N, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_P, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_N, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#if defined(CONFIG_HELIX4)
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_DM_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_DM_W, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_P, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_N, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_P, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_N, 0x0003003f);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif

#endif
#ifndef CONFIG_RUN_DDR_SHMOO
ddr_cntrl_prog:
#endif
#ifdef CONFIG_RUN_DDR_SHMOO
#ifdef SHMOO_DEBUG
	printf("PHY register dump after DDR PHY init\n");
#endif
	dump_phy_regs();
#endif

	ddr_init_regs(ddr_init_tab);
#ifdef CONFIG_NORTHSTAR
	if( !otp_status || (sku_id == 2)) {
		/* Mid SKU or non-OTP programmed, Data rate 1066*/
		ddr_init_regs(ddr3_init_tab);
	}
	else if(sku_id == 1){
		/* Low SKU, Data rate 800 */
		extern unsigned int ddr2_init_tab_400[];
		ddr_init_regs(ddr2_init_tab_400);
	}
	else {
		/* High SKU, Data rate 1600 */
		ddr_init_regs(ddr3_init_tab_1600);
	}
#else
	if(ddr_type) {
		/* DDR3 */	
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2))
		switch(ddr_clk) {
			case 333:
				ddr_init_regs(ddr3_init_tab_667);
				break;
			case 400:
				ddr_init_regs(ddr3_init_tab_800);
				break;
			case 467:
				ddr_init_regs(ddr3_init_tab_933);
				break;
			case 533:
				ddr_init_regs(ddr3_init_tab_1066);
				break;
			case 666:
				ddr_init_regs(ddr3_init_tab_1333);
				break;
			case 800:
				ddr_init_regs(ddr3_init_tab_1600);
				break;
		}

#else
		ddr_init_regs(ddr3_init_tab);
#endif
	}

	else {
		/* DDR2 */	
		ddr_init_regs(ddr2_init_tab);
	}
#endif

#ifdef CONFIG_NORTHSTAR
/*DDR_SHMOO_RELATED_CHANGE: no need to update these three registers if shmoo enabled*/
#ifndef CONFIG_RUN_DDR_SHMOO
	if(otp_status && (sku_id == 0)) 
	{
		/* High SKU, DDR-1600 */
		/* Auto initialization fails at DDRCLK 800 MHz, manually override */
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN, 0x00010120);
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_EN, 0x0001000d);
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_EN, 0x00010020);
	}
#endif

#endif

//turnon:
	/* Start the DDR */
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_00, 0x01);

//poll_ddr_ctrl:
#if defined(CONFIG_NORTHSTAR)
	while(!(reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) & 0x200));
#else
	while(!(reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) & 0x100));
#endif
	if(otp_status && (sku_id == 0)) {
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_DATA_DLY, 0x03);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);	
	}
	if(otp_status && (sku_id == 2)) {
		reg32_write((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_DATA_DLY, 0x02);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);	
	}


#ifdef CONFIG_RUN_DDR_SHMOO

/*DDR_SHMOO_RELATED_CHANGE: mark out mode register update as it should already be programmed before "turnon"*/
#if 0
	/* Program DDR controller mode registers */
	if(ddr_type) {
		/* DDR3 */	
		if(ddr_write_mode_regs(ddr3_mode_reg_tab)) {
			printf("Error: DRAM mode register write failed\n");
			return;
		}
	}
	else {
		/* DDR2 */	
		if(ddr_write_mode_regs(ddr2_mode_reg_tab)) {
			printf("Error: DRAM mode register write failed\n");
			return;
		}
	}
#endif

#ifdef SHMOO_DEBUG
	printf("PHY register dump after mode register write\n");
#endif
	dump_phy_regs();

	/* Run the SHMOO */
	/// Turn on all SHMOO options for NORTHSTAR - 16 bit PHY
/*DDR_SHMOO_RELATED_CHANGE: chanhe shmoo memory test base from 0x0 (write-through) to 0x1000000 (write-back)*/
	status = do_shmoo(/*(ddr40_addr_t)*/DDR_PHY_CONTROL_REGS_REVISION, 0, ((26 << 16) | (16 << 8) | DO_ALL_SHMOO), 0x1000000);
	if(status != SHMOO_NO_ERROR) {
		printf("Error: do_shmoo failed with error 0x%x\n", status);
		return;
	}


//DDR-EYE-SHMOO: added           
#ifdef CONFIG_MDE_BUILD_RW_EYE 
	{
		volatile unsigned int tmp=10000; //1000 is also OK
		extern unsigned mde_entry (void);

		*(unsigned int *)(0x1800c180) = 0xea68;                             // unlock
		*(unsigned int *)(0x1800c154) = 0x83c00000;     // write bit31=1 , sel_SW_setting
		while(tmp--);  //delay for correct read
		tmp = *(unsigned int *)(0x1800c15c);
		if( tmp & (1<<4) ) { //bit 4 is set as 1 by "shmoo" command
			//restore to original value
			*(unsigned int *)(0x1800c180) = 0xea68;                        // unlock
			*(unsigned int *)(0x1800c15c) = 0x0003060c;               // bit[7:4]: reset indicator
			*(unsigned int *)(0x1800c154) = 0x83c00000;               // write bit31=1 , sel_SW_setting
			#warning: MDE to run in non-relocated space in FLASH....
			printf ("Forcing to MDE entry at %08x\n", (unsigned) mde_entry );
			mde_entry ();
		}
	}
#endif /*CONFIG_MDE_BUILD_RW_EYE*/
#endif /* CONFIG_RUN_DDR_SHMOO */

}

#if 0
static uint32_t get_ddr_clock(uint32_t sku_id, int ddr_type)
{
#ifdef CONFIG_DDRCLK
	return CONFIG_DDRCLK;
#else
    uint32_t ddr_clk;
#if defined(CONFIG_HELIX4)
	if(ddr_type) 
		ddr_clk = 750; //ddr3 
	else
		ddr_clk = 400; //ddr2 
#elif defined(CONFIG_NS_PLUS)
	ddr_clk = 800;
#elif defined(CONFIG_HURRICANE2)
	/* Set the defaults according to SKU */
	if(sku_id == 0x8344) {
		ddr_clk = 400; /* Wolfhound - BCM53344 */
	}
	else if(sku_id == 0xb151) {
		ddr_clk = 400; /* Deerhound */
	}
	else {
		ddr_clk = 667; /* Hurricane2 - BCM56150 */
	}
#elif defined(CONFIG_KATANA2)
	if(ddr_type) 
		ddr_clk = 800; //ddr3 
	else
		ddr_clk = 400; //ddr2 
#endif
	return ddr_clk;
#endif
}
#endif

#if defined(CONFIG_RUN_DDR_SHMOO2) && defined(CONFIG_SHMOO_REUSE)

#define SHMOO_HEADER_MAGIC      "SHMO"
#define SHMOO_MIN_BLOCK_SIZE    0x10000

struct shmoo_sig_t {
    char            magic[4];
    
    uint32_t        dev_id:16;
    uint32_t        sku_id:16;
    
    uint32_t        ddr_type:2;
    uint32_t        ddr_clock:12;
    uint32_t        reserved1:18;
};

static const uint16_t ddr_phy_ctl_regs[] = {
    0x030,
    0x034,
    0x06c
};

static const uint16_t ddr_phy_wl_regs[] = {
    0x000,
    0x004,
    0x008,
    0x00c,
    0x010,
    0x014,
    0x018,
    0x01c,
    0x020,
    0x024,
    0x028,
    0x02c,
    0x030,
    0x034,
    0x038,
    0x03c,
    0x040,
    0x044,
    0x048,
    0x04c,
    0x050,
    0x054,
    0x058,
    0x05c,
    0x060,
    0x064,
    0x068,
    0x06c,
    0x070,
    0x074,
    0x0a4,
    0x0a8,
    0x0ac,
    0x0b0,
    0x0b4,
    0x0b8,
    0x0bc,
    0x0c0,
    0x0c4,
    0x0c8,
    0x0cc,
    0x0d0,
    0x0d4,
    0x0d8,
    0x0dc,
    0x0e0,
    0x0e4,
    0x0e8,
    0x0ec,
    0x0f0,
    0x0f4,
    0x0f8,
    0x0fc,
    0x100,
    0x104,
    0x108,
    0x10c,
    0x110,
    0x114,
    0x118,
    0x11c,
    0x120,
    0x124,
    0x128,
    0x12c,
    0x130,
    0x134,
    0x138,
    0x13c,
    0x140,
    0x144,
    0x148,
    0x14c,
    0x150,
    0x154,
    0x158,
    0x15c,
    0x160,
    0x164,
    0x168,
    0x16c,
    0x1a0,
    0x1a4,
    0x1a8,
    0x1ac,
    0x1b0
};
#ifdef CONFIG_IPROC_DDR_ECC
static const uint16_t ddr_phy_eccl_regs[] = {
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_W_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_P_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_N_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_W_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_W_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_W_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_W_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_DM_W_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_P_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_N_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_P_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_N_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_P_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_N_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_P_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_N_BASE,
DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN_BASE,
DDR_PHY_ECC_LANE_READ_DATA_DLY_BASE,
DDR_PHY_ECC_LANE_READ_CONTROL_BASE,
DDR_PHY_ECC_LANE_IDLE_PAD_CONTROL_BASE,
DDR_PHY_ECC_LANE_DRIVE_PAD_CTL_BASE,
DDR_PHY_ECC_LANE_WR_PREAMBLE_MODE_BASE,
};
#endif
#if defined(CONFIG_IPROC_NAND) && defined(CONFIG_ENV_IS_IN_NAND) && CONFIG_ENV_IS_IN_NAND

static int write_shmoo_to_flash(void *buf, int length)
{
    nand_info_t *nand;
    int ret = 0;
    uint32_t offset = CONFIG_SHMOO_REUSE_NAND_OFFSET;
    uint32_t end = offset + CONFIG_SHMOO_REUSE_NAND_RANGE;
    uint32_t blksize;
    
    /* Get flash handle */
    nand = &nand_info[0];
    if (nand->size < offset || nand->writesize == 0 || nand->erasesize == 0) {
        printf("Failed to initialize NAND flash for saving Shmoo values!\n");
        return -1;
    }

    /* For NAND with bad blocks, we always erase all blocks in the range */
    {
        nand_erase_options_t opts;
        memset(&opts, 0, sizeof(opts));
        opts.offset = offset / nand->erasesize * nand->erasesize;
        opts.length = (CONFIG_SHMOO_REUSE_NAND_RANGE - 1) / nand->erasesize * nand->erasesize + 1;
        opts.quiet  = 1;
        ret = nand_erase_opts(nand, &opts);
        if (ret) {
            printf("NAND flash erase failed, error=%d\n", ret);
            return ret;
        }
    }
    
    /* Write data */
    blksize = nand->erasesize > SHMOO_MIN_BLOCK_SIZE? 
        nand->erasesize : SHMOO_MIN_BLOCK_SIZE;
    while (offset < end) {
        if (nand_block_isbad(nand, offset)) {
            offset += blksize;
            continue;
        }
        ret = nand_write(nand, offset, (size_t *)&length, (u_char *)buf);
        if (ret) {
            printf("NAND flash write failed, error=%d\n", ret);
        }
        break;
    }

    return ret;
}

#elif defined (CONFIG_SPI_FLASH) && defined(CONFIG_ENV_IS_IN_SPI_FLASH) && CONFIG_ENV_IS_IN_SPI_FLASH

static int write_shmoo_to_flash(void *buf, int length)
{
    struct spi_flash *flash;
    int erase = 0;
    volatile uint32_t *flptr;
    int i, j, ret = 0;
    uint32_t offset = CONFIG_SHMOO_REUSE_QSPI_OFFSET;
    
    /* Check if erasing is required */
    flptr = (volatile uint32_t *)(IPROC_QSPI_MEM_BASE + offset / 4 * 4);    
    j = (length - 1) / 4 + 1;
    for(i=0; i<j; i++, flptr++) {
        if (*flptr != 0xFFFFFFFF) {
            erase = 1;
            break;
        }
    }
    
    /* Probe flash */
    flash = spi_flash_probe(
                CONFIG_ENV_SPI_BUS, 
                CONFIG_ENV_SPI_CS, 
                CONFIG_ENV_SPI_MAX_HZ, 
                CONFIG_ENV_SPI_MODE
                );
    if (!flash) {
        printf("Failed to initialize SPI flash for saving Shmoo values!\n");
        return -1;
    }
    
    /* Erase if necessary */
    if (erase) {
        ret = spi_flash_erase(
                flash, 
                offset / flash->sector_size * flash->sector_size, 
                flash->sector_size
                );
        if (ret) {
            printf("SPI flash erase failed, error=%d\n", ret);
            spi_flash_free(flash);
            return ret;
        }
    }
    
    /* Write data */
    ret = spi_flash_write(flash, offset, length, buf);
    if (ret) {
        printf("SPI flash write failed, error=%d\n", ret);
    }
   
    /* Free flash instance */
    spi_flash_free(flash);
    
    return ret;
}

#elif defined (CONFIG_ENV_IS_IN_FLASH)

static int write_shmoo_to_flash(void *buf, int length)
{
    int erase = 0;
    volatile uint32_t *flptr, shmoo_start;
    int i, j, ret = 0;
    uint32_t offset = CONFIG_SHMOO_REUSE_NOR_OFFSET;
    int sect_len;

    /* Check if erasing is required */
    flptr = (volatile uint32_t *)(IPROC_NOR_MEM_BASE + offset / 4 * 4);
    shmoo_start = flptr;
    j = (length - 1) / 4 + 1;
    for(i=0; i<j; i++, flptr++) {
        if (*flptr != 0xFFFFFFFF) {
            erase = 1;
            break;
        }
    }

    sect_len = (((length / 0x20000) + 1)*0x20000 - 1);   
    /* Erase if necessary */
    if (erase) {
        ret = flash_sect_erase((ulong)shmoo_start, (ulong)shmoo_start + sect_len);
        if (ret) {
            printf("NOR flash erase failed, error=%d, start addr: 0x%x, end addr: 0x%x\n", 
                            ret, (ulong)shmoo_start, (ulong)shmoo_start + sect_len);
            return ret;
        }
    }

    /* Write data */
    ret = flash_write((char *)buf, (ulong)shmoo_start, length);

    if (ret) {
        printf("NOR flash write failed, error=%d\n", ret);
    }


    return ret;

}
#else
 #error Flash (SPI or NAND) must be enabled 
#endif

#define RAND_MAGIC_1    0x0000444BUL
#define RAND_MAGIC_2    0x88740000UL
#define RAND_MAGIC_3    69069UL
#define RAND_SEED       0x5301beef
#define RAND_SEED_2     ((RAND_SEED << 21) + (RAND_SEED << 14) + (RAND_SEED << 7))
#define RAND_C_INIT     (((RAND_SEED_2 + RAND_MAGIC_1) << 1) + 1)
#define RAND_T_INIT     ((RAND_SEED_2 << (RAND_SEED_2 & 0xF)) + RAND_MAGIC_2)

static int simple_memory_test(void *start, uint32_t len)
{
    register uint32_t rand_c_value, rand_t_value, rand_value;
    register uint32_t i;
    register volatile uint32_t *paddr;
    
    len /= 4;
    paddr = (volatile uint32_t *)start;
    rand_c_value = RAND_C_INIT;
    rand_t_value = RAND_T_INIT;
    for(i=0; i<len; i++, paddr++) {
        rand_c_value *= RAND_MAGIC_3;
        rand_t_value ^= rand_t_value >> 15;
        rand_t_value ^= rand_t_value << 17;
        rand_value = rand_t_value ^ rand_c_value;
        *paddr = rand_value;
    }
    
    paddr = (volatile uint32_t *)start;
    rand_c_value = RAND_C_INIT;
    rand_t_value = RAND_T_INIT;
    for(i=0; i<len; i++, paddr++) {
        rand_c_value *= RAND_MAGIC_3;
        rand_t_value ^= rand_t_value >> 15;
        rand_t_value ^= rand_t_value << 17;
        rand_value = rand_t_value ^ rand_c_value;
        if (*paddr != rand_value) {
            return -1;
        }
    }
    
    return 0;
}

/* Return flash pointer; or NULL if validation failed */
static volatile uint32_t *validate_flash_shmoo_values(struct shmoo_sig_t *psig, int *ppairs)
{
	uint32_t dev_id, sku_id, ddr_type, ddr_clk;
    volatile uint32_t *ptr;
    volatile uint32_t *flptr;
    struct shmoo_sig_t sig;
    uint32_t checksum, pairs, length;
    uint32_t chksum;
    int offset;
    int i;

    /* Calculate required length (register/value pair) */
    pairs = 
        sizeof(ddr_phy_ctl_regs) / sizeof(ddr_phy_ctl_regs[0]) + 
        sizeof(ddr_phy_wl_regs) / sizeof(ddr_phy_wl_regs[0]) * 
        (CONFIG_SHMOO_REUSE_DDR_32BIT? 2 : 1);
#ifdef CONFIG_IPROC_DDR_ECC
	pairs += sizeof(ddr_phy_eccl_regs) / sizeof(ddr_phy_eccl_regs[0]);
#endif
        
    if (ppairs != NULL) {
        *ppairs = pairs;
    }
    
#if defined(CONFIG_ENV_IS_IN_NAND) && CONFIG_ENV_IS_IN_NAND
    /* Read SHMOO data from NAND */
    flptr = (volatile uint32_t *)(IPROC_NAND_MEM_BASE + CONFIG_SHMOO_REUSE_NAND_OFFSET);
    offset = (CONFIG_SHMOO_REUSE_NAND_RANGE - 1) / SHMOO_MIN_BLOCK_SIZE * SHMOO_MIN_BLOCK_SIZE;
#elif defined (CONFIG_ENV_IS_IN_FLASH) 
    /* Read SHMOO data from NOR */
    flptr = (volatile uint32_t *)(IPROC_NOR_MEM_BASE + CONFIG_SHMOO_REUSE_NOR_OFFSET);
    offset = 0;
#else
    /* Read SHMOO data from SPI */
    flptr = (volatile uint32_t *)(IPROC_QSPI_MEM_BASE + CONFIG_SHMOO_REUSE_QSPI_OFFSET);
    offset = 0;
#endif
    
    /* Get chip type and DDR type/clock */
    dev_id = (reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff;
#ifdef CONFIG_HURRICANE2
    sku_id = (reg32_read((volatile uint32_t *)CMIC_DEV_REV_ID)) & 0x0000ffff;
#else
    sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 2) & 0x03;
#endif
    ddr_type = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x1; 
    ddr_clk = get_ddr_clock(sku_id, ddr_type);

    /* Construct signature */
    memcpy(sig.magic, SHMOO_HEADER_MAGIC, 4);
    sig.dev_id = dev_id;
    sig.sku_id = sku_id;
    sig.ddr_type = ddr_type;
    sig.ddr_clock = ddr_clk;
    sig.reserved1 = 0;
    
    /* Provide signature data to caller */
    if (psig) {
        memcpy(psig, &sig, sizeof(sig));
    }
    
    /* Check signature (in min-blocks from bottom) */
    while (offset >= 0) {
        ptr = flptr + offset;
        if (!memcmp((void *)ptr, &sig, sizeof(sig))) {
            break;
        }
        offset -= SHMOO_MIN_BLOCK_SIZE;
    }
    if (offset < 0) {
        return NULL;
    }
    ptr += sizeof(sig) / 4;
    
    /* Verify checksum */
    checksum = *ptr++;
    length = *ptr++;
    if (pairs != length) {
        /* Pair count unmatched */
        return NULL;
    }
    chksum = 0;
    for(i=0; i<length * 2; i++, ptr++) {
        chksum += *ptr;
    }
    if (chksum != checksum) {
        return NULL;
    }
    
    return flptr + offset;
}

static int try_restore_shmoo(void)
{
    int invalid = 0;
    struct shmoo_sig_t sig;
    volatile uint32_t *flptr;
    volatile uint32_t *reg;
    int pairs, i;
    
    /* Validate values in flash */
    printf("Validate Shmoo parameters stored in flash ..... ");
    flptr = validate_flash_shmoo_values(&sig, &pairs);
    if (flptr == NULL) {
        printf("failed\n");
        return 1;
    }
    printf("OK\n");

    /* Check if user wants to skip restoring and run Shmoo */
    if (CONFIG_SHMOO_REUSE_DELAY_MSECS > 0) {
        char c = 0;
        unsigned long start;
        printf("Press Ctrl-C to run Shmoo ..... ");
        start = get_timer(0);
        while(get_timer(start) <= CONFIG_SHMOO_REUSE_DELAY_MSECS) {
            if (tstc()) {
                c = getc();
                if (c == 0x03) {
                    printf("Pressed.\n");
                    printf("Do you want to run the Shmoo? [y/N] ");
                    for(;;) {
                        c = getc();
                        if (c == 'y' || c == 'Y') {
                            printf("Y\n");
                            invalid = 1;
                            break;
                        } else if (c == '\r' || c == 'n' || c == 'N') {
                            if (c != '\r')
                                printf("N\n");
                            break;
                        }
                    }
                    break;
                } else {
                    c = 0;
                }
            }
        }
        if (c == 0) 
            printf("skipped\n");
    }
    
    if (invalid) {
        return 1;
    }
    
    /* Restore values from flash */
    printf("Restoring Shmoo parameters from flash ..... ");
    flptr += sizeof(struct shmoo_sig_t) / 4 + 2;
    for(i=0; i<pairs; i++) {
        reg = (volatile uint32_t *)(*flptr++);
        *reg = *flptr++;
        (void)*reg; /* Dummy read back */
    }
    printf("done\n");
    
    /* Perform memory test to see if the parameters work */
    if (CONFIG_SHMOO_REUSE_MEMTEST_LENGTH > 0 ) {
        printf("Running simple memory test ..... ");
        i = simple_memory_test(
            (void *)CONFIG_SHMOO_REUSE_MEMTEST_START,
            CONFIG_SHMOO_REUSE_MEMTEST_LENGTH);
        if (i) {
            printf("failed!\n");
            return 1;
        }
        printf("OK\n");
    }
    
    return 0;
}

void iproc_save_shmoo_values(void)
{
    uint32_t *buffer, *ptr;
    volatile uint32_t *flptr;
    uint32_t reg, val;
    struct shmoo_sig_t sig;
    int pairs, length;
    uint32_t chksum;
    int i;

    /* Check if flash already contains valid data  */
    flptr = validate_flash_shmoo_values(&sig, &pairs);
    if (flptr != NULL) {
        /* Check if the flash data are the same as current DDR PHY values */
        flptr += sizeof(struct shmoo_sig_t) / 4 + 2;
		for(i=0; i<pairs; i++) {
			reg = *flptr++;
			val = *flptr++;
			if (*(volatile uint32_t *)reg != val) {
				break;
			}
		}
		if (i == pairs) {
			/* No difference found; Saving skipped */
			return;
		}
    }
    
    /* Calculate size of buffer */
    length = 
        sizeof(struct shmoo_sig_t) + 
        sizeof(uint32_t) * 2 + 
        sizeof(uint32_t) * pairs * 2;
        
    /* Allocate buffer */
    buffer = malloc(length);
    if (buffer == NULL) {
        printf("Error allocating memory for saving Shmoo values!\n");
        return;
    }
    ptr = buffer;
    
    /* Fill signature */
    memcpy(ptr, &sig, sizeof(sig));
    ptr += sizeof(struct shmoo_sig_t) / 4 + 2;
    
    /* Copy registers and values to buffer */
    chksum = 0;
    for(i=0; i<sizeof(ddr_phy_ctl_regs) / sizeof(ddr_phy_ctl_regs[0]); i++) {
        reg = (uint32_t)DDR_PHY_CONTROL_REGS_REVISION + ddr_phy_ctl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        val = *(volatile uint32_t *)reg;
        *ptr++ = val;
        chksum += val;
    }
    for(i=0; i<sizeof(ddr_phy_wl_regs) / sizeof(ddr_phy_wl_regs[0]); i++) {
        reg = (uint32_t)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN + ddr_phy_wl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        val = *(volatile uint32_t *)reg;
        *ptr++ = val;
        chksum += val;
    }
#if CONFIG_SHMOO_REUSE_DDR_32BIT
    for(i=0; i<sizeof(ddr_phy_wl_regs) / sizeof(ddr_phy_wl_regs[0]); i++) {
        reg = (uint32_t)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_EN + ddr_phy_wl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        val = *(volatile uint32_t *)reg;
        *ptr++ = val;
        chksum += val;
    }
#endif
#ifdef CONFIG_IPROC_DDR_ECC
    for(i=0; i<sizeof(ddr_phy_eccl_regs) / sizeof(ddr_phy_eccl_regs[0]); i++) {
        reg = (uint32_t)DDR_DENALI_CTL_00 + ddr_phy_eccl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        val = *(volatile uint32_t *)reg;
        *ptr++ = val;
        chksum += val;
    }
#endif

    /* Fill checksum and length */
    buffer[sizeof(struct shmoo_sig_t) / 4] = chksum;
    buffer[sizeof(struct shmoo_sig_t) / 4 + 1] = pairs;
    
    /* Write to flash */
    printf("Writing Shmoo values into flash .....\n");
    i = write_shmoo_to_flash(buffer, length);
    
    /* Free buffer */
    free(buffer);
}

#endif /* CONFIG_RUN_DDR_SHMOO2 && CONFIG_SHMOO_REUSE */

#if !(defined(CONFIG_NORTHSTAR))
#include "asm/iproc/ddr40.h"
#include "asm/iproc/shmoo_ddr40.h"
#include "asm/iproc/ddr_bist.h"


static int clear_ddr(uint32_t offset, uint32_t size)
{
	unsigned long start;
	unsigned int i, val;

	*(unsigned int *)(0x18010400) &= ~0x1;

    for( i = 0; i < 1000; i++);
	reg32_write((volatile uint32_t *)DDR_DENALI_CTL_213, 0x00FFFFFF);
	reg32_write((volatile uint32_t *)DDR_BistConfig, 0x00000002);
	reg32_write((volatile uint32_t *)DDR_BistConfig, 0x00000003);
	reg32_write((volatile uint32_t *)DDR_BistConfig, 0x0000C003);
	reg32_write((volatile uint32_t *)DDR_BistGeneralConfigurations, 0x00000020);

	val =  255 << DDR_BistConfigurations__WriteWeight_R | 
		  0 << DDR_BistConfigurations__ReadWeight_R |
		  1 << DDR_BistConfigurations__ConsAddr8Banks;

	reg32_write((volatile uint32_t *)DDR_BistConfigurations, val);


	reg32_write((volatile uint32_t *)DDR_BistStartAddress, offset);
	reg32_write((volatile uint32_t *)DDR_BistEndAddress, (1 << DDR_BistEndAddress__BistEndAddress_WIDTH) - 1);
	reg32_write((volatile uint32_t *)DDR_BistNumberOfActions, (size + 31) / 32);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord0, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord1, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord2, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord3, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord4, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord5, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord6, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord7, 0);

	reg32_set_bits((volatile uint32_t *)DDR_BistConfigurations, 1 << DDR_BistConfigurations__IndWrRdAddrMode);
	
	reg32_set_bits((volatile uint32_t *)DDR_BistConfigurations, 1 << DDR_BistConfigurations__BistEn);

	start = get_timer(0);
	while(get_timer(start) <= 10000) {
		if(reg32_read((volatile uint32_t *)DDR_BistStatuses) & (1 << DDR_BistStatuses__BistFinished))
			break;
	}

	if((get_timer(start) <= 10000)  &&
	   (!reg32_read((volatile uint32_t *)DDR_BistErrorOccurred)))
	{
		printf("clear_ddr: OK\n");
		return(0);
	}
	printf("clear_ddr: Failed: 0x%lx\n", get_timer(start));
	if(reg32_read((volatile uint32_t *)DDR_BistErrorOccurred))
		printf("clear_ddr: Error occured\n");
	return(1);
}

void ddr_init2(void)
{
	int i;
	volatile unsigned int val;
	int ddr_type;
	uint32_t params, connect, ovride, status, otp_status, sku_id, ddr_clk, dev_id;
	uint32_t wire_dly[4] = {0};
	uint32_t unit = 0;
	int ddr32 = 0;

	dev_id = (reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff;
	printf("DEV ID = 0x%x\n", dev_id);
#if (defined(CONFIG_NS_PLUS))
	/* get device id from OTP */
	/* Write 0 to 0x18012120 (PAXB_0_CONFIG_IND_ADDR) */
	reg32_write((volatile uint32_t *)PAXB_0_CONFIG_IND_ADDR, 0);
	/* Read 0x18012124 (PAXB_0_CONFIG_IND_DATA), bits 31:16 will be the device id from OTP space */
	dev_id = reg32_read((volatile uint32_t *)PAXB_0_CONFIG_IND_DATA) >> 16;
	/* 0x8022:53022; 0x8023:53023; 0x8025:53025 */
	printf("PCIE CFG DEV ID = 0x%x\n", dev_id);

	for (i=8; i<=0xf; i++) {
		status = get_otp_mem(i, &otp_status);
		if (status) {
			printf("OTP status check failed\n");
			break;
		}
		printf("OTP offset(0x%x): 0x%x\n", i, otp_status);
	}

	// Check if NSP 25.  If so then 32bit mode
	// OTP[0xc] bits 31:21 => pcie_device_id[10:0]
	// if pcie_device_id == 0x25 then 32bit mode
	status = get_otp_mem(0x0c, &otp_status);
	if (!status) {
		printf("OTP[0xc]: 0x%x\n", otp_status);
		if ( ((otp_status>>21)&0x07ff) == 0x25) {
			printf("NSP25 32bit DDR\n");
			ddr32=1;
		}
	}

#endif /* (defined(CONFIG_NS_PLUS)) */


#ifdef CONFIG_HURRICANE2
	sku_id = (reg32_read((volatile uint32_t *)CMIC_DEV_REV_ID)) & 0x0000ffff;
#else
	sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 2) & 0x03;
#endif
	printf("SKU ID = 0x%x\n", sku_id);

	ddr_type = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x1; 
	printf("DDR type: DDR%d\n", (ddr_type) ? 3 : 2);
    
	ddr_clk = get_ddr_clock(sku_id, ddr_type);
	printf("MEMC 0 DDR speed = %dMHz\n", ddr_clk);

	status = change_ddr_clock(ddr_clk);
	if(status) {
		printf("CRU LCPLL configuratioin failed\n");
		return;
	}

	/* Get the DDR S1 and S2 out of reset */
	reg32_write((volatile uint32_t *)DDR_S1_IDM_RESET_CONTROL, 0);
	reg32_write((volatile uint32_t *)DDR_S2_IDM_RESET_CONTROL, 0);
	/* Set the ddr_ck to 400 MHz, 2x memc clock */
	reg32_write_masked((volatile uint32_t *)DDR_S1_IDM_IO_CONTROL_DIRECT, 0xfff << 16, /*ddr_clk*/ 0x190 << 16);     

	/* Wait for DDR PHY up */
	for(i=0; i < 0x19000; i++) {
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		if( val != 0)
			break; /* DDR PHY is up */
	}
	
	if(i == 0x19000) {
		printf("DDR PHY is not up\n");
		return;
	}
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	/* mhz = 0x190, 32 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190c4f3);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	/* ECC =1, DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0036048d);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
#if (defined(CONFIG_HURRICANE2))
	/* mhz = 0x190, 16 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190d4f3);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	/* DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0016048d);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#endif
#if (defined(CONFIG_NS_PLUS))
#if (defined(CONFIG_DDR32))
	/* mhz = 0x190, 32 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190c4f3);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	/* ECC =1, DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0036048d);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
#else
	if (ddr32) {
		/* mhz = 0x190, 32 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190c4f3);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		/* ECC =1, DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0036048d);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
	} else {
		/* mhz = 0x190, 16 bit bus, 16 bit chip width, 1.35v vddq, 8Gb chip size, jedec type = 25 (DDR3-1866M) */
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL, 0x0190d4f3);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

		/* DRAM type = 1 i.e DDR3, AL = 3, write recovery = 0, write latency = 9, CAS read latency = 13 */
		reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STRAP_CONTROL2, 0x0016048d);
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
	}
#endif /* (defined(CONFIG_DDR32)) */
#endif /* (defined(CONFIG_NS_PLUS)) */

	printf("ddr_init2: Calling soc_ddr40_set_shmoo_dram_config\n");
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
	soc_ddr40_set_shmoo_dram_config(unit, ((uint32) 0xFFFFFFFF) >> (32 - 2));
#elif (defined(CONFIG_HURRICANE2))
	soc_ddr40_set_shmoo_dram_config(unit, ((uint32) 0xFFFFFFFF) >> (32 - 1));
#elif (defined(CONFIG_NS_PLUS))
#if (defined(CONFIG_DDR32))
	soc_ddr40_set_shmoo_dram_config(unit, ((uint32) 0xFFFFFFFF) >> (32 - 2));
#else
	if (ddr32)
		soc_ddr40_set_shmoo_dram_config(unit, ((uint32) 0xFFFFFFFF) >> (32 - 2));
	else
		soc_ddr40_set_shmoo_dram_config(unit, ((uint32) 0xFFFFFFFF) >> (32 - 1));
#endif /* (defined(CONFIG_DDR32)) */
#else
#error "DRAM config is not set"
#endif

	printf("ddr_init2: Calling soc_ddr40_phy_calibrate\n");
    if(soc_ddr40_phy_calibrate(unit, 0, DDR_PHYTYPE_ENG, 0) != SOC_E_NONE) {

		printf("DDR phy calibration failed\n");
		return;
	}

ddr_cntrl_prog:
#ifdef CONFIG_RUN_DDR_SHMOO
	printf("PHY register dump after DDR PHY init\n");
	dump_phy_regs();
#endif
	printf("Programming controller register\n");
	ddr_init_regs(ddr_init_tab);
	if(ddr_type) {
		/* DDR3 */	
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_NS_PLUS) || defined(CONFIG_KATANA2))
		switch(ddr_clk) {
			case 333:
				ddr_init_regs(ddr3_init_tab_667);
				break;
			case 400:
				ddr_init_regs(ddr3_init_tab_800);
				break;
			case 467:
				ddr_init_regs(ddr3_init_tab_933);
				break;
			case 533:
				ddr_init_regs(ddr3_init_tab_1066);
				break;
			case 667:
				ddr_init_regs(ddr3_init_tab_1333);
				break;
			case 750:
			case 800:
				ddr_init_regs(ddr3_init_tab_1600);
				break;
		}
#else
		ddr_init_regs(ddr3_init_tab_667);
#endif
	}
	else {
		/* DDR2 */	
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_KATANA2))
		switch(ddr_clk) {
			case 333:
				ddr_init_regs(ddr2_init_tab_667);
				break;
			case 400:
				ddr_init_regs(ddr2_init_tab_800);
				break;
			case 533:
				ddr_init_regs(ddr2_init_tab_1066);
				break;
		}
#else
		ddr_init_regs(ddr2_init_tab);
#endif
	}
#if defined(CONFIG_HELIX4)
	if(ddr_clk == 750) {
		val = reg32_read((volatile uint32_t *)DDR_DENALI_CTL_22) & 0x3FFF; //bits 13:0 TREF_F1
		val = val * 750 / 800;
		reg32_write_masked((volatile uint32_t *)DDR_DENALI_CTL_22, 0x3fff, val);     
	}
#endif
#if (defined(CONFIG_NS_PLUS))
#if (defined(CONFIG_DDR32))
	reg32_set_bits(  (volatile uint32_t *)DDR_DENALI_CTL_85, 0x02000000);
	reg32_clear_bits((volatile uint32_t *)DDR_DENALI_CTL_86, 0x0e000000);
	reg32_clear_bits((volatile uint32_t *)DDR_DENALI_CTL_87, 0x00000100);
#else
	if (ddr32)  {
		reg32_set_bits(  (volatile uint32_t *)DDR_DENALI_CTL_85, 0x02000000);
		reg32_clear_bits((volatile uint32_t *)DDR_DENALI_CTL_86, 0x0e000000);
		reg32_clear_bits((volatile uint32_t *)DDR_DENALI_CTL_87, 0x00000100);
	}
#endif /* (defined(CONFIG_DDR32)) */
#endif /* (defined(CONFIG_NS_PLUS)) */
turnon:
	/* Start the DDR */
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_00, 0x01);

poll_ddr_ctrl:
	while(!(reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) & 0x100));
	printf("ddr_init2: Calling soc_ddr40_shmoo_ctl\n");
	
	/* Srini - Write to DDR controller registers */
	*(unsigned int *)(0x18010354) = 0x00FFFFFF;
	*(unsigned int *)(0x18010400) = 0x00000002;
	*(unsigned int *)(0x18010400) = 0x00000003;
	*(unsigned int *)(0x18010400) = 0x0000C003;
	*(unsigned int *)(0x18010408) = 0x00000020;
#ifdef CONFIG_RUN_DDR_SHMOO2

#ifdef CONFIG_SHMOO_REUSE
    if (try_restore_shmoo()) 
#endif /* CONFIG_SHMOO_REUSE */
    {
        soc_ddr40_shmoo_ctl(unit, 0, 2, 2, 0, 1);
    }
#if defined(CONFIG_IPROC_DDR_ECC)
	printf("Enabling DDR ECC reporting\n");
	/* Clear DDR interrupts if any */
	*(unsigned int *)(DDR_DENALI_CTL_213) = 0x00FFFFFF;
	__udelay(1000);
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_67, 0x01); //Disable auto correction
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_66, 0x01); //Enable ECC

#if 0 /* No overriding required, now ECC lane is shmooed */
	int mini_mode = 1;       // 0: FIFO Status; 1: ECC Syndrome
    int full_mode = 1;       // 0: FIFO Status; 1: ECC Syndrome
    if(shmoo_ecc_mini(mini_mode)) {
        printf("Mini ECC Shmoo Result: FAIL!!! (Mode: %d)\n", mini_mode);
        printf("Running 5D Full ECC Shmoo... (Mode: %d)\n", full_mode);
        shmoo_ecc_full(full_mode);
        printf("5D Full ECC Shmoo COMPLETE. Manual Tuning REQUIRED!!!\n");
    }
    else {
        printf("Mini ECC Shmoo Result: PASS (Mode: %d)\n", mini_mode);
        printf("5D Full ECC Shmoo DISABLED.\n");
        printf("ECC Lane Tuning COMPLETE.\n");
    }
#endif
#if 0
	/* Initialize DDR memory so that uninitialized reads won't report ecc error */
	uint32_t *dptr = CONFIG_PHYS_SDRAM_1;
	uint32_t dptr_end = CONFIG_PHYS_SDRAM_1 + CONFIG_PHYS_SDRAM_1_SIZE;
	printf("dptr_end: 0x%x\n", dptr_end);
	while (dptr < dptr_end) {
		*dptr = 0;
		dptr++; 
	}
#else
	clear_ddr(0, CONFIG_PHYS_SDRAM_1_SIZE);
#endif
	printf("Enabling DDR ECC correction\n");
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_66, 1 << 1); //Enable ECC correction
#endif
#else
	ddr_phy_ctl_regs_ovrd(ddr_phy_ctl_ovrd_tab);
	ddr_phy_wl_regs_ovrd(ddr_phy_wl0_tab);
#endif
	/* Release DDR slave port to AXI */
	reg32_clear_bits((volatile uint32_t *)DDR_BistConfig, 1 << DDR_BistConfig__axi_port_sel);
	printf("DDR Tune Completed\n");

	//dump_phy_regs();
}

#endif /*!(defined(CONFIG_NORTHSTAR)) */

