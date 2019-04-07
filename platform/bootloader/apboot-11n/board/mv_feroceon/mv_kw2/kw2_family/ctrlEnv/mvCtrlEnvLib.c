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

/* includes */
#include "mvCommon.h"
#include "mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "ctrlEnv/mvCtrlEthCompLib.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "gpp/mvGpp.h"
#include "cpu/mvCpu.h"

#if defined(MV_INCLUDE_PEX)
#include "pci-if/mvPciIf.h"
#include "pex/mvPex.h"
#include "pex/mvPexRegs.h"
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#if defined(CONFIG_MV_ETH_LEGACY)
#include "eth/mvEth.h"
#else
#include "neta/gbe/mvNeta.h"
#endif /* CONFIG_MV_ETH_LEGACY or CONFIG_MV_ETH_NETA */
#endif

#if defined(MV_INCLUDE_XOR)
#include "xor/mvXor.h"
#endif

#if defined(MV_INCLUDE_SATA)
#include "sata/CoreDriver/mvSata.h"
#endif

#if defined(MV_INCLUDE_USB)
#include "usb/mvUsb.h"
#endif

#if defined(MV_INCLUDE_AUDIO)
#include "audio/mvAudio.h"
#endif

#if defined(MV_INCLUDE_PDMA)
#include "ctrlEnv/sys/mvSysPdmaApi.h"
#endif

#if defined(MV_INCLUDE_TS)
#include "ts/mvTsu.h"
#endif

#if defined(MV_INCLUDE_TDM)
#include "mvSysTdmConfig.h"
#endif

/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

#if defined(MV_INCLUDE_TDM)
static MV_VOID mvCtrlTdmClkCtrlSet(MV_VOID);
#endif

extern MV_BOOL mvCtrlIsBootFromNOR(MV_VOID);

/*******************************************************************************
* mvCtrlEnvInit - Initialize Marvell controller environment.
*
* DESCRIPTION:
*       This function get environment information and initialize controller
*       internal/external environment. For example
*       1) MPP settings according to board MPP macros.
*		NOTE: It is the user responsibility to shut down all DMA channels
*		in device and disable controller sub units interrupts during
*		boot process.
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
MV_STATUS mvCtrlEnvInit(MV_VOID)
{
	MV_U32 mppGroup;
	MV_U32 devId;
	MV_U32 boardId;
	MV_U32 i;
	MV_U32 maxMppGrp = 1;
	MV_U32 mppVal = 0;
	MV_U32 bootVal = 0;
	MV_U32 mppGroupType = 0;
	MV_U32 mppGroup1[][3] = MPP_GROUP_1_TYPE;
	MV_U32 mppGroup2[][3] = MPP_GROUP_2_TYPE;
	MV_U32 mppGroup3[][3] = MPP_GROUP_3_TYPE;
	MV_U32 mppG1Mask[] = MPP_GROUP_1_MASK;
	MV_U32 mppG2Mask[] = MPP_GROUP_2_MASK;
	MV_U32 mppG3Mask[] = MPP_GROUP_3_MASK;
	MV_U32 ethCompOpt;
	MV_BOARD_SPEC_INIT *boardSpec;

	boardSpec = mvBoardSpecInitGet();
	if (boardSpec != NULL) {
		MV_U32 reg;
		i = 0;
		while (boardSpec[i].reg != TBL_TERM) {
			reg = MV_REG_READ(boardSpec[i].reg);
			reg &= ~boardSpec[i].mask;
			reg |= (boardSpec[i].val & boardSpec[i].mask);
			MV_REG_WRITE(boardSpec[i].reg, reg);
			i++;
		}
	}
	if (mvCpuL2Exists() == MV_TRUE)
		MV_REG_BIT_RESET(PMU_POWER_IF_POLARITY_REG, BIT0);
	else
		MV_REG_BIT_SET(PMU_POWER_IF_POLARITY_REG, BIT0);

	devId = mvCtrlModelGet();
	boardId = mvBoardIdGet();

	switch (devId) {
	case MV_6510_DEV_ID:
		maxMppGrp = MV_6510_MPP_MAX_GROUP;
		break;
	case MV_6530_DEV_ID:
		maxMppGrp = MV_6530_MPP_MAX_GROUP;
		break;
	case MV_6550_DEV_ID:
		maxMppGrp = MV_6550_MPP_MAX_GROUP;
		break;
	case MV_6560_DEV_ID:
		maxMppGrp = MV_6560_MPP_MAX_GROUP;
		break;
	}

	/* MPP Init */
	/* We split mpp init to 3 phases:
	 * 1. We init mpp[19:0] from the board info. mpp[23:20] will be overwritten
	 * in phase 2.
	 * 2. We detect the mpp group type and according the mpp values [35:20].
	 * 3. We detect the mpp group type and according the mpp values [49:36].
	 */
	/* Mpp phase 1 mpp[22:0] */
	/* Read MPP group from board level and assign to MPP register */
	for (mppGroup = 0; mppGroup < 3; mppGroup++) {
		mppVal = mvBoardMppGet(mppGroup);
		if (mppGroup == 0) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNAND() || mvCtrlIsBootFromNOR()) {
				mppVal &= ~0xffffffff;
				bootVal &= 0xffffffff;
				mppVal |= bootVal;
			}
		}
		if (mppGroup == 1) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNAND()) {
				mppVal &= ~0xfffff;
				bootVal &= 0xfffff;
				mppVal |= bootVal;
			}
		}

		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	/* only for CV checks, setting the SW_PPU_MDC & SW_PPU_MDIO MPPs */
	/* MV_REG_WRITE(mvCtrlMppRegGet(3), 0x03300000); - we need to support this in the future! */

	/* Identify MPPs group */
	mvBoardMppIdUpdate();

	/* Update the MPP & Internal Mux configurations according to the
	   MPP group detection results. */

	mppGroupType = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_1);
	/* Mpp phase 2 */
	/* Read MPP group from board level and assign to MPP register */
	i = 0;
	for (mppGroup = 2; mppGroup < 5; mppGroup++) {

		if (mppGroupType == MV_BOARD_OTHER) {
			mppVal = mvBoardMppGet(mppGroup);
		} else {
			mppVal = mvBoardMppGet(mppGroup);
			mppVal &= ~mppG1Mask[i];
			mppVal |= mppGroup1[mppGroupType][i];
			i++;
		}

		if (mppGroup == 4) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromSPI()) {
				mppVal &= ~0xffff;
				bootVal &= 0xffff;
				mppVal |= bootVal;
			} else if (mvCtrlIsBootFromNOR() == MV_NOR_LOW_MPPS) {
				mppVal &= ~0xff000000;
				bootVal &= 0xff000000;
				mppVal |= bootVal;
			}
		}

		if (mppGroup == 3) {
			ethCompOpt = mvBoardEthComplexConfigGet();
			/* DB settings only: MDC/MDIO - do we need to set it to SW or to GW?? */
			if ((mppVal & 0xFF00000) == 0x4400000) {
				/* Note: we only get here if the MPPs are not for TDM comm unit */
				if (ethCompOpt & (ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6)) {
					mppVal &= ~0xFF00000;
					mppVal |= 0x3300000;
				}
			}
		}

		/* Group 2 is shared mpp[23:23] */
		if (mppGroup == 2) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			mppVal &= ~0x0fffffff;
			bootVal &= 0x0fffffff;
			mppVal |= bootVal;
			/* SDIO support */
			if (mvBoardIsSdioEnabled() == MV_TRUE) {
				mppVal &= ~0x0FFFFFF0;
				mppVal |= 0x01111110;
			}
		}
		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	/* Mpp phase 3 RGMII1 */
	mppGroupType = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_2);
	/* Read MPP group from board level and assign to MPP register */
	i = 0;
	for (mppGroup = 4; mppGroup < 7; mppGroup++) {
		if (mppGroupType == MV_BOARD_OTHER)
			mppVal = mvBoardMppGet(mppGroup);
		else {
			mppVal = mvBoardMppGet(mppGroup);
			mppVal &= ~mppG2Mask[i];
			mppVal |= mppGroup2[mppGroupType][i];
			i++;
		}

		/* Group 4 is shared mpp[37:32] */
		if (mppGroup == 4) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_LOW_MPPS) {
				mppVal &= ~0xff000000;
				bootVal &= 0xff000000;
				mppVal |= bootVal;
			}
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			mppVal &= ~0x00ffffff;
			bootVal &= 0x00ffffff;
			mppVal |= bootVal;
		}

		if (mppGroup == 5) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_LOW_MPPS) {
				mppVal &= ~0xffffffff;
				bootVal &= 0xffffffff;
				mppVal |= bootVal;
			}
		}

		if (mppGroup == 6) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_LOW_MPPS) {
				mppVal &= ~0x000fffff;
				bootVal &= 0x000fffff;
				mppVal |= bootVal;
			}
		}

		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	/* Mpp phase 4 */
	mppGroupType = mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_3);
	/* Read MPP group from board level and assign to MPP register */
	i = 0;
	for (mppGroup = 6; mppGroup < 9; mppGroup++) {
		if (mppGroupType == MV_BOARD_OTHER)
			mppVal = mvBoardMppGet(mppGroup);
		else {
			mppVal = mvBoardMppGet(mppGroup);
			mppVal &= ~mppG3Mask[i];
			mppVal |= mppGroup3[mppGroupType][i];
			i++;
		}

		/* Group 6 is shared mpp[52:48] */
		if (mppGroup == 6) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			mppVal &= ~0x000fffff;
			bootVal &= 0x000fffff;
			mppVal |= bootVal;
		}

		if (mppGroup == 8) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_LOW_MPPS) {
				mppVal &= ~0xff00;
				bootVal &= 0xff00;
				mppVal |= bootVal;
			}
		}

		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	/* Mpp phase 5 */
	for (mppGroup = 9; mppGroup < 12; mppGroup++) {
		mppVal = mvBoardMppGet(mppGroup);
		if (mppGroup == 9) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_HIGH_MPPS) {
				mppVal &= ~0xffffffff;
				bootVal &= 0xffffffff;
				mppVal |= bootVal;
			}
		}
		if (mppGroup == 10) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_HIGH_MPPS) {
				mppVal &= ~0xffffffff;
				bootVal &= 0xffffffff;
				mppVal |= bootVal;
			}
		}
		if (mppGroup == 11) {
			bootVal = MV_REG_READ(mvCtrlMppRegGet(mppGroup));
			if (mvCtrlIsBootFromNOR() == MV_NOR_HIGH_MPPS) {
				mppVal &= ~0xf;
				bootVal &= 0xf;
				mppVal |= bootVal;
			}
		}
		MV_REG_WRITE(mvCtrlMppRegGet(mppGroup), mppVal);
	}

	mvEthernetComplexInit();
