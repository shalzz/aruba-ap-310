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

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    * 	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    * 	Neither the name of Marvell nor the names of its contributors may be
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

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "cpu/mvCpu.h"
#include "cntmr/mvCntmr.h"
#include "gpp/mvGpp.h"
#include "twsi/mvTwsi.h"
#include "pex/mvPex.h"
#include "device/mvDevice.h"
#include "gpp/mvGppRegs.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"

#if defined(CONFIG_MV_ETH_LEGACY)
#include "eth/gbe/mvEthGbe.h"
#else
#include "neta/gbe/mvNeta.h"
#endif /* CONFIG_MV_ETH_LEGACY or CONFIG_MV_ETH_NETA */

/* defines  */
/* #define MV_DEBUG */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

extern MV_CPU_ARM_CLK _cpuARMDDRCLK[];


extern MV_BOARD_INFO *boardInfoTbl[];
#define BOARD_INFO(boardId)	boardInfoTbl[boardId - BOARD_ID_BASE]

/* Locals */
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);

MV_U32 tClkRate = -1;
extern MV_U8 mvDbDisableModuleDetection;

#define MV_BRD_MODULE_CFG_BUILD(ethCfg, tdmCfg)	((tdmCfg << 24) | ethCfg)
#define MV_BRD_MODULE_CFG_ETH_GET(config)	(config & 0xFFFFFF)
#define MV_BRD_MODULE_CFG_TDM_GET(config)	(MV_8)(config >> 24)

static MV_BOOL mvBoardStaticModuleConfig = MV_FALSE;
static MV_U32 mvBoardModuleConfig = 0;

/*******************************************************************************
* mvBoardEnvInit - Init board
*
* DESCRIPTION:
*		In this function the board environment take care of device bank
*		initialization.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardEnvInit(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 nandDev;
	MV_32 width;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardEnvInit:Board unknown.\n");
		return;
	}
	nandDev = boardGetDevCSNum(0, BOARD_DEV_NAND_FLASH);
	if (nandDev != 0xFFFFFFFF) {
		/* Set NAND interface access parameters */
		nandDev = BOOT_CS;
		MV_REG_WRITE(DEV_BANK_PARAM_REG(nandDev), BOARD_INFO(boardId)->nandFlashReadParams);
		MV_REG_WRITE(DEV_BANK_PARAM_REG_WR(nandDev), BOARD_INFO(boardId)->nandFlashWriteParams);
		MV_REG_WRITE(DEV_NAND_CTRL_REG, BOARD_INFO(boardId)->nandFlashControl);
	}

	width = mvBoardGetDeviceWidth(0, BOARD_DEV_NOR_FLASH);
	if (width != 0xFFFFFFFF)
		mvDevWidthSet(BOOT_CS, width);

	/* Set GPP Out value */
	MV_REG_WRITE(GPP_DATA_OUT_REG(0), BOARD_INFO(boardId)->gppOutValLow);
	MV_REG_WRITE(GPP_DATA_OUT_REG(1), BOARD_INFO(boardId)->gppOutValMid);
	MV_REG_WRITE(GPP_DATA_OUT_REG(2), BOARD_INFO(boardId)->gppOutValHigh);

	/* set GPP polarity */
	mvGppPolaritySet(0, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValLow);
	mvGppPolaritySet(1, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValMid);
	mvGppPolaritySet(2, 0xFFFFFFFF, BOARD_INFO(boardId)->gppPolarityValHigh);

	/* Set GPP Out Enable */
	mvGppTypeSet(0, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValLow);
	mvGppTypeSet(1, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValMid);
	mvGppTypeSet(2, 0xFFFFFFFF, BOARD_INFO(boardId)->gppOutEnValHigh);

	/* Nand CE */
#if 0
	if (nandDev != 0xFFFFFFFF)
		mvDevNandDevCsSet(nandDev, MV_TRUE);
#endif
}

/*******************************************************************************
* mvBoardModelGet - Get Board model
*
* DESCRIPTION:
*       This function returns 16bit describing board model.
*       Board model is constructed of one byte major and minor numbers in the
*       following manner:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       String describing board model.
*
*******************************************************************************/
MV_U16 mvBoardModelGet(MV_VOID)
{
	return (mvBoardIdGet() >> 16);
}

/*******************************************************************************
* mbBoardRevlGet - Get Board revision
*
* DESCRIPTION:
*       This function returns a 32bit describing the board revision.
*       Board revision is constructed of 4bytes. 2bytes describes major number
*       and the other 2bytes describes minor munber.
*       For example for board revision 3.4 the function will return
*       0x00030004.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       String describing board model.
*
*******************************************************************************/
MV_U16 mvBoardRevGet(MV_VOID)
{
	return (mvBoardIdGet() & 0xFFFF);
}

/*******************************************************************************
* mvBoardNameGet - Get Board name
*
* DESCRIPTION:
*       This function returns a string describing the board model and revision.
*       String is extracted from board I2C EEPROM.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain board name string. Minimum size 32 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvBoardNameGet(char *pNameBuff)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsSPrintf(pNameBuff, "Board unknown.\n");
		return MV_ERROR;
	}

	mvOsSPrintf(pNameBuff, "%s", BOARD_INFO(boardId)->boardName);

	return MV_OK;
}

/*******************************************************************************
* mvBoardSpecInitGet -
*
* DESCRIPTION:
*       This routine returns the board specific initializtion information.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       A pointer to the array holding the board specific initializations.
*
*******************************************************************************/
MV_BOARD_SPEC_INIT *mvBoardSpecInitGet(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSpecInitGet: Board unknown.\n");
		return NULL;
	}

	return BOARD_INFO(boardId)->pBoardSpecInit;
}

/*******************************************************************************
* mvBoardIsPortInSgmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in SGMII or MV_FALSE
*	For all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in SGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum)
{
	MV_U32 ethCompOpt;

	if (ethPortNum >= BOARD_ETH_PORT_NUM) {
		mvOsPrintf("Invalid portNo=%d\n", ethPortNum);
		return MV_FALSE;
	}
	ethCompOpt = mvBoardEthComplexConfigGet();
	return ((ethCompOpt & ESC_OPT_SGMII) ? MV_TRUE : MV_FALSE);
}

/*******************************************************************************
* mvBoardIsPortInRgmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in RGMII or MV_FALSE
*	for all other options.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in RGMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInRgmii(MV_U32 ethPortNum)
{
	MV_U32 ethCompOpt;

	if (ethPortNum >= BOARD_ETH_PORT_NUM) {
		mvOsPrintf("Invalid portNo=%d\n", ethPortNum);
		return MV_FALSE;
	}

	ethCompOpt = mvBoardEthComplexConfigGet();

	if (ethPortNum == 0)
		return (ethCompOpt & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIB_MAC0)) ? MV_TRUE : MV_FALSE;
	else if (ethPortNum == 1)
		return (ethCompOpt & ESC_OPT_RGMIIA_MAC1) ? MV_TRUE : MV_FALSE;
	else
		return MV_FALSE;
}

#if 0
/*******************************************************************************
* mvBoardIsPortInGmii -
*
* DESCRIPTION:
*       This routine returns MV_TRUE for port number works in GMII or MV_FALSE
*	For all other options.
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE - port in GMII.
*       MV_FALSE - other.
*
*******************************************************************************/
MV_BOOL mvBoardIsPortInGmii(MV_VOID)
{
	MV_U32 devClassId, devClass = 0;
	if (mvBoardMppGroupTypeGet(devClass) == MV_BOARD_AUTO) {
		/* Get MPP module ID */
		devClassId = mvBoarModuleTypeGet(devClass);
		if (MV_BOARD_MODULE_GMII_ID == devClassId)
			return MV_TRUE;
	} else if (mvBoardMppGroupTypeGet(devClass) == MV_BOARD_GMII)
		return MV_TRUE;

	return MV_FALSE;
}
#endif /* 0 */

/*******************************************************************************
* mvBoardPhyAddrGet - Get the phy address
*
* DESCRIPTION:
*       This routine returns the Phy address of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing Phy address, -1 if the port number is wrong.
*
*******************************************************************************/
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardPhyAddrGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].boardEthSmiAddr;
}

/*******************************************************************************
* mvBoardMacSpeedGet - Get the Mac speed
*
* DESCRIPTION:
*       This routine returns the Mac speed if pre define of a given ethernet port.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BOARD_MAC_SPEED, -1 if the port number is wrong.
*
*******************************************************************************/
MV_BOARD_MAC_SPEED mvBoardMacSpeedGet(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardMacSpeedGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMacInfo[ethPortNum].boardMacSpeed;
}

/*******************************************************************************
* mvBoardIsInternalSwitchConnected
*
* DESCRIPTION:
*       This routine returns port's connection status
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 - if ethPortNum is connected to switch, 0 otherwise
*
*******************************************************************************/
MV_32 mvBoardIsInternalSwitchConnected(MV_U32 ethPortNum)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 ethCompOpt;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardIsInternalSwitchConnected: Board unknown.\n");
		return MV_ERROR;
	}

	if (ethPortNum >= BOARD_INFO(boardId)->numBoardMacInfo) {
		mvOsPrintf("mvBoardIsSwitchConnected: Illegal port number(%u)\n", ethPortNum);
		return MV_ERROR;
	}

	/* Check if internal switch is connected */
	ethCompOpt = mvBoardEthComplexConfigGet();
	if ((ethPortNum == 0) && (ethCompOpt & ESC_OPT_MAC0_2_SW_P4))
		return 1;
	else if ((ethPortNum == 1) && (ethCompOpt & ESC_OPT_MAC1_2_SW_P5))
		return 1;
	else
		return 0;
}

