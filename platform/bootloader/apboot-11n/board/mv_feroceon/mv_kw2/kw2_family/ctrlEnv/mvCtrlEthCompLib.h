/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * 	Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

    *	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *	Neither the name of Marvell nor the names of its contributors may be
	used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __INCmvCtrlEthCompLibh
#define __INCmvCtrlEthCompLibh

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysEthConfig.h"

/* QSGMII Control 0/1 Register. */
#define MV_ETHCOMP_QSGMII_CONTROL_REG(id)	(MV_ETH_COMPLEX_BASE + (id * 4))
/* QSGMII Control 1 Register. */
#define QSGCTRL_INV_SIG_DET_OFFSET		25
#define QSGCTRL_INV_SIG_DET_MASK		(0x1 << QSGCTRL_INV_SIG_DET_OFFSET)
#define QSGCTRL_RESETN_OFFSET			31
#define QSGCTRL_RESETN_MASK			(0x1 << QSGCTRL_RESETN_OFFSET)

#define MV_ETHCOMP_QSGMII_STATUS_REG		(MV_ETH_COMPLEX_BASE + 0x8)

#define MV_ETHCOMP_CTRL_REG(id)			(MV_ETH_COMPLEX_BASE + 0x10 + (id * 4))

/* Ethernet Complex Control 0 */
#define ETHCC_LPPHYMODE_OFFSET			0
#define ETHCC_LPPHYMODE_MASK			(0x3 << ETHCC_LPPHYMODE_OFFSET)
#define ETHCC_GBE_PHY_SMI_SRC_OFFSET		2
#define ETHCC_GBE_PHY_SMI_SRC_MASK		(0x1 << ETHCC_GBE_PHY_SMI_SRC_OFFSET)
#define ETHCC_SW_PORT6_MPP_OFFSET		3
#define ETHCC_SW_PORT6_MPP_MASK			(0x1 << ETHCC_SW_PORT6_MPP_OFFSET)
#define ETHCC_SW_PORT_0_SRC_OFFSET		4
#define ETHCC_SW_PORT_0_SRC_MASK		(0x3 << ETHCC_SW_PORT_0_SRC_OFFSET)
#define ETHCC_SW_PORT_123_SRC_OFFSET		6
#define ETHCC_SW_PORT_123_SRC_MASK		(0x1 << ETHCC_SW_PORT_123_SRC_OFFSET)
#define ETHCC_SW_PORT_5_SRC_OFFSET		8
#define ETHCC_SW_PORT_5_SRC_MASK		(0x3 << ETHCC_SW_PORT_5_SRC_OFFSET)
#define ETHCC_GPORT_0_SRC_OFFSET		10
#define ETHCC_GPORT_0_SRC_MASK			(0x3 << ETHCC_GPORT_0_SRC_OFFSET)
#define ETHCC_GPORT_1_SRC_OFFSET		12
#define ETHCC_GPORT_1_SRC_MASK			(0x3 << ETHCC_GPORT_1_SRC_OFFSET)
#define ETHCC_BGE_PHY_SRC_OFFSET		14
#define ETHCC_BGE_PHY_SRC_MASK			(0x3 << ETHCC_BGE_PHY_SRC_OFFSET)
#define ETHCC_SW_PORT5_MPP_OFFSET		16
#define ETHCC_SW_PORT5_MPP_MASK			(0x1 << ETHCC_SW_PORT5_MPP_OFFSET)
#define ETHCC_SW_FI_125_CLK_OFFSET		17
#define ETHCC_SW_FI_125_CLK_MASK		(0x3 << ETHCC_SW_FI_125_CLK_OFFSET)
#define ETHCC_GBE_PHY_SMI_ADD_OFFSET		19
#define ETHCC_GBE_PHY_SMI_ADD_MASK		(0x1F << ETHCC_GBE_PHY_SMI_ADD_OFFSET)
#define ETHCC_SW_P5_INBAND_SRC_OFFSET		24
#define ETHCC_SW_P5_INBAND_SRC_MASK		(0x1 << ETHCC_SW_P5_INBAND_SRC_OFFSET)
#define ETHCC_RFU2SW_P5_INBAND_OFFSET		25
#define ETHCC_RFU2SW_P5_INBAND_MASK		(0xF << ETHCC_RFU2SW_P5_INBAND_OFFSET)
#define ETHCC_LP_SERDES_DATA_SWAP_TX_OFFSET	29
#define ETHCC_LP_SERDES_DATA_SWAP_TX_MASK	(0x1 << ETHCC_LP_SERDES_DATA_SWAP_TX_OFFSET)
#define ETHCC_LP_SERDES_DATA_SWAP_RX_OFFSET	30
#define ETHCC_LP_SERDES_DATA_SWAP_RX_MASK	(0x1 << ETHCC_LP_SERDES_DATA_SWAP_RX_OFFSET)
#define ETHCC_SWP5_GBEPHY_TC_CLK_OFFSET		31
#define ETHCC_SWP5_GBEPHY_TC_CLK_MASK		(0x1 << ETHCC_SWP5_GBEPHY_TC_CLK_OFFSET)

/* Ethernet Complex Control 1 */
#define ETHCC_SWTCH_RESET_OFFSET		0
#define ETHCC_SWTCH_RESET_MASK			(0x1 << ETHCC_SWTCH_RESET_OFFSET)
#define ETHCC_SWTCH_MODE_OFFSET			8
#define ETHCC_SWTCH_MODE_MASK			(0xF << ETHCC_SWTCH_MODE_OFFSET)
#define ETHCC_SWTCH_CHIP_TYPE_OFFSET		12
#define ETHCC_SWTCH_CHIP_TYPE_MASK		(0xF << ETHCC_SWTCH_CHIP_TYPE_OFFSET)
#define ETHCC_SWTCH_P5_MODE_OFFSET		16
#define ETHCC_SWTCH_P5_MODE_MASK           	(0x7 << ETHCC_SWTCH_P5_MODE_OFFSET)
#define ETHCC_SWTCH_P5_HD_OFFSET		19
#define ETHCC_SWTCH_P5_HD_MASK			(0x1 << ETHCC_SWTCH_P5_HD_OFFSET)
#define ETHCC_SWTCH_P6_MODE_OFFSET		20
#define ETHCC_SWTCH_P6_MODE_MASK		(0x7 << ETHCC_SWTCH_P6_MODE_OFFSET)
#define ETHCC_SWTCH_P6_HD_OFFSET		23
#define ETHCC_SWTCH_P6_HD_MASK			(0x1 << ETHCC_SWTCH_P6_HD_OFFSET)
#define ETHCC_SWTCH_PTP_EXT_CLK_OFFSET		24
#define ETHCC_SWTCH_PTP_EXT_CLK_MASK		(0x1 << ETHCC_SWTCH_PTP_EXT_CLK_OFFSET)
#define ETHCC_SWTCH_ADDR_OFFSET			25
#define ETHCC_SWTCH_ADDR_MASK	        	(0x1F << ETHCC_SWTCH_ADDR_OFFSET)
#define ETHCC_SWTCH_JUMBO_EN_OFFSET		30
#define ETHCC_SWTCH_JUMBO_EN_MASK		(0x1 << ETHCC_SWTCH_JUMBO_EN_OFFSET)
#define ETHCC_SWTCH_DIS_AUTO_MED_OFFSET		31
#define ETHCC_SWTCH_DIS_AUTO_MED_MASK		(0x1 << ETHCC_SWTCH_DIS_AUTO_MED_OFFSET)

/* Ethernet Complex Control 2 */
#define ETHCC_SW_USE_EEPROM_OFFSET		8
#define ETHCC_SW_USE_EEPROM_MASK		(0x1 << ETHCC_SW_USE_EEPROM_OFFSET)
#define ETHCC_SW_FD_FLOW_DIS_OFFSET		9
#define ETHCC_SW_FD_FLOW_DIS_MASK		(0x1 << ETHCC_SW_FD_FLOW_DIS_OFFSET)
#define ETHCC_SW_HD_FLOW_DIS_OFFSET		10
#define ETHCC_SW_HD_FLOW_DIS_MASK		(0x1 << ETHCC_SW_HD_FLOW_DIS_OFFSET)
#define ETHCC_SW_100FX_MODE_OFFSET		11
#define ETHCC_SW_100FX_MODE_MASK		(0x7F << ETHCC_SW_100FX_MODE_OFFSET)
#define ETHCC_SW_PRT_STATE_OFFSET		18
#define ETHCC_SW_PRT_STATE_MASK			(0x7F << ETHCC_SW_PRT_STATE_OFFSET)
#define ETHCC_SW_CLK_125_OFFSET			25
#define ETHCC_SW_CLK_125_MASK			(0x1 << ETHCC_SW_CLK_125_OFFSET)

