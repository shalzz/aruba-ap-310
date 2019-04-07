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
* mvNfpPnc.c - Marvell Fast Network Processing
*
* DESCRIPTION:
*
*       Supported Features:
*       - OS independent.
*
*******************************************************************************/

#include "mvOs.h"
#include "mvDebug.h"
#include "mvList.h"
#include "gbe/mvNeta.h"
#include "pnc/mvTcam.h"
#include "pnc/mvPnc.h"
#include "mvNfp.h"

#ifdef NFP_NAT
static MV_LIST	*nat_pnc_inv_list = NULL;
static MV_STATUS mvNfpPncNatFibAdd(NFP_RULE_NAT *nat, NFP_RULE_FIB *fib);
#endif /* NFP_NAT */

NFP_RULE_PNC	*pnc_hash = NULL;
MV_STACK		*pnc_ipv6_ainfo_stack = NULL;

MV_STATUS _INIT mvNfpPncInit(MV_VOID)
{
	int		i;
	MV_U32	bytes = sizeof(NFP_RULE_PNC) * NFP_PNC_ARRAY_SIZE;

	pnc_hash = (NFP_RULE_PNC *) mvOsMalloc(bytes);
	if (pnc_hash == NULL) {
		mvOsPrintf("NFP (pnc): not enough memory\n");
		return MV_NO_RESOURCE;
	}

	mvOsMemset(pnc_hash, 0, bytes);
	pnc_ipv6_ainfo_stack = mvStackCreate(AI_MASK);
	if (pnc_ipv6_ainfo_stack == NULL) {
		mvOsPrintf("NFP (pnc): Can't create mvStack for IPv6 ainfo (%d)\n", AI_MASK);
		return MV_NO_RESOURCE;
	}
	for (i = AI_MASK; i > 0; i--)
		mvStackPush(pnc_ipv6_ainfo_stack, i);

	mvOsPrintf("NFP (pnc) init %d entries, %d bytes\n", NFP_PNC_ARRAY_SIZE, bytes);

	return MV_OK;
}

static MV_VOID mvNfpPncRulePrint(NFP_RULE_PNC *rule)
{
	mvOsPrintf("tid=%3d, flow=%x, ainfo=%d, fib=%p, nat=%p, flags=0x%x, age=%x",
				rule->tid, rule->flowid, rule->ainfo, rule->fib, rule->nat, rule->flags, rule->age);

	if (rule->fib) {
		if (rule->fib->family == MV_INET)
			mvOsPrintf(" (IPv4)");
		else
			mvOsPrintf(" (IPv6)");
	}
	mvOsPrintf("\n");

	if (rule->pmtno > 0)
		mvOsPrintf("HWF: pmtNo=%d, pmtIdx=%d\n", rule->pmtno,
				((rule->flowid & PNC_FLOWID_HWF_MOD_MASK) >> PNC_FLOWID_HWF_MOD_OFFS));

	mvOsPrintf("Flags: ");
	if (rule->flags & NFP_F_BUSY)
		mvOsPrintf("NFP_F_BUSY ");
	if (rule->flags & NFP_F_PARTIAL)
		mvOsPrintf("NFP_F_PARTIAL ");
	mvOsPrintf("\n");
}

MV_VOID mvNfpPncDump(MV_VOID)
{
	MV_U32 i;
	NFP_RULE_PNC *rule;

	mvOsPrintf("(pnc)\n");

	for (i = 0; i < NFP_PNC_ARRAY_SIZE; i++) {
		rule = pnc_hash + i;

		if (rule->flags) {
			mvOsPrintf(" [%3d] (%p): ", i, rule);
			mvNfpPncRulePrint(rule);
		}
	}
	if (pnc_ipv6_ainfo_stack)
		mvStackStatus(pnc_ipv6_ainfo_stack, 0);
}

/* Returns the first available entry in the PnC array */
/* Returns NFP_PNC_ARRAY_SIZE if no entry was found */
static MV_U32 mvNfpPncGetFirstFree(MV_VOID)
{
	MV_U32 i;

	for (i = 0; i < NFP_PNC_ARRAY_SIZE; i++) {
		if (!pnc_hash[i].flags)
			break;
	}

	return i;
}

/* Returns the last available entry in the PnC array */
/* Returns NFP_PNC_ARRAY_SIZE if no entry was found */
static MV_U32 mvNfpPncGetLastFree(MV_VOID)
{
	MV_U32 i;

	i = NFP_PNC_ARRAY_SIZE;
	do {
		i--;
		if (!pnc_hash[i].flags)
			break;
	} while (i > 0);

	if ((i == 0) && (pnc_hash[i].flags))
		return NFP_PNC_ARRAY_SIZE;

	return i;
}

