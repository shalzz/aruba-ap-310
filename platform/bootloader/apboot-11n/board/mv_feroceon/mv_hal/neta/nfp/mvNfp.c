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
* mvNfp.c - Marvell Network Fast Processing (Routing and NAT)
*
* DESCRIPTION:
*
*       Supported Features:
*       - OS independent.
*
*******************************************************************************/

/* includes */
#include "mvOs.h"
#include "mv802_3.h"
#include "mvDebug.h"
#include "gbe/mvEthRegs.h"
#include "gbe/mvNetaRegs.h"
#include "gbe/mvNeta.h"
#include "mvNfp.h"

static MV_U32		nfp_ports[MV_ETH_MAX_PORTS];
static NFP_STATS	nfp_stats[MV_ETH_MAX_PORTS];

unsigned int mgr_rule_jhash_iv;

#if 0
#define NFP_DBG mvOsPrintf
#else
#define NFP_DBG(X...)
#endif

#ifdef NFP_STAT
#define NFP_INC(p, s) nfp_stats[p].s++;
#else
#define NFP_INC(p, s)
#endif

/*
 * Init
 */
MV_VOID _INIT mvNfpInit(MV_VOID)
{
	mvOsMemset(nfp_ports, 0, sizeof(MV_U32) * MV_ETH_MAX_PORTS);
	mvOsMemset(nfp_stats, 0, sizeof(NFP_STATS) * MV_ETH_MAX_PORTS);
}

/*
 * Port capability, e.g. NAT/PPP/IPSEC
 */
MV_VOID mvNfpPortCapSet(MV_U32 port, MV_U32 cap, MV_U32 on)
{
	if (on)
		nfp_ports[port] |= cap;
	else
		nfp_ports[port] &= ~cap;
}

MV_U32 mvNfpPortCapGet(MV_U32 port)
{
	return nfp_ports[port];
}

static INLINE MV_U32 mvNfpPortCapNat(MV_U32 port)
{
	return nfp_ports[port] & NFP_P_NAT;
}

static INLINE void mvNfpFibMacUpdate(MV_U8 *pData, NFP_RULE_FIB *pFib)
{
	*(MV_U32 *) (pData + 0) = *(MV_U32 *) (&pFib->mh);
	*(MV_U32 *) (pData + 4) = *(MV_U32 *) (&pFib->da[2]);
	*(MV_U32 *) (pData + 8) = *(MV_U32 *) (&pFib->sa[0]);
	*(MV_U16 *) (pData + 12) = *(MV_U16 *) (&pFib->sa[4]);
}

static INLINE void mvNfpFibIPUpdate(MV_IP_HEADER *pIph)
{
	pIph->ttl--;
}

static INLINE void mvNfpFibIP6Update(MV_IP6_HEADER *pIp6Hdr)
{
	pIp6Hdr->hoplimit--;
}

#ifdef NFP_NAT
static INLINE NFP_RULE_NAT *mvNfpNatLookup(MV_U32 sip, MV_U32 dip, MV_U32 proto, MV_U32 ports)
{
	MV_U32 hash;
	NFP_RULE_NAT *rule;
	int family = MV_INET;

	if ((proto != MV_IP_PROTO_TCP) && (proto != MV_IP_PROTO_UDP))
		return NULL;
	hash = mv_jhash_2addr(family, (const MV_U8 *)&sip, (const MV_U8 *)&dip, ports | proto, mgr_rule_jhash_iv);
	hash &= NFP_NAT_HASH_MASK;

	rule = nat_hash[hash];

	while (rule) {
		if ((rule->sip == sip) && (rule->dip == dip) && (rule->ports == ports) && (rule->proto == proto)) {
			rule->age++;
			return rule;
		}
		rule = rule->next;
	}

	return NULL;
}

static INLINE void mvNfpNatUpdate(MV_IP_HEADER *pIpHdr, int ipHdrLen, NFP_RULE_NAT *pDnat, NFP_RULE_NAT *pSnat)
{
	MV_U16 *pPort = (MV_U16 *) ((char *)pIpHdr + ipHdrLen);

	if (pSnat) {
		pIpHdr->srcIP = pSnat->nip;
		*pPort = pSnat->nport;
	}

	if (pDnat) {
		pIpHdr->dstIP = pDnat->nip;
		pPort++;
		*pPort = pDnat->nport;
	}
}
#endif /* NFP_NAT */