/* Ethernet Complex Control 3 */
#define ETHCC_SW_PX_FRC_SPD_OFFSET(prt)		(prt * 4)
#define ETHCC_SW_PX_FRC_SPD_MASK(prt)		(0x1 << ETHCC_SW_PX_FRC_SPD_OFFSET(prt))
#define ETHCC_SW_PX_FRC_GMII_SPD_OFFSET(prt)	((prt * 4) + 1)
#define ETHCC_SW_PX_FRC_GMII_SPD_MASK(prt)	(0x1 << ETHCC_SW_PX_FRC_GMII_SPD_OFFSET(prt))
#define ETHCC_SW_PX_FRC_MII_SPD_OFFSET(prt)	((prt * 4) + 2)
#define ETHCC_SW_PX_FRC_MII_SPD_MASK(prt)	(0x1 << ETHCC_SW_PX_FRC_MII_SPD_OFFSET(prt))
#define ETHCC_FEPHY_TX_DATA_SMPL_OFFSET		26
#define ETHCC_FEPHY_TX_DATA_SMPL_MASK		(0x1 << ETHCC_FEPHY_TX_DATA_SMPL_OFFSET)
#define ETHCC_GEPHY_TX_DATA_SMPL_OFFSET		28
#define ETHCC_GEPHY_TX_DATA_SMPL_MASK		(0x3 << ETHCC_GEPHY_TX_DATA_SMPL_OFFSET)

/* PCSn Control Register. */
#define MV_ETHCOMP_PCS_CTRL_REG(id)		(MV_ETH_COMPLEX_BASE + 0x30 + (id * 4))
#define PCSCTRL_PORT_ANEG_AN_OFFSET		1
#define PCSCTRL_PORT_RESTART_AN_OFFSET		2
#define PCSCTRL_PORT_ADV_PAUSE_OFFSET		3
#define PCSCTRL_PORT_ADV_ASM_PAUSE_OFFSET	10
#define PCSCTRL_PORT_ANEG_DUPLEX_AN_OFFSET	17
#define PCSCTRL_PORT_FC_AN_OFFSET		19
#define PCSCTRL_PORT_SPEED_AN_OFFSET		21
#define PCSCTRL_PORT_ACTIVE_OFFSET		25
#define PCSCTRL_PORT_ACTIVE_MASK		(0x1 << PCSCTRL_PORT_ACTIVE_OFFSET)
#define PCSCTRL_PORT_EN_OFFSET			26
#define PCSCTRL_PORT_EN_MASK			(0x1 << PCSCTRL_PORT_EN_OFFSET)
#define PCSCTRL_PORT_RESET_OFFSET		27
#define PCSCTRL_PORT_RESET_MASK			(0x1 << PCSCTRL_PORT_RESET_OFFSET)

#define MV_ETHCOMP_PCS_STATUS_REG(id)		(MV_ETH_COMPLEX_BASE + 0x40 + (id * 4))
#define MV_ETHCOMP_INT_MAIN_CAUSE_REG		(MV_ETH_COMPLEX_BASE + 0x50)
#define MV_ETHCOMP_INT_MAIN_MASK_REG		(MV_ETH_COMPLEX_BASE + 0x54)
#define MV_ETHCOMP_FE_PHY_CTRL_REG		(MV_ETH_COMPLEX_BASE + 0x80)

#define MV_ETHCOMP_SWITCH_INT_OFFSET		0
#define MV_ETHCOMP_SWITCH_INT_MASK		    (0x1 << MV_ETHCOMP_SWITCH_INT_OFFSET)
#define MV_ETHCOMP_PCS0_LINK_INT_OFFSET		1
#define MV_ETHCOMP_PCS0_LINK_INT_MASK		(0x1 << MV_ETHCOMP_PCS0_LINK_INT_OFFSET)
#define MV_ETHCOMP_PCS1_LINK_INT_OFFSET		4
#define MV_ETHCOMP_PCS1_LINK_INT_MASK		(0x1 << MV_ETHCOMP_PCS1_LINK_INT_OFFSET)
#define MV_ETHCOMP_PCS2_LINK_INT_OFFSET		7
#define MV_ETHCOMP_PCS2_LINK_INT_MASK		(0x1 << MV_ETHCOMP_PCS2_LINK_INT_OFFSET)
#define MV_ETHCOMP_PCS3_LINK_INT_OFFSET		10
#define MV_ETHCOMP_PCS3_LINK_INT_MASK		(0x1 << MV_ETHCOMP_PCS3_LINK_INT_OFFSET)
#define MV_ETHCOMP_GEPHY_INT_OFFSET		13
#define MV_ETHCOMP_GEPHY_INT_MASK		(0x1 << MV_ETHCOMP_GEPHY_INT_OFFSET)

