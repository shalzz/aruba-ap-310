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

#ifndef __INCmvCtrlEnvLibh
#define __INCmvCtrlEnvLibh

/* includes */
#include "mvSysHwConfig.h"
#include "mvCommon.h"
#include "mvTypes.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"
/* #include "boardEnv/mvBoardEnvLib.h" */

/* 0 for Auto scan mode, 1 for manual. */
#define MV_INTERNAL_SWITCH_SMI_SCAN_MODE	0

/* typedefs */
typedef MV_STATUS(*MV_WIN_GET_FUNC_PTR) (MV_U32, MV_U32, MV_UNIT_WIN_INFO*);

/* This enumerator describes the possible HW cache coherency policies the   */
/* controllers supports.                                                    */
typedef enum _mvCachePolicy {
	NO_COHERENCY,		/* No HW cache coherency support                        */
	WT_COHERENCY,		/* HW cache coherency supported in Write Through policy */
	WB_COHERENCY		/* HW cache coherency supported in Write Back policy    */
} MV_CACHE_POLICY;

/* The swapping is referred to a 64-bit words (as this is the controller    */
/* internal data path width). This enumerator describes the possible        */
/* data swap types. Below is an example of the data 0x0011223344556677      */
typedef enum _mvSwapType {
	MV_BYTE_SWAP,		/* Byte Swap                77 66 55 44 33 22 11 00 */
	MV_NO_SWAP,		/* No swapping              00 11 22 33 44 55 66 77 */
	MV_BYTE_WORD_SWAP,	/* Both byte and word swap  33 22 11 00 77 66 55 44 */
	MV_WORD_SWAP,		/* Word swap                44 55 66 77 00 11 22 33 */
	SWAP_TYPE_MAX		/* Delimiter for this enumerator                                        */
} MV_SWAP_TYPE;

/*
** Define the different Ethernet complex sources for the RGMIIA/B and
** the FE/GE phy interfaces.
*/
typedef enum {
	EC_MAC0_SRC = 1,
	EC_MAC1_SRC,
	EC_SW_P0_SRC,
	EC_SW_P1_SRC,
	EC_SW_P4_SRC,
	EC_SW_P5_SRC,
	EC_SW_P6_SRC,
	EC_SRC_NONE
} MV_ETH_COMPLEX_MUX_SOURCES;

typedef struct {
	MV_ETH_COMPLEX_MUX_SOURCES feGeSrc;
	MV_ETH_COMPLEX_MUX_SOURCES rgmiiASrc;
	MV_ETH_COMPLEX_MUX_SOURCES swSrc;
} MV_ETH_COMPLEX_IF_SOURCES;

/* Define the different Ethernet & Sata complex connection options */
typedef enum {
	ESC_OPT_RGMIIA_MAC0 = 0x00001,	/* RGMIIA originated from MAC0 */
	ESC_OPT_RGMIIA_MAC1 = 0x00002,	/* RGMIIA originated from MAC1 */
	ESC_OPT_RGMIIA_SW_P5 = 0x00004,	/* RGMIIA originated from switch port 5. */
	ESC_OPT_RGMIIA_SW_P6 = 0x00008,	/* RGMIIA originated from switch port 6. */
	ESC_OPT_RGMIIB_MAC0 = 0x00010,	/* RGMIIB originated from MAC0 */
	ESC_OPT_MAC0_2_SW_P4 = 0x00020,	/* MAC0 connected to switch port 4. */
	ESC_OPT_MAC1_2_SW_P5 = 0x00040,	/* MAC1 connected to switch port 5. */
	ESC_OPT_GEPHY_MAC1 = 0x00080,	/* GE/FE Phy originated from MAC1 */
	ESC_OPT_GEPHY_SW_P0 = 0x00100,	/* GE/FE Phy originated from switch port 0 */
	ESC_OPT_GEPHY_SW_P5 = 0x00200,	/* GE/FE Phy originated from switch port 5 */
	ESC_OPT_FE3PHY = 0x00400,	/* 3xFE switch phys */
	ESC_OPT_PCS = 0x00800,		/* PCS from MAC0 */
	ESC_OPT_SGMII = 0x01000,	/* SGMII from MAC0 */
	ESC_OPT_QSGMII = 0x02000,	/* QSGMII option */
	ESC_OPT_SATA = 0x04000,		/* Sata option */
	ESC_OPT_AUTO = 0x08000,
	ESC_OPT_ILLEGAL = 0x10000,
	ESC_OPT_ALL = 0x1FFFF
} MV_ETH_SATA_COMPLEX_OPTIONS;

