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

/*******************************************************************************
* mvPdma.h - Header File for :
*
* DESCRIPTION:
*       This file contains Marvell Controller PDMA HW library API.
*       NOTE: This HW library API assumes PDMA source, destination and 
*       descriptors are cache coherent. 
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/


#ifndef __INCmvPdmah
#define __INCmvPdmah

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysPdmaConfig.h"

/* defines and enumerations */

#define MV_PDMA_DEBUG

#define MV_PDMA_MAX_CHANNELS_NUM	16	/* maximum number of PDMA channels */
#define MV_PDMA_DESC_ALIGNMENT		0x10	/* 16-byte aligment restriction */
#define MV_PDMA_MAX_TRANSFER_SIZE	8191	/* maximum 8KByte minus 1 */

/* use for enabling the PDMA unit interrupts		*/
/* pass as a bitmask OR, for example:			*/
/* (MV_PDMA_END_INTR_EN | MV_PDMA_START_INTR_EN)	*/
/* To identify normal channel stop it is recommended	*/
/* to use MV_PDMA_END_INTR_EN and not MV_PDMA_STOP_INTR_EN */
#define MV_PDMA_END_INTR_EN		DCMD_ENDIRQEN
#define MV_PDMA_START_INTR_EN		DCMD_STARTIRQEN
#define MV_PDMA_STOP_INTR_EN		DCSR_STOPIRQEN
#define MV_PDMA_REQ_AFTER_STOP_INTR_EN	DCSR_RASIRQEN
#define MV_PDMA_END_OF_RX_INTR_EN	DCSR_EORIRQEN

/* Note: do not change the values for these enumerations, */
/* they are used in mvPdmaChanAlloc() as indices to chanMapRequestOffsetTable. */
typedef enum {

	MV_PDMA_AC97_MIC =		0, 
	MV_PDMA_AC97_MODEM_RX =		1, 
	MV_PDMA_AC97_MODEM_TX =		2, 
	MV_PDMA_AC97_AUDIO_RX =		3, 
	MV_PDMA_AC97_AUDIO_TX =		4, 
	MV_PDMA_AC97_SURROUND_TX =	5, 
	MV_PDMA_AC97_CENTER_TX =	6, 
	MV_PDMA_NAND_DATA =		7, 
	MV_PDMA_NAND_COMMAND =		8, 
	MV_PDMA_SSP_RX =		9, 
	MV_PDMA_SSP_TX =		10, 
	MV_PDMA_MEMORY =		11	/* Note: MUST be last enum */
	
} MV_PDMA_PERIPH_TYPE;

typedef enum {

	MV_PDMA_PERIPH_TO_MEM, 
	MV_PDMA_MEM_TO_PERIPH, 
	MV_PDMA_MEM_TO_MEM

} MV_PDMA_TRANSACTION_TYPE;

typedef enum {

	MV_PDMA_CHANNEL_STOPPED, 
	MV_PDMA_CHANNEL_RUNNING, 
	MV_PDMA_CHANNEL_ERROR

} MV_PDMA_CHAN_STATE;

typedef enum {

	MV_PDMA_ALIGNMENT_DEFAULT, 
	MV_PDMA_ALIGNMENT_USER

} MV_PDMA_ALIGNMENT;

/* typedefs */

/* This struct describes PDMA channel */
typedef struct _mvPdmaChannel 
{
	MV_U8 chanNumber;
	MV_PDMA_PERIPH_TYPE periphType;
	MV_U8 srcFlowControl;
	MV_U8 dstFlowCtrl;
	MV_U32 intrEnMask;
	MV_U32 burstSize;
	MV_U32 width;
	
} MV_PDMA_CHANNEL;

/* This struct describes PDMA descriptor structure */
typedef struct _mvPdmaDesc
{
    MV_U32 	physDescPtr;	/* The physical address of the next descriptor, and a STOP flag bit */
    MV_U32 	physSrcAddr;	/* The physical source address                  */
    MV_U32 	physDestAddr;	/* The physical destination address             */
    MV_U32	commandValue;	/* The Command value, including transfer size	*/

} MV_PDMA_DESC;

#define MV_PDMA_DESC_SIZE	(sizeof(MV_PDMA_DESC))


/* mvPdma.h API list */
MV_STATUS mvPdmaHalInit(MV_U8 pdmaNumChannels);

MV_STATUS mvPdmaChanAlloc(MV_PDMA_PERIPH_TYPE chanType, 
			MV_U32 intrEnMask, 
			MV_PDMA_CHANNEL *chanHndl);

MV_STATUS mvPdmaChanFree(MV_PDMA_CHANNEL *chanHndl);

MV_STATUS mvPdmaChanAlignmentSet(MV_PDMA_CHANNEL *chanHndl, MV_PDMA_ALIGNMENT alignment);

MV_STATUS mvPdmaChanAlignmentGet(MV_PDMA_CHANNEL *chanHndl, MV_PDMA_ALIGNMENT *alignment);

MV_PDMA_CHAN_STATE mvPdmaChannelStateGet(MV_PDMA_CHANNEL *chanHndl);

MV_VOID mvPdmaChanRegsPrint(MV_PDMA_CHANNEL *chanHndl);

MV_STATUS mvPdmaChanTransfer(MV_PDMA_CHANNEL *chanHndl, 
				MV_PDMA_TRANSACTION_TYPE transType, 
				MV_U32 phySrcAddr, 
				MV_U32 phyDstAddr, 
				MV_U16 size, 
				MV_U32 phyNextDescriptor);

MV_STATUS mvPdmaMemInit(MV_PDMA_CHANNEL *chanHndl, 
			MV_U32 phyPatternPtr, 
			MV_U32 phyDstAddr, 
			MV_U16 size);

MV_U32 mvPdmaCommandRegCalc(MV_PDMA_CHANNEL *chanHndl, 
				MV_PDMA_TRANSACTION_TYPE transType, 
				MV_U16 size);
MV_STATUS mvPdmaCommandIntrEnable( MV_PDMA_CHANNEL *chanHndl,
				MV_U32 *command);
MV_STATUS mvPdmaUnitStateStore(MV_U32 *stateData, MV_U32 *len);

MV_STATUS mvPdmaWinInit(MV_UNIT_WIN_INFO *addrWinMap);
MV_STATUS mvPdmaWinWrite(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);
MV_STATUS mvPdmaWinRead(MV_U32 winNum, MV_UNIT_WIN_INFO *pAddrDecWin);
MV_STATUS mvPdmaWinEnable(MV_U32 winNum,MV_BOOL enable);

#ifdef MV_PDMA_DEBUG

MV_STATUS mvPdmaSrcAddrRegSet (MV_U8 chan, MV_U32 value);
MV_STATUS mvPdmaDstAddrRegSet(MV_U8 chan, MV_U32 value);
MV_STATUS mvPdmaCmdRegSet(MV_U8 chan, MV_U32 value);
MV_STATUS mvPdmaCntrlRegSet(MV_U8 chan, MV_U32 value);

#endif /* MV_PDMA_DEBUG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvPdmah */


