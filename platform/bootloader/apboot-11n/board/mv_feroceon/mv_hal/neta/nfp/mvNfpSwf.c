/*******************************************************************************
Copyright (C) Marvell Interfdbional Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
Interfdbional Ltd. and/or its affiliates ("Marvell") under the following
alterfdbive licensing terms.  Once you have made an election to distribute the
File under one of the following license alterfdbives, please (i) delete this
introductory statement regarding license alterfdbives, (ii) delete the two
license alterfdbives that you have not elected to use and (iii) preserve the
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
* mvNfpSwf.c - Marvell Fast Network Processing
*
* DESCRIPTION:
*
*       Supported Features:
*       - OS independent.
*
*******************************************************************************/

/* includes */
#include "mvOs.h"
#include "mvDebug.h"
#include "gbe/mvNeta.h"
#include "mvNfp.h"

#if 1
#define NFP_DBG mvOsPrintf
#else
#define NFP_DBG
#endif

#ifdef NFP_STAT
#define NFP_INC(p, s) nfp_stats[p].s++;
#else
#define NFP_INC(p, s)
#endif

#define	NFP_SWF_HASH_BITS 5
#define	NFP_SWF_HASH_SIZE (1<<NFP_SWF_HASH_BITS)
#define	NFP_SWF_HASH_MASK (NFP_SWF_HASH_SIZE-1)

typedef struct nfp_swf_stats {
	MV_U32 swf;
	MV_U32 swf_miss;
} NFP_SWF_STATS;

static NFP_RULE_SWF **nfp_swf_hash;
static NFP_SWF_STATS nfp_stats[MV_ETH_MAX_PORTS];

MV_STATUS _INIT mvNfpSwfInit(MV_VOID)
{
	MV_U32 bytes = sizeof(NFP_RULE_SWF *) * NFP_SWF_HASH_SIZE;

	nfp_swf_hash = (NFP_RULE_SWF **) mvOsMalloc(bytes);
	if (nfp_swf_hash == NULL) {
		mvOsPrintf("NFP (swf): not enough memory\n");
		return MV_NO_RESOURCE;
	}

	mvOsMemset(nfp_swf_hash, 0, bytes);
	mvOsPrintf("NFP (swf) init %d entries, %d bytes\n", NFP_SWF_HASH_SIZE, bytes);

	return MV_OK;
}

static INLINE MV_U32 mvNfpSwfRuleHash(MV_U32 flowid)
{
	return mv_jhash_3words(flowid, 0, 0);
}

static INLINE NFP_RULE_SWF *mvNfpSwfLookup(MV_U32 flowid)
{
	MV_U32 hash;
	NFP_RULE_SWF *rule;

	hash = mvNfpSwfRuleHash(flowid);
	hash &= NFP_SWF_HASH_MASK;

	rule = nfp_swf_hash[hash];

	while (rule) {
		if (rule->flowid == flowid)
			return rule;
		rule = rule->next;
	}

	return NULL;
}

MV_STATUS mvNfpSwfRuleAdd(NFP_RULE_SWF *rule2)
{
	MV_U32 hash;
	NFP_RULE_SWF *rule;

	hash = mvNfpSwfRuleHash(rule2->flowid);
	hash &= NFP_SWF_HASH_MASK;

	rule = nfp_swf_hash[hash];

	while (rule) {
		if (rule->flowid == rule2->flowid)
			goto out;

		rule = rule->next;
	}

	rule = (NFP_RULE_SWF *) mvOsMalloc(sizeof(NFP_RULE_SWF));

	if (!rule) {
		mvOsPrintf("%s: OOM\n", __func__);
		return MV_FAIL;
	}

	rule->next = nfp_swf_hash[hash];
	nfp_swf_hash[hash] = rule;
out:
	/*
	 * Port mapping:
	 * 0 - cpu, 1 - gbe0, 2 - gbe1
	 * 3..10 - tcont 0..7
	 */
	if (rule2->txp <= 2) {
		rule->port = rule2->txp - 1;
		rule->txp = 0;
	} else {
		rule->port = 2;
		rule->txp = rule2->txp - 3;
	}
	rule->flowid = rule2->flowid;
	rule->txq = rule2->txq;
	rule->mh_sel = rule2->mh_sel;

#ifdef NFP_DBG
	mvOsPrintf("NFP (swf) set flowid=%x port=%d txp=%x txq=%x mh=%x\n",
		   rule->flowid, rule->port, rule->txp, rule->txq, rule->mh_sel);
#endif
	return MV_OK;
}

