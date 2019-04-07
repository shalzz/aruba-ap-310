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
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "spi/mvSpi.h"
#include "spi/mvSpiCmnd.h"
#include "sflash/mvSysSFlash.h"

#define MV_SYS_SFLASH_MAX_CMD_LEN 4

static struct {
	MV_U8 buf[MV_SYS_SFLASH_MAX_CMD_LEN];
	MV_U32 bufLen;
	MV_U8  transType;
} mvSysSflashCmd;

/*******************************************************************************
* mvSysSflashCommandSet
*
* DESCRIPTION:
*	System interface for sending a command to the SPI flash.
*
* INPUT:
*       flashHandle: Handle passed by OS glue by which an SPI flash is
*		     identified.
*      	cmdBuff:     Command data to be written.
*	cmdLen:	     Command length in bytes.
*	transType:   Bitmask describing the transaction type, see 
*		     SYS_SFLASH_TRANS_XX for details.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvSysSflashCommandSet(MV_VOID *flashHandle, MV_U8* cmdBuff, MV_U32 cmdLen,
		MV_U8 transType)
{
	if (cmdLen > MV_SYS_SFLASH_MAX_CMD_LEN)
		return MV_ERROR;

	if (!(transType & SYS_SFLASH_TRANS_START) || (mvSysSflashCmd.transType != 0))
		return MV_ERROR;	

	memcpy(mvSysSflashCmd.buf,cmdBuff,cmdLen);	
	mvSysSflashCmd.bufLen = cmdLen;
	mvSysSflashCmd.transType = transType;

	if (transType & SYS_SFLASH_TRANS_END)
		return mvSysSflashDataWrite(flashHandle, NULL, 0, transType);

	return MV_OK;
}


/*******************************************************************************
* mvSysSflashDataRead
*
* DESCRIPTION:
*	System interface for reading SPI flash data.
*
* INPUT:
*       flashHandle: Handle passed by OS glue by which an SPI flash is
*		     identified.
*	dataBuff:    Buffer to read the data into.
*	dataLen:     Number of bytes to read.
*	dummyBytes:  Number of dummy bytes to read before reading the real
*		     data.
*	transType:   Bitmask describing the transaction type, see 
*		     SYS_SFLASH_TRANS_XX for details.
*
* OUTPUT:
*	dataBuff: The data as read from flash.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvSysSflashDataRead(MV_VOID *flashHandle, MV_U8* dataBuff, MV_U32 dataLen,
		MV_U32 dummyBytes, MV_U8 transType)
{
	MV_STATUS  ret;	

	if (!(mvSysSflashCmd.transType & SYS_SFLASH_TRANS_START))
		return MV_ERROR;

	ret = mvSpiWriteThenRead (0, mvSysSflashCmd.buf, mvSysSflashCmd.bufLen,
			dataBuff, dataLen, dummyBytes);
	if (transType & SYS_SFLASH_TRANS_END)
		memset(&mvSysSflashCmd,0,sizeof(mvSysSflashCmd));
	return ret;
}


/*******************************************************************************
* mvSysSflashDataWrite
*
* DESCRIPTION:
*	System interface for writing SPI flash data.
*
* INPUT:
*       flashHandle: Handle passed by OS glue by which an SPI flash is
*		     identified.
*	dataBuff:    Buffer holding the data to be written.
*	dataLen:     Number of bytes to write.
*	transType:   Bitmask describing the transaction type, see 
*		     SYS_SFLASH_TRANS_XX for details.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvSysSflashDataWrite(MV_VOID *flashHandle, MV_U8* dataBuff, MV_U32 dataLen,
		MV_U8 transType)
{
	MV_STATUS ret;

	if (!(mvSysSflashCmd.transType & SYS_SFLASH_TRANS_START))
		return MV_ERROR;

	ret = mvSpiWriteThenWrite (0, mvSysSflashCmd.buf, mvSysSflashCmd.bufLen, dataBuff, dataLen);
	if (transType & SYS_SFLASH_TRANS_END)
		memset(&mvSysSflashCmd,0,sizeof(mvSysSflashCmd));
	return ret;
}


/*******************************************************************************
* mvSysSflashFreqSet
*
* DESCRIPTION:
*	System interface for controlling the SPI interface frequency.
*
* INPUT:
*       flashHandle: Handle passed by OS glue by which an SPI flash is
*		     identified.
*	freq:	     The new frequency to be configured for the SPI IF.
*
* OUTPUT:
*	None.
*
* RETURN:
*	MV_OK on success,
*	MV_ERROR otherwise.
*
*******************************************************************************/
MV_STATUS mvSysSflashFreqSet(MV_VOID *flashHandle, MV_U32 freq)
{
    return mvSpiBaudRateSet(0, freq);
}

