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
* mvPdma.c - Implementation file for PDMA HW library
*
* DESCRIPTION:
*       This file contains Marvell Controller PDMA HW library API 
*       implementation.
*       NOTE: 
*       1) This HW library API assumes PDMA source, destination and 
*          descriptors are cache coherent. 
*       2) In order to gain high performance, the API does not parform 
*          API parameter checking.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvSysPdmaConfig.h"
#include "mvPdmaRegs.h"
#include "mvPdma.h"


/* defines  */
#define MV_CHANNEL_TAKEN	1
#define MV_CHANNEL_FREE		0

#ifdef MV_NFC_REG_DBG
extern MV_U32 mvNfcDbgFlag;
MV_U32 pdma_dbg_read(MV_U32 addr)
{
	MV_U32 reg = MV_MEMIO_LE32_READ((addr));
	if (mvNfcDbgFlag) mvOsPrintf("PDMA read  0x%08x = %08x\n", addr, reg);
	return reg;
}

MV_VOID pdma_dbg_write(MV_U32 addr, MV_U32 val)
{
	MV_MEMIO_LE32_WRITE((addr), (val));
	if (mvNfcDbgFlag) mvOsPrintf("PDMA write 0x%08x = %08x\n", addr, val);
}

MV_VOID pdma_dbg_bitset(MV_U32 offset, MV_U32 bitMask)
{
	MV_U32 reg1, reg2;

	reg1 = (MV_MEMIO32_READ(offset));
	reg2 = (reg1 | MV_32BIT_LE_FAST(bitMask));
	MV_MEMIO32_WRITE((offset), reg2);
	if (mvNfcDbgFlag) mvOsPrintf("PDMA bitset 0x%08x = %08x (old = %08x)\n", offset, reg2, reg1);
}

MV_VOID pdma_dbg_bitreset(MV_U32 offset, MV_U32 bitMask)
{
	MV_U32 reg1, reg2;

	reg1 = (MV_MEMIO32_READ(offset));
	reg2 = (reg1 & ~MV_32BIT_LE_FAST(bitMask));
	MV_MEMIO32_WRITE((offset), reg2);
	if (mvNfcDbgFlag) mvOsPrintf("PDMA bitreset 0x%08x = %08x (old = %08x)\n", offset, reg2, reg1);
}

#undef MV_REG_READ
#undef MV_REG_WRITE
#undef MV_REG_BIT_SET
#undef MV_REG_BIT_RESET
#define MV_REG_READ(x)		pdma_dbg_read(x)
#define MV_REG_WRITE(x,y)	pdma_dbg_write(x,y)
#define MV_REG_BIT_SET(x,y)	pdma_dbg_bitset(x,y)
#define MV_REG_BIT_RESET(x,y)	pdma_dbg_bitreset(x,y)
#endif

/* globals */
MV_U8 chanAllocTable[MV_PDMA_MAX_CHANNELS_NUM] = {0};
MV_U8 mvPdmaNumChannels;
/* This table contains the appropriate offsets for the channels, */
/* to be used when writing PDMA_REQUEST_CHAN_MAP_REG */ 
MV_U32 chanMapRequestOffsetTable[MV_PDMA_MAX_CHANNELS_NUM] = 
	{0x0120, 0x0124, 0x0128, 0x012C, 0x0130, 0x117C, 
	0x1180, 0x1184, 0x118C, 0x0134, 0x0138, 0, 0, 0, 0, 0};


