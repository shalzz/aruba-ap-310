/*
 * Copyright (c) 2012 - 2013 The Linux Foundation. All rights reserved.
 */

#include <common.h>
#include <asm/arch-ipq806x/clock.h>
#include <asm/arch-ipq806x/nss/clock.h>
#include <asm/arch-ipq806x/iomap.h>
#include <asm/io.h>

/**
 * uart_pll_vote_clk_enable - enables PLL8
 */
void uart_pll_vote_clk_enable(unsigned int clk_dummy)
{
	setbits_le32(BB_PLL_ENA_SC0_REG, BIT(8));

	if (!clk_dummy)
		while((readl(PLL_LOCK_DET_STATUS_REG) & BIT(8)) == 0);
}

/**
 * uart_set_rate_mnd - configures divider M and D values
 *
 * Sets the M, D parameters of the divider to generate the GSBI UART
 * apps clock.
 */
static void uart_set_rate_mnd(unsigned int gsbi_port, unsigned int m,
		unsigned int n)
{
	/* Assert MND reset. */
	setbits_le32(GSBIn_UART_APPS_NS_REG(gsbi_port), BIT(7));
	/* Program M and D values. */
	writel(MD16(m, n), GSBIn_UART_APPS_MD_REG(gsbi_port));
	/* Deassert MND reset. */
	clrbits_le32(GSBIn_UART_APPS_NS_REG(gsbi_port), BIT(7));
}

/**
 * uart_branch_clk_enable_reg - enables branch clock
 *
 * Enables branch clock for GSBI UART port.
 */
static void uart_branch_clk_enable_reg(unsigned int gsbi_port)
{
	setbits_le32(GSBIn_UART_APPS_NS_REG(gsbi_port), BIT(9));
}

/**
 * uart_local_clock_enable - configures N value and enables root clocks
 *
 * Sets the N parameter of the divider and enables root clock and
 * branch clocks for GSBI UART port.
 */
static void uart_local_clock_enable(unsigned int gsbi_port, unsigned int n,
					unsigned int m)
{
	unsigned int reg_val, uart_ns_val;
	void *const reg = (void *)GSBIn_UART_APPS_NS_REG(gsbi_port);

	/*
	* Program the NS register, if applicable. NS registers are not
	* set in the set_rate path because power can be saved by deferring
	* the selection of a clocked source until the clock is enabled.
	*/
	reg_val = readl(reg); // REG(0x29D4+(0x20*((n)-1)))
	reg_val &= ~(Uart_clk_ns_mask);
	uart_ns_val =  NS(BIT_POS_31,BIT_POS_16,n,m, 5, 4, 3, 1, 2, 0,3);
	reg_val |= (uart_ns_val & Uart_clk_ns_mask);
	writel(reg_val,reg);

	/* enable MNCNTR_EN */
	reg_val = readl(reg);
	reg_val |= BIT(8);
	writel(reg_val, reg);

	/* set source to PLL8 running @384MHz */
	reg_val = readl(reg);
	reg_val |= 0x3;
	writel(reg_val, reg);

	/* Enable root. */
	reg_val |= Uart_en_mask;
	writel(reg_val, reg);
	uart_branch_clk_enable_reg(gsbi_port);
}

/**
 * uart_set_gsbi_clk - enables HCLK for UART GSBI port
 */
static void uart_set_gsbi_clk(unsigned int gsbi_port)
{
	setbits_le32(GSBIn_HCLK_CTL_REG(gsbi_port), BIT(4));
}

/**
 * uart_clock_config - configures UART clocks
 *
 * Configures GSBI UART dividers, enable root and branch clocks.
 */
void uart_clock_config(unsigned int gsbi_port, unsigned int m,
		unsigned int n, unsigned int d, unsigned int clk_dummy)
{
	uart_set_rate_mnd(gsbi_port, m, d);
	uart_pll_vote_clk_enable(clk_dummy);
	uart_local_clock_enable(gsbi_port, n, m);
	uart_set_gsbi_clk(gsbi_port);
}

/**
 * nand_clock_config - configure NAND controller clocks
 *
 * Enable clocks to EBI2. Must be invoked before touching EBI2
 * registers.
 */
void nand_clock_config(void)
{
	writel(CLK_BRANCH_ENA(1) | ALWAYS_ON_CLK_BRANCH_ENA(1),
	       EBI2_CLK_CTL_REG);

	/* Wait for clock to stabilize. */
	udelay(10);
}

#ifdef CONFIG_IPQ806X_PCI
void pcie_clock_shutdown(clk_offset_t *pci_clk)
{
	/* PCIE_ALT_REF_CLK_NS */
	writel(0x0, pci_clk->alt_ref_clk_ns);
	/* PCIE_ALT_REF_CLK_ACR */
	writel(0x0, pci_clk->alt_ref_clk_acr);

	/* PCIE20_PARF_PHY_REFCLK */
	writel(0x1019, pci_clk->parf_phy_refclk);

	/* PCIE_ACLK_CTL */
	writel(0x0, pci_clk->aclk_ctl);

	/* PCIE_PCLK_CTL */
	writel(0x0, pci_clk->pclk_ctl);

	/* PCIE_HCLK_CTL */
	writel(0x0, pci_clk->hclk_ctl);

	/* PCIE_AUX_CLK_CTL */
	writel(0x0, pci_clk->aux_clk_ctl);
}