/* FE PHY Control Register. */
#define MV_ETHCOMP_FE_PHY_STATUS_REG		(MV_ETH_COMPLEX_BASE + 0x84)
#define ETHCC_FE_PHY_AN_MODE_OFFSET(prt)	((prt + 1) * 3)
#define ETHCC_FE_PHY_AN_MODE_MASK(prt)		(0xF << ETHCC_FE_PHY_AN_MODE_OFFSET(prt))
#define ETHCC_FE_PHY_XOVER_EN_OFFSET		14
#define ETHCC_FE_PHY_XOVER_EN_MASK		(1 << ETHCC_FE_PHY_XOVER_EN_OFFSET)
#define ETHCC_FE_PHY_RESET_OFFSET		15
#define ETHCC_FE_PHY_RESET_MASK			(0x1 << ETHCC_FE_PHY_RESET_OFFSET)
#define ETHCC_FE_PHY_EXT_PWR_DOWM_OFFSET	16
#define ETHCC_FE_PHY_EXT_PWR_DOWM_MASK		(0x7 << ETHCC_FE_PHY_EXT_PWR_DOWM_OFFSET)

#define MV_ETHCOMP_LP_SERDES_CTRL_REG		(MV_ETH_COMPLEX_BASE + 0x94)

/* GbE PHY Control Register. */
#define MV_ETHCOMP_GE_PHY_CTRL_REG		(MV_ETH_COMPLEX_BASE + 0xA0)
#define MV_ETHCOMP_RSRVD_CTRL_REG		(MV_ETH_COMPLEX_BASE + 0xA4)

#define PHYCTRL_ENRGY_DETECT_OFFSET		0x0
#define PHYCTRL_ENRGY_DETECT_MASK		(0x1 << PHYCTRL_ENRGY_DETECT_OFFSET)
#define PHYCTRL_CFG_ANEG_ADV_OFFSET		1
#define PHYCTRL_CFG_ANEG_ADV_MASK		(0x3 << PHYCTRL_CFG_ANEG_ADV_OFFSET)
#define PHYCTRL_CONF_MSTR_SLAVE_OFFSET		3
#define PHYCTRL_CONF_MSTR_SLAVE_MASK		(0x1 << PHYCTRL_CONF_MSTR_SLAVE_OFFSET)
#define PHYCTRL_XOVER_EN_OFFSET			4
#define PHYCTRL_XOVER_EN_MASK			(0x1 << PHYCTRL_XOVER_EN_OFFSET)
#define PHYCTRL_PHY_PWR_DOWN_OFFSET		5
#define PHYCTRL_PHY_PWR_DOWN_MASK		(0x1 << PHYCTRL_PHY_PWR_DOWN_OFFSET)
#define PHYCTRL_PG_PORT_NUM_OFFSET		6
#define PHYCTRL_PG_PORT_NUM_MASK		(0x7 << PHYCTRL_PG_PORT_NUM_OFFSET)
#define PHYCTRL_CONF_ADV_EEE_OFFSET		9
#define PHYCTRL_CONF_ADV_EEE_MASK		(0x1 << PHYCTRL_CONF_ADV_EEE_OFFSET)
#define PHYCTRL_CONF_NORMAL_MODE_OFFSET		10
#define PHYCTRL_CONF_NORMAL_MODE_MASK		(0x1 << PHYCTRL_CONF_NORMAL_MODE_OFFSET)
#define PHYCTRL_THERM_SENS_EN_OFFSET		11
#define PHYCTRL_THERM_SENS_EN_MASK		(0x1 << PHYCTRL_THERM_SENS_EN_OFFSET)
#define PHYCTRL_RESET_OFFSET			14
#define PHYCTRL_RESET_MASK			(0x1 << PHYCTRL_RESET_OFFSET)
#define PHYCTRL_DPLL_RESET_OFFSET		16
#define PHYCTRL_DPLL_RESET_MASK			(0x1 << PHYCTRL_DPLL_RESET_OFFSET)
#define PHYCTRL_CLCK125_EN_OFFSET		17
#define PHYCTRL_CLCK125_EN_MASK			(0x1 << PHYCTRL_CLCK125_EN_OFFSET)
#define PHYCTRL_CLCK50_EN_OFFSET		18
#define PHYCTRL_CLCK50_EN_MASK			(0x1 << PHYCTRL_CLCK50_EN_OFFSET)
#define PHYCTRL_EXT_PWR_DOWN_SRC_OFFSET		25
#define PHYCTRL_EXT_PWR_DOWN_SRC_MASK		(0x1 << PHYCTRL_EXT_PWR_DOWN_SRC_OFFSET)