/*******************************************************************************
* mvPdmaHalInit - Initialize the PDMA engine
*
* DESCRIPTION:
*               This function initializes the PDMA unit. 
* INPUT:
*       pdmaNumChannels - max number of PDMA channels.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM if initialization fails.
*       MV_OK on success.
*******************************************************************************/
MV_STATUS mvPdmaHalInit(MV_U8 pdmaNumChannels)
{
	int chan = 0;

	if ((pdmaNumChannels > MV_PDMA_MAX_CHANNELS_NUM) || 
		(pdmaNumChannels < (MV_PDMA_MEMORY+1)))
		return MV_BAD_PARAM;

	for (chan = 0; chan < MV_PDMA_MAX_CHANNELS_NUM; chan++)
		chanAllocTable[chan] = MV_CHANNEL_FREE;

	mvPdmaNumChannels = pdmaNumChannels;

	for (chan = 0; chan < pdmaNumChannels; chan++) {
		MV_REG_WRITE((MV_PDMA_REGS_BASE + chanMapRequestOffsetTable[chan]), 0);
		/* Note: do not write to PDMA_DESC_ADDR_REG, it changes the state of the channel to initialized */
		MV_REG_WRITE(PDMA_SRC_ADDR_REG(chan), 0);
		MV_REG_WRITE(PDMA_DST_ADDR_REG(chan), 0);
		MV_REG_WRITE(PDMA_COMMAND_REG(chan), DCMD_DEF_VALUE);
		MV_REG_WRITE(PDMA_CTRL_STATUS_REG(chan), DCSR_DEF_VALUE);
	}

	MV_REG_WRITE(PDMA_ALIGNMENT_REG, 0);
	MV_REG_WRITE(PDMA_INTR_CAUSE_REG, 0);
	return MV_OK;
}


/*******************************************************************************
* mvPdmaChanAlloc - Allocate a PDMA channel
*
* DESCRIPTION:
*               This function allocates a PDMA channel for use according to the given parameters. 
* INPUT:
*       chanType - channel type (peripheral type or memory).
*       intrEnMask - bitmask for enabling the PDMA unit interrupts. 
*					 Use the MV_PDMA_<x>_INTR_EN flags to enable various interrupts.
*					 Example: (MV_PDMA_END_INTR_EN | MV_PDMA_START_INTR_EN).
*
* OUTPUT:
*       chanHndl - handle to a PDMA channel, which include channel parameters.
*
* RETURN:
*       MV_NO_RESOURCE or MV_BAD_PARAM on failure.
*       MV_OK on success.
*******************************************************************************/
MV_STATUS mvPdmaChanAlloc(MV_PDMA_PERIPH_TYPE chanType, 
			MV_U32 intrEnMask, 
			MV_PDMA_CHANNEL *chanHndl)
{
	int i = 0;

	if (chanHndl == NULL)
		return MV_BAD_PARAM;

	/* check if channel is free or allocated - use chanType as an index */
	/* for memory-to-memory transactions, we have several channels available */
	if (chanType != MV_PDMA_MEMORY) {
		if (chanAllocTable[chanType] == MV_CHANNEL_FREE)
			chanHndl->chanNumber = chanType;
		else
			return MV_NO_RESOURCE;
	}
	else {
		for (i = MV_PDMA_MEMORY; i < mvPdmaNumChannels; i++) {
			if (chanAllocTable[i] == MV_CHANNEL_FREE) {
				chanHndl->chanNumber = i;
				break;
			}
		}
		if (i == mvPdmaNumChannels) {
			return MV_NO_RESOURCE;
		}
	}
	
	chanHndl->periphType = chanType;

	chanHndl->intrEnMask = intrEnMask;
	
	/* set channel source and destination flow control */
	switch (chanType) {

	/* peripheral is source */
	case MV_PDMA_AC97_MIC:
	case MV_PDMA_AC97_MODEM_RX:
	case MV_PDMA_AC97_AUDIO_RX: 
	case MV_PDMA_SSP_RX:
		chanHndl->srcFlowControl = 1;
		chanHndl->dstFlowCtrl = 0;
		break;
	/* peripheral is target */
	case MV_PDMA_AC97_MODEM_TX: 	
	case MV_PDMA_AC97_AUDIO_TX:
	case MV_PDMA_AC97_SURROUND_TX:
	case MV_PDMA_AC97_CENTER_TX:
	case MV_PDMA_NAND_COMMAND:
	case MV_PDMA_SSP_TX:
		chanHndl->srcFlowControl = 0; 
		chanHndl->dstFlowCtrl = 1;
		break;
	/* the NAND peripheral can be either source or target, */
	/* so flow control will set later according to transaction    */
	case MV_PDMA_NAND_DATA:
		chanHndl->srcFlowControl = 0;
		chanHndl->dstFlowCtrl = 0;	
		break;
	/* memory to memory transactions */
	case MV_PDMA_MEMORY:
		chanHndl->srcFlowControl = 0; 
		chanHndl->dstFlowCtrl = 0;
		break;
	}

	/* set channel maximum burst size and peripheral width */
	switch (chanType) {

	case MV_PDMA_AC97_MIC:
	case MV_PDMA_AC97_MODEM_RX:
	case MV_PDMA_AC97_MODEM_TX: 
	case MV_PDMA_AC97_AUDIO_RX: 
	case MV_PDMA_AC97_AUDIO_TX: 
	case MV_PDMA_AC97_SURROUND_TX:
	case MV_PDMA_AC97_CENTER_TX:
	case MV_PDMA_SSP_RX:
	case MV_PDMA_SSP_TX:
		chanHndl->burstSize = DCMD_BURST_32_BYTES;
		chanHndl->width = DCMD_WIDTH_4_BYTE;
		break;
	case MV_PDMA_NAND_DATA:
		chanHndl->burstSize = DCMD_BURST_32_BYTES;
		chanHndl->width = DCMD_WIDTH_8_BYTE;
		break;
	case MV_PDMA_NAND_COMMAND:
		chanHndl->burstSize = DCMD_BURST_16_BYTES;
		chanHndl->width = DCMD_WIDTH_8_BYTE;
		break;
	case MV_PDMA_MEMORY:
		chanHndl->burstSize = DCMD_BURST_32_BYTES;
		chanHndl->width = 0; /* must be 0 for memory to memory transactions */
		break;
	}
	/* This is required only for peripheral to memory or memory to peripheral channels, */
	/* but not needed for memory to memory channels */
	if (chanType != MV_PDMA_MEMORY) {
		MV_REG_WRITE((MV_PDMA_REGS_BASE + chanMapRequestOffsetTable[chanHndl->chanNumber]), 
				((chanHndl->chanNumber & DRCMR_CHLNUM_MASK) | DRCMR_MAPVLD_BIT));

	}

	/* AC97 channels need to work in 1 byte alignment, */
	/* while other channels need to work in 8 byte alignment. */
	if ((chanType >= MV_PDMA_AC97_MIC) && (chanType <= MV_PDMA_AC97_CENTER_TX)) {
		MV_REG_BIT_SET(PDMA_ALIGNMENT_REG, (1 << chanType)); /* note in this case chanType == chanHndl->chanNumber */
	}

	chanAllocTable[chanHndl->chanNumber] = MV_CHANNEL_TAKEN;

	return MV_OK;
}