#ifdef CONFIG_MV_ETH_NFP_PPP
static INLINE int removePppoeHeader(MV_U8 *pData)
{
	/* writing IP ethertype to the new location of ether header */
	pData[MV_ETH_ALEN + MV_PPPOE_HDR_SIZE] = 0x08;
	pData[MV_ETH_ALEN + MV_PPPOE_HDR_SIZE + 1] = 0x00;

	return MV_PPPOE_HDR_SIZE;
}

static INLINE int addPppoeHeader(MV_U8 *pData, NFP_RULE_FIB *pFib, MV_U16 ipLen)
{
	PPPoE_HEADER *pPPPNew;

	pData = pData - MV_PPPOE_HDR_SIZE;

	pData[MV_ETH_ALEN] = 0x88;
	pData[MV_ETH_ALEN + 1] = 0x64;

	pPPPNew = (PPPoE_HEADER *) (pData + MV_ETH_ALEN + MV_ETH_MH_SIZE);
	pPPPNew->version = 0x11;
	pPPPNew->code = 0x0;
	pPPPNew->proto = 0x2100;
	pPPPNew->session = pFib->sid;

	/* calculate PPPoE payload len considering padding for short packets */
	pPPPNew->len = MV_16BIT_BE(ipLen + MV_PPP_HDR_SIZE);
	return -MV_PPPOE_HDR_SIZE;
}
#endif /* CONFIG_MV_ETH_NFP_PPP */

#ifdef NFP_PNC
/*
 * NFP Process with PnC
 */
