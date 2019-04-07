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

    *	Redistributions of source code must retain the above copyright notice,
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
#ifndef __INCmvBoardEnvLibh
#define __INCmvBoardEnvLibh

/* defines */
/* The below constant macros defines the board I2C EEPROM data offsets */

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "mvSysHwConfig.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include "twsi/mvTwsi.h"

/* DUART stuff for Tclk detection only */
#define DUART_BAUD_RATE			115200
#define MAX_CLOCK_MARGINE		5000000	/* Maximum detected clock margine */

/* Voice devices assembly modes */
#define DAISY_CHAIN_MODE	1
#define DUAL_CHIP_SELECT_MODE   0
#define INTERRUPT_TO_MPP        1
#define INTERRUPT_TO_TDM	0

#define BOARD_ETH_PORT_NUM  MV_ETH_MAX_PORTS
#define BOARD_ETH_SWITCH_PORT_NUM	8
#define BOARD_ETH_SWITCH_CPUPORT_NUM	2

#define	MV_BOARD_MAX_USB_IF		1
#define MV_BOARD_MAX_MPP		12
#define MV_BOARD_NAME_LEN  		0x20

/*
#define MV_BOARD_CFG_IS_GPON_EN(cfg)	(cfg & 0x1)
#define MV_BOARD_CFG_IS_EPON_EN(cfg)	(!MV_BOARD_CFG_IS_EPON(cfg))
#define MV_BOARD_CFG_IS_QSGMII_EN(cfg)	((cfg & 0x8) && (cfg & 0x10))
#define MV_BOARD_CFG_IS_SGMII_EN(cfg)	((cfg & 0x8) && (!(cfg & 0x10)))
#define MV_BOARD_CFG_IS_SATA_EN(cfg)	(!(cfg & 0x8))
#define MV_BOARD_CFG_IS_3FE_1GE_EN(cfg)	(cfg & 0x20)
#define MV_BOARD_CFG_IS_4FE_EN(cfg)	(!MV_BOARD_CFG_IS_3FE_1GE_EN(cfg))
*/
/* EPPROM Modules detection information */
#define MV_BOARD_CFG_IS_SW_FORCE_P1_SRC(cfg)	(cfg & 0x1)
/* SW_SRC
JP5
 0       Auto Detect
 1       Force Port 1
*/
#define MV_BOARD_CFG_IS_3FE_EN(cfg)		(cfg & 0x2)
/* SW_SRC
JP6
 0       3xFE disabled
 1       3xFE enabled
*/
#define MV_BOARD_CFG_SERDES_SRC(cfg)	((cfg & 0xC)>>2)
/* SERDES_SRC
JP8 JP10
 0   0    None
 1   0    SATA
 0   1    SGMII
 1   1    N/A
*/
#define MV_BOARD_CFG_RGMIIA_SRC(cfg)	((cfg & 0x30)>>4)
/* RGMIIA_SRC
JP11 JP12
 0    0   None
 1    0   MAC0 source
 0    1   MAC1 source
 1    1   Switch port 5 source
*/
#define MV_BOARD_CFG_FEGEPHY_SRC(cfg)	((cfg & 0xC0)>>6)
/* GEPHY_SRC
JP17 JP18
 0    0   None
 1    0   MAC1 source
 0    1   Switch port 4 source
 1    1   Switch port 5 source
*/

#define MV_BOARD_EEPROM_MODULE_ADDR		0x24
#define MV_BOARD_EEPROM_MODULE_ADDR_TYPE	ADDR7_BIT

#define MV_BOARD_RGMIIA_MODULE_ADDR		0x22
#define MV_BOARD_RGMIIA_MODULE_ADDR_TYPE	ADDR7_BIT
#define MV_BOARD_RGMIIA_MODULE_MUX_CHNUM	1

#define MV_BOARD_RGMIIB_MODULE_ADDR		0x20
#define MV_BOARD_RGMIIB_MODULE_ADDR_TYPE	ADDR7_BIT
#define MV_BOARD_RGMIIB_MODULE_MUX_CHNUM	1

#define MV_BOARD_QSGMII_MODULE_ADDR		0x23
#define MV_BOARD_QSGMII_MODULE_ADDR_TYPE	ADDR7_BIT
#define MV_BOARD_QSGMII_MODULE_MUX_CHNUM	1

#define MV_BOARD_GE_SFP_MODULE_ADDR		0x50
#define MV_BOARD_GE_SFP_MODULE_ADDR_TYPE	ADDR7_BIT
#define MV_BOARD_GE_SFP_MODULE_MUX_CHNUM	1

