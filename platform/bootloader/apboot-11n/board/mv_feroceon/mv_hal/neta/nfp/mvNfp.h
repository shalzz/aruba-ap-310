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
* mvNfp.h - Header File for Marvell NFP (Routing and NAT)
*
* DESCRIPTION:
*       This header file contains macros, typedefs and function declarations
* 	specific to the Marvell Network Fast Processing (Routing and NAT).
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __mvNfp_h__
#define __mvNfp_h__

#include "mvSysHwConfig.h"
#include "mvTypes.h"
#include "mvCommon.h"
#include "mvStack.h"
#include "mv802_3.h"
#include "gbe/mvEthRegs.h"
#include "pnc/mvPnc.h"

#include "mvNfpDefs.h"


/*
 * Port capabilities
 */
#define	NFP_P_NAT		0x1
#define NFP_P_MH		0x2

extern unsigned int mgr_rule_jhash_iv;

/* The golden ration: an arbitrary value */
#define MV_JHASH_GOLDEN_RATIO           0x9e3779b9

#define MV_JHASH 0x9e3779b9
#define MV_JHASH_MIX(a, b, c)       \
{                                   \
    a -= b; a -= c; a ^= (c>>13);   \
    b -= c; b -= a; b ^= (a<<8);    \
    c -= a; c -= b; c ^= (b>>13);   \
    a -= b; a -= c; a ^= (c>>12);   \
    b -= c; b -= a; b ^= (a<<16);   \
    c -= a; c -= b; c ^= (b>>5);    \
    a -= b; a -= c; a ^= (c>>3);    \
    b -= c; b -= a; b ^= (a<<10);   \
    c -= a; c -= b; c ^= (b>>15);   \
}

static INLINE MV_U32 mv_jhash_array(const MV_U8 * k, MV_U32 length, MV_U32 initval)
{
	MV_U32 a, b, c, l;

	l = length;
	a = b = MV_JHASH_GOLDEN_RATIO;
	c = initval;

	while (l >= 12) {
		a += (k[0] + ((MV_U32) k[1] << 8) + ((MV_U32) k[2] << 16) + ((MV_U32) k[3] << 24));
		b += (k[4] + ((MV_U32) k[5] << 8) + ((MV_U32) k[6] << 16) + ((MV_U32) k[7] << 24));
		c += (k[8] + ((MV_U32) k[9] << 8) + ((MV_U32) k[10] << 16) + ((MV_U32) k[11] << 24));
		MV_JHASH_MIX(a, b, c);
		k += 12;
		l -= 12;
	}

	c += length;
	switch (l) {
	case 11:
		c += ((MV_U32) k[10] << 24);
	case 10:
		c += ((MV_U32) k[9] << 16);
	case 9:
		c += ((MV_U32) k[8] << 8);
	case 8:
		b += ((MV_U32) k[7] << 24);
	case 7:
		b += ((MV_U32) k[6] << 16);
	case 6:
		b += ((MV_U32) k[5] << 8);
	case 5:
		b += k[4];
	case 4:
		a += ((MV_U32) k[3] << 24);
	case 3:
		a += ((MV_U32) k[2] << 16);
	case 2:
		a += ((MV_U32) k[1] << 8);
	case 1:
		a += k[0];
	default:
		/* do nothing */;
	}

	MV_JHASH_MIX(a, b, c);
	return c;
}

static INLINE MV_U32 mv_jhash_3words(MV_U32 a, MV_U32 b, MV_U32 c, MV_U32 initval)
{
	a += MV_JHASH_GOLDEN_RATIO;
	b += MV_JHASH_GOLDEN_RATIO;
	c += initval;
	MV_JHASH_MIX(a, b, c);

	return c;
}

static INLINE MV_U32 mv_jhash_2addr(int family, const MV_U8 * aa, const MV_U8 * ab, MV_U32 c, MV_U32 initval)
{
	MV_U32 a, b;

	if (family == MV_INET6) {
		a = mv_jhash_array(aa, 16, initval);
		b = mv_jhash_array(ab, 16, initval);
	} else {
		a = *((const MV_U32 *)(aa));
		b = *((const MV_U32 *)(ab));
	}

	return mv_jhash_3words(a, b, c, initval);
}

/* L3 address copy. Supports AF_INET and AF_INET6 */
static inline void l3_addr_copy(int family, u8 *dst, const u8 *src)
{
	const u32 *s = (const u32 *)src;
	u32 *d = (u32 *) dst;

	*d++ = *s++;		/* 4 */
	if (family == MV_INET)
		return;

	*d++ = *s++;		/* 8 */
	*d++ = *s++;		/* 12 */
	*d++ = *s++;		/* 16 */
}

static INLINE MV_U32 l3_addr_eq(int family, const MV_U8 * a, const MV_U8 * b)
{
	const MV_U32 *aa = (const MV_U32 *)a;
	const MV_U32 *bb = (const MV_U32 *)b;
	MV_U32 r;

	r = *aa++ ^ *bb++;	/* 4 */
	if (family == MV_INET)
		return !r;

	r |= *aa++ ^ *bb++;	/* 8 */
	r |= *aa++ ^ *bb++;	/* 12 */
	r |= *aa++ ^ *bb++;	/* 16 */

	return !r;
}

