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

#include <mvCommon.h>
#include <mvOs.h>
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "eth/gbe/mvEthRegs.h"

static MV_STATUS mvEthCompSerdesConfig(MV_U32 ethCompCfg);
static void mvEthCompSerdesRegWrite(MV_U32 regaddr, MV_U16 value);
static MV_U16 mvEthCompSerdesRegRead(MV_U32 regaddr);

static MV_BOOL gEthComplexSkipInit = MV_FALSE;

typedef struct {
	MV_U32 reg;
	MV_U16 val;
	MV_U16 mask;
} MV_SERDES_CFG;

static MV_SERDES_CFG serdesDefVal[] = {
	{0x22, 0x1046, 0xFFFF},
	{0x01, 0xF803, 0xFEFF},
	{0x02, 0x0140, 0x83F0},
	{0x03, 0x3600, 0xFF0F},
	{0x04, 0x600C, 0xF0FF},
	{0x05, 0x0000, 0xFFFF},
	{0x06, 0x3377, 0xBFFF},
	{0x07, 0x0200, 0xFFFF},
	{0x0D, 0xC958, 0xFFFF},
	{0x0E, 0x9000, 0xFFFF},
	{0x0F, 0xAA62, 0xFFFF},
	{0x10, 0x9000, 0xFFFF},
	{0x11, 0x0BEB, 0xFFFF},
	{0x12, 0x9055, 0xFFFF},
	{0x13, 0x0BEB, 0xFFFF},
	{0x14, 0x5055, 0xFFFF},
	{0x15, 0x0000, 0xCFFF},
	{0x16, 0x0000, 0xFFFF},
	{0x17, 0x0000, 0xFFFF},
	{0x18, 0x0000, 0xFFFF},
	{0x19, 0x0000, 0xFFFF},
	{0x1A, 0x0000, 0xFFFF},
	{0x1B, 0x0000, 0xFFFF},
	{0x1C, 0x0000, 0x0000},
	{0x1D, 0x0000, 0x0000},
	{0x1E, 0x0000, 0x0000},
	{0x1F, 0x0000, 0x0000},
	{0x20, 0x0000, 0x0000},
	{0x21, 0x0030, 0xFFFF},
	{0x23, 0x0800, 0xFFFF},
	{0x24, 0x4243, 0xDFFF},
	{0x25, 0x03FF, 0xFFFF},
	{0x26, 0x0100, 0xFFFF},
	{0x27, 0x0519, 0xFFFF},
	{0x28, 0x5502, 0xFFFF},
	{0x29, 0x5555, 0xFFFF},
	{0x3A, 0x0000, 0xFFFF},
	{0x4D, 0x2195, 0xFFFF},
	{0x4F, 0x6000, 0xFBFF},
	{0x50, 0x0040, 0xFFFF},
	{0x51, 0x0280, 0xFFFF},
	{0x51, 0x0280, 0xFFFF},
	{0x52, 0xE008, 0xFFFF},
	{0x53, 0x24B1, 0xFFFF},
	{0x54, 0x0045, 0xFFFF},
	{0x55, 0x4100, 0xCFFF},
	{0x56, 0x0000, 0xFF0C},
	{0x57, 0x0000, 0xFFE0},
	{0x58, 0xB200, 0xFFE0},
	{0x59, 0x0000, 0xFFE0},
	{0x5A, 0x3000, 0x7F8F},
	{0x5B, 0x0000, 0xFFE0},
	{0x60, 0x0180, 0xFFF0},
	{0x61, 0x0000, 0x0040},
	{0x62, 0x0500, 0x8FFF},
	{0x63, 0xFC00, 0xFFFF},
	{0x64, 0x0150, 0xFFFF},
	{0x65, 0x0280, 0xFFFF},
	{0x66, 0x2400, 0xFFFF},
	{0x67, 0x000F, 0xFFFF},
	{0x68, 0x0466, 0xFFFF},
	{0x69, 0x2860, 0xFFFF},
	{0x6A, 0x00F0, 0xFFFF},
	{0x6B, 0x0000, 0xD800},
	{0x6C, 0x0000, 0xFFFF},
	{0x6D, 0x1400, 0x7FFF},
	{0x75, 0x0080, 0x00FF},
	{0x79, 0x0000, 0xFFFF},
	{0x7A, 0x0000, 0x0000},
	{0x7B, 0x0000, 0xFFFF},
	{0x7C, 0x0000, 0xFFFF},
	{0x7D, 0x0000, 0xFFFF},
	{0x7E, 0x0022, 0xFFFF},
	{0x7F, 0x0000, 0x00FF}
};

static void mvEthCompSetSerdesDefaults(void)
{
	MV_U32 i;
	MV_U32 size = sizeof(serdesDefVal) / sizeof(serdesDefVal[0]);
	MV_SERDES_CFG *srdsCfg;
	MV_U16 val;

	for (i = 0; i < size; i++) {
		srdsCfg = &serdesDefVal[i];
		if (srdsCfg->mask == 0x0)
			continue;
		val = mvEthCompSerdesRegRead(srdsCfg->reg);
		val &= ~srdsCfg->mask;
		val |= srdsCfg->val;
		mvEthCompSerdesRegWrite(srdsCfg->reg, val);
	}
	return;
}

