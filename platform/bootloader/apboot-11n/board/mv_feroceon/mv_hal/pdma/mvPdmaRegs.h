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

#ifndef __INCmvPdmaSpech
#define __INCmvPdmaSpech

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "mvSysPdmaConfig.h"

#define PDMA_CHAN_MAP_REG_BASE			(MV_PDMA_REGS_BASE + 0x100)
#define PDMA_DESC_REG_BASE			(MV_PDMA_REGS_BASE + 0x200)
/* PDMA channel registers */
#define PDMA_CTRL_STATUS_REG(chan)		(MV_PDMA_REGS_BASE + ((chan) * 0x4))
#define PDMA_REQUEST_CHAN_MAP_REG(chan)		(PDMA_CHAN_MAP_REG_BASE + ((chan) * 0x4))
#define PDMA_DESC_ADDR_REG(chan)		(PDMA_DESC_REG_BASE + ((chan) * 0x10))
#define PDMA_SRC_ADDR_REG(chan)			(PDMA_DESC_REG_BASE + 0x4 + ((chan) * 0x10))
#define PDMA_DST_ADDR_REG(chan)			(PDMA_DESC_REG_BASE + 0x8 + ((chan) * 0x10))
#define PDMA_COMMAND_REG(chan)			(PDMA_DESC_REG_BASE + 0xC + ((chan) * 0x10))

/* PDMA Address Decoding Base and Control Registers */ 
#define PDMA_WINDOW_BASE_REG(winNum)		(MV_PDMA_REGS_BASE + 0x4000 + ((winNum) << 3))
#define PDMA_WINDOW_CONTROL_REG(winNum)		(MV_PDMA_REGS_BASE + 0x4004 + ((winNum) << 3))

/* PDMA Alignment Register */
#define PDMA_ALIGNMENT_REG			(MV_PDMA_REGS_BASE + 0xA0)

/* PDMA Interrupt Register */
#define PDMA_INTR_CAUSE_REG			(MV_PDMA_REGS_BASE + 0xF0)

/* PDMA register fields */

/* PDMA Request to Channel Map Register (DRCMRx) */
#define DRCMR_CHLNUM_OFFS			0	/* Valid Channel Number Offset */
#define DRCMR_CHLNUM_MASK			(0x1F << DRCMR_CHLNUM_OFFS)	/* Valid Channel Number */
#define DRCMR_MAPVLD_BIT			BIT7	/* Map Valid Channel */

/* PDMA Descriptor Address Register (DDADRx) */
#define DDADR_STOP				BIT0	/* Stop channel after processing this descriptor */
#define DDADR_BREN				BIT1	/* Enable descriptor branch */
#define DDADR_DESC_ADDR_OFFSET			4	/* Descriptor address */
#define DDADR_DESC_ADDR_MASK			(0x0FFFFFFF << DDADR_DESC_ADDR_OFFSET)

/* PDMA Command Register (DCMDx) */
#define DCMD_SIZE_8_BYTES			1
#define DCMD_SIZE_16_BYTES			2
#define DCMD_SIZE_32_BYTES			3

#define DCMD_W_1_BYTE				1
#define DCMD_W_2_BYTE				2
#define DCMD_W_4_BYTE				3
#define DCMD_W_8_BYTE				0

