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

    *   Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
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

#include <common.h>
#include <command.h>
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysEthPhyConfig.h"
#include "mvEthPhyRegs.h"
#include "mvEthPhy.h"

static 	MV_VOID	mvEthPhyPower(MV_U32 ethPortNum, MV_BOOL enable);

static MV_ETHPHY_HAL_DATA ethphyHalData;

/*******************************************************************************
* mvEthPhyHalInit -
*
* DESCRIPTION:
*       Initialize the ethernet phy unit HAL.
*
* INPUT:
*       halData	- Ethernet PHY HAL data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK on success, MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvEthPhyHalInit(MV_ETHPHY_HAL_DATA *halData)
{
	mvOsMemcpy(&ethphyHalData, halData, sizeof(MV_ETHPHY_HAL_DATA));

	return MV_OK;
}
/*******************************************************************************
* mvEthPhyInit -
*
* DESCRIPTION:
*       Initialize the ethernet phy unit.
*
* INPUT:
*       ethPortNum - The port number on which to initialize the PHY.
*	eeeEnable  - Whether to enable EEE or not.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK on success, MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvEthPhyInit(MV_U32 ethPortNum, MV_BOOL eeeEnable)
{
	MV_U32     phyAddr = 0;
	MV_U16     deviceId;
	MV_U16     id1, id2;

	if (ethPortNum != ((MV_U32) -1))
		phyAddr = ethphyHalData.phyAddr[ethPortNum];

	/* Set page as 0 */
	if (mvEthPhyRegWrite(phyAddr, 22, 0) != MV_OK)
		return MV_ERROR;

	/* Reads ID1 */
	if (mvEthPhyRegRead(phyAddr, 2, &id1) != MV_OK)
		return MV_ERROR;

	/* Reads ID2 */
	if (mvEthPhyRegRead(phyAddr, 3, &id2) != MV_OK)
		return MV_ERROR;

	if (!MV_IS_MARVELL_OUI(id1, id2)) {
		mvOsPrintf("Cannot find Marvell Device id1 %x id2 %x\n", id1, id2);
		return MV_ERROR;
	}

	deviceId = (id2 & 0x3F0) >> 4;
	switch (deviceId) {
	case MV_PHY_88E1116:
	case MV_PHY_88E1116R:
		mvEthE1116PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E3061:
		mvEthE3016PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1111:
		mvEthE1111PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1112:
		mvEthE1112PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1011:
		mvEthE1011PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E1121:
		mvEth1121PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E114X:
		mvEth1145PhyBasicInit(ethPortNum);
		break;
	case MV_PHY_88E154X:
	/* case MV_PHY_KW2_INTERNAL_GE: */
		if (ethPortNum != ((MV_U32) -1))
			mvEthInternalGEPhyBasicInit(ethPortNum);
		else
			mvEth1540PhyBasicInit(eeeEnable);
		break;
	case MV_PHY_88E1340S:
	case MV_PHY_88E1340:
	case MV_PHY_88E1340M:
		mvEth1340PhyBasicInit();
		break;
	case MV_PHY_88E104X:
	case MV_PHY_88E10X0:
	case MV_PHY_88E10X0S:
	case MV_PHY_88E3082:
	case MV_PHY_88E1149:
	case MV_PHY_88E1181:
	case MV_PHY_88E3016_88E3019:
	case MV_PHY_88E1240:
	case MV_PHY_88E1149R:
	case MV_PHY_88E1119R:
	case MV_PHY_88E1310:
	default:
			mvOsPrintf("Unknown Device(%#x). Initialization failed\n", deviceId);
			return MV_ERROR;
	}
	return MV_OK;
}

void    rdPhy(MV_U32 phyAddr, MV_U32 regOffs)
{
	MV_U16      data;
	MV_STATUS   status;

	status = mvEthPhyRegRead(phyAddr, regOffs, &data);
	if (status == MV_OK)
		mvOsPrintf("reg=%d: 0x%04x\n", regOffs, data);
	else
		mvOsPrintf("Read failed\n");
}


/*******************************************************************************
* mvEthPhyRegRead - Read from ethernet phy register.
*
* DESCRIPTION:
*       This function reads ethernet phy register.
*
* INPUT:
*       phyAddr - Phy address.
*       regOffs - Phy register offset.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit phy register value, or 0xffff on error
*
*******************************************************************************/
MV_STATUS mvEthPhyRegRead(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 *data)
{
	MV_U32 			smiReg;
	volatile MV_U32 timeout;

	/* check parameters */
	if ((phyAddr << ETH_PHY_SMI_DEV_ADDR_OFFS) & ~ETH_PHY_SMI_DEV_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegRead: Err. Illegal PHY device address %d\n",
				phyAddr);
		return MV_FAIL;
	}
	if ((regOffs <<  ETH_PHY_SMI_REG_ADDR_OFFS) & ~ETH_PHY_SMI_REG_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegRead: Err. Illegal PHY register offset %d\n",
				regOffs);
		return MV_FAIL;
	}

	timeout = ETH_PHY_TIMEOUT;
	/* wait till the SMI is not busy*/
	do {
		/* read smi register */
		smiReg = MV_REG_READ(ETH_PHY_SMI_REG);
		if (timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegRead: SMI busy timeout\n");
			return MV_FAIL;
		}
	} while (smiReg & ETH_PHY_SMI_BUSY_MASK);

	/* fill the phy address and regiser offset and read opcode */
	smiReg = (phyAddr <<  ETH_PHY_SMI_DEV_ADDR_OFFS) | (regOffs << ETH_PHY_SMI_REG_ADDR_OFFS)|
			   ETH_PHY_SMI_OPCODE_READ;

	/* write the smi register */
	MV_REG_WRITE(ETH_PHY_SMI_REG, smiReg);

	timeout = ETH_PHY_TIMEOUT;

	/*wait till readed value is ready */
	do {
		/* read smi register */
		smiReg = MV_REG_READ(ETH_PHY_SMI_REG);

		if (timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegRead: SMI read-valid timeout\n");
			return MV_FAIL;
		}
	} while (!(smiReg & ETH_PHY_SMI_READ_VALID_MASK));

	/* Wait for the data to update in the SMI register */
	for (timeout = 0; timeout < ETH_PHY_TIMEOUT; timeout++)
		;

	*data = (MV_U16)(MV_REG_READ(ETH_PHY_SMI_REG) & ETH_PHY_SMI_DATA_MASK);

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyRegWrite - Write to ethernet phy register.
*
* DESCRIPTION:
*       This function write to ethernet phy register.
*
* INPUT:
*       phyAddr - Phy address.
*       regOffs - Phy register offset.
*       data    - 16bit data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK if write succeed, MV_BAD_PARAM on bad parameters , MV_ERROR on error .
*		MV_TIMEOUT on timeout
*
*******************************************************************************/
MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data)
{
	MV_U32 			smiReg;
	volatile MV_U32 timeout;

	/* check parameters */
	if ((phyAddr <<  ETH_PHY_SMI_DEV_ADDR_OFFS) & ~ETH_PHY_SMI_DEV_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegWrite: Err. Illegal phy address \n");
		return MV_BAD_PARAM;
	}
	if ((regOffs <<  ETH_PHY_SMI_REG_ADDR_OFFS) & ~ETH_PHY_SMI_REG_ADDR_MASK) {
		mvOsPrintf("mvEthPhyRegWrite: Err. Illegal register offset \n");
		return MV_BAD_PARAM;
	}

	timeout = ETH_PHY_TIMEOUT;

	/* wait till the SMI is not busy*/
	do {
		/* read smi register */
		smiReg = MV_REG_READ(ETH_PHY_SMI_REG);
		if (timeout-- == 0) {
			mvOsPrintf("mvEthPhyRegWrite: SMI busy timeout\n");
		return MV_TIMEOUT;
		}
	} while (smiReg & ETH_PHY_SMI_BUSY_MASK);

	/* fill the phy address and regiser offset and write opcode and data*/
	smiReg = (data << ETH_PHY_SMI_DATA_OFFS);
	smiReg |= (phyAddr <<  ETH_PHY_SMI_DEV_ADDR_OFFS) | (regOffs << ETH_PHY_SMI_REG_ADDR_OFFS);
	smiReg &= ~ETH_PHY_SMI_OPCODE_READ;

	/* write the smi register */
	MV_REG_WRITE(ETH_PHY_SMI_REG, smiReg);

	return MV_OK;
}


/*******************************************************************************
* mvEthPhyReset - Reset ethernet Phy.
*
* DESCRIPTION:
*       This function resets a given ethernet Phy.
*
* INPUT:
*       phyAddr - Phy address.
*       timeout - in millisec
*
* OUTPUT:
*       None.
*
* RETURN:   MV_OK       - Success
*           MV_TIMEOUT  - Timeout
*
*******************************************************************************/
MV_STATUS mvEthPhyReset(MV_U32 phyAddr, int timeout)
{
	MV_U16  phyRegData;

	/* Reset the PHY */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
		return MV_FAIL;

	/* Set bit 15 to reset the PHY */
	phyRegData |= ETH_PHY_CTRL_RESET_MASK;
	mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, phyRegData);

	/* Wait untill Reset completed */
	while (timeout > 0) {
		mvOsSleep(100);
		timeout -= 100;

		if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
			return MV_FAIL;

		if ((phyRegData & ETH_PHY_CTRL_RESET_MASK) == 0)
			return MV_OK;
	}
	return MV_TIMEOUT;
}


