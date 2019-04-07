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
#include "mvCommon.h"
#include "mvBoardEnvLib.h"
#include "mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"

#define DB_88F6535_BOARD_TWSI_DEF_NUM		0x5
#define DB_88F6535_BOARD_SWITCH_NUM         	0x1
#define DB_88F6535_BOARD_MAC_INFO_NUM		0x2
#define DB_88F6535_BOARD_GPP_INFO_NUM		0x4
#define DB_88F6535_BOARD_MPP_CONFIG_NUM		0x1
#define DB_88F6535_BOARD_MPP_GROUP_TYPE_NUM	0x1

#define DB_88F6535_BOARD_DEVICE_CONFIG_NUM	0x1
#define DB_88F6535_BOARD_DEBUG_LED_NUM		0x0

#define DB_88F6535_BOARD_NAND_READ_PARAMS	0x000C0282
#define DB_88F6535_BOARD_NAND_WRITE_PARAMS	0x00010305
/* NAND care support for small page chips */
#define DB_88F6535_BOARD_NAND_CONTROL		0x01c00543

MV_U8 mvDbDisableModuleDetection = 0;

MV_BOARD_TWSI_INFO db88f6535InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
	{BOARD_DEV_TWSI_SATR, 0x4C, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4D, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4E, ADDR7_BIT},
	{BOARD_DEV_TWSI_SATR, 0x4F, ADDR7_BIT},
	{BOARD_TWSI_MUX, 0x70, ADDR7_BIT}
};

MV_BOARD_MAC_INFO db88f6535InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_AUTO, 0x8},
	{BOARD_MAC_SPEED_AUTO, 0x9}
};

MV_BOARD_MPP_TYPE_INFO db88f6535InfoBoardMppTypeInfo[] = {
	{
		.boardMppTdm = MV_BOARD_AUTO,
		.ethSataComplexOpt = ESC_OPT_AUTO
	}
};

MV_BOARD_GPP_INFO db88f6535InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
	{BOARD_GPP_USB_VBUS, 31},
	{BOARD_GPP_PEX_RESET, 67},
	{BOARD_GPP_SDIO_DETECT, 68},
	{BOARD_GPP_SDIO_WP, 69}
};

MV_DEV_CS_INFO db88f6535InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MTD_NAND_LNC)
	{NAND_NOR_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#elif defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#elif defined(MV_NOR)
	{NAND_NOR_CS, N_A, BOARD_DEV_NOR_FLASH, 16}	/* NOR DEV */
#else
	{N_A, N_A, N_A, N_A}			/* No device */
#endif
};

MV_BOARD_MPP_INFO db88f6535InfoBoardMppConfigValue[] = {
	{{
	  DB_88F6535_MPP0_7,
	  DB_88F6535_MPP8_15,
	  DB_88F6535_MPP16_23,
	  DB_88F6535_MPP24_31,
	  DB_88F6535_MPP32_39,
	  DB_88F6535_MPP40_47,
	  DB_88F6535_MPP48_55,
	  DB_88F6535_MPP56_63,
	  DB_88F6535_MPP64_71,
	  DB_88F6535_MPP72_79,
	  DB_88F6535_MPP80_87,
	  DB_88F6535_MPP88_88
	  }
	 }
};

MV_BOARD_SWITCH_INFO db88f6535InfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {1, 2, 3, 0, -1, -1, -1, -1},
	 .cpuPort = 4,
	 .connectedPort = {4, 5, -1},
	 .smiScanMode = 2,
	 .quadPhyAddr = 0
	 }
};

MV_BOARD_TDM_INFO db88f6535Tdm880[] = {
	{1},
	{2}
};

MV_BOARD_TDM_INFO db88f6535Tdm792[] = {
	{1},
	{2},
	{3},
	{4},
	{6},
	{7}
};

MV_BOARD_TDM_INFO db88f6535Tdm3215[] = { {1} };

MV_BOARD_TDM_SPI_INFO db88f6535TdmSpiInfo[] = { {0} };

MV_BOARD_SPEC_INIT db88f6535BoardSpecInit[] = {
	{
		.reg = PMU_POWER_IF_POLARITY_REG,
		.mask = (BIT1),
		.val = 0
	},
	{
		.reg = TBL_TERM,
		.val = TBL_TERM
	}
};