#define MV_BOARD_TDM_792_ADAPT_MODULE_ADDR	0x27
#define MV_BOARD_TDM_880_MODULE_ADDR		0x26
#define MV_BOARD_TDM_3215_MODULE_ADDR		0x23
#define MV_BOARD_TDM_MODULE_ADDR_TYPE		ADDR7_BIT
#define MV_BOARD_TDM_MODULE_MUX_CHNUM		0

typedef struct _boardData {
	MV_U32 magic;
	MV_U16 boardId;
	MV_U8 boardVer;
	MV_U8 boardRev;
	MV_U32 reserved1;
	MV_U32 reserved2;
} BOARD_DATA;

typedef enum _devBoardMppGroupClass {
	MV_BOARD_MPP_GROUP_1,
	MV_BOARD_MPP_GROUP_2,
	MV_BOARD_MPP_GROUP_3,
	MV_BOARD_MAX_MPP_GROUP
} MV_BOARD_MPP_GROUP_CLASS;

typedef enum _devBoardMppTypeClass {
	MV_BOARD_AUTO,
	MV_BOARD_TDM_2CH,
	MV_BOARD_TDM_32CH,
	MV_BOARD_GE0,
	MV_BOARD_GE1,
	MV_BOARD_SW_P5,
	MV_BOARD_SW_P6,
	MV_BOARD_OTHER
} MV_BOARD_MPP_TYPE_CLASS;

#ifdef MV_TDM_2CHANNELS
#define MV_BOARD_TDM 	MV_BOARD_TDM_2CH
#else
#define MV_BOARD_TDM	MV_BOARD_TDM_32CH
#endif

typedef enum {
	MV_BOARD_MODULE_TS_ID = 0x00001,
	MV_BOARD_MODULE_QSGMII_ID = 0x00002,
	MV_BOARD_MODULE_SGMII_ID = 0x00004,
	MV_BOARD_MODULE_GE_SFP_ID = 0x00008,
	MV_BOARD_MODULE_RGMIIA_ID = 0x00010,
	MV_BOARD_MODULE_RGMIIB_ID = 0x00020,
	MV_BOARD_MODULE_MIIA_ID = 0x00040,
	MV_BOARD_MODULE_MIIB_ID = 0x00080,
	MV_BOARD_MODULE_TDM_1_ID = 0x00100,
	MV_BOARD_MODULE_TDM_2_ID = 0x00200,
	MV_BOARD_MODULE_TDM_3_ID = 0x00400,
	MV_BOARD_MODULE_TDM_4_ID = 0x00800,
	MV_BOARD_MODULE_GPON_ID = 0x01000,
	MV_BOARD_MODULE_EPON_ID = 0x02000,
	MV_BOARD_MODULE_4FE_PHY_ID = 0x04000,
	MV_BOARD_MODULE_FE_GE_PHY_ID = 0x08000,
	MV_BOARD_MODULE_SATA_ID = 0x10000,
	MV_BOARD_MODULE_SDIO_ID = 0x20000,
	MV_BOARD_MODULE_OTHER_ID = 0x80000
} MV_BOARD_MODULE_ID_CLASS;

typedef struct _boardMppTypeInfo {
	MV_BOARD_MPP_TYPE_CLASS boardMppTdm;

	/* Ethernet / Sata complex                      */
	/* A bitmask of MV_ETH_SATA_COMPLEX_OPTIONS     */
	MV_U32 ethSataComplexOpt;
} MV_BOARD_MPP_TYPE_INFO;

typedef enum _devBoardClass {
	BOARD_DEV_NOR_FLASH,
	BOARD_DEV_NAND_FLASH,
	BOARD_DEV_SEVEN_SEG,
	BOARD_DEV_FPGA,
	BOARD_DEV_SRAM,
	BOARD_DEV_SPI_FLASH,
	BOARD_DEV_OTHER,
} MV_BOARD_DEV_CLASS;

typedef enum _devTwsiBoardClass {
	BOARD_TWSI_RTC,
	BOARD_DEV_TWSI_EXP,
	BOARD_DEV_TWSI_SATR,
	BOARD_TWSI_MUX,
	BOARD_TWSI_AUDIO_DEC,
	BOARD_TWSI_OTHER
} MV_BOARD_TWSI_CLASS;

