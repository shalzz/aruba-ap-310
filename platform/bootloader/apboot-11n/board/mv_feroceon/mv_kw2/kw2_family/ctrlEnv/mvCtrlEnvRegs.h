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

#ifndef __INCmvCtrlEnvRegsh
#define __INCmvCtrlEnvRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CV Support */
#define PEX0_MEM0 	PEX0_MEM
#define PCI0_MEM0	PEX0_MEM
#define PEX1_MEM0 	PEX1_MEM
#define PCI1_MEM0	PEX1_MEM

/* Controller revision info */
#define PCI_CLASS_CODE_AND_REVISION_ID			    0x008
#define PCCRIR_REVID_OFFS				    0		/* Revision ID */
#define PCCRIR_REVID_MASK				    (0xff << PCCRIR_REVID_OFFS)

/* Controler environment registers offsets */
#define MV_TDM_2CH_IRQ_NUM	20
#define MV_TDM_IRQ_NUM		30

/* Power Management Memory Power Down Register	*/
#define POWER_MNG_MEM_CTRL_REG			0x18218

#define PMC_COMMSTOPMEM_OFFS			0
#define PMC_COMMSTOPMEM_MASK			(1 << PMC_COMMSTOPMEM_OFFS)
#define PMC_COMMSTOPMEM_EN			(0 << PMC_COMMSTOPMEM_OFFS)
#define PMC_COMMSTOPMEM_STOP			(1 << PMC_COMMSTOPMEM_OFFS)

#define PMC_PEXSTOPMEM_OFFS(id)			(1 + (id * 8))
#define PMC_PEXSTOPMEM_MASK(id)			(1 << PMC_PEXSTOPMEM_OFFS(id))
#define PMC_PEXSTOPMEM_EN(id)			(0 << PMC_PEXSTOPMEM_OFFS(id))
#define PMC_PEXSTOPMEM_STOP(id)			(1 << PMC_PEXSTOPMEM_OFFS(id))

#define PMC_USBSTOPMEM_OFFS			2
#define PMC_USBSTOPMEM_MASK			(1 << PMC_USBSTOPMEM_OFFS)
#define PMC_USBSTOPMEM_EN			(0 << PMC_USBSTOPMEM_OFFS)
#define PMC_USBSTOPMEM_STOP			(1 << PMC_USBSTOPMEM_OFFS)

#define PMC_DUNITSTOPMEM_OFFS			3
#define PMC_DUNITSTOPMEM_MASK			(1 << PMC_DUNITSTOPMEM_OFFS)
#define PMC_DUNITSTOPMEM_EN			(0 << PMC_DUNITSTOPMEM_OFFS)
#define PMC_DUNITSTOPMEM_STOP			(1 << PMC_DUNITSTOPMEM_OFFS)

#define PMC_RUNITSTOPMEM_OFFS			4
#define PMC_RUNITSTOPMEM_MASK			(1 << PMC_RUNITSTOPMEM_OFFS)
#define PMC_RUNITSTOPMEM_EN			(0 << PMC_RUNITSTOPMEM_OFFS)
#define PMC_RUNITSTOPMEM_STOP			(1 << PMC_RUNITSTOPMEM_OFFS)

#define PMC_XORSTOPMEM_OFFS(port)		(5+(port*2))
#define PMC_XORSTOPMEM_MASK(port)		(1 << PMC_XORSTOPMEM_OFFS(port))
#define PMC_XORSTOPMEM_EN(port)			(0 << PMC_XORSTOPMEM_OFFS(port))
#define PMC_XORSTOPMEM_STOP(port)		(1 << PMC_XORSTOPMEM_OFFS(port))

#define PMC_SATASTOPMEM_OFFS			6
#define PMC_SATASTOPMEM_MASK			(1 << PMC_SATASTOPMEM_OFFS)
#define PMC_SATASTOPMEM_EN			(0 << PMC_SATASTOPMEM_OFFS)
#define PMC_SATASTOPMEM_STOP			(1 << PMC_SATASTOPMEM_OFFS)

#define PMC_SESTOPMEM_OFFS			8
#define PMC_SESTOPMEM_MASK			(1 << PMC_SESTOPMEM_OFFS)
#define PMC_SESTOPMEM_EN			(0 << PMC_SESTOPMEM_OFFS)
#define PMC_SESTOPMEM_STOP			(1 << PMC_SESTOPMEM_OFFS)