#if defined(MV_INCLUDE_TDM)
	mvCtrlTdmClkCtrlSet();
#endif

	/* Setup Ana-Grp1 config register. */
	MV_REG_WRITE(ANA_GRP1_CONFIG_REG, ANA_GRP1_CFG_DEF_VAL);

	return MV_OK;
}

/*******************************************************************************
* mvCtrlEthSataComplexBuildConfig
*
* DESCRIPTION:
*	Build the Ethernet / Sata comples configuration according to the
*	on-board modules.
*
* INPUT:
*	brdModules - A bitmask of the on-board modules.
*
* OUTPUT:
*	ethConfig - A bitmask representing the ethernet complex configuration.
*
* RETURN:
*       MV_STATUS - MV_OK, MV_ERROR.
*
*******************************************************************************/
MV_STATUS mvCtrlEthSataComplexBuildConfig(MV_U32 brdModules, MV_ETH_COMPLEX_IF_SOURCES *ifSrc, MV_U32 *ethConfig)
{
	MV_U32 enabled = 0;
	MV_U32 mask = ESC_OPT_ALL;
	MV_U8 swExist = 0;
	MV_U32 tmpMask;

	/* Sata */
	if (brdModules & MV_BOARD_MODULE_SATA_ID) {
		if (brdModules & MV_BOARD_MODULE_QSGMII_ID)
			enabled |= ESC_OPT_ILLEGAL;
		else {
			enabled |= ESC_OPT_SATA;
			mask &= ~(ESC_OPT_QSGMII | ESC_OPT_SGMII);
		}
	}

	/* QSGMII */
	if (brdModules & MV_BOARD_MODULE_QSGMII_ID) {
		if (brdModules & (MV_BOARD_MODULE_SATA_ID | MV_BOARD_MODULE_4FE_PHY_ID))
			enabled |= ESC_OPT_ILLEGAL;
		else {
			enabled |= ESC_OPT_QSGMII;
			mask &= ~(ESC_OPT_SATA | ESC_OPT_FE3PHY | ESC_OPT_SGMII);
			swExist = 1;
		}
	}

	/* 3xFE */
	if (brdModules & MV_BOARD_MODULE_4FE_PHY_ID) {
		if (brdModules & MV_BOARD_MODULE_QSGMII_ID)
			enabled |= ESC_OPT_ILLEGAL;
		else {
			enabled |= ESC_OPT_FE3PHY;
			mask &= ~(ESC_OPT_QSGMII);
			swExist = 1;
		}
	}

	if (!swExist) {
		mask &= ~(ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6 |
			  ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5 | ESC_OPT_GEPHY_SW_P0 | ESC_OPT_GEPHY_SW_P5);
	}

	/* RGMII1 */
	if (brdModules & (MV_BOARD_MODULE_RGMIIB_ID | MV_BOARD_MODULE_MIIB_ID)) {
		enabled |= ESC_OPT_RGMIIB_MAC0;
		mask &= ~ESC_OPT_RGMIIB_MAC0;
	}

	/* RGMIIA */
	if (brdModules & (MV_BOARD_MODULE_RGMIIA_ID | MV_BOARD_MODULE_MIIA_ID)) {
		tmpMask = (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIA_MAC1 | ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_SW_P6);
		tmpMask &= mask;
		if (MV_IS_POWER_OF_2(tmpMask))	/* RMGII0 source is well defined. */
			enabled |= tmpMask;
		/* Need to use external info to figure out RGMIIA source. */
		else if (ifSrc->rgmiiASrc == EC_MAC0_SRC) {
			enabled |= ESC_OPT_RGMIIA_MAC0;
			mask &= ~(tmpMask ^ ESC_OPT_RGMIIA_MAC0);
		} else if (ifSrc->rgmiiASrc == EC_MAC1_SRC) {
			enabled |= ESC_OPT_RGMIIA_MAC1;
			mask &= ~ESC_OPT_GEPHY_MAC1;
			mask &= ~(tmpMask ^ ESC_OPT_RGMIIA_MAC1);
		} else if (ifSrc->rgmiiASrc == EC_SW_P5_SRC) {
			enabled |= ESC_OPT_RGMIIA_SW_P5;
			mask &= ~(ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIA_MAC1);
			mask &= ~(tmpMask ^ ESC_OPT_RGMIIA_SW_P5);
		} else if (ifSrc->rgmiiASrc == EC_SW_P6_SRC) {
			enabled |= ESC_OPT_RGMIIA_SW_P6;
			mask &= ~(ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIA_MAC1);
			mask &= ~(tmpMask ^ ESC_OPT_RGMIIA_SW_P6);
		}
	}

	/* FE / GE PHY */
	if (brdModules & MV_BOARD_MODULE_FE_GE_PHY_ID) {
/*  		tmpMask = ESC_OPT_GEPHY_MAC1 | ESC_OPT_GEPHY_SW_P0 | ESC_OPT_GEPHY_SW_P5; */
/*  		tmpMask &= mask; */
		/*  		if(MV_IS_POWER_OF_2(tmpMask))*//* FE/GE source is well defined. */
/*  			enabled |= tmpMask; */
		/* Need to use external info to figure out GE/FE source. */
		if (ifSrc->feGeSrc == EC_MAC1_SRC) {
			enabled |= ESC_OPT_GEPHY_MAC1;
			mask &= ~(tmpMask ^ ESC_OPT_GEPHY_MAC1);
		} else if (ifSrc->feGeSrc == EC_SW_P0_SRC) {
			enabled |= ESC_OPT_GEPHY_SW_P0;
			mask &= ~(tmpMask ^ ESC_OPT_GEPHY_SW_P0);
		} else if (ifSrc->feGeSrc == EC_SW_P5_SRC) {
			enabled |= ESC_OPT_GEPHY_SW_P5;
			mask &= ~(tmpMask ^ ESC_OPT_GEPHY_SW_P5);
		}
	}

	/* SGMII */
	if (brdModules & MV_BOARD_MODULE_SGMII_ID)
		enabled |= ESC_OPT_SGMII;

	if (swExist) {
		if ((!(enabled & (ESC_OPT_RGMIIA_MAC0 | ESC_OPT_RGMIIB_MAC0))) && (ifSrc->swSrc == EC_SRC_NONE))
			enabled |= ESC_OPT_MAC0_2_SW_P4;
 		else if (!(enabled & (ESC_OPT_GEPHY_SW_P5 | ESC_OPT_RGMIIA_SW_P5 | ESC_OPT_RGMIIA_MAC1 | ESC_OPT_GEPHY_MAC1)))
			enabled |= ESC_OPT_MAC1_2_SW_P5;
		else
			enabled |= ESC_OPT_ILLEGAL;
	}

	*ethConfig = enabled;
	return MV_OK;

}

