/*******************************************************************************
/ mvNfpSec.c
*/

#include "mvCommon.h"
#include "mvTypes.h"
#include "mvOs.h"
#include "cesa/mvCesa.h"
#include "cesa/mvCesaRegs.h"
#include "mvNfpSec.h"


static MV_NFP_SEC_STATE nfpSecStates[NFP_SEC_MAX_STATES];
static MV_NFP_SEC_RULE **nfpSecRuleDb;
static MV_U32 nfpSecRuleDbSize;
static MV_U32 nfp_sec_jhash_iv;

#define NFP_MAGIC	0x4E465021	/* "NFP!" */

/* extern void nfp_cesa_lock(void); */
/* extern void nfp_cesa_unlock(void); */

void nfp_cesa_lock(void)
{
}
void nfp_cesa_unlock(void)
{
}


static MV_STATUS mvNfpSecAhIn(MV_NFP_SEC_INFO *info)
{
	MV_STATUS status;
	return status;
}

static MV_STATUS mvNfpSecPacketInject(MV_NFP_SEC_INFO *info)
{
	MV_STATUS status;
	return status;
}

static MV_STATUS mvNfpSecEspIn(MV_NFP_SEC_INFO *info)
{
	MV_CESA_CRYPTO_ALG enc;
	MV_CESA_MAC_MODE mac;
	MV_CESA_COMMAND *cmd;
	MV_STATUS  status;
	MV_U8 *byte;
	info->hLen += sizeof(MV_ESP_HEADER);
	enc = info->pEspRule->state->cesaSessionIn.cryptoAlgorithm;
	mac = info->pEspRule->state->cesaSessionIn.macMode;
	if (enc == MV_CESA_CRYPTO_NULL && mac == MV_CESA_MAC_NULL) {
		byte = info->pPkt->pFrags->bufVirtPtr +
			info->pPkt->pFrags->dataSize - info->tLen - 1;
		info->proto = *byte--;
		info->tLen  = *byte+2;
		return mvNfpSecPacketInject(info);
	}
/*	info->receiver = mvNfpSecEspPostIn; */
	cmd = &info->cesaCommand;
	memset(cmd, 0, sizeof(MV_CESA_COMMAND));
	cmd->pReqPrv = info;
	cmd->pSrc    = &info->cesaMbuf;
	cmd->pDst    = &info->cesaMbuf;
	cmd->sessionId = info->pEspRule->state->cesaSidIn;
	/*if (enc != MV_CESA_CRYPTO_NULL)
	   cmd->ivOffset = (MV_U8*)info->pEspHdr - (MV_U8*) */
	return status;
}

MV_VOID mvNfpSecStateDestroy(MV_NFP_SEC_STATE *state)
{
	state->refCnt -= 1;
	if (!state->refCnt) {
		nfp_cesa_lock();
		mvCesaSessionClose(state->cesaSidIn);
		mvCesaSessionClose(state->cesaSidOut);
		nfp_cesa_unlock();
	}
}

MV_VOID mvNfpSecRuleDelete(MV_NFP_SEC_RULE *pRule)
	{

	MV_NFP_SEC_STATE *state = pRule->state;
	mvOsFree(pRule);
	mvNfpSecStateDestroy(state);
	}

MV_STATUS mvNfpSecInit(MV_U32 dbSize)
	 {
	 if (dbSize == 0)
		return MV_BAD_PARAM;

	nfpSecRuleDbSize = dbSize;
	nfpSecRuleDb = mvOsMalloc(dbSize * sizeof(MV_NFP_SEC_RULE *));
	if (!nfpSecRuleDb)
		return MV_NO_RESOURCE;

	memset(nfpSecStates, 0, sizeof(nfpSecStates));
	memset(nfpSecRuleDb, 0, dbSize * sizeof(MV_NFP_SEC_RULE *));

	nfp_sec_jhash_iv = mvOsRand();

	return MV_OK;

}



MV_NFP_SEC_RULE *mvNfpSecRuleFind(int family, const MV_U8 *dstL3,
								const MV_U8 *srcL3, MV_U32 proto, MV_U32 spi)
	{

	MV_NFP_SEC_RULE *pPrevRule, *pRule, *tmp;
	MV_U32 hash;

	hash = mv_jhash_2addr(family, dstL3, srcL3, 0, nfp_sec_jhash_iv);
	hash &= (nfpSecRuleDbSize - 1);

	pPrevRule = NULL;
	pRule = nfpSecRuleDb[hash];

	while (pRule) {
		if (!pRule->state->osState) {
			tmp = pRule;

			if (pPrevRule)
				pRule = pPrevRule->next = pRule->next;
			else
				pRule = nfpSecRuleDb[hash] = pRule->next;

			mvNfpSecRuleDelete(tmp);
			continue;
		}

		if (pRule->family == family &&
		    l3_addr_eq(family, pRule->dstL3, dstL3) &&
		    l3_addr_eq(family, pRule->srcL3, srcL3) &&
		    pRule->state->proto == proto &&
		    (pRule->state->spi == spi || spi == 0))
			break;

		pPrevRule = pRule;
		pRule = pRule->next;
	}

	return pRule;
 }

MV_STATUS mvNfpSecRuleInsert(int family, const MV_U8 *dstL3, const MV_U8 *srcL3,
							 MV_NFP_SEC_STATE *state)
	{
	MV_NFP_SEC_RULE	*pRule;
	MV_U32 hash;
	if (mvNfpSecRuleFind(family, dstL3, srcL3, state->proto, state->spi))
		return MV_ALREADY_EXIST;
	pRule = mvOsMalloc(sizeof(MV_NFP_SEC_RULE));
	if (!pRule)
		return MV_NO_RESOURCE;
	pRule->family  = family;
	pRule->state   = state;
	l3_addr_copy(family, pRule->dstL3, dstL3);
	l3_addr_copy(family, pRule->srcL3, srcL3);

	state->refCnt += 1;
	hash = mv_jhash_2addr(family, dstL3, srcL3, 0, nfp_sec_jhash_iv);
	hash &= (nfpSecRuleDbSize-1);
	pRule->next = nfpSecRuleDb[hash];
	nfpSecRuleDb[hash] = pRule;
	return MV_OK;
	}

MV_STATUS mvNfpProcessIn(MV_U32 ifIndex, MV_NFP_SEC_RULE *pAhRule,
						 MV_NFP_SEC_RULE *pEspRule, MV_IP_HEADER *pIpHdr,
						 MV_AH_HEADER *pAhHdr, MV_ESP_HEADER *pEspHdr)
{
	MV_NFP_SEC_INFO *info;
	MV_STATUS status;
	info = mvOsMalloc(sizeof(MV_NFP_SEC_INFO));
	if (!info)
		return MV_NO_RESOURCE;
	info->magic       = NFP_MAGIC;
	info->ifIndex     = ifIndex;
	/* info->pFpStats = pFpStats; */
	info->pAhRule     = pAhRule;
	info->pEspRule    = pEspRule;
	info->pIpHdr      = pIpHdr;
	info->pAhHdr      = pAhHdr;
	info->pEspHdr     = pEspHdr;
	if (pAhRule)
		status = mvNfpSecAhIn(info);
	else if (pEspRule)
		status = mvNfpSecEspIn(info);
	else
		status = MV_BAD_PARAM;
	if (status == MV_OK)
		mvOsFree(info);
	return status;
}

