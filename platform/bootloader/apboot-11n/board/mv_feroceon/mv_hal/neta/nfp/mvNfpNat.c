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
* mvNfpNat.c - Marvell Fast Network Processing
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
#include "mvNfpDefs.h"
#include "mvNfp.h"

#define NFP_DBG(x...) mvOsPrintf(x)
/* #define NFP_DBG(x...) */

NFP_RULE_NAT **nat_hash = NULL;

MV_STATUS _INIT mvNfpNatInit(MV_VOID)
{
	MV_U32 bytes = sizeof(NFP_RULE_NAT *) * NFP_NAT_HASH_SIZE;

	nat_hash = (NFP_RULE_NAT **)mvOsMalloc(bytes);
	if (nat_hash == NULL) {
		mvOsPrintf("NFP (nat): not enough memory\n");
		return MV_NO_RESOURCE;
	}

	mvOsMemset(nat_hash, 0, bytes);

	mvOsPrintf("NFP (nat) init %d entries, %d bytes\n", NFP_NAT_HASH_SIZE, bytes);

	return MV_OK;
}

static MV_VOID mvNfpNatRulePrint(NFP_RULE_NAT *nat)
{
	mvOsPrintf(MV_IPQUAD_FMT":%d"
				"->"MV_IPQUAD_FMT":%d"
				" %-2d "
				"%s:"MV_IPQUAD_FMT" "
				"age=%x (%p)\n",
				MV_IPQUAD(nat->sip), MV_16BIT_BE(nat->ports & 0xFFFF),
				MV_IPQUAD(nat->dip), MV_16BIT_BE(nat->ports >> 16),
				nat->proto, nat->flags & NFP_F_DNAT ? "DNAT" : "SNAT",
				MV_IPQUAD(nat->nip), nat->age, nat);
}

static INLINE MV_U32 mvNfpNatHash(NFP_RULE_NAT *nat)
{
	int family = MV_INET;
	return mv_jhash_2addr(family, (const MV_U8 *) &nat->sip, (const MV_U8 *) &nat->dip, nat->ports | nat->proto,
						    mgr_rule_jhash_iv);

}

static INLINE NFP_RULE_NAT *mvNfpNatLookup(NFP_RULE_NAT *nat2)
{
	MV_U32 hash;
	NFP_RULE_NAT *nat;

	hash = mvNfpNatHash(nat2);
	hash &= NFP_NAT_HASH_MASK;

	nat = nat_hash[hash];

	while (nat) {
		if ((nat->sip == nat2->sip) && (nat->dip == nat2->dip) &&
			(nat->ports == nat2->ports) && (nat->proto == nat2->proto))
			return nat;
		nat = nat->next;
	}

	return NULL;
}

MV_STATUS   mvNfpNatRuleAge(NFP_RULE_NAT *nat2)
{
	NFP_RULE_NAT *nat;

	nat = mvNfpNatLookup(nat2);
	if (nat) {
		nat2->age = nat->age;
		nat->age = 0;
	return MV_OK;
	}
    nat2->age = 0;
	return MV_NOT_FOUND;
}

MV_STATUS mvNfpNatRuleAdd(NFP_RULE_NAT *nat2)
{
	MV_U32 hash;
	NFP_RULE_NAT *nat;
	nat = mvNfpNatLookup(nat2);
	if (nat)
		return MV_OK;
	hash = mvNfpNatHash(nat2);
	hash &= NFP_NAT_HASH_MASK;

	nat = (NFP_RULE_NAT *)mvOsMalloc(sizeof(NFP_RULE_NAT));
	if (!nat) {
		mvOsPrintf("%s: OOM\n", __func__);
		return MV_FAIL;
	}

	mvOsMemcpy(nat, nat2, sizeof(NFP_RULE_NAT));

	nat->next = nat_hash[hash];
	nat_hash[hash] = nat;

	NFP_DBG("NFP (nat) add %p\n", nat);

#ifdef NFP_PNC
	mvNfpPncNatAdd(nat);
#endif

    return MV_OK;
}

MV_STATUS mvNfpNatRuleDel(NFP_RULE_NAT *nat2)
{
	MV_U32 hash;
	NFP_RULE_NAT *nat, *prev;

	hash = mvNfpNatHash(nat2);
	hash &= NFP_NAT_HASH_MASK;

	nat = nat_hash[hash];
	prev = NULL;

	while (nat) {
		if ((nat->sip == nat2->sip) &&
			(nat->dip == nat2->dip) &&
			(nat->ports == nat2->ports) &&
			(nat->proto == nat2->proto)) {

			if (prev)
				prev->next = nat->next;
			else
				nat_hash[hash] = nat->next;
#ifdef NFP_PNC
			mvNfpPncNatDel(nat);
#endif
			NFP_DBG("NFP (nat) del %p\n", nat);

			mvOsFree(nat);
			return MV_OK;
		}

		prev = nat;
		nat = nat->next;
	}

	return MV_NOT_FOUND;
}

void    mvNfpNatClean(void)
{
	int             i;
	NFP_RULE_NAT    *nat, *next;

	for (i = 0; i < NFP_NAT_HASH_SIZE; i++) {
		nat = nat_hash[i];

		while (nat) {
#ifdef NFP_PNC
		mvNfpPncNatDel(nat);
#endif
		next = nat->next;
		mvOsFree(nat);
		nat = next;
	}
	nat_hash[i] = NULL;
    }
}

void	mvNfpNatDestroy(void)
{
	if (nat_hash)
		mvOsFree(nat_hash);
}

void    mvNfpNatDump(void)
{
	MV_U32 i;
	NFP_RULE_NAT *nat;

	mvOsPrintf("(nat)\n");
	for (i = 0; i < NFP_NAT_HASH_SIZE; i++) {
		nat = nat_hash[i];

		while (nat) {
			mvOsPrintf(" [%2d] ", i);
			mvNfpNatRulePrint(nat);

			nat = nat->next;
		}
	}
}