/*******************************************************************************
* mvCtrlMppRegGet - return reg address of mpp group
*
* DESCRIPTION:
*
* INPUT:
*       mppGroup - MPP group.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_U32 - Register address.
*
*******************************************************************************/
MV_U32 mvCtrlMppRegGet(MV_U32 mppGroup)
{
	MV_U32 ret;

	if (mppGroup >= MV_65XX_MPP_MAX_GROUP)
		mppGroup = 0;

	ret = MPP_CONTROL_REG(mppGroup);

	return ret;
}

#if defined(MV_INCLUDE_PEX)
/*******************************************************************************
* mvCtrlPexMaxIfGet - Get Marvell controller number of PEX interfaces.
*
* DESCRIPTION:
*       This function returns Marvell controller number of PEX interfaces.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of PEX interfaces. If controller
*		ID is undefined the function returns '0'.
*
*******************************************************************************/
MV_U32 mvCtrlPexMaxIfGet(MV_VOID)
{
	MV_U32 devId;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
		return MV_PEX_MAX_IF_6510;
		break;
	default:
		return MV_PEX_MAX_IF;
		break;
	}
}
#endif

/*******************************************************************************
* mvCtrlEthMaxPortGet - Get Marvell controller number of etherent ports.
*
* DESCRIPTION:
*       This function returns Marvell controller number of etherent port.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of etherent port.
*
*******************************************************************************/
MV_U32 mvCtrlEthMaxPortGet(MV_VOID)
{
	MV_U32 res = 0;

	res = MV_ETH_MAX_PORTS;
	return res;
}

#if defined(MV_INCLUDE_SATA)
/*******************************************************************************
* mvCtrlSataMaxPortGet - Get Marvell controller number of Sata ports.
*
* DESCRIPTION:
*       This function returns Marvell controller number of Sata ports.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of Sata ports.
*
*******************************************************************************/
MV_U32 mvCtrlSataMaxPortGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
	case MV_6530_DEV_ID:
		res = MV_SATA_6510_6530_MAX_CHAN;
		break;
	case MV_6550_DEV_ID:
	case MV_6560_DEV_ID:
		res = MV_SATA_6550_6560_MAX_CHAN;
		break;
	}
	return res;
}
#endif

