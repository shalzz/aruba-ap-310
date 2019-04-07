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
* mvNfpFib.c - Marvell Fast Network Processing
*
* DESCRIPTION:
*
*       Supported Features:
*       - OS independent.
*
*******************************************************************************/

#include "mvOs.h"
#include "mvDebug.h"
#include "gbe/mvNeta.h"
#include "mvNfpDefs.h"
#include "mvNfp.h"

/*#define NFP_DBG(x...) mvOsPrintf(x)*/
#define NFP_DBG(x...)

NFP_RULE_FIB **fib_hash = NULL;

MV_STATUS _INIT mvNfpFibInit(void)
{
	MV_U32 bytes = sizeof(NFP_RULE_FIB *) * NFP_FIB_HASH_SIZE;

	fib_hash = (NFP_RULE_FIB **) mvOsMalloc(bytes);
	if (fib_hash == NULL) {
		mvOsPrintf("NFP (fib): not enough memory\n");
		return MV_NO_RESOURCE;
	}

	mvOsMemset(fib_hash, 0, bytes);

	mvOsPrintf("NFP (fib) init %d entries, %d bytes\n", NFP_FIB_HASH_SIZE, bytes);

	return MV_OK;
}

static void mvNfpFibRulePrint(NFP_RULE_FIB *fib)
{
	if (fib->family == MV_INET) {

		mvOsPrintf("IPv4: " MV_IPQUAD_FMT "->" MV_IPQUAD_FMT"\n",
			   MV_IPQUAD(*((u32 *) fib->srcL3)), MV_IPQUAD(*((u32 *) fib->dstL3)));
	} else {
		mvOsPrintf("IPv6: " MV_IP6_FMT "->" MV_IP6_FMT"\n",
					MV_IP6_ARG(fib->srcL3), MV_IP6_ARG(fib->dstL3));

	}
	mvOsPrintf("     mh=%2.2x:%2.2x da=" MV_MACQUAD_FMT " sa=" MV_MACQUAD_FMT "\n",
				((MV_U8 *) &fib->mh)[0], ((MV_U8 *) &fib->mh)[1], MV_MACQUAD(fib->da), MV_MACQUAD(fib->sa));
	mvOsPrintf("     port=%d flags=0x%x ref=%d age=%x fib=%p pnc=%p\n",
				fib->outport, fib->flags, fib->ref, fib->age, fib, fib->pnc);

	mvOsPrintf("Flags: ");
	if (fib->flags & NFP_F_DYNAMIC)
		mvOsPrintf("NFP_F_DYNAMIC ");
	if (fib->flags & NFP_F_STATIC)
		mvOsPrintf("NFP_F_STATIC ");
	if (fib->flags & NFP_F_INV)
		mvOsPrintf("NFP_F_INV ");
	if (fib->flags & NFP_F_SNAT)
		mvOsPrintf("NFP_F_SNAT ");
	if (fib->flags & NFP_F_DNAT)
		mvOsPrintf("NFP_F_DNAT ");
	if (fib->flags & NFP_F_BUSY)
		mvOsPrintf("NFP_F_BUSY ");
	if (fib->flags & NFP_F_PPPOE_ADD)
		mvOsPrintf("NFP_F_PPPOE_ADD ");
	if (fib->flags & NFP_F_PPPOE_REMOVE)
		mvOsPrintf("NFP_F_PPPOE_REMOVE ");
	if (fib->flags & NFP_F_HWF)
		mvOsPrintf("NFP_F_HWF ");

	mvOsPrintf("\n");
}

MV_STATUS mvNfpFibRuleArpAdd(int family, NFP_RULE_FIB *fib2)
{
	MV_U32			i = NFP_FIB_HASH_SIZE;
	NFP_RULE_FIB	*fib;
	MV_STATUS		status = MV_NOT_FOUND;

	if (!fib_hash)
		return MV_FAIL;

	while (i--) {
		fib = fib_hash[i];
		while (fib) {
			if ((l3_addr_eq(family, fib->defGtwL3, fib2->defGtwL3))) {

				/* Found routing entry */
				if (fib->flags & NFP_F_INV) {
					/* Validate new route */
					fib->flags &= ~NFP_F_INV;
					mvOsMemcpy(fib->da, fib2->da, 6);
				} else if (memcmp(fib->da, fib2->da, 6)) {
					/* Update already valid route with new MAC address. FIXME - update HWF rule */
					mvOsMemcpy(fib->da, fib2->da, 6);
				} else {
					/* Do nothing - MAC address don't changed */
				}
				status = MV_OK;
			}
			fib = fib->next;
		}
	}
	return status;
}