void pcie_clock_config(clk_offset_t *pci_clk)
{
	/* PCIE_ALT_REF_CLK_NS */
	writel(0x0A59, pci_clk->alt_ref_clk_ns);

	/* PCIE_ALT_REF_CLK_ACR */
	writel(0x0, pci_clk->alt_ref_clk_acr);

	/* PCIE_ACLK_FS */
	writel(0x4F, pci_clk->aclk_fs);

	/* PCIE_PCLK_FS */
	writel(0x4F, pci_clk->pclk_fs);

	/* PCIE_ACLK_CTL */
	writel(0x10, pci_clk->aclk_ctl);

	/* PCIE_PCLK_CTL */
	writel(0x10, pci_clk->pclk_ctl);

	/* PCIE_HCLK_CTL */
	writel(0x10, pci_clk->hclk_ctl);

	/* PCIE_AUX_CLK_CTL */
	writel(0x10, pci_clk->aux_clk_ctl);
}
#endif /* CONFIG_IPQ806X_PCI */
#ifdef CONFIG_IPQ806X_I2C
/**
 * i2c_set_rate_mnd - configures divider M and D values
 *
 * Sets the M, D parameters of the divider to generate the GSBI QUP
 * apps clock.
 */
static void i2c_set_rate_mnd(unsigned int gsbi_port, unsigned int m,
                unsigned int n)
{
        /* Assert MND reset. */
        setbits_le32(GSBIn_QUP_APPS_NS_REG(gsbi_port), BIT(7));
        /* Program M and D values. */
        writel(MD16(m, n), GSBIn_QUP_APPS_MD_REG(gsbi_port));
        /* Deassert MND reset. */
        clrbits_le32(GSBIn_QUP_APPS_NS_REG(gsbi_port), BIT(7));
}

/**
 * i2c_pll_vote_clk_enable - enables PLL8
 */
void i2c_pll_vote_clk_enable(unsigned int clk_dummy)
{
        setbits_le32(BB_PLL_ENA_SC0_REG, BIT(8));

        if (!clk_dummy)
                while((readl(PLL_LOCK_DET_STATUS_REG) & BIT(8)) == 0);
}

/**
 * i2c_branch_clk_enable_reg - enables branch clock
 *
 * Enables branch clock for GSBI I2C port.
 */
static void i2c_branch_clk_enable_reg(unsigned int gsbi_port)
{
        setbits_le32(GSBIn_QUP_APPS_NS_REG(gsbi_port), BIT(9));
}

/**
 * i2c_local_clock_enable - configures N value and enables root clocks
 *
 * Sets the N parameter of the divider and enables root clock and
 * branch clocks for GSBI I2C port.
 */
static void i2c_local_clock_enable(unsigned int gsbi_port, unsigned int n,
                                        unsigned int m)
{
        unsigned int reg_val, i2c_ns_val;
        void *const reg = (void *)GSBIn_QUP_APPS_NS_REG(gsbi_port);

        /*
        * Program the NS register, if applicable. NS registers are not
        * set in the set_rate path because power can be saved by deferring
        * the selection of a clocked source until the clock is enabled.
        */
        reg_val = readl(reg);
        reg_val &= ~(I2C_clk_ns_mask);
        i2c_ns_val =  NS(BIT_POS_23,BIT_POS_16,n,m, 5, 4, 3, 4, 2, 0, 3);
        reg_val |= (i2c_ns_val & I2C_clk_ns_mask);
        writel(reg_val,reg);

        /* enable MNCNTR_EN */
        reg_val = readl(reg);
        reg_val |= BIT(8);
        writel(reg_val, reg);

        /* set source to PLL8 running @384MHz */
        reg_val = readl(reg);
        reg_val |= 0x3;
        writel(reg_val, reg);

        /* Enable root. */
        reg_val |= I2C_en_mask;
        writel(reg_val, reg);
        i2c_branch_clk_enable_reg(gsbi_port);
}

/**
 * i2c_set_gsbi_clk - enables HCLK for I2C GSBI port
 */
static void i2c_set_gsbi_clk(unsigned int gsbi_port)
{
        setbits_le32(GSBIn_HCLK_CTL_REG(gsbi_port), BIT(4));
}

/**
 * i2c_clock_config - configures I2C clocks
 *
 * Configures GSBI I2C dividers, enable root and branch clocks.
 */
void i2c_clock_config(unsigned int gsbi_port, unsigned int m,
                unsigned int n, unsigned int d, unsigned int clk_dummy)
{
        i2c_set_rate_mnd(gsbi_port, m, d);
        i2c_pll_vote_clk_enable(clk_dummy);
        i2c_local_clock_enable(gsbi_port, n, m);
        i2c_set_gsbi_clk(gsbi_port);
}
#endif