#if defined(MV_INCLUDE_XOR)
/*******************************************************************************
* mvCtrlXorMaxChanGet - Get Marvell controller number of XOR channels.
*
* DESCRIPTION:
*       This function returns Marvell controller number of XOR channels.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of XOR channels.
*
*******************************************************************************/
MV_U32 mvCtrlXorMaxChanGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
	case MV_6530_DEV_ID:
		res = MV_XOR_6510_6530_MAX_CHAN;
		break;
	default:
		res = MV_XOR_MAX_CHAN;
		break;
	}
	return res;
}

/*******************************************************************************
* mvCtrlXorMaxUnitGet - Get Marvell controller number of XOR units.
*
* DESCRIPTION:
*       This function returns Marvell controller number of XOR units.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Marvell controller number of XOR units.
*
*******************************************************************************/
MV_U32 mvCtrlXorMaxUnitGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
	case MV_6530_DEV_ID:
		res = MV_XOR_6510_6530_MAX_UNIT;
		break;
	default:
		res = MV_XOR_MAX_UNIT;
		break;
	}
	return res;
}

#endif

#if defined(MV_INCLUDE_USB)
/*******************************************************************************
* mvCtrlUsbHostMaxGet - Get number of Marvell Usb  controllers
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       returns number of Marvell USB  controllers.
*
*******************************************************************************/
MV_U32 mvCtrlUsbMaxGet(void)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
	case MV_6530_DEV_ID:
		res = MV_USB_6510_6530_MAX_PORTS;
		break;
	default:
		res = MV_USB_MAX_PORTS;
		break;
	}

	return res;
}
#endif

#if defined(MV_INCLUDE_LEGACY_NAND)
/*******************************************************************************
* mvCtrlNandSupport - Return if this controller has integrated NAND flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if NAND is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlNandSupport(MV_VOID)
{
	return MV_65XX_NAND;
}
#endif

#if defined(MV_INCLUDE_SDIO)
/*******************************************************************************
* mvCtrlSdioSupport - Return if this controller has integrated SDIO flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if SDIO is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlSdioSupport(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
	case MV_6530_DEV_ID:
		res = MV_6510_6530_SDIO;
		break;
	case MV_6550_DEV_ID:
	case MV_6560_DEV_ID:
		res = MV_6550_6560_SDIO;
		break;
	}
	return res;
}
#endif

#if defined(MV_INCLUDE_TS)
/*******************************************************************************
* mvCtrlTsSupport - Return if this controller has integrated TS flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if TS is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlTsSupport(MV_VOID)
{
	return MV_65XX_TS;
}
#endif

#if defined(MV_INCLUDE_TDM)
/*******************************************************************************
* mvCtrlTdmClkCtrlSet - Set TDM Clock Out Divider Control register
*
* DESCRIPTION:
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
static MV_VOID mvCtrlTdmClkCtrlSet(MV_VOID)
{
	MV_U32 clkReg;

	clkReg = MV_REG_READ(TDM_PON_CLK_OUT_DIV_CONTROL_REG);
	clkReg = clkReg & ~(TDM_CLK_DIV_RATIO_MASK | TDM_CLK_SRC_SEL_MASK);
	MV_REG_WRITE(TDM_PON_CLK_OUT_DIV_CONTROL_REG,
		     (clkReg | TDM_CLK_DIV_RATIO_8M | TDM_LOAD_RATIO_MASK | PLL_MODE));
	mvOsUDelay(1);
	MV_REG_WRITE(TDM_PON_CLK_OUT_DIV_CONTROL_REG, (clkReg | TDM_CLK_DIV_RATIO_8M | PLL_MODE));
}

/*******************************************************************************
* mvCtrlTdmSupport - Return if this controller has integrated TDM flash support
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if TDM is supported and MV_FALSE otherwise
*
*******************************************************************************/
MV_U32 mvCtrlTdmSupport(MV_VOID)
{
	return MV_65XX_TDM;
}

/*******************************************************************************
* mvCtrlTdmMaxGet - Return the maximum number of TDM ports.
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The number of TDM ports in device.
*
*******************************************************************************/
MV_U32 mvCtrlTdmMaxGet(MV_VOID)
{
	MV_U32 devId;
	MV_U32 res = 0;

	devId = mvCtrlModelGet();

	switch (devId) {
	case MV_6510_DEV_ID:
		res = MV_6510_TDM_MAX_PORTS;
		break;
	case MV_6530_DEV_ID:
		res = MV_6530_TDM_MAX_PORTS;
		break;
	case MV_6550_DEV_ID:
		res = MV_6550_TDM_MAX_PORTS;
		break;
	case MV_6560_DEV_ID:
		res = MV_6560_TDM_MAX_PORTS;
		break;
	}
	return res;
}

/*******************************************************************************
* mvCtrlTdmTypeGet
*
* DESCRIPTION:
*	Return the TDM unit type being compiled in.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*	The TDM unit type.
*
*******************************************************************************/
MV_UNIT_ID mvCtrlTdmUnitTypeGet(MV_VOID)
{
#ifdef MV_TDM_SUPPORT
	return TDM_2CH_UNIT_ID;
#else
	return TDM_32CH_UNIT_ID;
#endif
}

/*******************************************************************************
* mvCtrlTdmUnitIrqGet
*
* DESCRIPTION:
*	Return the TDM unit IRQ number depending on the TDM unit compilation
*	options.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	None.
******************************************************************************/
MV_U32 mvCtrlTdmUnitIrqGet(MV_VOID)
{
#ifdef MV_TDM_SUPPORT
	return MV_TDM_2CH_IRQ_NUM;
#else
	return MV_TDM_IRQ_NUM;
#endif
}

#endif

/*******************************************************************************
* mvCtrlModelGet - Get Marvell controller device model (Id)
*
* DESCRIPTION:
*       This function returns 16bit describing the device model (ID) as defined
*       in PCI Device and Vendor ID configuration register offset 0x0.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       16bit desscribing Marvell controller ID
*
*******************************************************************************/
MV_U16 mvCtrlModelGet(MV_VOID)
{
	MV_U32 devId;
	MV_U16 model = 0;

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID, 0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	devId = MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PEX_DEVICE_AND_VENDOR_ID));

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif

	model = (MV_U16) ((devId >> 16) & 0xFFFF);
	return model;
}