#define	NFP_FIB_HASH_BITS   14
#define	NFP_FIB_HASH_SIZE   (1 << NFP_FIB_HASH_BITS)
#define	NFP_FIB_HASH_MASK   (NFP_FIB_HASH_SIZE - 1)

#define	NFP_NAT_HASH_BITS   14
#define	NFP_NAT_HASH_SIZE   (1 << NFP_NAT_HASH_BITS)
#define	NFP_NAT_HASH_MASK   (NFP_NAT_HASH_SIZE - 1)

#define	NFP_PNC_ARRAY_SIZE  ((TE_FLOW_NFP_END - TE_FLOW_NFP) + 1)

/*
 * Flags
 */
#define	NFP_F_DYNAMIC		0x1
#define	NFP_F_STATIC		0x2
#define	NFP_F_INV		    0x4
#define	NFP_F_SNAT		    0x8
#define	NFP_F_DNAT		    0x10
#define	NFP_F_BUSY		    0x20
#define	NFP_F_PPPOE_ADD     0x40
#define	NFP_F_PPPOE_REMOVE  0x80
#define NFP_F_HWF			0x100
#define NFP_F_PARTIAL		0x200

struct nfp_rule_pnc;

typedef struct nfp_rule_fib {
#ifdef NFP_FIB
	struct nfp_rule_fib *next;
	MV_U8	srcL3[MV_MAX_L3_ADDR_SIZE];
	MV_U8	dstL3[MV_MAX_L3_ADDR_SIZE];
	MV_U16	mh;
	MV_U8	da[MV_MAC_ADDR_SIZE];
	MV_U8	sa[MV_MAC_ADDR_SIZE];
	MV_U32	age;
	MV_U8	defGtwL3[MV_MAX_L3_ADDR_SIZE];
	MV_U32	flags;
	void	*outdev;
	MV_U8	outport;
#ifdef CONFIG_MV_ETH_NFP_HWF
	MV_U8	hwf_txp;
	MV_U8	hwf_txq;
	MV_U8	reserved;
#else
	MV_U8	reserved[3];
#endif /* CONFIG_MV_ETH_NFP_HWF */
	MV_U32	ref;
	struct nfp_rule_pnc *pnc;
	int		family;
#ifdef CONFIG_MV_ETH_NFP_PPP
	MV_U16	sid;		/* PPPoE session id */
#endif

#endif
} NFP_RULE_FIB;

typedef struct nfp_rule_nat {
#ifdef NFP_NAT
	struct nfp_rule_nat *next;
	MV_U32 sip;
	MV_U32 dip;
	MV_U32 ports;
	MV_U16 proto;
	MV_U16 nport;
	MV_U32 nip;
	MV_U32 flags;
	MV_U32 age;
	struct nfp_rule_pnc *pnc;
#endif
} NFP_RULE_NAT;

typedef struct nfp_rule_swf {
#ifdef NFP_SWF
	struct nfp_rule_swf *next;
	MV_U32 flowid;
	MV_U8 port;
	MV_U8 txp;
	MV_U8 txq;
	MV_U8 mh_sel;
#endif				/* NFP_SWF */
} NFP_RULE_SWF;

typedef struct nfp_rule_pnc {
#ifdef NFP_PNC
	NFP_RULE_FIB		*fib;
	NFP_RULE_NAT		*nat;
	MV_U32				age;
	MV_U32				flowid;
	MV_U16				tid;		/* TCAM entry ID */
	MV_U8				ainfo;
	MV_U8				pmtno;      /* Number of used PMT entires */
	MV_U32				flags;
	struct nfp_rule_pnc *next;
#endif							/* PNC */
} NFP_RULE_PNC;

typedef struct {
#ifdef NFP_STAT
	MV_U32 parse;
	MV_U32 process;
	MV_U32 process6;
	MV_U32 non_ip;
	MV_U32 ipv4_csum_err;
	MV_U32 ipv4_frag;
	MV_U32 mac_mcast;
	MV_U32 ttl_exp;
	MV_U32 l4_unknown;
	MV_U32 l4_csum_err;

#ifdef CONFIG_MV_ETH_NFP_PPP
	MV_U32 pppoe_add;
	MV_U32 pppoe_remove;
#endif				/* CONFIG_MV_ETH_NFP_PPP */

#ifdef NFP_FIB
	MV_U32 fib_hit;
	MV_U32 fib_miss;
	MV_U32 fib_inv;
#endif				/* NFP_FIB */

#ifdef NFP_NAT
	MV_U32 dnat_hit;
	MV_U32 dnat_miss;
	MV_U32 dnat_inv;
	MV_U32 snat_hit;
	MV_U32 snat_miss;
	MV_U32 snat_inv;
#endif				/* NFP_NAT */

#ifdef NFP_SWF
	MV_U32 swf_hit;
	MV_U32 swf_miss;
#endif

#ifdef NFP_PNC
	MV_U32 pnc_in;
	MV_U32 pnc_out;
	MV_U32 pnc_fib_hit;
	MV_U32 pnc_snat_hit;
	MV_U32 pnc_dnat_hit;
	MV_U32 pnc_flow_oor;
	MV_U32 pnc_flow_err;
	MV_U32 pnc_dnat_err;
	MV_U32 pnc_fib_err;
	MV_U32 pnc_snat_err;
#endif				/* NFP_PNC */

#ifdef CONFIG_MV_ETH_NFP_PPP
	MV_U32 pnc_pppoe_add;
	MV_U32 pnc_pppoe_remove;
#endif				/* CONFIG_MV_ETH_NFP_PPP */

#endif				/* NFP_STAT */
} NFP_STATS;