/*******************************************************************************
* mvEthPhyRestartAN - Restart ethernet Phy Auto-Negotiation.
*
* DESCRIPTION:
*       This function resets a given ethernet Phy.
*
* INPUT:
*       phyAddr - Phy address.
*       timeout - in millisec; 0 - no timeout (don't wait)
*
* OUTPUT:
*       None.
*
* RETURN:   MV_OK       - Success
*           MV_TIMEOUT  - Timeout
*
*******************************************************************************/
MV_STATUS mvEthPhyRestartAN(MV_U32 phyAddr, int timeout)
{
	MV_U16  phyRegData;

	/* Reset the PHY */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
		return MV_FAIL;

	/* Set bit 12 to Enable autonegotiation of the PHY */
	phyRegData |= ETH_PHY_CTRL_AN_ENABLE_MASK;

	/* Set bit 9 to Restart autonegotiation of the PHY */
	phyRegData |= ETH_PHY_CTRL_AN_RESTART_MASK;
	mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, phyRegData);

	if (timeout == 0)
		return MV_OK;

	/* Wait untill Auotonegotiation completed */
	while (timeout > 0) {
		mvOsSleep(100);
		timeout -= 100;

		if (mvEthPhyRegRead(phyAddr, ETH_PHY_STATUS_REG, &phyRegData) != MV_OK)
			return MV_FAIL;

		if (phyRegData & ETH_PHY_STATUS_AN_DONE_MASK)
			return MV_OK;
	}
	return MV_TIMEOUT;
}


/*******************************************************************************
* mvEthPhyDisableAN - Disable Phy Auto-Negotiation and set forced Speed and Duplex
*
* DESCRIPTION:
*       This function disable AN and set duplex and speed.
*
* INPUT:
*       phyAddr - Phy address.
*       speed   - port speed. 0 - 10 Mbps, 1-100 Mbps, 2 - 1000 Mbps
*       duplex  - port duplex. 0 - Half duplex, 1 - Full duplex
*
* OUTPUT:
*       None.
*
* RETURN:   MV_OK   - Success
*           MV_FAIL - Failure
*
*******************************************************************************/
MV_STATUS mvEthPhyDisableAN(MV_U32 phyAddr, int speed, int duplex)
{
	MV_U16  phyRegData;

	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &phyRegData) != MV_OK)
		return MV_FAIL;

	switch (speed) {
	case 0: /* 10 Mbps */
			phyRegData &= ~ETH_PHY_CTRL_SPEED_LSB_MASK;
			phyRegData &= ~ETH_PHY_CTRL_SPEED_MSB_MASK;
			break;

	case 1: /* 100 Mbps */
			phyRegData |= ETH_PHY_CTRL_SPEED_LSB_MASK;
			phyRegData &= ~ETH_PHY_CTRL_SPEED_MSB_MASK;
			break;

	case 2: /* 1000 Mbps */
			phyRegData &= ~ETH_PHY_CTRL_SPEED_LSB_MASK;
			phyRegData |= ETH_PHY_CTRL_SPEED_MSB_MASK;
			break;

	default:
			mvOsOutput("Unexpected speed = %d\n", speed);
			return MV_FAIL;
	}

	switch (duplex) {
	case 0: /* half duplex */
			phyRegData &= ~ETH_PHY_CTRL_DUPLEX_MASK;
			break;

	case 1: /* full duplex */
			phyRegData |= ETH_PHY_CTRL_DUPLEX_MASK;
			break;

	default:
			mvOsOutput("Unexpected duplex = %d\n", duplex);
	}
	/* Clear bit 12 to Disable autonegotiation of the PHY */
	phyRegData &= ~ETH_PHY_CTRL_AN_ENABLE_MASK;

	/* Clear bit 9 to DISABLE, Restart autonegotiation of the PHY */
	phyRegData &= ~ETH_PHY_CTRL_AN_RESTART_MASK;
	mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, phyRegData);

	return MV_OK;
}

MV_STATUS   mvEthPhyLoopback(MV_U32 phyAddr, MV_BOOL isEnable)
{
	MV_U16      regVal, ctrlVal;
	MV_STATUS   status;

	/* Set loopback speed and duplex accordingly with current */
	/* Bits: 6, 8, 13 */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &ctrlVal) != MV_OK)
		return MV_FAIL;

	if (isEnable) {
		/* Select page 2 */
		mvEthPhyRegWrite(phyAddr, 22, 2);

		mvEthPhyRegRead(phyAddr, 21, &regVal);
		regVal &= ~(ETH_PHY_CTRL_DUPLEX_MASK | ETH_PHY_CTRL_SPEED_LSB_MASK |
				ETH_PHY_CTRL_SPEED_MSB_MASK | ETH_PHY_CTRL_AN_ENABLE_MASK);
		regVal |= (ctrlVal & (ETH_PHY_CTRL_DUPLEX_MASK | ETH_PHY_CTRL_SPEED_LSB_MASK |
					ETH_PHY_CTRL_SPEED_MSB_MASK | ETH_PHY_CTRL_AN_ENABLE_MASK));
		mvEthPhyRegWrite(phyAddr, 21, regVal);

		/* Select page 0 */
		mvEthPhyRegWrite(phyAddr, 22, 0);

		/* Disable Energy detection   R16[9:8] = 00 */
		/* Disable MDI/MDIX crossover R16[6:5] = 00 */
		mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_CTRL_REG, &regVal);
		regVal &= ~(BIT5 | BIT6 | BIT8 | BIT9);
		mvEthPhyRegWrite(phyAddr, ETH_PHY_SPEC_CTRL_REG, regVal);

		status = mvEthPhyReset(phyAddr, 1000);
		if (status != MV_OK) {
			mvOsPrintf("mvEthPhyReset failed: status=0x%x\n", status);
			return status;
		}

		/* Set loopback */
		ctrlVal |= ETH_PHY_CTRL_LOOPBACK_MASK;
		mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, ctrlVal);
	} else {
		/* Cancel Loopback */
		ctrlVal &= ~ETH_PHY_CTRL_LOOPBACK_MASK;
		mvEthPhyRegWrite(phyAddr, ETH_PHY_CTRL_REG, ctrlVal);

		status = mvEthPhyReset(phyAddr, 1000);
		if (status != MV_OK) {
			mvOsPrintf("mvEthPhyReset failed: status=0x%x\n", status);
			return status;
		}

		/* Enable Energy detection   R16[9:8] = 11 */
		/* Enable MDI/MDIX crossover R16[6:5] = 11 */
		mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_CTRL_REG, &regVal);
		regVal |= (BIT5 | BIT6 | BIT8 | BIT9);
		mvEthPhyRegWrite(phyAddr, ETH_PHY_SPEC_CTRL_REG, regVal);

		status = mvEthPhyReset(phyAddr, 1000);
		if (status != MV_OK) {
			mvOsPrintf("mvEthPhyReset failed: status=0x%x\n", status);
			return status;
		}
	}

	return MV_OK;
}

/*******************************************************************************
* mvEthPhyCheckLink -
*
* DESCRIPTION:
*	check link in phy port
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:   MV_TRUE if link is up, MV_FALSE if down
*
*******************************************************************************/
MV_BOOL mvEthPhyCheckLink(MV_U32 phyAddr)
{
	MV_U16 val_st, val_ctrl, val_spec_st;

	/* read status reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_STATUS_REG, &val_st) != MV_OK)
		return MV_FALSE;

	/* read control reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &val_ctrl) != MV_OK)
		return MV_FALSE;

	/* read special status reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_STATUS_REG, &val_spec_st) != MV_OK)
		return MV_FALSE;

	/* Check for PHY exist */
	if ((val_ctrl == ETH_PHY_SMI_DATA_MASK) && (val_st & ETH_PHY_SMI_DATA_MASK))
		return MV_FALSE;


	if (val_ctrl & ETH_PHY_CTRL_AN_ENABLE_MASK) {
		if (val_st & ETH_PHY_STATUS_AN_DONE_MASK)
			return MV_TRUE;
		else
			return MV_FALSE;
	} else {
		if (val_spec_st & ETH_PHY_SPEC_STATUS_LINK_MASK)
			return MV_TRUE;
	}
	return MV_FALSE;
}