MV_STATUS mvNfpPnC(MV_U32 port, NETA_RX_DESC *pRxDesc, MV_ETH_PKT *pPkt)
{
	NFP_RULE_PNC *pPnc;
	NFP_RULE_FIB *pFib;
	MV_U8 *pData;
	MV_U8 *pWrite = NULL;
	MV_IP_HEADER *pIph = NULL;
	MV_IP6_HEADER *pIp6Hdr = NULL;
	int	ipHdrLen, ipOffset, flowId, shift = 0;
	MV_U16			ipLen = 0;
	MV_U8			tos = 0;
	MV_U8 *srcL3;
	MV_U8 *dstL3;
	int family = 0;
#ifdef NFP_NAT
	NFP_RULE_NAT	*pSnat, *pDnat;
#endif
	NFP_INC(port, pnc_in);

	if ((pRxDesc->status & NETA_RX_L3_MASK) == NETA_RX_L3_IP4_ERR) {
		NFP_INC(port, ipv4_csum_err);
		return MV_TERMINATE;
	} else
	if ((pRxDesc->status & NETA_RX_L3_MASK) == NETA_RX_L3_UN) {
		NFP_INC(port, non_ip);
		return MV_TERMINATE;
    }

    if ((((pRxDesc->status & NETA_RX_L4_MASK) == NETA_RX_L4_TCP) ||
		((pRxDesc->status & NETA_RX_L4_MASK) == NETA_RX_L4_UDP)) &&
		((pRxDesc->status & NETA_RX_L4_CSUM_OK_MASK) == 0)) {
		NFP_INC(port, l4_csum_err);
		return MV_TERMINATE;
	}

	flowId = pRxDesc->pncFlowId;

#ifdef CONFIG_MV_ETH_NFP_DEBUG
	/* sanity */
	if (flowId >= NFP_PNC_ARRAY_SIZE) {
		NFP_INC(port, pnc_flow_oor);
		return MV_TERMINATE;
	}
#endif /* CONFIG_MV_ETH_NFP_DEBUG */

	pPnc = pnc_hash + flowId;
	/* sanity - check that the flowId received in the descriptor matches the one we have in the	*/
	/* pnc_hash database. Note they could be different in case the rule was deleted *after* the	*/
	/* packet was already received and processed by the PnC (and got the flowId), for example	*/
	/* in a case of link down. So this is not just debug code.					*/
	if (pPnc->flowid != flowId) {
		NFP_INC(port, pnc_flow_err);
		return MV_TERMINATE;
	}

	pPnc->age++;

	pFib = pPnc->fib;

#ifdef CONFIG_MV_ETH_NFP_DEBUG
    /* sanity: Routing must be valid */
	if (!pFib || (pFib->flags & NFP_F_INV)) {
		NFP_INC(port, pnc_fib_err);
		return MV_TERMINATE;
	}
#endif /* CONFIG_MV_ETH_NFP_DEBUG */

	pFib->age++;
	NFP_INC(port, pnc_fib_hit);
#ifdef NFP_NAT
	/* DNAT */
	pDnat = NULL;
	if (mvNfpPortCapNat(port)) {
		if (!pPnc->nat) {
			NFP_INC(port, pnc_dnat_err);
		    return MV_TERMINATE;
		}
		if (pPnc->nat->flags & NFP_F_DNAT) {
		    pDnat = pPnc->nat;
		    NFP_INC(port, pnc_dnat_hit);
		    pDnat->age++;
		}
	}

	/* SNAT */
	pSnat = NULL;
	if (mvNfpPortCapNat(pFib->outport)) {
		if (!pPnc->nat) {
			NFP_INC(port, pnc_snat_err);
			return MV_TERMINATE;
		}
	if (pPnc->nat->flags & NFP_F_SNAT) {
			pSnat = pPnc->nat;
			NFP_INC(port, pnc_snat_hit);
			pSnat->age++;
		}
	}
#endif /* NFP_NAT */

    /* Calculate pointers to L2, L3 and L4 headers */
    pData = pPkt->pBuf +  pPkt->offset;
    ipOffset = (pRxDesc->status & NETA_RX_L3_OFFSET_MASK) >> NETA_RX_L3_OFFSET_OFFS;
    ipHdrLen = ((pRxDesc->status & NETA_RX_IP_HLEN_MASK) >> NETA_RX_IP_HLEN_OFFS) << 2;
    if ((pRxDesc->status & NETA_RX_L3_MASK) == NETA_RX_L3_IP6) {
		family = MV_INET6;
		pIp6Hdr = (MV_IP6_HEADER *)(pData + ipOffset);
		if (pIp6Hdr->hoplimit <= 1) {
			NFP_INC(port, ttl_exp);
			return MV_TERMINATE;
		}
		NFP_INC(port, process6);
		srcL3 = pIp6Hdr->srcAddr;
		dstL3 = pIp6Hdr->dstAddr;
		/*		tos   = pIp6Hdr->priority; */
    } else {
		family = MV_INET;
		pIph = (MV_IP_HEADER *)(pData + ipOffset);

		/* Check TTL validity - add to PNC - TBD */
		if (pIph->ttl <= 1) {
			NFP_INC(port, ttl_exp);
			return MV_TERMINATE;
		}
		tos = pIph->tos;

		/* Remove padding */
		ipLen = MV_16BIT_BE(pIph->totalLength);
		pPkt->bytes = (ipLen + ipOffset);
	}

	/* Update packet */
	if (pFib) {
#ifdef CONFIG_MV_ETH_NFP_PPP
	    /* remove pppoe header */
	    if (pFib->flags & NFP_F_PPPOE_REMOVE) {
			int pppShift = removePppoeHeader(pData);
		    shift += pppShift;
		    pPkt->bytes -= pppShift;
		    NFP_INC(port, pppoe_remove);
	    }
	    /* add pppoe header */
	    if (pFib->flags & NFP_F_PPPOE_ADD) {
		    int pppShift = addPppoeHeader(pData, pFib, ipLen);
		    shift += pppShift;
		    pPkt->bytes -= pppShift;
		    NFP_INC(port, pppoe_add);
		}
#endif /* CONFIG_MV_ETH_NFP_PPP */

		mvNfpFibMacUpdate(pData + shift, pFib);
		if (family == MV_INET)
			mvNfpFibIPUpdate(pIph);
		else
			mvNfpFibIP6Update(pIp6Hdr);


	    /* Process 2B of MH */
	    if (!(nfp_ports[pFib->outport] & NFP_P_MH)) {
			shift += MV_ETH_MH_SIZE;
			pPkt->bytes -= MV_ETH_MH_SIZE;
	    }
		if (family == MV_INET) {

			pPkt->tx_cmd |= NETA_TX_L3_IP4 | NETA_TX_IP_CSUM_MASK |
						NETA_TX_PKT_OFFSET_MASK(pPkt->offset + shift) |
			((ipOffset - shift) << NETA_TX_L3_OFFSET_OFFS) |
			((ipHdrLen >> 2) << NETA_TX_IP_HLEN_OFFS);
			pWrite = (MV_U8 *)pIph + ipHdrLen;
		} else {
			pPkt->tx_cmd |= NETA_TX_L3_IP6 |
						NETA_TX_PKT_OFFSET_MASK(pPkt->offset + shift) |
						((ipOffset - shift) << NETA_TX_L3_OFFSET_OFFS) |
						((ipHdrLen >> 2) << NETA_TX_IP_HLEN_OFFS);
			pWrite = (MV_U8 *)pIp6Hdr + ipHdrLen;
		}


#ifdef NFP_NAT
	if (pDnat || pSnat) {
			mvNfpNatUpdate(pIph, ipHdrLen, pDnat, pSnat);
			pPkt->tx_cmd &= ~NETA_TX_L4_CSUM_MASK;
			if ((pRxDesc->status & NETA_RX_L4_MASK) == NETA_RX_L4_TCP)
				pPkt->tx_cmd |= NETA_TX_L4_TCP | NETA_TX_L4_CSUM_FULL;
			else if ((pRxDesc->status & NETA_RX_L4_MASK) == NETA_RX_L4_UDP)
				pPkt->tx_cmd |= NETA_TX_L4_UDP | NETA_TX_L4_CSUM_FULL;
			pWrite += 4;
	}
#endif /* NFP_NAT */
	}
	if (pWrite != NULL) {
		if (shift < 0)
			pData += shift;
		/*mvOsPrintf("cacheFlashInv: pData=%p, size=%d\n", pData, (pWrite - pData));*/
		mvOsCacheFlushInv(NULL, pData, (pWrite - pData));
	}
    pPkt->dev = pFib->outdev;
    pPkt->tos = tos;

	NFP_INC(port, pnc_out);

	return MV_OK;
}
#endif /* PNC */