MV_VOID mvNfpInit(MV_VOID);
MV_STATUS mvNfpRx(MV_U32 inPort, NETA_RX_DESC *pRxDesc, MV_ETH_PKT *pPkt);
MV_VOID mvNfpPortCapSet(MV_U32 port, MV_U32 cap, MV_U32 on);
MV_U32 mvNfpPortCapGet(MV_U32 port);

MV_VOID mvNfpStats(MV_U32);
MV_VOID mvNfpStats(MV_U32 port);

#ifdef NFP_FIB
extern NFP_RULE_FIB **fib_hash;

static INLINE NFP_RULE_FIB *mvNfpFibLookup(int family, const MV_U8 *src_l3, const MV_U8 *dst_l3)
{
	MV_U32 hash;
	NFP_RULE_FIB *rule;
	hash = mv_jhash_2addr(family, src_l3, dst_l3, (u32) 0, mgr_rule_jhash_iv);
	hash &= NFP_FIB_HASH_MASK;
	rule = fib_hash[hash];
	while (rule) {
		if (!(rule->family == family))
			continue;
		if (l3_addr_eq(family, rule->srcL3, src_l3) && (l3_addr_eq(family, rule->dstL3, dst_l3)))
			return rule;
		rule = rule->next;
	}
	return NULL;
}

MV_STATUS mvNfpFibInit(void);
MV_STATUS mvNfpFibRuleAdd(int family, NFP_RULE_FIB *fib2);
MV_STATUS mvNfpFibRuleDel(int family, NFP_RULE_FIB *fib2);
MV_STATUS mvNfpFibRuleAge(int family, NFP_RULE_FIB *fib2);

MV_STATUS mvNfpFibRuleArpAdd(int family, NFP_RULE_FIB *fib2);
MV_STATUS mvNfpFibRuleArpDel(int family, NFP_RULE_FIB *fib2);
MV_STATUS mvNfpFibRuleArpAge(int family, NFP_RULE_FIB *fib2);

void mvNfpFibDump(void);
void mvNfpFibClean(void);
void mvNfpFibDestroy(void);
#endif /* NFP_FIB */

#ifdef NFP_NAT
extern NFP_RULE_NAT **nat_hash;
MV_STATUS mvNfpNatInit(MV_VOID);
MV_STATUS mvNfpNatRuleAdd(NFP_RULE_NAT *rule);
MV_STATUS mvNfpNatRuleDel(NFP_RULE_NAT *rule);
MV_STATUS mvNfpNatRuleAge(NFP_RULE_NAT *rule);
void mvNfpNatDump(void);
void mvNfpNatClean(void);
void mvNfpNatDestroy(void);
#endif /* NFP_NAT */

#ifdef NFP_SWF
MV_STATUS mvNfpSwfInit(void);
MV_STATUS mvNfpSwfRuleAdd(NFP_RULE_SWF *rule);
MV_STATUS mvNfpSwfRuleDel(NFP_RULE_SWF *rule);
void mvNfpSwfDump(void);
MV_U32 mvNfpSwfStats(MV_U8 *buf);
MV_STATUS mvNfpSwf(MV_U32 port, NETA_RX_DESC *pRxDesc, MV_ETH_PKT *pPkt);
#endif /* NFP_SWF */

#ifdef NFP_PNC
extern NFP_RULE_PNC *pnc_hash;
MV_STATUS mvNfpPncInit(void);
MV_STATUS mvNfpPncFibAdd(NFP_RULE_FIB *rule);
MV_STATUS mvNfpPncFibDel(NFP_RULE_FIB *rule);
MV_STATUS mvNfpPncNatAdd(NFP_RULE_NAT *rule);
MV_STATUS mvNfpPncNatDel(NFP_RULE_NAT *rule);
void mvNfpPncDump(void);
MV_STATUS mvNfpPnC(MV_U32 port, NETA_RX_DESC *pRxDesc, MV_ETH_PKT *pPkt);
#endif /* NFP_PNC */

#ifdef CONFIG_MV_ETH_NFP_PMT
MV_STATUS	mvNfpPmtInit(void);
int			mvNfpPmtAlloc(int port, int number);
void		mvNfpPmtFree(int port, int idx, int number);
MV_STATUS   mvNfpPmtFibUpdate(int idx, NFP_RULE_FIB *pFib);
#endif /* CONFIG_MV_ETH_NFP_PMT */

#endif /* __mvNfp_h__ */