#define DCMD_LEN_OFFS				0	/* Length of the transfer in bytes */
#define DCMD_LEN_MASK				(0x1FFF << DCMD_LEN_OFFS)
#define DCMD_OVERREAD				13	/* Over-read bit */
#define DCMD_WIDTH_OFFS				14	/* Width of the on-chip peripheral */
#define DCMD_WIDTH_MASK				(0x3 << DCMD_WIDTH_OFFS)
#define DCMD_WIDTH_1_BYTE			(DCMD_W_1_BYTE << DCMD_WIDTH_OFFS)
#define DCMD_WIDTH_2_BYTE			(DCMD_W_2_BYTE << DCMD_WIDTH_OFFS)
#define DCMD_WIDTH_4_BYTE			(DCMD_W_4_BYTE << DCMD_WIDTH_OFFS)
#define DCMD_WIDTH_8_BYTE			(DCMD_W_8_BYTE << DCMD_WIDTH_OFFS)
#define DCMD_BURST_OFFS				16	/* Maximum burst size of each data transfer */
#define DCMD_BURST_MASK				(0x3 << DCMD_BURST_OFFS)
#define DCMD_BURST_8_BYTES			(DCMD_SIZE_8_BYTES << DCMD_BURST_OFFS)
#define DCMD_BURST_16_BYTES			(DCMD_SIZE_16_BYTES << DCMD_BURST_OFFS)
#define DCMD_BURST_32_BYTES			(DCMD_SIZE_32_BYTES << DCMD_BURST_OFFS)
#define DCMD_ENDIRQEN				BIT21	/* End interrupt enable */
#define DCMD_STARTIRQEN				BIT22	/* Start interrupt enable */
#define DCMD_ADDRMODE				BIT23	/* Addressing mode */
#define DCMD_CMPEN				BIT25	/* Descriptor compare enable */
#define DCMD_FLOWTRG				BIT28	/* Destination flow control */
#define DCMD_FLOWSRC				BIT29	/* Source flow control */
#define DCMD_INCTRGADDR				BIT30	/* Destination address increment */
#define DCMD_INCSRCADDR				BIT31	/* Source address increment */

/* 32 bytes burst, 4 bytes width */
#define DCMD_DEF_VALUE				(DCMD_BURST_32_BYTES | DCMD_WIDTH_4_BYTE)

/* PDMA Channel Control/Status Register (DCSRx) */
#define DCSR_BUSERRINTR				BIT0	/* Bus error interrupt */
#define DCSR_STARTINTR				BIT1	/* Start interrupt */
#define DCSR_ENDINTR				BIT2	/* End interrupt */
#define DCSR_STOPINTR				BIT3	/* Stop interrupt */
#define DCSR_RASINTR				BIT4	/* Request after channel stopped interrupt */
#define DCSR_REQPEND				BIT8	/* Request pending */
#define DCSR_EORINT				BIT9	/* End of receive interrupt */
#define DCSR_CMPST				BIT10	/* Descriptor compare status */
#define DCSR_MASKRUN				BIT22	/* Mask DCSR[RUN] during a programmed IO write to DCSR */
#define DCSR_RASIRQEN				BIT23	/* Request after channel stopped interrupt enable */
#define DCSR_EORSTOPEN				BIT26	/* Stop channel on end of receive */
#define DCSR_EORJMPEN				BIT27	/* Jump to the next descriptor on end of receive */
#define DCSR_EORIRQEN				BIT28	/* End of receive interrupt enable */
#define DCSR_STOPIRQEN				BIT29	/* Stop interrupt enable */
#define DCSR_NODESCFETCH			BIT30	/* No descriptor fetch transfer */
#define DCSR_RUN				BIT31	/* Run - start the channel */

/* write '1' to clear interrupts */
#define DCSR_DEF_VALUE				DCSR_EORINT	| \
						DCSR_RASINTR	| \
						DCSR_STOPINTR	| \
						DCSR_ENDINTR	| \
						DCSR_STARTINTR	| \
						DCSR_BUSERRINTR

/* General PDMA */
#define PDMA_MAX_ADDR_DEC_WIN		4	/* Maximum address decode windows */

 
/* PDMA Window Base Register */
#define PDMA_WIN_BASE_OFFS		16	/* Base address */
#define PDMA_WIN_BASE_MASK		(0xFFFF << PDMA_WIN_BASE_OFFS)

/* PDMA Window Control Register */
#define PDMA_WIN_EN			BIT0	/* Window enable */
#define PDMA_WIN_WRBL			BIT1	/* Write to crossbar burst limit */	
#define PDMA_WIN_TARGET_OFFS		4	/* The target interface associated with window */		
#define PDMA_WIN_TARGET_MASK		(0xF << PDMA_WIN_TARGET_OFFS)
#define PDMA_WIN_ATTR_OFFS		8	/* The target attributes associated with window */
#define PDMA_WIN_ATTR_MASK		(0xFF << PDMA_WIN_ATTR_OFFS)
#define PDMA_WIN_SIZE_OFFS		16	/* Window size */
#define PDMA_WIN_SIZE_MASK		(0xFFFF << PDMA_WIN_SIZE_OFFS)

#define PDMA_WIN_SIZE_GRANULARITY	0x10000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __INCmvPdmaSpech */