MV_BOARD_INFO db88f6535Info = {
	.boardName = "DB-88F6535P",
	.numBoardMppTypeValue = DB_88F6535_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = db88f6535InfoBoardMppTypeInfo,
	.numBoardMppConfigValue = DB_88F6535_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = db88f6535InfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = DB_88F6535_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = db88f6535InfoBoardDeCsInfo,
	.numBoardTwsiDev = DB_88F6535_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = db88f6535InfoBoardTwsiDev,
	.numBoardMacInfo = DB_88F6535_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = db88f6535InfoBoardMacInfo,
	.numBoardGppInfo = DB_88F6535_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = db88f6535InfoBoardGppInfo,
	.activeLedsNumber = DB_88F6535_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = NULL,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = DB_88F6535_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = DB_88F6535_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = DB_88F6535_GPP_OUT_ENA_HIGH,
	.gppOutValLow = DB_88F6535_GPP_OUT_VAL_LOW,
	.gppOutValMid = DB_88F6535_GPP_OUT_VAL_MID,
	.gppOutValHigh = DB_88F6535_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = DB_88F6535_GPP_POL_LOW,
	.gppPolarityValMid = DB_88F6535_GPP_POL_MID,
	.gppPolarityValHigh = DB_88F6535_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = db88f6535InfoBoardSwitchValue,
	.switchInfoNum = DB_88F6535_BOARD_SWITCH_NUM,

	/* PON configuration. */
	.ponConfigValue = BOARD_PON_AUTO,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {2, 6, 1},
	.pBoardTdmInt2CsInfo = {db88f6535Tdm880,
				db88f6535Tdm792,
				db88f6535Tdm3215},
	.boardTdmInfoIndex = -1,

	.pBoardSpecInit = db88f6535BoardSpecInit,

	/* NAND init params */
	.nandFlashReadParams = DB_88F6535_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = DB_88F6535_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = DB_88F6535_BOARD_NAND_CONTROL,
	.pBoardTdmSpiInfo = db88f6535TdmSpiInfo
};

/* RD-88F6510-SFU */
#define RD_88F6510_BOARD_TWSI_DEF_NUM		0x0
#define RD_88F6510_BOARD_SWITCH_NUM		0x1
#define RD_88F6510_BOARD_MAC_INFO_NUM		0x2
#define RD_88F6510_BOARD_GPP_INFO_NUM		0x1
#define RD_88F6510_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6510_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6510_BOARD_DEVICE_CONFIG_NUM	0x1
#define RD_88F6510_BOARD_DEBUG_LED_NUM		0x0

#define RD_88F6510_BOARD_NAND_READ_PARAMS	0x000C0282
#define RD_88F6510_BOARD_NAND_WRITE_PARAMS	0x00010305
/* NAND care support for small page chips */
#define RD_88F6510_BOARD_NAND_CONTROL		0x01c00543

MV_BOARD_TWSI_INFO rd88f6510InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
};

MV_BOARD_MAC_INFO rd88f6510InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x8}
	,
	{BOARD_MAC_SPEED_1000M, 0x9}
/* 	{BOARD_MAC_SPEED_AUTO, 0x9} */

};

MV_BOARD_MPP_TYPE_INFO rd88f6510InfoBoardMppTypeInfo[] = {
	{
	 .boardMppTdm = MV_BOARD_TDM_2CH,
	 .ethSataComplexOpt = ESC_OPT_MAC0_2_SW_P4 | ESC_OPT_MAC1_2_SW_P5 | ESC_OPT_FE3PHY | ESC_OPT_GEPHY_SW_P0
	 }
};

MV_BOARD_GPP_INFO rd88f6510InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
	{BOARD_GPP_PEX_RESET, 67}
};

MV_DEV_CS_INFO rd88f6510InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MTD_NAND_LNC)
	{NAND_NOR_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#elif defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}		/* SPI DEV */
#else
	{N_A, N_A, N_A, N_A}			/* No device */
#endif
};

MV_BOARD_MPP_INFO rd88f6510InfoBoardMppConfigValue[] = {
	{{
		RD_88F6510_MPP0_7,
		RD_88F6510_MPP8_15,
		RD_88F6510_MPP16_23,
		RD_88F6510_MPP24_31,
		RD_88F6510_MPP32_39,
		RD_88F6510_MPP40_47,
		RD_88F6510_MPP48_55,
		RD_88F6510_MPP56_63,
		RD_88F6510_MPP64_69,
		RD_88F6510_MPP72_79,
		RD_88F6510_MPP80_87,
		RD_88F6510_MPP88_88
	  }
	 }
};

MV_BOARD_SWITCH_INFO rd88f6510InfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {1, 2, 3, 0, -1, -1, -1, -1}
	 ,
	 .cpuPort = 4,
	 .connectedPort = {4, 5, -1}
	 ,
	 .smiScanMode = 2,
	 .quadPhyAddr = 0
	 }
};

MV_BOARD_TDM_INFO rd88f6510Tdm880[] = { {1} };

MV_BOARD_TDM_SPI_INFO rd88f6510TdmSpiInfo[] = { {0} };