#if 0
/*******************************************************************************
* mvBoardSwitchNumPortsGet
*
* DESCRIPTION:
*       This routine returns the number of switch ports.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Number of switch ports, 0 - if no switch is connected.
*
*******************************************************************************/
MV_32 mvBoardSwitchNumPortsGet(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 ethCompOpt;
	MV_U32 numPorts = 0;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSwitchNumPortsGet: Board unknown.\n");
		return MV_ERROR;
	}

	/* Check if internal switch is connected */
	ethCompOpt = mvBoardEthComplexConfigGet();
	if (ethCompOpt & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)) {
		if (ethCompOpt & ESC_OPT_FE3PHY)
			numPorts += 3;
		if (ethCompOpt & ESC_OPT_QSGMII)
			numPorts += 4;
		if (ethCompOpt & (ESC_OPT_GEPHY_SW_P0 | ESC_OPT_GEPHY_SW_P5))
			numPorts += 1;
		if (ethCompOpt & (ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6))
			numPorts += 1;
	}

	return numPorts;
}
#endif

/*******************************************************************************
* mvBoardIsQsgmiiModuleConnected
*
* DESCRIPTION:
*       This routine returns whether the QSGMII module is connected or not.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if QSGMII module is connected, MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvBoardIsQsgmiiModuleConnected(MV_VOID)
{
	MV_U32 ethCompOpt;

	/* Check if internal switch is connected */
	ethCompOpt = mvBoardEthComplexConfigGet();
	if (ethCompOpt & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)) {
		if (ethCompOpt & ESC_OPT_QSGMII)
			return MV_TRUE;
	}
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardGePhySwitchPortGet
*
* DESCRIPTION:
*       This routine returns whether the internal GE PHY is connected to
*	Switch Port 0, Switch port 5 or not connected to any Switch port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       0 if the internal GE PHY is connected to Switch Port 0,
*	5 if the internal GE PHY is connected to Switch Port 5,
*	-1 otherwise.
*
*******************************************************************************/
MV_32 mvBoardGePhySwitchPortGet(MV_VOID)
{
	MV_U32 ethCompOpt;

	/* Check if internal switch is connected */
	ethCompOpt = mvBoardEthComplexConfigGet();
	if (ethCompOpt & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)) {
		if (ethCompOpt & ESC_OPT_GEPHY_SW_P0)
			return 0;
		if (ethCompOpt & ESC_OPT_GEPHY_SW_P5)
			return 5;
	}
	return -1;
}

/*******************************************************************************
* mvBoardRgmiiASwitchPortGet
*
* DESCRIPTION:
*       This routine returns whether RGMII-A is connected to
*	Switch Port 5, Switch port 6 or not connected to any Switch port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       5 if the internal GE PHY is connected to Switch Port 5,
*	6 if the internal GE PHY is connected to Switch Port 6,
*	-1 otherwise.
*
*******************************************************************************/
MV_32 mvBoardRgmiiASwitchPortGet(MV_VOID)
{
	MV_U32 ethCompOpt;

	/* Check if internal switch is connected */
	ethCompOpt = mvBoardEthComplexConfigGet();
	if (ethCompOpt & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)) {
		if (ethCompOpt & ESC_OPT_RGMIIA_SW_P5)
			return 5;
		if (ethCompOpt & ESC_OPT_RGMIIA_SW_P6)
			return 6;
	}
	return -1;
}

/*******************************************************************************
* mvBoardSwitchInfoUpdate
*
* DESCRIPTION:
*	Update switch information structure according to modules detection
*	result.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK - on success,
*	MV_ERROR - On failure.
*
*******************************************************************************/
static MV_STATUS mvBoardSwitchInfoUpdate(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_U32 ethCompOpt;
	MV_U32 i;
	MV_U32 swIdx = 0;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSwitchInfoUpdate: Board unknown.\n");
		return MV_ERROR;
	}

	if (boardId != DB_88F6535_BP_ID && boardId != DB_ARRAN_ID
            && boardId != DB_TAMDHU_ID)
		return MV_OK;

	if ((!mvBoardIsInternalSwitchConnected(0)) && (!mvBoardIsInternalSwitchConnected(1))) {
		BOARD_INFO(boardId)->switchInfoNum = 0;
		return MV_OK;
	}

	ethCompOpt = mvBoardEthComplexConfigGet();

	/* Update the cpuPort & connectedPort fields */
	BOARD_INFO(boardId)->pSwitchInfo[swIdx].cpuPort = -1;
	for (i = 0; i < MV_ETH_MAX_PORTS; i++)
		BOARD_INFO(boardId)->pSwitchInfo[swIdx].connectedPort[i] = -1;

	/* Check if port 4 connected to MAC0 */
	if (ethCompOpt & ESC_OPT_MAC0_2_SW_P4) {
		BOARD_INFO(boardId)->pSwitchInfo[swIdx].cpuPort = 4;
		BOARD_INFO(boardId)->pSwitchInfo[swIdx].connectedPort[0] = 4;
	}

	/* Check if port 5 connected to MAC1 */
	if (ethCompOpt & ESC_OPT_MAC1_2_SW_P5) {
		BOARD_INFO(boardId)->pSwitchInfo[swIdx].cpuPort = 5;
		BOARD_INFO(boardId)->pSwitchInfo[swIdx].connectedPort[1] = 5;
	}

	i = 0;
	if (ethCompOpt & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)) {

		if (ethCompOpt & ESC_OPT_QSGMII) {
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[0] = 0;
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[1] = 1;
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[2] = 2;
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[3] = 3;
			i = 4;
		} else if (ethCompOpt & ESC_OPT_FE3PHY) {
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[0] = 1;
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[1] = 2;
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[2] = 3;
			i = 3;
		}

		if (ethCompOpt & ESC_OPT_GEPHY_SW_P0)
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[i++] = 0;
		else if (ethCompOpt & ESC_OPT_GEPHY_SW_P5)
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[i++] = 5;

		if (ethCompOpt & ESC_OPT_RGMIIA_SW_P6)
			BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[i++] = 6;
	}


	/* Set all unused switch ports to -1 */
	for (; i < BOARD_ETH_SWITCH_PORT_NUM; i++)
		BOARD_INFO(boardId)->pSwitchInfo[swIdx].switchPort[i] = -1;

	/* Set number of switch entries */
	BOARD_INFO(boardId)->switchInfoNum = 1;

	return MV_OK;
}

/*******************************************************************************
* mvBoardSwitchIrqGet - Get the IRQ number for the link status indication
*
* DESCRIPTION:
*       This routine returns the IRQ number for the link status indication.
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*	the number of the IRQ for the link status indication, -1 if the port
*	number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSwitchIrqGet(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardLinkStatusIrqGet: Board unknown.\n");
		return -1;
	}
	if (BOARD_INFO(boardId)->switchInfoNum == 0)
		return -1;

	return BOARD_INFO(boardId)->pSwitchInfo[0].switchIrq;
}

/*******************************************************************************
* mvBoardSwitchConnectedPortGet -
*
* DESCRIPTION:
*       This routine returns the switch port connected to the ethPort
*
* INPUT:
*       ethPortNum - Ethernet port number.
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_32 mvBoardSwitchConnectedPortGet(MV_U32 ethPort)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSwitchConnectedPortGet: Board unknown.\n");
		return -1;
	}
	if (BOARD_INFO(boardId)->switchInfoNum == 0)
		return -1;

	return BOARD_INFO(boardId)->pSwitchInfo[0].connectedPort[ethPort];
}

/*******************************************************************************
* mvBoardSwitchPortGet - Get the mapping between the board connector and the
*                        Ethernet Switch port
*
* DESCRIPTION:
*       This routine returns the matching Switch port.
*
* INPUT:
*	    boardPortNum - logical number of the connector on the board
*
* OUTPUT:
*       None.
*
* RETURN:
*       the matching Switch port, -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSwitchPortGet(MV_U32 switchIdx, MV_U32 boardPortNum)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSwitchPortGet: Board unknown.\n");
		return -1;
	}
	if (boardPortNum >= BOARD_ETH_SWITCH_PORT_NUM) {
		mvOsPrintf("mvBoardSwitchPortGet: Illegal board port number.\n");
		return -1;
	}
	if ((BOARD_INFO(boardId)->switchInfoNum == 0) || (switchIdx >= BOARD_INFO(boardId)->switchInfoNum))
		return -1;

	return BOARD_INFO(boardId)->pSwitchInfo[switchIdx].switchPort[boardPortNum];
}

/*******************************************************************************
* mvBoardSwitchPortMap
*
* DESCRIPTION:
*	Map front panel connector number to switch port number.
*
* INPUT:
*	switchIdx - The switch index.
*	switchPortNum - The switch port number to get the mapping for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The switch port mapping.
*	OR -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSwitchPortMap(MV_U32 switchIdx, MV_U32 switchPortNum)
{
	int i;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSwitchPortMap: Board unknown.\n");
		return -1;
	}
	if ((BOARD_INFO(boardId)->switchInfoNum == 0) || (switchIdx >= BOARD_INFO(boardId)->switchInfoNum))
		return -1;

	for (i = 0; i < BOARD_ETH_SWITCH_PORT_NUM; i++) {
		if (BOARD_INFO(boardId)->pSwitchInfo[switchIdx].switchPort[i] == switchPortNum)
			return i;
	}
	return -1;
}

/*******************************************************************************
* mvBoardSwitchCpuPortGet - Get the the Ethernet Switch CPU port
*
* DESCRIPTION:
*       This routine returns the Switch CPU port.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       the Switch CPU port, -1 if the switch is not connected.
*
*******************************************************************************/
MV_32 mvBoardSwitchCpuPortGet(MV_U32 switchIdx)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSwitchCpuPortGet: Board unknown.\n");
		return -1;
	}
	if ((BOARD_INFO(boardId)->switchInfoNum == 0) || (switchIdx >= BOARD_INFO(boardId)->switchInfoNum))
		return -1;

	return BOARD_INFO(boardId)->pSwitchInfo[switchIdx].cpuPort;
}

