#include <config.h>
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/iproc/reg_utils.h"

#ifdef CONFIG_APBOOT
#pragma GCC optimize("-O0")
#endif

#if defined(CONFIG_HURRICANE2)
unsigned int genpll_clk_tab[] = {
	/*  mode,	ndiv,	MDIV
						0	1	2	3	4	5 */

		0,		80,		10,	0,	0,	5,	8,	0,		/* 400 MHz AXI */
		2,		80,		10,	0,	0,	7,	8,	0,		/* 285 MHz AXI */
		3,		80,		10,	0,	0,	8,	8,	0,		/* 250 MHz AXI */	
		4,		80,		10,	0,	0,	10,	8,	0,		/* 200 MHz AXI */	
		5,		80,		10,	0,	0,	20,	8,	0,		/* 100 MHz AXI */	
		0xffffffff
};
#endif
#if defined(CONFIG_HELIX4)
unsigned int genpll_clk_tab[] = {
	/*  mode,	ndiv,	MDIV
						0	1	2	3	4	5 */
		0,		60,		12,	0,	50,	3,	6,	0,		/* 500 MHz AXI(Ch3), ch0:125, ch2:30, ch4:250 */
		1,		80,		16,	0,	50,	5,	8,	0,		/* 400 MHz AXI */
		2,		60,		12,	0,	50,	5,	6,	0,		/* 300 MHz AXI */
		3,		60,		12,	0,	50,	6,	6,	0,		/* 250 MHz AXI */	
		4,		80,		16,	0,	50,	10,	8,	0,		/* 200 MHz AXI */	
		5,		60,		12,	0,	50,	15,	6,	0,		/* 100 MHz AXI */	
		0xffffffff
};
#endif

#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2))

/*
** 
** 
**
*/
uint32_t iproc_config_genpll(uint32_t mode)
{
   volatile uint32_t addr;
   volatile uint32_t rdata;
   volatile uint32_t wdata;
   volatile uint32_t lock;
   int i = 0;

	while(1) {
		if(genpll_clk_tab[i] == mode)
			break;
		if(genpll_clk_tab[i] == 0xffffffff) {
			return(1);
		}
		i += 8;
	}

	// Write fast_lock =1
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL0);
   rdata |= (1<<IPROC_WRAP_GEN_PLL_CTRL0__FAST_LOCK);
   reg32_write(IPROC_WRAP_GEN_PLL_CTRL0,rdata);

   // Write NDIV
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1);
   rdata &= 0xfffffc00;
   rdata |= (genpll_clk_tab[i+1] << IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_R);
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1,rdata);

   // Write MDIV
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL2);
   rdata &= 0xff0000ff;
   rdata |= ((genpll_clk_tab[i+5] <<IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_R)|
            (genpll_clk_tab[i+6]<<IPROC_WRAP_GEN_PLL_CTRL2__CH4_MDIV_R));
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL2,rdata);

   // Write PLL_LOAD
   rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3);
   rdata |= (1<<IPROC_WRAP_GEN_PLL_CTRL3__SW_TO_GEN_PLL_LOAD);
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3,rdata);

   // Load Channel3 & Channel4
   rdata &= 0xffc0ffff;
   rdata |= (0x18<<IPROC_WRAP_GEN_PLL_CTRL3__LOAD_EN_CH_R);
   reg32_write((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL3,rdata);

   // Wait for IPROC_WWRAP GENPLL lock
   do{
     rdata = reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_STATUS);
     lock = ((rdata>>IPROC_WRAP_GEN_PLL_STATUS__GEN_PLL_LOCK)&1);
   }while(!lock);

	return(0);
}

uint32_t iproc_get_axi_clk(uint32_t refclk)
{
   uint32_t ndiv, mdiv, pdiv;

   ndiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1) >> IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_R) &
			((1 << IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_WIDTH) -1);
   if(ndiv == 0)
	   ndiv = 1 << IPROC_WRAP_GEN_PLL_CTRL1__NDIV_INT_WIDTH;

   pdiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL1) >> IPROC_WRAP_GEN_PLL_CTRL1__PDIV_R) &
			((1 << IPROC_WRAP_GEN_PLL_CTRL1__PDIV_WIDTH) -1);
   if(pdiv == 0)
	   pdiv = 1 << IPROC_WRAP_GEN_PLL_CTRL1__PDIV_WIDTH;

   mdiv = (reg32_read((volatile uint32_t *)IPROC_WRAP_GEN_PLL_CTRL2) >> IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_R) &
			((1 << IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_WIDTH) -1);
   if(mdiv == 0)
	   mdiv = 1 << IPROC_WRAP_GEN_PLL_CTRL2__CH3_MDIV_WIDTH;

	return((refclk / (pdiv * mdiv)) * ndiv);
}
#endif