MV_BOARD_INFO rd88f6510Info = {
	.boardName = "RD-88F6510-SFU",
	.numBoardMppTypeValue = RD_88F6510_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = rd88f6510InfoBoardMppTypeInfo,
	.numBoardMppConfigValue = RD_88F6510_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = rd88f6510InfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = RD_88F6510_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = rd88f6510InfoBoardDeCsInfo,
	.numBoardTwsiDev = RD_88F6510_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = rd88f6510InfoBoardTwsiDev,
	.numBoardMacInfo = RD_88F6510_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = rd88f6510InfoBoardMacInfo,
	.numBoardGppInfo = RD_88F6510_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = rd88f6510InfoBoardGppInfo,
	.activeLedsNumber = RD_88F6510_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = NULL,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = RD_88F6510_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = RD_88F6510_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = RD_88F6510_GPP_OUT_ENA_HIGH,
	.gppOutValLow = RD_88F6510_GPP_OUT_VAL_LOW,
	.gppOutValMid = RD_88F6510_GPP_OUT_VAL_MID,
	.gppOutValHigh = RD_88F6510_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = RD_88F6510_GPP_POL_LOW,
	.gppPolarityValMid = RD_88F6510_GPP_POL_MID,
	.gppPolarityValHigh = RD_88F6510_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = rd88f6510InfoBoardSwitchValue,
	.switchInfoNum = RD_88F6510_BOARD_SWITCH_NUM,

	/* PON configuration. */
	.ponConfigValue = BOARD_PON_AUTO,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {1}
	,
	.pBoardTdmInt2CsInfo = {rd88f6510Tdm880,
				}
	,
	.boardTdmInfoIndex = 0,

	/* NAND init params */
	.nandFlashReadParams = RD_88F6510_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = RD_88F6510_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = RD_88F6510_BOARD_NAND_CONTROL,
	.pBoardTdmSpiInfo = rd88f6510TdmSpiInfo
};

/* RD-88F6560-GW */
#define RD_88F6560_BOARD_TWSI_DEF_NUM		0x0
#define RD_88F6560_BOARD_SWITCH_NUM		0x1
#define RD_88F6560_BOARD_MAC_INFO_NUM		0x2
#define RD_88F6560_BOARD_GPP_INFO_NUM		11
#define RD_88F6560_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6560_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6560_BOARD_DEVICE_CONFIG_NUM	0x1
#define RD_88F6560_BOARD_DEBUG_LED_NUM		0x1

#define RD_88F6560_BOARD_NAND_READ_PARAMS	0x000C0282
#define RD_88F6560_BOARD_NAND_WRITE_PARAMS	0x00010305
/* NAND care support for small page chips */
#define RD_88F6560_BOARD_NAND_CONTROL		0x01c00543

MV_BOARD_TWSI_INFO rd88f6560InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
};

MV_BOARD_MAC_INFO rd88f6560InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x8}
	,
	{BOARD_MAC_SPEED_AUTO, 0x9}
};

MV_BOARD_MPP_TYPE_INFO rd88f6560InfoBoardMppTypeInfo[] = {
	{
	 .boardMppTdm = MV_BOARD_TDM_2CH,
	 .ethSataComplexOpt = ESC_OPT_GEPHY_MAC1 | ESC_OPT_RGMIIA_MAC0,
	}
};

MV_BOARD_GPP_INFO rd88f6560InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
	{BOARD_GPP_SDIO_WP, 31},
	{BOARD_GPP_HDD_POWER, 37},
	{BOARD_GPP_FAN_POWER, 40},
	{BOARD_GPP_WPS_BUTTON, 47},
	{BOARD_GPP_SDIO_DETECT, 48},
	{BOARD_GPP_USB_VBUS, 58},
	{BOARD_GPP_USB_OC, 59},
	{BOARD_GPP_CONF, 60},
	{BOARD_GPP_CONF, 61},
	{BOARD_GPP_CONF, 62},
	{BOARD_GPP_PEX_RESET, 77}
};

MV_DEV_CS_INFO rd88f6560InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MTD_NAND_LNC)
	{NAND_NOR_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#elif defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#else
	{N_A, N_A, N_A, N_A}			/* No device */
#endif
};

MV_BOARD_MPP_INFO rd88f6560InfoBoardMppConfigValue[] = {
	{{
	  RD_88F6560_MPP0_7,
	  RD_88F6560_MPP8_15,
	  RD_88F6560_MPP16_23,
	  RD_88F6560_MPP24_31,
	  RD_88F6560_MPP32_39,
	  RD_88F6560_MPP40_47,
	  RD_88F6560_MPP48_55,
	  RD_88F6560_MPP56_63,
	  RD_88F6560_MPP64_71,
	  RD_88F6560_MPP72_79,
	  RD_88F6560_MPP80_87,
	  RD_88F6560_MPP88_88
	  }
	 }
};

MV_BOARD_SWITCH_INFO rd88f6560InfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {1, 2, 3, 0, -1, -1, -1, -1}
	 ,
	 .cpuPort = 4,
	 .connectedPort = {4, 5, -1}
	 ,
	 .smiScanMode = 2,
	 .quadPhyAddr = 0
	 }
};

MV_U8 rd88f6560LedGppPin[] = { 68 };

MV_BOARD_TDM_INFO rd88f6560Tdm880[] = { {1} };

MV_BOARD_TDM_SPI_INFO rd88f6560TdmSpiInfo[] = { {1} };

MV_BOARD_SPEC_INIT rd88f6560BoardSpecInit[] = {
	{
		.reg = PMU_POWER_IF_POLARITY_REG,
		.mask = (BIT1),
		.val = 0
	},
	{
		.reg = TBL_TERM,
		.val = TBL_TERM
	}
};