/*******************************************************************************
* mvCtrlRevGet - Get Marvell controller device revision number
*
* DESCRIPTION:
*       This function returns 8bit describing the device revision as defined
*       in PCI Express Class Code and Revision ID Register.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       8bit desscribing Marvell controller revision number
*
*******************************************************************************/
MV_U8 mvCtrlRevGet(MV_VOID)
{
	MV_U8 revNum;
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Check pex power state */
	MV_U32 pexPower;
	pexPower = mvCtrlPwrClckGet(PEX_UNIT_ID, 0);
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_TRUE);
#endif
	revNum = (MV_U8) MV_REG_READ(PEX_CFG_DIRECT_ACCESS(0, PCI_CLASS_CODE_AND_REVISION_ID));
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
	/* Return to power off state */
	if (pexPower == MV_FALSE)
		mvCtrlPwrClckSet(PEX_UNIT_ID, 0, MV_FALSE);
#endif
	return ((revNum & PCCRIR_REVID_MASK) >> PCCRIR_REVID_OFFS);
}

/*******************************************************************************
* mvCtrlNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvCtrlNameGet(char *pNameBuff)
{
	mvOsSPrintf(pNameBuff, "%s%x Rev %d", SOC_NAME_PREFIX, mvCtrlModelGet(), mvCtrlRevGet());

	return MV_OK;
}

/*******************************************************************************
* mvCtrlModelRevGet - Get Controller Model (Device ID) and Revision
*
* DESCRIPTION:
*       This function returns 32bit value describing both Device ID and Revision
*       as defined in PCI Express Device and Vendor ID Register and device revision
*	    as defined in PCI Express Class Code and Revision ID Register.

*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing both controller device ID and revision number
*
*******************************************************************************/
MV_U32 mvCtrlModelRevGet(MV_VOID)
{
	return ((mvCtrlModelGet() << 16) | mvCtrlRevGet());
}

/*******************************************************************************
* mvCtrlModelRevNameGet - Get Marvell controller name
*
* DESCRIPTION:
*       This function returns a string describing the device model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain device name string. Minimum size 30 chars.
*
* RETURN:
*
*       MV_ERROR if informantion can not be read.
*******************************************************************************/
MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff)
{
	switch (mvCtrlModelRevGet()) {
	case MV_6510_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6510_Z1_NAME);
		break;
	case MV_6530_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6530_Z1_NAME);
		break;
	case MV_6550_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6550_Z1_NAME);
		break;
	case MV_6560_Z1_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6560_Z1_NAME);
		break;
	case MV_6510_Z2_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6510_Z2_NAME);
		break;
	case MV_6530_Z2_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6530_Z2_NAME);
		break;
	case MV_6550_Z2_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6550_Z2_NAME);
		break;
	case MV_6560_Z2_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6560_Z2_NAME);
		break;

	case MV_6510_A0_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6510_A0_NAME);
		break;
	case MV_6530_A0_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6530_A0_NAME);
		break;
	case MV_6550_A0_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6550_A0_NAME);
		break;
	case MV_6560_A0_ID:
		mvOsSPrintf(pNameBuff, "%s", MV_6560_A0_NAME);
		break;

	default:
		mvCtrlNameGet(pNameBuff);
		break;
	}

	return MV_OK;
}

static const char *cntrlName[] = TARGETS_NAME_ARRAY;

/*******************************************************************************
* mvCtrlTargetNameGet - Get Marvell controller target name
*
* DESCRIPTION:
*       This function convert the trget enumeration to string.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Target name (const MV_8 *)
*******************************************************************************/
const MV_8 *mvCtrlTargetNameGet(MV_TARGET target)
{
	if (target >= MAX_TARGETS)
		return "target unknown";

	return cntrlName[target];
}

/*******************************************************************************
* mvCtrlPexAddrDecShow - Print the PEX address decode map (BARs and windows).
*
* DESCRIPTION:
*		This function print the PEX address decode map (BARs and windows).
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
static MV_VOID mvCtrlPexAddrDecShow(MV_VOID)
{
	MV_PEX_BAR pexBar;
	MV_PEX_DEC_WIN win;
	MV_U32 pexIf;
	MV_U32 bar, winNum;

	for (pexIf = 0; pexIf < mvCtrlPexMaxIfGet(); pexIf++) {
		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexIf))
			continue;
		mvOsOutput("\n");
		mvOsOutput("PEX%d:\n", pexIf);
		mvOsOutput("-----\n");

		mvOsOutput("\nPex Bars \n\n");

		for (bar = 0; bar < PEX_MAX_BARS; bar++) {
			memset(&pexBar, 0, sizeof(MV_PEX_BAR));

			mvOsOutput("%s ", pexBarNameGet(bar));

			if (mvPexBarGet(pexIf, bar, &pexBar) == MV_OK) {
				if (pexBar.enable) {
					mvOsOutput("base %08x, ", pexBar.addrWin.baseLow);
					mvSizePrint(pexBar.addrWin.size);
					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}
		mvOsOutput("\nPex Decode Windows\n\n");

		for (winNum = 0; winNum < PEX_MAX_TARGET_WIN - 2; winNum++) {
			memset(&win, 0, sizeof(MV_PEX_DEC_WIN));

			mvOsOutput("win%d - ", winNum);

			if (mvPexTargetWinRead(pexIf, winNum, &win) == MV_OK) {
				if (win.winInfo.enable) {
					mvOsOutput("%s base %08x, ",
						   mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win.winInfo)),
						   win.winInfo.addrWin.baseLow);
					mvOsOutput("....");
					mvSizePrint(win.winInfo.addrWin.size);

					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}

		memset(&win, 0, sizeof(MV_PEX_DEC_WIN));

		mvOsOutput("default win - ");

		if (mvPexTargetWinRead(pexIf, MV_PEX_WIN_DEFAULT, &win) == MV_OK) {
			mvOsOutput("%s ", mvCtrlTargetNameGet(win.target));
			mvOsOutput("\n");
		}
		memset(&win, 0, sizeof(MV_PEX_DEC_WIN));

		mvOsOutput("Expansion ROM - ");

		if (mvPexTargetWinRead(pexIf, MV_PEX_WIN_EXP_ROM, &win) == MV_OK) {
			mvOsOutput("%s ", mvCtrlTargetNameGet(win.target));
			mvOsOutput("\n");
		}
	}
}

/*******************************************************************************
* mvUnitAddrDecShow - Print the Unit's address decode map.
*
* DESCRIPTION:
*       This is a generic function for printing the different unit's address
*	decode map.
*
* INPUT:
*       unit	- The unit to print the address decode for.
*	name	- The unit's name.
*	winGetFuncPtr - A pointer to the HAL's window get function.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
static void mvUnitAddrDecShow(MV_U8 numUnits, MV_UNIT_ID unitId, const char *name, MV_WIN_GET_FUNC_PTR winGetFuncPtr)
{
	MV_UNIT_WIN_INFO win;
	MV_U32 unit, i;

	for (unit = 0; unit < numUnits; unit++) {

		if (MV_FALSE == mvCtrlPwrClckGet(unitId, unit))
			continue;
		mvOsOutput("\n");
		mvOsOutput("%s %d:\n", name, unit);
		mvOsOutput("----\n");

		for (i = 0; i < 16; i++) {
			memset(&win, 0, sizeof(MV_UNIT_WIN_INFO));

			mvOsOutput("win%d - ", i);

			if (winGetFuncPtr(unit, i, &win) == MV_OK) {
				if (win.enable) {
					mvOsOutput("%s base %08x, ",
						   mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win)),
						   win.addrWin.baseLow);
					mvOsOutput("....");
					mvSizePrint(win.addrWin.size);
					mvOsOutput("\n");
				} else
					mvOsOutput("disable\n");
			}
		}
	}
	return;
}

/*******************************************************************************
* mvCtrlAddrDecShow - Print the Controller units address decode map.
*
* DESCRIPTION:
*		This function the Controller units address decode map.
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
MV_VOID mvCtrlAddrDecShow(MV_VOID)
{
	mvCpuIfAddDecShow();
	mvAhbToMbusAddDecShow();
#if defined(MV_INCLUDE_PEX)
	mvCtrlPexAddrDecShow();
#endif
#if defined(MV_INCLUDE_USB)
	mvUnitAddrDecShow(mvCtrlUsbMaxGet(), USB_UNIT_ID, "USB", mvUsbWinRead);
#endif

#if defined(MV_INCLUDE_GIG_ETH)
#if defined(CONFIG_MV_ETH_LEGACY)
	mvUnitAddrDecShow(mvCtrlEthMaxPortGet(), ETH_GIG_UNIT_ID, "ETH", mvEthWinRead);
#else
	mvUnitAddrDecShow(mvCtrlEthMaxPortGet(), ETH_GIG_UNIT_ID, "ETH", mvNetaWinRead);
#endif /* CONFIG_MV_ETH_LEGACY */
#endif /* MV_INCLUDE_GIG_ETH */

