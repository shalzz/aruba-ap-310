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

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "boardEnv/mvBoardEnvLib.h"
#include "cpu/mvCpu.h"
#include "eth/mvEth.h"


/*******************************************************************************
* mvSysEthInit - Initialize the Eth subsystem
*
* DESCRIPTION:
*
* INPUT:
*       None
* OUTPUT:
*		None
* RETURN:
*       None
*
*******************************************************************************/
MV_VOID mvSysEthInit(MV_VOID)
{
	MV_ETH_HAL_DATA halData;
	MV_U32 port;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;

	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if(status != MV_OK)
		return;

	{
		int i;
		for(i = 0; i < MAX_TARGETS; i++) {
			if(addrWinMap[i].enable == MV_FALSE)
				continue;
// 			printf("%d - Base 0x%08x , Size = 0x%08x.\n", i,
// 					addrWinMap[i].addrWin.baseLow,
// 					addrWinMap[i].addrWin.size);
		}
	}
	halData.maxPortNum = mvCtrlEthMaxPortGet();
	halData.cpuPclk = mvCpuPclkGet();
	halData.tclk = mvBoardTclkGet();
#ifdef ETH_DESCR_IN_SRAM
	halData.sramSize = mvCtrlSramSizeGet();
#endif

	for (port=0;port < halData.maxPortNum;port++) {
		if(mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port) == MV_FALSE) {
			halData.portData[port].powerOn = MV_FALSE;
			continue;
		}
		status = mvEthWinInit(port, addrWinMap);
		if(status == MV_OK) {
			halData.portData[port].powerOn = MV_TRUE;
			halData.portData[port].phyAddr = mvBoardPhyAddrGet(port);
			halData.portData[port].isSgmii = mvBoardIsPortInSgmii(port);
			halData.portData[port].macSpeed = mvBoardMacSpeedGet(port);
		}
	}

	mvEthHalInit(&halData);

	return;
}

/*******************************************************************************
* mvEthAddrDecShow - Print the Etherent address decode map.
*
* DESCRIPTION:
*       This function print the Etherent address decode map.
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
void    mvEthPortAddrDecShow(int port)
{
    MV_UNIT_WIN_INFO	win;
    int             	i;

    mvOsOutput( "\n" );
    mvOsOutput( "ETH %d:\n", port );
    mvOsOutput( "----\n" );

    for( i = 0; i < ETH_MAX_DECODE_WIN; i++ )
    {
        memset( &win, 0, sizeof(ETH_MAX_DECODE_WIN) );

        mvOsOutput( "win%d - ", i );

        if( mvEthWinRead(port, i, &win ) == MV_OK )
        {
            if( win.enable )
            {
		/* In MV88F6781 there is no differentiation between different DRAM Chip Selects,	*/
		/* they all use the same target ID and attributes. So in order to print correctly	*/
		/* we use i as the target. When i is 1 it is SDRAM_CS1 etc.				*/
		if (mvCtrlTargetByWinInfoGet(&win) != SDRAM_CS0) {
                	mvOsOutput( "%s base %08x, ",
                		mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win)), win.addrWin.baseLow );
		}
		else {
                	mvOsOutput( "%s base %08x, ",
                		mvCtrlTargetNameGet(i), win.addrWin.baseLow );
		}
                mvOsOutput( "...." );
                mvSizePrint( win.addrWin.size );

                mvOsOutput( "\n" );
            }
            else
                mvOsOutput( "disable\n" );
        }
    }
    return;
}

void    mvEthAddrDecShow(void)
{
    int port;

    for(port=0; port<MV_ETH_MAX_PORTS; port++)
    {
	if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, port))
		continue;
	mvEthPortAddrDecShow(port);
    }
}