#define MV_ETHCOMP_GE_PHY_STATUS_REG		(MV_ETH_COMPLEX_BASE + 0xA8)

/* Serdes Configuration Register */
#define MV_ETHCOMP_SERDESCFG_REG		(MV_ETH_ONLY_REGS_BASE + 0x4A0)
#define MV_SATACOMP_SERDESCFG_REG		(MV_SATA_REGS_OFFSET + 0x2058)

#define SERDES_CFG_PU_PLL_OFFSET		0
#define SERDES_CFG_PU_PLL_MASK			(0x1 << SERDES_CFG_PU_PLL_OFFSET)
#define SERDES_CFG_PU_RX_OFFSET			1
#define SERDES_CFG_PU_RX_MASK			(0x1 << SERDES_CFG_PU_RX_OFFSET)
#define SERDES_CFG_PU_TX_OFFSET			2
#define SERDES_CFG_PU_TX_MASK			(0x1 << SERDES_CFG_PU_TX_OFFSET)
#define SERDES_CFG_TX_IDLE_OFFSET		3
#define SERDES_CFG_TX_IDLE_MASK			(0x1 << SERDES_CFG_TX_IDLE_OFFSET)
#define SERDES_CFG_RX_INIT_OFFSET		4
#define SERDES_CFG_RX_INIT_MASK			(0x1 << SERDES_CFG_RX_INIT_OFFSET)
#define SERDES_CFG_PHY_GEN_TX_OFFSET		5
#define SERDES_CFG_PHY_GEN_TX_MASK		(0xF << SERDES_CFG_PHY_GEN_TX_OFFSET)
#define SERDES_CFG_PHY_GEN_RX_OFFSET		9
#define SERDES_CFG_PHY_GEN_RX_MASK		(0xF << SERDES_CFG_PHY_GEN_RX_OFFSET)
#define SERDES_CFG_DIG_TEST_EN_OFFSET		13
#define SERDES_CFG_DIG_TEST_EN_MASK		(0x1 << SERDES_CFG_DIG_TEST_EN_OFFSET)
#define SERDES_CFG_IDDQ_OFFSET			14
#define SERDES_CFG_IDDQ_MASK			(0x1 << SERDES_CFG_IDDQ_OFFSET)

/* MAC Configuration Register */
#define MV_MAC_SERIAL_CTRL1_REG		(MV_ETH_ONLY_REGS_BASE + 0x44C)
#define MV_MAC_SERIAL_CTRL0_REG		(MV_ETH_ONLY_REGS_BASE + 0x43C)

/* Serdes Status Register */
#define MV_ETHCOMP_SERDESSTS_REG		(MV_ETH_ONLY_REGS_BASE + 0x4A4)
#define MV_SATACOMP_SERDESSTS_REG		(MV_SATA_REGS_OFFSET + 0x205C)

void mvEthCompSkipInitSet(MV_BOOL skip);
MV_STATUS mvEthCompMac2SwitchConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSwitchReset(MV_U32 ethCompCfg);
MV_STATUS mvEthCompMac2RgmiiConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompMac1ToGePhyConfig(MV_U32 ethCompCfg, MV_U32 smiAddr);
MV_STATUS mvEthCompSwP45ToGePhyConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSwP5ToRgmiiConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSwTo3FePhyConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSataConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSwToQsgmiiPhyConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompMac0ToSgmiiConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthCompSwP1ToSgmiiConfig(MV_U32 ethCompCfg);
MV_STATUS mvEthernetComplexShutdownIf(MV_BOOL integSwitch, MV_BOOL gePhy, MV_BOOL fePhy);
MV_STATUS mvEthernetComplexInit(void);

#endif /* __INCmvCtrlEthCompLibh */