MV_BOARD_INFO rd88f6560Info = {
	.boardName = "RD-88F6560-GW",
	.numBoardMppTypeValue = RD_88F6560_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = rd88f6560InfoBoardMppTypeInfo,
	.numBoardMppConfigValue = RD_88F6560_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = rd88f6560InfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = RD_88F6560_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = rd88f6560InfoBoardDeCsInfo,
	.numBoardTwsiDev = RD_88F6560_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = rd88f6560InfoBoardTwsiDev,
	.numBoardMacInfo = RD_88F6560_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = rd88f6560InfoBoardMacInfo,
	.numBoardGppInfo = RD_88F6560_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = rd88f6560InfoBoardGppInfo,
	.activeLedsNumber = RD_88F6560_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = rd88f6560LedGppPin,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = RD_88F6560_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = RD_88F6560_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = RD_88F6560_GPP_OUT_ENA_HIGH,
	.gppOutValLow = RD_88F6560_GPP_OUT_VAL_LOW,
	.gppOutValMid = RD_88F6560_GPP_OUT_VAL_MID,
	.gppOutValHigh = RD_88F6560_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = RD_88F6560_GPP_POL_LOW,
	.gppPolarityValMid = RD_88F6560_GPP_POL_MID,
	.gppPolarityValHigh = RD_88F6560_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = rd88f6560InfoBoardSwitchValue,
	.switchInfoNum = RD_88F6560_BOARD_SWITCH_NUM,

	/* PON configuration. */
	.ponConfigValue = BOARD_GPON_CONFIG,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {1},
	.pBoardTdmInt2CsInfo = { rd88f6560Tdm880 },
	.boardTdmInfoIndex = 0,

	.pBoardSpecInit = rd88f6560BoardSpecInit,

	/* NAND init params */
	.nandFlashReadParams = RD_88F6560_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = RD_88F6560_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = RD_88F6560_BOARD_NAND_CONTROL,
	.pBoardTdmSpiInfo = rd88f6560TdmSpiInfo
};

/* RD-88F6530-MDU */
#define RD_88F6530_BOARD_TWSI_DEF_NUM		0x0
#define RD_88F6530_BOARD_SWITCH_NUM			0x0
#define RD_88F6530_BOARD_MAC_INFO_NUM		0x2
#define RD_88F6530_BOARD_GPP_INFO_NUM		0x0
#define RD_88F6530_BOARD_MPP_CONFIG_NUM		0x1
#define RD_88F6530_BOARD_MPP_GROUP_TYPE_NUM	0x1
#define RD_88F6530_BOARD_DEVICE_CONFIG_NUM	0x1
#define RD_88F6530_BOARD_DEBUG_LED_NUM		0x0

#define RD_88F6530_BOARD_NAND_READ_PARAMS	0x000C0282
#define RD_88F6530_BOARD_NAND_WRITE_PARAMS	0x00010305
#define RD_88F6530_BOARD_NAND_CONTROL		0x01c00543

MV_BOARD_TWSI_INFO rd88f6530InfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
};

MV_BOARD_MAC_INFO rd88f6530InfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x8}
	,			/* This connected to SGMII to xcat ,right now forced SGMII 1G */
	{BOARD_MAC_SPEED_AUTO, 0x9}
};

MV_BOARD_MPP_TYPE_INFO rd88f6530InfoBoardMppTypeInfo[] = {
	{
	 .boardMppTdm = MV_BOARD_TDM_2CH,
	 .ethSataComplexOpt = ESC_OPT_SGMII | ESC_OPT_GEPHY_MAC1}
};

MV_BOARD_GPP_INFO rd88f6530InfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
};

MV_DEV_CS_INFO rd88f6530InfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MTD_NAND_LNC)
	{NAND_NOR_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#elif defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#else
	{N_A, N_A, N_A, N_A}			/* No device */
#endif
};

MV_BOARD_MPP_INFO rd88f6530InfoBoardMppConfigValue[] = {
	{{
	  RD_88F6530_MPP0_7,
	  RD_88F6530_MPP8_15,
	  RD_88F6530_MPP16_23,
	  RD_88F6530_MPP24_31,
	  RD_88F6530_MPP32_39,
	  RD_88F6530_MPP40_47,
	  RD_88F6530_MPP48_55,
	  RD_88F6530_MPP56_63,
	  RD_88F6530_MPP64_69,
	  RD_88F6530_MPP72_79,
	  RD_88F6530_MPP80_87,
	  RD_88F6530_MPP88_88
	  }
	 }
};

MV_BOARD_TDM_INFO rd88f6530Tdm880[] = { {1}
};

MV_BOARD_TDM_SPI_INFO rd88f6530TdmSpiInfo[] = { {0} };

