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
#include "pex/mvPex.h"
#include "pex/mvPexRegs.h"
#include "ctrlEnv/sys/mvCpuIf.h"

MV_STATUS mvPexTargetWinGet(MV_U32 pexIf, MV_U32 winNum, MV_PEX_DEC_WIN *pAddrDecWin);


/*******************************************************************************
* mvSysPexInit - Initialize the Pex subsystem
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
MV_STATUS mvSysPexInit(MV_U32 pexIf, MV_PEX_TYPE pexType)
{
	MV_PEX_HAL_DATA halData;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;

	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if(status == MV_OK)
		status = mvPexWinInit(pexIf, pexType, addrWinMap);

	if(status == MV_OK) {
		halData.ctrlModel = mvCtrlModelGet();
		halData.maxPexIf = mvCtrlPexMaxIfGet();
		status = mvPexInit(pexIf, pexType, &halData);
	}

	return status;
}

/*******************************************************************************
* mvSysPexCpuIfEnable -
*
* DESCRIPTION:
*	Enable PCI-E in CPU subsystem.
*
* INPUT:
*       pexIf - The PCI-E Interface number to enable.
*
* OUTPUT:
*	None.
*
* RETURN:
*       None.
*
*******************************************************************************/
MV_VOID mvSysPexCpuIfEnable(MV_U32 pexIf)
{
	mvCpuIfEnablePex(pexIf);
	return;
}


/*******************************************************************************
* mvPexAddrDecShow - Print the PEX address decode map (BARs and windows).
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
MV_VOID mvPexAddrDecShow(MV_VOID)
{
	MV_PEX_BAR pexBar;
	MV_PEX_DEC_WIN win;
	MV_U32 pexIf;
	MV_U32 bar,winNum;

	for( pexIf = 0; pexIf < mvCtrlPexMaxIfGet(); pexIf++ )
	{
		if (MV_FALSE == mvCtrlPwrClckGet(PEX_UNIT_ID, pexIf)) continue;
		mvOsOutput( "\n" );
		mvOsOutput( "PEX%d:\n", pexIf );
		mvOsOutput( "-----\n" );

		mvOsOutput( "\nPex Bars \n\n");

		for( bar = 0; bar < PEX_MAX_BARS; bar++ ) 
		{
			memset( &pexBar, 0, sizeof(MV_PEX_BAR) );

			mvOsOutput( "%s ", pexBarNameGet(bar) );

			if( mvPexBarGet( pexIf, bar, &pexBar ) == MV_OK )
			{
				if( pexBar.enable )
				{
                    			mvOsOutput( "base %08x, ", pexBar.addrWin.baseLow );
                    			mvSizePrint( pexBar.addrWin.size );
                    			mvOsOutput( "\n" );
				}
				else
					mvOsOutput( "disable\n" );
			}
		}
		mvOsOutput( "\nPex Decode Windows\n\n");

		for( winNum = 0; winNum < PEX_MAX_TARGET_WIN - 2; winNum++)
		{
			memset( &win, 0,sizeof(MV_PEX_DEC_WIN) );

			mvOsOutput( "win%d - ", winNum );

			if ( mvPexTargetWinRead(pexIf,winNum,&win) == MV_OK)
			{
				if (win.winInfo.enable)
				{
					/* In MV88F6781 there is no differentiation between different		*/
					/* DRAM Chip Selects, they all use the same target ID and attributes.	*/
					/* So in order to print correctly we use i as the target.		*/
					/* When i is 1 it is SDRAM_CS1 etc.			 		*/
					if (mvCtrlTargetByWinInfoGet(&win.winInfo) != SDRAM_CS0) {
						mvOsOutput( "%s base %08x, ",
							mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&win.winInfo)), 
							win.winInfo.addrWin.baseLow );
					}
					else {
						mvOsOutput( "%s base %08x, ",
							mvCtrlTargetNameGet(winNum), win.winInfo.addrWin.baseLow );
					}
					mvOsOutput( "...." );
					mvSizePrint( win.winInfo.addrWin.size );

					mvOsOutput( "\n" );
				}
				else
					mvOsOutput( "disable\n" );
			}
		}
	
		memset( &win, 0,sizeof(MV_PEX_DEC_WIN) );

		mvOsOutput( "default win - " );

		if ( mvPexTargetWinRead(pexIf, MV_PEX_WIN_DEFAULT, &win) == MV_OK)
		{
			mvOsOutput( "%s ",
			mvCtrlTargetNameGet(win.target) );
			mvOsOutput( "\n" );
		}
		memset( &win, 0,sizeof(MV_PEX_DEC_WIN) );

		mvOsOutput( "Expansion ROM - " );

		if ( mvPexTargetWinRead(pexIf, MV_PEX_WIN_EXP_ROM, &win) == MV_OK)
		{
			mvOsOutput( "%s ",
			mvCtrlTargetNameGet(win.target) );
			mvOsOutput( "\n" );
		}
	}
}