#if defined(MV_INCLUDE_XOR)
	mvUnitAddrDecShow(mvCtrlXorMaxChanGet(), XOR_UNIT_ID, "XOR", mvXorTargetWinRead);
#endif
#if defined(MV_INCLUDE_SATA)
	mvUnitAddrDecShow(mvCtrlSataMaxPortGet(), SATA_UNIT_ID, "Sata", mvSataWinRead);
#endif
#if defined(MV_INCLUDE_TS)
	mvUnitAddrDecShow(1, TS_UNIT_ID, "TS", mvTsuWinRead);
#endif
}

/*******************************************************************************
* ctrlSizeToReg - Extract size value for register assignment.
*
* DESCRIPTION:
*       Address decode size parameter must be programed from LSB to MSB as
*       sequence of 1's followed by sequence of 0's. The number of 1's
*       specifies the size of the window in 64 KB granularity (e.g. a
*       value of 0x00ff specifies 256x64k = 16 MB).
*       This function extract the size value from the size parameter according
*		to given aligment paramter. For example for size 0x1000000 (16MB) and
*		aligment 0x10000 (64KB) the function will return 0x00FF.
*
* INPUT:
*       size - Size.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size register value correspond to size parameter.
*		If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32 ctrlSizeToReg(MV_U32 size, MV_U32 alignment)
{
	MV_U32 retVal;

	/* Check size parameter alignment               */
	if ((0 == size) || (MV_IS_NOT_ALIGN(size, alignment))) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size is zero or not aligned.\n"));
		return -1;
	}

	/* Take out the "alignment" portion out of the size parameter */
	alignment--;		/* Now the alignmet is a sequance of '1' (e.g. 0xffff)          */
	/* and size is 0x1000000 (16MB) for example     */
	while (alignment & 1) {	/* Check that alignmet LSB is set       */
		size = (size >> 1);	/* If LSB is set, move 'size' one bit to right      */
		alignment = (alignment >> 1);
	}

	/* If after the alignment first '0' was met we still have '1' in                */
	/* it then aligment is invalid (not power of 2)                                 */
	if (alignment) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", (MV_U32) alignment));
		return -1;
	}

	/* Now the size is shifted right according to aligment: 0x0100                  */
	size--;			/* Now the size is a sequance of '1': 0x00ff                    */
	retVal = size;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's         */
	while (size & 1)	/* Check that LSB is set    */
		size = (size >> 1);	/* If LSB is set, move one bit to the right         */

	if (size) {		/* Sequance of 1's is over. Check that we have no other 1's         */
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Size parameter 0x%x invalid.\n", size));
		return -1;
	}
	return retVal;
}

/*******************************************************************************
* ctrlRegToSize - Extract size value from register value.
*
* DESCRIPTION:
*       This function extract a size value from the register size parameter
*		according to given aligment paramter. For example for register size
*		value 0xff and aligment 0x10000 the function will return 0x01000000.
*
* INPUT:
*       regSize   - Size as in register format.	See ctrlSizeToReg.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size.
*		If value is '-1' size parameter or aligment are invalid.
*******************************************************************************/
MV_U32 ctrlRegToSize(MV_U32 regSize, MV_U32 alignment)
{
	MV_U32 temp;

	/* Check that LSB to MSB is sequence of 1's followed by sequence of 0's         */
	temp = regSize;		/* Now the size is a sequance of '1': 0x00ff            */

	while (temp & 1)	/* Check that LSB is set                                    */
		temp = (temp >> 1);	/* If LSB is set, move one bit to the right         */

	if (temp) {		/* Sequance of 1's is over. Check that we have no other 1's         */
		DB(mvOsPrintf("ctrlRegToSize: ERR. Size parameter 0x%x invalid.\n", regSize));
		return -1;
	}

	/* Check that aligment is a power of two                                        */
	temp = alignment - 1;	/* Now the alignmet is a sequance of '1' (0xffff)          */

	while (temp & 1)	/* Check that alignmet LSB is set                           */
		temp = (temp >> 1);	/* If LSB is set, move 'size' one bit to right      */

	/* If after the 'temp' first '0' was met we still have '1' in 'temp'            */
	/* then 'temp' is invalid (not power of 2)                                      */
	if (temp) {
		DB(mvOsPrintf("ctrlSizeToReg: ERR. Alignment parameter 0x%x invalid.\n", alignment));
		return -1;
	}

	regSize++;		/* Now the size is 0x0100                                       */

	/* Add in the "alignment" portion to the register size parameter                */
	alignment--;		/* Now the alignmet is a sequance of '1' (e.g. 0xffff)          */

	while (alignment & 1) {	/* Check that alignmet LSB is set                       */
		regSize = (regSize << 1);	/* LSB is set, move 'size' one bit left     */
		alignment = (alignment >> 1);
	}

	return regSize;
}

