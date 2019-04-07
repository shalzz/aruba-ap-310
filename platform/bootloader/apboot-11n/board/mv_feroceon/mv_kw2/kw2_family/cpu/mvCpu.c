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

#include "cpu/mvCpu.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvRegs.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"

/* defines  */
#ifdef MV_DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

/* locals */

/*******************************************************************************
* mvCpuClockEntryGet - Get the CPU / DDR / L2 entry index from the
*			frequncies table.
*
* DESCRIPTION:
*       This routine searches for a matching entry in the CPU / DDR / L2
*	frequencies table according to the S@R value.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Index of matching entry, or 0xFFFFFFFF if S@R value is not valid.
*
*******************************************************************************/
static MV_U32 mvCpuClockEntryGet(MV_VOID)
{
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

	return res;
}

/*******************************************************************************
* mvCpuPclkGet - Get the CPU pClk (pipe clock)
*
* DESCRIPTION:
*       This routine extract the CPU core clock.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       32bit clock cycles in MHertz.
*
*******************************************************************************/
MV_U32 mvCpuPclkGet(MV_VOID)
{
#if defined(PCLCK_AUTO_DETECT)
	MV_U32 idx;
	MV_CPU_ARM_CLK cpuDdrL2Tbl[] = MV_CPU_DDR_L2_CLCK_TBL;

	idx = mvCpuClockEntryGet();
	if (idx == 0xFFFFFFFF)
		return 0;
	else
		return cpuDdrL2Tbl[idx].cpuClk;
#else
	return MV_DEFAULT_PCLK
#endif
}

/*******************************************************************************
* mvCpuL2ClkGet - Get the CPU L2 (CPU bus clock)
*
* DESCRIPTION:
*       This routine extract the CPU L2 clock.
*
* RETURN:
*       32bit clock cycles in Hertz.
*
*******************************************************************************/
MV_U32 mvCpuL2ClkGet(MV_VOID)
{
#ifdef L2CLK_AUTO_DETECT
	MV_U32 idx;
	MV_CPU_ARM_CLK cpuDdrL2Tbl[] = MV_CPU_DDR_L2_CLCK_TBL;

	idx = mvCpuClockEntryGet();
	if (idx == 0xFFFFFFFF)
		return 0;
	else
		return cpuDdrL2Tbl[idx].l2Clk;
#else
	return MV_BOARD_DEFAULT_L2CLK;
#endif
}

/*******************************************************************************
* mvCpuL2Exists
*
* DESCRIPTION:
*       This routine checks if L2 exists according to reset strap.
*
* RETURN:
*       MV_TRUE if L2 exists,
*	MV_FALSE otherwise.
*
*******************************************************************************/
MV_BOOL mvCpuL2Exists(MV_VOID)
{
	MV_U32 sar0;

	/* Read S@R register value */
	sar0 = MV_REG_READ(MPP_SAMPLE_AT_RESET(0));
	if (sar0 & MSAR_L2EXIST_MASK)
		return MV_TRUE;
	else
		return MV_FALSE;
}

/*******************************************************************************
* mvCpuNameGet - Get CPU name
*
* DESCRIPTION:
*       This function returns a string describing the CPU model and revision.
*
* INPUT:
*       None.
*
* OUTPUT:
*       pNameBuff - Buffer to contain board name string. Minimum size 32 chars.
*
* RETURN:
*       None.
*******************************************************************************/
MV_VOID mvCpuNameGet(char *pNameBuff)
{
	MV_U32 cpuModel;

	cpuModel = mvOsCpuPartGet();

	/* The CPU module is indicated in the Processor Version Register (PVR) */
	switch (cpuModel) {
	case CPU_PART_MRVL131:
		mvOsSPrintf(pNameBuff, "%s (Rev %d)", "Marvell Feroceon", mvOsCpuRevGet());
		break;
	case CPU_PART_ARM926:
		mvOsSPrintf(pNameBuff, "%s (Rev %d)", "ARM926", mvOsCpuRevGet());
		break;
	case CPU_PART_ARM946:
		mvOsSPrintf(pNameBuff, "%s (Rev %d)", "ARM946", mvOsCpuRevGet());
		break;
	default:
		mvOsSPrintf(pNameBuff, "??? (0x%04x) (Rev %d)", cpuModel, mvOsCpuRevGet());
		break;
	}			/* switch  */

	return;
}

#define MV_PROC_STR_SIZE 60

static void mvCpuIfGetPrefetchMode(MV_8 *buf)
{
    MV_U32 regVal = MV_REG_READ(CPU_CONFIG_REG);
    int count;

    if (regVal & BIT16)
	count = mvOsSPrintf(buf, "I-cache prefetching enabled");
    else
	count = mvOsSPrintf(buf, "I-cache prefetching disabled");
    if (regVal & BIT17)
	mvOsSPrintf(buf + count, "\nD-cache prefetching enabled");
    else
	mvOsSPrintf(buf + count, "\nD-cache prefetching disabled");
}