#define PMC_NFCSTOPMEM_OFFS			10
#define PMC_NFCSTOPMEM_MASK			(1 << PMC_NFCSTOPMEM_OFFS)
#define PMC_NFCSTOPMEM_EN			(0 << PMC_NFCSTOPMEM_OFFS)
#define PMC_NFCSTOPMEM_STOP			(1 << PMC_NFCSTOPMEM_OFFS)

#define PMC_GESTOPMEM_OFFS(port)		(16 + (port))
#define PMC_GESTOPMEM_MASK(port)		(1 << PMC_GESTOPMEM_OFFS(port))
#define PMC_GESTOPMEM_EN(port)			(0 << PMC_GESTOPMEM_OFFS(port))
#define PMC_GESTOPMEM_STOP(port)		(1 << PMC_GESTOPMEM_OFFS(port))

#define PMC_GUNITSTOPMEM_OFFS			18
#define PMC_GUNITSTOPMEM_MASK			(1 << PMC_GUNITSTOPMEM_OFFS)
#define PMC_GUNITSTOPMEM_EN			(0 << PMC_GUNITSTOPMEM_OFFS)
#define PMC_GUNITSTOPMEM_STOP			(1 << PMC_GUNITSTOPMEM_OFFS)

#define PMC_PNCSTOPMEM_OFFS			19
#define PMC_PNCSTOPMEM_MASK			(1 << PMC_PNCSTOPMEM_OFFS)
#define PMC_PNCSTOPMEM_EN			(0 << PMC_PNCSTOPMEM_OFFS)
#define PMC_PNCSTOPMEM_STOP			(1 << PMC_PNCSTOPMEM_OFFS)

#define PMC_BMSTOPMEM_OFFS			20
#define PMC_BMSTOPMEM_MASK			(1 << PMC_BMSTOPMEM_OFFS)
#define PMC_BMSTOPMEM_EN			(0 << PMC_BMSTOPMEM_OFFS)
#define PMC_BMSTOPMEM_STOP			(1 << PMC_BMSTOPMEM_OFFS)

#define PMC_PONSTOPMEM_OFFS			21
#define PMC_PONSTOPMEM_MASK			(1 << PMC_PONSTOPMEM_OFFS)
#define PMC_PONSTOPMEM_EN			(0 << PMC_PONSTOPMEM_OFFS)
#define PMC_PONSTOPMEM_STOP			(1 << PMC_PONSTOPMEM_OFFS)

#define PMC_GPNMCSTOPMEM_OFFS			22
#define PMC_GPNMCSTOPMEM_MASK			(1 << PMC_GPNMCSTOPMEM_OFFS)
#define PMC_GPNMCSTOPMEM_EN			(0 << PMC_GPNMCSTOPMEM_OFFS)
#define PMC_GPNMCSTOPMEM_STOP			(1 << PMC_GPNMCSTOPMEM_OFFS)


/*  Power Management Clock Gating Control Register	*/
#define POWER_MNG_CTRL_REG			0x1821C

#define PMC_TDMSTOPCLOCK_OFFS			0
#define PMC_TDMSTOPCLOCK_MASK			(1 << PMC_TDMSTOPCLOCK_OFFS)
#define PMC_TDMSTOPCLOCK_EN			(1 << PMC_TDMSTOPCLOCK_OFFS)
#define PMC_TDMSTOPCLOCK_STOP			(0 << PMC_TDMSTOPCLOCK_OFFS)

#define PMC_PEXPHYSTOPCLOCK_OFFS		1
#define PMC_PEXPHYSTOPCLOCK_MASK		(1 << PMC_PEXPHYSTOPCLOCK_OFFS)
#define PMC_PEXPHYSTOPCLOCK_EN			(1 << PMC_PEXPHYSTOPCLOCK_OFFS)
#define PMC_PEXPHYSTOPCLOCK_STOP		(0 << PMC_PEXPHYSTOPCLOCK_OFFS)

#define PMC_PEXSTOPCLOCK_OFFS(id)		(2 + (16 * id))
#define PMC_PEXSTOPCLOCK_MASK(id)		(1 << PMC_PEXSTOPCLOCK_OFFS(id))
#define PMC_PEXSTOPCLOCK_EN(id)			(1 << PMC_PEXSTOPCLOCK_OFFS(id))
#define PMC_PEXSTOPCLOCK_STOP(id)		(0 << PMC_PEXSTOPCLOCK_OFFS(id))