/*******************************************************************************
* ctrlSizeRegRoundUp - Round up given size
*
* DESCRIPTION:
*       This function round up a given size to a size that fits the
*       restrictions of size format given an aligment parameter.
*		to given aligment paramter. For example for size parameter 0xa1000 and
*		aligment 0x1000 the function will return 0xFF000.
*
* INPUT:
*       size - Size.
*		alignment - Size alignment.	Note that alignment must be power of 2!
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit describing size value correspond to size in register.
*******************************************************************************/
MV_U32 ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment)
{
	MV_U32 msbBit = 0;
	MV_U32 retSize;

	/* Check if size parameter is already comply with restriction   */
	if (!(-1 == ctrlSizeToReg(size, alignment)))
		return size;

	while (size) {
		size = (size >> 1);
		msbBit++;
	}

	retSize = (1 << msbBit);

	if (retSize < alignment)
		return alignment;
	else
		return retSize;
}

/*******************************************************************************
* mvCtrlIsBootFromSPI
*
* DESCRIPTION:
*       Check if device is configured to boot from SPI flash according to the
*	SAR registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if device boot from SPI.
*******************************************************************************/
MV_BOOL mvCtrlIsBootFromSPI(MV_VOID)
{
	MV_U32 satr;

	satr = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));

	satr = (satr & MSAR_BOOT_MODE_MASK) >> MSAR_BOOT_MODE_OFFS;
	satr = (1 << satr);
	if (satr & MSAR_BOOT_SPI_W_BOOTROM_MASK)
		return MV_TRUE;
	else
		return MV_FALSE;
}

/*******************************************************************************
* mvCtrlIsBootFromNAND
*
* DESCRIPTION:
*       Check if device is confiogured to boot from NAND flash according to the SAR
*	registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if device boot from NAND.
*******************************************************************************/
MV_BOOL mvCtrlIsBootFromNAND(MV_VOID)
{
	MV_U32 satr;
	MV_U32 i;
	MV_U32 satrList[] = MSAR_BOOT_NAND_W_BOOTROM_LIST;
	MV_U32 satrListLegacy[] = MSAR_BOOT_NAND_LEG_W_BOOTROM_LIST;

	satr = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));

	satr = (satr & MSAR_BOOT_MODE_MASK) >> MSAR_BOOT_MODE_OFFS;
	for (i = 0; i < MV_ARRAY_SIZE(satrList); i++) {
		if (satrList[i] == satr)
			return MV_TRUE;
	}

	for (i = 0; i < MV_ARRAY_SIZE(satrListLegacy); i++) {
		if (satrListLegacy[i] == satr)
			return MV_TRUE;
	}

	return MV_FALSE;
}

/*******************************************************************************
* mvCtrlIsBootFromNOR
*
* DESCRIPTION:
*       Check if device is confiogured to boot from NOR flash according to the SAR
*	registers.
*
* INPUT:
*	None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if device is booting from NOR.
*******************************************************************************/
MV_BOOL mvCtrlIsBootFromNOR(MV_VOID)
{
	MV_U32 satr;
	MV_U32 i;
	MV_U32 satrList[] = MSAR_BOOT_NOR_W_BOOTROM_LIST;

	satr = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));

	satr = (satr & MSAR_BOOT_MODE_MASK) >> MSAR_BOOT_MODE_OFFS;
	for (i = 0; i < MV_ARRAY_SIZE(satrList); i++) {
		if (satrList[i] == satr) {
			if ((satr >> 4) == 0x1)
				return MV_NOR_LOW_MPPS;
			if ((satr >> 4) == 0x2)
				return MV_NOR_HIGH_MPPS;
		}
	}
	return MV_FALSE;
}

#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
/*******************************************************************************
* mvCtrlPwrSaveOn - Set Power save mode
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID mvCtrlPwrSaveOn(MV_VOID)
{
	unsigned long old, temp;
	MV_U32 reg;

	/* Disable int */
	__asm__ __volatile__("mrs %0, cpsr\n" "orr %1, %0, #0xc0\n" "msr cpsr_c, %1" : "=r"(old), "=r"(temp)
			     : : "memory");

	MV_REG_BIT_SET(CPU_CTRL_STAT_REG, CCSR_CPU_SW_INT_BLK_MASK);

	reg = MV_REG_READ(POWER_MNG_CTRL_REG);

	if (!(reg & PMC_POWERSAVE_MASK)) {
		/* Set SoC in power save */
		MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_POWERSAVE_MASK);

		/* Wait for int */
		__asm__ __volatile__("mcr    p15, 0, r0, c7, c0, 4");
	}

	MV_REG_BIT_RESET(CPU_CTRL_STAT_REG, CCSR_CPU_SW_INT_BLK_MASK);

	/* Enabled int */
	__asm__ __volatile__("msr cpsr_c, %0" : : "r"(old)
			     : "memory");
}


/*******************************************************************************
* mvCtrlPwrSaveOff - Go out of power save mode
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID mvCtrlPwrSaveOff(MV_VOID)
{
	unsigned long old, temp;
	MV_U32 reg;

	/* Disable int */
	__asm__ __volatile__("mrs %0, cpsr\n" "orr %1, %0, #0xc0\n" "msr cpsr_c, %1" : "=r"(old), "=r"(temp)
			     : : "memory");
	MV_REG_BIT_SET(CPU_CTRL_STAT_REG, CCSR_CPU_SW_INT_BLK_MASK);

	reg = MV_REG_READ(POWER_MNG_CTRL_REG);

	if (reg & PMC_POWERSAVE_MASK) {
		/* Set SoC in power save */
		MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_POWERSAVE_MASK);

		/* Wait for int */
		__asm__ __volatile__("mcr    p15, 0, r0, c7, c0, 4");
	}

	MV_REG_BIT_RESET(CPU_CTRL_STAT_REG, CCSR_CPU_SW_INT_BLK_MASK);

	/* Enabled int */
	__asm__ __volatile__("msr cpsr_c, %0" : : "r"(old)
			     : "memory");

}