#if defined(CONFIG_KATANA2)
uint32_t iproc_get_axi_clk(uint32_t refclk)
{
	return(495000000);
}
#endif
#if defined(CONFIG_NS_PLUS)
unsigned int genpll_clk_tab[] = {
	/* VCO Frequency 1500MHz */
	/*  mode,	GENPLL_CTRL5	GENPLL_CTRL6	GENPLL_CTRL7 */ /*ndiv MDIV0 MDIV1 MDIV2 MDIV3 MDIV4 MDIV5 */
		0,		0x03c00000,		0x01060932,		0x00030508,		/*60   6     9     50    3     5     8    AXI: 500 MHz; SW_TOP: 166.67 MHz */
		1,		0x03c00000,		0x01060a32,		0x00030508,		/*60   6     10    50    3     5     8    AXI: 500 MHz; SW_TOP: 150    MHz */
		2,		0x03c00000,		0x01060b32,		0x00030508,		/*60   6     11    50    3     5     8    AXI: 500 MHz; SW_TOP: 136.36 MHz */
		3,		0x03c00000,		0x01060c32,		0x00030508,		/*60   6     12    50    3     5     8    AXI: 500 MHz; SW_TOP: 125    MHz */
		4,		0x03c00000,		0x01060932,		0x00060508,		/*60   6     9     50    6     5     8    AXI: 250 MHz; SW_TOP: 166.67 MHz */
		5,		0x03c00000,		0x01060a32,		0x00060508,		/*60   6     10    50    6     5     8    AXI: 250 MHz; SW_TOP: 150    MHz */
		6,		0x03c00000,		0x01060b32,		0x00060508,		/*60   6     11    50    6     5     8    AXI: 250 MHz; SW_TOP: 136.36 MHz */
		7,		0x03c00000,		0x01060c32,		0x00060508,		/*60   6     12    50    6     5     8    AXI: 250 MHz; SW_TOP: 125    MHz */
		0xffffffff
};
/*
** 
** 
**
*/
uint32_t iproc_config_genpll(uint32_t mode)
{
	int i = 0;
        volatile uint32_t rdata;

	while(1) {
		if(genpll_clk_tab[i] == mode)
			break;
		if(genpll_clk_tab[i] == 0xffffffff) {
			/* printf("%s: ERROR: Can't find clock configuration requested\n", __FUNCTION__); */
			return(1);
		}
		i += 4;
	}

/* 	printf("%s: mode(0x%x): CONTROL5(0x%x) CONTROL6(0x%x) CONTROL7(0x%x)\n",                              */
/* 		 __FUNCTION__, genpll_clk_tab[i], genpll_clk_tab[i+1], genpll_clk_tab[i+2], genpll_clk_tab[i+3]); */
	/* unlock access to PLL registers */
	reg32_write((volatile uint32_t *)CRU_CLKSET_KEY_OFFSET, 0xea68);
	/* Write CONTROL1: Reset LOAD_EN */
        rdata = reg32_read((volatile uint32_t *)CRU_GENPLL_CONTROL1_OFFSET);
        rdata &= 0xffffffc0;
	reg32_write((volatile uint32_t *)CRU_GENPLL_CONTROL1_OFFSET, rdata);
	/* Write CONTROL5: NDIV_INT */
	reg32_write((volatile uint32_t *)CRU_GENPLL_CONTROL5_OFFSET, genpll_clk_tab[i+1]);
	/* Write CONTROL6: PDIV, CHO_MDIV, CH1_MDIV, CH2_MDIV */
	reg32_write((volatile uint32_t *)CRU_GENPLL_CONTROL6_OFFSET, genpll_clk_tab[i+2]);
	/* Write CONTROL7: CH3_MDIV, CH4_MDIV, CH35_MDIV */
	reg32_write((volatile uint32_t *)CRU_GENPLL_CONTROL7_OFFSET, genpll_clk_tab[i+3]);
	/* Write CONTROL1: Enable LOAD_EN */
        rdata = reg32_read((volatile uint32_t *)CRU_GENPLL_CONTROL1_OFFSET);
        rdata |= 0x3f;
	reg32_write((volatile uint32_t *)CRU_GENPLL_CONTROL1_OFFSET, rdata);
	/* Lock access to PLL registers */
	reg32_write((volatile uint32_t *)CRU_CLKSET_KEY_OFFSET, 0);

	return(0);
}