MV_BOARD_INFO rd88f6530Info = {
	.boardName = "RD-88F6530-MDU",
	.numBoardMppTypeValue = RD_88F6530_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = rd88f6530InfoBoardMppTypeInfo,
	.numBoardMppConfigValue = RD_88F6530_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = rd88f6530InfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = RD_88F6530_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = rd88f6530InfoBoardDeCsInfo,
	.numBoardTwsiDev = RD_88F6530_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = rd88f6530InfoBoardTwsiDev,
	.numBoardMacInfo = RD_88F6530_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = rd88f6530InfoBoardMacInfo,
	.numBoardGppInfo = RD_88F6530_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = rd88f6530InfoBoardGppInfo,
	.activeLedsNumber = RD_88F6530_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = NULL,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = RD_88F6530_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = RD_88F6530_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = RD_88F6530_GPP_OUT_ENA_HIGH,
	.gppOutValLow = RD_88F6530_GPP_OUT_VAL_LOW,
	.gppOutValMid = RD_88F6530_GPP_OUT_VAL_MID,
	.gppOutValHigh = RD_88F6530_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = RD_88F6530_GPP_POL_LOW,
	.gppPolarityValMid = RD_88F6530_GPP_POL_MID,
	.gppPolarityValHigh = RD_88F6530_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = NULL,
	.switchInfoNum = 0,

	/* PON configuration. */
	.ponConfigValue = BOARD_GPON_CONFIG,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {1}
	,
	.pBoardTdmInt2CsInfo = {rd88f6530Tdm880,
				}
	,
	.boardTdmInfoIndex = 0,

	/* NAND init params */
	.nandFlashReadParams = RD_88F6530_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = RD_88F6530_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = RD_88F6530_BOARD_NAND_CONTROL,
	.pBoardTdmSpiInfo = rd88f6530TdmSpiInfo
};

/* Customer specific board place holder*/

#define DB_CUSTOMER_BOARD_PCI_IF_NUM		        0x0
#define DB_CUSTOMER_BOARD_TWSI_DEF_NUM		        0x0
#define DB_CUSTOMER_BOARD_MAC_INFO_NUM		        0x0
#define DB_CUSTOMER_BOARD_GPP_INFO_NUM		        0x0
#define DB_CUSTOMER_BOARD_MPP_GROUP_TYPE_NUN        0x0
#define DB_CUSTOMER_BOARD_MPP_CONFIG_NUM		    0x0
#define DB_CUSTOMER_BOARD_DEVICE_CONFIG_NUM	    	0x0
#define DB_CUSTOMER_BOARD_DEBUG_LED_NUM				0x0
#define DB_CUSTOMER_BOARD_NAND_READ_PARAMS		    0x000E02C2
#define DB_CUSTOMER_BOARD_NAND_WRITE_PARAMS		    0x00010305
#define DB_CUSTOMER_BOARD_NAND_CONTROL		        0x01c00543

MV_U8 dbCustomerInfoBoardDebugLedIf[] = { 0 };

MV_BOARD_MAC_INFO dbCustomerInfoBoardMacInfo[] =
    /* {{MV_BOARD_MAC_SPEED     boardMacSpeed,  MV_U8   boardEthSmiAddr}} */
{ {BOARD_MAC_SPEED_AUTO, 0x0}
};

MV_BOARD_TWSI_INFO dbCustomerInfoBoardTwsiDev[] =
    /* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
{ {BOARD_TWSI_OTHER, 0x0, ADDR7_BIT}
};

MV_BOARD_MPP_TYPE_INFO dbCustomerInfoBoardMppTypeInfo[] = { {MV_BOARD_OTHER, MV_BOARD_OTHER}
};

MV_DEV_CS_INFO dbCustomerInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MTD_NAND_LNC)
	{NAND_NOR_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#elif defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#else
	{N_A, N_A, N_A, N_A}			/* No device */
#endif
};

MV_BOARD_MPP_INFO dbCustomerInfoBoardMppConfigValue[] = { {{
							    DB_CUSTOMER_MPP0_7,
							    DB_CUSTOMER_MPP8_15,
							    DB_CUSTOMER_MPP16_23,
							    DB_CUSTOMER_MPP24_31,
							    DB_CUSTOMER_MPP32_39,
							    DB_CUSTOMER_MPP40_47,
							    DB_CUSTOMER_MPP48_55}
							   }
};

MV_BOARD_INFO dbCustomerInfo = { };

/* Arran 6560 */
#define ARRAN_BOARD_TWSI_DEF_NUM		0x0
#define ARRAN_BOARD_SWITCH_NUM		0x1
#define ARRAN_BOARD_MAC_INFO_NUM		0x2
#define ARRAN_BOARD_GPP_INFO_NUM		0x0
#define ARRAN_BOARD_MPP_CONFIG_NUM		0x1
#define ARRAN_BOARD_MPP_GROUP_TYPE_NUM	0x1

#if defined(MV_NAND) && defined(MV_SPI)
#define ARRAN_BOARD_DEVICE_CONFIG_NUM	0x2
#else
#define ARRAN_BOARD_DEVICE_CONFIG_NUM	0x1
#endif

#define ARRAN_BOARD_DEBUG_LED_NUM		0x0

#define ARRAN_BOARD_NAND_READ_PARAMS	0x000C0282
#define ARRAN_BOARD_NAND_WRITE_PARAMS	0x00010305
/* NAND care support for small page chips */
#define ARRAN_BOARD_NAND_CONTROL		0x01c00543

MV_BOARD_TWSI_INFO arranInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
};

MV_BOARD_MAC_INFO arranInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x1}
	,
	{BOARD_MAC_SPEED_1000M, 0x0}
};