/*******************************************************************************
* mvPdmaChanFree - Free a PDMA channel
*
* DESCRIPTION:
*               This function frees a previously allocated PDMA channel. 
* INPUT:
*       chanHndl - handle to a PDMA channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*******************************************************************************/
MV_STATUS mvPdmaChanFree(MV_PDMA_CHANNEL *chanHndl)
{
	if (chanHndl == NULL)
		return MV_BAD_PARAM;

	chanAllocTable[chanHndl->chanNumber] = MV_CHANNEL_FREE;
	
	return MV_OK;
}


/*******************************************************************************
* mvPdmaChanAlignmentSet - Set the alignment of a PDMA channel
*
* DESCRIPTION:
*               This function sets the aligment of an allocated PDMA channel. 
* INPUT:
*       chanHndl - handle to a PDMA channel.
*	alignment - alignment type: default (4 or 8 byte aligned) or user (1 byte aligned).
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*******************************************************************************/
MV_STATUS mvPdmaChanAlignmentSet(MV_PDMA_CHANNEL *chanHndl, MV_PDMA_ALIGNMENT alignment)
{
	if (chanHndl == NULL)
		return MV_BAD_PARAM;

	if (alignment == MV_PDMA_ALIGNMENT_USER)
		MV_REG_BIT_SET(PDMA_ALIGNMENT_REG, (1 << (chanHndl->chanNumber)));
	else if (alignment == MV_PDMA_ALIGNMENT_DEFAULT)
		MV_REG_BIT_RESET(PDMA_ALIGNMENT_REG, (1 << (chanHndl->chanNumber)));
	else
		return MV_BAD_PARAM;

	return MV_OK;
}