/******************************************************************************
* mvEthCompSkipInitSet
*
* DESCRIPTION:
*	Configure the eth-complex to skip initialization.
*
* INPUT:
*	skip - MV_TRUE to skip initialization.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
void mvEthCompSkipInitSet(MV_BOOL skip)
{
	gEthComplexSkipInit = skip;
	return;
}

/******************************************************************************
* mvEthCompMac2SwitchConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0/1 to switch ports 5/6 mode.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/

#ifdef ARUBA_MARVELL_SWITCH
#define SWITCH_PHY_ADDR(x) 0x8
#else
#define SWITCH_PHY_ADDR(x) mvBoardPhyAddrGet(x)
#endif

MV_STATUS mvEthCompMac2SwitchConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg, portEnabled = 0;

	if (!(ethCompCfg & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)))
		return MV_OK;

	/* GbE-MAC-0 to Switch P4 (1000Mbps)    */
	/* GbE-MAC-1 to Switch P5 (1000Mbps)    */

	/* Set switch phy address */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(1));
	reg &= ~ETHCC_SWTCH_ADDR_MASK;
	if (ethCompCfg & ESC_OPT_MAC0_2_SW_P4)
		reg |= (SWITCH_PHY_ADDR(0) << ETHCC_SWTCH_ADDR_OFFSET);
	else
		reg |= (SWITCH_PHY_ADDR(1) << ETHCC_SWTCH_ADDR_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(1), reg);

	/*
	 * 3.1.2. Switch power-on configurations:
	 * 3.1.2.1. Activate Switch ports 1-6 (1 bit per port): set Regunit
	 * Ethernet_Complex_Control_2 register, field SwitchPortState, to 0x7E.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(2));
	reg &= ~ETHCC_SW_PRT_STATE_MASK;

	if (ethCompCfg & ESC_OPT_RGMIIA_SW_P5)
		portEnabled |= BIT5;
	if (ethCompCfg & ESC_OPT_RGMIIA_SW_P6)
		portEnabled |= BIT6;
	if (ethCompCfg & ESC_OPT_MAC0_2_SW_P4)
		portEnabled |= BIT4;
	if (ethCompCfg & ESC_OPT_MAC1_2_SW_P5)
		portEnabled |= BIT5;
	if (ethCompCfg & ESC_OPT_GEPHY_SW_P0)
		portEnabled |= BIT0;
	if (ethCompCfg & ESC_OPT_GEPHY_SW_P5)
		portEnabled |= BIT5;
	if (ethCompCfg & ESC_OPT_FE3PHY)
		portEnabled |= BIT1 | BIT2 | BIT3;
	if (ethCompCfg & ESC_OPT_QSGMII)
		portEnabled |= BIT0 | BIT1 | BIT2 | BIT3;

	reg |= (portEnabled << ETHCC_SW_PRT_STATE_OFFSET);	/* 0x7E */
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(2), reg);

	/* 3.1.3. Ethernet-Complex configuration:
	 * 3.1.3.1. Configure Switch 125Mhz clock source: set
	 * Regunit Ethernet_Complex_Control_0 register, field SwFi125ClkSrc to
	 * MiscPLL (0x1).
	 */
	MV_REG_BIT_RESET(MV_ETHCOMP_CTRL_REG(0), ETHCC_SW_FI_125_CLK_MASK);
	MV_REG_BIT_SET(MV_ETHCOMP_CTRL_REG(0), (1 << ETHCC_SW_FI_125_CLK_OFFSET));

	/* 3.1.3.2. Configure G-0 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field Gport0Source to
	 * "SwitchPort6" (0x1).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	if (ethCompCfg & ESC_OPT_MAC0_2_SW_P4) {
		reg &= ~ETHCC_GPORT_0_SRC_MASK;
		reg |= (0x1 << ETHCC_GPORT_0_SRC_OFFSET);
	}

	/* 3.2.3.2. Configure G-1 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field Gport1Source to
	 * SwitchPort5" (0x1).
	 */
	if (ethCompCfg & ESC_OPT_MAC1_2_SW_P5) {
		reg &= ~ETHCC_GPORT_1_SRC_MASK;
		reg |= (0x1 << ETHCC_GPORT_1_SRC_OFFSET);
	}
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	/* 3.2.3.3. Configure Switch P5 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwitchPort5Source to
	 * "Gport1" (0x1).
	 */
	if (ethCompCfg & ESC_OPT_MAC1_2_SW_P5) {
		reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
		reg &= ~ETHCC_SW_PORT_5_SRC_MASK;
		reg |= (0x1 << ETHCC_SW_PORT_5_SRC_OFFSET);
		MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);
	}

	/* 3.1.4.2. Wait for Switch, Switch Global Status Register, field
	 * EEInt to be set to 0x1.
	 */
	mvOsDelay(100);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSwitchReset
*
* DESCRIPTION:
*	Reset switch device after being configured by ethernet complex functions.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwitchReset(MV_U32 ethCompCfg)
{
	MV_U32 reg;

	if (!(ethCompCfg & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5))) {
		/* If no switch is connected, then we need to enable the 25MHz
		 ** clock, and get the switch out of reset
		 */

		/* Set switch phy address so it does not collide with other
		 ** addresses.
		 */
		reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(1));
		reg &= ~ETHCC_SWTCH_ADDR_MASK;
		reg |= (0xF << ETHCC_SWTCH_ADDR_OFFSET);
		MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(1), reg);
		MV_REG_BIT_SET(MV_ETHCOMP_CTRL_REG(0), (1 << ETHCC_SW_FI_125_CLK_OFFSET));
		MV_REG_BIT_SET(MV_ETHCOMP_CTRL_REG(1), ETHCC_SWTCH_RESET_MASK);
		return MV_OK;
	}

	/* Disable polling on MAC ports. */
	if (ethCompCfg & ESC_OPT_MAC0_2_SW_P4)
		MV_REG_BIT_RESET(ETH_UNIT_CONTROL_REG(0), BIT1);
	if (ethCompCfg & ESC_OPT_MAC1_2_SW_P5)
		MV_REG_BIT_RESET(ETH_UNIT_CONTROL_REG(1), BIT1);

	/*
	 * 3.1.4. Reset de-assertion:
	 * 3.1.4.1. De-assert Switch reset: set Regunit
	 * Ethernet_Complex_Control_1 register, field SwitchReset to 0x1.
	 */
	MV_REG_BIT_SET(MV_ETHCOMP_CTRL_REG(1), ETHCC_SWTCH_RESET_MASK);

//#warning "Fix this to poll the Switch EEInt after reset"
	mvOsDelay(100);
	return MV_OK;
}

/******************************************************************************
* mvEthCompMac2RgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0/1 to RGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac2RgmiiConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;

	/* 3.3. GbE-MAC-0/1 to MII (10Mbps/ 100 Mbps) or RGMII
	 * (10Mbps/ 100Mbps/ 1000Mbps).
	 */

	/* 3.3.2. Ethernet-Complex configuration:
	 * 3.3.2.1. Configure G-0/1 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field Gport1Source or
	 * Gport0Source to "ExtRGMIIA/B" (0x0).
	 */
	if (mvBoardIsPortInRgmii(0)) {
		reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
		reg &= ~ETHCC_GPORT_0_SRC_MASK;
		MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);
	}

	if (mvBoardIsPortInRgmii(1)) {
		reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
		reg &= ~ETHCC_GPORT_1_SRC_MASK;
		MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);
	}

	return MV_OK;
}