static void mvCpuIfGetL2EccMode(MV_8 *buf)
{
	int count;

	MV_U32 regVal = MV_REG_READ(CPU_L2_CONFIG_REG);
	if (regVal & BIT2)
		count = mvOsSPrintf(buf, "L2 ECC Enabled");
	else
		count = mvOsSPrintf(buf, "L2 ECC Disabled");

        if (regVal & BIT4)
 		mvOsSPrintf(buf + count, "\nL2 is Write-through");
	else
		mvOsSPrintf(buf + count, "\nL2 is Write-back");
}

static void mvCpuIfGetL2Mode(MV_8 *buf)
{
	MV_U32 regVal = 0;
	__asm volatile ("mrc	p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register */
	if (regVal & BIT22)
		mvOsSPrintf(buf, "L2 Enabled");
	else
		mvOsSPrintf(buf, "L2 Disabled");
}

static void mvCpuIfGetL2PrefetchMode(MV_8 *buf)
{
	MV_U32 regVal = 0;
	__asm volatile ("mrc	p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register */
	if (regVal & BIT24)
		mvOsSPrintf(buf, "L2 Prefetch Disabled");
	else
		mvOsSPrintf(buf, "L2 Prefetch Enabled");
}

static void mvCpuIfGetWriteAllocMode(MV_8 *buf)
{
	MV_U32 regVal = 0;
	__asm volatile ("mrc	p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register */
	if (regVal & BIT28)
		mvOsSPrintf(buf, "Write Allocate Enabled");
	else
		mvOsSPrintf(buf, "Write Allocate Disabled");
}

static void mvCpuIfGetCpuStreamMode(MV_8 *buf)
{
	MV_U32 regVal = 0;
	__asm volatile ("mrc	p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register */
	if (regVal & BIT29)
		mvOsSPrintf(buf, "CPU Streaming Enabled");
	else
		mvOsSPrintf(buf, "CPU Streaming Disabled");
}

#undef mvOsPrintf
#define mvOsPrintf printf

static void mvCpuIfPrintCpuRegs(void)
{
	MV_U32 regVal = 0;

	__asm volatile ("mrc p15, 1, %0, c15, c1, 0" : "=r" (regVal));	/* Read Marvell extra features register */
	mvOsPrintf("Extra Feature Reg = 0x%x\n", regVal);

	__asm volatile ("mrc	p15, 0, %0, c1, c0, 0" : "=r" (regVal));	/* Read Control register */
	mvOsPrintf("Control Reg = 0x%x\n", regVal);

	__asm volatile ("mrc	p15, 0, %0, c0, c0, 0" : "=r" (regVal));	/* Read ID Code register */
	mvOsPrintf("ID Code Reg = 0x%x\n", regVal);

	__asm volatile ("mrc	p15, 0, %0, c0, c0, 1" : "=r" (regVal));	/* Read Cache Type register */
	mvOsPrintf("Cache Type Reg = 0x%x\n", regVal);

   regVal = MV_REG_READ(CPU_CONFIG_REG);
   mvOsPrintf("CPU Config Reg = 0x%x\n",regVal);

   regVal = MV_REG_READ(CPU_L2_CONFIG_REG);
   mvOsPrintf("CPU L2 Config Reg = 0x%x\n",regVal);
}

MV_U32 mvCpuIfPrintSystemConfig(MV_8 *buffer, MV_U32 index)
{
	MV_U32 count = 0;

	MV_8 L2_ECC_str[MV_PROC_STR_SIZE];
	MV_8 L2_En_str[MV_PROC_STR_SIZE];
	MV_8 L1_Prefetch_str[MV_PROC_STR_SIZE];
	MV_8 L2_Prefetch_str[MV_PROC_STR_SIZE];
	MV_8 Write_Alloc_str[MV_PROC_STR_SIZE];
	MV_8 Cpu_Stream_str[MV_PROC_STR_SIZE];

	mvCpuIfGetL2Mode(L2_En_str);
	mvCpuIfGetL2EccMode(L2_ECC_str);
	mvCpuIfGetL2PrefetchMode(L2_Prefetch_str);
	mvCpuIfGetPrefetchMode(L1_Prefetch_str);
	mvCpuIfGetWriteAllocMode(Write_Alloc_str);
	mvCpuIfGetCpuStreamMode(Cpu_Stream_str);
	mvCpuIfPrintCpuRegs();

	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_En_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_ECC_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", L2_Prefetch_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", L1_Prefetch_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", Write_Alloc_str);
	count += mvOsSPrintf(buffer + count + index, "%s\n", Cpu_Stream_str);
	count += mvOsSPrintf(buffer + count + index, "CPU Config Reg = 0x%08x\n", MV_REG_READ(CPU_CONFIG_REG));
	count += mvOsSPrintf(buffer + count + index, "L2 Config Reg = 0x%08x\n", MV_REG_READ(CPU_L2_CONFIG_REG));
	return count;
}

MV_U32 whoAmI(MV_VOID)
{
	return 0;
}