/*******************************************************************************
* mvBoardSmiScanModeGet - Get Switch SMI scan mode
*
* DESCRIPTION:
*       This routine returns Switch SMI scan mode.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       1 for SMI_MANUAL_MODE, -1 if the port number is wrong or if not relevant.
*
*******************************************************************************/
MV_32 mvBoardSmiScanModeGet(MV_U32 switchIdx)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardSmiScanModeGet: Board unknown.\n");
		return -1;
	}
	if ((BOARD_INFO(boardId)->switchInfoNum == 0) || (switchIdx >= BOARD_INFO(boardId)->switchInfoNum))
		return -1;

	return BOARD_INFO(boardId)->pSwitchInfo[switchIdx].smiScanMode;
}

/*******************************************************************************
* mvBoardQuadPhyAddrGet - Get QUAD phy SMI address.
*
* DESCRIPTION:
*       This routine returns the external QUAD phy address.
*
* INPUT:
*       switchIdx - index of the switch. Only 0 is supported.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The QUAD phy start address or -1 if error.
*
*******************************************************************************/
MV_32 mvBoardQuadPhyAddrGet(MV_U32 switchIdx)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardQuadPhyAddrGet: Board unknown.\n");
		return -1;
	}

	if ((BOARD_INFO(boardId)->switchInfoNum == 0) || (switchIdx >= BOARD_INFO(boardId)->switchInfoNum))
		return -1;

	return BOARD_INFO(boardId)->pSwitchInfo[switchIdx].quadPhyAddr;

}

/*******************************************************************************
* mvBoardPhySpecInitGet -
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: Return MV_TRUE and parameters in case board need spesific phy init,
*        otherwise return MV_FALSE.
*
*
*******************************************************************************/
MV_BOOL mvBoardPhySpecInitGet(MV_U32 *regOff, MV_U32 *data)
{
	return MV_FALSE;
}

/*******************************************************************************
* mvBoardTclkGet - Get the board Tclk (Controller clock)
*
* DESCRIPTION:
*       This routine extract the controller core clock.
*       This function uses the controller counters to make identification.
*		Note: In order to avoid interference, make sure task context switch
*		and interrupts will not occure during this function operation
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardTclkGet(MV_VOID)
{
#if defined(TCLK_AUTO_DETECT)
	MV_U32 tmpTClkRate = MV_BOARD_TCLK_166MHZ;

	tmpTClkRate = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	tmpTClkRate &= MSAR_TCLCK_MASK;

	switch (tmpTClkRate) {
	case MSAR_TCLCK_166:
		return MV_BOARD_TCLK_166MHZ;
		break;
	case MSAR_TCLCK_200:
		return MV_BOARD_TCLK_200MHZ;
		break;
	default:
		return -1;
	}
#else
	return MV_BOARD_TCLK_200MHZ;
#endif
}

/*******************************************************************************
* mvBoardSysClkGet - Get the board SysClk (CPU bus clock)
*
* DESCRIPTION:
*       This routine extract the CPU bus clock.
*
* INPUT:
*       countNum - Counter number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvBoardSysClkGet(MV_VOID)
{
#ifdef SYSCLK_AUTO_DETECT
	MV_U32 i, res = -1;
	MV_U32 sar0, sar1;
	MV_U32 clockSatr;
	MV_CPU_ARM_CLK cpuDdrL2Tbl[] = MV_CPU_DDR_L2_CLCK_TBL;

	/* Read S@R registers value */
	sar0 = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	sar1 = MV_REG_READ(MPP_SAMPLE_AT_RESET(1));
	clockSatr = MSAR_CPU_DDR_L2_CLCK_EXTRACT(sar0, sar1);

	/* Search for a matching entry */
	i = 0;
	while (cpuDdrL2Tbl[i].satrValue != -1) {
		if (cpuDdrL2Tbl[i].satrValue == clockSatr) {
			res = i;
			break;
		}
		i++;
	}

	if (res == 0xFFFFFFFF)
		return 0;
	else
		return cpuDdrL2Tbl[i].ddrClk;
#else
	return MV_BOARD_DEFAULT_SYSCLK;
#endif
}

/*******************************************************************************
* mvBoardDebugLedNumGet - Get number of debug Leds
*
* DESCRIPTION:
* INPUT:
*       boardId
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_U32 mvBoardDebugLedNumGet(MV_U32 boardId)
{
	return BOARD_INFO(boardId)->activeLedsNumber;
}

/*******************************************************************************
* mvBoardDebugLeg - Set the board debug Leds
*
* DESCRIPTION: turn on/off status leds.
*
*
* INPUT:
*       hexNum - Number to be displied in hex by Leds.
*		 Note: assuming all debug leds reside within same group.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvBoardDebugLed(MV_U32 hexNum)
{
	MV_U32 val = 0, totalMask, currentBitMask = 1, i;
	MV_U8 group = 0;
	MV_U32 boardId = mvBoardIdGet();

	if (BOARD_INFO(boardId)->pLedGppPin == NULL)
		return;

	totalMask = (1 << BOARD_INFO(boardId)->activeLedsNumber) - 1;
	hexNum &= totalMask;
	totalMask = 0;
	group = (BOARD_INFO(boardId)->pLedGppPin[0] / 32);

	for (i = 0; i < BOARD_INFO(boardId)->activeLedsNumber; i++) {
		if (hexNum & currentBitMask)
			val |= (1 << (BOARD_INFO(boardId)->pLedGppPin[i] % 32));
		totalMask |= (1 << (BOARD_INFO(boardId)->pLedGppPin[i] % 32));
		currentBitMask = (currentBitMask << 1);
	}

	if (BOARD_INFO(boardId)->ledsPolarity)
		mvGppValueSet(group, totalMask, val);
	else
		mvGppValueSet(group, totalMask, ~val);
}

/*******************************************************************************
* mvBoarGpioPinGet - mvBoarGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		gppClass - MV_BOARD_GPP_CLASS enum.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS gppClass, MV_U32 index)
{
	MV_U32 boardId, i;
	MV_U32 indexFound = 0;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardRTCGpioPinGet:Board unknown.\n");
		return MV_ERROR;
	}

	for (i = 0; i < BOARD_INFO(boardId)->numBoardGppInfo; i++) {
		if (BOARD_INFO(boardId)->pBoardGppInfo[i].devClass == gppClass) {
			if (indexFound == index)
				return (MV_U32) BOARD_INFO(boardId)->pBoardGppInfo[i].gppPinNum;
			else
				indexFound++;
		}
	}
	return MV_ERROR;
}

/*******************************************************************************
* mvBoardReset - mvBoardReset
*
* DESCRIPTION:
*			Reset the board
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       None
*
*******************************************************************************/
MV_VOID mvBoardReset(MV_VOID)
{
	MV_32 resetPin;

	/* Get gpp reset pin if define */
	resetPin = mvBoardResetGpioPinGet();
	if (resetPin != MV_ERROR) {
		MV_REG_BIT_RESET(GPP_DATA_OUT_REG(0), (1 << resetPin));
		MV_REG_BIT_RESET(GPP_DATA_OUT_EN_REG(0), (1 << resetPin));
	} else {
		/* No gpp reset pin was found, try to reset ussing
		 ** system reset out */
		MV_REG_BIT_SET(CPU_RSTOUTN_MASK_REG, BIT2);
		MV_REG_BIT_SET(CPU_SYS_SOFT_RST_REG, BIT0);
	}
}

/*******************************************************************************
* mvBoardResetGpioPinGet - mvBoardResetGpioPinGet
*
* DESCRIPTION:
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardResetGpioPinGet(MV_VOID)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_RESET, 0);
}

/*******************************************************************************
* mvBoardSDIOGpioPinGet - mvBoardSDIOGpioPinGet
*
* DESCRIPTION:
*	used for hotswap detection
* INPUT:
*	type - Type of SDIO GPP to get.
*
* OUTPUT:
*	None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardSDIOGpioPinGet(MV_BOARD_GPP_CLASS type)
{
	if ((type != BOARD_GPP_SDIO_POWER) && (type != BOARD_GPP_SDIO_DETECT) && (type != BOARD_GPP_SDIO_WP))
		return MV_FAIL;

	return mvBoarGpioPinNumGet(type, 0);
}

/*******************************************************************************
* mvBoardUSBVbusGpioPinGet - return Vbus input GPP
*
* DESCRIPTION:
*
* INPUT:
*		int  devNo.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardUSBVbusGpioPinGet(MV_32 devId)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_USB_VBUS, devId);
}

/*******************************************************************************
* mvBoardUSBVbusEnGpioPinGet - return Vbus Enable output GPP
*
* DESCRIPTION:
*
* INPUT:
*		int  devNo.
*
* OUTPUT:
*		None.
*
* RETURN:
*       GPIO pin number. The function return -1 for bad parameters.
*
*******************************************************************************/
MV_32 mvBoardUSBVbusEnGpioPinGet(MV_32 devId)
{
	return mvBoarGpioPinNumGet(BOARD_GPP_USB_VBUS_EN, devId);
}