#define PMC_USBSTOPCLOCK_OFFS			3
#define PMC_USBSTOPCLOCK_MASK			(1 << PMC_USBSTOPCLOCK_OFFS)
#define PMC_USBSTOPCLOCK_EN			(1 << PMC_USBSTOPCLOCK_OFFS)
#define PMC_USBSTOPCLOCK_STOP			(0 << PMC_USBSTOPCLOCK_OFFS)

#define PMC_SDIOSTOPCLOCK_OFFS			4
#define PMC_SDIOSTOPCLOCK_MASK			(1 << PMC_SDIOSTOPCLOCK_OFFS)
#define PMC_SDIOSTOPCLOCK_EN			(1 << PMC_SDIOSTOPCLOCK_OFFS)
#define PMC_SDIOSTOPCLOCK_STOP			(0 << PMC_SDIOSTOPCLOCK_OFFS)

#define PMC_COMMSTOPCLOCK_OFFS			5
#define PMC_COMMSTOPCLOCK_MASK			(1 << PMC_COMMSTOPCLOCK_OFFS)
#define PMC_COMMSTOPCLOCK_EN			(1 << PMC_COMMSTOPCLOCK_OFFS)
#define PMC_COMMSTOPCLOCK_STOP			(0 << PMC_COMMSTOPCLOCK_OFFS)

#define PMC_RUNITSTOPCLOCK_OFFS			7
#define PMC_RUNITSTOPCLOCK_MASK			(1 << PMC_RUNITSTOPCLOCK_OFFS)
#define PMC_RUNITSTOPCLOCK_EN			(1 << PMC_RUNITSTOPCLOCK_OFFS)
#define PMC_RUNITSTOPCLOCK_STOP			(0 << PMC_RUNITSTOPCLOCK_OFFS)

#define PMC_XORSTOPCLOCK_OFFS(unit)		(8 + (unit))
#define PMC_XORSTOPCLOCK_MASK(unit)		(1 << PMC_XORSTOPCLOCK_OFFS(unit))
#define PMC_XORSTOPCLOCK_EN(unit)		(1 << PMC_XORSTOPCLOCK_OFFS(unit))
#define PMC_XORSTOPCLOCK_STOP(unit)		(0 << PMC_XORSTOPCLOCK_OFFS(unit))

#define PMC_POWERSAVE_OFFS			11
#define PMC_POWERSAVE_MASK			(1 << PMC_POWERSAVE_OFFS)
#define PMC_POWERSAVE_EN			(1 << PMC_POWERSAVE_OFFS)
#define PMC_POWERSAVE_STOP			(0 << PMC_POWERSAVE_OFFS)

#define PMC_SATASTOPCLOCK_OFFS			(14)
#define PMC_SATASTOPCLOCK_MASK			(1 << PMC_SATASTOPCLOCK_OFFS)
#define PMC_SATASTOPCLOCK_EN			(1 << PMC_SATASTOPCLOCK_OFFS)
#define PMC_SATASTOPCLOCK_STOP			(0 << PMC_SATASTOPCLOCK_OFFS)

#define PMC_SESTOPCLOCK_OFFS			(15)
#define PMC_SESTOPCLOCK_MASK			(1 << PMC_SESTOPCLOCK_OFFS)
#define PMC_SESTOPCLOCK_EN			(1 << PMC_SESTOPCLOCK_OFFS)
#define PMC_SESTOPCLOCK_STOP			(0 << PMC_SESTOPCLOCK_OFFS)

#define PMC_NFECCSTOPCLOCK_OFFS			(16)
#define PMC_NFECCSTOPCLOCK_MASK			(1 << PMC_NFECCSTOPCLOCK_OFFS)
#define PMC_NFECCSTOPCLOCK_EN			(1 << PMC_NFECCSTOPCLOCK_OFFS)
#define PMC_NFECCSTOPCLOCK_STOP			(0 << PMC_NFECCSTOPCLOCK_OFFS)

#define PMC_NFCSTOPCLOCK_OFFS			(17)
#define PMC_NFCSTOPCLOCK_MASK			(1 << PMC_NFCSTOPCLOCK_OFFS)
#define PMC_NFCSTOPCLOCK_EN			(1 << PMC_NFCSTOPCLOCK_OFFS)
#define PMC_NFCSTOPCLOCK_STOP			(0 << PMC_NFCSTOPCLOCK_OFFS)

