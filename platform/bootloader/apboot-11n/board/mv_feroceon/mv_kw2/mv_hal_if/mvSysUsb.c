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
#include "usb/mvUsb.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"
#include "usb/mvUsbRegs.h"

/*******************************************************************************
* mvSysUsbHalInit - Initialize the USB subsystem
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
MV_STATUS   mvSysUsbInit(MV_U32 dev, MV_BOOL isHost)
{
	MV_USB_HAL_DATA halData;
	MV_UNIT_WIN_INFO addrWinMap[MAX_TARGETS + 1];
	MV_STATUS status;

	status = mvCtrlAddrWinMapBuild(addrWinMap, MAX_TARGETS + 1);
	if(status == MV_OK)
		status = mvUsbWinInit(dev, addrWinMap);

	if(status == MV_OK) {
		halData.ctrlModel = mvCtrlModelGet();
		halData.ctrlRev = mvCtrlRevGet();
		status = mvUsbHalInit(dev, isHost, &halData);
	}

	return status;
}

/*******************************************************************************
* mvUsbAddrDecShow - Print the USB address decode map.
*
* DESCRIPTION:
*       This function print the USB address decode map.
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
MV_VOID mvUsbAddrDecShow(MV_VOID)
{
#ifndef MV_INCLUDE_USB
    return;
#else
    MV_UNIT_WIN_INFO	addrDecWin;
    int         	i, winNum;

    mvOsOutput( "\n" );
    mvOsOutput( "USB:\n" );
    mvOsOutput( "----\n" );

    for(i=0; i<mvCtrlUsbMaxGet(); i++)
    {
        mvOsOutput( "Device %d:\n", i);

        for(winNum = 0; winNum < MV_USB_MAX_ADDR_DECODE_WIN; winNum++)
        {
            memset(&addrDecWin, 0, sizeof(MV_DEC_WIN) );

            mvOsOutput( "win%d - ", winNum );

            if( mvUsbWinRead(i, winNum, &addrDecWin ) == MV_OK )
            {
                if( addrDecWin.enable )
                {
			/* In MV88F6781 there is no differentiation between different DRAM Chip Selects,	*/
			/* they all use the same target ID and attributes. So in order to print correctly	*/
			/* we use i as the target. When i is 1 it is SDRAM_CS1 etc.				*/
			if (mvCtrlTargetByWinInfoGet(&addrDecWin) != SDRAM_CS0) {
				mvOsOutput( "%s base %08x, ",
                       		mvCtrlTargetNameGet(mvCtrlTargetByWinInfoGet(&addrDecWin)), addrDecWin.addrWin.baseLow );
			}
			else {
				mvOsOutput( "%s base %08x, ",
                        		mvCtrlTargetNameGet(winNum), addrDecWin.addrWin.baseLow);
			}

                    	mvSizePrint( addrDecWin.addrWin.size );

#if defined(MV645xx) || defined(MV646xx)
                    switch( addrDecWin.addrWinAttr.swapType)
                    {
                        case MV_BYTE_SWAP:
                            mvOsOutput( "BYTE_SWAP, " );
                            break;
                        case MV_NO_SWAP:
                            mvOsOutput( "NO_SWAP  , " );
                            break;
                        case MV_BYTE_WORD_SWAP:
                            mvOsOutput( "BYTE_WORD_SWAP, " );
                            break;
                        case MV_WORD_SWAP:
                            mvOsOutput( "WORD_SWAP, " );
                            break;
                        default:
                            mvOsOutput( "SWAP N/A , " );
                    }

                    switch( addrDecWin.addrWinAttr.cachePolicy )
                    {
                        case NO_COHERENCY:
                            mvOsOutput( "NO_COHERENCY , " );
                            break;
                        case WT_COHERENCY:
                            mvOsOutput( "WT_COHERENCY , " );
                            break;
                        case WB_COHERENCY:
                            mvOsOutput( "WB_COHERENCY , " );
                            break;
                        default:
                            mvOsOutput( "COHERENCY N/A, " );
                    }

                    switch( addrDecWin.addrWinAttr.pcixNoSnoop )
                    {
                        case 0:
                            mvOsOutput( "PCI-X NS inactive, " );
                            break;
                        case 1:
                            mvOsOutput( "PCI-X NS active  , " );
                            break;
                        default:
                            mvOsOutput( "PCI-X NS N/A     , " );
                    }

                    switch( addrDecWin.addrWinAttr.p2pReq64 )
                    {
                        case 0:
                            mvOsOutput( "REQ64 force" );
                            break;
                        case 1:
                            mvOsOutput( "REQ64 detect" );
                            break;
                        default:
                            mvOsOutput( "REQ64 N/A" );
                    }
#endif /* MV645xx || MV646xx */
                    mvOsOutput( "\n" );
                }
                else
                    mvOsOutput( "disable\n" );
            }
        }
    }
#endif
}