/*******************************************************************************
* mvBoardGpioIntMaskGet - Get GPIO mask for interrupt pins
*
* DESCRIPTION:
*		This function returns a 32-bit mask of GPP pins that connected to
*		interrupt generating sources on board.
*		For example if UART channel A is hardwired to GPP pin 8 and
*		UART channel B is hardwired to GPP pin 4 the fuinction will return
*		the value 0x000000110
*
* INPUT:
*		None.
*
* OUTPUT:
*		None.
*
* RETURN:
*		See description. The function return -1 if board is not identified.
*
*******************************************************************************/
MV_U32 mvBoardGpioIntMaskGet(MV_U32 gppGrp)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardGpioIntMaskGet:Board unknown.\n");
		return MV_ERROR;
	}

	switch (gppGrp) {
	case (0):
		return BOARD_INFO(boardId)->intsGppMaskLow;
		break;
	case (1):
		return BOARD_INFO(boardId)->intsGppMaskMid;
		break;
	case (2):
		return BOARD_INFO(boardId)->intsGppMaskHigh;
		break;
	default:
		return MV_ERROR;
	}
}

/*******************************************************************************
* mvBoardMppGet - Get board dependent MPP register value
*
* DESCRIPTION:
*		MPP settings are derived from board design.
*		MPP group consist of 8 MPPs. An MPP group represent MPP
*		control register.
*       This function retrieves board dependend MPP register value.
*
* INPUT:
*       mppGroupNum - MPP group number.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing MPP control register value.
*
*******************************************************************************/
MV_32 mvBoardMppGet(MV_U32 mppGroupNum)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardMppGet:Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMppConfigValue[0].mppGroup[mppGroupNum];
}

/*******************************************************************************
* mvBoardEthComplexConfigGet - Return ethernet complex board configuration.
*
* DESCRIPTION:
*	Returns the ethernet / Sata complex configuration from the board spec
*	structure.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing the ethernet complex config.
*
*******************************************************************************/
MV_U32 mvBoardEthComplexConfigGet(MV_VOID)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardEthComplexConfigGet: Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->pBoardMppTypeValue->ethSataComplexOpt;
}

/*******************************************************************************
* mvBoardEthComplexConfigSet - Set ethernet complex board configuration.
*
* DESCRIPTION:
*	Sets the ethernet / Sata complex configuration in the board spec
*	structure.
*
* INPUT:
*       ethConfig - 32bit value describing the ethernet complex config.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardEthComplexConfigSet(MV_U32 ethConfig)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardEthComplexConfigSet: Board unknown.\n");
		return;
	}

	/* Set ethernet complex configuration. */
	BOARD_INFO(boardId)->pBoardMppTypeValue->ethSataComplexOpt = ethConfig;

	/* Update link speed for MAC0 / 1 */
	/* If MAC 0 is connected to switch, then set to speed 1000Mbps */
	if (mvBoardIsInternalSwitchConnected(0))
		BOARD_INFO(boardId)->pBoardMacInfo[0].boardMacSpeed = BOARD_MAC_SPEED_1000M;
	if (mvBoardIsInternalSwitchConnected(1))
		BOARD_INFO(boardId)->pBoardMacInfo[1].boardMacSpeed = BOARD_MAC_SPEED_1000M;
	if (ethConfig & ESC_OPT_SGMII)
		BOARD_INFO(boardId)->pBoardMacInfo[1].boardMacSpeed = BOARD_MAC_SPEED_1000M;
	return;
}

/*******************************************************************************
* mvBoardPonConfigGet
*
* DESCRIPTION:
*	Returns the PON port configuration from the board spec structure.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit value describing the PON port configuration.
*
*****************************************************************************/
MV_U32 mvBoardPonConfigGet(MV_VOID)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardPonConfigGet:Board unknown.\n");
		return MV_ERROR;
	}

	return BOARD_INFO(boardId)->ponConfigValue;
}

/*******************************************************************************
* mvBoardPonConfigSet
*
* DESCRIPTION:
*	Set the PON port configuration from the board spec structure.
*
* INPUT:
*       ponConfig - 32bit value describing the PON port configuration.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardPonConfigSet(MV_U32 ponConfig)
{
	MV_U32 boardId;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardPonConfigSet:Board unknown.\n");
		return;
	}

	BOARD_INFO(boardId)->ponConfigValue = ponConfig;
	return;
}

/*******************************************************************************
* mvBoardGppConfigGet
*
* DESCRIPTION:
*	Get board configuration according to the input configuration GPP's.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The value of the board configuration GPP's.
*
*******************************************************************************/
MV_U32 mvBoardGppConfigGet(MV_VOID)
{
	MV_U32 boardId, i;
	MV_U32 result = 0;
	MV_U32 gpp;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardGppConfigGet: Board unknown.\n");
		return 0;
	}

	for (i = 0; i < BOARD_INFO(boardId)->numBoardGppInfo; i++) {
		if (BOARD_INFO(boardId)->pBoardGppInfo[i].devClass == BOARD_GPP_CONF) {
			gpp = BOARD_INFO(boardId)->pBoardGppInfo[i].gppPinNum;
			result <<= 1;
			result |= (mvGppValueGet(gpp >> 5, 1 << (gpp & 0x1F)) >> (gpp & 0x1F));
		}
	}
	return result;

}


/*******************************************************************************
* mvBoardModuleConfigSet
*
* DESCRIPTION:
*	Set modules scan result, in order to skip scanning opration if needed.
*
* INPUT:
*	modConfig - Bitmask including the scan result of the ethernet complex
*		    and the TDM modules.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardModuleConfigSet(MV_U32 modConfig)
{
	mvBoardStaticModuleConfig = MV_TRUE;
	mvBoardModuleConfig = modConfig;
	return;
}

/*******************************************************************************
* mvBoardModuleConfigGet
*
* DESCRIPTION:
*	Get modules scan result.
*
* INPUT:
*	None.
*
* OUTPUT:
*	modConfig - Bitmask including the scan result of the ethernet complex
*		    and the TDM modules.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardModuleConfigGet(MV_U32 *modConfig)
{
	MV_U32 ethCompCfg;
	MV_8 tdmCfg;
	MV_U32 boardId = mvBoardIdGet();

	ethCompCfg = mvBoardEthComplexConfigGet();
	tdmCfg = BOARD_INFO(boardId)->boardTdmInfoIndex;

	*modConfig = MV_BRD_MODULE_CFG_BUILD(ethCompCfg, tdmCfg);

	return;
}


/*******************************************************************************
* mvBoardMppIdUpdate - Update MPP ID's according to modules auto-detection.
*
* DESCRIPTION:
*	Update MPP ID's according to on-board modules as detected using TWSI bus.
*
* INPUT:
*	ethSrc	- Sources for the rgmiiA & fe/Ge interfaces. might be needed in
*		case where modules detection is not sufficient to configure
*		internal Mux'es.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardMppIdUpdate(MV_VOID)
{
	MV_U32 devId;
	MV_U32 ethComplexOpt;
	MV_U32 brdModules;
	MV_U32 ethConfig;
	MV_U32 ponOpt;
	MV_ETH_COMPLEX_IF_SOURCES ethCfgSrc;

	devId = mvCtrlModelGet();

	/* Setup ethernet / sata complex configuration. */
	ethComplexOpt = mvBoardEthComplexConfigGet();
	if (ethComplexOpt == ESC_OPT_AUTO) {
		if (mvBoardStaticModuleConfig == MV_FALSE) {
			mvBoardEthSataModulesScan(&brdModules, &ethCfgSrc);
			mvCtrlEthSataComplexBuildConfig(brdModules, &ethCfgSrc, &ethConfig);
		} else {
			ethConfig = MV_BRD_MODULE_CFG_ETH_GET(mvBoardModuleConfig);
		}
		mvBoardEthComplexConfigSet(ethConfig);
		mvBoardSwitchInfoUpdate();
	} else if (mvBoardIdGet() == RD_88F6560_GW_ID) {
		/* Check the board configuration GPP's to see if Sata port is
		 ** enabled.
		 */
		if (mvBoardGppConfigGet() == 1) {
			ethConfig = mvBoardEthComplexConfigGet();
			ethConfig &= ~ESC_OPT_QSGMII;
			ethConfig |= (ESC_OPT_SATA | ESC_OPT_FE3PHY);
			mvBoardEthComplexConfigSet(ethConfig);
		}
	}

	/* Setup TDM configuration. */
	mvBoardTdmModulesScan();

	/* Setup xPON complex configuration */
	ponOpt = mvBoardPonConfigGet();
	if (ponOpt == BOARD_PON_AUTO) {
		mvBoardPonModulesScan(&ponOpt);
		mvBoardPonConfigSet(ponOpt);
	}

#ifndef CONFIG_TAMDHU
	/* Update MPP output voltage for RGMIIB & RGMII2 to 1.8V */
	ethConfig = mvBoardEthComplexConfigGet();
	if (ethConfig & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIA_MAC1 | ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6)) {
		/* Set GE_A & GE_B output voltage to 1.8V */
		MV_REG_BIT_SET(MPP_OUTPUT_DRIVE_REG, MPP_GE_A_1_8_OUTPUT_DRIVE | MPP_GE_B_1_8_OUTPUT_DRIVE);
	} else if (ethConfig & ESC_OPT_RGMIIB_MAC0) {
		/* Set GE_A output voltage to 1.8V */
		MV_REG_BIT_SET(MPP_OUTPUT_DRIVE_REG, MPP_GE_A_1_8_OUTPUT_DRIVE);
	}
#endif

#if defined(CONFIG_MV_ETH_LEGACY) || defined(CONFIG_MV_ETH_NETA)
	/* Set port to MII for MII module */
	ethConfig = mvBoardEthComplexConfigGet();
	if (ethConfig & ESC_OPT_RGMIIA_MAC1)
		mvEthGmacRgmiiSet(1, 1);
	if (ethConfig & (ESC_OPT_RGMIIB_MAC0 | ESC_OPT_RGMIIA_MAC0))
		mvEthGmacRgmiiSet(0, 1);

	if (ethConfig & ESC_OPT_MAC0_2_SW_P4)
		mvEthGmacRgmiiSet(0, 0);
	if (ethConfig & (ESC_OPT_GEPHY_MAC1 | ESC_OPT_MAC1_2_SW_P5))
		mvEthGmacRgmiiSet(1, 0);
#endif /* CONFIG_MV_ETH_LEGACY || CONFIG_MV_ETH_NETA */

	return;
}

/*******************************************************************************
* mvBoardMppGroupTypeGet
*
* DESCRIPTION:
*
* INPUT:
*       mppGroupClass - MPP group number 0  for MPP[35:20] or 1 for MPP[49:36].
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_BOARD_MPP_TYPE_CLASS mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_CLASS mppGroupClass)
{
	MV_U32 boardId;
	MV_U32 ethCompCfg;
	MV_BOARD_MPP_TYPE_CLASS result = MV_BOARD_OTHER;

	boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardMppGet:Board unknown.\n");
		return MV_ERROR;
	}

	if ((boardId != DB_88F6535_BP_ID) || (mvDbDisableModuleDetection))
		return MV_BOARD_OTHER;

	switch (mppGroupClass) {
	case (MV_BOARD_MPP_GROUP_1):
		/* Get TDM / TDU MPP type.      */
#ifdef MV_INCLUDE_TDM
		if (mvBoardTdmDevicesCountGet() > 0) {
			if (mvCtrlTdmUnitTypeGet() == TDM_2CH_UNIT_ID)
				result = MV_BOARD_TDM_2CH;
			else
				result = MV_BOARD_TDM_32CH;
		}
#endif
		break;
	case (MV_BOARD_MPP_GROUP_2):
		/* Get GE0 MPP type.    */
		ethCompCfg = BOARD_INFO(boardId)->pBoardMppTypeValue->ethSataComplexOpt;
		if (ethCompCfg & ESC_OPT_RGMIIB_MAC0)
			result = MV_BOARD_GE0;
		if (result == MV_BOARD_OTHER) {
#ifdef MV_INCLUDE_TDM
			/* Get TDM / TDU MPP type.      */
			if ((mvBoardTdmDevicesCountGet() > 0) && (mvCtrlTdmUnitTypeGet() == TDM_32CH_UNIT_ID))
				result = MV_BOARD_TDM_32CH;
#endif
		}
		break;
	case (MV_BOARD_MPP_GROUP_3):
		/* Get GE0 / GE1 / SW_P5 MPP type. */
		ethCompCfg = BOARD_INFO(boardId)->pBoardMppTypeValue->ethSataComplexOpt;
		if (ethCompCfg & ESC_OPT_RGMIIA_MAC0)
			result = MV_BOARD_GE0;
		else if (ethCompCfg & ESC_OPT_RGMIIA_MAC1)
			result = MV_BOARD_GE1;
		else if (ethCompCfg & ESC_OPT_RGMIIA_SW_P5)
			result = MV_BOARD_SW_P5;
		else if (ethCompCfg & ESC_OPT_RGMIIA_SW_P6)
			result = MV_BOARD_SW_P6;
		else
			result = MV_BOARD_OTHER;
		break;
	default:
		break;
	}

	return result;
}

/*******************************************************************************
* mvBoardTdmSpiModeGet - return SLIC/DAA connection
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*
*******************************************************************************/
MV_32 mvBoardTdmSpiModeGet(MV_VOID)
{
	return DUAL_CHIP_SELECT_MODE;
}

/*******************************************************************************
* mvBoardTdmDevicesCountGet
*
* DESCRIPTION:
*	Return the number of TDM devices on board.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	Number of devices.
*
*******************************************************************************/
MV_U8 mvBoardTdmDevicesCountGet(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_8 index;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardTdmDevicesCountGet: Board unknown.\n");
		return 0;
	}

	index = BOARD_INFO(boardId)->boardTdmInfoIndex;
	if (index == (MV_8)-1)
		return 0;

	return BOARD_INFO(boardId)->numBoardTdmInfo[(MV_U8)index];
}

/*******************************************************************************
* mvBoardTdmSpiCsGet
*
* DESCRIPTION:
*	Return the SPI Chip-select number for a given device.
*
* INPUT:
*	devId	- The Slic device ID to get the SPI CS for.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The SPI CS if found, -1 otherwise.
*
*******************************************************************************/
MV_U8 mvBoardTdmSpiCsGet(MV_U8 devId)
{
	MV_U32 boardId = mvBoardIdGet();
	MV_8 index;

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardTdmDevicesCountGet: Board unknown.\n");
		return -1;
	}

	index = BOARD_INFO(boardId)->boardTdmInfoIndex;
	if (index == (MV_8)-1)
		return 0;

	if (devId >= BOARD_INFO(boardId)->numBoardTdmInfo[(MV_U8)index])
		return -1;

	return BOARD_INFO(boardId)->pBoardTdmInt2CsInfo[(MV_U8)index][devId].spiCs;
}

/*******************************************************************************
* mvBoardTdmSpiIdGet
*
* DESCRIPTION:
*	Return SPI port ID per board.
*
* INPUT:
*	None
*
* OUTPUT:
*       None.
*
* RETURN:
*	SPI port ID.
*
*******************************************************************************/
MV_U8 mvBoardTdmSpiIdGet(MV_VOID)
{
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardTdmSpiIdGet: Board unknown.\n");
		return -1;
	}

	return BOARD_INFO(boardId)->pBoardTdmSpiInfo[0].spiId;
}

/*******************************************************************************
* mvBoardModuleTypePrint
*
* DESCRIPTION:
*	Print on-board detected modules.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	None.
*
*******************************************************************************/
MV_VOID mvBoardMppModuleTypePrint(MV_VOID)
{
	MV_U32 ethConfig;
	MV_U32 ponOpt;
	mvOsOutput("Modules Detected:\n");
	/* GPON / EPON */
	ponOpt = mvBoardPonConfigGet();
	if (ponOpt == BOARD_EPON_CONFIG)
		mvOsOutput("       EPON module detected.\n");
	else if (ponOpt == BOARD_GPON_CONFIG)
		mvOsOutput("       GPON module detected.\n");
	else
		mvOsOutput("       No PON module.\n");

	/* TDM */
	if (mvBoardTdmDevicesCountGet() > 0)
		mvOsOutput("       TDM module.\n");

	ethConfig = mvBoardEthComplexConfigGet();

	if (ethConfig & ESC_OPT_ILLEGAL) {
		mvOsOutput("       Illegal configuration detected!\n");
		return;
	}
	/* Sata */
	if (ethConfig & ESC_OPT_SATA)
		mvOsOutput("       Sata module detected.\n");

	/* Ethernet Complex */

	/* RGMIIA */
	if (ethConfig & ESC_OPT_RGMIIA_MAC0)
		mvOsOutput("       RGMIIA Module on MAC0.\n");
	if (ethConfig & ESC_OPT_RGMIIA_MAC1)
		mvOsOutput("       RGMIIA Module on MAC1.\n");
	if (ethConfig & ESC_OPT_RGMIIA_SW_P5)
		mvOsOutput("       RGMIIA Module on Switch port #5.\n");
	if (ethConfig & ESC_OPT_RGMIIA_SW_P6)
		mvOsOutput("       RGMIIA Module on Switch port #6.\n");

	/* RGMIIB */
	if (ethConfig & ESC_OPT_RGMIIB_MAC0)
		mvOsOutput("       RGMIIB Module on MAC0.\n");

	/* Ethernet Switch */
	if (ethConfig & (ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5)) {
		if (ethConfig & ESC_OPT_MAC0_2_SW_P4)
			mvOsOutput("       Ethernet Switch on MAC0.\n");
		if (ethConfig & ESC_OPT_MAC1_2_SW_P5)
			mvOsOutput("       Ethernet Switch on MAC1.\n");
	}

	/* 3xFE PHY */
	if (ethConfig & ESC_OPT_FE3PHY)
		mvOsOutput("       3xFE PHY Module.\n");

	/* Internal FE/GE Phy */
	if (ethConfig & (ESC_OPT_GEPHY_MAC1 | ESC_OPT_GEPHY_SW_P0 | ESC_OPT_GEPHY_SW_P5)) {
		if (ethConfig & ESC_OPT_GEPHY_MAC1)
			mvOsOutput("       GE-PHY on MAC1.\n");
		if (ethConfig & ESC_OPT_GEPHY_SW_P0)
			mvOsOutput("       GE-PHY on Switch port #0.\n");
		if (ethConfig & ESC_OPT_GEPHY_SW_P5)
			mvOsOutput("       GE-PHY on Switch port #5.\n");
	}

	/* QSGMII */
	if (ethConfig & ESC_OPT_QSGMII)
		mvOsOutput("       QSGMII Module.\n");

	/* SGMII */
	if (ethConfig & ESC_OPT_SGMII)
		mvOsOutput("       SGMII Module.\n");

	return;
}


/*******************************************************************************
* mvBoardIsGbEPortConnected
*
* DESCRIPTION:
*	Checks if a given GbE port is actually connected to the GE-PHY, internal Switch or any RGMII module.
*
* INPUT:
*	port - GbE port number (0 or 1).
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_TRUE if port is connected, MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvBoardIsGbEPortConnected(MV_U32 ethPortNum)
{
	MV_U32 ethConfig;

	ethConfig = mvBoardEthComplexConfigGet();

	if (ethConfig & ESC_OPT_ILLEGAL)
		return MV_FALSE;

	if (ethPortNum == 0) {
		if (ethConfig & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIB_MAC0 | ESC_OPT_MAC0_2_SW_P4))
			return MV_TRUE;
		else
			return MV_FALSE;
	}

	if (ethPortNum == 1) {
		if (ethConfig & (ESC_OPT_RGMIIA_MAC1 | ESC_OPT_MAC1_2_SW_P5 | ESC_OPT_GEPHY_MAC1))
			return MV_TRUE;
		else
			return MV_FALSE;
	}

	mvOsPrintf("mvBoardIsGbEPortConnected: unknown port %d\n", ethPortNum);
	return MV_FALSE;
}


/* Board devices API managments */

/*******************************************************************************
* mvBoardGetDeviceNumber - Get number of device of some type on the board
*
* DESCRIPTION:
*
* INPUT:
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		number of those devices else the function returns 0
*
*
*******************************************************************************/
MV_32 mvBoardGetDevicesNumber(MV_BOARD_DEV_CLASS devClass)
{
	MV_U32 foundIndex = 0, devNum;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("mvBoardGetDeviceNumber:Board unknown.\n");
		return 0xFFFFFFFF;
	}

	for (devNum = START_DEV_CS; devNum < BOARD_INFO(boardId)->numBoardDeviceIf; devNum++) {
		if (BOARD_INFO(boardId)->pDevCsInfo[devNum].devClass == devClass)
			foundIndex++;
	}

	return foundIndex;
}

/*******************************************************************************
* mvBoardGetDeviceBaseAddr - Get base address of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*	Base address else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceBaseAddr(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return mvCpuIfTargetWinBaseLowGet(DEV_TO_TARGET(devEntry->deviceCS));

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDeviceBusWidth - Get Bus width of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		Bus width else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceBusWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return devEntry->devWidth;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardGetDeviceWidth - Get dev width of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		dev width else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return devEntry->devWidth;

	return MV_ERROR;
}

/*******************************************************************************
* mvBoardGetDeviceWinSize - Get the window size of a device existing on the board
*
* DESCRIPTION:
*
* INPUT:
*       devIndex - The device sequential number on the board
*		devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*       None.
*
* RETURN:
*       If the device is found on the board the then the functions returns the
*		window size else the function returns 0xffffffff
*
*
*******************************************************************************/
MV_32 mvBoardGetDeviceWinSize(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return mvCpuIfTargetWinSizeGet(DEV_TO_TARGET(devEntry->deviceCS));

	return 0xFFFFFFFF;
}

/*******************************************************************************
* boardGetDevEntry - returns the entry pointer of a device on the board
*
* DESCRIPTION:
*
* INPUT:
*	devIndex - The device sequential number on the board
*	devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*	None.
*
* RETURN:
*	If the device is found on the board the then the functions returns the
*	dev number else the function returns 0x0
*
*******************************************************************************/
static MV_DEV_CS_INFO *boardGetDevEntry(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_U32 foundIndex = 0, devIndex;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("boardGetDevEntry: Board unknown.\n");
		return NULL;
	}

	for (devIndex = START_DEV_CS; devIndex < BOARD_INFO(boardId)->numBoardDeviceIf; devIndex++) {
		if (BOARD_INFO(boardId)->pDevCsInfo[devIndex].devClass == devClass) {
			if (foundIndex == devNum)
				return &(BOARD_INFO(boardId)->pDevCsInfo[devIndex]);
			foundIndex++;
		}
	}

	/* device not found */
	return NULL;
}

/*******************************************************************************
* boardGetDevCSNum
*
* DESCRIPTION:
*	Return the device's chip-select number.
*
* INPUT:
*	devIndex - The device sequential number on the board
*	devType - The device type ( Flash,RTC , etc .. )
*
* OUTPUT:
*	None.
*
* RETURN:
*	If the device is found on the board the then the functions returns the
*	dev number else the function returns 0x0
*
*******************************************************************************/
MV_U32 boardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass)
{
	MV_DEV_CS_INFO *devEntry;
	MV_U32 boardId = mvBoardIdGet();

	if (!((boardId >= BOARD_ID_BASE) && (boardId < MV_MAX_BOARD_ID))) {
		mvOsPrintf("Board unknown.\n");
		return 0xFFFFFFFF;
	}

	devEntry = boardGetDevEntry(devNum, devClass);
	if (devEntry != NULL)
		return devEntry->deviceCS;

	return 0xFFFFFFFF;
}

/*******************************************************************************
* mvBoardTwsiAddrTypeGet -
*
* DESCRIPTION:
*	Return the TWSI address type for a given twsi device class.
*
* INPUT:
*	twsiClass - The TWSI device to return the address type for.
*	index	  - The TWSI device index (Pass 0 in case of a single
*		    device)
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TWSI address type.
*
*******************************************************************************/
MV_U8 mvBoardTwsiAddrTypeGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++) {
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddrType;
			else
				indexFound++;
		}
	}
	return (MV_ERROR);
}

/*******************************************************************************
* mvBoardTwsiAddrGet -
*
* DESCRIPTION:
*	Return the TWSI address for a given twsi device class.
*
* INPUT:
*	twsiClass - The TWSI device to return the address type for.
*	index	  - The TWSI device index (Pass 0 in case of a single
*		    device)
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TWSI address.
*
*******************************************************************************/
MV_U8 mvBoardTwsiAddrGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index)
{
	int i;
	MV_U32 indexFound = 0;
	MV_U32 boardId = mvBoardIdGet();

	for (i = 0; i < BOARD_INFO(boardId)->numBoardTwsiDev; i++) {
		if (BOARD_INFO(boardId)->pBoardTwsiDev[i].devClass == twsiClass) {
			if (indexFound == index)
				return BOARD_INFO(boardId)->pBoardTwsiDev[i].twsiDevAddr;
			else
				indexFound++;
		}
	}
	return (0xFF);
}

/*******************************************************************************
* mvBoardNandWidthGet -
*
* DESCRIPTION: Get the width of the first NAND device in byte.
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN: 1, 2, 4 or MV_ERROR
*
*
*******************************************************************************/
MV_32 mvBoardNandWidthGet(MV_VOID)
{
	MV_U32 devNum;
	MV_U32 devWidth;
	MV_U32 boardId = mvBoardIdGet();

	for (devNum = START_DEV_CS; devNum < BOARD_INFO(boardId)->numBoardDeviceIf; devNum++) {
		devWidth = mvBoardGetDeviceWidth(devNum, BOARD_DEV_NAND_FLASH);
		if (devWidth != MV_ERROR)
			return (devWidth / 8);
	}

	/* NAND wasn't found */
	return MV_ERROR;
}

MV_U32 gBoardId = -1;

/*******************************************************************************
* mvBoardIdGet - Get Board model
*
* DESCRIPTION:
*       This function returns board ID.
*       Board ID is 32bit word constructed of board model (16bit) and
*       board revision (16bit) in the following way: 0xMMMMRRRR.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit board ID number, '-1' if board is undefined.
*
*******************************************************************************/
MV_U32 mvBoardIdGet(MV_VOID)
{
	MV_U32 tmpBoardId = -1;

	if (gBoardId == -1) {
#if defined(DB_88F6500)
		tmpBoardId = DB_88F6535_BP_ID;
#elif defined(RD_88F6510)
		tmpBoardId = RD_88F6510_SFU_ID;
#elif defined(RD_88F6530)
		tmpBoardId = RD_88F6530_MDU_ID;
#elif defined(RD_88F6560)
		tmpBoardId = RD_88F6560_GW_ID;
#elif defined(DB_CUSTOMER)
		tmpBoardId = DB_CUSTOMER_ID;
#elif defined(CONFIG_ARRAN)
		tmpBoardId = DB_ARRAN_ID;
#elif defined(CONFIG_BOWMORE)
		tmpBoardId = DB_BOWMORE_ID;
#elif defined(CONFIG_TAMDHU)
		tmpBoardId = DB_TAMDHU_ID;
#endif
		gBoardId = tmpBoardId;
	}

	return gBoardId;
}

/*******************************************************************************
* mvBoardTwsiSatRGet -
*
* DESCRIPTION:
*
* INPUT:
*	device num - one of three devices
*	reg num - 0 or 1
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_U8 mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;
	MV_U8 data;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read MPP module ID */
	DB(mvOsPrintf("Board: Read S@R device read\n"));
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_DEV_TWSI_SATR, devNum);

	twsiSlave.validOffset = MV_TRUE;
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;

	if (MV_OK != mvTwsiRead(0, &twsiSlave, &data, 1)) {
		DB(mvOsPrintf("Board: Read S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Read S@R succeded\n"));

	return data;
}

/*******************************************************************************
* mvBoardTwsiSatRSet -
*
* DESCRIPTION:
*
* INPUT:
*	devNum - one of three devices
*	regNum - 0 or 1
*	regVal - value
*
*
* OUTPUT:
*		None.
*
* RETURN:
*		reg value
*
*******************************************************************************/
MV_STATUS mvBoardTwsiSatRSet(MV_U8 devNum, MV_U8 regNum, MV_U8 regVal)
{
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	/* TWSI init */
	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read MPP module ID */
	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_DEV_TWSI_SATR, devNum);
	twsiSlave.validOffset = MV_TRUE;
	DB(mvOsPrintf("Board: Write S@R device addr %x, type %x, data %x\n",
		      twsiSlave.slaveAddr.address, twsiSlave.slaveAddr.type, regVal));
	/* Use offset as command */
	twsiSlave.offset = regNum;
	twsiSlave.moreThen256 = MV_FALSE;
	if (MV_OK != mvTwsiWrite(0, &twsiSlave, &regVal, 1)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}
	DB(mvOsPrintf("Board: Write S@R succeded\n"));

	return MV_OK;
}

/*******************************************************************************
* SatR Configuration functions
*******************************************************************************/
MV_U8 mvBoardFreqGet(MV_VOID)
{
	MV_U8 sar0, sar1;

	sar0 = mvBoardTwsiSatRGet(0, 0);
	sar1 = mvBoardTwsiSatRGet(2, 0);
	return MSAR_CPU_DDR_L2_CLCK_TWSI_EXTRACT(sar0, sar1);
}

MV_STATUS mvBoardFreqSet(MV_U8 freqVal)
{
	MV_U8 sar0, sar1;

	sar0 = freqVal & 0x1f;
	if (MV_OK != mvBoardTwsiSatRSet(0, 0, sar0)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	sar1 = mvBoardTwsiSatRGet(2, 0);
	sar1 = MV_SET_BIT(sar1, 4, MV_GET_BIT(freqVal, 5));
	if (MV_OK != mvBoardTwsiSatRSet(2, 0, sar1)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write Freq S@R succeeded\n"));
	return MV_OK;
}

MV_U8 mvBoardSSCGGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(1, 0);
	return MV_GET_BIT(sar, 3);
}

MV_STATUS mvBoardSSCGSet(MV_U8 val)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(1, 0);
	sar = MV_SET_BIT(sar, 3, (val & 0x1));
	if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write Freq S@R succeeded\n"));
	return MV_OK;
}

MV_U8 mvBoardPexClkGet(MV_U8 pexIf)
{
	MV_U8 sar;
	MV_U8 sarDev;
	MV_U8 bit;

	if (pexIf == 0) {
		sarDev = 2;
		bit = 2;
	} else {
		sarDev = 3;
		bit = 0;
	}

	sar = mvBoardTwsiSatRGet(sarDev, 0);
	return MV_GET_BIT(sar, bit);
}

MV_STATUS mvBoardPexClkSet(MV_U8 pexIf, MV_U8 val)
{
	MV_U8 sar;
	MV_U8 sarDev;
	MV_U8 bit;

	if (pexIf == 0) {
		sarDev = 2;
		bit = 2;
	} else {
		sarDev = 3;
		bit = 0;
	}

	sar = mvBoardTwsiSatRGet(sarDev, 0);
	sar = MV_SET_BIT(sar, bit, (val & 0x1));
	if (MV_OK != mvBoardTwsiSatRSet(sarDev, 0, sar)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write Pex Clock S@R succeeded\n"));
	return MV_OK;
}

MV_U8 mvBoardSaRL2ExistGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 0);
	return MV_GET_BIT(sar, 3);
}

MV_U8 mvBoardSaRL2ExistSet(MV_U8 val)
{
	MV_U8 sar;
	MV_U8 sarDev;
	MV_U8 bit;

	sarDev = 3;
	bit = 3;

	sar = mvBoardTwsiSatRGet(sarDev, 0);
	sar = MV_SET_BIT(sar, bit, (val & 0x1));
	if (MV_OK != mvBoardTwsiSatRSet(sarDev, 0, sar)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write L2-Exist S@R succeeded\n"));
	return MV_OK;
}


MV_U8 mvBoardPonGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 0);
	return MV_GET_BIT(sar, 4);
}

MV_STATUS mvBoardPonSet(MV_U8 val)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(3, 0);
	sar = MV_SET_BIT(sar, 4, (val & 0x1));
	if (MV_OK != mvBoardTwsiSatRSet(3, 0, sar)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write Freq S@R succeeded\n"));
	return MV_OK;
}

MV_U8 mvBoardTClkGet(MV_VOID)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(1, 0);
	return (sar & 0x3);
}

MV_STATUS mvBoardTClkSet(MV_U8 val)
{
	MV_U8 sar;

	sar = mvBoardTwsiSatRGet(1, 0);
	sar = ((sar & ~(0x3)) | (val & 0x3));
	if (MV_OK != mvBoardTwsiSatRSet(1, 0, sar)) {
		DB(mvOsPrintf("Board: Write S@R fail\n"));
		return MV_ERROR;
	}

	DB(mvOsPrintf("Board: Write Freq S@R succeeded\n"));
	return MV_OK;
}

/*******************************************************************************
* End of SatR Configuration functions
*******************************************************************************/