typedef enum _devGppBoardClass {
	BOARD_GPP_RTC,
	BOARD_GPP_MV_SWITCH,
	BOARD_GPP_USB_VBUS,
	BOARD_GPP_USB_VBUS_EN,
	BOARD_GPP_USB_OC,
	BOARD_GPP_USB_HOST_DEVICE,
	BOARD_GPP_REF_CLCK,
	BOARD_GPP_VOIP_SLIC,
	BOARD_GPP_LIFELINE,
	BOARD_GPP_BUTTON,
	BOARD_GPP_TS_BUTTON_C,
	BOARD_GPP_TS_BUTTON_U,
	BOARD_GPP_TS_BUTTON_D,
	BOARD_GPP_TS_BUTTON_L,
	BOARD_GPP_TS_BUTTON_R,
	BOARD_GPP_POWER_BUTTON,
	BOARD_GPP_RESTOR_BUTTON,
	BOARD_GPP_WPS_BUTTON,
	BOARD_GPP_HDD0_POWER,
	BOARD_GPP_HDD1_POWER,
	BOARD_GPP_FAN_POWER,
	BOARD_GPP_RESET,
	BOARD_GPP_POWER_ON_LED,
	BOARD_GPP_HDD_POWER,
	BOARD_GPP_SDIO_POWER,
	BOARD_GPP_SDIO_DETECT,
	BOARD_GPP_SDIO_WP,
	BOARD_GPP_SWITCH_PHY_INT,
	BOARD_GPP_TSU_DIRCTION,
	BOARD_GPP_PEX_RESET,
	BOARD_GPP_CONF,
	BOARD_GPP_OTHER
} MV_BOARD_GPP_CLASS;

typedef struct _devCsInfo {
	MV_U8 deviceCS;
	MV_U32 params;
	MV_U32 devClass;	/* MV_BOARD_DEV_CLASS */
	MV_U8 devWidth;
} MV_DEV_CS_INFO;

typedef struct _boardSwitchInfo {
	MV_32 switchIrq;
	MV_32 switchPort[BOARD_ETH_SWITCH_PORT_NUM];
	MV_32 cpuPort;
	MV_32 connectedPort[MV_ETH_MAX_PORTS];
	MV_32 smiScanMode;
	MV_32 quadPhyAddr;
} MV_BOARD_SWITCH_INFO;

typedef struct _boardLedInfo {
	MV_U8 activeLedsNumber;
	MV_U8 ledsPolarity;	/* '0' or '1' to turn on led */
	MV_U8 *gppPinNum;	/* Pointer to GPP values */
} MV_BOARD_LED_INFO;

typedef struct _boardGppInfo {
	MV_BOARD_GPP_CLASS devClass;
	MV_U8 gppPinNum;
} MV_BOARD_GPP_INFO;

typedef struct _boardTwsiInfo {
	MV_BOARD_TWSI_CLASS devClass;
	MV_U8 twsiDevAddr;
	MV_U8 twsiDevAddrType;
} MV_BOARD_TWSI_INFO;

typedef enum _boardMacSpeed {
	BOARD_MAC_SPEED_10M,
	BOARD_MAC_SPEED_100M,
	BOARD_MAC_SPEED_1000M,
	BOARD_MAC_SPEED_AUTO,
} MV_BOARD_MAC_SPEED;

typedef struct _boardMacInfo {
	MV_BOARD_MAC_SPEED boardMacSpeed;
	MV_U8 boardEthSmiAddr;
} MV_BOARD_MAC_INFO;

typedef struct _boardMppInfo {
	MV_U32 mppGroup[MV_BOARD_MAX_MPP];
} MV_BOARD_MPP_INFO;

typedef enum {
	BOARD_EPON_CONFIG,
	BOARD_GPON_CONFIG,
	BOARD_PON_NONE,
	BOARD_PON_AUTO
} MV_BOARD_PON_CONFIG;

typedef struct {
	MV_U8 spiCs;
} MV_BOARD_TDM_INFO;

typedef struct {
	MV_U8 spiId;
} MV_BOARD_TDM_SPI_INFO;

typedef enum {
	BOARD_TDM_SLIC_880 = 0,
	BOARD_TDM_SLIC_792,
	BOARD_TDM_SLIC_3215,
	BOARD_TDM_SLIC_OTHER,
	BOARD_TDM_SLIC_COUNT
} MV_BOARD_TDM_SLIC_TYPE;

typedef struct {
	MV_U32	reg;
	MV_U32	mask;
	MV_U32	val;
} MV_BOARD_SPEC_INIT;