/*******************************************************************************
* mvPdmaChanAlignmentGet - Get the alignment of a PDMA channel
*
* DESCRIPTION:
*               This function gets the aligment of an allocated PDMA channel. 
* INPUT:
*       chanHndl - handle to a PDMA channel.
*
* OUTPUT:
*       alignment - alignment type: default (4 or 8 byte aligned) or user (1 byte aligned).
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*******************************************************************************/
MV_STATUS mvPdmaChanAlignmentGet(MV_PDMA_CHANNEL *chanHndl, MV_PDMA_ALIGNMENT *alignment)
{
	MV_U32 reg = 0;

	if (chanHndl == NULL)
		return MV_BAD_PARAM;

	reg = MV_REG_READ(PDMA_ALIGNMENT_REG);
	reg &= (1 << (chanHndl->chanNumber));
	if (reg)
		*alignment = MV_PDMA_ALIGNMENT_USER;
	else
		*alignment = MV_PDMA_ALIGNMENT_DEFAULT;

	return MV_OK;
}


/*******************************************************************************
* mvPdmaChannelStateGet - Return the state of a PDMA channel
*
* DESCRIPTION:
*               This function returns the state of a PDMA channel. 
* INPUT:
*       chanHndl - handle to a PDMA channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*	MV_BAD_PARAM on failure.
*       MV_PDMA_CHANNEL_STOPPED if channel is stopped or un-initialized.
*       MV_PDMA_CHANNEL_RUNNING if channel is running.
*       MV_PDMA_CHANNEL_ERROR if the channel is in an error state.
*******************************************************************************/
MV_PDMA_CHAN_STATE mvPdmaChannelStateGet(MV_PDMA_CHANNEL *chanHndl)
{
	MV_U32 regVal = 0;

#ifdef MV_PDMA_DEBUG
	/* check parameter validity only in debug mode for better performance */
	if (chanHndl == NULL)
		return MV_BAD_PARAM;
#endif /* MV_PDMA_DEBUG */

	regVal = MV_REG_READ(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber));

	if (regVal & DCSR_BUSERRINTR)
		return MV_PDMA_CHANNEL_ERROR;

	if (regVal & DCSR_STOPINTR)
		return MV_PDMA_CHANNEL_STOPPED;

	return MV_PDMA_CHANNEL_RUNNING;
}


/*******************************************************************************
* mvPdmaChanTransfer - Transfer data from source to destination
* 
* DESCRIPTION:       
*       This function performs a DMA transaction using a given channel.
*
*       This function supports both descriptor-fetch and non-descriptor-fetch modes. 
*       To use the function in descriptor-fetch mode just set phyNextDescriptor parameter 
*       with the descriptor physical address. The transfer will be according to the 
*       parameters in the descriptor. Note: in descriptor-fetch mode, the other parameters 
*       (transType, phySrcAddr, phyDstAddr, size) are not used and the PDMA_COMMAND_REG 
*       is set according to the value in the descriptor !
*
*       To use the non-descriptor-fetch mode set phyNextDescriptor to NULL.
*
*       The user is asked to keep the following restrictions:
*       1) This function does not take into consideration CPU MMU issues.
*          In order for the PDMA engine to access the appropreate source 
*          and destination, address parameters must be given as physical addresses.
*       2) This function does not take care of cache coherency issues. The source,
*          destination and in case of chain mode the descriptor list are assumed
*          to be cache coherent.
*       3) In case of descriptor-fetch mode, the function does not align the user descriptor
*          chain. Instead, the user must make sure the descriptor chain is 
*          aligned according to PDMA requirements.
*       4) This function does not check parameters validity (for example, does size 
*          parameter exceed the maximum byte count).
*       5) This function does not check the current state of the channel and assumes it is 
*          not currently running.
*
* INPUT:
*       chanHndl - handle to a PDMA channel.
*       transType - peripheral to memory, memory to peripheral or memory to memory.
*       phySrcAddr - physical source address.
*       phyDstAddr - physical destination address.
*       size - length of transaction in bytes, should not exceed 8KByte.
*       phyNextDescriptor - physical address of the descriptor, or NULL if not required.
*
* OUTPUT:
*       None.
*
* RETURS:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaChanTransfer(MV_PDMA_CHANNEL *chanHndl, 
					MV_PDMA_TRANSACTION_TYPE transType, 
					MV_U32 phySrcAddr, 
					MV_U32 phyDstAddr, 
					MV_U16 size, 
					MV_U32 phyNextDescriptor)
{   
	MV_U32 regVal = 0;

#ifdef MV_PDMA_DEBUG
	/* check parameter validity only in debug mode for better performance */
	if (chanHndl == NULL)
		return MV_BAD_PARAM;

	if ((chanHndl->periphType == MV_PDMA_MEMORY) && 
		(transType != MV_PDMA_MEM_TO_MEM))
		return MV_BAD_PARAM;
	
	if (size > MV_PDMA_MAX_TRANSFER_SIZE)
		return MV_BAD_PARAM;