/*******************************************************************************
* mvBoardEthSataModulesScan
*
* DESCRIPTION:
*	Scan the on-board ethernet & Sata complex modules, and return a
*	list of all available modules.
*
* INPUT:
*	None.
*
* OUTPUT:
*	modules - A bitmask of all modules detected on board
*	(bitwise or from MV_BOARD_MODULE_ID_CLASS)
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardEthSataModulesScan(MV_U32 *modules, MV_ETH_COMPLEX_IF_SOURCES *ethSrcCfg)
{
	MV_U32 result = 0;
	MV_U8 cfg, boardCfg;
	MV_STATUS status;
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	if (mvBoardIdGet() != DB_88F6535_BP_ID && mvBoardIdGet() != DB_ARRAN_ID
            && mvBoardIdGet() != DB_TAMDHU_ID)
		return MV_OK;

	/* Go over all possible modules and check for existence */

	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);
	/* Read the board config EEPROM */
	twsiSlave.slaveAddr.address = MV_BOARD_EEPROM_MODULE_ADDR;
	twsiSlave.slaveAddr.type = MV_BOARD_EEPROM_MODULE_ADDR_TYPE;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	status = mvTwsiRead(0, &twsiSlave, &boardCfg, 1);
	if (status != MV_OK) {
		*modules = 0;
		return status;
	}
	switch (MV_BOARD_CFG_IS_SW_FORCE_P1_SRC(boardCfg)) {
	case 1:
		ethSrcCfg->swSrc = EC_MAC1_SRC;
		break;
	case 0:
	default:
		ethSrcCfg->swSrc = EC_SRC_NONE;
		break;
	}

	switch (MV_BOARD_CFG_RGMIIA_SRC(boardCfg)) {
	case 0:
		ethSrcCfg->rgmiiASrc = EC_SW_P6_SRC;
		break;
	case 1:
		ethSrcCfg->rgmiiASrc = EC_MAC0_SRC;
		break;
	case 2:
		ethSrcCfg->rgmiiASrc = EC_MAC1_SRC;
		break;
	case 3:
		ethSrcCfg->rgmiiASrc = EC_SW_P5_SRC;
		break;
	default:
		ethSrcCfg->rgmiiASrc = EC_SRC_NONE;
		break;
	}

	switch (MV_BOARD_CFG_FEGEPHY_SRC(boardCfg)) {
	case 1:
		ethSrcCfg->feGeSrc = EC_MAC1_SRC;
		break;
	case 2:
		ethSrcCfg->feGeSrc = EC_SW_P0_SRC;
		break;
	case 3:
		ethSrcCfg->feGeSrc = EC_SW_P5_SRC;
		break;
	case 0:
	default:
		ethSrcCfg->feGeSrc = EC_SRC_NONE;
		break;
	}

	/* Sata / QSGMII / SGMII */
	switch (MV_BOARD_CFG_SERDES_SRC(boardCfg)) {
	case 1:
		result |= MV_BOARD_MODULE_SATA_ID;
		break;
	case 2:
		result |= MV_BOARD_MODULE_SGMII_ID;
		break;
	case 3:
		result |= MV_BOARD_MODULE_SDIO_ID;
		break;
	case 0:
	default:
		break;
	}
	/* FEGE PHY */
	switch (MV_BOARD_CFG_FEGEPHY_SRC(boardCfg)) {
	case 1:
	case 2:
	case 3:
		result |= MV_BOARD_MODULE_FE_GE_PHY_ID;
		break;
	case 0:
	default:
		break;
	}
	/* 4FE / 3FE+1GE */
	if (MV_BOARD_CFG_IS_3FE_EN(boardCfg))
		result |= MV_BOARD_MODULE_4FE_PHY_ID;

	/* RGMII 0 */
	twsiSlave.slaveAddr.address = MV_BOARD_RGMIIA_MODULE_ADDR;
	twsiSlave.slaveAddr.type = MV_BOARD_RGMIIA_MODULE_ADDR_TYPE;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	if (mvBoardTwsiReadByteThruMux(MV_BOARD_RGMIIA_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK) {
		if (cfg == 0x3)
			result |= MV_BOARD_MODULE_RGMIIA_ID;
		else if (cfg == 0x6)
			result |= MV_BOARD_MODULE_MIIA_ID;
	}

	/* RGMII 1 */
	twsiSlave.slaveAddr.address = MV_BOARD_RGMIIB_MODULE_ADDR;
	twsiSlave.slaveAddr.type = MV_BOARD_RGMIIB_MODULE_ADDR_TYPE;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	if (mvBoardTwsiReadByteThruMux(MV_BOARD_RGMIIB_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK) {
		if (cfg == 0x3)
			result |= MV_BOARD_MODULE_RGMIIB_ID;
		else if (cfg == 0x6)
			result |= MV_BOARD_MODULE_MIIB_ID;
	}

	/* QSGMII */
	twsiSlave.slaveAddr.address = MV_BOARD_QSGMII_MODULE_ADDR;
	twsiSlave.slaveAddr.type = MV_BOARD_QSGMII_MODULE_ADDR_TYPE;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	if (mvBoardTwsiReadByteThruMux(MV_BOARD_QSGMII_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK) {
		if (((cfg & 0xf) == 0x8) || ((cfg & 0xf) == 0xb))
			result |= MV_BOARD_MODULE_QSGMII_ID;
	}

	/* GE SFP */
	twsiSlave.slaveAddr.address = MV_BOARD_GE_SFP_MODULE_ADDR;
	twsiSlave.slaveAddr.type = MV_BOARD_GE_SFP_MODULE_ADDR_TYPE;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	if (mvBoardTwsiReadByteThruMux(MV_BOARD_GE_SFP_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK)
		result |= MV_BOARD_MODULE_GE_SFP_ID;

	*modules = result;
	return MV_OK;
}

/*******************************************************************************
* mvBoardIsSdioEnabled
*
* DESCRIPTION:
*	Check if SDIO is enabled on KW2-DB board.
*	the check is done by reading the on-board EEPROM.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_TRUE if SDIO is enabled,
*	MV_FALSE otherwise.
*	On all boards except DB this function will return MV_TRUE.
*
*******************************************************************************/
MV_STATUS mvBoardIsSdioEnabled(MV_VOID)
{
	MV_U8 boardCfg;
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	if (mvBoardIdGet() != DB_88F6535_BP_ID && mvBoardIdGet() != DB_ARRAN_ID
            && mvBoardIdGet() != DB_TAMDHU_ID)
		return MV_TRUE;

	/* Go over all possible modules and check for existence */

	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	/* Read the board config EEPROM */
	twsiSlave.slaveAddr.address = MV_BOARD_EEPROM_MODULE_ADDR;
	twsiSlave.slaveAddr.type = MV_BOARD_EEPROM_MODULE_ADDR_TYPE;
	twsiSlave.validOffset = MV_TRUE;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	if (mvTwsiRead(0, &twsiSlave, &boardCfg, 1) != MV_OK)
		return MV_FALSE;

	if (MV_BOARD_CFG_SERDES_SRC(boardCfg) == 3)
		return MV_TRUE;

	return MV_FALSE;
}

/*******************************************************************************
* mvBoardTdmModulesScan
*
* DESCRIPTION:
*	Scan the on-board TDM modules.
*
* INPUT:
*	None.
*
* OUTPUT:
*	modules - A bitmask of all modules detected on board
*	(bitwise or from MV_BOARD_MODULE_ID_CLASS)
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardTdmModulesScan(MV_VOID)
{
	MV_U32 result = 0;
	MV_U8 cfg;
	MV_TWSI_SLAVE twsiSlave;
	MV_U32 boardId = mvBoardIdGet();

	/* Perform scan only for DB board. */
	if (boardId != DB_88F6535_BP_ID && boardId != DB_ARRAN_ID
            && boardId != DB_TAMDHU_ID) {
		/* We have only a single Slic config for RD boards. */
		BOARD_INFO(boardId)->boardTdmInfoIndex = 0;
		return MV_OK;
	}

	if (mvBoardStaticModuleConfig == MV_TRUE) {
		BOARD_INFO(boardId)->boardTdmInfoIndex = MV_BRD_MODULE_CFG_TDM_GET(mvBoardModuleConfig);
		result = 1;
	}

	/* TDM-3215 */
	if (result == 0) {
		twsiSlave.slaveAddr.address = MV_BOARD_TDM_3215_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_TDM_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvBoardTwsiReadByteThruMux(MV_BOARD_TDM_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK) {
			BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_3215;
			result = 1;
		}
	}

	/* TDM-880 */
	if (result == 0) {
		twsiSlave.slaveAddr.address = MV_BOARD_TDM_880_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_TDM_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvBoardTwsiReadByteThruMux(MV_BOARD_TDM_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK) {
			BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_880;
			result = 1;
		}
	}

	/* TDM-792 Adapter */
	if (result == 0) {
		twsiSlave.slaveAddr.address = MV_BOARD_TDM_792_ADAPT_MODULE_ADDR;
		twsiSlave.slaveAddr.type = MV_BOARD_TDM_MODULE_ADDR_TYPE;
		twsiSlave.validOffset = MV_TRUE;
		twsiSlave.offset = 0;
		twsiSlave.moreThen256 = MV_FALSE;
		if (mvBoardTwsiReadByteThruMux(MV_BOARD_TDM_MODULE_MUX_CHNUM, 0, &twsiSlave, &cfg) == MV_OK) {
			BOARD_INFO(boardId)->boardTdmInfoIndex = BOARD_TDM_SLIC_792;
			result = 1;
		}
	}

	if (result == 0) {
		/* No device is found */
		BOARD_INFO(boardId)->boardTdmInfoIndex = (MV_8)-1;
	}

	return MV_OK;
}

/*******************************************************************************
* mvBoardPonModulesScan
*
* DESCRIPTION:
*	Scan the on-board PON modules.
*
* INPUT:
*	None.
*
* OUTPUT:
*	modules - A bitmask of all modules detected on board
*	(bitwise or from MV_BOARD_MODULE_ID_CLASS)
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardPonModulesScan(MV_U32 *modules)
{
	MV_U32 result = 0;

	/* GPON / EPON */
	if (mvCtrlIsGponMode() == MV_TRUE)
		result |= BOARD_GPON_CONFIG;
	else
		result |= BOARD_EPON_CONFIG;

	*modules = result;
	return MV_OK;
}

/*******************************************************************************
* mvBoardTwsiMuxChannelSet
*
* DESCRIPTION:
*	Set the channel number of the on-board TWSI mux.
*
* INPUT:
*	chNum	- The channel number to set.
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardTwsiMuxChannelSet(MV_U8 muxChNum)
{
	static MV_U8 currChNum = 0xFF;
	MV_TWSI_SLAVE twsiSlave;
	MV_TWSI_ADDR slave;

	if (currChNum == muxChNum)
		return MV_OK;

	slave.type = ADDR7_BIT;
	slave.address = 0;
	mvTwsiInit(0, TWSI_SPEED, mvBoardTclkGet(), &slave, 0);

	twsiSlave.slaveAddr.address = mvBoardTwsiAddrGet(BOARD_TWSI_MUX, 0);
	twsiSlave.slaveAddr.type = mvBoardTwsiAddrTypeGet(BOARD_TWSI_MUX, 0);
	twsiSlave.validOffset = 0;
	twsiSlave.offset = 0;
	twsiSlave.moreThen256 = MV_FALSE;
	muxChNum += 4;
	return mvTwsiWrite(0, &twsiSlave, &muxChNum, 1);
}

/*******************************************************************************
* mvBoardTwsiReadByteThruMux
*
* DESCRIPTION:
*	Read a single byte from a TWSI device through the TWSI Mux.
*
* INPUT:
*	muxChNum	- The Twsi Mux channel number to read through.
*	chNum		- The TWSI channel number.
*	pTwsiSlave	- The TWSI slave address.
*	data		- Buffer to read into (1 byte).
*
* OUTPUT:
*	None.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvBoardTwsiReadByteThruMux(MV_U8 muxChNum, MV_U8 chNum, MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *data)
{
	MV_STATUS res;

	/* Set Mux channel */
	res = mvBoardTwsiMuxChannelSet(muxChNum);
	if (res == MV_OK)
		res = mvTwsiRead(chNum, pTwsiSlave, data, 1);

	return res;
}