#define PMC_GESTOPCLOCK_OFFS(port)		(24 + (port))
#define PMC_GESTOPCLOCK_MASK(port)		(1 << PMC_GESTOPCLOCK_OFFS(port))
#define PMC_GESTOPCLOCK_EN(port)		(1 << PMC_GESTOPCLOCK_OFFS(port))
#define PMC_GESTOPCLOCK_STOP(port)		(0 << PMC_GESTOPCLOCK_OFFS(port))

#define PMC_PONSTOPCLOCK_OFFS			(28)
#define PMC_PONSTOPCLOCK_MASK			(1 << PMC_PONSTOPCLOCK_OFFS)
#define PMC_PONSTOPCLOCK_EN			(1 << PMC_PONSTOPCLOCK_OFFS)
#define PMC_PONSTOPCLOCK_STOP			(0 << PMC_PONSTOPCLOCK_OFFS)

#define PMC_SWITCHSTOPCLOCK_OFFS		(30)
#define PMC_SWITCHSTOPCLOCK_MASK		(1 << PMC_SWITCHSTOPCLOCK_OFFS)
#define PMC_SWITCHSTOPCLOCK_EN			(1 << PMC_SWITCHSTOPCLOCK_OFFS)
#define PMC_SWITCHSTOPCLOCK_STOP		(0 << PMC_SWITCHSTOPCLOCK_OFFS)

/* Software Reset Control Register */
#define SOFT_RESET_CTRL_REG			0x18220
#define SRC_XORSWRSTN_OFFSET			0
#define SRC_XORSWRSTN_MASK			(0x1 << SRC_XORSWRSTN_OFFSET)
#define SRC_CRYPTOSWRSTN_OFFSET			1
#define SRC_CRYPTOSWRSTN_MASK			(0x1 << SRC_CRYPTOSWRSTN_OFFSET)
#define SRC_TDMSWRSTN_OFFSET			2
#define SRC_TDMSWRSTN_MASK			(0x1 << SRC_TDMSWRSTN_OFFSET)
#define SRC_USBSWRSTN_OFFSET			3
#define SRC_USBSWRSTN_MASK			(0x1 << SRC_USBSWRSTN_OFFSET)
#define SRC_SDIOSWRSTN_OFFSET			4
#define SRC_SDIOSWRSTN_MASK			(0x1 << SRC_SDIOSWRSTN_OFFSET)
#define SRC_COMMSWRSTN_OFFSET			5
#define SRC_COMMSWRSTN_MASK			(0x1 << SRC_COMMSWRSTN_OFFSET)
#define SRC_G01SWRSTN_OFFSET			8
#define SRC_G01SWRSTN_MASK			(0x1 << SRC_G01SWRSTN_OFFSET)
#define SRC_GPONSWRSTN_OFFSET			10
#define SRC_GPONSWRSTN_MASK			(0x1 << SRC_GPONSWRSTN_OFFSET)
#define SRC_SATASWRSTN_OFFSET			12
#define SRC_SATASWRSTN_MASK			(0x1 << SRC_SATASWRSTN_OFFSET)
#define SRC_NFCSWRSTN_OFFSET			14
#define SRC_NFCSWRSTN_MASK			(0x1 << SRC_NFCSWRSTN_OFFSET)
#define SRC_LPSRDSSWRSTN_OFFSET			24
#define SRC_LPSRDSSWRSTN_MASK			(0x1 << SRC_LPSRDSSWRSTN_OFFSET)

/* TDM and PON Clock Out Divider Control Register */
#define TDM_PON_CLK_OUT_DIV_CONTROL_REG		0x18740
#define TDM_CLK_DIV_RATIO_OFFS			0
#define TDM_CLK_DIV_RATIO_MASK			(0x3f << TDM_CLK_DIV_RATIO_OFFS)
#define TDM_CLK_DIV_RATIO_8M			(0x3 << TDM_CLK_DIV_RATIO_OFFS)
#define TDM_CLK_DIV_RATIO_4M			(0x6 << TDM_CLK_DIV_RATIO_OFFS)
#define TDM_CLK_DIV_RATIO_2M			(0xc << TDM_CLK_DIV_RATIO_OFFS)
#define TDM_LOAD_RATIO_OFFS			9
#define TDM_LOAD_RATIO_MASK			(1 << TDM_LOAD_RATIO_OFFS)
#define TDM_CLK_SRC_SEL_OFFS			14
#define TDM_CLK_SRC_SEL_MASK			(3 << TDM_CLK_SRC_SEL_OFFS)
#define PLL_MODE				(0 << TDM_CLK_SRC_SEL_OFFS)
#define EXT_XTAL_MODE				(1 << TDM_CLK_SRC_SEL_OFFS)
#define EXT_REF_MODE				(2 << TDM_CLK_SRC_SEL_OFFS)