#endif /* MV_PDMA_DEBUG */

	if (phyNextDescriptor) {
		/* working in descriptor-fetch mode */

		/* clear DCSR_RUN and DCSR_NODESCFETCH bits,			*/
		/* also enable the requested interrupts for this channel	*/
		/* note in this mode, the value of the command register		*/
		/* is determined according to the descriptor, so the		*/
		/* MV_PDMA_END_INTR_EN and MV_PDMA_START_INTR_EN bits in	*/
		/* the intrEnMask are irrelevant.				*/
		regVal = ((chanHndl->intrEnMask		& 
			(MV_PDMA_STOP_INTR_EN		|
			MV_PDMA_REQ_AFTER_STOP_INTR_EN	| 
			MV_PDMA_END_OF_RX_INTR_EN)	& 
			~DCSR_RUN			& 
			~DCSR_NODESCFETCH));

		MV_REG_WRITE(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), regVal);
		/* set the descriptor address */
		MV_REG_WRITE(PDMA_DESC_ADDR_REG(chanHndl->chanNumber), phyNextDescriptor);
		/* set the DCSR_RUN bit to start the operation */
		MV_REG_BIT_SET(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), DCSR_RUN);
	}
	else {
		/* working in non-descriptor-fetch mode */

		/* clear the DCSR_RUN bit and set the DCSR_NODESCFETCH bit */
		regVal = ((chanHndl->intrEnMask		&
			(MV_PDMA_STOP_INTR_EN		|
			MV_PDMA_REQ_AFTER_STOP_INTR_EN	|
			MV_PDMA_END_OF_RX_INTR_EN)	&
			~DCSR_RUN)			|
			DCSR_NODESCFETCH);

		MV_REG_WRITE(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), regVal);
		/* write source and destination addresses */
		MV_REG_WRITE(PDMA_SRC_ADDR_REG(chanHndl->chanNumber), phySrcAddr);
		MV_REG_WRITE(PDMA_DST_ADDR_REG(chanHndl->chanNumber), phyDstAddr);
		/* write command register */
		regVal = (chanHndl->intrEnMask	& 
			(MV_PDMA_END_INTR_EN |
			MV_PDMA_START_INTR_EN));
		if (transType == MV_PDMA_PERIPH_TO_MEM) {
			regVal &= ~(DCMD_INCSRCADDR | DCMD_FLOWTRG);
			regVal |= (DCMD_INCTRGADDR | DCMD_FLOWSRC);
		}
		else if (transType == MV_PDMA_MEM_TO_PERIPH) {
			regVal &= ~(DCMD_INCTRGADDR | DCMD_FLOWSRC);
			regVal |= (DCMD_INCSRCADDR | DCMD_FLOWTRG);
		}
		else {
			regVal &= ~(DCMD_FLOWSRC | DCMD_FLOWTRG);
			regVal |= (DCMD_INCSRCADDR | DCMD_INCTRGADDR);
		}
		regVal |= chanHndl->burstSize;
		regVal |= chanHndl->width;
		regVal |= (size & DCMD_LEN_MASK);
		regVal |= (1 << DCMD_OVERREAD);

		MV_REG_WRITE(PDMA_COMMAND_REG(chanHndl->chanNumber), regVal);

		/* set the DCSR_RUN bit to start the operation */
		MV_REG_BIT_SET(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), DCSR_RUN);
	}
	
	return MV_OK;
}

