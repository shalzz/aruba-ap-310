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
/* includes */
#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvPdmaRegs.h"
#include "mvPdma.h"

/* Locals */

MV_TARGET pdmaAddrDecPrioTap[] = 
{
#if defined(MV_INCLUDE_SDRAM_CS0)
	SDRAM_CS0,
#endif
#if defined(MV_INCLUDE_SDRAM_CS1)
	SDRAM_CS1,
#endif
#if 0 
	INTER_REGS, /* Not required for PDMA because we deal with the NAND registers separately */
#endif
   	TBL_TERM
};

static MV_STATUS pdmaWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin);

/*******************************************************************************
* mvPdmaWinInit
*
* DESCRIPTION:
*       Initialize the PDMA peripheral target address window.
*
* INPUT:
*	addWinMap: An array holding the address decoding information for the
*		    system.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_ERROR if register parameters are invalid.
*
*******************************************************************************/
MV_STATUS mvPdmaWinInit(MV_UNIT_WIN_INFO *addrWinMap)
{
    MV_U32		winNum;
    MV_UNIT_WIN_INFO	*addrDecWin;
    MV_U32		winPrioIndex = 0;

    /* Initiate PDMA address decode */

    /* First disable all address decode windows */
    for (winNum = 0; winNum < PDMA_MAX_ADDR_DEC_WIN; winNum++)
	MV_REG_BIT_RESET(PDMA_WINDOW_CONTROL_REG(winNum), PDMA_WIN_EN);

    /* Go through all windows in user table until table terminator	*/
    for (winNum = 0; ((pdmaAddrDecPrioTap[winPrioIndex] != TBL_TERM) &&
			    (winNum < PDMA_MAX_ADDR_DEC_WIN)); ) {
        addrDecWin = &addrWinMap[pdmaAddrDecPrioTap[winPrioIndex]];
	if (addrDecWin->enable == MV_TRUE) {
	    if (MV_OK != mvPdmaWinWrite(winNum, addrDecWin)) {
		mvOsPrintf("mvPdmaInit: ERR. mvPdmaWinSet failed\n");
		return MV_ERROR;
	    }
	    winNum++;
	}
	winPrioIndex++;			
    }

    /* Window configuration for NAND registers */
    MV_REG_WRITE(PDMA_WINDOW_BASE_REG(2), INTER_REGS_BASE);
    MV_REG_WRITE(PDMA_WINDOW_CONTROL_REG(2), 0x03FF03C1);
    
    return MV_OK;
}