typedef struct _boardInfo {
	char boardName[MV_BOARD_NAME_LEN];
	MV_U8 numBoardMppTypeValue;
	MV_BOARD_MPP_TYPE_INFO *pBoardMppTypeValue;
	MV_U8 numBoardMppConfigValue;
	MV_BOARD_MPP_INFO *pBoardMppConfigValue;
	MV_U32 intsGppMaskLow;
	MV_U32 intsGppMaskMid;
	MV_U32 intsGppMaskHigh;
	MV_U8 numBoardDeviceIf;
	MV_DEV_CS_INFO *pDevCsInfo;
	MV_U8 numBoardTwsiDev;
	MV_BOARD_TWSI_INFO *pBoardTwsiDev;
	MV_U8 numBoardMacInfo;
	MV_BOARD_MAC_INFO *pBoardMacInfo;
	MV_U8 numBoardGppInfo;
	MV_BOARD_GPP_INFO *pBoardGppInfo;
	MV_U8 activeLedsNumber;
	MV_U8 *pLedGppPin;
	MV_U8 ledsPolarity;	/* '0' or '1' to turn on led */

	/* GPP values */
	MV_U32 gppOutEnValLow;
	MV_U32 gppOutEnValMid;
	MV_U32 gppOutEnValHigh;
	MV_U32 gppOutValLow;
	MV_U32 gppOutValMid;
	MV_U32 gppOutValHigh;
	MV_U32 gppPolarityValLow;
	MV_U32 gppPolarityValMid;
	MV_U32 gppPolarityValHigh;

	/* External Switch Configuration */
	MV_BOARD_SWITCH_INFO *pSwitchInfo;
	MV_U32 switchInfoNum;

	/* PON configuration. */
	MV_BOARD_PON_CONFIG ponConfigValue;

	/* TDM configuration */
	/* We hold a different configuration array for each possible slic that
	 ** can be connected to board.
	 ** When modules are scanned, then we select the index of the relevant
	 ** slic's information array.
	 ** For RD and Customers boards we only need to initialize a single
	 ** entry of the arrays below, and set the boardTdmInfoIndex to 0.
	 */
	MV_U8 numBoardTdmInfo[BOARD_TDM_SLIC_COUNT];
	MV_BOARD_TDM_INFO *pBoardTdmInt2CsInfo[BOARD_TDM_SLIC_COUNT];
	MV_8 boardTdmInfoIndex;

	/* Board specific initialization. Performed before initializing the
	** SoC. */
	MV_BOARD_SPEC_INIT	*pBoardSpecInit;

	/* NAND init params */
	MV_U32 nandFlashReadParams;
	MV_U32 nandFlashWriteParams;
	MV_U32 nandFlashControl;
	MV_BOARD_TDM_SPI_INFO *pBoardTdmSpiInfo;
} MV_BOARD_INFO;

/* For backward compatability with Legacy mode */
#define mvBoardIsSwitchConnected(port)  (mvBoardSwitchConnectedPortGet(port) != -1)
#define mvBoardLinkStatusIrqGet(port)   mvBoardSwitchIrqGet()

MV_VOID mvBoardEnvInit(MV_VOID);
MV_U16 mvBoardModelGet(MV_VOID);
MV_U16 mvBoardRevGet(MV_VOID);
MV_STATUS mvBoardNameGet(char *pNameBuff);
MV_BOARD_SPEC_INIT *mvBoardSpecInitGet(MV_VOID);
MV_BOOL mvBoardIsPortInSgmii(MV_U32 ethPortNum);
MV_BOOL mvBoardIsPortInRgmii(MV_U32 ethPortNum);
MV_32 mvBoardPhyAddrGet(MV_U32 ethPortNum);