MV_STATUS mvNfpSwfRuleDel(NFP_RULE_SWF *rule2)
{
	MV_U32 hash;
	NFP_RULE_SWF *rule, *prev;

	hash = mvNfpSwfRuleHash(rule2->flowid);
	hash &= NFP_SWF_HASH_MASK;

	rule = nfp_swf_hash[hash];
	prev = NULL;

	while (rule) {
		if (rule->flowid == rule2->flowid) {

			if (prev)
				prev->next = rule->next;
			else
				nfp_swf_hash[hash] = rule->next;
#ifdef NFP_DBG
			mvOsPrintf("NFP (swf) del flowid=%x\n", rule->flowid);
#endif
			mvOsFree(rule);

			return MV_OK;
		}

		prev = rule;
		rule = rule->next;
	}

	return MV_NOT_FOUND;
}

MV_VOID mvNfpSwfDump(MV_VOID)
{
	MV_U32 i;
	NFP_RULE_SWF *rule;

	mvOsPrintf("(swf)\n");
	for (i = 0; i < NFP_SWF_HASH_SIZE; i++) {
		rule = nfp_swf_hash[i];

		while (rule) {
			mvOsPrintf("NFP (swf) [%2d] flowid=0x%x port=%d txp=%d txq=%d mh=%x\n",
				   i, rule->flowid, rule->port, rule->txp, rule->txq, rule->mh_sel);
			rule = rule->next;
		}
	}
}

MV_STATUS mvNfpSwf(MV_U32 rxPort, NETA_RX_DESC * rxDesc, MV_ETH_PKT * pPkt)
{
	NFP_RULE_SWF *rule;

	rule = mvNfpSwfLookup(rxDesc->pncFlowId);

	if (!rule) {
		NFP_INC(rxPort, swf_miss);
		return MV_NOT_FOUND;
	}

	NFP_INC(rxPort, swf);

	pPkt->hw_cmd = rxDesc->hw_cmd & (NETA_RX_GEM_PID_MASK | NETA_RX_COLOR_MASK);
	pPkt->hw_cmd |= ((rxDesc->hw_cmd & NETA_RX_DSA_MASK) >> NETA_RX_DSA_OFFS) << NETA_TX_DSA_OFFS;
	pPkt->hw_cmd |= rxDesc->pncFlowId << NETA_TX_MOD_CMD_OFFS;
	pPkt->hw_cmd |= rule->mh_sel << NETA_TX_MH_SEL_OFFS;

	pPkt->tx_cmd = NETA_TX_L4_CSUM_NOT | NETA_TX_PKT_OFFSET_MASK(pPkt->offset);

	/* Process 2B of MH */
	if (rule->mh_sel == (NETA_TX_MH_UNCHANGE >> NETA_TX_MH_SEL_OFFS)) {
		MV_U16 *pMh = (MV_U16 *) (pPkt->pBuf + pPkt->offset);

/*        *pMh = mvNfpPortTxMhGet(rule->port); - To be FIXED */
		mvOsCacheLineFlushInv(NULL, pMh);
	}
	pPkt->port = rule->port;
	pPkt->txp = rule->txp;
	pPkt->txq = rule->txq;

	return MV_OK;
}

MV_U32 mvNfpSwfStats(MV_U8 *buf)
{
	MV_U32 port, off = 0;

	off += mvOsSPrintf(buf + off, "swf_hits....");
	for (port = 0; port < MV_ETH_MAX_PORTS; port++)
		off += mvOsSPrintf(buf + off, "%4u ", nfp_stats[port].swf);
	off += mvOsSPrintf(buf + off, "\n");

	off += mvOsSPrintf(buf + off, "swf_miss....");
	for (port = 0; port < MV_ETH_MAX_PORTS; port++)
		off += mvOsSPrintf(buf + off, "%4u ", nfp_stats[port].swf_miss);
	off += mvOsSPrintf(buf + off, "\n");

	mvOsMemset(&nfp_stats[port], 0, sizeof(NFP_SWF_STATS));

	return off;
}