/*  Ana Grp1 Config Register	*/
#define ANA_GRP1_CONFIG_REG			0x18480
#define ANA_GRP1_CFG_DEF_VAL			0x68

/* Controler environment registers offsets */
#define MPP_CONTROL_REG(id)			(0x18000 + (id * 4))

/* Sample at Reset */
#define MPP_SAMPLE_AT_RESET(id)			(0x18230 + (id * 4))

#define MPP_GPON_MODE_SEL_OFFS			27
#define MPP_GPON_MODE_SEL_MASK			(0x1 << MPP_GPON_MODE_SEL_OFFS)

/* SYSRSTn Length Counter */
#define SYSRST_LENGTH_COUNTER_REG		0x18250
#define SLCR_COUNT_OFFS				0
#define SLCR_COUNT_MASK				(0x1FFFFFFF << SLCR_COUNT_OFFS)
#define SLCR_CLR_OFFS				31
#define SLCR_CLR_MASK				(1 << SLCR_CLR_OFFS)

#define LED_MATRIX_CTRL_REG(id)			(0x18280 + (id * 4))

/* Device ID */
#define CHIP_BOND_REG				0x18238
#define PCKG_OPT_MASK				0x3

#define MPP_OUTPUT_DRIVE_REG			0x184E4
#define MPP_GE_A_OUTPUT_DRIVE_OFFS		6
#define MPP_GE_A_1_8_OUTPUT_DRIVE		(0x1 << MPP_GE_A_OUTPUT_DRIVE_OFFS)
#define MPP_GE_A_2_5_OUTPUT_DRIVE		(0x2 << MPP_GE_A_OUTPUT_DRIVE_OFFS)
#define MPP_GE_B_OUTPUT_DRIVE_OFFS		14
#define MPP_GE_B_1_8_OUTPUT_DRIVE		(0x1 << MPP_GE_B_OUTPUT_DRIVE_OFFS)
#define MPP_GE_B_2_5_OUTPUT_DRIVE		(0x2 << MPP_GE_B_OUTPUT_DRIVE_OFFS)

/* Sample At Reset bits */
#define MSAR_BOOT_MODE_OFFS			11
#define MSAR_BOOT_MODE_MASK			(0x3f << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI_W_BOOTROM_MASK		(1 << 8)
#define MSAR_BOOT_SPI_W_BOOTROM_LIST		{ 8 }
#define MSAR_BOOT_NAND_LEG_W_BOOTROM_LIST	{ 0x4, 0x5, 0x6, 0x7, 0x9, 0xF }
#define MSAR_BOOT_NAND_W_BOOTROM_LIST		{ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,	\
						  0x37, 0x39, 0x3A, 0x3B, 0x3D, 0x3E, 0x3F }
#if 0
#define MSAR_BOOT_NOR_W_BOOTROM_LIST_LOW	{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,	\
						  0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1E, 0x1F }
#define MSAR_BOOT_NOR_W_BOOTROM_LIST_HIGH	{ 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,	\
						  0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2E, 0x2F }
#endif
#define MSAR_BOOT_NOR_W_BOOTROM_LIST		{ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,	\
						  0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1E, 0x1F,	\
						  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,	\
						  0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2E, 0x2F }

#define MSAR_TCLCK_OFFS				9
#define MSAR_TCLCK_MASK				(0x3 << MSAR_TCLCK_OFFS)
#define MSAR_TCLCK_166				(0x1 << MSAR_TCLCK_OFFS)
#define MSAR_TCLCK_200				(0x2 << MSAR_TCLCK_OFFS)

#define MV_NOR_LOW_MPPS                2
#define MV_NOR_HIGH_MPPS       3

/* Extract CPU, L2, DDR clocks SAR value from
** bits 56,39,34,33,40,28
*/
#define MSAR_CPU_DDR_L2_CLCK_EXTRACT(sar0, sar1)	((sar0 >> 1) & 0x3F)

#define MSAR_CPU_DDR_L2_CLCK_TWSI_EXTRACT(sar0, sar1)	(((sar0  & 0x1F) | ((sar1 & 0x10) << 1)))