/*******************************************************************************
* mvPdmaCommandRegCalc - Calculate the value of the command register according 
* 			 to the given parameters.
* 
* DESCRIPTION:       
*       This function calculates the required value for the PDMA command register.
*	This is useful when working in descriptor-fetch (chained) mode, to help the 
*	user build the descriptor chain. In this mode, the command register is 
*	written according to the last field in the descriptor.
*
*       This function does not check parameters validity (for example, does size 
*       parameter exceed the maximum byte count).
*
* INPUT:
*       chanHndl - handle to a PDMA channel.
*       transType - peripheral to memory, memory to peripheral or memory to memory.
*       size - length of transaction in bytes, should not exceed 8KByte.
*
* OUTPUT:
*       None.
*
* RETURS:
*       MV_U32 - the value of the command register according to the parameters.
*
*******************************************************************************/
MV_U32 mvPdmaCommandRegCalc(	MV_PDMA_CHANNEL *chanHndl, 
				MV_PDMA_TRANSACTION_TYPE transType, 
				MV_U16 size)
{
	MV_U32 regVal = 0;

	if (transType == MV_PDMA_PERIPH_TO_MEM) {
		regVal &= ~(DCMD_INCSRCADDR | DCMD_FLOWTRG);
		regVal |= (DCMD_INCTRGADDR | DCMD_FLOWSRC);
	}
	else if (transType == MV_PDMA_MEM_TO_PERIPH) {
		regVal &= ~(DCMD_INCTRGADDR | DCMD_FLOWSRC);
		regVal |= (DCMD_INCSRCADDR | DCMD_FLOWTRG);
	}
	else {
		regVal &= ~(DCMD_FLOWSRC | DCMD_FLOWTRG);
		regVal |= (DCMD_INCSRCADDR | DCMD_INCTRGADDR);
	}
	regVal |= chanHndl->burstSize;
	regVal |= chanHndl->width;
	regVal |= (size & DCMD_LEN_MASK);
	regVal |= (1 << DCMD_OVERREAD);

	return regVal;
}


/*******************************************************************************
* mvPdmaCommandIntrEnable - Enable interrupts for a given PDMA command.
* 
* DESCRIPTION:       
*       This function enables interrupts for a given PDMA command that was
*	previously calculated by mvPdmaCommandRegCalc().
*	The interrupts are enabled according to the intrMask passed to the
*	mvPdmaChanAlloc() API.
*
* INPUT:
*       chanHndl - handle to a PDMA channel.
*       command - The command to enable the interrupts for.
*
* OUTPUT:
*       command - The input command including the proper interrupt bits.
*
* RETURS:
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaCommandIntrEnable( MV_PDMA_CHANNEL *chanHndl,
				MV_U32 *command)
{
	*command |= (chanHndl->intrEnMask & (MV_PDMA_END_INTR_EN | MV_PDMA_START_INTR_EN));
	return MV_OK;
}


/*******************************************************************************
* mvPdmaMemInit - Initialize a memory buffer with a given value pattern
* 
* DESCRIPTION:       
*       This function initializes a memory buffer with a user supplied pattern.
*       The user is asked to keep the following restrictions:
*       1) This function does not take into consideration CPU MMU issues.
*          In order for the PDMA engine to access the appropreate source 
*          and destination, address parameters must be given as physical addresses.
*       2) This function does not take care of cache coherency issues. The source,
*          destination and in case of chain mode the descriptor list are assumed
*          to be cache coherent.
*       3) No chain mode support.
*       4) This function does not check parameters validity (for example, does size 
*          parameter exceed the maximum byte count.
*
* INPUT:
*       chanHndl - handle to a PDMA channel.
*       phyPatternPtr - physical source address of the pattern.
*       phyDstAddr    - physical destination address.
*       size          - The total number of bytes to initialize.
*
* OUTPUT:
*       None.
*
* RETURS:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaMemInit(MV_PDMA_CHANNEL *chanHndl, 
			MV_U32 phyPatternPtr, 
			MV_U32 phyDstAddr, 
			MV_U16 size)
{   
	MV_U32 regVal = 0;
#ifdef MV_PDMA_DEBUG
	/* check parameter validity only in debug mode for better performance */
	if (chanHndl == NULL)
		return MV_BAD_PARAM;

	if (chanHndl->periphType != MV_PDMA_MEMORY)
		return MV_BAD_PARAM;
	
	if ((size > MV_PDMA_MAX_TRANSFER_SIZE) || (size == 0))
		return MV_BAD_PARAM;