/*
 * NFP Process without PnC
 */
MV_STATUS mvNfpRx(MV_U32 port, NETA_RX_DESC *pRxDesc, MV_ETH_PKT *pPkt)
{
	NFP_RULE_FIB	*pFib;
	MV_U32			ports = 0, proto = 0;
	MV_U8 			*pData, *pEth, *pWrite = NULL;
	MV_IP_HEADER 	*pIph = NULL;
	MV_IP6_HEADER 	*pIp6Hdr = NULL;
	int 			ipHdrLen, ipOffset, shift = 0;
	MV_U16 			ipLen = 0;
	MV_U8 			tos = 0;
#ifdef NFP_NAT
	NFP_RULE_NAT 	*pDnat = NULL;
	NFP_RULE_NAT 	*pSnat = NULL;
#endif
	int family = 0;
	MV_U8 *srcL3;
	MV_U8 *dstL3;

	NFP_INC(port, parse);

	pData = pPkt->pBuf + pPkt->offset;

#ifdef CONFIG_MV_ETH_PNC
	if ((pRxDesc->status & NETA_RX_L3_MASK) == NETA_RX_L3_IP4_ERR) {
		NFP_INC(port, ipv4_csum_err);
		return MV_TERMINATE;
	} else if ((pRxDesc->status & NETA_RX_L3_MASK) == NETA_RX_L3_UN) {
		NFP_INC(port, non_ip);
		return MV_TERMINATE;
	}
	if ((((pRxDesc->status & NETA_RX_L4_MASK) == NETA_RX_L4_TCP) ||
	     ((pRxDesc->status & NETA_RX_L4_MASK) == NETA_RX_L4_UDP)) &&
	    ((pRxDesc->status & NETA_RX_L4_CSUM_OK_MASK) == 0)) {
		NFP_INC(port, l4_csum_err);
		return MV_TERMINATE;
	}

	ipOffset = (pRxDesc->status & NETA_RX_L3_OFFSET_MASK) >> NETA_RX_L3_OFFSET_OFFS;
	ipHdrLen = ((pRxDesc->status & NETA_RX_IP_HLEN_MASK) >> NETA_RX_IP_HLEN_OFFS) << 2;
#else
	if (((pRxDesc->status & ETH_RX_NOT_LLC_SNAP_FORMAT_MASK) == 0) ||
	    ((pRxDesc->status & ETH_RX_IP_HEADER_OK_MASK) == 0)) {
		NFP_INC(port, non_ip);
		return MV_TERMINATE;
	}

	if ((((pRxDesc->status & ETH_RX_L4_TYPE_MASK) == ETH_RX_L4_TCP_TYPE) ||
	     ((pRxDesc->status & ETH_RX_L4_TYPE_MASK) == ETH_RX_L4_UDP_TYPE)) &&
	    ((pRxDesc->status & NETA_RX_L4_CSUM_OK_MASK) == 0)) {
		NFP_INC(port, l4_csum_err);
		return MV_TERMINATE;
	}

	if ((pRxDesc->status & ETH_RX_VLAN_TAGGED_FRAME_MASK))
		ipOffset = MV_ETH_MH_SIZE + sizeof(MV_802_3_HEADER) + MV_VLAN_HLEN;
	else
		ipOffset = MV_ETH_MH_SIZE + sizeof(MV_802_3_HEADER);

	ipHdrLen = sizeof(MV_IP_HEADER);
#endif /* CONFIG_MV_ETH_PNC */

	pEth = pData + MV_ETH_MH_SIZE;
	if (pEth[0] & 0x01) {	/* Check multicast and broadcast */
		NFP_INC(port, mac_mcast);
		return MV_TERMINATE;
	}
	if (NETA_RX_L3_IS_IP6(pRxDesc->status)) {
		family = MV_INET6;
		pIp6Hdr = (MV_IP6_HEADER *) (pData + ipOffset);
		if (pIp6Hdr->hoplimit <= 1) {
			NFP_INC(port, ttl_exp);
			return MV_TERMINATE;
		}

		NFP_INC(port, process6);

		srcL3 = pIp6Hdr->srcAddr;
		dstL3 = pIp6Hdr->dstAddr;
	} else {
		family = MV_INET;
		pIph = (MV_IP_HEADER *) (pData + ipOffset);
		if (NETA_RX_IP_IS_FRAG(pRxDesc->status)) {
			NFP_INC(port, ipv4_frag);
			return MV_TERMINATE;
		}

		if (pIph->ttl <= 1) {
			NFP_INC(port, ttl_exp);
			return MV_TERMINATE;
		}

		/* Remove padding */
		ipLen = MV_16BIT_BE(pIph->totalLength);
		pPkt->bytes = (ipLen + ipOffset);

		NFP_INC(port, process);

		/* save 5-tuple information from received packet */
		dstL3 = (MV_U8 *) &(pIph->dstIP);
		srcL3 = (MV_U8 *) &(pIph->srcIP);

		proto = pIph->protocol;
		ports = *(MV_U32 *) ((char *)pIph + ipHdrLen);
		tos = pIph->tos;

#ifdef NFP_NAT
		/* DNAT */
		if (mvNfpPortCapNat(port)) {
			if (!NETA_RX_L4_CSUM_IS_OK(pRxDesc->status)) {
				NFP_INC(port, l4_unknown);
				return MV_TERMINATE;
			}
			pDnat = mvNfpNatLookup(pIph->srcIP, pIph->dstIP, proto, ports);

			if (!pDnat) {
				NFP_INC(port, dnat_miss);
				return MV_TERMINATE;
			}

			if ((pDnat->flags & NFP_F_DNAT) == 0) {
				NFP_INC(port, dnat_inv);
				return MV_TERMINATE;
			}

			NFP_INC(port, dnat_hit);

			/* In case we found a DNAT rule,
			   we need to update the dip and destination port for future FIB and SNAT lookups */
			dstL3 = (MV_U8 *) &(pDnat->nip);
			ports = (pDnat->nport << 16) | (ports & 0x0000FFFF);
		} else
			pDnat = NULL;
#endif /* NFP_NAT */
	}

	pFib = mvNfpFibLookup(family, srcL3, dstL3);

	if (!pFib) {
		if (family == MV_INET)
			NFP_DBG("%s failed: " MV_IPQUAD_FMT "->" MV_IPQUAD_FMT "\n",
				__func__, MV_IPQUAD(*((u32 *) srcL3)), MV_IPQUAD(*((u32 *) dstL3)));
		else
			NFP_DBG("%s failed: " MV_IP6_FMT "->" MV_IP6_FMT "\n",
				__func__, MV_IP6_ARG(srcL3), MV_IP6_ARG(dstL3));

		NFP_INC(port, fib_miss);
		return MV_TERMINATE;
	}
	pFib->age++;

	if (pFib->flags & NFP_F_INV) {
		NFP_INC(port, fib_inv);
		return MV_TERMINATE;
	}
	NFP_INC(port, fib_hit);

#ifdef NFP_NAT
	if (pIph != NULL) {
		/* SNAT */
		if (mvNfpPortCapNat(pFib->outport)) {
			if (!NETA_RX_L4_CSUM_IS_OK(pRxDesc->status)) {
				NFP_INC(port, l4_unknown);
				return MV_TERMINATE;
			}
			/* We must not use here pIph->srcIP
			   since it might not reflect an update */
			pSnat = mvNfpNatLookup(*((u32 *) srcL3), *((u32 *) dstL3), proto, ports);

			if (!pSnat) {
				NFP_INC(port, snat_miss);
				return MV_TERMINATE;
			}

			if ((pSnat->flags & NFP_F_SNAT) == 0) {
				NFP_INC(port, snat_inv);
				return MV_TERMINATE;
			}

			NFP_INC(port, snat_hit);
		} else
			pSnat = NULL;
	}
#endif /* NFP_NAT */

	/* if (pIph != NULL) */
	/* Modify packet */
	if (pFib) {
#ifdef CONFIG_MV_ETH_NFP_PPP
		/* remove pppoe header */
		if (pFib->flags & NFP_F_PPPOE_REMOVE) {
			int pppShift = removePppoeHeader(pData);

			shift += pppShift;
			pPkt->bytes -= pppShift;
			NFP_INC(port, pppoe_remove);
		}
		/* add pppoe header */
		if (pFib->flags & NFP_F_PPPOE_ADD) {
			int pppShift = addPppoeHeader(pData, pFib, ipLen);

			shift += pppShift;
			pPkt->bytes -= pppShift;
			NFP_INC(port, pppoe_add);

		}
#endif /* CONFIG_MV_ETH_NFP_PPP */

		mvNfpFibMacUpdate(pData + shift, pFib);
		if (family == MV_INET)
			mvNfpFibIPUpdate(pIph);
		else
			mvNfpFibIP6Update(pIp6Hdr);

		/* Process 2B of MH */
		if (!(nfp_ports[pFib->outport] & NFP_P_MH)) {
			shift += MV_ETH_MH_SIZE;
			pPkt->bytes -= MV_ETH_MH_SIZE;
		}
		if (family == MV_INET) {

			pPkt->tx_cmd |= NETA_TX_L3_IP4 | NETA_TX_IP_CSUM_MASK |
			    NETA_TX_PKT_OFFSET_MASK(pPkt->offset + shift) |
			    ((ipOffset - shift) << NETA_TX_L3_OFFSET_OFFS) | ((ipHdrLen >> 2) << NETA_TX_IP_HLEN_OFFS);
			pWrite = (MV_U8 *) pIph + ipHdrLen;
		} else {
			pPkt->tx_cmd |= NETA_TX_L3_IP6 |
			    NETA_TX_PKT_OFFSET_MASK(pPkt->offset + shift) |
			    ((ipOffset - shift) << NETA_TX_L3_OFFSET_OFFS) | ((ipHdrLen >> 2) << NETA_TX_IP_HLEN_OFFS);
			pWrite = (MV_U8 *) pIp6Hdr + ipHdrLen;
		}

#ifdef NFP_NAT
		if (pDnat || pSnat) {
			mvNfpNatUpdate(pIph, ipHdrLen, pDnat, pSnat);

			pPkt->tx_cmd &= ~NETA_TX_L4_CSUM_MASK;
			if (NETA_RX_L4_IS_TCP(pRxDesc->status))
				pPkt->tx_cmd |= NETA_TX_L4_TCP | NETA_TX_L4_CSUM_FULL;
			else if (NETA_RX_L4_IS_UDP(pRxDesc->status))
				pPkt->tx_cmd |= NETA_TX_L4_UDP | NETA_TX_L4_CSUM_FULL;

			pWrite += 4;
		}
#endif /* NFP_NAT */
	}
	/* Flash maximum accessed data before TX */
	if (pWrite != NULL) {
		if (shift < 0)
			pData += shift;

		/*mvOsPrintf("cacheFlashInv: pData=%p, size=%d\n", pData, (pWrite - pData)); */
		mvOsCacheFlushInv(NULL, pData, (pWrite - pData));
	}
	pPkt->dev = pFib->outdev;
	pPkt->tos = tos;

	return MV_OK;
}