/*******************************************************************************
* mvPdmaWinWrite
*
* DESCRIPTION:
*       This function sets a peripheral target (e.g. SDRAM bank0, PCI_MEM0) 
*       address window, also known as address decode window. 
*       After setting this target window, the PDMA will be able to access the 
*       target within the address window. 
*
* INPUT:
*       winNum      - PDMA to target address decode window number.
*       pAddrDecWin - PDMA target window data structure.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK on success,
*	MV_BAD_PARAM if winNum is invalid.
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvPdmaWinWrite(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
    MV_U32  baseReg, sizeReg;
    MV_U32  sizeToReg=0;

    /* Parameter checking   */
    if (winNum >= PDMA_MAX_ADDR_DEC_WIN) {
	mvOsPrintf("mvPdmaWinSet: ERR. Invalid win num %d\n",winNum);
	return MV_BAD_PARAM;
    }
    
    /* Check if the requested window overlapps with current windows         */
    if (MV_TRUE == pdmaWinOverlapDetect(winNum, &pAddrDecWin->addrWin))
    {
	mvOsPrintf("mvPdmaWinSet: ERR. Window %d overlap\n", winNum);
	return MV_ERROR;
    }

    /* check if address is aligned to the size */
    if (MV_IS_NOT_ALIGN(pAddrDecWin->addrWin.baseLow, pAddrDecWin->addrWin.size)) {
	mvOsPrintf("mvPdmaWinSet: Error setting PDMA window %d.\n"\
			"Address 0x%08x is unaligned to size 0x%x.\n",
			winNum,
			pAddrDecWin->addrWin.baseLow,
			pAddrDecWin->addrWin.size);
	return MV_ERROR;
    }

    /* Size parameter validity check.			*/
    if (!MV_IS_POWER_OF_2(pAddrDecWin->addrWin.size)) {
	mvOsPrintf("mvPdmaWinWrite: Illegal window size.\n");
	return MV_ERROR;
    }

    baseReg = MV_REG_READ(PDMA_WINDOW_BASE_REG(winNum));
    sizeReg = MV_REG_READ(PDMA_WINDOW_CONTROL_REG(winNum));

    /* Write to address decode Base Address Register	*/
    baseReg &= ~PDMA_WIN_BASE_MASK;
    baseReg |= (pAddrDecWin->addrWin.baseLow & PDMA_WIN_BASE_MASK);

    /* set size */
    sizeToReg = (pAddrDecWin->addrWin.size / PDMA_WIN_SIZE_GRANULARITY) - 1;
    sizeReg &= ~PDMA_WIN_SIZE_MASK;
    sizeReg |= (sizeToReg << PDMA_WIN_SIZE_OFFS);

    /* set attributes */
    sizeReg &= ~PDMA_WIN_ATTR_MASK;
    sizeReg |= pAddrDecWin->attrib << PDMA_WIN_ATTR_OFFS;

    /* set target ID */
    sizeReg &= ~PDMA_WIN_TARGET_MASK;
    sizeReg |= pAddrDecWin->targetId << PDMA_WIN_TARGET_OFFS;

    /* for the safe side we disable the window before writing the new values */
    mvPdmaWinEnable(winNum,MV_FALSE);

    MV_REG_WRITE(PDMA_WINDOW_BASE_REG(winNum), baseReg);

    /* Write to address decode Size Register                             */
    MV_REG_WRITE(PDMA_WINDOW_CONTROL_REG(winNum), sizeReg);

    /* Enable address decode target window                               */
    if (pAddrDecWin->enable == MV_TRUE)
	mvPdmaWinEnable(winNum, MV_TRUE);

    return MV_OK;
}

/*******************************************************************************
* mvPdmaWinRead
*
* DESCRIPTION:
*	Read PDMA peripheral target address window.
*
* INPUT:
*       winNum - PDMA to target address decode window number.
*
* OUTPUT:
*       pAddrDecWin - PDMA target window data structure.
*
* RETURN:
*	MV_BAD_PARAM if winNum is invalid.
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvPdmaWinRead(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin)
{
    MV_U32 sizeReg, baseReg;
    MV_U32 sizeRegVal;

    /* Parameter checking   */
    if (winNum >= PDMA_MAX_ADDR_DEC_WIN) {
	mvOsPrintf("mvPdmaWinGet: ERR. Invalid winNum %d\n", winNum);
	return MV_NOT_SUPPORTED;
    }

    baseReg = MV_REG_READ(PDMA_WINDOW_BASE_REG(winNum));
    sizeReg = MV_REG_READ(PDMA_WINDOW_CONTROL_REG(winNum));

    /* base and size */
    sizeRegVal = (sizeReg & PDMA_WIN_SIZE_MASK) >> PDMA_WIN_SIZE_OFFS;
    pAddrDecWin->addrWin.size = (sizeRegVal + 1) * PDMA_WIN_SIZE_GRANULARITY;
    pAddrDecWin->addrWin.baseLow = (baseReg & PDMA_WIN_BASE_MASK)/* >> PDMA_WIN_BASE_OFFS*/;
    pAddrDecWin->addrWin.baseHigh = 0;

    /* attrib and targetId */
    pAddrDecWin->attrib = (sizeReg & PDMA_WIN_ATTR_MASK) >> PDMA_WIN_ATTR_OFFS;
    pAddrDecWin->targetId = (sizeReg & PDMA_WIN_TARGET_MASK) >> PDMA_WIN_TARGET_OFFS;

    /* Check if window is enabled   */
    if (sizeReg & PDMA_WIN_EN)
	pAddrDecWin->enable = MV_TRUE;
    else
    	pAddrDecWin->enable = MV_FALSE;

    return MV_OK;
}