#define PMU_POWER_IF_POLARITY_REG             (MV_DEV_PMU_REGS_OFFSET + 0x4)

#ifndef MV_ASMLANGUAGE

#define CLCK_TBL_SAT_IDX	0
#define CLCK_TBL_CPU_IDX	1
#define CLCK_TBL_DDR_IDX	2
#define CLCK_TBL_L2_IDX		3

/*	satr	CPU		SysClock/DDR	L2		*/
#define MV_CPU_DDR_L2_CLCK_TBL    { \
	{31,	1200000000,	400000000,	480000000	},\
	{36,	1600000000,	533000000,	533000000	},\
	{37,	1600000000,	400000000,	533000000	},\
	{6,     533000000,      267000000,      267000000       },\
	{20,    666000000,      333000000,      333000000       },\
	{21,    800000000,      400000000,      400000000       },\
	{22,    1000000000,     400000000,      500000000       },\
	{(MV_U32)-1, (MV_U32)-1, (MV_U32)-1,	(MV_U32)-1	} \
}

/* These macros help units to identify a target Mbus Arbiter group */
#define MV_TARGET_IS_DRAM(target)   \
			((target >= SDRAM_CS0) && (target <= SDRAM_CS3))

#define MV_TARGET_IS_PEX0(target)	\
			((target >= PEX0_MEM) && (target <= PEX0_IO))

#define MV_TARGET_IS_PEX1(target)	\
			((target >= PEX1_MEM) && (target <= PEX1_IO))

#define MV_TARGET_IS_PEX(target) (MV_TARGET_IS_PEX0(target) || MV_TARGET_IS_PEX1(target))

#define MV_TARGET_IS_DEVICE(target) \
			((target >= DEVICE_CS0) && (target <= DEVICE_CS3))

#define MV_PCI_DRAM_BAR_TO_DRAM_TARGET(bar)   0

#define	MV_TARGET_IS_AS_BOOT(target) ((target) == BOOT_ROM_CS)

#define MV_CHANGE_BOOT_CS(target)	(((target) == DEV_BOOCS) ? BOOT_ROM_CS : (target))
#if 0
		sampleAtResetTargetArray[((MV_REG_READ(MPP_SAMPLE_AT_RESET(0)) & MSAR_BOOT_MODE_MASK)	\
			>> MSAR_BOOT_MODE_OFFS)] : (target))
#endif

#define TCLK_TO_COUNTER_RATIO   1   /* counters running in Tclk */

#define MSAR_L2EXIST_OFFS			30
#define MSAR_L2EXIST_MASK			(1 << MSAR_L2EXIST_OFFS)

#define MV_NOR_LOW_MPPS		2
#define MV_NOR_HIGH_MPPS	3

#define BOOT_TARGETS_NAME_ARRAY {	\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	BOOT_ROM_CS,			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	TBL_TERM,			\
	BOOT_ROM_CS			\
}

/* For old competability */
/*#define MV_BOOTDEVICE_INDEX   	0*/

#define START_DEV_CS   		DEV_CS0
#define DEV_TO_TARGET(dev)	((dev) + START_DEV_CS)

#define PCI_IF0_MEM0		PEX0_MEM
#define PCI_IF0_IO		PEX0_IO
#define PCI_IF1_MEM0		PEX1_MEM
#define PCI_IF1_IO		PEX1_IO

/* This enumerator defines the Marvell controller target ID      */
typedef enum _mvTargetId {
	DRAM_TARGET_ID  = 0,	/* Port 0 -> DRAM interface	*/
	DEV_TARGET_ID   = 1,	/* Port 1 -> Legacy Nand/SPI	*/
	CRYPT_TARGET_ID = 3,	/* Port 3 --> Crypto Engine	*/
	PEX0_TARGET_ID  = 4,	/* Port 4 -> PCI Express0	*/
	PEX1_TARGET_ID  = 4,	/* Port 4 -> PCI Express1	*/
	ETH_TARGET_ID = 7,	/* Port 7 -> ETH0/1		*/
	PON_TARGET_ID   = 10,	/* PON unit */
	PNC_BM_TARGET_ID = 12, 	/* Port 12 -> PNC + BM Unit	*/
	NFC_TARGET_ID = 0xF,	/* Port 15 -> NFC		*/
	MAX_TARGETS_ID
} MV_TARGET_ID;


#endif /* MV_ASMLANGUAGE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