/******************************************************************************
* mvEthCompMac1ToGePhyConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC1 to integrated GE PHY mode.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*	smiAddr - SMI address to assign for the integrated GE phy.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac1ToGePhyConfig(MV_U32 ethCompCfg, MV_U32 smiAddr)
{
	MV_U32 reg;

	if (!(ethCompCfg & ESC_OPT_GEPHY_MAC1))
		return MV_OK;

	/* 3.4. GPHY to GbE-MAC-1 (10Mbps/ 100Mbps/ 1000Mbps)
	 * Select a unique SMI address (0x0-0x1F, excluding 0x7, 0x8) for the
	 * GbE-PHY.
	 */

	/* 3.4.2. Ethernet-Complex configuration:
	 * 3.4.2.1. Configure G-1 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field Gport1Source to 0x0.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	reg &= ~ETHCC_GPORT_1_SRC_MASK;
	reg |= (0x2 << ETHCC_GPORT_1_SRC_OFFSET);

	/* 3.4.2.2. Configure GbE-PHY connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field GePhySource to "Gport1 " (0x1).
	 */
	reg &= ~ETHCC_BGE_PHY_SRC_MASK;
	reg |= (0x1 << ETHCC_BGE_PHY_SRC_OFFSET);

	/* 3.4.2.3. Configure GbE-Phy SMI master: set Regunit
	 * Ethernet_Complex_Control_0 register, field GePhySmiSource to "Gport0 " (0x1).
	 */
	reg &= ~ETHCC_GBE_PHY_SMI_SRC_MASK;
	reg |= (0x1 << ETHCC_GBE_PHY_SMI_SRC_OFFSET);

	/* 3.4.2.4. Configure GbE-Phy SMI address: set Regunit
	 * Ethernet_Complex_Control_0 register, field GbEPhySMIAddress, to
	 * the selected GbE-PHY SMI address.
	 */
	reg &= ~ETHCC_GBE_PHY_SMI_ADD_MASK;
	reg |= (smiAddr << ETHCC_GBE_PHY_SMI_ADD_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	/* 3.4.3. Power-on configurations:
	 * 3.4.3.1. Configure GbE-PHY capabilities to advertise: set Regunit
	 * GbE Phy Control register, field ConfAnegAdvertise to 0x3.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_GE_PHY_CTRL_REG);
	reg &= ~PHYCTRL_CFG_ANEG_ADV_MASK;
	reg |= (0x3 << PHYCTRL_CFG_ANEG_ADV_OFFSET);
	reg |= PHYCTRL_XOVER_EN_MASK;
	MV_REG_WRITE(MV_ETHCOMP_GE_PHY_CTRL_REG, reg);
#if 0
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(3));
	reg &= ~ETHCC_GEPHY_TX_DATA_SMPL_MASK;
	reg |= (0x2 << ETHCC_GEPHY_TX_DATA_SMPL_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(3), reg);
#endif
	/* 3.4.4. Reset de-assertion:
	 * 3.4.4.1. De-assert GbE-PHY reset: set Regunit GbE Phy Control
	 * register, field Reset to 0x1.
	 */
	MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_DPLL_RESET_MASK);
	MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_RESET_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSwP05ToGePhyConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch port 4/5 to integrated GE PHY.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwP05ToGePhyConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;
	MV_U32 smiAddr = 0;

	if (!(ethCompCfg & (ESC_OPT_GEPHY_SW_P0 | ESC_OPT_GEPHY_SW_P5)))
		return MV_OK;

	/* 3.5. GPHY to Switch P4/P5 (10Mbps/ 100Mbps/ 1000Mbps) */
	/* Select a unique SMI address (0x0-0x1F, excluding 0x7, 0x8) for the
	 * GbE-PHY.
	 */

	/* 3.5.2.2. Configure Switch P5/P4 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field
	 * "SwitchPort4Source"/" SwitchPort5Source" to GbEPHY (0x2).
	 * 3.5.2.3. Configure GbE-PHY connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field GePhySource to
	 * SwitchPort4 (0x2) or SwitchPort5 (0x0).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	if (ethCompCfg & ESC_OPT_GEPHY_SW_P0) {
		reg &= ~ETHCC_SW_PORT_0_SRC_MASK;
		reg |= (0x2 << ETHCC_SW_PORT_0_SRC_OFFSET);
		reg &= ~ETHCC_BGE_PHY_SRC_MASK;
		reg |= (0x2 << ETHCC_BGE_PHY_SRC_OFFSET);
		smiAddr = 0x0;
	}
	if (ethCompCfg & ESC_OPT_GEPHY_SW_P5) {
		reg &= ~ETHCC_SW_PORT_5_SRC_MASK;
		reg |= (0x2 << ETHCC_SW_PORT_5_SRC_OFFSET);
		reg &= ~ETHCC_BGE_PHY_SRC_MASK;
		reg |= (0x0 << ETHCC_BGE_PHY_SRC_OFFSET);
		smiAddr = 0x5;
	}

	/* 3.5.2.4. Configure GbE-Phy SMI master: set Regunit
	 * Ethernet_Complex_Control_0 register, field GePhySmiSource to
	 * Switch (0x0).
	 */
	reg &= ~ETHCC_GBE_PHY_SMI_SRC_MASK;

	/* 3.5.2.5. Configure GbE-Phy SMI address: set Regunit
	 * Ethernet_Complex_Control_0 register, field GbEPhySMIAddress, to
	 * the selected GbE-PHY SMI address.
	 */
	reg &= ~ETHCC_GBE_PHY_SMI_ADD_MASK;
	reg |= (smiAddr << ETHCC_GBE_PHY_SMI_ADD_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	reg = MV_REG_READ(MV_ETHCOMP_GE_PHY_CTRL_REG);
	reg &= ~PHYCTRL_CFG_ANEG_ADV_MASK;
	reg |= (0x3 << PHYCTRL_CFG_ANEG_ADV_OFFSET);
	reg |= PHYCTRL_XOVER_EN_MASK;
	MV_REG_WRITE(MV_ETHCOMP_GE_PHY_CTRL_REG, reg);
#if 0
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(3));
	reg &= ~ETHCC_GEPHY_TX_DATA_SMPL_MASK;
	reg |= (0x2 << ETHCC_GEPHY_TX_DATA_SMPL_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(3), reg);