MV_VOID mvNfpStats(MV_U32 port)
{
	mvOsPrintf("\n====================================================\n");
	mvOsPrintf(" NFP statistics");
	mvOsPrintf("\n-------------------------------\n");

#ifdef NFP_STAT
	mvOsPrintf("nfp_parsed....................%10u\n", nfp_stats[port].parse);
	mvOsPrintf("nfp_non_ip....................%10u\n", nfp_stats[port].non_ip);
	mvOsPrintf("nfp_ipv4_csum_err.............%10u\n", nfp_stats[port].ipv4_csum_err);
	mvOsPrintf("nfp_ipv4_frag.................%10u\n", nfp_stats[port].ipv4_frag);
	mvOsPrintf("nfp_mac_mcast.................%10u\n", nfp_stats[port].mac_mcast);
	mvOsPrintf("nfp_ttl_exp...................%10u\n", nfp_stats[port].ttl_exp);
	mvOsPrintf("nfp_l4_unknown................%10u\n", nfp_stats[port].l4_unknown);
	mvOsPrintf("nfp_l4_csum_err...............%10u\n", nfp_stats[port].l4_csum_err);
	mvOsPrintf("nfp_process...................%10u\n", nfp_stats[port].process);
	mvOsPrintf("nfp_process_ipv6..............%10u\n", nfp_stats[port].process6);
#ifdef NFP_FIB
	mvOsPrintf("nfp_fib_hit...................%10u\n", nfp_stats[port].fib_hit);
	mvOsPrintf("nfp_fib_miss..................%10u\n", nfp_stats[port].fib_miss);
	mvOsPrintf("nfp_fib_inv...................%10u\n", nfp_stats[port].fib_inv);
#endif /* NFP_FIB */

#ifdef NFP_NAT
	mvOsPrintf("nfp_dnat_hit..................%10u\n", nfp_stats[port].dnat_hit);
	mvOsPrintf("nfp_dnat_miss.................%10u\n", nfp_stats[port].dnat_miss);
	mvOsPrintf("nfp_dnat_inv..................%10u\n", nfp_stats[port].dnat_inv);

	mvOsPrintf("nfp_snat_hit..................%10u\n", nfp_stats[port].snat_hit);
	mvOsPrintf("nfp_snat_miss.................%10u\n", nfp_stats[port].snat_miss);
	mvOsPrintf("nfp_snat_inv..................%10u\n", nfp_stats[port].snat_inv);
#endif /* NFP_NAT */

#ifdef NFP_SWF
	mvOsPrintf("nfp_swf_hit...................%10u\n", nfp_stats[port].swf_hit);
	mvOsPrintf("nfp_swf_miss..................%10u\n", nfp_stats[port].swf_miss);
#endif /* NFP_SWF */

#ifdef NFP_PNC
	mvOsPrintf("pnc_in........................%10u\n", nfp_stats[port].pnc_in);
	mvOsPrintf("pnc_dnat_hit..................%10u\n", nfp_stats[port].pnc_dnat_hit);
	mvOsPrintf("pnc_fib_hit...................%10u\n", nfp_stats[port].pnc_fib_hit);
	mvOsPrintf("pnc_snat_hit..................%10u\n", nfp_stats[port].pnc_snat_hit);
	mvOsPrintf("pnc_out.......................%10u\n", nfp_stats[port].pnc_out);
	mvOsPrintf("pnc_flow_oor..................%10u\n", nfp_stats[port].pnc_flow_oor);
	mvOsPrintf("pnc_flow_err..................%10u\n", nfp_stats[port].pnc_flow_err);
	mvOsPrintf("pnc_dnat_err..................%10u\n", nfp_stats[port].pnc_dnat_err);
	mvOsPrintf("pnc_fib_err...................%10u\n", nfp_stats[port].pnc_fib_err);
	mvOsPrintf("pnc_snat_err..................%10u\n", nfp_stats[port].pnc_snat_err);
#endif /* NFP_PNC */

#ifdef CONFIG_MV_ETH_NFP_PPP
	mvOsPrintf("pppoe_add.....................%10u\n", nfp_stats[port].pppoe_add);
	mvOsPrintf("pppoe_remove..................%10u\n", nfp_stats[port].pppoe_remove);
#endif /* CONFIG_MV_ETH_NFP_PPP */

	mvOsMemset(&nfp_stats[port], 0, sizeof(NFP_STATS));
#endif /* NFP_STAT */
}
