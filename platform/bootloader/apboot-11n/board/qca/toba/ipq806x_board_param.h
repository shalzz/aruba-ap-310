/* * Copyright (c) 2013 The Linux Foundation. All rights reserved.* */

#ifndef   _IPQ806X_BOARD_PARAM_H_
#define   _IPQ806X_BOARD_PARAM_H_

#include <asm/arch/iomap.h>
#include "ipq806x_cdp.h"
#include <asm-armv7/mach-types.h>
#include <asm-armv7/sizes.h>
#include <asm/arch/gpio.h>
#include <asm/arch/nss/msm_ipq806x_gmac.h>
#include <asm/arch/phy.h>
#include <asm/arch/clock.h>

gpio_func_data_t gmac0_gpio[] = {
	{
		.gpio = 0,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 1,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 2,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 66,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_16MA,
		.enable = GPIO_ENABLE
	},
};

gpio_func_data_t gmac1_gpio[] = {
	{
		.gpio = 0,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 1,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 51,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 52,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 59,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 60,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 61,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 62,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 27,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 28,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 29,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 30,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 31,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = 32,
		.func = 2,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
};

gpio_func_data_t gmac2_gpio[] = {
	{
		.gpio = 0,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = 1,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
};

gpio_func_data_t general_gpio[] = {
	{
		.gpio = GPIO_EXT_WDOG_POKE,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_RED_STATUS_LED,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_GREEN_STATUS_LED,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_AMBER_STATUS_LED,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
/*	{
		.gpio = GPIO_GREEN_WL_LED,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_AMBER_WL_LED,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},*/
	{
		.gpio = GPIO_HW_RESET,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_TPM_RESET_L,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_PHY_0_RST_L,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
/*	{
		.gpio = GPIO_PHY_1_RST_L,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},*/
	{
		.gpio = GPIO_POR_RST_CLK,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_WD_LATCH_CLR_L,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
/*	{
		.gpio = GPIO_USB_ENABLE,
		.func = 0,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},*/
	{
		.gpio = GPIO_BLE_RST_L,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = GPIO_WD_DISABLE,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_DISABLE
	},
	{
		.gpio = GPIO_POWER_AT,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = GPIO_POWER_DC,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = GPIO_IPM_IRQ_L,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = GPIO_CONFIG_CLEAR,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = GPIO_POR_STS_IN,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
	{
		.gpio = GPIO_WD_STS_IN,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
/*	{
		.gpio = GPIO_USB_FLAG,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},*/
	{
		.gpio = GPIO_PHY0_INT,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},
/*	{
		.gpio = GPIO_PHY1_INT,
		.func = 0,
		.dir = GPIO_INPUT,
		.pull = GPIO_NO_PULL,
		.drvstr = GPIO_8MA,
		.enable = GPIO_ENABLE
	},*/
};

#ifdef CONFIG_IPQ806X_PCI
/* Address of PCIE20 PARF */
#define PCIE20_0_PARF_PHYS      0x1b600000
#define PCIE20_1_PARF_PHYS      0x1b800000
#define PCIE20_2_PARF_PHYS      0x1ba00000

/* Address of PCIE20 ELBI */
#define PCIE20_0_ELBI_PHYS      0x1b502000
#define PCIE20_1_ELBI_PHYS      0x1b702000
#define PCIE20_2_ELBI_PHYS      0x1b902000


/* Address of PCIE20 */
#define PCIE20_0_PHYS           0x1b500000
#define PCIE20_1_PHYS           0x1b700000
#define PCIE20_2_PHYS           0x1b900000
#define PCIE20_SIZE             SZ_4K

#define PCIE20_0_AXI_BAR_PHYS	0x08000000
#define PCIE20_0_AXI_BAR_SIZE	SZ_128M
#define PCIE20_1_AXI_BAR_PHYS	0x2E000000
#define PCIE20_1_AXI_BAR_SIZE	SZ_64M
#define PCIE20_2_AXI_BAR_PHYS	0x32000000
#define PCIE20_2_AXI_BAR_SIZE	SZ_64M
#define PCIE_AXI_CONF_SIZE	SZ_1M

#define MSM_PCIE_DEV_CFG_ADDR   0x01000000

#define PCIE20_PLR_IATU_VIEWPORT        0x900
#define PCIE20_PLR_IATU_CTRL1           0x904
#define PCIE20_PLR_IATU_CTRL2           0x908
#define PCIE20_PLR_IATU_LBAR            0x90C
#define PCIE20_PLR_IATU_UBAR            0x910
#define PCIE20_PLR_IATU_LAR             0x914
#define PCIE20_PLR_IATU_LTAR            0x918
#define PCIE20_PLR_IATU_UTAR            0x91c

#define PCIE20_PARF_CONFIG_BITS        0x50
#define PCIE20_ELBI_SYS_CTRL           0x04


#define PCIE20_0_RESET            (MSM_CLK_CTL_BASE + 0x22DC)
#define PCIE20_1_RESET            (MSM_CLK_CTL_BASE + 0x3A9C)
#define PCIE20_2_RESET            (MSM_CLK_CTL_BASE + 0x3ADC)
#define PCIE20_CAP                     0x70
#define PCIE20_CAP_LINKCTRLSTATUS      (PCIE20_CAP + 0x10)

gpio_func_data_t pci_0_rst_gpio = {
		.gpio = GPIO_PCIE_1_RST_L,
		.func = 0,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_2MA,
		.enable = GPIO_DISABLE
};

gpio_func_data_t pci_1_rst_gpio = {
		.gpio = GPIO_PCIE_2_RST_L,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.enable = GPIO_DISABLE
};

/*gpio_func_data_t pci_2_rst_gpio = {
		.gpio = 63,
		.func = 1,
		.dir = GPIO_OUTPUT,
		.pull = GPIO_PULL_UP,
		.drvstr = GPIO_12MA,
		.enable = GPIO_DISABLE
};*/

clk_offset_t pcie_0_clk	= {
	.aclk_ctl = PCIE_0_ACLK_CTL,
	.pclk_ctl = PCIE_0_PCLK_CTL,
	.hclk_ctl = PCIE_0_HCLK_CTL,
	.aux_clk_ctl = PCIE_0_AUX_CLK_CTL,
	.alt_ref_clk_ns = PCIE_0_ALT_REF_CLK_NS,
	.alt_ref_clk_acr = PCIE_0_ALT_REF_CLK_ACR,
	.aclk_fs = PCIE_0_ACLK_FS,
	.pclk_fs = PCIE_0_PCLK_FS,
	.parf_phy_refclk = PCIE20_0_PARF_PHY_REFCLK
};

clk_offset_t pcie_1_clk	= {
	.aclk_ctl = PCIE_1_ACLK_CTL,
	.pclk_ctl = PCIE_1_PCLK_CTL,
	.hclk_ctl = PCIE_1_HCLK_CTL,
	.aux_clk_ctl = PCIE_1_AUX_CLK_CTL,
	.alt_ref_clk_ns = PCIE_1_ALT_REF_CLK_NS,
	.alt_ref_clk_acr = PCIE_1_ALT_REF_CLK_ACR,
	.aclk_fs = PCIE_1_ACLK_FS,
	.pclk_fs = PCIE_1_PCLK_FS,
	.parf_phy_refclk = PCIE20_1_PARF_PHY_REFCLK
};

clk_offset_t pcie_2_clk	= {
	.aclk_ctl = PCIE_2_ACLK_CTL,
	.pclk_ctl = PCIE_2_PCLK_CTL,
	.hclk_ctl = PCIE_2_HCLK_CTL,
	.aux_clk_ctl = PCIE_2_AUX_CLK_CTL,
	.alt_ref_clk_ns = PCIE_2_ALT_REF_CLK_NS,
	.alt_ref_clk_acr = PCIE_2_ALT_REF_CLK_ACR,
	.aclk_fs = PCIE_2_ACLK_FS,
	.pclk_fs = PCIE_2_PCLK_FS,
	.parf_phy_refclk = PCIE20_2_PARF_PHY_REFCLK
};

#define pcie_board_cfg(_id)	\
{									\
	.pci_rst_gpio		= &pci_##_id##_rst_gpio,		\
	.parf			= PCIE20_##_id##_PARF_PHYS,		\
	.elbi			= PCIE20_##_id##_ELBI_PHYS,		\
	.pcie20			= PCIE20_##_id##_PHYS,			\
	.axi_bar_start		= PCIE20_##_id##_AXI_BAR_PHYS,		\
	.axi_bar_size		= PCIE20_##_id##_AXI_BAR_SIZE,		\
	.pcie_rst		= PCIE20_##_id##_RESET,			\
	.pci_clks		= &pcie_##_id##_clk			\
}

#define PCIE20_PARF_PHY_CTRL           0x40
#define __mask(a, b)    (((1 << ((a) + 1)) - 1) & ~((1 << (b)) - 1))
#define __set(v, a, b)  (((v) << (b)) & __mask(a, b))
#define PCIE20_PARF_PHY_CTRL_PHY_TX0_TERM_OFFST(x)      __set(x, 20, 16)
#define PCIE20_PARF_PCS_DEEMPH         0x34
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1(x)        __set(x, 21, 16)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB(x)  __set(x, 13, 8)
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB(x)    __set(x, 5, 0)
#define PCIE20_PARF_PCS_SWING          0x38
#define PCIE20_PARF_PCS_SWING_TX_SWING_FULL(x)          __set(x, 14, 8)
#define PCIE20_PARF_PCS_SWING_TX_SWING_LOW(x)           __set(x, 6, 0)
#define PCIE20_PARF_PHY_REFCLK         0x4C
#define PCIE_SFAB_AXI_S5_FCLK_CTL       (MSM_CLK_CTL_BASE + 0x2154)
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL0 0x818
#define PCIE20_AXI_MSTR_RESP_COMP_CTRL1 0x81c
#endif /* CONFIG_IPQ806X_PCI*/

#define gmac_board_cfg(_b, _sec, _p, _p0, _p1, _mp, _pn, ...)	\
{									\
	.base			= NSS_GMAC##_b##_BASE,			\
	.unit			= _b,					\
	.is_macsec		= _sec,					\
	.phy			= PHY_INTERFACE_MODE_##_p,		\
	.phy_addr		= { .count = _pn, { __VA_ARGS__ } },	\
	.mac_pwr0		= _p0,					\
	.mac_pwr1		= _p1,					\
	.mac_conn_to_phy	= _mp					\
}

#define gmac_board_cfg_invalid()	{ .unit = -1, }

/* Board specific parameter Array */
board_ipq806x_params_t board_params[] = {
	/*
	 * Replicate DB149 details for RUMI until, the board no.s are
	 * properly sorted out
	 */
	{
		.machid = MACH_TYPE_IPQ806X_RUMI3,
		.ddr_size = (256 << 20),
		.uart_gsbi = GSBI_1,
		.uart_gsbi_base = UART_GSBI1_BASE,
		.uart_dm_base = UART1_DM_BASE,
		.mnd_value = { 48, 125, 63 },
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.dbg_uart_gpio = {
			{
				.gpio = 51,
				.func = 1,
				.dir = GPIO_OUTPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
			{
				.gpio = 52,
				.func = 1,
				.dir = GPIO_INPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
		},
		.clk_dummy = 1,
	},
	{
		.machid = MACH_TYPE_IPQ806X_DB149,
		.ddr_size = (256 << 20),
		.uart_gsbi = GSBI_2,
		.uart_gsbi_base = UART_GSBI2_BASE,
		.uart_dm_base = UART2_DM_BASE,
		.mnd_value = { 12, 625, 313 },
		.gmac_gpio_count = ARRAY_SIZE(gmac0_gpio),
		.gmac_gpio = gmac0_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.dbg_uart_gpio = {
			{
				.gpio = 22,
				.func = 1,
				.dir = GPIO_OUTPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
			{
				.gpio = 23,
				.func = 1,
				.dir = GPIO_INPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
		}
	},
	{
		.machid = MACH_TYPE_IPQ806X_DB149_1XX,
		.ddr_size = (256 << 20),
		.uart_gsbi = GSBI_2,
		.uart_gsbi_base = UART_GSBI2_BASE,
		.uart_dm_base = UART2_DM_BASE,
		.mnd_value = { 12, 625, 313 },
		.gmac_gpio_count = ARRAY_SIZE(gmac0_gpio),
		.gmac_gpio = gmac0_gpio,
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.flashdesc = NOR_MMC,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.dbg_uart_gpio = {
			{
				.gpio = 22,
				.func = 1,
				.dir = GPIO_OUTPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
			{
				.gpio = 23,
				.func = 1,
				.dir = GPIO_INPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
		}
	},
	{
		.machid = MACH_TYPE_IPQ806X_TB726,
		.ddr_size = (256 << 20),
		.uart_gsbi = GSBI_2,
		.uart_gsbi_base = UART_GSBI2_BASE,
		.uart_dm_base = UART2_DM_BASE,
		.mnd_value = { 12, 625, 313 },
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,

		/* This GMAC config table is not valid as of now.
		 * To accomodate this config, TB726 board needs
		 * hardware rework.Moreover this setting is not
		 * been validated in TB726 board */
		.gmac_cfg = {
			gmac_board_cfg(0, 0, RGMII, 0, 0, 0,
					1, 4),
			gmac_board_cfg(1, 1, SGMII, 0, 0, 0,
					4, 0, 1, 2, 3),
			gmac_board_cfg(2, 1, SGMII, 0, 0, 1,
					1, 6),
			gmac_board_cfg(3, 1, SGMII, 0, 0, 1,
					1, 7),
		},
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_SPANSION,
		},
		.dbg_uart_gpio = {
			{
				.gpio = 22,
				.func = 1,
				.dir = GPIO_OUTPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
			{
				.gpio = 23,
				.func = 1,
				.dir = GPIO_INPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
		}

	},
	{
		.machid = MACH_TYPE_IPQ806X_AP148,
		.ddr_size = (512 << 20),
		.uart_gsbi = GSBI_4,
		.uart_gsbi_base = UART_GSBI4_BASE,
		.uart_dm_base = UART4_DM_BASE,
		.mnd_value = { 12, 625, 313 },
#ifdef CONFIG_TOBA
        /* AR8033 on SGMII with PHY Address = 5, GMAC3(.base = 2) ,only use GE port with apboot*/
		.gmac_gpio_count = ARRAY_SIZE(gmac2_gpio),
		.gmac_gpio = gmac2_gpio,
		.gmac_cfg = {
			gmac_board_cfg(2, 0, SGMII, 0, 0, 1, 1, 5),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
#else
		.gmac_gpio_count = ARRAY_SIZE(gmac1_gpio),
		.gmac_gpio = gmac1_gpio,
		.gmac_cfg = {
			gmac_board_cfg(1, 1, RGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 1, 4),
			gmac_board_cfg(2, 1, SGMII, S17_RGMII0_1_8V,
					S17_RGMII1_1_8V, 0, 4, 0, 1, 2, 3),
			gmac_board_cfg_invalid(),
			gmac_board_cfg_invalid(),
		},
#endif
		.flashdesc = NAND_NOR,
		.flash_param = {
			.mode =	NOR_SPI_MODE_0,
			.bus_number = GSBI_BUS_5,
			.chip_select = SPI_CS_0,
			.vendor = SPI_NOR_FLASH_VENDOR_MICRON,
		},
		.dbg_uart_gpio = {
			{
				.gpio = 10,
				.func = 1,
				.dir = GPIO_OUTPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
			{
				.gpio = 11,
				.func = 1,
				.dir = GPIO_INPUT,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_DISABLE
			},
		},
#ifdef CONFIG_IPQ806X_I2C
		.i2c_gsbi = GSBI_7,
		.i2c_gsbi_base = UART_GSBI7_BASE,
		.i2c_mnd_value = { 1, 4, 2 },
		.i2c_gpio = {
			{
				.gpio = 13,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_ENABLE
			},
			{
				.gpio = 12,
				.func = 1,
				.pull = GPIO_NO_PULL,
				.drvstr = GPIO_12MA,
				.enable = GPIO_ENABLE
			},
		},
#endif
#ifdef CONFIG_IPQ806X_PCI
		.pcie_cfg = {
			pcie_board_cfg(0),
			pcie_board_cfg(1),
		}
#endif /* CONFIG_IPQ806X_PCI */
	},
};

#define NUM_IPQ806X_BOARDS	ARRAY_SIZE(board_params)
#endif
