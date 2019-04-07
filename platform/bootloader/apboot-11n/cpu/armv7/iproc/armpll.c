#include <config.h>
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/iproc/reg_utils.h"

#ifdef CONFIG_APBOOT
#pragma GCC optimize("-O0")
#endif

unsigned int armpll_clk_tab[] = {
	/*  mode,	ndiv_int,	ndiv_frac,	pdiv,	freqid */
		25,		64,			1,			1,		0,	
		100,	64,			1,			1,		2,	
		400,	64,			1,			1,		6,	
		448,	71,			713050,		1,		6,	
		500,	80,			1,			1,		6,	
		560,	89,			629145,		1,		6,	
		600,	96,			1,			1,		6,	
		800,	64,			1,			1,		7,	
		896,	71,			713050,		1,		7,	
		1000,	80,			1,			1,		7,	
		1120,	89,			629145,		1,		7,	
		1200,	96,			1,			1,		7,	
		0xffffffff
};

/*
** 
** 
**
*/
uint32_t iproc_config_armpll(uint32_t clkmhz)
{
   uint32_t status = 0, freqid;
   int i = 0, j = 0;

	while(1) {
		if(armpll_clk_tab[i] == clkmhz)
			break;
		if(armpll_clk_tab[i] == 0xffffffff) {
			printf("Error: Requested clock configuration not supported\n");
			status = 1;
			goto armpll_config_done;
		}
		i += 5;
	}

	/* Enable write access */
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_WR_ACCESS, 0xa5a501);

	if(clkmhz == 25)
		freqid = 0;
	else
		freqid = 2;

	/* Bypass ARM clock and run on sysclk */
		reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_FREQ,
		1 << IHOST_PROC_CLK_POLICY_FREQ__priv_access_mode |
		freqid << IHOST_PROC_CLK_POLICY_FREQ__policy3_freq_R |
		freqid<< IHOST_PROC_CLK_POLICY_FREQ__policy2_freq_R |
		freqid << IHOST_PROC_CLK_POLICY_FREQ__policy1_freq_R |
		freqid << IHOST_PROC_CLK_POLICY_FREQ__policy0_freq_R);

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL, 
		1 << IHOST_PROC_CLK_POLICY_CTL__GO |
		1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC);

	for(j = 0; j < 0x100000; j++) {
		if(!(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL) & (1 << IHOST_PROC_CLK_POLICY_CTL__GO)))
			break;
	}

	if(j == 0x100000) {
		printf("CCU polling timedout\n");
		status = 1;
		goto armpll_config_done;
	}

	if(clkmhz == 25 || clkmhz == 100) {
		status = 0;
		goto armpll_config_done;
	}

	/* Now it is safe to program the PLL */
	reg32_write_masked((volatile uint32_t *)IHOST_PROC_CLK_PLLARMB, 
		(1 << IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_WIDTH) -1, 
		armpll_clk_tab[i + 2] << IHOST_PROC_CLK_PLLARMB__pllarm_ndiv_frac_R);	

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA, 
			1 << IHOST_PROC_CLK_PLLARMA__pllarm_lock |
			armpll_clk_tab[i+1] << IHOST_PROC_CLK_PLLARMA__pllarm_ndiv_int_R |  
			armpll_clk_tab[i+3] << IHOST_PROC_CLK_PLLARMA__pllarm_pdiv_R |
			1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_resetb);

	for(j = 0; j < 0x100000; j++) {
		if(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA) & (1 << IHOST_PROC_CLK_PLLARMA__pllarm_lock))
			break;
	}
	if(j == 0x100000) {
		printf("ARM PLL lock failed\n");
		status = 1;
		goto armpll_config_done;
	}

	reg32_set_bits((volatile uint32_t *)IHOST_PROC_CLK_PLLARMA, 1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb);
	/* Set the policy */
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_FREQ,
		1 << IHOST_PROC_CLK_POLICY_FREQ__priv_access_mode |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy3_freq_R |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy2_freq_R |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy1_freq_R |
		armpll_clk_tab[i+4] << IHOST_PROC_CLK_POLICY_FREQ__policy0_freq_R);

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE0_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_CORE1_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_SWITCH_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_ARM_PERIPH_CLKGATE, 0x00000303); 
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_APB0_CLKGATE, 0x00000303); 

	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL, 
		1 << IHOST_PROC_CLK_POLICY_CTL__GO |
		1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC);

	for(j = 0; j < 0x100000; j++) {
		if(!(reg32_read((volatile uint32_t *)IHOST_PROC_CLK_POLICY_CTL) & (1 << IHOST_PROC_CLK_POLICY_CTL__GO)))
			break;
	}

	if(j == 0x100000) {
		printf("CCU polling timedout\n");
		status = 1;
		goto armpll_config_done;
	}

	status = 0;
armpll_config_done:
	/* Disable access to PLL registers */
	reg32_write((volatile uint32_t *)IHOST_PROC_CLK_WR_ACCESS, 0);

	return(status);
}