/*******************************************************************************
* mvCtrlPwrClckSet - Set Power State for specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_PEXSTOPCLOCK_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_GESTOPCLOCK_MASK(index));
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_GESTOPCLOCK_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SATASTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SATASTOPCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SESTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SESTOPCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_USBSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_USBSTOPCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_SDIOSTOPCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_TDM)
	case TDM_2CH_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_TDMSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_TDMSTOPCLOCK_MASK);
		break;
	case TDM_32CH_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_COMMSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_COMMSTOPCLOCK_MASK);
		break;
#endif
#if defined(MV_INCLUDE_PON)
	case XPON_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_PONSTOPCLOCK_MASK);
		else
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_PONSTOPCLOCK_MASK);
		break;
#endif
	case NFC_UNIT_ID:
		if (enable == MV_FALSE) {
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_NFCSTOPCLOCK_MASK);
			MV_REG_BIT_RESET(POWER_MNG_CTRL_REG, PMC_NFECCSTOPCLOCK_MASK);
		} else {
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_NFCSTOPCLOCK_MASK);
			MV_REG_BIT_SET(POWER_MNG_CTRL_REG, PMC_NFECCSTOPCLOCK_MASK);
		}
		break;
	default:
		break;
	}
}

/*******************************************************************************
* mvCtrlPwrClckGet - Get Power State of specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg = MV_REG_READ(POWER_MNG_CTRL_REG);
	MV_BOOL state = MV_TRUE;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEXSTOPCLOCK_MASK(index)) == PMC_PEXSTOPCLOCK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if ((reg & PMC_GESTOPCLOCK_MASK(index)) == PMC_GESTOPCLOCK_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & PMC_SATASTOPCLOCK_MASK) == PMC_SATASTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if ((reg & PMC_SESTOPCLOCK_MASK) == PMC_SESTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & PMC_USBSTOPCLOCK_MASK) == PMC_USBSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SDIO)
	case SDIO_UNIT_ID:
		if ((reg & PMC_SDIOSTOPCLOCK_MASK) == PMC_SDIOSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_TDM)
	case TDM_2CH_UNIT_ID:
		if ((reg & PMC_TDMSTOPCLOCK_MASK) == PMC_TDMSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;

	case TDM_32CH_UNIT_ID:
		if ((reg & PMC_COMMSTOPCLOCK_MASK) == PMC_COMMSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_PON)
	case XPON_UNIT_ID:
		if ((reg & PMC_PONSTOPCLOCK_MASK) == PMC_PONSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
	case NFC_UNIT_ID:
		if ((reg & PMC_NFCSTOPCLOCK_MASK) == PMC_NFCSTOPCLOCK_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
	default:
		state = MV_TRUE;
		break;
	}

	return state;
}

/*******************************************************************************
* mvCtrlPwrMemSet - Set Power State for memory on specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
*******************************************************************************/
MV_VOID mvCtrlPwrMemSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_PEXSTOPMEM_MASK(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_PEXSTOPMEM_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_GESTOPMEM_MASK(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_GESTOPMEM_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_INTEG_SATA)
	case SATA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_SATASTOPMEM_MASK);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_SATASTOPMEM_MASK);
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_SESTOPMEM_MASK);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_SESTOPMEM_MASK);
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_USBSTOPMEM_MASK);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_USBSTOPMEM_MASK);
		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_XORSTOPMEM_MASK(index));
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_XORSTOPMEM_MASK(index));
		break;
#endif
#if defined(MV_INCLUDE_PON)
	case XPON_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_PONSTOPMEM_MASK);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_PONSTOPMEM_MASK);
		break;
#endif
#if defined(MV_INCLUDE_BM)
	case BM_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_BMSTOPMEM_MASK);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_BMSTOPMEM_MASK);
		break;
#endif
#if defined(MV_INCLUDE_PNC)
	case PNC_UNIT_ID:
		if (enable == MV_FALSE)
			MV_REG_BIT_SET(POWER_MNG_MEM_CTRL_REG, PMC_PNCSTOPMEM_MASK);
		else
			MV_REG_BIT_RESET(POWER_MNG_MEM_CTRL_REG, PMC_PNCSTOPMEM_MASK);
		break;
#endif
	default:
		break;
	}
}

/*******************************************************************************
* mvCtrlPwrMemGet - Get Power State of memory on specific Unit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*
* RETURN:
******************************************************************************/
MV_BOOL mvCtrlPwrMemGet(MV_UNIT_ID unitId, MV_U32 index)
{
	MV_U32 reg = MV_REG_READ(POWER_MNG_MEM_CTRL_REG);
	MV_BOOL state = MV_TRUE;

	switch (unitId) {
#if defined(MV_INCLUDE_PEX)
	case PEX_UNIT_ID:
		if ((reg & PMC_PEXSTOPMEM_MASK(index)) == PMC_PEXSTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_GIG_ETH)
	case ETH_GIG_UNIT_ID:
		if ((reg & PMC_GESTOPMEM_MASK(index)) == PMC_GESTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_SATA)
	case SATA_UNIT_ID:
		if ((reg & PMC_SATASTOPMEM_MASK) == PMC_SATASTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_CESA)
	case CESA_UNIT_ID:
		if ((reg & PMC_SESTOPMEM_MASK) == PMC_SESTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_USB)
	case USB_UNIT_ID:
		if ((reg & PMC_USBSTOPMEM_MASK) == PMC_USBSTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_XOR)
	case XOR_UNIT_ID:
		if ((reg & PMC_XORSTOPMEM_MASK(index)) == PMC_XORSTOPMEM_STOP(index))
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_PON)
	case XPON_UNIT_ID:
		if ((reg & PMC_PONSTOPMEM_MASK) == PMC_PONSTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_BM)
	case BM_UNIT_ID:
		if ((reg & PMC_BMSTOPMEM_MASK) == PMC_BMSTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
#if defined(MV_INCLUDE_PNC)
	case PNC_UNIT_ID:
		if ((reg & PMC_PNCSTOPMEM_MASK) == PMC_PNCSTOPMEM_STOP)
			state = MV_FALSE;
		else
			state = MV_TRUE;
		break;
#endif
	default:
		state = MV_TRUE;
		break;
	}

	return state;
}
#else
MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable)
{
	return;
}

MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index)
{
	return MV_TRUE;
}
#endif /* #if defined(MV_INCLUDE_CLK_PWR_CNTRL) */

/*******************************************************************************
* mvCtrlIsGponMode
*
* DESCRIPTION:
*	returns the GPON/EPON mode select.
*
* INPUT:
*	None.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_TRUE - GPON.
*	MV_FALSE - EPON.
******************************************************************************/
MV_BOOL mvCtrlIsGponMode(void)
{
	MV_U32 reg = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));

	if (MV_GET_BIT(reg, MPP_GPON_MODE_SEL_OFFS) == 0)
		return MV_TRUE;	/* 0 - GPON mode */

	return MV_FALSE;	/* 1 - EPON mode */
}