MV_32 mvBoardSwitchIrqGet(MV_VOID);
MV_32 mvBoardSwitchConnectedPortGet(MV_U32 ethPortNum);
/* Map front panel connector number to switch port number */
MV_32 mvBoardSwitchPortGet(MV_U32 switchIdx, MV_U32 boardPortNum);
/* Map front panel connector number to switch port number */
MV_32 mvBoardSwitchPortMap(MV_U32 switchIdx, MV_U32 switchPortNum);
MV_32 mvBoardSwitchCpuPortGet(MV_U32 switchIdx);
MV_32 mvBoardSmiScanModeGet(MV_U32 switchIdx);
MV_32 mvBoardQuadPhyAddrGet(MV_U32 switchIdx);
MV_32 mvBoardSwitchNumPortsGet(MV_VOID);
MV_BOOL mvBoardIsQsgmiiModuleConnected(MV_VOID);
MV_32 mvBoardGePhySwitchPortGet(MV_VOID);
MV_32 mvBoardRgmiiASwitchPortGet(MV_VOID);
MV_BOOL mvBoardPhySpecInitGet(MV_U32 *regOff, MV_U32 *data);
MV_U32 mvBoardTclkGet(MV_VOID);
MV_U32 mvBoardSysClkGet(MV_VOID);
MV_U32 mvBoardDebugLedNumGet(MV_U32 boardId);
MV_VOID mvBoardDebugLed(MV_U32 hexNum);
MV_32 mvBoarGpioPinNumGet(MV_BOARD_GPP_CLASS class, MV_U32 index);
MV_VOID mvBoardReset(MV_VOID);
MV_32 mvBoardResetGpioPinGet(MV_VOID);
MV_32 mvBoardSDIOGpioPinGet(MV_BOARD_GPP_CLASS type);
MV_32 mvBoardUSBVbusGpioPinGet(MV_32 devId);
MV_32 mvBoardUSBVbusEnGpioPinGet(MV_32 devId);
MV_U32 mvBoardGpioIntMaskGet(MV_U32 gppGrp);
MV_32 mvBoardMppGet(MV_U32 mppGroupNum);
MV_U32 mvBoardEthComplexConfigGet(MV_VOID);
MV_VOID mvBoardEthComplexConfigSet(MV_U32 ethConfig);
MV_U32 mvBoardPonConfigGet(MV_VOID);
MV_VOID mvBoardPonConfigSet(MV_U32 ponConfig);
MV_U32 mvBoardGppConfigGet(MV_VOID);
MV_VOID mvBoardModuleConfigSet(MV_U32 modConfig);
MV_VOID mvBoardModuleConfigGet(MV_U32 *modConfig);
MV_VOID mvBoardMppIdUpdate(MV_VOID);
MV_32 mvBoardTdmSpiModeGet(MV_VOID);
MV_U8 mvBoardTdmDevicesCountGet(MV_VOID);
MV_U8 mvBoardTdmSpiCsGet(MV_U8 devId);
MV_U8 mvBoardTdmSpiIdGet(MV_VOID);
MV_VOID mvBoardMppModuleTypePrint(MV_VOID);
MV_BOOL mvBoardIsGbEPortConnected(MV_U32 ethPortNum);
MV_32 mvBoardGetDevicesNumber(MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceBaseAddr(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceBusWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceWidth(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_32 mvBoardGetDeviceWinSize(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_U32 boardGetDevCSNum(MV_32 devNum, MV_BOARD_DEV_CLASS devClass);
MV_U8 mvBoardTwsiAddrTypeGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index);
MV_U8 mvBoardTwsiAddrGet(MV_BOARD_TWSI_CLASS twsiClass, MV_U32 index);
MV_32 mvBoardNandWidthGet(MV_VOID);
MV_U32 mvBoardIdGet(MV_VOID);

MV_U8 mvBoardTwsiSatRGet(MV_U8 devNum, MV_U8 regNum);
MV_STATUS mvBoardTwsiSatRSet(MV_U8 devNum, MV_U8 regNum, MV_U8 regVal);
MV_U8 mvBoardFreqGet(MV_VOID);
MV_STATUS mvBoardFreqSet(MV_U8 freqVal);
MV_U8 mvBoardSSCGGet(MV_VOID);
MV_STATUS mvBoardSSCGSet(MV_U8 val);
MV_U8 mvBoardTClkGet(MV_VOID);
MV_STATUS mvBoardTClkSet(MV_U8 val);
MV_U8 mvBoardPexClkGet(MV_U8 pexIf);
MV_STATUS mvBoardPexClkSet(MV_U8 pexIf, MV_U8 val);
MV_U8 mvBoardSaRL2ExistGet(MV_VOID);
MV_U8 mvBoardSaRL2ExistSet(MV_U8 val);
MV_U8 mvBoardPonGet(MV_VOID);
MV_STATUS mvBoardPonSet(MV_U8 val);

MV_STATUS mvBoardEthSataModulesScan(MV_U32 *modules, MV_ETH_COMPLEX_IF_SOURCES *ethSrcCfg);
MV_STATUS mvBoardIsSdioEnabled(MV_VOID);
MV_STATUS mvBoardTdmModulesScan(MV_VOID);
MV_STATUS mvBoardPonModulesScan(MV_U32 *modules);
MV_STATUS mvBoardTwsiMuxChannelSet(MV_U8 muxChNum);
MV_STATUS mvBoardTwsiReadByteThruMux(MV_U8 muxChNum, MV_U8 chNum, MV_TWSI_SLAVE *pTwsiSlave, MV_U8 *data);
MV_BOARD_MAC_SPEED mvBoardMacSpeedGet(MV_U32 ethPortNum);
MV_32 mvBoardIsInternalSwitchConnected(MV_U32 ethPortNum);
MV_BOARD_MPP_TYPE_CLASS mvBoardMppGroupTypeGet(MV_BOARD_MPP_GROUP_CLASS mppGroupClass);
#endif /* __INCmvBoardEnvLibh */