MV_STATUS mvNfpPncFibAdd(NFP_RULE_FIB *fib)
{
	MV_U32			i, j, rxq;
	NFP_RULE_PNC	*pnc = NULL;

	int family = fib->family;
	if (family == MV_INET) {
		MV_U32 fibSip32 = *((u32 *) fib->srcL3);
		MV_U32 fibDip32 = *((u32 *) fib->dstL3);
		i = mvNfpPncGetLastFree();
		if (i < NFP_PNC_ARRAY_SIZE) {
			pnc = pnc_hash + i;
			pnc->flags = NFP_F_BUSY;
			pnc->fib = fib;
			pnc->flowid = i;
			pnc->age = 0;
			fib->pnc = pnc;
			fib->ref++;
			rxq = CONFIG_MV_ETH_RXQ_DEF;

#ifdef CONFIG_MV_ETH_NFP_HWF
			if (fib->flags & NFP_F_HWF) {
				int pmtIdx = mvNfpPmtAlloc(fib->outport, 8);

				if (pmtIdx != -1) {
					/* Add PMT commands for this flow */
					mvNfpPmtFibUpdate(pmtIdx, fib);

					/* FlowID for HWF entry: mod_cmd=i, gem_port-ignore, outport, txp, txq */
					pnc->pmtno = 8;
					pnc->flowid = (((1 + fib->outport + fib->hwf_txp) << PNC_FLOWID_HWF_TXP_OFFS) |
							(pmtIdx << PNC_FLOWID_HWF_MOD_OFFS));
					rxq = fib->hwf_txq;
					mvOsPrintf("HWF tid=%d, pmtid=%d: flowid=0x%08x, port=%d, txp=%d, txq=%d\n",
						i, pmtIdx, pnc->flowid, fib->outport, fib->hwf_txp, fib->hwf_txq);
				}
			}
#endif /* CONFIG_MV_ETH_NFP_HWF */

			/* Set 2 tuples rule */
			pnc->tid = TE_FLOW_NFP + i;
			pnc_ipv4_2_tuples_add(pnc->tid, pnc->flowid, fibSip32, fibDip32, rxq);
			/*mvOsPrintf("%s: tid=%d\n", __func__, pnc->tid);*/
		}
	} else {
		/* IPv6 entry require 2 PNC entries and available unique number for AInfo */
		if (mvStackIndex(pnc_ipv6_ainfo_stack) == 0) {
			mvOsPrintf("%s for IPv6: no more unique ainfo values\n", __func__);
			return MV_NO_RESOURCE;
		}
		/* Find PNC entry - major */
		i = mvNfpPncGetLastFree();
		if (i == NFP_PNC_ARRAY_SIZE) {
			mvOsPrintf("%s for IPv6: no more PNC entries (major)\n", __func__);
			return MV_NO_RESOURCE;
		}
		pnc = pnc_hash + i;
		pnc->flags = NFP_F_BUSY;

		j = mvNfpPncGetLastFree();
		if (i == NFP_PNC_ARRAY_SIZE) {
			pnc->flags = 0;

			mvOsPrintf("%s for IPv6: no more PNC entries (partial)\n", __func__);
			return MV_NO_RESOURCE;
		}
		pnc->fib = fib;
		pnc->flowid = i;
		pnc->age = 0;
		pnc->tid = TE_FLOW_NFP + i;
		pnc->ainfo = (MV_U8)mvStackPop(pnc_ipv6_ainfo_stack);
		pnc->next = pnc_hash + j;
		pnc->next->flags = NFP_F_BUSY | NFP_F_PARTIAL;
		pnc->next->tid = TE_FLOW_NFP + j;
		pnc->next->next = NULL;

		rxq = 0;
		fib->pnc = pnc;
		fib->ref++;
		pnc_ipv6_2_tuples_add(pnc->next->tid, pnc->tid,
				  pnc->flowid, pnc->ainfo, fib->srcL3, fib->dstL3, rxq);
	}

#ifdef NFP_NAT
	/* Try to complete incomplete NAT rule */
	{
		MV_LIST			*list = nat_pnc_inv_list;
		NFP_RULE_NAT	*nat;
		const MV_U8		*natDip;
		const MV_U8		*natSip;

		while (list) {
			nat = (NFP_RULE_NAT *) list->data;
			natSip = (const MV_U8 *)&nat->sip;
			if (nat->flags & NFP_F_DNAT)
				natDip = (const MV_U8 *)&nat->nip;
			else
				natDip = (const MV_U8 *)&nat->dip;

			if ((l3_addr_eq(family, fib->srcL3, natSip)) && (l3_addr_eq(family, fib->dstL3, natDip))) {
				/* complete PNC entry */
				mvNfpPncNatFibAdd(nat, fib);
				list = mvListDel(&list);
			} else
				list = list->next;
		}
	}
#endif /* NFP_NAT */

	return MV_OK;
}