#endif
	/* 3.5.3.2. De-assert GbE-PHY reset: set Regunit GbE Phy Control
	 * register, field Reset to 0x1.
	 */
	MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_DPLL_RESET_MASK);
	MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_RESET_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSwP56ToRgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch port 5 or 6 to RGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwP56ToRgmiiConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;

	if (!(ethCompCfg & (ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6)))
		return MV_OK;

	/* 3.6. Switch P5 to MII (10Mbps/ 100 Mbps) or RGMII (10Mbps/
	 * 100Mbps/ 1000Mbps)
	 */

	/* 3.6.3. Ethernet-Complex configuration:
	 * 3.6.3.2. Configure Switch P5 connection: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwitchPort5Source to
	 * "ExtRGMIIB" (0x0).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	if (ethCompCfg & ESC_OPT_RGMIIA_SW_P5)
		reg &= ~ETHCC_SW_PORT_5_SRC_MASK;

	/* 3.6.3.3. Configure MII mode or RGMII mode: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwitchPort5MppMode,
	 * to RGMII (0x0) or MII (0x1).
	 */
	if (ethCompCfg & ESC_OPT_RGMIIA_SW_P5)
		reg &= ~ETHCC_SW_PORT5_MPP_MASK;
	else if (ethCompCfg & ESC_OPT_RGMIIA_SW_P6)
		reg &= ~ETHCC_SW_PORT6_MPP_MASK;

	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);
	return MV_OK;
}