#endif /* MV_PDMA_DEBUG */

	/* working in non-descriptor-fetch mode */

	/* clear the DCSR_RUN bit and set the DCSR_NODESCFETCH bit	*/
	regVal = ((chanHndl->intrEnMask		& 
		(MV_PDMA_STOP_INTR_EN		|
		MV_PDMA_REQ_AFTER_STOP_INTR_EN	| 
		MV_PDMA_END_OF_RX_INTR_EN)	& 
		~DCSR_RUN)			|
		DCSR_NODESCFETCH);

	MV_REG_WRITE(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), regVal);
	/* write source and destination addresses */
	MV_REG_WRITE(PDMA_SRC_ADDR_REG(chanHndl->chanNumber), phyPatternPtr);
	MV_REG_WRITE(PDMA_DST_ADDR_REG(chanHndl->chanNumber), phyDstAddr);
	/* write command register */
	regVal = (chanHndl->intrEnMask	& 
		(MV_PDMA_END_INTR_EN	|
		MV_PDMA_START_INTR_EN));

	regVal &= ~(DCMD_FLOWSRC | DCMD_FLOWTRG | DCMD_INCSRCADDR);
	regVal |= DCMD_INCTRGADDR;
		
	regVal |= chanHndl->burstSize;
	regVal |= chanHndl->width;
	regVal |= (size & DCMD_LEN_MASK);

	/* set the DCSR_RUN bit to start the operation */
	MV_REG_BIT_SET(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), DCSR_RUN);

	return MV_OK;
}

/*******************************************************************************
* mvPdmaUnitStateStore - Store the PDMA Unit state.
* 
* DESCRIPTION:       
*       This function stores the PDMA unit registers before the unit is suspended.
*	The stored registers are placed into the input buffer which will be used for
*	the restore operation.
*
* INPUT:
*       regsData	- Buffer to store the unit state registers (Must
*			  include at least 64 entries)
*	len		- Number of entries in regsData input buffer.
*
* OUTPUT:
*       regsData	- Unit state registers. The registers are stored in
*			  pairs of (reg, value).
*       len		- Number of entries in regsData buffer (Must be even).
*
* RETURS:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaUnitStateStore(MV_U32 *stateData, MV_U32 *len)
{
	MV_U32 i;

	if((stateData == NULL) || (len == NULL))
		return MV_BAD_PARAM;

	for(i = 0; i < sizeof(chanMapRequestOffsetTable) / sizeof(MV_U32); i++) {
		stateData[i * 2] = MV_PDMA_REGS_BASE + chanMapRequestOffsetTable[i];
		stateData[i * 2 + 1] = MV_REG_READ(MV_PDMA_REGS_BASE + chanMapRequestOffsetTable[i]);
	}

	stateData[i * 2] = PDMA_ALIGNMENT_REG;
	stateData[i * 2 + 1] = MV_REG_READ(PDMA_ALIGNMENT_REG);
	i++;

	*len = i;
	return MV_OK;
}

#ifdef MV_PDMA_DEBUG
/************************************ For Debug *******************************/