MV_BOARD_MPP_TYPE_INFO arranInfoBoardMppTypeInfo[] = {
	{
	 .boardMppTdm = MV_BOARD_TDM_2CH,
//	 .ethSataComplexOpt = ESC_OPT_QSGMII | ESC_OPT_GEPHY_MAC1 | ESC_OPT_MAC0_2_SW_P4}
	 .ethSataComplexOpt = ESC_OPT_QSGMII | ESC_OPT_MAC1_2_SW_P5 | ESC_OPT_MAC0_2_SW_P4}
};

MV_BOARD_GPP_INFO arranInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
};

MV_DEV_CS_INFO arranInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MV_NAND)
	{BOOT_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#endif
#if defined(MV_NAND) && defined(MV_SPI)
	,
#endif
#if defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO arranInfoBoardMppConfigValue[] = {
	{{
	  ARRAN_MPP0_7,
	  ARRAN_MPP8_15,
	  ARRAN_MPP16_23,
	  ARRAN_MPP24_31,
	  ARRAN_MPP32_39,
	  ARRAN_MPP40_47,
	  ARRAN_MPP48_55,
	  ARRAN_MPP56_63,
	  ARRAN_MPP64_71,
	  ARRAN_MPP72_79,
	  ARRAN_MPP80_87,
	  ARRAN_MPP88_88
	  }
	 }
};

MV_BOARD_SWITCH_INFO arranInfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {1, 2, 3, 0, -1, -1, -1, -1}
	 ,
	 .cpuPort = 4,
	 .connectedPort = {4, 5, -1}
	 ,
	 .smiScanMode = 2,
	 .quadPhyAddr = 0
	 }
};

MV_BOARD_TDM_INFO arranTdm880[] = { {1}
};

MV_BOARD_INFO arranInfo = {
	.boardName = "Arran",
	.numBoardMppTypeValue = ARRAN_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = arranInfoBoardMppTypeInfo,
	.numBoardMppConfigValue = ARRAN_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = arranInfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = ARRAN_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = arranInfoBoardDeCsInfo,
	.numBoardTwsiDev = ARRAN_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = arranInfoBoardTwsiDev,
	.numBoardMacInfo = ARRAN_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = arranInfoBoardMacInfo,
	.numBoardGppInfo = ARRAN_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = arranInfoBoardGppInfo,
	.activeLedsNumber = ARRAN_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = NULL,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = ARRAN_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = ARRAN_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = ARRAN_GPP_OUT_ENA_HIGH,
	.gppOutValLow = ARRAN_GPP_OUT_VAL_LOW,
	.gppOutValMid = ARRAN_GPP_OUT_VAL_MID,
	.gppOutValHigh = ARRAN_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = ARRAN_GPP_POL_LOW,
	.gppPolarityValMid = ARRAN_GPP_POL_MID,
	.gppPolarityValHigh = ARRAN_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = arranInfoBoardSwitchValue,
	.switchInfoNum = ARRAN_BOARD_SWITCH_NUM,

	/* PON configuration. */
	.ponConfigValue = BOARD_PON_NONE,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {1}
	,
	.pBoardTdmInt2CsInfo = {arranTdm880,
				}
	,
	.boardTdmInfoIndex = 0,

	/* NAND init params */
	.nandFlashReadParams = ARRAN_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = ARRAN_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = ARRAN_BOARD_NAND_CONTROL
};

/* Bowmore */
#define BOWMORE_BOARD_TWSI_DEF_NUM		0x0
#define BOWMORE_BOARD_SWITCH_NUM		0x1
#define BOWMORE_BOARD_MAC_INFO_NUM		0x2
#define BOWMORE_BOARD_GPP_INFO_NUM		0x0
#define BOWMORE_BOARD_MPP_CONFIG_NUM		0x1
#define BOWMORE_BOARD_MPP_GROUP_TYPE_NUM	0x1

#if defined(MV_NAND) && defined(MV_SPI)
#define BOWMORE_BOARD_DEVICE_CONFIG_NUM	0x2
#else
#define BOWMORE_BOARD_DEVICE_CONFIG_NUM	0x1
#endif

#define BOWMORE_BOARD_DEBUG_LED_NUM		0x0

#define BOWMORE_BOARD_NAND_READ_PARAMS	0x000C0282
#define BOWMORE_BOARD_NAND_WRITE_PARAMS	0x00010305
/* NAND care support for small page chips */
#define BOWMORE_BOARD_NAND_CONTROL		0x01c00543

MV_BOARD_TWSI_INFO bowmoreInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
};

MV_BOARD_MAC_INFO bowmoreInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x1}
	,
	{BOARD_MAC_SPEED_1000M, 0x0}
};

MV_BOARD_MPP_TYPE_INFO bowmoreInfoBoardMppTypeInfo[] = {
	{
	 .boardMppTdm = MV_BOARD_TDM_2CH,
//	 .ethSataComplexOpt = ESC_OPT_QSGMII | ESC_OPT_GEPHY_MAC1 | ESC_OPT_MAC0_2_SW_P4}
	 .ethSataComplexOpt = ESC_OPT_QSGMII | ESC_OPT_MAC1_2_SW_P5 | ESC_OPT_MAC0_2_SW_P4}
};

MV_BOARD_GPP_INFO bowmoreInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
};