/*******************************************************************************
* pdmaWinOverlapDetect - Detect PDMA address windows overlapping
*
* DESCRIPTION:
*       An unpredicted behaviur is expected in case PDMA address decode 
*       windows overlapps.
*       This function detects PDMA address decode windows overlapping of a 
*       specified window. The function does not check the window itself for 
*       overlapping. The function also skipps disabled address decode windows.
*
* INPUT:
*       winNum      - address decode window number.
*       pAddrDecWin - An address decode window struct.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_TRUE if the given address window overlap current address
*       decode map, MV_FALSE otherwise, MV_ERROR if reading invalid data 
*       from registers.
*
*******************************************************************************/
static MV_STATUS pdmaWinOverlapDetect(MV_U32 winNum, MV_ADDR_WIN *pAddrWin)
{
	MV_U32		 winControlReg;
	MV_U32		 winNumIndex;
	MV_UNIT_WIN_INFO addrDecWin;

	/* Read base address enable register. Do not check disabled windows */
	winControlReg = MV_REG_READ(PDMA_WINDOW_CONTROL_REG(winNum));

	for (winNumIndex = 0; winNumIndex < PDMA_MAX_ADDR_DEC_WIN; winNumIndex++)
	{
		/* Do not check window itself		*/
		if (winNumIndex == winNum)
		{
			continue;
		}

		/* Do not check disabled windows	*/
		if (!(winControlReg & PDMA_WIN_EN))
		{
			continue;
		}

		/* Get window parameters 	*/
		if (MV_OK != mvPdmaWinRead(winNumIndex, &addrDecWin))
		{
			mvOsPrintf("pdmaWinOverlapDetect: mvPdmaWinGet returned error\n");
            		return MV_ERROR;
		}

		if (MV_TRUE == mvWinOverlapTest(pAddrWin, &(addrDecWin.addrWin)))
		{
			mvOsPrintf("pdmaWinOverlapDetect: ctrlWinOverlapTest returned error\n");
			return MV_TRUE;
		}        
	}
	return MV_FALSE;
}


/*******************************************************************************
* mvPdmaWinEnable - Enable/disable a PDMA to target address window
*
* DESCRIPTION:
*       This function enable/disable a PDMA to target address window.
*       According to parameter 'enable' the routine will enable the 
*       window, thus enabling PDMA accesses (before enabling the window it is 
*       tested for overlapping). Otherwise, the window will be disabled.
*
* INPUT:
*       winNum - PDMA to target address decode window number.
*       enable - Enable/disable parameter.
*
* OUTPUT:
*       N/A
*
* RETURN:
*       MV_ERROR if decode window number was wrong or enabled window overlapps.
*
*******************************************************************************/
MV_STATUS mvPdmaWinEnable(MV_U32 winNum,MV_BOOL enable)
{
    /* Parameter checking   */
    if (winNum >= PDMA_MAX_ADDR_DEC_WIN) {
	mvOsPrintf("mvPdmaWinEnable:ERR. Invalid winNum%d\n",winNum);
	return MV_ERROR;
    }

    if (enable == MV_TRUE)
	MV_REG_BIT_SET(PDMA_WINDOW_CONTROL_REG(winNum), PDMA_WIN_EN);
    else
	/* Disable address decode target window */
	MV_REG_BIT_RESET(PDMA_WINDOW_CONTROL_REG(winNum), PDMA_WIN_EN);

    return MV_OK;
}