/* mcspLib.h API list */
MV_STATUS mvCtrlEnvEthSrcCfgSet(MV_ETH_COMPLEX_IF_SOURCES *ethSrc);

MV_STATUS mvCtrlEnvInit(MV_VOID);
MV_U32 mvCtrlMppRegGet(MV_U32 mppGroup);

#if defined(MV_INCLUDE_PEX)
MV_U32 mvCtrlPexMaxIfGet(MV_VOID);
#else
#define   mvCtrlPexMaxIfGet()	(0)
#endif

#define   mvCtrlPciIfMaxIfGet()	(mvCtrlPexMaxIfGet())

MV_U32 mvCtrlEthMaxPortGet(MV_VOID);
#if defined(MV_INCLUDE_XOR)
MV_U32 mvCtrlXorMaxChanGet(MV_VOID);
MV_U32 mvCtrlXorMaxUnitGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_USB)
MV_U32 mvCtrlUsbMaxGet(MV_VOID);
#endif
#if defined(MV_INCLUDE_LEGACY_NAND)
MV_U32 mvCtrlNandSupport(MV_VOID);
#endif
#if defined(MV_INCLUDE_SDIO)
MV_U32 mvCtrlSdioSupport(MV_VOID);
#endif
#if defined(MV_INCLUDE_TDM)
MV_U32 mvCtrlTdmSupport(MV_VOID);
MV_U32 mvCtrlTdmMaxGet(MV_VOID);
MV_UNIT_ID mvCtrlTdmUnitTypeGet(MV_VOID);
MV_U32 mvCtrlTdmUnitIrqGet(MV_VOID);
#endif

MV_U16 mvCtrlModelGet(MV_VOID);
MV_U8 mvCtrlRevGet(MV_VOID);
MV_STATUS mvCtrlNameGet(char *pNameBuff);
MV_U32 mvCtrlModelRevGet(MV_VOID);
MV_STATUS mvCtrlModelRevNameGet(char *pNameBuff);
MV_VOID mvCtrlAddrDecShow(MV_VOID);
const MV_8 *mvCtrlTargetNameGet(MV_TARGET target);
MV_U32 ctrlSizeToReg(MV_U32 size, MV_U32 alignment);
MV_U32 ctrlRegToSize(MV_U32 regSize, MV_U32 alignment);
MV_U32 ctrlSizeRegRoundUp(MV_U32 size, MV_U32 alignment);
MV_U32 mvCtrlSysRstLengthCounterGet(MV_VOID);
MV_STATUS ctrlWinOverlapTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);
MV_STATUS ctrlWinWithinWinTest(MV_ADDR_WIN *pAddrWin1, MV_ADDR_WIN *pAddrWin2);

MV_VOID mvCtrlPwrClckSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable);
MV_BOOL mvCtrlPwrClckGet(MV_UNIT_ID unitId, MV_U32 index);
MV_VOID mvCtrlPwrMemSet(MV_UNIT_ID unitId, MV_U32 index, MV_BOOL enable);
MV_BOOL mvCtrlIsBootFromSPI(MV_VOID);
MV_BOOL mvCtrlIsBootFromSPIUseNAND(MV_VOID);
MV_BOOL mvCtrlIsBootFromNAND(MV_VOID);
MV_BOOL mvCtrlIsBootFromNOR(MV_VOID);
#if defined(MV_INCLUDE_CLK_PWR_CNTRL)
MV_VOID mvCtrlPwrSaveOn(MV_VOID);
MV_VOID mvCtrlPwrSaveOff(MV_VOID);
#endif
MV_BOOL mvCtrlPwrMemGet(MV_UNIT_ID unitId, MV_U32 index);
MV_VOID mvMPPConfigToSPI(MV_VOID);
MV_VOID mvMPPConfigToDefault(MV_VOID);
MV_STATUS mvCtrlEthSataComplexBuildConfig(MV_U32 brdModules, MV_ETH_COMPLEX_IF_SOURCES *ifSrc, MV_U32 *ethConfig);
MV_U32 mvCtrlSataMaxPortGet(MV_VOID);

MV_BOOL mvCtrlIsGponMode(void);

#endif /* __INCmvCtrlEnvLibh */