MV_DEV_CS_INFO bowmoreInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MV_NAND)
	{BOOT_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#endif
#if defined(MV_NAND) && defined(MV_SPI)
	,
#endif
#if defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO bowmoreInfoBoardMppConfigValue[] = {
	{{
	  BOWMORE_MPP0_7,
	  BOWMORE_MPP8_15,
	  BOWMORE_MPP16_23,
	  BOWMORE_MPP24_31,
	  BOWMORE_MPP32_39,
	  BOWMORE_MPP40_47,
	  BOWMORE_MPP48_55,
	  BOWMORE_MPP56_63,
	  BOWMORE_MPP64_71,
	  BOWMORE_MPP72_79,
	  BOWMORE_MPP80_87,
	  BOWMORE_MPP88_88
	  }
	 }
};

MV_BOARD_SWITCH_INFO bowmoreInfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {1, 2, 3, 0, -1, -1, -1, -1}
	 ,
	 .cpuPort = 4,
	 .connectedPort = {4, 5, -1}
	 ,
	 .smiScanMode = 2,
	 .quadPhyAddr = 0
	 }
};

MV_BOARD_TDM_INFO bowmoreTdm880[] = { {1}
};

MV_BOARD_INFO bowmoreInfo = {
	.boardName = "Bowmore",
	.numBoardMppTypeValue = BOWMORE_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = bowmoreInfoBoardMppTypeInfo,
	.numBoardMppConfigValue = BOWMORE_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = bowmoreInfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = BOWMORE_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = bowmoreInfoBoardDeCsInfo,
	.numBoardTwsiDev = BOWMORE_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = bowmoreInfoBoardTwsiDev,
	.numBoardMacInfo = BOWMORE_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = bowmoreInfoBoardMacInfo,
	.numBoardGppInfo = BOWMORE_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = bowmoreInfoBoardGppInfo,
	.activeLedsNumber = BOWMORE_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = NULL,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = BOWMORE_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = BOWMORE_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = BOWMORE_GPP_OUT_ENA_HIGH,
	.gppOutValLow = BOWMORE_GPP_OUT_VAL_LOW,
	.gppOutValMid = BOWMORE_GPP_OUT_VAL_MID,
	.gppOutValHigh = BOWMORE_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = BOWMORE_GPP_POL_LOW,
	.gppPolarityValMid = BOWMORE_GPP_POL_MID,
	.gppPolarityValHigh = BOWMORE_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = bowmoreInfoBoardSwitchValue,
	.switchInfoNum = BOWMORE_BOARD_SWITCH_NUM,

	/* PON configuration. */
	.ponConfigValue = BOARD_PON_NONE,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {1}
	,
	.pBoardTdmInt2CsInfo = {bowmoreTdm880,
				}
	,
	.boardTdmInfoIndex = 0,

	/* NAND init params */
	.nandFlashReadParams = BOWMORE_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = BOWMORE_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = BOWMORE_BOARD_NAND_CONTROL
};

/* Tamdhu */
#define TAMDHU_BOARD_TWSI_DEF_NUM		0x0
#define TAMDHU_BOARD_SWITCH_NUM		0x1
#define TAMDHU_BOARD_MAC_INFO_NUM		0x2
#define TAMDHU_BOARD_GPP_INFO_NUM		0x0
#define TAMDHU_BOARD_MPP_CONFIG_NUM		0x1
#define TAMDHU_BOARD_MPP_GROUP_TYPE_NUM	0x1

#if defined(MV_NAND) && defined(MV_SPI)
#define TAMDHU_BOARD_DEVICE_CONFIG_NUM	0x2
#else
#define TAMDHU_BOARD_DEVICE_CONFIG_NUM	0x1
#endif

#define TAMDHU_BOARD_DEBUG_LED_NUM		0x0

#define TAMDHU_BOARD_NAND_READ_PARAMS	0x000C0282
#define TAMDHU_BOARD_NAND_WRITE_PARAMS	0x00010305
/* NAND care support for small page chips */
#define TAMDHU_BOARD_NAND_CONTROL		0x01c00543

MV_BOARD_TWSI_INFO tamdhuInfoBoardTwsiDev[] = {
	/* {{MV_BOARD_DEV_CLASS devClass, MV_U8 twsiDevAddr, MV_U8 twsiDevAddrType}} */
};

MV_BOARD_MAC_INFO tamdhuInfoBoardMacInfo[] = {
	/* {{MV_BOARD_MAC_SPEED boardMacSpeed, MV_U8 boardEthSmiAddr}} */
	{BOARD_MAC_SPEED_1000M, 0x0}
	,
	{BOARD_MAC_SPEED_AUTO, 0x9}
	,
};

MV_BOARD_MPP_TYPE_INFO tamdhuInfoBoardMppTypeInfo[] = {
	{
	 .boardMppTdm = MV_BOARD_TDM_2CH,
	 .ethSataComplexOpt = ESC_OPT_GEPHY_MAC1 | ESC_OPT_RGMIIA_MAC0,
	}
};

MV_BOARD_GPP_INFO tamdhuInfoBoardGppInfo[] = {
	/* {{MV_BOARD_GPP_CLASS devClass, MV_U8 gppPinNum}} */
};

