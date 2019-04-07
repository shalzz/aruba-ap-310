/* nfp/mvNfpSec.h */

#ifndef NFP_MVNFPSEC_H
#define NFP_MVNFPSEC_H

#include "mvCommon.h"
#include "mvTypes.h"
#include "mvOs.h"

#include "cesa/mvCesa.h"
#include "gbe/mvNeta.h"
#include "nfp/mvNfp.h"

#ifdef CONFIG_MV_ETH_NFP_SEC


typedef struct {
	/* Cesa sessions */
	MV_U16			cesaSidIn;
	MV_CESA_OPEN_SESSION	cesaSessionIn;
	MV_U16			cesaSidOut;
	MV_CESA_OPEN_SESSION	cesaSessionOut;
	MV_U8			proto;
	MV_U32			spi;
	MV_U32			outSeqNum;
	MV_VOID			*osState;
	/* Reference count */
	unsigned int		refCnt;
} MV_NFP_SEC_STATE;

typedef struct __MV_NFP_SEC_RULE {
	MV_U32			family;
	MV_U8			dstL3[MV_MAX_L3_ADDR_SIZE];
	MV_U8			srcL3[MV_MAX_L3_ADDR_SIZE];
	MV_NFP_SEC_STATE		*state;
	struct __MV_NFP_SEC_RULE *next;
} MV_NFP_SEC_RULE;

/* TBD: should this define be here ? or in mv_hal/cesa/mvCesa.h */
#define MV_CESA_NFP_RESERVED_SESSIONS	64
#define NFP_HMAC_LENGTH		12
#define NFP_SEC_MAX_STATES (MV_CESA_NFP_RESERVED_SESSIONS / 2)

typedef struct __MV_NFP_SEC_INFO {
	MV_U32			magic;
	MV_CESA_COMMAND		cesaCommand;
	MV_CESA_MBUF		cesaMbuf;

	MV_U32				ifIndex;
	MV_PKT_INFO			*pPkt;
	/* MV_FP_STATS			*pFpStats; */
	MV_NFP_SEC_RULE		*pAhRule;
	MV_NFP_SEC_RULE		*pEspRule;
	MV_IP_HEADER		*pIpHdr;
	MV_AH_HEADER		*pAhHdr;
	MV_ESP_HEADER		*pEspHdr;
	/* MV_STATUS		(*receiver)(struct __MV_NFP_SEC_INFO *); */
	MV_IP_HEADER		ipHdr;
	MV_U8			digest[NFP_HMAC_LENGTH];

	MV_U16			hLen;
	MV_U16			tLen;
	MV_U8			proto;
} MV_NFP_SEC_INFO;

extern MV_STATUS mvNfpSecInit(MV_U32 dbSize);
/*
extern MV_VOID *mvFpSecStateCreate(MV_CESA_CRYPTO_ALG enc, const MV_U8 *encKey,
	MV_U8 encKeyLen, MV_CESA_MAC_MODE mac, const MV_U8 *macKey,
	MV_U8 macKeyLen, MV_U8 proto, MV_U32 spi, MV_U32 outSeqNum,
	MV_VOID *osState);
extern MV_VOID mvFpSecStateMarkDead(MV_FP_SEC_STATE *state);
extern MV_VOID mvFpSecStateDestroy(MV_FP_SEC_STATE *state);
*/
extern MV_NFP_SEC_RULE *mvNfpSecRuleFind(int family, const MV_U8 *dstL3,
	const MV_U8 *srcL3, MV_U32 proto, MV_U32 spi);
extern MV_STATUS mvNfpSecRuleInsert(int family, const MV_U8 *dstL3, const MV_U8 *srcL3,
	MV_NFP_SEC_STATE *state);
/*

extern MV_VOID mvFpSecRuleDelete(MV_FP_SEC_RULE *pRule);
extern MV_U32 mvFpSecGetOutSeqNum(MV_FP_SEC_STATE *state);
extern MV_VOID mvFpSecSetOutSeqNum(MV_FP_SEC_STATE *state, MV_U32 seqNum);

extern MV_STATUS mvFpSecProcessDone(MV_FP_SEC_INFO *info);
extern MV_STATUS mvFpSecProcessIn(MV_U32 ifIndex, MV_PKT_INFO *pPkt,
	MV_FP_STATS *pFpStats, MV_FP_SEC_RULE *pAhRule,
	MV_FP_SEC_RULE *pEspRule, MV_IP_HEADER *pIpHdr,
	MV_AH_HEADER *pAhHdr, MV_ESP_HEADER *pEspHdr);
extern MV_STATUS mvFpSecProcessOut(MV_U32 ifIndex, MV_PKT_INFO *pPkt,
	MV_FP_STATS *pFpStats, MV_FP_SEC_RULE *pAhRule,
	MV_FP_SEC_RULE *pEspRule, MV_IP_HEADER *pIpHdr);

extern MV_VOID mvFpSecDbPrint(MV_VOID);
*/

#endif /* CONFIG_MV_ETH_NFP_SEC */

#endif