/*******************************************************************************
* mvEthPhyPrintStatus -
*
* DESCRIPTION:
*	print port Speed, Duplex, Auto-negotiation, Link.
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:   16bit phy register value, or 0xffff on error
*
*******************************************************************************/
MV_STATUS	mvEthPhyPrintStatus(MV_U32 phyAddr)
{
	MV_U16 val;

	/* read control reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_CTRL_REG, &val) != MV_OK)
		return MV_ERROR;

	if (val & ETH_PHY_CTRL_AN_ENABLE_MASK)
		mvOsOutput("Auto negotiation: Enabled\n");
	else
		mvOsOutput("Auto negotiation: Disabled\n");


	/* read specific status reg */
	if (mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_STATUS_REG, &val) != MV_OK)
		return MV_ERROR;

	switch (val & ETH_PHY_SPEC_STATUS_SPEED_MASK) {
	case ETH_PHY_SPEC_STATUS_SPEED_1000MBPS:
			mvOsOutput("Speed: 1000 Mbps\n");
			break;
	case ETH_PHY_SPEC_STATUS_SPEED_100MBPS:
			mvOsOutput("Speed: 100 Mbps\n");
			break;
	case ETH_PHY_SPEC_STATUS_SPEED_10MBPS:
			mvOsOutput("Speed: 10 Mbps\n");
			break;
	default:
			mvOsOutput("Speed: Uknown\n");
			break;

	}

	if (val & ETH_PHY_SPEC_STATUS_DUPLEX_MASK)
		mvOsOutput("Duplex: Full\n");
	else
		mvOsOutput("Duplex: Half\n");


	if (val & ETH_PHY_SPEC_STATUS_LINK_MASK)
		mvOsOutput("Link: up\n");
	else
		mvOsOutput("Link: down\n");

	return MV_OK;
}

MV_STATUS	mvEthPhyStatus( MV_U32 phyAddr, char *buf, unsigned len )
{
	MV_U16 val;

	snprintf(buf, len, "unknown");

	/* read control reg */ 
	if( mvEthPhyRegRead( phyAddr, ETH_PHY_CTRL_REG, &val) != MV_OK )
		return MV_ERROR;

#if 0
	if( val & ETH_PHY_CTRL_AN_ENABLE_MASK )
		mvOsOutput( "Auto negotiation: Enabled\n" );
	else
		mvOsOutput( "Auto negotiation: Disabled\n" );
#endif

	/* read specific status reg */ 
	if( mvEthPhyRegRead( phyAddr, ETH_PHY_SPEC_STATUS_REG, &val) != MV_OK )
		return MV_ERROR;

	switch (val & ETH_PHY_SPEC_STATUS_SPEED_MASK)
	{
		case ETH_PHY_SPEC_STATUS_SPEED_1000MBPS:
			snprintf(buf, len,  "1 Gb/s" );
			break;
		case ETH_PHY_SPEC_STATUS_SPEED_100MBPS:
			snprintf(buf, len,  "100 Mb/s" );
			break;
		case ETH_PHY_SPEC_STATUS_SPEED_10MBPS:
			snprintf(buf, len,  "10 Mb/s" );
			break;
		default:
			snprintf(buf, len, "speed unknown" );
			break;

	}

	// XXX strlcat()
	if( val & ETH_PHY_SPEC_STATUS_DUPLEX_MASK )
		strcat(buf, ", full duplex");
	else
		strcat(buf, ", half duplex");
 
#if 0
	if( val & ETH_PHY_SPEC_STATUS_LINK_MASK )
		mvOsOutput("Link: up\n");
	else
		mvOsOutput("Link: down\n");
#endif
	return MV_OK;
}

/*******************************************************************************
* mvEthE1111PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1111PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Phy recv and tx delay */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 20, &reg);
	reg |= BIT1 | BIT7;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 20, reg);

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 24, 0x4111);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);

	if (ethphyHalData.boardSpecInit == MV_TRUE)
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum],
				ethphyHalData.specRegOff , ethphyHalData.specData);

}

/*******************************************************************************
* mvEthE1112PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1112PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Set phy address */
	/*MV_REG_WRITE(ETH_PHY_ADDR_REG(ethPortNum), mvBoardPhyAddrGet(ethPortNum));*/

	/* Implement PHY errata */
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, 0x140);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, 0x8140);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 3);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, 0x103);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);

}

/*******************************************************************************
* mvEthE1116PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1116PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0x3);
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
	reg &= ~0xf;
	reg	|= 0x1;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0x0);

	/* Set RGMII delay */
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2);
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 21, &reg);
	reg	|= (BIT5 | BIT4);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 21, reg);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);
}


/*******************************************************************************
* mvEthE3016PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE3016PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Leds link and activity*/
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 22, &reg);
	reg &= ~0xf;
	reg	|= 0xa;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, reg);

	/* Set RGMII (RX) delay and copper mode */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 28, &reg);
	reg &= ~(BIT3 | BIT10 | BIT11);
	reg	|= (BIT10);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 28, reg);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);
}


/*******************************************************************************
* mvEthE1011PhyBasicInit -
*
* DESCRIPTION:
*	Do a basic Init to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID		mvEthE1011PhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 reg;

	/* Phy recv and tx delay */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 20, &reg);
	reg &= ~(BIT1 | BIT7);
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 20, reg);

	/* Leds link and activity*/
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 24, 0x4111);

	/* reset the phy */
	mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 0, &reg);
	reg |= BIT15;
	mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 0, reg);

}

/*******************************************************************************
* mvEthE1112PhyPowerDown -
*
* DESCRIPTION:
*	Power down the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID	mvEthE1112PhyPowerDown(MV_U32 ethPortNum)
{
	mvEthPhyPower(ethPortNum, MV_FALSE);
}

/*******************************************************************************
* mvEthE1112PhyPowerUp -
*
* DESCRIPTION:
*	Power up the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
MV_VOID	mvEthE1112PhyPowerUp(MV_U32 ethPortNum)
{
	mvEthPhyPower(ethPortNum, MV_TRUE);
}

/*******************************************************************************
* mvEthPhyPower -
*
* DESCRIPTION:
*	Do a basic power down/up to the Phy , including reset
*
* INPUT:
*       ethPortNum - Ethernet port number
*	enable - MV_TRUE - power up
*		 MV_FALSE - power down
*
* OUTPUT:
*       None.
*
* RETURN:   None
*
*******************************************************************************/
static MV_VOID	mvEthPhyPower(MV_U32 ethPortNum, MV_BOOL enable)
{
	MV_U16 reg;
	if (enable == MV_FALSE) {
	/* Power down command */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2); 		/* select page 2 */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
		reg |= BIT3;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);		/* select to disable the SERDES */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0); 		/* select page 0 */

		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 3);		/* Power off LED's */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, 0x88);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg |= ETH_PHY_CTRL_RESET_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* software reset */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg |= ETH_PHY_CTRL_POWER_DOWN_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* power down the PHY */
	} else {
	/* Power up command */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 2); 		/* select page 2 */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], 16, &reg);
		reg &= ~BIT3;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, reg);		/* select to enable the SERDES */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0); 		/* select page 0 */

		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 3);		/* Power on LED's */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 16, 0x03);
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], 22, 0);

		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg |= ETH_PHY_CTRL_RESET_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* software reset */
		mvEthPhyRegRead(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, &reg);
		reg &= ~ETH_PHY_CTRL_POWER_DOWN_MASK;
		mvEthPhyRegWrite(ethphyHalData.phyAddr[ethPortNum], ETH_PHY_CTRL_REG, reg);	/* power up the PHY */
	}
}


/*******************************************************************************
* mvEth1145PhyInit - Initialize MARVELL 1145 Phy
*
* DESCRIPTION:
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvEth1145PhyBasicInit(MV_U32 port)
{
    MV_U16 value;

    /* Set Link1000 output pin to be link indication, set Tx output pin to be activity */
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x18, ETH_PHY_LED_ACT_LNK_DV);
    mvOsDelay(10);

	    /* Add delay to RGMII Tx and Rx */
    mvEthPhyRegRead(ethphyHalData.phyAddr[port], 0x14, &value);
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x14, (value | BIT1 | BIT7));
    mvOsDelay(10);
#if 0 /* Fix by yotam */
    if (boardId != RD_78XX0_AMC_ID &&
	    boardId != RD_78XX0_H3C_ID) {
	    /* Set port 2 - Phy addr 9 to RGMII */
	if (port == 2) {
		mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x1b, 0x808b);
		mvOsDelay(10);
	}

	/* Set port 1 - Phy addr a to SGMII */
	if (port == 1) {
	    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x1b, 0x8084);
	    mvOsDelay(10);

		/* Reset Phy */
	    mvEthPhyRegRead(ethphyHalData.phyAddr[port], 0x00, &value);
	    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x00, (value | BIT15));
	    mvOsDelay(10);
	#if defined(SGMII_OUTBAND_AN)
		/* Set port 1 - Phy addr A Page 1 */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x16, 0x1);
		mvOsDelay(10);

		/* Set port 1 - Phy addr A disable A.N. */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x0, 0x140);
		mvOsDelay(10);

		/* Set port 1 - Phy addr A reset */
		mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x0, 0x8140);
		mvOsDelay(10);

		mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x16, 0x0);
		mvOsDelay(10);
	#endif
	}
    }