MV_DEV_CS_INFO tamdhuInfoBoardDeCsInfo[] = {
	/*{deviceCS, params, devType, devWidth} */
#if defined(MV_NAND)
	{BOOT_CS, N_A, BOARD_DEV_NAND_FLASH, 8}	/* NAND DEV */
#endif
#if defined(MV_NAND) && defined(MV_SPI)
	,
#endif
#if defined(MV_SPI)
	{SPI_CS0, N_A, BOARD_DEV_SPI_FLASH, 8}	/* SPI DEV */
#endif
};

MV_BOARD_MPP_INFO tamdhuInfoBoardMppConfigValue[] = {
	{{
	  TAMDHU_MPP0_7,
	  TAMDHU_MPP8_15,
	  TAMDHU_MPP16_23,
	  TAMDHU_MPP24_31,
	  TAMDHU_MPP32_39,
	  TAMDHU_MPP40_47,
	  TAMDHU_MPP48_55,
	  TAMDHU_MPP56_63,
	  TAMDHU_MPP64_71,
	  TAMDHU_MPP72_79,
	  TAMDHU_MPP80_87,
	  TAMDHU_MPP88_88
	  }
	 }
};

MV_BOARD_SWITCH_INFO tamdhuInfoBoardSwitchValue[] = {
	{
	 .switchIrq = 29,	/* set to -1 for timer operation */
	 .switchPort = {1, 2, 3, 0, -1, -1, -1, -1}
	 ,
	 .cpuPort = 4,
	 .connectedPort = {4, 5, -1}
	 ,
	 .smiScanMode = 2,
	 .quadPhyAddr = 0
	 }
};

MV_BOARD_TDM_INFO tamdhuTdm880[] = { {1}
};

MV_BOARD_INFO tamdhuInfo = {
	.boardName = "Tamdhu",
	.numBoardMppTypeValue = TAMDHU_BOARD_MPP_GROUP_TYPE_NUM,
	.pBoardMppTypeValue = tamdhuInfoBoardMppTypeInfo,
	.numBoardMppConfigValue = TAMDHU_BOARD_MPP_CONFIG_NUM,
	.pBoardMppConfigValue = tamdhuInfoBoardMppConfigValue,
	.intsGppMaskLow = 0,
	.intsGppMaskMid = 0,
	.intsGppMaskHigh = 0,
	.numBoardDeviceIf = TAMDHU_BOARD_DEVICE_CONFIG_NUM,
	.pDevCsInfo = tamdhuInfoBoardDeCsInfo,
	.numBoardTwsiDev = TAMDHU_BOARD_TWSI_DEF_NUM,
	.pBoardTwsiDev = tamdhuInfoBoardTwsiDev,
	.numBoardMacInfo = TAMDHU_BOARD_MAC_INFO_NUM,
	.pBoardMacInfo = tamdhuInfoBoardMacInfo,
	.numBoardGppInfo = TAMDHU_BOARD_GPP_INFO_NUM,
	.pBoardGppInfo = tamdhuInfoBoardGppInfo,
	.activeLedsNumber = TAMDHU_BOARD_DEBUG_LED_NUM,
	.pLedGppPin = NULL,
	.ledsPolarity = 0,

	/* GPP values */
	.gppOutEnValLow = TAMDHU_GPP_OUT_ENA_LOW,
	.gppOutEnValMid = TAMDHU_GPP_OUT_ENA_MID,
	.gppOutEnValHigh = TAMDHU_GPP_OUT_ENA_HIGH,
	.gppOutValLow = TAMDHU_GPP_OUT_VAL_LOW,
	.gppOutValMid = TAMDHU_GPP_OUT_VAL_MID,
	.gppOutValHigh = TAMDHU_GPP_OUT_VAL_HIGH,
	.gppPolarityValLow = TAMDHU_GPP_POL_LOW,
	.gppPolarityValMid = TAMDHU_GPP_POL_MID,
	.gppPolarityValHigh = TAMDHU_GPP_POL_HIGH,

	/* External Switch Configuration */
	.pSwitchInfo = tamdhuInfoBoardSwitchValue,
	.switchInfoNum = TAMDHU_BOARD_SWITCH_NUM,

	/* PON configuration. */
	.ponConfigValue = BOARD_PON_NONE,

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	.numBoardTdmInfo = {1}
	,
	.pBoardTdmInt2CsInfo = {tamdhuTdm880,
				}
	,
	.boardTdmInfoIndex = 0,

	/* NAND init params */
	.nandFlashReadParams = TAMDHU_BOARD_NAND_READ_PARAMS,
	.nandFlashWriteParams = TAMDHU_BOARD_NAND_WRITE_PARAMS,
	.nandFlashControl = TAMDHU_BOARD_NAND_CONTROL
};

MV_BOARD_INFO *boardInfoTbl[] = {
	&db88f6535Info,
	&rd88f6510Info,
	&rd88f6560Info,
	&rd88f6530Info,
	&dbCustomerInfo,
	&arranInfo,
	&bowmoreInfo,
	&tamdhuInfo,
};