MV_STATUS mvNfpPncFibDel(NFP_RULE_FIB *fib)
{
	NFP_RULE_PNC *temp, *pnc = fib->pnc;

	if (pnc == NULL)
		return MV_OK;

	if (pnc->fib != fib) {
		mvOsPrintf("NFP (pnc) bug pnc=%p pnc->fib=%p fib=%p\n", pnc, pnc->fib, fib);
		return MV_FAIL;
	}
	while (pnc) {
		pnc_te_del(pnc->tid);

		if (pnc->ainfo)
			mvStackPush(pnc_ipv6_ainfo_stack, (int)pnc->ainfo);

#ifdef CONFIG_MV_ETH_NFP_HWF
		if (pnc->pmtno > 0) {
			int pmtIdx = (pnc->flowid & PNC_FLOWID_HWF_MOD_MASK) >> PNC_FLOWID_HWF_MOD_OFFS;
/*
			mvOsPrintf("mvNfpPmtFree: outport=%d, pmtIdx=%d, pmtNo=%d\n",
						fib->outport, pmtIdx, pnc->pmtno);
*/
			mvNfpPmtFree(fib->outport, pmtIdx, pnc->pmtno);
		}
#endif /* CONFIG_MV_ETH_NFP_HWF */

		temp = pnc;
		pnc = pnc->next;
		memset(temp, 0, sizeof(NFP_RULE_PNC));
	}
	fib->pnc = NULL;
	fib->ref--;

	return MV_OK;
}

#ifdef NFP_NAT

static MV_STATUS mvNfpPncNatFibAdd(NFP_RULE_NAT *nat, NFP_RULE_FIB *fib)
{
	NFP_RULE_PNC *pnc;
	MV_U32 i, rxq;

	i = mvNfpPncGetFirstFree();
	if (i < NFP_PNC_ARRAY_SIZE) {
		pnc = pnc_hash + i;

		pnc->nat = nat;
		pnc->fib = fib;
		pnc->fib->ref++;

		pnc->flags = NFP_F_BUSY;
		pnc->flowid = i;
		nat->pnc = pnc;
		rxq = 0;

		/* Add TCAM entry */
		pnc->tid = TE_FLOW_NFP + i;
		pnc_ipv4_5_tuples_add(pnc->tid, pnc->flowid, nat->sip, nat->dip, nat->proto, nat->ports, rxq);
	}

	return MV_OK;

}

MV_STATUS mvNfpPncNatAdd(NFP_RULE_NAT *nat)
{
	NFP_RULE_FIB *fib;

	int family = MV_INET;
	const MV_U8 *natSip = (const MV_U8 *)&nat->sip;
	const MV_U8 *natDip = (const MV_U8 *)&nat->dip;
	const MV_U8 *natNip = (const MV_U8 *)&nat->nip;

	if (nat->flags & NFP_F_DNAT)
		fib = mvNfpFibLookup(family, natSip, natNip);
	else
		fib = mvNfpFibLookup(family, natSip, natDip);

	if (fib)
		return mvNfpPncNatFibAdd(nat, fib);

	mvOsPrintf("NFP (pnc) no route for %s: " MV_IPQUAD_FMT "->" MV_IPQUAD_FMT " nip=" MV_IPQUAD_FMT "\n",
		   (nat->flags & NFP_F_DNAT) ? "DNAT" : "SNAT", MV_IPQUAD(nat->sip),
		   MV_IPQUAD(nat->dip), MV_IPQUAD(nat->nip));

	/* remember incomplete NAT entry */
	mvListAddHead(&nat_pnc_inv_list, (MV_ULONG) nat);

	return MV_NOT_READY;
}

MV_STATUS mvNfpPncNatDel(NFP_RULE_NAT *nat)
{
	NFP_RULE_PNC *pnc = nat->pnc;

	if (pnc) {
		if (pnc->nat != nat) {
			mvOsPrintf("NFP (pnc) bug pnc=%p pnc->nat=%p nat=%p\n", pnc, pnc->nat, nat);
			goto out;
		}

		if (pnc->fib)
			pnc->fib->ref--;

		pnc_te_del(pnc->tid);

		pnc->fib = NULL;
		pnc->nat = NULL;
		pnc->flowid = 0;
		pnc->flags = 0;
		nat->pnc = NULL;
	}
out:
	return MV_OK;
}
#endif /* NFP_NAT */