uint32_t iproc_get_axi_clk(uint32_t refclk)
{
   uint32_t ndiv, mdiv, pdiv, axi, num;

   ndiv = (reg32_read((volatile uint32_t *)CRU_GENPLL_CONTROL5_OFFSET) >> CRU_GENPLL_CONTROL5__NDIV_INT_R) &
			((1 << CRU_GENPLL_CONTROL5__NDIV_INT_WIDTH) -1);
   if(ndiv == 0)
	   ndiv = 1 << CRU_GENPLL_CONTROL5__NDIV_INT_WIDTH;

   pdiv = (reg32_read((volatile uint32_t *)CRU_GENPLL_CONTROL6_OFFSET) >> CRU_GENPLL_CONTROL6__PDIV_R) &
			((1 << CRU_GENPLL_CONTROL6__PDIV_WIDTH) -1);
   if(pdiv == 0)
	   pdiv = 1 << CRU_GENPLL_CONTROL6__PDIV_WIDTH;

   mdiv = (reg32_read((volatile uint32_t *)CRU_GENPLL_CONTROL7_OFFSET) >> CRU_GENPLL_CONTROL7__CH3_MDIV_R) &
			((1 << CRU_GENPLL_CONTROL7__CH3_MDIV_WIDTH) -1);
   if(mdiv == 0)
	   mdiv = 1 << CRU_GENPLL_CONTROL7__CH3_MDIV_WIDTH;

	num = (refclk / (pdiv * mdiv)) * ndiv;
	/* round to nearest 1000000 */
	axi = ((num+500000)/1000000)*1000000;
	printf("%s: refclk(0x%x), ndiv(0x%x) pdiv(0x%x) mdiv(0x%x): AXICLK:(0x%x)\n",
		 __FUNCTION__, refclk, ndiv, pdiv, mdiv, axi);
	return(axi);
}
#endif

#if defined(CONFIG_NORTHSTAR)
/* Fixme: Currently using fixed values, to be improved later */
uint32_t iproc_get_axi_clk(uint32_t refclk)
{
		unsigned int sku_id;
		sku_id = ((*((unsigned int *)0x1810D500)) >> 2) & 0x03;
		if(sku_id == 1 || sku_id == 2) {
			return(400000000);
		}
		else {
			return(500000000);
		}
}
#endif



uint32_t iproc_get_uart_clk(uint32_t uart)
{
	uint32_t uartclk, uartclkovr, uartclksel; 

	uartclk = iproc_get_axi_clk(CONFIG_SYS_REF_CLK) / 4; /* APB clock */

	if (uart < 2) {
		/* CCA UART */
		uartclkovr = (reg32_read((volatile uint32_t *)ChipcommonA_CoreCtrl) >> ChipcommonA_CoreCtrl__UARTClkOvr) & 0x01;
#if defined(CONFIG_NORTHSTAR)
		uartclksel = (reg32_read((volatile uint32_t *)APBX_IDM_IDM_IO_CONTROL_DIRECT) >> APBX_IDM_IDM_IO_CONTROL_DIRECT__UARTClkSel) & 0x01;
#else
		uartclksel = (reg32_read((volatile uint32_t *)APBW_IDM_IDM_IO_CONTROL_DIRECT) >> APBW_IDM_IDM_IO_CONTROL_DIRECT__UARTClkSel) & 0x01;
#endif
		if(!uartclkovr) {
			if(uartclksel) {
				uartclk /= ((reg32_read((volatile uint32_t *)ChipcommonA_ClkDiv) >> ChipcommonA_ClkDiv__UartClkDiv_R) & 
					        ((1 << ChipcommonA_ClkDiv__UartClkDiv_WIDTH) - 1));
			}
			else{
				uartclk = CONFIG_SYS_REF_CLK; /* Reference clock */
			}
		}
	}
	return(uartclk);
}