#endif

	    /* Set Phy TPVL to 0 */
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x10, 0x60);
    mvOsDelay(10);

    /* Reset Phy */
    mvEthPhyRegRead(ethphyHalData.phyAddr[port], 0x00, &value);
    mvEthPhyRegWrite(ethphyHalData.phyAddr[port], 0x00, (value | BIT15));
    mvOsDelay(10);

    return;
}


/*******************************************************************************
* mvEthSgmiiToCopperPhyInit - Initialize Test board 1112 Phy
*
* DESCRIPTION:
*
* INPUT:
*       phyAddr - Phy address.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvEthSgmiiToCopperPhyBasicInit(MV_U32 port)
{
    MV_U16 value;
    MV_U16 phyAddr = 0xC;

   /* Port 0 phyAdd c */
   /* Port 1 phyAdd d */
    mvEthPhyRegWrite(phyAddr + port, 22, 3);
    mvEthPhyRegWrite(phyAddr + port, 16, 0x103);
    mvEthPhyRegWrite(phyAddr + port, 22, 0);

		/* reset the phy */
    mvEthPhyRegRead(phyAddr + port, 0, &value);
    value |= BIT15;
    mvEthPhyRegWrite(phyAddr + port, 0, value);
}


MV_VOID mvEth1121PhyBasicInit(MV_U32 port)
{
	MV_U16 value;
	MV_U16 phyAddr = ethphyHalData.phyAddr[port];

	/* Change page select to 2 */
	value = 2;
	mvEthPhyRegWrite(phyAddr, 22, value);
	mvOsDelay(10);

	/* Set RGMII rx delay */
	mvEthPhyRegRead(phyAddr, 21, &value);
	value |= BIT5;
	mvEthPhyRegWrite(phyAddr, 21, value);
	mvOsDelay(10);

	/* Change page select to 0 */
	value = 0;
	mvEthPhyRegWrite(phyAddr, 22, value);
	mvOsDelay(10);

	/* reset the phy */
	mvEthPhyRegRead(phyAddr, 0, &value);
	value |= BIT15;
	mvEthPhyRegWrite(phyAddr, 0, value);
	mvOsDelay(10);
}
MV_VOID mvEthInternalGEPhyBasicInit(MV_U32 ethPortNum)
{
	MV_U16 value;
	MV_U16 phyAddr = ethphyHalData.phyAddr[ethPortNum];

	mvEthPhyRegWrite(phyAddr, 0x16, 0x00FF);
	mvEthPhyRegWrite(phyAddr, 0x11, 0x0FD0);
	mvEthPhyRegWrite(phyAddr, 0x10, 0x214C);
	mvEthPhyRegWrite(phyAddr, 0x11, 0x0000);
	mvEthPhyRegWrite(phyAddr, 0x10, 0x2000);
	mvEthPhyRegWrite(phyAddr, 0x11, 0x0F16);
	mvEthPhyRegWrite(phyAddr, 0x10, 0x2146);
	mvEthPhyRegWrite(phyAddr, 0x16, 0x0);

	/* reset the phy */
	mvEthPhyRegRead(phyAddr, 0, &value);
	value |= BIT15;
	mvEthPhyRegWrite(phyAddr, 0, value);
	mvOsDelay(10);

	return;
}

unsigned pmap[4] = { 0, 0x800, 0x1000, 0x1800 };

static unsigned
aruba_linkcrypt_read(int port, unsigned reg)
{
	MV_U16 dlow, dhigh;

	reg |= pmap[port];
	/* enable linkcrypt page */
	mvEthPhyRegWrite(port, 0x16, 0x10);

	/* read from 'reg' */
	mvEthPhyRegWrite(port, 0x0, reg);

	mvEthPhyRegRead(port, 0x2, &dlow);
	mvEthPhyRegRead(port, 0x3, &dhigh);

	/* restore to page 0 */
	mvEthPhyRegWrite(port, 0x16, 0x0);

	return (dhigh << 16) | dlow;
}

static void
aruba_linkcrypt_write(int port, unsigned reg, unsigned val)
{
	MV_U16 dlow, dhigh;

	reg |= pmap[port];

	dhigh = val >> 16;
	dhigh &= 0xffff;

	dlow = val & 0xffff;

	/* enable linkcrypt page */
	mvEthPhyRegWrite(port, 0x16, 0x10);

	/* write to 'reg' */
	mvEthPhyRegWrite(port, 0x1, reg);

	mvEthPhyRegWrite(port, 0x2, dlow);
	mvEthPhyRegWrite(port, 0x3, dhigh);

	/* restore to page 0 */
	mvEthPhyRegWrite(port, 0x16, 0x0);
}

unsigned
aruba_xmdio_read(int port, int dev, unsigned reg)
{
	MV_U16 val;

	/* set to page 0 */
	mvEthPhyRegWrite(port, 0x16, 0x0);

	/* program device and reg */
	mvEthPhyRegWrite(port, 0xd, dev);
	mvEthPhyRegWrite(port, 0xe, reg);

	/* indicate data access required */
	mvEthPhyRegWrite(port, 0xd, dev | 0x4000);

	/* read value */
	mvEthPhyRegRead(port, 0xe, &val);

	return val;
}

void
aruba_xmdio_write(int port, int dev, unsigned reg, unsigned val)
{
	/* set to page 0 */
	mvEthPhyRegWrite(port, 0x16, 0x0);

	/* program device and reg */
	mvEthPhyRegWrite(port, 0xd, dev);
	mvEthPhyRegWrite(port, 0xe, reg);

	/* indicate data access required */
	mvEthPhyRegWrite(port, 0xd, dev | 0x4000);

	/* write value */
	mvEthPhyRegWrite(port, 0xe, val);
}

#define Y0	0x0eb00100
#define A0	0x0eb00200
#define A1	0x0eb10200

/*
 * We used to just read the revision. However, for A1, the revision is
 * still the same, but the PHY ID register has had its revision field
 * incremented. Fortunately, the basic ID is the same, so Linux will
 * not be affected.  Since the init script is supposed to be skipped for
 * A1, we need to distinguish A0 and A1, which we now do by combining
 * the ID register value with the revision register value
 */
static void
mv1540_get_version(int addr, unsigned *v)
{
	*v = 0;
	MV_U16 val;
	MV_U16 id;

	/* register 31_251.15:0 has the revision */
	mvEthPhyRegWrite(addr, 0x16, 0xfb);

	mvEthPhyRegRead(addr, 0x1f, &val);

	mvEthPhyRegWrite(addr, 0x16, 0x0000);

	/* read ID */
	mvEthPhyRegRead(addr, 0x3, &id);

	*v = ((unsigned)id << 16) | val;

}