/*******************************************************************************
* mvPdmaSrcAddrRegSet - Write to PDMA Channel Source Address Register (DSADRx)
*
* DESCRIPTION:
*               This function simply writes a given value to the 
*		PDMA Channel Source Address Register (DSADRx), where x is the channel number. 
* INPUT:
*       chan - channel number (0 to 15).
*       value - value to write to the register.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaSrcAddrRegSet (MV_U8 chan, MV_U32 value)
{
	if (chan > mvPdmaNumChannels)
		return MV_BAD_PARAM;

	MV_REG_WRITE(PDMA_SRC_ADDR_REG(chan), value);
	return MV_OK;
}

/*******************************************************************************
* mvPdmaDstAddrRegSet - Write to PDMA Channel Destination Address Register (DTADRx)
*
* DESCRIPTION:
*               This function simply writes a given value to the 
*		PDMA Channel Destination Address Register (DTADRx), where x is the channel number. 
* INPUT:
*       chan - channel number (0 to 15).
*       value - value to write to the register.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaDstAddrRegSet(MV_U8 chan, MV_U32 value)
{
	if (chan > mvPdmaNumChannels)
		return MV_BAD_PARAM;

	MV_REG_WRITE(PDMA_DST_ADDR_REG(chan), value);
	return MV_OK;
}

/*******************************************************************************
* mvPdmaCmdRegSet - Write to PDMA Channel Command Register (DCMDx)
*
* DESCRIPTION:
*               This function simply writes a given value to the 
*		PDMA Channel Command Register (DCMDx), where x is the channel number. 
* INPUT:
*       chan - channel number (0 to 15).
*       value - value to write to the register.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaCmdRegSet(MV_U8 chan, MV_U32 value)
{
	if (chan > mvPdmaNumChannels)
		return MV_BAD_PARAM;

	MV_REG_WRITE(PDMA_COMMAND_REG(chan), value);
	return MV_OK;
}

/*******************************************************************************
* mvPdmaCntrlRegSet - Write to PDMA Channel Control/Status Register (DCSRx)
*
* DESCRIPTION:
*               This function simply writes a given value to the 
*		PDMA Channel Control/Status Register (DCSRx), where x is the channel number. 
* INPUT:
*       chan - channel number (0 to 15).
*       value - value to write to the register.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_BAD_PARAM on failure.
*       MV_OK on success.
*
*******************************************************************************/
MV_STATUS mvPdmaCntrlRegSet(MV_U8 chan, MV_U32 value)
{
	if (chan > mvPdmaNumChannels)
		return MV_BAD_PARAM;

	MV_REG_WRITE(PDMA_CTRL_STATUS_REG(chan), value);
	return MV_OK;
}

#endif /* MV_PDMA_DEBUG */

/*******************************************************************************
* mvPdmaChanRegsPrint - Print the registers of a PDMA channel
*
* DESCRIPTION:
*               This function prints all the registers of a given PDMA channel. 
* INPUT:
*       chanHndl - handle to a PDMA channel.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*******************************************************************************/
MV_VOID mvPdmaChanRegsPrint(MV_PDMA_CHANNEL *chanHndl)
{
	if (chanHndl == NULL)
		return;

	mvOsPrintf("\t PDMA Channel #%d Registers:\n", chanHndl->chanNumber);

	mvOsPrintf("PDMA_REQUEST_CHAN_MAP_REG             : 0x%X = 0x%08x\n", 
                (MV_PDMA_REGS_BASE + chanMapRequestOffsetTable[chanHndl->chanNumber]), 
                MV_REG_READ((MV_PDMA_REGS_BASE + chanMapRequestOffsetTable[chanHndl->chanNumber])));

	mvOsPrintf("PDMA_DESC_ADDR_REG               : 0x%X = 0x%08x\n", 
                PDMA_DESC_ADDR_REG(chanHndl->chanNumber), 
                MV_REG_READ(PDMA_DESC_ADDR_REG(chanHndl->chanNumber)));

	mvOsPrintf("PDMA_SRC_ADDR_REG               : 0x%X = 0x%08x\n", 
                PDMA_SRC_ADDR_REG(chanHndl->chanNumber), 
                MV_REG_READ(PDMA_SRC_ADDR_REG(chanHndl->chanNumber)));

	mvOsPrintf("PDMA_DST_ADDR_REG               : 0x%X = 0x%08x\n", 
                PDMA_DST_ADDR_REG(chanHndl->chanNumber), 
                MV_REG_READ(PDMA_DST_ADDR_REG(chanHndl->chanNumber)));

	mvOsPrintf("PDMA_COMMAND_REG               : 0x%X = 0x%08x\n", 
                PDMA_COMMAND_REG(chanHndl->chanNumber), 
                MV_REG_READ(PDMA_COMMAND_REG(chanHndl->chanNumber)));

	mvOsPrintf("PDMA_CTRL_STATUS_REG               : 0x%X = 0x%08x\n", 
                PDMA_CTRL_STATUS_REG(chanHndl->chanNumber), 
                MV_REG_READ(PDMA_CTRL_STATUS_REG(chanHndl->chanNumber)));
}