/******************************************************************************
* mvEthCompSwTo3FePhyConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch ports 1-4 to 3xFE output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwTo3FePhyConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;
	MV_U32 i;

	if (!(ethCompCfg & ESC_OPT_FE3PHY))
		return MV_OK;

	/* 3.7. Switch P1,2 & 3 to FE-PHY (10Mbps/ 100 Mbps) */
	/* 3.7.2. Ethernet-Complex configuration :
	 * 3.7.2.2. Connect Switch ports 1,2 & 3 to FE-PPHY ports 0,1 & 2: set
	 * Regunit Ethernet_Complex_Control_0 register, field
	 * SwitchPort123Source to "FEPHY" (0x1).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	reg &= ~ETHCC_SW_PORT_123_SRC_MASK;
	reg |= (0x1 << ETHCC_SW_PORT_123_SRC_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	/* 3.7.2.3. Enable Switch and FE-PHY TX clock, by setting speed to 25MHz
	 * (this is needed due to design bug): set Regunit Ethernet Complex
	 * Control 3 register, fields "SwPort1ForceSpeed",
	 * "SwPort1ForceMiiSpeed", "SwPort2ForceSpeed", "SwPort2ForceMiiSpeed",
	 * "SwPort3ForceSpeed" and  "SwPort3ForceMiiSpeed" to force MII-100,
	 * i.e. 25MHz TX clock. All these fields should be set to 0x1.
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(3));
	for (i = 1; i < 4; i++) {
		reg &= ~ETHCC_SW_PX_FRC_SPD_MASK(i);
/* 		reg |= ETHCC_SW_PX_FRC_MII_SPD_MASK(i); */
	}
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(3), reg);

	/* 3.7.3. Reset de-assertion:
	 * 3.7.3.1. De-assert F-PHY reset: set Regunit FE PHY Control 0
	 * register, field "fe_phy_pd_reset_a" to Normal mode (0x1).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_FE_PHY_CTRL_REG);
	for (i = 0; i < 3; i++)
		reg |= (0x3 << ETHCC_FE_PHY_AN_MODE_OFFSET(i));

	MV_REG_BIT_RESET(MV_ETHCOMP_CTRL_REG(3), ETHCC_FEPHY_TX_DATA_SMPL_MASK);

	/* Enable FE Phy Crossover */
	reg |= (0x1 << ETHCC_FE_PHY_XOVER_EN_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_FE_PHY_CTRL_REG, reg);

	MV_REG_BIT_RESET(MV_ETHCOMP_FE_PHY_CTRL_REG, ETHCC_FE_PHY_RESET_MASK);
	mvOsDelay(100);
	MV_REG_BIT_SET(MV_ETHCOMP_FE_PHY_CTRL_REG, ETHCC_FE_PHY_RESET_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSataConfig
*
* DESCRIPTION:
*	Configure ethernet complex for sata port output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSataConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;
	MV_U32 tmp;
	MV_U16 serdesReg;

	if (!(ethCompCfg & ESC_OPT_SATA))
		return MV_OK;

	/* 3.8.3. LP_SERDES_PHY initialization:
	 * 3.8.3.1. Set LP_SERDES to reset: set Regunit Software Reset Control
	 * register to Reset (0x1).
	 */
	MV_REG_BIT_SET(SOFT_RESET_CTRL_REG, SRC_LPSRDSSWRSTN_MASK);

	/* 3.8.3.2. De-assert LP_SERDES reset: set Regunit Software Reset
	 * Control register to 0x0.
	 */
	MV_REG_BIT_RESET(SOFT_RESET_CTRL_REG, SRC_LPSRDSSWRSTN_MASK);

	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));

	/* 3.8.2.4. Connect LP_SERDES_PHY to required source: set Regunit
	 * Ethernet_Complex_Control_0 register, field "LpphyMode" to "lpphyMode".
	 */
	tmp = 0x1;

	reg &= ~ETHCC_LPPHYMODE_MASK;
	reg |= (tmp << ETHCC_LPPHYMODE_OFFSET);
	reg |= ETHCC_LP_SERDES_DATA_SWAP_TX_MASK | ETHCC_LP_SERDES_DATA_SWAP_RX_MASK;
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	mvEthCompSetSerdesDefaults();

	/* 3.8.3.3. Power up LP_SERDES: set Gunit Serdes Configuration
	 * (SERDESCFG) register, fields "PU_TX", "PU_RX, "PU_PLL" (bits 1,2,3)
	 * to Enable (0x1)
	 */
	reg = MV_REG_READ(MV_SATACOMP_SERDESCFG_REG);
	reg &= ~SERDES_CFG_PU_TX_MASK;
	reg &= ~SERDES_CFG_PU_RX_MASK;
	reg &= ~SERDES_CFG_PU_PLL_MASK;
	MV_REG_WRITE(MV_SATACOMP_SERDESCFG_REG, reg);

	/* 3.8.3.4. Configuring the LP_SERDES to Reference clock 25M, and
	 * PHY_MODE to SERDES APB3 Reg 0x1. Access to LP_SERDES registers is
	 * done by accessing Gunit0, base address
	 * (0xE00 + (Serdes Reg Address << 2)) 0xE00 + 0x1<<2 = 0x4 + 0xE00 = 0xE04.
	 * Configure to -0xF88.
	 */
	mvEthCompSerdesRegWrite(0x1, 0xF801);

	/* 3.8.3.5. Configyre LP_SERED data rate, by setting Gunit Serdes
	 * Configuration (SERDESCFG) register, fields "PIN_PHY_GEN_TX" and
	 * "PIN_PHY_GEN_RX" to 5Gbps (0xA).
	 */
	tmp = 0x1;

	reg = MV_REG_READ(MV_SATACOMP_SERDESCFG_REG);
	reg &= ~SERDES_CFG_PHY_GEN_RX_MASK;
	reg |= (tmp << SERDES_CFG_PHY_GEN_RX_OFFSET);
	reg &= ~SERDES_CFG_PHY_GEN_TX_MASK;
	reg |= (tmp << SERDES_CFG_PHY_GEN_TX_OFFSET);
	MV_REG_WRITE(MV_SATACOMP_SERDESCFG_REG, reg);

	/* Set PHY Gen Tx & Rx to 0xA, Same as in reg MV_ETHCOMP_SERDESCFG_REG
	 ** Bit[9] - Dig_test_bus_en.
	 */
	mvEthCompSerdesRegWrite(0x26, 0x111);

	/* 3.8.3.6. Configuring the LP_SERDES to 10 bit interface, by setting
	 * its internal register 0x23[11:10] to 0x1. Access to LP_SERDES
	 * registers is done by accessing Gunit0, base address
	 * (0xE00 + (Serdes Reg Address << 2)) = 0xE00 + 0x23<<2 =  0xE00 + 0x8c = 0xE8c.
	 */
	serdesReg = mvEthCompSerdesRegRead(0x23);
	serdesReg &= ~(0x3 << 10);

	if ((ethCompCfg & ESC_OPT_SATA) || (ethCompCfg & ESC_OPT_QSGMII))
		serdesReg |= (0x1 << 10);

	mvEthCompSerdesRegWrite(0x23, serdesReg);

	/* 3.8.3.7. Wait for LP_SERDES pin "PLL_READY" to be 0x1. This pin is
	 * reflected in Gunit Serdes Status (SERDESSTS) register, bit[2].
	 */
	do {
		reg = MV_REG_READ(MV_SATACOMP_SERDESSTS_REG);
	} while ((reg & (1 << 2)) == 0);

	/* 3.8.3.8. Set PIN_RX_INIT to 1b1 for at least 16 TXDCLK cycles. Set
	 * Gunit Serdes Configuration (SERDESCFG) register, field "PIN_RX_INIT"
	 * to Enable (0x1).
	 */
	MV_REG_BIT_SET(MV_SATACOMP_SERDESCFG_REG, SERDES_CFG_RX_INIT_MASK);
	mvOsSleep(10);

	/* 3.8.3.9. Wait for LP_SERDES pin "PIN_RX_INIT_DONE" to be 0x1. This
	 * pin is reflected in Gunit Serdes Status (SERDESSTS) register, bit[0].
	 */
	do {
		reg = MV_REG_READ(MV_SATACOMP_SERDESSTS_REG);
	} while ((reg & 0x1) == 0);

	/* 3.8.3.10. Set PIN_RX_INIT back to 0x0. Set Gunit Serdes Configuration
	 * (SERDESCFG) register; field "PIN_RX_INIT" to Enable (0x0).
	 */
	MV_REG_BIT_RESET(MV_SATACOMP_SERDESCFG_REG, SERDES_CFG_RX_INIT_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSwToQsgmiiPhyConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch ports 1-3 to QSGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwToQsgmiiPhyConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;
	MV_U32 i;

	if (!(ethCompCfg & ESC_OPT_QSGMII))
		return MV_OK;

	/* 3.8. Switch P1-4 to LP_SERDES_PHY, using QSGMII (10Mbps/ 100Mbps/ 1000Mbps) */
	/* 3.8.2. Ethernet-Complex configuration :
	 * 3.8.2.1. Configure Switch 125Mhz clock source: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwFi125ClkSrc to QSGMII (0x0).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	reg &= ~ETHCC_SW_FI_125_CLK_MASK;

	/* 3.8.2.2. Connect Switch ports 1,2 & 3 to PCS-0,1 & 2: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwitchPort123Source to PCS (0x0)
	 */
	reg &= ~ETHCC_SW_PORT_123_SRC_MASK;

	/* 3.8.2.3. Connect Switch ports 0 to PCS-3: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwitchPort123Source to PCS (0x0).
	 */
	reg &= ~ETHCC_SW_PORT_0_SRC_MASK;
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	/* Initialize Serdes. */
	mvEthCompSerdesConfig(ethCompCfg);

	/* 3.8.4. Reset de-assertion:
	 * 3.8.4.1. De-assert QSGMII reset: set Regunit QSGMII Control 1
	 * register, field QsgmiiRstn to 0x1.
	 */
	MV_REG_BIT_SET(MV_ETHCOMP_QSGMII_CONTROL_REG(1), QSGCTRL_RESETN_MASK);

	/* 3.8.4.2. De-assert all PCSs reset: set registers PCS_0_Control,
	 * PCS_1_Control PCS_2_Control and PCS_3_Control, fields PCS<i>_Port_Reset to 0x0.
	 */
	for (i = 0; i < 4; i++)
		MV_REG_BIT_RESET(MV_ETHCOMP_PCS_CTRL_REG(i), PCSCTRL_PORT_RESET_MASK);

	/* 3.8.5. PCS configurations (for PCSs 01,2 & 3):
	 * 3.8.5.1. Enable PCS ports: set Regunit PCS<i>Control register,
	 * fields Pcs<i>PortActive and Pcs<i>En to active (0x1)
	 */
	for (i = 0; i < 4; i++) {
		reg = MV_REG_READ(MV_ETHCOMP_PCS_CTRL_REG(i));
		reg |= ((0x1 << PCSCTRL_PORT_ACTIVE_OFFSET) |
			(0x1 << PCSCTRL_PORT_ANEG_AN_OFFSET) |
			(0x1 << PCSCTRL_PORT_RESTART_AN_OFFSET) |
			(0x1 << PCSCTRL_PORT_ADV_PAUSE_OFFSET) |
			(0x1 << PCSCTRL_PORT_ADV_ASM_PAUSE_OFFSET) |
			(0x1 << PCSCTRL_PORT_ANEG_DUPLEX_AN_OFFSET) |
			(0x1 << PCSCTRL_PORT_FC_AN_OFFSET) | (0x1 << PCSCTRL_PORT_SPEED_AN_OFFSET));
		MV_REG_WRITE(MV_ETHCOMP_PCS_CTRL_REG(i), reg);
	}

	/* Enable PCS */
	for (i = 0; i < 4; i++)
		MV_REG_BIT_SET(MV_ETHCOMP_PCS_CTRL_REG(i), PCSCTRL_PORT_EN_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompMac0ToSgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for MAC0 to SGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompMac0ToSgmiiConfig(MV_U32 ethCompCfg)
{
	if (!(ethCompCfg & ESC_OPT_SGMII))
		return MV_OK;
	MV_REG_BIT_SET(MV_MAC_SERIAL_CTRL0_REG, BIT23 | BIT22 | BIT21 | BIT13 | BIT4 | BIT3 | BIT2);
	MV_REG_WRITE(MV_MAC_SERIAL_CTRL1_REG,
		     (MV_REG_READ(MV_MAC_SERIAL_CTRL1_REG) & ~(BIT6 | BIT3)) | (BIT12 | BIT7 | BIT2));
	MV_REG_BIT_SET(MV_MAC_SERIAL_CTRL0_REG, BIT1 | BIT10);

/* 	MV_REG_BIT_SET(MV_MAC_SERIAL_CTRL1_REG, BIT12 | BIT7 | BIT2); */

	/* 3.8.3. LP_SERDES_PHY initialization:
	 * 3.8.3.1. Set LP_SERDES to reset: set Regunit Software Reset Control
	 * register to Reset (0x1).
	 */
	MV_REG_BIT_SET(SOFT_RESET_CTRL_REG, SRC_LPSRDSSWRSTN_MASK);

	/* 3.8.3.2. De-assert LP_SERDES reset: set Regunit Software Reset
	 * Control register to 0x0.
	 */
	MV_REG_BIT_RESET(SOFT_RESET_CTRL_REG, SRC_LPSRDSSWRSTN_MASK);

	/* 3.9. GbE-MAC-0 to LP_SERDES_PHY, using SSGMII
	 * 3.9.1. Ethernet-Complex configuration:
	 */
	/* Initialize Serdes. */
	mvEthCompSerdesConfig(ethCompCfg);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSwP1ToSgmiiConfig
*
* DESCRIPTION:
*	Configure ethernet complex for Switch port1 to SGMII output.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthCompSwP1ToSgmiiConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;

	if (!(ethCompCfg & ESC_OPT_SGMII))
		return MV_OK;

	/* 3.10. Switch P1 to LP_SERDES_PHY, using TBI
	 * 3.10.2. Ethernet-Complex configuration:
	 * 3.10.2.1. Configure Switch 125Mhz clock source: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwFi125ClkSrc to LpSERDES (0x2).
	 */
	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));
	reg &= ~ETHCC_SW_FI_125_CLK_MASK;

	/* 3.10.2.2. Connect Switch ports 1 to PCS-0: set Regunit
	 * Ethernet_Complex_Control_0 register, field SwitchPort123Source to PCS (0x0)
	 */
	reg &= ~ETHCC_SW_PORT_123_SRC_MASK;
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	/* Initialize Serdes. */
	mvEthCompSerdesConfig(ethCompCfg);

	/* 3.10.4. Reset de-assertion:
	 * 3.10.4.1. De-assert all PCSs reset: set registers PCS_0_Control,
	 * PCS_1_Control PCS_2_Control and PCS_3_Control, fields PCS<i>_Port_Reset to 0x0.
	 */
	MV_REG_BIT_RESET(MV_ETHCOMP_PCS_CTRL_REG(0), PCSCTRL_PORT_RESET_MASK);

	/* 3.10.5. PCS-0 configurations:
	 * OPEN: This section was never tried on our test-plan. We should ramp
	 * up this test!!! The following sections are written based on section. (QSGMII)
	 * 3.10.5.1. Enable PCS ports: set Regunit PCS0Control register, fields
	 * Pcs0PortActive and Pcs0En to active (0x1)
	 */
	MV_REG_BIT_SET(MV_ETHCOMP_PCS_CTRL_REG(0), PCSCTRL_PORT_ACTIVE_MASK);
	MV_REG_BIT_SET(MV_ETHCOMP_PCS_CTRL_REG(0), PCSCTRL_PORT_EN_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSerdesConfig
*
* DESCRIPTION:
*	Initialize serdes according to require PHY mode.
*
* INPUT:
*	ethCompCfg - Ethernet complex configuration bitmap.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
static MV_STATUS mvEthCompSerdesConfig(MV_U32 ethCompCfg)
{
	MV_U32 reg;
	MV_U32 tmp;
	MV_U16 serdesReg;

	reg = MV_REG_READ(MV_ETHCOMP_CTRL_REG(0));

	/* 3.8.2.4. Connect LP_SERDES_PHY to required source: set Regunit
	 * Ethernet_Complex_Control_0 register, field "LpphyMode" to "lpphyMode".
	 */
	if (ethCompCfg & ESC_OPT_SGMII)
		tmp = 0x0;
	else if (ethCompCfg & ESC_OPT_QSGMII)
		tmp = 0x2;
	else if (ethCompCfg & ESC_OPT_SATA)
		tmp = 0x1;
	else
		tmp = 0x3;

	reg &= ~ETHCC_LPPHYMODE_MASK;
	reg |= (tmp << ETHCC_LPPHYMODE_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_CTRL_REG(0), reg);

	/* 3.8.2.5. Change signal detect indication polarity: set Regunit
	 * QSGMII Control 1 register, field "QsgmiiInvSigdet" to 0x1.
	 */
	if (ethCompCfg & ESC_OPT_QSGMII) {
		reg = MV_REG_READ(MV_ETHCOMP_QSGMII_CONTROL_REG(1));
		reg &= ~QSGCTRL_INV_SIG_DET_MASK;
		reg |= (0x1 << QSGCTRL_INV_SIG_DET_OFFSET);
		MV_REG_WRITE(MV_ETHCOMP_QSGMII_CONTROL_REG(1), reg);
	}

	/* 3.8.3. LP_SERDES_PHY initialization:
	 * 3.8.3.1. Set LP_SERDES to reset: set Regunit Software Reset Control
	 * register to Reset (0x1).
	 */
	MV_REG_BIT_SET(SOFT_RESET_CTRL_REG, SRC_LPSRDSSWRSTN_MASK);

	/* 3.8.3.2. De-assert LP_SERDES reset: set Regunit Software Reset
	 * Control register to 0x0.
	 */
	MV_REG_BIT_RESET(SOFT_RESET_CTRL_REG, SRC_LPSRDSSWRSTN_MASK);

	mvEthCompSetSerdesDefaults();
	if (ethCompCfg & ESC_OPT_QSGMII) {
		mvEthCompSerdesRegWrite(0x25, 0x0);
		mvEthCompSerdesRegWrite(0x24, mvEthCompSerdesRegRead(0x24) | 0x8000);
	}

	/* 3.8.3.3. Power up LP_SERDES: set Gunit Serdes Configuration
	 * (SERDESCFG) register, fields "PU_TX", "PU_RX, "PU_PLL" (bits 1,2,3)
	 * to Enable (0x1)
	 */
	reg = MV_REG_READ(MV_ETHCOMP_SERDESCFG_REG);
	reg &= ~SERDES_CFG_PU_TX_MASK;
	reg &= ~SERDES_CFG_PU_RX_MASK;
	reg &= ~SERDES_CFG_PU_PLL_MASK;
	MV_REG_WRITE(MV_ETHCOMP_SERDESCFG_REG, reg);

	/* 3.8.3.4. Configuring the LP_SERDES to Reference clock 25M, and
	 * PHY_MODE to SERDES APB3 Reg 0x1. Access to LP_SERDES registers is
	 * done by accessing Gunit0, base address
	 * (0xE00 + (Serdes Reg Address << 2)) 0xE00 + 0x1<<2 = 0x4 + 0xE00 = 0xE04.
	 * Configure to -0xF88.
	 */
	mvEthCompSerdesRegWrite(0x1, 0xF880);	/* Was 0xF881 */

	/* 3.8.3.5. Configyre LP_SERED data rate, by setting Gunit Serdes
	 * Configuration (SERDESCFG) register, fields "PIN_PHY_GEN_TX" and
	 * "PIN_PHY_GEN_RX" to 5Gbps (0xA).
	 */
	tmp = 0x4;
	if (ethCompCfg & ESC_OPT_QSGMII)
		tmp = 0xA;
	if (ethCompCfg & ESC_OPT_SATA)
		tmp = 0x1;

	reg = MV_REG_READ(MV_ETHCOMP_SERDESCFG_REG);
	reg &= ~SERDES_CFG_PHY_GEN_RX_MASK;
	reg |= (tmp << SERDES_CFG_PHY_GEN_RX_OFFSET);
	reg &= ~SERDES_CFG_PHY_GEN_TX_MASK;
	reg |= (tmp << SERDES_CFG_PHY_GEN_TX_OFFSET);
	MV_REG_WRITE(MV_ETHCOMP_SERDESCFG_REG, reg);

	/* Set PHY Gen Tx & Rx to 0xA, Same as in reg MV_ETHCOMP_SERDESCFG_REG
	 ** Bit[9] - Dig_test_bus_en.
	 */
	if (ethCompCfg & ESC_OPT_QSGMII)
		mvEthCompSerdesRegWrite(0x26, 0x3AA);

	if (ethCompCfg & ESC_OPT_SGMII)
		mvEthCompSerdesRegWrite(0x26, 0x144);

	if (ethCompCfg & ESC_OPT_SATA)
		mvEthCompSerdesRegWrite(0x26, 0x111);

	/* 3.8.3.6. Configuring the LP_SERDES to 10 bit interface, by setting
	 * its internal register 0x23[11:10] to 0x1. Access to LP_SERDES
	 * registers is done by accessing Gunit0, base address
	 * (0xE00 + (Serdes Reg Address << 2)) = 0xE00 + 0x23<<2 =  0xE00 + 0x8c = 0xE8c.
	 */
	serdesReg = mvEthCompSerdesRegRead(0x23);
	serdesReg &= ~(0x3 << 10);

	if ((ethCompCfg & ESC_OPT_SATA) || (ethCompCfg & ESC_OPT_QSGMII))
		serdesReg |= (0x1 << 10);

/* 	serdesReg |= (0x1 << 10); */
/* 	serdesReg |= (0x1 << 12); */
/* 	serdesReg |= (0x1 << 13); // Analog loopback. */
	mvEthCompSerdesRegWrite(0x23, serdesReg);

#if 0
	serdesReg = mvEthCompSerdesRegRead(0x51);
	serdesReg |= (0x1 << 9);
	mvEthCompSerdesRegWrite(0x51, serdesReg);
#endif

	/* 3.8.3.7. Wait for LP_SERDES pin "PLL_READY" to be 0x1. This pin is
	 * reflected in Gunit Serdes Status (SERDESSTS) register, bit[2].
	 */
	do {
		reg = MV_REG_READ(MV_ETHCOMP_SERDESSTS_REG);
	} while ((reg & (1 << 2)) == 0);

	/* 3.8.3.8. Set PIN_RX_INIT to 1b1 for at least 16 TXDCLK cycles. Set
	 * Gunit Serdes Configuration (SERDESCFG) register, field "PIN_RX_INIT"
	 * to Enable (0x1).
	 */
	MV_REG_BIT_SET(MV_ETHCOMP_SERDESCFG_REG, SERDES_CFG_RX_INIT_MASK);
	mvOsSleep(10);

	/* 3.8.3.9. Wait for LP_SERDES pin "PIN_RX_INIT_DONE" to be 0x1. This
	 * pin is reflected in Gunit Serdes Status (SERDESSTS) register, bit[0].
	 */
	do {
		reg = MV_REG_READ(MV_ETHCOMP_SERDESSTS_REG);
	} while ((reg & 0x1) == 0);

	/* 3.8.3.10. Set PIN_RX_INIT back to 0x0. Set Gunit Serdes Configuration
	 * (SERDESCFG) register; field "PIN_RX_INIT" to Enable (0x0).
	 */
	MV_REG_BIT_RESET(MV_ETHCOMP_SERDESCFG_REG, SERDES_CFG_RX_INIT_MASK);

	return MV_OK;
}

/******************************************************************************
* mvEthCompSerdesRegWrite
*
* DESCRIPTION:
*	Write to a serdes register.
*	The Serdes register address is calculated as follows:
*		GPORT-0 Base + 0xE00 + (regAddr << 2)
*
* INPUT:
*	regAddr - The serdes register to write to.
*	value - Value to be written.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
*******************************************************************************/
static void mvEthCompSerdesRegWrite(MV_U32 regAddr, MV_U16 value)
{
	MV_U32 ethCompCfg = mvBoardEthComplexConfigGet();

	if (ethCompCfg & ESC_OPT_SATA) {
		MV_REG_WRITE(0x82800 + (regAddr << 2), value);
		/* MV_REG_WRITE(0x72E00 + (regAddr << 2), value); */
	} else {
		MV_REG_WRITE(0x72E00 + (regAddr << 2), value);
	}
	return;
}

/******************************************************************************
* mvEthCompSerdesRegRead
*
* DESCRIPTION:
*	Read a serdes register.
*	The Serdes register address is calculated as follows:
*		GPORT-0 Base + 0xE00 + (regAddr << 2)
*
* INPUT:
*	regAddr - The serdes register to read.
*
* OUTPUT:
*	None.
*
* RETURN:
*	The register's value.
*******************************************************************************/
static MV_U16 mvEthCompSerdesRegRead(MV_U32 regAddr)
{
	MV_U16 data;
	MV_U32 data32;
	MV_U32 ethCompCfg = mvBoardEthComplexConfigGet();

	if (ethCompCfg & ESC_OPT_SATA) {
		data32 = MV_REG_READ(0x82800 + (regAddr << 2));
		data = (MV_U16) (data32 & 0xFFFF);
/*		data32 = MV_REG_READ(0x72E00 + (regAddr << 2));
		data = (MV_U16)(data32 & 0xFFFF);*/
	} else {
		data32 = MV_REG_READ(0x72E00 + (regAddr << 2));
		data = (MV_U16) (data32 & 0xFFFF);
	}
	return data;
}

/******************************************************************************
* mvEthernetComplexShutdownIf
*
* DESCRIPTION:
*	Shutdown ethernet complex interfaces.
*
* INPUT:
*	integSwitch	- MV_TRUE to shutdown the integrated switch.
*	gePhy		- MV_TRUE to shutdown the GE-PHY
*	fePhy		- MV_TRUE to shutdown the 3xFE PHY.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthernetComplexShutdownIf(MV_BOOL integSwitch, MV_BOOL gePhy, MV_BOOL fePhy)
{
	if (gePhy == MV_TRUE) {
		MV_REG_BIT_RESET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_PHY_PWR_DOWN_MASK);
		MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_EXT_PWR_DOWN_SRC_MASK);
		MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_PHY_PWR_DOWN_MASK);
	}

	if (fePhy == MV_TRUE) {
		MV_REG_BIT_SET(MV_ETHCOMP_FE_PHY_CTRL_REG, ETHCC_FE_PHY_RESET_MASK);
		MV_REG_BIT_SET(MV_ETHCOMP_FE_PHY_CTRL_REG, ETHCC_FE_PHY_EXT_PWR_DOWM_MASK);
	}

	return MV_OK;
}

/******************************************************************************
* mvEthernetComplexInit
*
* DESCRIPTION:
*	Initialize the ethernet complex according to the boardEnv setup.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*******************************************************************************/
MV_STATUS mvEthernetComplexInit(void)
{
	MV_U32 ethCompCfg = mvBoardEthComplexConfigGet();
	MV_U32 reg, port;

	if (gEthComplexSkipInit == MV_TRUE)
		return MV_OK;

	/* Disable Polling mode. */
	MV_REG_BIT_RESET(ETH_UNIT_CONTROL_REG(0), BIT1);
	MV_REG_BIT_RESET(ETH_UNIT_CONTROL_REG(1), BIT1);

	/* Set PHY address for MAC ports to 8 & 9 accordingly. */
	/* The MAC Phy addresses need to be set before we enable the internal
	 ** PHY / switch.
	 */
	if (ethCompCfg & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIB_MAC0 | ESC_OPT_MAC0_2_SW_P4)) {
		port = 0;
		reg = MV_REG_READ(ETH_PHY_ADDR_REG(port));
		reg &= ~ETH_PHY_ADDR_MASK;
		reg |= SWITCH_PHY_ADDR(port);
		MV_REG_WRITE(ETH_PHY_ADDR_REG(port), reg);
	}

	MV_REG_WRITE(ETH_PHY_ADDR_REG(1), 0x9);
	if (ethCompCfg & (ESC_OPT_RGMIIA_MAC1 | ESC_OPT_MAC1_2_SW_P5 | ESC_OPT_GEPHY_MAC1)) {
		port = 1;
		reg = MV_REG_READ(ETH_PHY_ADDR_REG(port));
		reg &= ~ETH_PHY_ADDR_MASK;
		reg |= SWITCH_PHY_ADDR(port);
		MV_REG_WRITE(ETH_PHY_ADDR_REG(port), reg);
	}

	/*  First, initialize the switch. */
	mvEthCompMac2SwitchConfig(ethCompCfg);

	/*  MAC0 / 1 to RGMII. */
	mvEthCompMac2RgmiiConfig(ethCompCfg);

	/*  MAC1 to GE PHY. */
	mvEthCompMac1ToGePhyConfig(ethCompCfg, mvBoardPhyAddrGet(1));

	/*  Switch Port0/5 to GE PHY. */
	mvEthCompSwP05ToGePhyConfig(ethCompCfg);

	/*  Switch Port5 to RGMII. */
	mvEthCompSwP56ToRgmiiConfig(ethCompCfg);

	/*  Switch to 3xFE phy. */
	mvEthCompSwTo3FePhyConfig(ethCompCfg);

	/*  Sata. */
	mvEthCompSataConfig(ethCompCfg);

	/*  Switch to QSGMII. */
	mvEthCompSwToQsgmiiPhyConfig(ethCompCfg);

	/*  MAC0 to SGMII. */
	mvEthCompMac0ToSgmiiConfig(ethCompCfg);
#if 0
	/*  Switch port 1 to SGMII. */
	mvEthCompSwP1ToSgmiiConfig(ethCompCfg);
#endif
	/*  Reset the switch after all configurations are done. */
	mvEthCompSwitchReset(ethCompCfg);

#if 0
	/*  Re-reset the PHY in case it's connected to MAC1. */
	if (ethCompCfg & ESC_OPT_GEPHY_MAC1) {
		MV_REG_BIT_RESET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_RESET_MASK);
		MV_REG_BIT_SET(MV_ETHCOMP_GE_PHY_CTRL_REG, PHYCTRL_RESET_MASK);
	}
#endif
	if (!(ethCompCfg & ESC_OPT_MAC0_2_SW_P4))
		MV_REG_BIT_SET(ETH_UNIT_CONTROL_REG(0), BIT1);
	if (!(ethCompCfg & ESC_OPT_MAC1_2_SW_P5))
		MV_REG_BIT_SET(ETH_UNIT_CONTROL_REG(1), BIT1);

	return MV_OK;
}