MV_VOID mvEth1540PhyBasicInit(MV_BOOL eeeEnable)
{
	int i;
	MV_U16 reg;
	int startAddr, endAddr;
	unsigned val, version1540;

	startAddr = ethphyHalData.quadPhyStartAddr;
	endAddr = startAddr + 4;

#ifdef CONFIG_APBOOT
	printf("PHY:   ");
#endif
	mv1540_get_version(startAddr, &version1540);

	for (i = startAddr; i < endAddr; i++) {
		/* Enable QSGMII AN */
		/* Set page to 4. */
		mvEthPhyRegWrite(i, 0x16, 4);
		/* Enable AN */
		mvEthPhyRegWrite(i, 0x0, 0x1140);
		/* Set page to 0. */
		mvEthPhyRegWrite(i, 0x16, 0);

		/* Power up the phy */
		mvEthPhyRegRead(i, ETH_PHY_CTRL_REG, &reg);
		reg &= ~(ETH_PHY_CTRL_POWER_DOWN_MASK);
		mvEthPhyRegWrite(i, ETH_PHY_CTRL_REG, reg);
		mvOsDelay(100);
		switch (version1540) {
		case Y0:
			if (eeeEnable) {
				/* set ELU#0 default match */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x10);
				/* RW U1 P0 R1 H0104 */
				mvEthPhyRegWrite(i, 0x1, 0x0104);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R1 H0904 */
				mvEthPhyRegWrite(i, 0x1, 0x0904);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1104 */
				mvEthPhyRegWrite(i, 0x1, 0x1104);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1904 */
				mvEthPhyRegWrite(i, 0x1, 0x1904);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* set ILU#0 default match */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0207 */
				mvEthPhyRegWrite(i, 0x1, 0x0207);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R1 H0A07 */
				mvEthPhyRegWrite(i, 0x1, 0x0a07);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1207 */
				mvEthPhyRegWrite(i, 0x1, 0x1207);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1A07 */
				mvEthPhyRegWrite(i, 0x1, 0x1a07);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* change the wiremac ipg from 12 to 11 */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0041 */
				mvEthPhyRegWrite(i, 0x1, 0x0041);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R1 H0841 */
				mvEthPhyRegWrite(i, 0x1, 0x0841);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1041 */
				mvEthPhyRegWrite(i, 0x1, 0x1041);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1841 */
				mvEthPhyRegWrite(i, 0x1, 0x1841);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);


				/* change the setting to not drop badtag */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H000b */
				mvEthPhyRegWrite(i, 0x1, 0x000b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H080b */
				mvEthPhyRegWrite(i, 0x1, 0x080b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H100b */
				mvEthPhyRegWrite(i, 0x1, 0x100b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H180b */
				mvEthPhyRegWrite(i, 0x1, 0x180b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* set MACSec EEE Entry/Exit Timer */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H03C0 */
				mvEthPhyRegWrite(i, 0x1, 0x03c0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R1 H0BC0 */
				mvEthPhyRegWrite(i, 0x1, 0x0bc0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R1 H13C0 */
				mvEthPhyRegWrite(i, 0x1, 0x13c0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R1 H1BC0 */
				mvEthPhyRegWrite(i, 0x1, 0x1bc0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* Start of EEE Workaround */
				/* RW U1 P0-3 R22 H00FB */
				mvEthPhyRegWrite(i, 0x16, 0x00FB);
				/* RW U1 P0-3 R11 H1120 */
				mvEthPhyRegWrite(i, 0xB , 0x1120);
				/* RW U1 P0-3 R8  H3666 */
				mvEthPhyRegWrite(i, 0x8 , 0x3666);
				/* RW U1 P0-3 R22 H00FF */
				mvEthPhyRegWrite(i, 0x16, 0x00FF);
				/* RW U1 P0-3 R17 H0F0C */
				mvEthPhyRegWrite(i, 0x11, 0x0F0C);
				/* RW U1 P0-3 R16 H2146 */
				mvEthPhyRegWrite(i, 0x10, 0x2146);
				/* RW U1 P0-3 R17 Hc090 */
				mvEthPhyRegWrite(i, 0x11, 0xC090);
				/* RW U1 P0-3 R16 H2147 */
				mvEthPhyRegWrite(i, 0x10, 0x2147);
				/* RW U1 P0-3 R17 H0000 */
				mvEthPhyRegWrite(i, 0x11, 0x0000);
				/* RW U1 P0-3 R16 H2000 */
				mvEthPhyRegWrite(i, 0x10, 0x2000);
				/* RW U1 P0-3 R17 H6000 */
				mvEthPhyRegWrite(i, 0x11, 0x6000);
				/* RW U1 P0-3 R16 H2143 */
				mvEthPhyRegWrite(i, 0x10, 0x2143);
				/* RW U1 P0-3 R17 HC004 */
				mvEthPhyRegWrite(i, 0x11, 0xC004);
				/* RW U1 P0-3 R16 H2100 */
				mvEthPhyRegWrite(i, 0x10, 0x2100);
				/* RW U1 P0-3 R17 H49E8 */
				mvEthPhyRegWrite(i, 0x11, 0x49E8);
				/* RW U1 P0-3 R16 H2144 */
				mvEthPhyRegWrite(i, 0x10, 0x2144);
				/* RW U1 P0-3 R17 H3180 */
				mvEthPhyRegWrite(i, 0x11, 0x3180);
				/* RW U1 P0-3 R16 H2148 */
				mvEthPhyRegWrite(i, 0x10, 0x2148);
				/* RW U1 P0-3 R17 HFC44 */
				mvEthPhyRegWrite(i, 0x11, 0xFC44);
				/* RW U1 P0-3 R16 H214B */
				mvEthPhyRegWrite(i, 0x10, 0x214B);
				/* RW U1 P0-3 R17 H7FD2 */
				mvEthPhyRegWrite(i, 0x11, 0x7FD2);
				/* RW U1 P0-3 R16 H214C */
				mvEthPhyRegWrite(i, 0x10, 0x214C);
				/* RW U1 P0-3 R17 H2240 */
				mvEthPhyRegWrite(i, 0x11, 0x2240);
				/* RW U1 P0-3 R16 H214D */
				mvEthPhyRegWrite(i, 0x10, 0x214D);
				/* RW U1 P0-3 R17 H3008 */
				mvEthPhyRegWrite(i, 0x11, 0x3008);
				/* RW U1 P0-3 R16 H214E */
				mvEthPhyRegWrite(i, 0x10, 0x214E);
				/* RW U1 P0-3 R17 H3DF0 */
				mvEthPhyRegWrite(i, 0x11, 0x3DF0);
				/* RW U1 P0-3 R16 H214F */
				mvEthPhyRegWrite(i, 0x10, 0x214F);
				/* RW U1 P0-3 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0);

				/* Enable EEE Auto-neg advertisement on P0-P7 ports */
				/* RW U1 P0-3 R13 H0007 */
				mvEthPhyRegWrite(i, 0xD , 0x0007);
				/* RW U1 P0-3 R14 H003C */
				mvEthPhyRegWrite(i, 0xE , 0x003C);
				/* RW U1 P0-3 R13 H4007 */
				mvEthPhyRegWrite(i, 0xD , 0x4007);
				/* RW U1 P0-3 R14 H0006 */
				mvEthPhyRegWrite(i, 0xE , 0x0006);

				/* Soft-Reset on P0-P7 ports */
				/* RW U1 P0-3 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0);
				/* RW U1 P0-3 R0  H9140 */
				mvEthPhyRegWrite(i, 0x0 , 0x9140);

				/* Enable MACsec EEE Master Mode on P0-3 ports */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1  H03C1 */
				mvEthPhyRegWrite(i, 0x1 , 0x03C1);
				/* RW U1 P0 R2  H0001 */
				mvEthPhyRegWrite(i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				mvEthPhyRegWrite(i, 0x3 , 0x0000);
				/* RW U1 P0 R1  H0BC1 */
				mvEthPhyRegWrite(i, 0x1 , 0x0BC1);
				/* RW U1 P0 R2  H0001 */
				mvEthPhyRegWrite(i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				mvEthPhyRegWrite(i, 0x3 , 0x0000);
				/* RW U1 P0 R1  H13C1 */
				mvEthPhyRegWrite(i, 0x1 , 0x13C1);
				/* RW U1 P0 R2  H0001 */
				mvEthPhyRegWrite(i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				mvEthPhyRegWrite(i, 0x3 , 0x0000);
				/* RW U1 P0 R1  H1BC1 */
				mvEthPhyRegWrite(i, 0x1 , 0x1BC1);
				/* RW U1 P0 R2  H0001 */
				mvEthPhyRegWrite(i, 0x2 , 0x0001);
				/* RW U1 P0 R3  H0000 */
				mvEthPhyRegWrite(i, 0x3 , 0x0000);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);
			} else {
				/* set ELU#0 default match  */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0104 */
				mvEthPhyRegWrite(i, 0x1, 0x0104);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R1 H0904 */
				mvEthPhyRegWrite(i, 0x1, 0x0904);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1104 */
				mvEthPhyRegWrite(i, 0x1, 0x1104);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R1 H1904 */
				mvEthPhyRegWrite(i, 0x1, 0x1904);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H4000 */
				mvEthPhyRegWrite(i, 0x3, 0x4000);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* set ILU#0 default match */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0207 */
				mvEthPhyRegWrite(i, 0x1, 0x0207);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R1 H0A07 */
				mvEthPhyRegWrite(i, 0x1, 0x0a07);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1207 */
				mvEthPhyRegWrite(i, 0x1, 0x1207);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R1 H1A07 */
				mvEthPhyRegWrite(i, 0x1, 0x1a07);
				/* RW U1 P0 R2 h4000 */
				mvEthPhyRegWrite(i, 0x2, 0x4000);
				/* RW U1 P0 R3 H0000 */
				mvEthPhyRegWrite(i, 0x3, 0x0000);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* change the wiremac ipg from 12 to 11 */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H0041 */
				mvEthPhyRegWrite(i, 0x1, 0x0041);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R1 H0841 */
				mvEthPhyRegWrite(i, 0x1, 0x0841);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1041 */
				mvEthPhyRegWrite(i, 0x1, 0x1041);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R1 H1841 */
				mvEthPhyRegWrite(i, 0x1, 0x1841);
				/* RW U1 P0 R2 h00b1 */
				mvEthPhyRegWrite(i, 0x2, 0x00b1);
				/* RW U1 P0 R3 H0002 */
				mvEthPhyRegWrite(i, 0x3, 0x0002);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* change the wiremac max size to 2048 */
				val = aruba_linkcrypt_read(i, 0x40);
				val &= ~0xfffc; /* 0x3fff << 2 */
				val |= (2048 << 2);
				aruba_linkcrypt_write(i, 0x40, val);

				/* change the system mac max size to 2048 */
				val = aruba_linkcrypt_read(i, 0x50);
				val &= ~0xfffc; /* 0x3fff << 2 */
				val |= (2048 << 2);
				aruba_linkcrypt_write(i, 0x50, val);

				/* change the setting to not drop badtag */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H000b */
				mvEthPhyRegWrite(i, 0x1, 0x000b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H080b */
				mvEthPhyRegWrite(i, 0x1, 0x080b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H100b */
				mvEthPhyRegWrite(i, 0x1, 0x100b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R1 H180b */
				mvEthPhyRegWrite(i, 0x1, 0x180b);
				/* RW U1 P0 R2 h0000 */
				mvEthPhyRegWrite(i, 0x2, 0x0000);
				/* RW U1 P0 R3 H0fb4 */
				mvEthPhyRegWrite(i, 0x3, 0x0fb4);
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				/* set MACSec EEE Entry/Exit Timer */
				/* RW U1 P0 R22 H0010 */
				mvEthPhyRegWrite(i, 0x16, 0x0010);
				/* RW U1 P0 R1 H03C0 */
				mvEthPhyRegWrite(i, 0x1, 0x03c0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R1 H0BC0 */
				mvEthPhyRegWrite(i, 0x1, 0x0bc0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R1 H13C0 */
				mvEthPhyRegWrite(i, 0x1, 0x13c0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/* RW U1 P0 R1 H1BC0 */
				mvEthPhyRegWrite(i, 0x1, 0x1bc0);
				/* RW U1 P0 R2 H111E */
				mvEthPhyRegWrite(i, 0x2, 0x111e);
				/* RW U1 P0 R3 H111E */
				mvEthPhyRegWrite(i, 0x3, 0x111e);
				/*  RW U1 P0 R22 H0000 */
				/*  RW U1 P0-3 R22 H00FB */
				/*  RW U1 P0-3 R11 H1120 */
				/*  RW U1 P0-3 R8  H3666 */
				/*  RW U1 P0-3 R22 H00FF */
				/*  RW U1 P0-3 R17 H0F0C */
				/*  RW U1 P0-3 R16 H2146 */
				/*  RW U1 P0-3 R17 Hc090 */
				/*  RW U1 P0-3 R16 H2147 */
				/*  RW U1 P0-3 R17 H0000 */
				/*  RW U1 P0-3 R16 H2000 */
				/*  RW U1 P0-3 R17 H6000 */
				/*  RW U1 P0-3 R16 H2143 */
				/*  RW U1 P0-3 R17 HC004 */
				/*  RW U1 P0-3 R16 H2100 */
				/*  RW U1 P0-3 R17 H49E8 */
				/*  RW U1 P0-3 R16 H2144 */
				/*  RW U1 P0-3 R17 H3180 */
				/*  RW U1 P0-3 R16 H2148 */
				/*  RW U1 P0-3 R17 HFC44 */
				/*  RW U1 P0-3 R16 H214B */
				/*  RW U1 P0-3 R17 H7FD2 */
				/*  RW U1 P0-3 R16 H214C */
				/*  RW U1 P0-3 R17 H2240 */
				/*  RW U1 P0-3 R16 H214D */
				/*  RW U1 P0-3 R17 H3008 */
				/*  RW U1 P0-3 R16 H214E */
				/*  RW U1 P0-3 R17 H3DF0 */
				/*  RW U1 P0-3 R16 H214F */
				/*  RW U1 P0-3 R22 H0000 */
				/*  RW U1 P0-3 R13 H0007 */
				/*  RW U1 P0-3 R14 H003C */
				/*  RW U1 P0-3 R13 H4007 */
				/*  RW U1 P0-3 R14 H0006 */
				/*  RW U1 P0-3 R22 H0000 */
				/*  RW U1 P0-3 R0  H9140 */
				/*  RW U1 P0 R22 H0010 */
				/*  RW U1 P0 R1  H03C1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/*  RW U1 P0 R1  H0BC1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/*  RW U1 P0 R1  H13C1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/*  RW U1 P0 R1  H1BC1 */
				/*  RW U1 P0 R2  H0001 */
				/*  RW U1 P0 R3  H0000 */
				/* RW U1 P0 R22 H0000 */
				mvEthPhyRegWrite(i, 0x16, 0x0000);

				if (eeeEnable == MV_FALSE) {
					/* Disable MACSec */
					mvEthPhyRegWrite(i, 0x16, 0x12);
					mvEthPhyRegRead(i, 27, &reg);
					reg &= ~(1 << 13);
					mvEthPhyRegWrite(i, 27, reg);
				}
				mvEthPhyRegWrite(i, 0x16, 0x0000);
			}
			break;
		case A0:
			/* This script applies to 88E15XX MACSec Rev A0 PHY Initialization */
			/* Date: September 29, 2010 */
			/* This script is used as example to perform phy and macsec initialization for Port 0-3 */
			/* - MACSec Initialization */
			/* - PHY Initialization  */
			/* For ARUBA ONLY THIS IS VALID ONLY FOR A0 REVISION */
			/* RW u1 P0-3 R22 H0000 */
			mvEthPhyRegWrite(i, 0x16, 0x0000);
			/* --------------------------------------- */
			/* MACSec Initialization */
			/* --------------------------------------- */
			/* Disable drop_bad_tag bit for Port 0-3 */
			/* RW u1 P0 R22 H0010 */
			mvEthPhyRegWrite(i, 0x16, 0x0010);
			/* RW u1 P0 R1 H000b */
			mvEthPhyRegWrite(i, 0x1, 0x000b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H080b */
			mvEthPhyRegWrite(i, 0x1, 0x080b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H100b */
			mvEthPhyRegWrite(i, 0x1, 0x100b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H180b */
			mvEthPhyRegWrite(i, 0x1, 0x180b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R22 H0000 */
			mvEthPhyRegWrite(i, 0x16, 0x0000);
			/* --------------------------------------- */
			/* PHY Initialization */
			/* --------------------------------------- */
			/* RW u1 P0-3 R22 h00FA */
			mvEthPhyRegWrite(i, 0x16, 0x00fa);
			/* RW u1 P0-3 R8 h0010 */
			mvEthPhyRegWrite(i, 0x8, 0x0010);
			/* RW u1 P0-3 R22 h00FB */
			mvEthPhyRegWrite(i, 0x16, 0x00fb);
			/* RW u1 P0-3 R1 h4099 */
			mvEthPhyRegWrite(i, 0x1, 0x4099);
			/* RW u1 P0-3 R3 h1120 */
			mvEthPhyRegWrite(i, 0x3, 0x1120);
			/* RW u1 P0-3 R11 h113C */
			mvEthPhyRegWrite(i, 0xb, 0x113c);
			/* RW u1 P0-3 R14 h8100 */
			mvEthPhyRegWrite(i, 0xe, 0x8100);
			/* RW u1 P0-3 R15 h112A */
			mvEthPhyRegWrite(i, 0xf, 0x112a);
			/* RW u1 P0-3 R22 h00FC */
			mvEthPhyRegWrite(i, 0x16, 0x00fc);
			/* RW u1 P0-3 R1 h20B0 */
			mvEthPhyRegWrite(i, 0x1, 0x20b0);
			/* RW u1 P0-3 R22 h00FF	 */
			mvEthPhyRegWrite(i, 0x16, 0x00ff);
			/* RW u1 P0-3 R17 h0000 */
			mvEthPhyRegWrite(i, 0x11, 0x0000);
			/* RW u1 P0-3 R16 h2000 */
			mvEthPhyRegWrite(i, 0x10, 0x2000);
			/* RW u1 P0-3 R17 h4444 */
			mvEthPhyRegWrite(i, 0x11, 0x4444);
			/* RW u1 P0-3 R16 h2140 */
			mvEthPhyRegWrite(i, 0x10, 0x2140);
			/* RW u1 P0-3 R17 h8064 */
			mvEthPhyRegWrite(i, 0x11, 0x8064);
			/* RW u1 P0-3 R16 h2141 */
			mvEthPhyRegWrite(i, 0x10, 0x2141);
			/* RW u1 P0-3 R17 h0108 */
			mvEthPhyRegWrite(i, 0x11, 0x0108);
			/* RW u1 P0-3 R16 h2144 */
			mvEthPhyRegWrite(i, 0x10, 0x2144);
			/* RW u1 P0-3 R17 h0F16 */
			mvEthPhyRegWrite(i, 0x11, 0x0f16);
			/* RW u1 P0-3 R16 h2146 */
			mvEthPhyRegWrite(i, 0x10, 0x2146);
			/* RW u1 P0-3 R17 h8C44 */
			mvEthPhyRegWrite(i, 0x11, 0x8c44);
			/* RW u1 P0-3 R16 h214B */
			mvEthPhyRegWrite(i, 0x10, 0x214b);
			/* RW u1 P0-3 R17 h0F90 */
			mvEthPhyRegWrite(i, 0x11, 0x0f90);
			/* RW u1 P0-3 R16 h214C */
			mvEthPhyRegWrite(i, 0x10, 0x214c);
			/* RW u1 P0-3 R17 hBA33 */
			mvEthPhyRegWrite(i, 0x11, 0xba33);
			/* RW u1 P0-3 R16 h214D */
			mvEthPhyRegWrite(i, 0x10, 0x214d);
			/* RW u1 P0-3 R17 h39AA */
			mvEthPhyRegWrite(i, 0x11, 0x39aa);
			/* RW u1 P0-3 R16 h214F */
			mvEthPhyRegWrite(i, 0x10, 0x214f);
			/* RW u1 P0-3 R17 h8433 */
			mvEthPhyRegWrite(i, 0x11, 0x8433);
			/* RW u1 P0-3 R16 h2151 */
			mvEthPhyRegWrite(i, 0x10, 0x2151);
			/* RW u1 P0-3 R17 h2010 */
			mvEthPhyRegWrite(i, 0x11, 0x2010);
			/* RW u1 P0-3 R16 h2152 */
			mvEthPhyRegWrite(i, 0x10, 0x2152);
			/* RW u1 P0-3 R17 h99EB */
			mvEthPhyRegWrite(i, 0x11, 0x99eb);
			/* RW u1 P0-3 R16 h2153 */
			mvEthPhyRegWrite(i, 0x10, 0x2153);
			/* RW u1 P0-3 R17 h2f3B */
			mvEthPhyRegWrite(i, 0x11, 0x2f3b);
			/* RW u1 P0-3 R16 h2154 */
			mvEthPhyRegWrite(i, 0x10, 0x2154);
			/* RW u1 P0-3 R17 h584E */
			mvEthPhyRegWrite(i, 0x11, 0x584e);
			/* RW u1 P0-3 R16 h2156 */
			mvEthPhyRegWrite(i, 0x10, 0x2156);
			/* RW u1 P0-3 R17 h1223 */
			mvEthPhyRegWrite(i, 0x11, 0x1223);
			/* RW u1 P0-3 R16 h2158 */
			mvEthPhyRegWrite(i, 0x10, 0x2158);
			/* RW u1 P0-3 R22 h0000 */
			mvEthPhyRegWrite(i, 0x16, 0x0000);

			/* change the wiremac max size to 2048 */
			val = aruba_linkcrypt_read(i, 0x40);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (2048 << 2);
			aruba_linkcrypt_write(i, 0x40, val);

			/* change the system mac max size to 2048 */
			val = aruba_linkcrypt_read(i, 0x50);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (2048 << 2);
			aruba_linkcrypt_write(i, 0x50, val);
#if 1
			/* per Marvell, this is needed for EEE (802.1az) */
			/* --------------------------------------- */
			/* Soft-Reset */
			/* --------------------------------------- */
			/* RW u1 P0-3 R22 H0000 */
			mvEthPhyRegWrite(i, 0x16, 0x0000);
			/* RW u1 P0-3 R0 H9140 */
			mvEthPhyRegWrite(i, 0x0, 0x9140);
			mdelay(10);
#endif
			break;
		case A1:
		default:
			/* --------------------------------------- */
			/* MACSec Initialization */
			/* --------------------------------------- */
			/* Disable drop_bad_tag bit for Port 0-3 */
			/* RW u1 P0 R22 H0010 */
			mvEthPhyRegWrite(i, 0x16, 0x0010);
			/* RW u1 P0 R1 H000b */
			mvEthPhyRegWrite(i, 0x1, 0x000b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H080b */
			mvEthPhyRegWrite(i, 0x1, 0x080b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H100b */
			mvEthPhyRegWrite(i, 0x1, 0x100b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R1 H180b */
			mvEthPhyRegWrite(i, 0x1, 0x180b);
			/* RW u1 P0 R2 h0000 */
			mvEthPhyRegWrite(i, 0x2, 0x0000);
			/* RW u1 P0 R3 H0fb4 */
			mvEthPhyRegWrite(i, 0x3, 0x0fb4);
			/* RW u1 P0 R22 H0000 */
			mvEthPhyRegWrite(i, 0x16, 0x0000);

			/* from rev A1 release notes dated 2012/07/11 */
			mvEthPhyRegWrite(i, 0x16, 0x00ff);
			mvEthPhyRegWrite(i, 0x11, 0x2148);
			mvEthPhyRegWrite(i, 0x10, 0x2144);
			mvEthPhyRegWrite(i, 0x11, 0xdc0c);
			mvEthPhyRegWrite(i, 0x10, 0x2159);

			/* erratum 3.5; ensure that QSGMII auto-neg is on */
			mvEthPhyRegWrite(i, 0x16, 0x0004);
			mvEthPhyRegWrite(i, 0x00, 0x9140);
			mvEthPhyRegWrite(i, 0x16, 0x0000);

			/* change the wiremac max size to 2048 */
			val = aruba_linkcrypt_read(i, 0x40);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (2048 << 2);
			aruba_linkcrypt_write(i, 0x40, val);

			/* change the system mac max size to 2048 */
			val = aruba_linkcrypt_read(i, 0x50);
			val &= ~0xfffc; /* 0x3fff << 2 */
			val |= (2048 << 2);
			aruba_linkcrypt_write(i, 0x50, val);
#if 1
			/* per Marvell, this is needed for EEE (802.1az) */
			/* --------------------------------------- */
			/* Soft-Reset */
			/* --------------------------------------- */
			/* RW u1 P0-3 R22 H0000 */
			mvEthPhyRegWrite(i, 0x16, 0x0000);
			/* RW u1 P0-3 R0 H9140 */
			mvEthPhyRegWrite(i, 0x0, 0x9140);
			mdelay(10);
#endif
			break;
		}
#ifdef CONFIG_APBOOT
		printf(".");
#endif
	}
	/*  Sleep 3000 */
	mvOsDelay(300);
#ifdef CONFIG_APBOOT
	printf("\b\b\b\bdone\n");
#endif

}

MV_VOID mvEth1340PhyBasicInit(void)
{
	int i;
	MV_U16 reg;
	int startAddr, endAddr;

	startAddr = ethphyHalData.quadPhyStartAddr;
	endAddr = startAddr + 4;

	for (i = startAddr; i < endAddr; i++) {
		mvEthPhyRegWrite(i, 0x16, 0);
		mvEthPhyRegWrite(i, 0x1d, 3);
		mvEthPhyRegWrite(i, 0x1e, 2);
		mvEthPhyRegWrite(i, 0x1d, 0);

		/* Power up the phy */
		/* mvEthPhyRegRead(i,ETH_PHY_CTRL_REG, &reg); */
		/* reg |= ETH_PHY_CTRL_RESET_MASK; */
		/* mvEthPhyRegWrite(i,ETH_PHY_CTRL_REG, reg);   software reset */

		/* Enable QSGMII AN */
		/* Set page to 4. */
		mvEthPhyRegWrite(i, 0x16, 4);
		/* Enable AN */
		mvEthPhyRegWrite(i, 0x0, 0x1140);
		/* Set page to 0. */
		mvEthPhyRegWrite(i, 0x16, 0);

		mvEthPhyRegRead(i, ETH_PHY_CTRL_REG, &reg);
		reg &= ~(ETH_PHY_CTRL_POWER_DOWN_MASK);
		mvEthPhyRegWrite(i, ETH_PHY_CTRL_REG, reg);
	}
}

#if 1
int
do_eee(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
        if (argc != 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
        }

        i = simple_strtoul(argv[1], 0, 10);
        printf("Enabling EEE on port %u\n", i);

	/* This script applies to 88E15XX MACSec Rev A0 PHY Initialization */
	/* Date: September 10, 2010 */
	/* This script is used as example to perform phy and macsec initialization for Port 0-3 */
	/* - MACSec Initialization */
	/* - PHY Initialization  */
	/* - Enable EEE Master (Legacy) Mode */
	/* MARVELL CONFIDENTIAL - FOR ARUBA USE ONLY  */
	/* RW u1 P0-3 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);
	/* --------------------------------------- */
	/* MACSec Initialization */
	/* --------------------------------------- */
	/* Disable drop_bad_tag bit for Port 0-3 */
	/* RW u1 P0 R22 H0010 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);
	/* RW u1 P0 R1 H000b */
	mvEthPhyRegWrite(i, 0x1, 0x000b);
	/* RW u1 P0 R2 h0000 */
	mvEthPhyRegWrite(i, 0x2, 0x0000);
	/* RW u1 P0 R3 H0fb4 */
	mvEthPhyRegWrite(i, 0x3, 0x0fb4);
	/* RW u1 P0 R1 H080b */
	mvEthPhyRegWrite(i, 0x1, 0x080b);
	/* RW u1 P0 R2 h0000 */
	mvEthPhyRegWrite(i, 0x2, 0x0000);
	/* RW u1 P0 R3 H0fb4 */
	mvEthPhyRegWrite(i, 0x3, 0x0fb4);
	/* RW u1 P0 R1 H100b */
	mvEthPhyRegWrite(i, 0x1, 0x100b);
	/* RW u1 P0 R2 h0000 */
	mvEthPhyRegWrite(i, 0x2, 0x0000);
	/* RW u1 P0 R3 H0fb4 */
	mvEthPhyRegWrite(i, 0x3, 0x0fb4);
	/* RW u1 P0 R1 H180b */
	mvEthPhyRegWrite(i, 0x1, 0x180b);
	/* RW u1 P0 R2 h0000 */
	mvEthPhyRegWrite(i, 0x2, 0x0000);
	/* RW u1 P0 R3 H0fb4 */
	mvEthPhyRegWrite(i, 0x3, 0x0fb4);
	/* RW u1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);
	/* --------------------------------------- */
	/* PHY Initialization */
	/* --------------------------------------- */
	/* RW u1 P0-3 R22 h00FA */
	mvEthPhyRegWrite(i, 0x16, 0x00fa);
	/* RW u1 P0-3 R8 h0010 */
	mvEthPhyRegWrite(i, 0x8, 0x0010);
	/* RW u1 P0-3 R22 h00FB */
	mvEthPhyRegWrite(i, 0x16, 0x00fb);
	/* RW u1 P0-3 R1 h4099 */
	mvEthPhyRegWrite(i, 0x1, 0x4099);
	/* RW u1 P0-3 R3 h1120 */
	mvEthPhyRegWrite(i, 0x3, 0x1120);
	/* RW u1 P0-3 R11 h113C */
	mvEthPhyRegWrite(i, 0xb, 0x113c);
	/* RW u1 P0-3 R14 h8100 */
	mvEthPhyRegWrite(i, 0xe, 0x8100);
	/* RW u1 P0-3 R15 h112A */
	mvEthPhyRegWrite(i, 0xf, 0x112a);
	/* RW u1 P0-3 R22 h00FC */
	mvEthPhyRegWrite(i, 0x16, 0x00fc);
	/* RW u1 P0-3 R1 h20B0 */
	mvEthPhyRegWrite(i, 0x1, 0x20b0);
	/* RW u1 P0-3 R22 h00FF	 */
	mvEthPhyRegWrite(i, 0x16, 0x00ff);
	/* RW u1 P0-3 R17 h0000 */
	mvEthPhyRegWrite(i, 0x11, 0x0000);
	/* RW u1 P0-3 R16 h2000 */
	mvEthPhyRegWrite(i, 0x10, 0x2000);
	/* RW u1 P0-3 R17 h4444 */
	mvEthPhyRegWrite(i, 0x11, 0x4444);
	/* RW u1 P0-3 R16 h2140 */
	mvEthPhyRegWrite(i, 0x10, 0x2140);
	/* RW u1 P0-3 R17 h8064 */
	mvEthPhyRegWrite(i, 0x11, 0x8064);
	/* RW u1 P0-3 R16 h2141 */
	mvEthPhyRegWrite(i, 0x10, 0x2141);
	/* RW u1 P0-3 R17 h0108 */
	mvEthPhyRegWrite(i, 0x11, 0x0108);
	/* RW u1 P0-3 R16 h2144 */
	mvEthPhyRegWrite(i, 0x10, 0x2144);
	/* RW u1 P0-3 R17 h0F16 */
	mvEthPhyRegWrite(i, 0x11, 0x0f16);
	/* RW u1 P0-3 R16 h2146 */
	mvEthPhyRegWrite(i, 0x10, 0x2146);
	/* RW u1 P0-3 R17 h8C44 */
	mvEthPhyRegWrite(i, 0x11, 0x8c44);
	/* RW u1 P0-3 R16 h214B */
	mvEthPhyRegWrite(i, 0x10, 0x214b);
	/* RW u1 P0-3 R17 h0F90 */
	mvEthPhyRegWrite(i, 0x11, 0x0f90);
	/* RW u1 P0-3 R16 h214C */
	mvEthPhyRegWrite(i, 0x10, 0x214c);
	/* RW u1 P0-3 R17 hBA33 */
	mvEthPhyRegWrite(i, 0x11, 0xba33);
	/* RW u1 P0-3 R16 h214D */
	mvEthPhyRegWrite(i, 0x10, 0x214d);
	/* RW u1 P0-3 R17 h39AA */
	mvEthPhyRegWrite(i, 0x11, 0x39aa);
	/* RW u1 P0-3 R16 h214F */
	mvEthPhyRegWrite(i, 0x10, 0x214f);
	/* RW u1 P0-3 R17 h8433 */
	mvEthPhyRegWrite(i, 0x11, 0x8433);
	/* RW u1 P0-3 R16 h2151 */
	mvEthPhyRegWrite(i, 0x10, 0x2151);
	/* RW u1 P0-3 R17 h2010 */
	mvEthPhyRegWrite(i, 0x11, 0x2010);
	/* RW u1 P0-3 R16 h2152 */
	mvEthPhyRegWrite(i, 0x10, 0x2152);
	/* RW u1 P0-3 R17 h99EB */
	mvEthPhyRegWrite(i, 0x11, 0x99eb);
	/* RW u1 P0-3 R16 h2153 */
	mvEthPhyRegWrite(i, 0x10, 0x2153);
	/* RW u1 P0-3 R17 h2f3B */
	mvEthPhyRegWrite(i, 0x11, 0x2f3b);
	/* RW u1 P0-3 R16 h2154 */
	mvEthPhyRegWrite(i, 0x10, 0x2154);
	/* RW u1 P0-3 R17 h584E */
	mvEthPhyRegWrite(i, 0x11, 0x584e);
	/* RW u1 P0-3 R16 h2156 */
	mvEthPhyRegWrite(i, 0x10, 0x2156);
	/* RW u1 P0-3 R17 h1223 */
	mvEthPhyRegWrite(i, 0x11, 0x1223);
	/* RW u1 P0-3 R16 h2158 */
	mvEthPhyRegWrite(i, 0x10, 0x2158);
	/* RW u1 P0-3 R22 h0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);
	/* --------------------------------------- */
	/* Enable EEE Master (Legacy) Mode  */
	/* --------------------------------------- */
	/*Enable EEE_Auto-neg for 1000BASE-T and 100BASE-TX */
	/* RW u1 P0-3 R22 h0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);
	/* RW u1 P0-3 R13 h0007 */
	mvEthPhyRegWrite(i, 0xd, 0x0007);
	/* RW u1 P0-3 R14 h003C */
	mvEthPhyRegWrite(i, 0xe, 0x003c);
	/* RW u1 P0-3 R13 h4007 */
	mvEthPhyRegWrite(i, 0xd, 0x4007);
	/* RW u1 P0-3 R14 h0006 */
	mvEthPhyRegWrite(i, 0xe, 0x0006);
	/* Enable MACSec (Reg 27_2.13 = '1')  */
	/* RW u1 P0-3 R22 H0012 */
	mvEthPhyRegWrite(i, 0x16, 0x0012);
	/* RW u1 P0-3 R27 xx1x xxxx xxxx xxxx */
	mvEthPhyRegWrite(i, 0x1b, 0x0004);
	/* Enable EEE Master (Legacy) Mode  */
	/* RW u1 P0 R22 H0010 */
	mvEthPhyRegWrite(i, 0x16, 0x0010);
	/* RW u1 P0 R1 H03c1 */
	mvEthPhyRegWrite(i, 0x1, 0x03c1);
	/* RW u1 P0 R2 h0001 */
	mvEthPhyRegWrite(i, 0x2, 0x0001);
	/* RW u1 P0 R3 H0000 */
	mvEthPhyRegWrite(i, 0x3, 0x0000);
	/* RW u1 P0 R1 H0bc1 */
	mvEthPhyRegWrite(i, 0x1, 0x0bc1);
	/* RW u1 P0 R2 h0001 */
	mvEthPhyRegWrite(i, 0x2, 0x0001);
	/* RW u1 P0 R3 H0000 */
	mvEthPhyRegWrite(i, 0x3, 0x0000);
	/* RW u1 P0 R1 H13c1 */
	mvEthPhyRegWrite(i, 0x1, 0x13c1);
	/* RW u1 P0 R2 h0001 */
	mvEthPhyRegWrite(i, 0x2, 0x0001);
	/* RW u1 P0 R3 H0000 */
	mvEthPhyRegWrite(i, 0x3, 0x0000);
	/* RW u1 P0 R1 H1bc1 */
	mvEthPhyRegWrite(i, 0x1, 0x1bc1);
	/* RW u1 P0 R2 h0001 */
	mvEthPhyRegWrite(i, 0x2, 0x0001);
	/* RW u1 P0 R3 H0000 */
	mvEthPhyRegWrite(i, 0x3, 0x0000);
	/* RW u1 P0 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);
	/* --------------------------------------- */
	/* Soft-Reset */
	/* --------------------------------------- */
	/* RW u1 P0-3 R22 H0000 */
	mvEthPhyRegWrite(i, 0x16, 0x0000);
	/* RW u1 P0-3 R0 H9140 */
	mvEthPhyRegWrite(i, 0x0, 0x9140);

	return 0;
}

U_BOOT_CMD(
	eee,     2,     1,      do_eee,
	"eee - enable eee\n",
	"- enable eee\n"
);

int
do_dump_eee(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i;
    if (argc != 2) {
	printf ("Usage:\n%s\n", cmdtp->usage);
	return 1;
    }

    i = simple_strtoul(argv[1], 0, 10);
    printf("Partner capabilities for port %u: 0x%x\n", i, 
           aruba_xmdio_read(i, 0x7, 0x3d));

    return 0;
}

U_BOOT_CMD(
	dump_eee,     2,     1,      do_dump_eee,
	"dump_eee - show EEE partner capabilities\n",
	"- show EEE partner capabilities\n"
);
#endif