/* Invalidate all routes with rule2->defGtwL3 */
MV_STATUS mvNfpFibRuleArpDel(int family, NFP_RULE_FIB *fib2)
{
	MV_U32			i = NFP_FIB_HASH_SIZE;
	NFP_RULE_FIB	*fib;
	MV_STATUS		status = MV_NOT_FOUND;

	if (!fib_hash)
		return MV_FAIL;

	while (i--) {
		fib = fib_hash[i];
		while (fib) {
			if ((l3_addr_eq(family, fib->defGtwL3, fib2->defGtwL3))) {

				/* Invalidate existing route. FIXME - update HWF rule */
				fib->flags &= ~NFP_F_INV;
				status = MV_OK;
			}
			fib = fib->next;
		}
	}
	return status;
}

/* If exist routing entires with the rule2->defGtwL3 - prevent aging */
MV_STATUS mvNfpFibRuleArpAge(int family, NFP_RULE_FIB *fib2)
{
	MV_U32			i = NFP_FIB_HASH_SIZE;
	NFP_RULE_FIB	*fib;

	fib2->age = 0;
	if (!fib_hash)
		return MV_FAIL;

	while (i--) {
		fib = fib_hash[i];
		while (fib) {
			if ((l3_addr_eq(family, fib->defGtwL3, fib2->defGtwL3))) {

				/* Prevent neigbour aging */
				fib2->age = 1;
				return MV_OK;
			}
			fib = fib->next;
		}
	}
	return MV_NOT_FOUND;
}

MV_STATUS mvNfpFibRuleAge(int family, NFP_RULE_FIB *fib2)
{
	NFP_RULE_FIB *fib;

	fib = mvNfpFibLookup(family, fib2->srcL3, fib2->dstL3);
	if (fib) {
		fib2->age = fib->age;
		fib->age = 0;
		return MV_OK;
	}
	fib2->age = 0;
	return MV_NOT_FOUND;
}

MV_STATUS mvNfpFibRuleAdd(int family, NFP_RULE_FIB *fib2)
{
	MV_U32 hash;
	NFP_RULE_FIB *fib;

	fib = mvNfpFibLookup(family, fib2->srcL3, fib2->dstL3);
	if (fib) {
		MV_U32 ref = fib->ref;
		mvOsMemcpy(fib, fib2, sizeof(NFP_RULE_FIB));
		fib->ref = ref;
		goto out;
	}

	hash = mv_jhash_2addr(family, fib2->srcL3, fib2->dstL3, (u32) 0, mgr_rule_jhash_iv);
	hash &= NFP_FIB_HASH_MASK;

	fib = (NFP_RULE_FIB *) mvOsMalloc(sizeof(NFP_RULE_FIB));
	if (!fib) {
		mvOsPrintf("NFP (fib) %s OOM\n", __func__);
		return MV_FAIL;
	}

	mvOsMemcpy(fib, fib2, sizeof(NFP_RULE_FIB));

	fib->next = fib_hash[hash];
	fib_hash[hash] = fib;
out:
	NFP_DBG("NFP (fib) add %p\n", fib);

#ifdef NFP_PNC
	mvNfpPncFibAdd(fib);
#endif

	return MV_OK;
}

MV_STATUS mvNfpFibRuleDel(int family, NFP_RULE_FIB *fib2)
{
	MV_U32 hash;
	NFP_RULE_FIB *fib, *prev;

	hash = mv_jhash_2addr(family, (const MV_U8 *)&fib2->srcL3, (const MV_U8 *)&fib2->dstL3, 0, mgr_rule_jhash_iv);
	hash &= NFP_FIB_HASH_MASK;

	fib = fib_hash[hash];
	prev = NULL;

	while (fib) {
		if ((l3_addr_eq(family, fib->srcL3, fib2->srcL3)) && (l3_addr_eq(family, fib->dstL3, fib2->dstL3))) {

#ifdef NFP_PNC
			mvNfpPncFibDel(fib);
#endif
			if (fib->ref) {
				fib->flags = NFP_F_INV;
				NFP_DBG("NFP (fib) del %p, retain ref=%d \n", fib, fib->ref);
				return MV_OK;
			}

			if (prev)
				prev->next = fib->next;
			else
				fib_hash[hash] = fib->next;

			NFP_DBG("NFP (fib) del %p\n", fib);
			mvOsFree(fib);

			return MV_OK;
		}

		prev = fib;
		fib = fib->next;
	}

	return MV_NOT_FOUND;
}

void mvNfpFibClean(void)
{
	int i;
	NFP_RULE_FIB *fib, *next;

	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];
		while (fib) {
#ifdef NFP_PNC
			mvNfpPncFibDel(fib);
#endif
			next = fib->next;
			mvOsFree(fib);
			fib = next;
		}
		fib_hash[i] = NULL;
	}
}

void mvNfpFibDestroy(void)
{
	if (fib_hash != NULL)
		mvOsFree(fib_hash);
}

void mvNfpFibDump(void)
{
	MV_U32 i;
	NFP_RULE_FIB *fib;

	mvOsPrintf("(fib)\n");
	for (i = 0; i < NFP_FIB_HASH_SIZE; i++) {
		fib = fib_hash[i];

		while (fib) {
			mvOsPrintf(" [%4d] ", i);
			mvNfpFibRulePrint(fib);
			fib = fib->next;
		}
	}
}
