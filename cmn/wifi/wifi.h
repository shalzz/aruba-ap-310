#ifndef WIFI_H
#define WIFI_H

#include <asm/types.h>
#include <linux/version.h>
#ifdef __KERNEL__
#include <linux/if_ether.h>
#endif

#include "../include/aruba-byteorder.h"

#ifndef __KERNEL__
#define BIT(x) (1 << (x))
#endif

#ifndef __KERNEL__
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <byteswap.h>
#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define le_to_host32(n) (n)
#define host_to_le32(n) (n)
#define aruba_be_to_host16(n) bswap_16(n)
#define aruba_host_to_be16(n) bswap_16(n)
#define aruba_be_to_host32(n) bswap_32(n)
#define aruba_host_to_be32(n) bswap_32(n)
#else
#include <byteswap.h>
#define le_to_host16(n) bswap_16(n)
#define host_to_le16(n) bswap_16(n)
#define le_to_host32(n) bswap_32(n)
#define host_to_le32(n) bswap_32(n)
#define aruba_be_to_host16(n) (n)
#define aruba_host_to_be16(n) (n)
#define aruba_be_to_host32(n) (n)
#define aruba_host_to_be32(n) (n)
#endif
#endif

#ifndef WEP_KEY_LEN
#define WEP_KEY_LEN 13
#endif

#ifndef WEP_KEYS
#define WEP_KEYS 4
#endif

#define WIFI_AID_MASK 0x3fffU
#define WIFI_MCAST_MASK ((__u8)0x01)
#define WIFI_IS_MULTICAST(_l2addr) (((__u8*)_l2addr)[0] & WIFI_MCAST_MASK)

#define WIFI_MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define WIFI_MAC_ELTS(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

/* IEEE 802.11 defines */

#define WLAN_FC_PVER (BIT(1) | BIT(0))
#define WLAN_FC_TODS BIT(8)
#define WLAN_FC_FROMDS BIT(9)
#define WLAN_FC_MOREFRAG BIT(10)
#define WLAN_FC_RETRY BIT(11)
#define WLAN_FC_PWRMGT BIT(12)
#define WLAN_FC_MOREDATA BIT(13)
#define WLAN_FC_ISWEP BIT(14)
#define WLAN_FC_ORDER BIT(15)

#define WLAN_FC_GET_TYPE(fc) (((fc) & (BIT(3) | BIT(2))) >> 2)
#define WLAN_FC_GET_STYPE(fc) \
	(((fc) & (BIT(7) | BIT(6) | BIT(5) | BIT(4))) >> 4)

#define WLAN_GET_SEQ_FRAG(seq) ((seq) & (BIT(3) | BIT(2) | BIT(1) | BIT(0)))
#define WLAN_GET_SEQ_SEQ(seq) \
	(((seq) & (~(BIT(3) | BIT(2) | BIT(1) | BIT(0)))) >> 4)

#define WLAN_FC_TYPE_MGMT 0
#define WLAN_FC_TYPE_CTRL 1
#define WLAN_FC_TYPE_DATA 2

/* management */
#define WLAN_FC_STYPE_ASSOC_REQ 0
#define WLAN_FC_STYPE_ASSOC_RESP 1
#define WLAN_FC_STYPE_REASSOC_REQ 2
#define WLAN_FC_STYPE_REASSOC_RESP 3
#define WLAN_FC_STYPE_PROBE_REQ 4
#define WLAN_FC_STYPE_PROBE_RESP 5
#define WLAN_FC_STYPE_BEACON 8
#define WLAN_FC_STYPE_ATIM 9
#define WLAN_FC_STYPE_DISASSOC 10
#define WLAN_FC_STYPE_AUTH 11
#define WLAN_FC_STYPE_DEAUTH 12
#define WLAN_FC_STYPE_ACTION 13

// For SAPCP support
#define WLAN_FC_STYPE_SAPCP 15

#define WLAN_FC_STYPE_SAPCP_HELLO      1
//#define WLAN_FC_STYPE_SAPCP_PROBE      2 deprecated
#define WLAN_FC_STYPE_SAPCP_MTU        3
#define WLAN_FC_STYPE_SAPCP_AGEOUT     4
#define WLAN_FC_STYPE_SAPCP_HEARTBEAT  5
#define WLAN_FC_STYPE_SAPCP_DEAUTH     6
#define WLAN_FC_STYPE_SAPCP_DISASSOC   7
#define WLAN_FC_STYPE_SAPCP_PROBE_RESP 8
#define WLAN_FC_STYPE_SAPCP_TUNNEL_UPDATE 9
#define WLAN_FC_STYPE_SAPCP_LASER_BEAM_ACTIVE 10
#define WLAN_FC_STYPE_SAPCP_CLIENT_IP 11
#define WLAN_FC_STYPE_SAPCP_LASER_BEAM_ACTIVE_V2 12
#define WLAN_FC_STYPE_SAPCP_STS  13   /* AP Statistics - like round trip delay */
#define WLAN_FC_STYPE_SAPCP_TS_ALIVE   14
#define WLAN_FC_STYPE_SAPCP_BAND_STEER_NEW_STA 15
#define WLAN_FC_STYPE_SAPCP_BAND_STEER_INIT_REQUEST 16
#define WLAN_FC_STYPE_SAPCP_CLIENT_DENSITY 17
#define WLAN_FC_STYPE_SAPCP_VLAN        18
#define WLAN_FC_STYPE_SAPCP_VIRT_BCN    19
#define WLAN_FC_STYPE_SAPCP_ARM_NEIGHBOR_REPORT 21 
#define WLAN_FC_STYPE_SAPCP_MIC_ERROR   22
#define WLAN_FC_STYPE_SAPCP_TXFAIL      23
#define WLAN_FC_STYPE_SAPCP_CLEAR_STATE 24
#define WLAN_FC_STYPE_SAPCP_LOW_SNR     25
#define WLAN_FC_STYPE_SAPCP_DOS_STA_REJECTED 26
#ifdef __FAT_AP__
#define WLAN_FC_STYPE_SAPCP_STICKY      27
#define WLAN_FC_STYPE_SAPCP_BAND_STEER  28
#define WLAN_FC_STYPE_SAPCP_BAND_BALANCING 29
#define WLAN_FC_STYPE_SAPCP_CHAN_UTIL   30
#define WLAN_FC_STYPE_SAPCP_CAPABILITY  31
#endif

#ifdef __FAT_AP__
#define WLAN_FC_STYPE_SAPCP_ANYSPOT     32
#else
#define WLAN_FC_STYPE_SAPCP_ANYSPOT     27
#endif

#ifdef __FAT_AP__
#define WLAN_FC_STYPE_SAPCP_CLIENT_SILENT_BACK    33
#define WLAN_FC_STYPE_SAPCP_FASTRECOVERY  34
#endif

#define WLAN_FC_STYPE_SAPCP_MFP_DEAUTH   35
#define WLAN_FC_STYPE_SAPCP_MFP_DISASSOC 36

#define WLAN_SAPCP_CLIENT_DENSITY_SIZE  128

#ifdef __FAT_AP__
#define WLAN_SAPCP_SLB_DEF_INTERVAL 3
#define WLAN_SAPCP_SLB_DEF_REPORT_INTERVAL 30
#define WLAN_SAPCP_SLB_DEF_NB_MATCHING 60
#define WLAN_SAPCP_SLB_DEF_CLIENT_THRESHOLD 30
#define WLAN_SAPCP_SLB_DEF_THRESHOLD 5
#define WLAN_SAPCP_SLB_DEF_ENTRY_AGE 300
#define WLAN_SAPCP_SLB_DEF_STA_ENTRY_AGE 1000
#define WLAN_SAPCP_SLB_MODE_CHANNEL 1
#define WLAN_SAPCP_SLB_MODE_RADIO 2
#define WLAN_SAPCP_SLB_MODE_CHAN_RADIO (WLAN_SAPCP_SLB_MODE_CHANNEL | WLAN_SAPCP_SLB_MODE_RADIO)
#endif

/* control */
#define WLAN_FC_STYPE_BAR 8
#define WLAN_FC_STYPE_BLKACK 9
#define WLAN_FC_STYPE_PSPOLL 10
#define WLAN_FC_STYPE_RTS 11
#define WLAN_FC_STYPE_CTS 12
#define WLAN_FC_STYPE_ACK 13
#define WLAN_FC_STYPE_CFEND 14
#define WLAN_FC_STYPE_CFENDACK 15

/* data */
#define WLAN_FC_STYPE_DATA 0
#define WLAN_FC_STYPE_DATA_CFACK 1
#define WLAN_FC_STYPE_DATA_CFPOLL 2
#define WLAN_FC_STYPE_DATA_CFACKPOLL 3
#define WLAN_FC_STYPE_NULLFUNC 4
#define WLAN_FC_STYPE_CFACK 5
#define WLAN_FC_STYPE_CFPOLL 6
#define WLAN_FC_STYPE_CFACKPOLL 7

#define WLAN_FC_STYPE_QOS_DATA 8
#define WLAN_FC_STYPE_QOS_DATA_CFACK 9
#define WLAN_FC_STYPE_QOS_DATA_CFPOLL 10
#define WLAN_FC_STYPE_QOS_DATA_CFACKPOLL 11
#define WLAN_FC_STYPE_QOS_NULLFUNC 12
#define WLAN_FC_STYPE_QOS_RESERVED_1 13
#define WLAN_FC_STYPE_QOS_CFPOLL 14
#define WLAN_FC_STYPE_QOS_CFACKPOLL 15

/* Authentication algorithms */
#define WLAN_AUTH_OPEN       0 //Open System
#define WLAN_AUTH_SHARED_KEY 1 //Shared Key
#define WLAN_AUTH_FT         2 //fastbss-transition 
#define WLAN_AUTH_SAE        3 //Simultaneous Authentication of Equals (SAE)
// Cisco uses this for LEAP
#define WLAN_AUTH_OTHER 128

#define WLAN_INITIAL_TIMEOUT 30 // seconds of inactivity before node in 802.11-AUTH state timed out.

#define WLAN_AUTH_CHALLENGE_LEN 128

/* WPA3-SAE auth transaction-ids (seq). */
#define WLAN_AUTH_COMMIT 1
#define WLAN_AUTH_CONFIRM 2

#ifndef __KERNEL__
#define WLAN_CAPABILITY_ESS BIT(0)
#define WLAN_CAPABILITY_IBSS BIT(1)
#define WLAN_CAPABILITY_CF_POLLABLE BIT(2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST BIT(3)
#define WLAN_CAPABILITY_PRIVACY BIT(4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE BIT(5)
#define WLAN_CAPABILITY_SPECTRUM_MGMT  BIT(8)

#if 0
// Bit 8 is really used for spectrum mgmt (11h)
#define WLAN_CAPABILITY_COMPRESSION BIT(8) // aruba specific
#endif
#define WLAN_CAPABILITY_SPECTRUM_MGMT BIT(8)
// for 11g
#define WLAN_CAPABILITY_SHORT_SLOT BIT(10)
#define WLAN_CAPABILITY_UAPSD BIT(11)
#define WLAN_CAPABILITY_RRM BIT(12)
#define WLAN_CAPABILOTY_DSS_OFDM BIT(13)
#endif


#define WIFI_BIT_AND(dst, src, bits) \
    ((dst) = ((dst) & ~(bits)) | ((dst) & (src) & (bits)))

#define WIFI_BIT_COPY(dst, src, bits) \
    ((dst) = ((dst) & ~(bits)) | ((src) & (bits)))


/*
 * Bit field macros
 */

#define WIFI_BF_MASK(sym)       ((1 << sym##_BITS) - 1)
#define WIFI_BF_GET(data, sym)  (((data) >> sym##_OFFS) & WIFI_BF_MASK(sym))

#define WIFI_BF_SET(data, sym, val)                             \
    do {                                                        \
        (data) &= ~(WIFI_BF_MASK(sym) << sym##_OFFS);           \
        (data) |= (((val) & WIFI_BF_MASK(sym)) << sym##_OFFS);  \
    } while(0)

/*
 * High-throughput (802.11n) definitions
 */

#define WLAN_HT_MAX_MCS                       76
#define WLAN_HT_MAX_STREAMS                   4
/*
 * We currently only support MCS 0-31.
 */

#define WLAN_HT_MAX_SUPPORTED_MCS             31

/*
 * Basic MCS hardcoded as 0-7
 */

#define WLAN_HT_MAX_BASIC_MCS                 7

#define WLAN_HT_SEC_CHAN_OFFSET               4
// diff between highest and lowest 20mhz channels of a 80mhz channel
#define WLAN_VHT_80MHZ_CHAN_OFFSET            12

// diff between lowest 20mhz channels of two contiguous 80mhz channel segments 
#define WLAN_VHT_80MHZ_SEGMENT_OFFSET         16

#define WLAN_VHT_160MHZ_CHAN_OFFSET            28

/* 
 *Num of MCS supported by the device legacy & 11n/ac/ax 
 */
typedef enum {
    WIFI_MCS_0,
    WIFI_MCS_1,
    WIFI_MCS_2,
    WIFI_MCS_3,
    WIFI_MCS_4,
    WIFI_MCS_5,
    WIFI_MCS_6,
    WIFI_MCS_7,
    WIFI_MCS_8,
    WIFI_MCS_9,
    WIFI_MCS_10,
    WIFI_MCS_11,
    WIFI_NUM_MCS
} wifi_num_mcs_t;

typedef enum {
    WIFI_NSS_1,
    WIFI_NSS_2,
    WIFI_NSS_3,
    WIFI_NSS_4,
    WIFI_NSS_5,
    WIFI_NSS_6,
    WIFI_NSS_7,
    WIFI_NSS_8,
    WIFI_NUM_SS
} wifi_num_ss_t;

typedef enum {
    WIFI_GI_p4,
    WIFI_GI_p8,
    WIFI_GI_1p6,
    WIFI_GI_3p2,
    WIFI_NUM_GI
} wifi_num_gi_t;

typedef enum {
    WIFI_BW_20,
    WIFI_BW_40,
    WIFI_BW_80,
    WIFI_BW_160,
    WIFI_NUM_BW
} wifi_num_bw_t;

#define WLAN_NUM_MCS	WIFI_NUM_MCS 
#define WLAN_NUM_STREAMS WIFI_NUM_SS 
#define WLAN_NUM_GI  WIFI_NUM_GI 
#define WLAN_NUM_BW  WIFI_NUM_BW 

/*
 * HT Capabilities Element
 */

/*
 * HT Capabilities Info
 */

#define WLAN_HT_CAP_INFO_LDPC_CODING          BIT(0)
#define WLAN_HT_CAP_INFO_CHAN_WIDTH_ALLOW_40  BIT(1)
#define WLAN_HT_CAP_INFO_SM_PWR_SAVE_OFFS     2
#define WLAN_HT_CAP_INFO_SM_PWR_SAVE_BITS     2
#define WLAN_HT_CAP_INFO_SM_PWR_SAVE_STATIC   0
#define WLAN_HT_CAP_INFO_SM_PWR_SAVE_DYNAMIC  1
#define WLAN_HT_CAP_INFO_SM_PWR_SAVE_DISABLED 3
#define WLAN_HT_CAP_INFO_GF                   BIT(4)
#define WLAN_HT_CAP_INFO_SHORT_20             BIT(5)
#define WLAN_HT_CAP_INFO_SHORT_40             BIT(6)
#define WLAN_HT_CAP_INFO_TX_STBC              BIT(7)
#define WLAN_HT_CAP_INFO_RX_STBC_OFFS         8
#define WLAN_HT_CAP_INFO_RX_STBC_BITS         2
#define WLAN_HT_CAP_INFO_DEL_BLK_ACK          BIT(10)
#define WLAN_HT_CAP_INFO_MAX_AMSDU_LEN_OFFS   11
#define WLAN_HT_CAP_INFO_MAX_AMSDU_LEN_BITS   1
#define WLAN_HT_CAP_INFO_DSSS_CCK_40          BIT(12)
#define WLAN_HT_CAP_INFO_PSMP                 BIT(13)
#define WLAN_HT_CAP_INFO_INTOL_40             BIT(14)
#define WLAN_HT_CAP_INFO_LSIG_TXOP_PROT       BIT(15)

/*
 * Supported MCS Set Highest Rate
 */

#define WLAN_HT_CAP_MCS_HIGHEST_RATE_OFFS     6
#define WLAN_HT_CAP_MCS_HIGHEST_RATE_BITS     10

/*
 * Supported MCS Set TX Info
 */

#define WLAN_HT_CAP_MCS_TX_SET_DEF            BIT(0)
#define WLAN_HT_CAP_MCS_TX_RX_NEQ             BIT(1)
#define WLAN_HT_CAP_MCS_TX_MAX_STREAMS_OFFS   2
#define WLAN_HT_CAP_MCS_TX_MAX_STREAMS_BITS   2
#define WLAN_HT_CAP_MCS_TX_UNEQ_MOD_SUP       BIT(4)

/*
 * A-MPDU Params
 */

#define WLAN_HT_CAP_AMPDU_MAX_RX_FACT_OFFS    0
#define WLAN_HT_CAP_AMPDU_MAX_RX_FACT_BITS    2
#define WLAN_HT_CAP_AMPDU_MIN_MPDU_SPAC_OFFS  2
#define WLAN_HT_CAP_AMPDU_MIN_MPDU_SPAC_BITS  3

/*
 * HT Extended Capabilities
 */

#define WLAN_HT_CAP_EXT_PCO                  BIT(0)
#define WLAN_HT_CAP_EXT_PCO_TRANS_TIME_OFFS  1
#define WLAN_HT_CAP_EXT_PCO_TRANS_TIME_BITS  2
#define WLAN_HT_CAP_EXT_MCS_FEEDBACK_OFFS    8
#define WLAN_HT_CAP_EXT_MCS_FEEDBACK_BITS    2
#define WLAN_HT_CAP_EXT_HTC                  BIT(10)
#define WLAN_HT_CAP_EXT_RD_RESP              BIT(11)

/*
 * TxBF Capabailities
 * XXX
 */
#define WLAN_VHT_CHECK_MUMIMO_CLIENT(_INFO) \
    ((_INFO) & IEEE80211_VHTCAP_MU_BFORMEE) ? 1 : 0

#define WLAN_VHT_CHECK_MUMIMO_BSS(_INFO) \
    ((_INFO) & IEEE80211_VHTCAP_MU_BFORMER) ? 1 : 0

/*
 * ASEL Capabailities
 * XXX
 */


/*
 * HT Capabilities field values
 */

#define WLAN_HT_CAP_AMSDU_MAX_LEN_3839       0
#define WLAN_HT_CAP_AMSDU_MAX_LEN_7935       1

#define WLAN_HT_CAP_AMPDU_MIN_SPC_NONE       0
#define WLAN_HT_CAP_AMPDU_MIN_SPC_P25US      1
#define WLAN_HT_CAP_AMPDU_MIN_SPC_P5US       2
#define WLAN_HT_CAP_AMPDU_MIN_SPC_1US        3
#define WLAN_HT_CAP_AMPDU_MIN_SPC_2US        4
#define WLAN_HT_CAP_AMPDU_MIN_SPC_4US        5
#define WLAN_HT_CAP_AMPDU_MIN_SPC_8US        6
#define WLAN_HT_CAP_AMPDU_MIN_SPC_16US       7

#define WLAN_HT_CAP_AMPDU_MAX_RX_LEN_8191    0
#define WLAN_HT_CAP_AMPDU_MAX_RX_LEN_16383   1
#define WLAN_HT_CAP_AMPDU_MAX_RX_LEN_32767   2
#define WLAN_HT_CAP_AMPDU_MAX_RX_LEN_65535   3


/*
 * HT Information Element
 */

#define WLAN_HT_INFO1_SEC_CHAN_OFFS         0
#define WLAN_HT_INFO1_SEC_CHAN_BITS         2
#define WLAN_HT_INFO1_CHAN_WIDTH_ALLOW_40   BIT(2)
#define WLAN_HT_INFO1_RIFS                  BIT(3)
#define WLAN_HT_INFO1_PSMP_ONLY             BIT(4)
#define WLAN_HT_INFO1_SERV_INTVL_GRAN_OFFS  5
#define WLAN_HT_INFO1_SERV_INTVL_GRAN_BITS  3

#define WLAN_HT_INFO2_OP_MODE_OFFS          0
#define WLAN_HT_INFO2_OP_MODE_BITS          2
#define WLAN_HT_INFO2_NON_GF_PRESENT        BIT(2)
#define WLAN_HT_INFO2_TX_BURST_LIMIT        BIT(3)
#define WLAN_HT_INFO2_OBSS_NON_HT_PRESENT   BIT(4)

#define WLAN_HT_INFO3_DUAL_BEACON           BIT(6)
#define WLAN_HT_INFO3_DUAL_CTS_PROT         BIT(7)
#define WLAN_HT_INFO3_SEC_BEACON            BIT(8)
#define WLAN_HT_INFO3_LSIG_TXOP_FULL        BIT(9)
#define WLAN_HT_INFO3_PCO_ACTIVE            BIT(10)
#define WLAN_HT_INFO3_PCO_PHASE             BIT(11)


//extoff not defined here take as chan2 of 8080 operation mode.
#define WLAN_HT_INFO_SEC_CHAN_NONE          0
#define WLAN_HT_INFO_SEC_CHAN_ABOVE         1
// '2' is reserved in 11N standard
#define WLAN_HT_INFO_SEC_CHAN_RESERVED      2
#define WLAN_HT_INFO_SEC_CHAN_BELOW         3
#define WLAN_VHT_INFO_80MHZ_CHAN            4
#define WLAN_VHT_INFO_160MHZ_CHAN            5

#define WLAN_VHT80_CENTER_OFFSET 6 // Center of 80M block == (first channel + 6).

/* Status codes */
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_STATUS_SUCCESS 0
#define WLAN_STATUS_UNSPECIFIED_FAILURE 1
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_STATUS_CAPABILITY_UNSUPPORTED 10
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_STATUS_CAPS_UNSUPPORTED WLAN_STATUS_CAPABILITY_UNSUPPORTED
#define WLAN_STATUS_REASSOC_NO_ASSOC 11
#define WLAN_STATUS_ASSOC_DENIED_UNSPEC 12
#define WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG 13
#define WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION 14
#define WLAN_STATUS_CHALLENGE_FAIL 15
#define WLAN_STATUS_AUTH_TIMEOUT 16
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STATION 17
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA \
            WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STATION
#define WLAN_STATUS_ASSOC_DENIED_RATES 18
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
/* 802.11b */
#define WLAN_STATUS_ASSOC_DENIED_NOSHORT 19
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_STATUS_ASSOC_DENIED_NOPBCC 20
#define WLAN_STATUS_ASSOC_DENIED_NOAGILITY 21
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
/* 802.11W */
#define WLAN_STATUS_ASSOC_TRY_LATER 30
#define WLAN_STATUS_ASSOC_MFP_POL_VIOLATION 31
#define WLAN_STATUS_ASSOC_DENIED_INVALID_IE 40

/* 802.11r and WPA3 related */
#define WLAN_STATUS_R0KH_UNREACHABLE              28
#define WLAN_STATUS_FAILED_INVALID_IE             40
#define WLAN_STATUS_INVALID_GRP_CIPHER            41
#define WLAN_STATUS_INVALID_PAIR_CIPHER           42
#define WLAN_STATUS_FAILED_INVALID_AKMP           43
#define WLAN_STATUS_CIPHER_REJECTED               46
#define WLAN_STATUS_FAILED_INVALID_FT_FRM_COUNT   52
#define WLAN_STATUS_FAILED_INVALID_PMKID          53
#define WLAN_STATUS_FAILED_INVALID_MDIE           54
#define WLAN_STATUS_FAILED_INVALID_FTIE           55

/* 802.11u related */
#define WLAN_STATUS_ADV_PROT_NOT_SUPPORTED        59
#define WLAN_STATUS_NO_OUTSTANDING_GAS_REQUEST    60
#define WLAN_STATUS_RESP_REC_FROM_SERVER          61
#define WLAN_STATUS_GAS_TIMEOUT                   62
#define WLAN_STATUS_GAS_QUERY_RESP_TOO_LARGE      63
#define WLAN_STATUS_GAS_SERVER_UNREACHABLE        65
#define WLAN_STATUS_GAS_TRANSMISSION_FAILURE      79
/* 
 * This status code is for assoc_resp
 */
#define WLAN_STATUS_ARUBA_AP_HAS_MAX_STA          76

/* WPA3 */
#define WLAN_STATUS_UNSUPPORTED_GROUP          77

/*
 * Reason codes
 *
 * New codes get added right before WLAN_REASON_ARUBA_SPECIFIC_END, NO EXCEPTIONS.
 * When merging, a tie goes to the released version!  These codes are exported
 * to external systems and need to be kept consistent across releases.
 *
 */
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_REASON_UNSPECIFIED 1
#define WLAN_REASON_PREV_AUTH_NOT_VALID 2
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_REASON_DEAUTHENTICATE_LEAVING 3
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_REASON_DEAUTH_LEAVING WLAN_REASON_DEAUTHENTICATE_LEAVING
#define WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY 4
#define WLAN_REASON_DISASSOC_AP_BUSY 5
#define WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA 6
#define WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA 7
#define WLAN_REASON_DISASSOC_STA_HAS_LEFT 8
#define WLAN_REASON_STA_REQ_ASSOC_WITHOUT_AUTH 9
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */

/*
 * The existing 'stm/wifi_mgmt.h' status_code_message[] definitions interpret
 * values 10 - 21 as the ieee80211.h WLAN_STATUS codes.  This means codes 10-20
 * are not available for reuse and that we already have two duplicate
 * definitions we will have to live with (12 and 13). PLEASE DO NOT ADD NEW
 * DUPLICATES!
 *
 * enum ieee80211_statuscode {
 *   WLAN_STATUS_SUCCESS = 0,
 *   WLAN_STATUS_UNSPECIFIED_FAILURE = 1,
 *   WLAN_STATUS_CAPS_UNSUPPORTED = 10,
 *   WLAN_STATUS_REASSOC_NO_ASSOC = 11,
 *   WLAN_STATUS_ASSOC_DENIED_UNSPEC = 12,
 *   WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG = 13,
 *   WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION = 14,
 *   WLAN_STATUS_CHALLENGE_FAIL = 15,
 *   WLAN_STATUS_AUTH_TIMEOUT = 16,
 *   WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA = 17,
 *   WLAN_STATUS_ASSOC_DENIED_RATES = 18,
 *   * 802.11b *
 *   WLAN_STATUS_ASSOC_DENIED_NOSHORTPREAMBLE = 19,
 *   WLAN_STATUS_ASSOC_DENIED_NOPBCC = 20,
 *   WLAN_STATUS_ASSOC_DENIED_NOAGILITY = 21,
 *   ...
 *   }
 *
 */

#define WLAN_REASON_DISASSOC_DUE_TO_BSS_TRANS_MGMT 12
#define WLAN_REASON_DISASSOC_RXAUTH 13

/*
 * Aruba specific codes
 *
 * Currently we use reason code and status code mapping interchangeably.  This
 * means that new status codes need corresponding and matching reason code
 * definitions here.  More importantly, THE STATUS CODE ENUM VALUE IS DECIDED BY
 * THE NEW REASON CODE ENUM VALUE, which as stated above IS ADDED RIGHT BEFORE
 * WLAN_REASON_ARUBA_SPECIFIC_END.
 *
 * For example, WLAN_STATUS_ARUBA_AP_HAS_MAX_STA defined above for status code
 * was duplicated below but renamed to WLAN_REASON_ARUBA_MAX_CLIENT_ASSOC to 
 * hold the space for 76.
 *
 * Do not forget to also update:
 *
 * 1 - The status_code_message array from 'stm/wifi_mgmt.h'
 * 2 - The proto files at 'java/sc_nbapi/nbapi_helper_proc/schema_new.proto' and
 *     'nbapi/objects/schema.proto'
 */
#define WLAN_REASON_ARUBA_SPECIFIC_START          21
#define WLAN_REASON_ARUBA_CLIENT_MATCH            22
#define WLAN_REASON_ARUBA_STA_BLACKLIST           23
#define WLAN_REASON_ARUBA_TIMERANGE_ACTIVE        24
#define WLAN_REASON_ARUBA_COUNTER_MEASURE_ACTIVE  25
#define WLAN_REASON_ARUBA_INTERNAL_ERROR          26
#define WLAN_REASON_ARUBA_AP_GOING_DOWN           27
#define WLAN_REASON_ARUBA_R0KH_UNREACHABLE        28        /* 11r */
#define WLAN_REASON_ARUBA_USER_ACTION             29
#define WLAN_REASON_ARUBA_ASSOC_FLOOD             30
#define WLAN_REASON_ARUBA_AGEOUT                  31
#define WLAN_REASON_ARUBA_VLAN_NOT_PRESENT        32
#define WLAN_REASON_ARUBA_DENY_BCAST_PROBE        33
#define WLAN_REASON_ARUBA_VOIP_CAC                34
#define WLAN_REASON_ARUBA_AAA_DEAUTH              35

/*
 * Mesh related REASON code only used for association between mesh VAP, 
 * They will not mixed with management frames between normal AP and client. 
 * So these mesh reason code value can reuse reason code of Standard WIFI status code. 
 * These mesh related reason code should never be changed to keep mesh back compatible. 
 */
#define WLAN_REASON_ARUBA_MAX_CHILDREN            35        /* mesh */
#define WLAN_REASON_ARUBA_CLUSTER_MISMATCH        36        /* mesh */
#define WLAN_REASON_ARUBA_STALE_ROUTE             37        /* mesh */
#define WLAN_REASON_ARUBA_SUCCESS_OPEN            38        /* mesh */
#define WLAN_REASON_ARUBA_PREV_AUTH_FAIL          39        /* mesh */

#define WLAN_REASON_ARUBA_INVALID_IE              40        /* 11r */
#define WLAN_REASON_ARUBA_LEGACY_NOT_ALLOWED      42
#define WLAN_REASON_ARUBA_INVALID_AKMP            43        /* 11r */
#define WLAN_REASON_ARUBA_AP_AGEOUT               44
#define WLAN_REASON_ARUBA_DELETE_HOSPITALITY_USER 45
#define WLAN_REASON_ARUBA_DELETE_BRIDGE_USER      46
#define WLAN_REASON_ARUBA_DELETE_DTUNNEL_OR_BRIDGE_USER    47
#define WLAN_REASON_ARUBA_DISCONNECT              48
#define WLAN_REASON_ARUBA_APAE_DISCONNECT         49
#define WLAN_REASON_ARUBA_EAP_CHALLENGE_FAIL      50
#define WLAN_REASON_ARUBA_KEY_PROPAGATION_FAIL    51
#define WLAN_REASON_ARUBA_PTK_CHALLENGE_FAIL      52
#define WLAN_REASON_ARUBA_INVALID_PMKID           53        /* 11r */
#define WLAN_STATUS_ARUBA_INVALID_MDIE            54        /* 11r */
#define WLAN_STATUS_ARUBA_INVALID_FTIE            55        /* 11r */
#define WLAN_REASON_ARUBA_AP_AGEOUT_INTERNAL      56
#define WLAN_REASON_ARUBA_STICKY_CLIENT           57
#define WLAN_REASON_ARUBA_BAND_STEER              58
#define WLAN_REASON_ARUBA_BAND_BALANCE            59
#define WLAN_REASON_ARUBA_AP_AGEOUT_TXFAIL        60
#define WLAN_REASON_ARUBA_AP_CLEAR_STATE          61
#define WLAN_REASON_ARUBA_LOAD_BALANCING          62
/* Don't change order of next 4 macros. STM increments a counter
 * based on the relative order of these 4 macros
 */
#define WLAN_REASON_ARUBA_USER_LICENSE_LIMIT_HIT  63
#define WLAN_REASON_ARUBA_NO_AAA_PROF_FOUND       64
#define WLAN_REASON_ARUBA_DOT1X_SETUP_FAILED      65
#define WLAN_REASON_ARUBA_SUPPLICANT_UP_FAILED    66 
#define WLAN_REASON_ARUBA_SERVER_TERM_ACTION      67
#define WLAN_REASON_ARUBA_ROAMING                 68
#define WLAN_REASON_ARUBA_CELLULAR_HA             69 //cellular handoff assist
#define WLAN_REASON_ARUBA_HA_FAILOVER             70
#define WLAN_REASON_ARUBA_AUTH_STALE_ENTRY        71
#define WLAN_REASON_ARUBA_LOW_SNR                 72
#define WLAN_REASON_ARUBA_STA_UP_TIMEOUT          73
#define WLAN_REASON_ARUBA_APSTM_ASSOC_MISMATCH    74 
#define WLAN_REASON_ARUBA_VHT_STEER               75
#define WLAN_REASON_ARUBA_MAX_CLIENT_ASSOC        76
#define WLAN_REASON_ARUBA_A2C_STA_NOT_FOUND       77
#define WLAN_REASON_ARUBA_A2C_SAP_NOT_FOUND       78
#define WLAN_REASON_ARUBA_INTERNAL_ERROR_AUTH     79
#define WLAN_REASON_ARUBA_VOICE_ROAM              80
#define WLAN_REASON_ARUBA_APSTM_AID_MISMATCH      81
#define WLAN_REASON_ARUBA_APSTM_INVALID_AID       82
#define WLAN_REASON_ARUBA_APSTM_WRONG_DRVR_MGMT   83
#define WLAN_REASON_ARUBA_STM_IE_NOT_FOUND        84
#define WLAN_REASON_ARUBA_VLAN_ASSOC_FOUND        85
#define WLAN_REASON_ARUBA_VLAN_AID_NOTFOUND       86
#define WLAN_REASON_ARUBA_VLAN_BRIDGE_LKUP_RESULT 87
#define WLAN_REASON_ARUBA_VLAN_BRIDGE_LKUP_MISMATCH 88
#define WLAN_REASON_ARUBA_VLAN_NEW_ASSIGNMENT     89
#define WLAN_REASON_ARUBA_MU_STEER                90
#define WLAN_REASON_ARUBA_INVALID_MAC             91
#define WLAN_REASON_ARUBA_UAC_CHANGE              92
#define WLAN_REASON_ARUBA_NO_UAC_IN_BMAP          93
#define WLAN_REASON_ARUBA_STA_UP_WRONG_UAC        94
#define WLAN_REASON_ARUBA_STA_DEL_NOT_SBY_RDY     95
#define WLAN_REASON_ARUBA_AGEOUT_STA_WAIT_ASSOC   96
#define WLAN_REASON_ARUBA_AID_MISMATCH            97
#define WLAN_REASON_ARUBA_AGEOUT_TSTAMP_OLD       98
#define WLAN_REASON_ARUBA_AUTH_STA_UP_FAIL        99
#define WLAN_REASON_ARUBA_AUTH_STA_ACTIVATE_FAIL  100
#define WLAN_REASON_ARUBA_UAC_DOWN                101
#define WLAN_REASON_ARUBA_DORMANT_STA_DEL         102
#define WLAN_REASON_ARUBA_PUTN_DUR_BEING_DNLDED   103
#define WLAN_REASON_ARUBA_PUTN_FAILURE            104
#define WLAN_REASON_ARUBA_MFP_DEC_FAIL            105
/* ADD NEW REASON CODES BEFORE THIS COMMENT AND ADJUST WLAN_REASON_ARUBA_SPECIFIC_END */
#ifdef __FAT_AP__
#define WLAN_REASON_ARUBA_CHAN_UTIL               106
#define WLAN_REASON_ARUBA_CAPABILITY              107
#define WLAN_REASON_ARUBA_CLIENT_SILENT_BACK      108
#define WLAN_REASON_ARUBA_FASTRECOVERY            109
#define WLAN_REASON_ARUBA_SPECIFIC_END            110
#else
#define WLAN_REASON_ARUBA_SPECIFIC_END            106
#endif
#define WLAN_REASON_ARUBA_SPECIFIC_INTERNAL     0xff

#define WLAN_SUB_REASON_AGEOUT_NONE             0
#define WLAN_SUB_REASON_AGEOUT_DATA_INACTIVITY	1
#define WLAN_SUB_REASON_AGEOUT_ASSOC_TIMEOUT	2
#define WLAN_SUB_REASON_AGEOUT_TSTAMP_OLD	3
#define WLAN_SUB_REASON_AGEOUT_UNSPECIFIED	4

/* 802.11 standard reasons to correct mis-numbered proprietary codes OTA. */
#define WLAN_STD_REASON_DOT1X_AUTH_FAILED          23
#define WLAN_STD_REASON_INVALID_PMKID              49

/* Information Element IDs */
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_FH_PARAMS 2
#define WLAN_EID_DS_PARAMS 3
#define WLAN_EID_CF_PARAMS 4
#define WLAN_EID_TIM 5
#define WLAN_EID_IBSS_PARAMS 6
#define WLAN_EID_COUNTRY 7
#define WLAN_EID_CHALLENGE 16
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_EXTENDED_RATES 50
#define WLAN_EID_WPA       221
#define WLAN_EID_WPA2      48
#define WLAN_EID_RSNIE     48
#define WLAN_EID_NONERP 42
#define WLAN_EID_POWER_CONSTRAINT 32
#define WLAN_EID_POWER_CAPABILITY 33
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_TPC_REQUEST 34
#define WLAN_EID_TPC_REPORT 35
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_SUPP_CHANNELS 36
#define WLAN_EID_CSA 37
#define WLAN_EID_MEASUREMENT_REQUEST 38
#define WLAN_EID_MEASUREMENT_REPORT 39
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_QUIET 40
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_HT_CAPABILITIES 45
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_RSN             48
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_MDIE            54
#define WLAN_EID_FTIE            55
#define WLAN_EID_TIE             56
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_RIC_DATA        57
#define WLAN_EID_TIMEOUT_INTERVAL 56
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_EXT_CSA         60
#define WLAN_EID_HT_INFORMATION  61
#define WLAN_EID_SEC_CH_OFF      62
#define WLAN_EID_RRM_CAPABILITIES 70
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_MMIE             76
#define WLAN_EID_BSS_MAX_IDLE_PERIOD 90
#define WLAN_EID_INTERWORKING     107
#define WLAN_EID_QOS_MAP_SET      110
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_EXT_CAPABILITIES 127
#define WLAN_EID_VHT_CAPABILITIES 191
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_VHT_OPERATION    192
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_BW_CSA           194
#define WLAN_EID_BW_CSA_WRAPPER   196
#define WLAN_EID_OPMODENOTIFY     199
#define WLAN_EID_VENDOR_SPECIFIC_ 221
#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
#define WLAN_EID_VENDOR_SPECIFIC WLAN_EID_VENDOR_SPECIFIC_
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */
#define WLAN_EID_EXTENSION        255

/* Extended tags */
#define WLAN_XID_OWE_DHIE         32

#define WLAN_NONERP_PRESENT BIT(0)
#define WLAN_NONERP_USE_PROTECTION BIT(1)
#define WLAN_NONERP_BARKER_PREAMBLE BIT(2)

#define WLAN_LB_MAX_AP_NAME 64

#define WLAN_EID_OPMODENOTIFY_LEN     1
#define WLAN_EID_RRM_CAPABILITIES_LEN 5
#define WLAN_EID_MDIE_LEN             3
#define WLAN_EID_RIC_DATA_LEN         4
#define WLAN_EID_FTIE_LEN_MIN         82
#define WLAN_EID_RSNIE_LEN_MIN        38
#define WLAN_EID_TSPEC_LEN            61
#define WLAN_EID_BSS_MAX_IDLE_PERIOD_LEN 3
#define WLAN_EID_EXT_CAPABILITIES_LEN sizeof(extended_caps_elem)

#define WLAN_EID_EXT_HE_CAPABILITIES  35
#define WLAN_EID_EXT_HE_OPERATION     36

#define WLAN_CSA_RADAR_MPOINT         0xdeadbeef
#define WLAN_CSA_RADAR_MODE           0x40

#define WLAN_ACTION_CAT_SPECTRUM        0
#define WLAN_SPECTRUM_ACTION_CSA        4

#define WLAN_ACTION_CAT_PUBLIC         4
#define WLAN_PUB_ACTION_CHANNEL_SWITCH 4

#define WLAN_ACTION_BLOCKACK_CATEGORY 3   /* BA */
#define WLAN_ACTION_BA_ADDBA_REQUEST  0   /* ADDBA request */
#define WLAN_ACTION_BA_ADDBA_RESPONSE 1   /* ADDBA response */
#define WLAN_ACTION_BA_DELBA          2   /* DELBA */


#define VENDOR_SPECIFIC_ACTION_CATEGORY 127

/* VHT Capabilities: Cap Info */
#define WLAN_VHT_CAP_INFO_CHAN_WIDTH_8080               BIT(2)
#define WLAN_VHT_CAP_INFO_CHAN_WIDTH_8080_AND_160       BIT(3)

/* HE Capabilities */

// HE MAC Capabilities (B0-B31)
#define WLAN_HE_CAP_DYN_FRAG_LEVEL_OFFS  3
#define WLAN_HE_CAP_DYN_FRAG_LEVEL_BITS  2
#define WLAN_HE_CAP_DYN_FRAG_MIN_SZ_OFFS 8
#define WLAN_HE_CAP_DYN_FRAG_MIN_SZ_BITS 2
#define WLAN_HE_CAP_MULTI_TID_AGGR_OFFS  12
#define WLAN_HE_CAP_MULTI_TID_AGGR_BITS  3
#define WLAN_HE_CAP_AMPDU_MAX_LEN_OFFS   27
#define WLAN_HE_CAP_AMPDU_MAX_LEN_BITS   2


// HE PHY Capabilities (B0-B31)
#define WLAN_HE_CAP_CHAN_WIDTH_SET_OFFS  1
#define WLAN_HE_CAP_CHAN_WIDTH_SET_BITS  6
#define WLAN_HE_CAP_PUNCTURE_PREAMBLE_OFFS 8
#define WLAN_HE_CAP_PUNCTURE_PREAMBLE_BITS 4
#define WLAN_HE_CAP_FULL_BW_UL_MUMIMO_OFFS 22
#define WLAN_HE_CAP_FULL_BW_UL_MUMIMO_BITS 1

#define WLAN_HE_CHECK_UL_MUMIMO_CLIENT(he_cap) \
    WIFI_BF_GET(he_cap.he_phy_cap_0_31, WLAN_HE_CAP_FULL_BW_UL_MUMIMO) != 0

#define WLAN_HE_CHECK_PUNC_PREAMBLE_CLIENT(he_cap) \
    WIFI_BF_GET(he_cap.he_phy_cap_0_31, WLAN_HE_CAP_PUNCTURE_PREAMBLE) != 0

/* HE Operation */
#define WLAN_HE_OP_TXOP_RTS_THRESH_OFFS  10
#define WLAN_HE_OP_TXOP_RTS_THRESH_BITS  10

#define RADAR_BLACKOUT_INTERVAL (31 * 60) // 31 Mins.

typedef struct _virt_bcn_elem {
    __u8 mac[6];
    __u8 signal;
    __u8 rssi;
    __u8 noise_floor;
    __u16 rtt; /* Round Trip Time, in unit of nanoseconds, 0 means invalid or not-set */
    __u32 last_packet_time; /* seconds since EPOCH */
} __attribute__ ((packed)) virt_bcn_elem;

#define WLAN_SUBELEM_PMK_R1KH_ID      1  
#define WLAN_SUBELEM_GTK              2 
#define WLAN_SUBELEM_PMK_R0KH_ID      3  
#define WLAN_SUBELEM_IGTK             4

#define IS_WFA_OUI(oui) ((oui[0]==0x50) && (oui[1]==0x6f) && (oui[2]==0x9a))

#ifdef __FAT_AP__
#define WLAN_DOT11R_FIXED_R0KHID      "\xde\xad\xbe\xaf\xde\xad"  
#endif

#if defined(ANUL)
#define WIFI_FROM_DS                  0x0200
#define WIFI_TO_DS                    0x0100
#define WLAN_QOS_TID                  0x0f
#endif

#if !defined(ARUBA_80211AD_MOD) && (((LINUX_VERSION_CODE != KERNEL_VERSION(2,6,35)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4,1,27))) \
    || !defined(__KERNEL__) || defined(CONFIG_ASAP_MOD))
struct ieee80211_hdr {
  __u16 frame_control;
  __u16 duration_id;
  __u8 addr1[6];
  __u8 addr2[6];
  __u8 addr3[6];
  __u16 seq_ctrl;
  /* followed by '__u8 addr4[6];' if ToDS and FromDS is set in data frame
   */
} __attribute__ ((packed));
#endif

#define IEEE80211_HDRLEN (sizeof(struct ieee80211_hdr))
#define WLAN_DTIM_INTVAL_MIN (1)
#define WLAN_DTIM_INTVAL_MAX (255)

typedef struct QoSControl{
  __u8  tid:4,
        eosp:1,
        ackpolicy:2,
        reserved:1;
  __u8  txop:8;

} QOS_CTRL;

#if defined(ANUL)
#define       GRE_PRIO_MASK 0xE000  /* Bits indicating gre-prio in de-tunnel */
#define       GRE_PRIO_POS  13 
/* These are here to prevent an unnecessary fn call. */
#define	PRIO_80211D_NONE	2	/* None = - */
#define	PRIO_80211D_BK		1	/* BK - Background */
#define	PRIO_80211D_BE		0	/* BE - Best-effort */
#define	PRIO_80211D_EE		3	/* EE - Excellent-effort */
#define	PRIO_80211D_CL		4	/* CL - Controlled Load */
#define	PRIO_80211D_VI		5	/* Vi - Video */
#define	PRIO_80211D_VO		6	/* Vo - Voice */

#define WIFI_TID_NUM    7
#define WIFI_TID_MASK   7
extern const int tid_to_8021DP[WIFI_TID_NUM];
static inline int 
anul_tid_to_8021DP(unsigned char ii)
{
    ii &= WIFI_TID_MASK;
    return tid_to_8021DP[ii];
}
/*
 * Was meant to be ANUL, but these dont' work as uint8_t is not
 * defined uniformly
 */
#if 0
/* Three Address format */
struct wifi_qos_frame {
    uint8_t    wifi_fc[2];
    uint8_t    wifi_dur[2];
    uint8_t    wifi_addr1[6];
    uint8_t    wifi_addr2[6];
    uint8_t    wifi_addr3[6];
    uint8_t    wifi_seq[2];
    uint8_t    wifi_qos[2];
} __attribute__ ((packed));

struct wifi_qoscntl {
    uint8_t    wifi_qos[2];
};

struct wifi_frame_addr4 {
    uint8_t    wifi_fc[2];
    uint8_t    wifi_dur[2];
    uint8_t    wifi_addr1[6];
    uint8_t    wifi_addr2[6];
    uint8_t    wifi_addr3[6];
    uint8_t    wifi_seq[2];
    uint8_t    wifi_addr4[6];
} __attribute__ ((packed));


/* Four address QOS Frame */
struct wifi_qosframe_addr4 {
    uint8_t    wifi_fc[2];
    uint8_t    wifi_dur[2];
    uint8_t    wifi_addr1[6];
    uint8_t    wifi_addr2[6];
    uint8_t    wifi_addr3[6];
    uint8_t    wifi_seq[2];
    uint8_t    wifi_addr4[6];
    uint8_t    wifi_qos[2];
} __attribute__ ((packed));


#endif // 0
#endif //ANUL

/*
 * Per GCC 4.4.x guidelines, __attribute__ ((packed))
 * will be ignore for anonymous structures. So, we need
 * to move it ahead of the struct's instance. 
 * For e.g.
 *     struct {
 *         ...
 *     } xyz __attribute__ ((packed));
 * can be replaced as:
 *     struct __attribute__ ((packed)) {
 *         ...
 *     } xyz;
 * or as:
 *     struct {
 *         ...
 *     } __attribute__ ((packed)) xyz;
 *
 * NOTE:
 * Though some of the struct definitions don't
 * require additional __attribute__ ((packed))
 * (as they are already packed), it is safer to
 * include in case they are expanded later.
 */
struct sapcp_payload {
    __u16 type __attribute__ ((packed));
    union {
        struct {
            __u32 timestamp;
            __u32 currtime;
            __u16 hbt_seq_num;
        } __attribute__ ((packed)) heartbeat;
        struct {
            __u32 rtt;
        } __attribute__ ((packed)) sts;
        struct {
            __u16 mtu;
        } __attribute__ ((packed)) mtu;
        struct {
            __u8 mac[6];
            __u8 bssid[6];
#ifdef __FAT_AP__
            __u64 inocts;
            __u64 outocts;
            __u32 inpkts;
            __u32 outpkts;
#endif
            __u32 reason;
        } __attribute__ ((packed)) ageout;
        struct {
            __u32 channel;
            __u8  ap_name[WLAN_LB_MAX_AP_NAME];
            __u32 lms_ip;
            __u32 master_ip;
        } __attribute__ ((packed)) lb;
        struct {
            __u32 ip;
        } __attribute__ ((packed)) client_ip;
        struct {
            __u8  num;
            char  variable[0];
        }band_steer_sta;
        struct {
            __u8  enable;
        }client_balance;
        struct {
            __u8 map[WLAN_SAPCP_CLIENT_DENSITY_SIZE];
        }client_density;
        struct {
            __u8  num;
            __u8  variable[0];
        }virtual_bcn;
        struct {
            __u32 seqno;
            __u8  channel;
            __u8  channel_sec;
            __u8  tx_power;
            __u8  max_ssids;
            __u8  num;
            __u8  variable[0];
        }neighbor_report;
        struct {
            __u8 mac[6];
        }mic;
    }su;
} __attribute__ ((packed));

/*
 * Also, for a struct to be packed, as long as the
 * struct/union/enum-based member field definitions
 * are packed, it is not necessary to "pack" the member
 * fields of primitive data-type since this would be
 * meaning-less. Also, __attribute__ ((packed)) on a 
 * member field of type unpacked struct will have no
 * effect on its packing.
 */
struct ieee8023_mgmt {
    __u8   da[6];
    __u8   sa[6];
    __u16  etype;
    struct sapcp_payload sapcp;
} __attribute__ ((packed));

/*
 * The __attribute__ ((packed)) on a member field
 * instance of unpacked struct will have no
 * effect on the struct's packing though the struct
 * is being asked to be packed.
 */
#if !defined(ARUBA_80211AD_MOD) && (((LINUX_VERSION_CODE != KERNEL_VERSION(2,6,35)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4,1,27))) \
    || !defined(__KERNEL__) || defined(CONFIG_ASAP_MOD))
struct ieee80211_mgmt {
  __u16 frame_control;
  __u16 duration;
  __u8 da[6];
  __u8 sa[6];
  __u8 bssid[6];
  __u16 seq_ctrl;
  union {
    struct {
      __u16 auth_alg;
      __u16 auth_transaction;
      __u16 status_code;
      /* possibly followed by Challenge text */
      __u8 variable[0];
    } __attribute__((packed)) auth;
    struct {
      __u16 reason_code;
    } __attribute__ ((packed)) deauth;
    struct {
      __u16 capab_info;
      __u16 listen_interval;
      /* followed by SSID and Supported rates */
      __u8 variable[0];
    } __attribute__ ((packed))  assoc_req;
    struct {
      __u16 capab_info;
      __u16 status_code;
      __u16 aid;
      /* followed by Supported rates */
      __u8 variable[0];
    } __attribute__ ((packed)) assoc_resp, reassoc_resp;
    struct {
      __u16 capab_info;
      __u16 listen_interval;
      __u8 current_ap[6];
      /* followed by SSID and Supported rates */
      __u8 variable[0];
    } __attribute__ ((packed)) reassoc_req;
    struct {
      __u16 reason_code;
    } disassoc;
    struct {
      __u8 timestamp[8];
      __u16 beacon_int;
      __u16 capab_info;
      /* followed by some of SSID, Supported rates,
       * FH Params, DS Params, CF Params, IBSS Params, TIM */
      __u8 variable[0];
    } __attribute__ ((packed)) beacon;
    struct {
      __u8 variable[0];
    } __attribute__ ((packed)) probe_req;
    struct {
      __u32 time_stamp_low;
      __u32 time_stamp_high;
      __u16 beacon_interval;
      __u16 capab_info;
      __u8 variable[0];
    } __attribute__ ((packed)) probe_resp;
    struct sapcp_payload sapcp;
  } __attribute__ ((packed)) u;
} __attribute__ ((packed));
#endif

struct ieee80211_mgmt_payload {
  union {
    struct {
      __u16 auth_alg;
      __u16 auth_transaction;
      __u16 status_code;
      /* possibly followed by Challenge text */
      __u8 variable[0];
    } __attribute__ ((packed)) auth;
    struct {
      __u16 reason_code;
    } __attribute__ ((packed)) deauth;
    struct {
      __u16 capab_info;
      __u16 listen_interval;
      /* followed by SSID and Supported rates */
      __u8 variable[0];
    } __attribute__ ((packed)) assoc_req;
    struct {
      __u16 capab_info;
      __u16 status_code;
      __u16 aid;
      /* followed by Supported rates */
      __u8 variable[0];
    } __attribute__ ((packed)) assoc_resp, reassoc_resp;
    struct {
      __u16 capab_info;
      __u16 listen_interval;
      __u8 current_ap[6];
      /* followed by SSID and Supported rates */
      __u8 variable[0];
    } __attribute__ ((packed)) reassoc_req;
    struct {
      __u16 reason_code;
    } __attribute__ ((packed)) disassoc;
    struct {
      __u8 timestamp[8];
      __u16 beacon_int;
      __u16 capab_info;
      /* followed by some of SSID, Supported rates,
       * FH Params, DS Params, CF Params, IBSS Params, TIM */
      __u8 variable[0];
    } __attribute__ ((packed)) beacon;
    struct {
      __u8 variable[0];
    } __attribute__ ((packed)) probe_req;
    struct {
      __u32 time_stamp_low;
      __u32 time_stamp_high;
      __u16 beacon_interval;
      __u16 capab_info;
      __u8 variable[0];
    } __attribute__ ((packed)) probe_resp;
    struct {
      __u16 type;
      __u16 len;
      __u16 initial_len;
    } __attribute__ ((packed)) sapcp;
  } __attribute__ ((packed)) u;
} __attribute__ ((packed));

struct ieee80211_action_user {
    __u8   category;
    __u8   action;
} __attribute__ ((packed));

//EID 37: csa
struct ieee80211_csa_body {
    __u8            mode;
    __u8            channel;
    __u8            count;
} __attribute__ ((packed));

struct ieee80211_ie_csa {
    __u8            ie;
    __u8            len;
    __u8            mode;
    __u8            channel;
    __u8            count;
} __attribute__ ((packed));

//EID 60: ext csa
struct ieee80211_extcsa_body {
    __u8            mode;
    __u8            reg;
    __u8            channel;
    __u8            count;
} __attribute__ ((packed));

struct ieee80211_ie_extcsa {
    __u8            ie;
    __u8            len;
    __u8            mode;
    __u8            reg;
    __u8            channel;
    __u8            count;
} __attribute__ ((packed));

#if !defined(__KERNEL__) || defined(CONFIG_ASAP_MOD)
#ifndef KERNEL_SAP
struct ieee80211_pspoll {
    __u16 frame_control;
    __u16 aid;
    __u8 bssid[6];
    __u8 ta[6];
} __attribute__ ((packed));
#else
struct ieee80211_pspoll {
  u16 frame_control;
  u16 aid;
  u8 bssid[6];
  u8 ta[6];
} __attribute__ ((packed));
#endif
#endif


typedef struct _ssc_frame_payload {
    __u8  rate;
    __u8  power;
} ssc_frame_payload;


/*
 * High-throughput (802.11) element structs
 */

#define WLAN_HT_SUP_MCS_RX_MASK_BYTES  10
#define WLAN_HT_SUP_MCS_RSVD_BYTES     3
#define WLAN_HT_BASIC_MCS_BYTES        16

typedef struct {
    __u16 ht_cap_info;
    __u8  ampdu_params;
    __u8  sup_mcs_rx_mask[WLAN_HT_SUP_MCS_RX_MASK_BYTES];
    __u16 sup_mcs_highest_rate;
    __u8  sup_mcs_tx_info;
    __u8  sup_mcs_reserved[WLAN_HT_SUP_MCS_RSVD_BYTES];
    __u16 ht_ext_cap;
    __u32 txbf_cap;
    __u8  asel_cap;
} __attribute__ ((packed)) ieee80211_ht_cap;

typedef struct {
    __u8  primary_chan;
    __u8  info1;
    __u16 info2;
    __u16 info3;
    __u8  basic_mcs[WLAN_HT_BASIC_MCS_BYTES];
} __attribute__ ((packed)) ieee80211_ht_info;

/*
 * 802.11ac VHT Capability IE
 */
typedef struct _ieee80211_vht_cap {
        __u32   vht_cap_info;
        __u16   rx_mcs_map;          /* B0-B15 Max Rx MCS for each SS */
        __u16   rx_high_data_rate;   /* B16-B28 Max Rx data rate,
                                            Note:  B29-B31 reserved */
        __u16   tx_mcs_map;          /* B32-B47 Max Tx MCS for each SS */
        __u16   tx_high_data_rate;   /* B48-B60 Max Tx data rate,
                                            Note: B61-B63 reserved */
} __attribute__ ((packed)) ieee80211_vht_cap;

typedef struct {
    __u32 he_mac_cap_0_31;
    __u8  he_mac_cap_32_39;
    __u32 he_phy_cap_0_31;
    __u32 he_phy_cap_32_63;
    __u8  he_phy_cap_64_71;
} __attribute__ ((packed)) ieee80211_he_cap;

typedef struct _ieee80211_he_mcs {
    __u16 rx_he_mcs_map;
    __u16 tx_he_mcs_map;
} __attribute__ ((packed)) ieee80211_he_mcs;

typedef struct _ieee80211_he_rates {
    __u8 num_rates;
    ieee80211_he_mcs sup_rates[3];
} __attribute__ ((packed)) ieee80211_he_rates;

typedef struct {
    /* Single octet IE */
    __u8 channel_width : 2;
    __u8 reserved : 2;
    __u8 rx_nss : 3;
    __u8 rx_nss_type: 1;
} __attribute__ ((packed)) ieee80211_opmode;

#define WLAN_HT_CAP_READ(p, b)                                          \
    do {                                                                \
        memcpy((p), (b), sizeof(ieee80211_ht_cap));                     \
        (p)->ht_cap_info  = le_to_host16((p)->ht_cap_info);             \
        (p)->sup_mcs_highest_rate = le_to_host16((p)->sup_mcs_highest_rate); \
        (p)->ht_ext_cap   = le_to_host16((p)->ht_ext_cap);              \
        (p)->txbf_cap     = le_to_host32((p)->txbf_cap);                \
    } while (0)

#define WLAN_HT_CAP_WRITE(p, b)                                         \
    do {                                                                \
        ieee80211_ht_cap __o = *(p);                                    \
        __o.ht_cap_info = host_to_le16(__o.ht_cap_info);                \
        __o.sup_mcs_highest_rate = host_to_le16(__o.sup_mcs_highest_rate); \
        __o.ht_ext_cap  = host_to_le16(__o.ht_ext_cap);                 \
        __o.txbf_cap    = host_to_le32(__o.txbf_cap);                   \
        memcpy((b), &__o, sizeof(ieee80211_ht_cap));                    \
    } while (0)

#define WLAN_HT_INFO_READ(p, b)                                         \
    do {                                                                \
        memcpy((p), (b), sizeof(ieee80211_ht_info));                    \
        (p)->info2 = le_to_host16((p)->info2);                          \
        (p)->info3 = le_to_host16((p)->info3);                          \
    } while (0)

#define WLAN_HT_INFO_WRITE(p, b)                                        \
    do {                                                                \
        ieee80211_ht_info __o = *(p);                                   \
        __o.info2 = host_to_le16(__o.info2);                           \
        __o.info3 = host_to_le16(__o.info3);                           \
        memcpy((b), &__o, sizeof(ieee80211_ht_info));                   \
    } while (0)

#define WLAN_HT_MCS_SET(mcs, i)       ((mcs)[(i) >> 3] |= (1 << ((i) & 7)))
#define WLAN_HT_MCS_CLR(mcs, i)       ((mcs)[(i) >> 3] &= ~(1 << ((i) & 7)))
#define WLAN_HT_MCS_TST(mcs, i)       ((mcs)[(i) >> 3] & (1 << ((i) & 7)))


/*
 * Pre-standard vendor-specific HT elements
 */

#define WLAN_VENDOR_SPEC_SIZE   4

#define WLAN_GET_VENDOR_SPEC(p, oui, eid)                               \
    do {                                                                \
        (oui) = ((p)[0] << 16) | ((p)[1] << 8) | (p)[2];                \
        (eid) = (p)[3];                                                 \
    } while (0)

#define WLAN_SET_VENDOR_SPEC(p, oui, eid)                               \
    do {                                                                \
        (p)[0] = ((oui) >> 16) & 0xff;                                  \
        (p)[1] = ((oui) >> 8) & 0xff;                                   \
        (p)[2] = (oui) & 0xff;                                          \
        (p)[3] = (eid);                                                 \
    } while (0)

#define WLAN_HT_VENDOR_SPEC_OUI          0x00904c
#define WLAN_HT_VENDOR_SPEC_EID_HT_CAP   51
#define WLAN_HT_VENDOR_SPEC_EID_HT_INFO  52

// Broadcom uses 0x0408 for vendor-specific g-band turbo-rates (256-QAM)
#define WLAN_HT_VENDOR_SPEC_EID_VHT_CAP   4
#define WLAN_HT_VENDOR_SPEC_EID_VHT_CAP_EXTRA   8

/*
 * Very high-throughput (802.11ac)
 */
enum wlan_max_mpdu_size_en { // Standard values: don't modify.
    WLAN_VHT_MPDU_3895=0,
    WLAN_VHT_MPDU_7991=1,
    WLAN_VHT_MPDU_11454=2
};
enum wlan_max_tx_amsdu_size_en {
    WLAN_TX_AMSDU_DISABLED=0, // Non-VHT only: not set incase of VHT
    WLAN_TX_AMSDU_3839=1,
    WLAN_TX_AMSDU_4096=2, // unused - placeholder.
    WLAN_TX_AMSDU_7935=3,
    WLAN_TX_AMSDU_11398=4,
    WLAN_TX_AMSDU_LAST=0xf // 4bits at most
};


/*
 * Max VHT MCS is 9
 */
#define WLAN_VHT_MAX_MCS                9
#define WLAN_VHT_MIN_MCS                7

/*
 * We currently support spatial streams 1-8.
 */
#define WLAN_VHT_MAX_STREAMS            8

#define WLAN_VHT_TX_MCS_SET(mcs, i)     ((mcs) |= (1 << ((i) & 0xF)))
#define WLAN_VHT_TX_MCS_CLR(mcs, i)     ((mcs) &= ~(1 << ((i) & 0xF)))
#define WLAN_VHT_TX_MCS_TST(mcs, i)     ((mcs) & (1 << ((i) & 0xF)))

#if 1
/*
 * MCS 0-9 for SS=1-8
 */
#define WLAN_VHT_DEFAULT_MCS_MAP        0xFFEA

#define WLAN_VHT_MCS_MAP_NSS1_MASK      0xFFFC
#define WLAN_VHT_MCS_MAP_NSS2_MASK      0xFFF0
#define WLAN_VHT_MCS_MAP_NSS3_MASK      0xFFC0
#define WLAN_VHT_MCS_MAP_NSS4_MASK      0xFF00
#define WLAN_VHT_MCS_MAP_NSS5_MASK      0xFC00
#define WLAN_VHT_MCS_MAP_NSS6_MASK      0xF000
#define WLAN_VHT_MCS_MAP_NSS7_MASK      0xC000
#define WLAN_VHT_MCS_MAP_NSS8_MASK      0x0000

/*  11ac spec states it is mandatory to support MCS 0-7 and NSS=1 */ 
#define WLAN_VHT_BASIC_MCS_MAP        0x0000

/*
 * 2-bit max MCS for a spatial stream represented in VHT IE mcs map.
 */
#define WLAN_VHT_MAX_MCS_4_SS_0_7           0
#define WLAN_VHT_MAX_MCS_4_SS_0_8           1
#define WLAN_VHT_MAX_MCS_4_SS_0_9           2
#define WLAN_VHT_MAX_MCS_4_SS_UNSUPPORTED   3
#endif

// set 2-bit max mcs value r for spatial stream ss
#define WLAN_VHT_MAX_MCS_4_SS_SET(map, r, ss)   \
    ((map) = (((map) & ~(3 << (((ss) - 1) << 1))) | ((3 & (r)) << (((ss) - 1) << 1))))

#define WLAN_VHT_MAX_MCS_4_SS_GET(map, ss)      (((map) >> (((ss) - 1) << 1)) & 3)

/*
 * each bit represents one supported mcs, x-bit represents x-mcs. 
 * for example, 0xff uses 0-7 bits to indicate 0-7 mcs.
 */ 
#define WIFI_VHT_MCS_CODE_TO_MCS_MAP(mcs_code) \
    ((mcs_code == WLAN_VHT_MAX_MCS_4_SS_0_7) ? 0xff : \
     (mcs_code == WLAN_VHT_MAX_MCS_4_SS_0_8) ? 0x1ff : \
     (mcs_code == WLAN_VHT_MAX_MCS_4_SS_0_9) ? 0x3ff : 0) 

#define WLAN_VHT_CAP_READ(p, b)                                             \
    do {                                                                    \
        memcpy((p), (b), sizeof(ieee80211_vhtcap));                        \
        (p)->vht_cap_info           = le_to_host32((p)->vht_cap_info);      \
        (p)->rx_mcs_map         = le_to_host16((p)->rx_mcs_map);            \
        (p)->rx_high_data_rate  = le_to_host16((p)->rx_high_data_rate);     \
        (p)->tx_mcs_map         = le_to_host16((p)->tx_mcs_map);            \
        (p)->tx_high_data_rate  = le_to_host16((p)->tx_high_data_rate);     \
    } while (0)

#define WLAN_VHT_CAP_WRITE(p, b)                                            \
    do {                                                                    \
        ieee80211_vhtcap __o   = *(p);                                     \
        __o.vht_cap_info            = host_to_le32(__o.vht_cap_info);       \
        __o.rx_mcs_map          = host_to_le16(__o.rx_mcs_map);             \
        __o.rx_high_data_rate   = host_to_le16(__o.rx_high_data_rate);      \
        __o.tx_mcs_map          = host_to_le16(__o.tx_mcs_map);             \
        __o.tx_high_data_rate   = host_to_le16(__o.tx_high_data_rate);      \
        memcpy((b), &__o, sizeof(ieee80211_vhtcap));                       \
    } while (0)

#define WLAN_VHT_OP_READ(p, b)                                            \
    do {                                                                    \
        memcpy((p), (b), sizeof(ieee80211_vhtop));                       \
        (p)->vhtop_basic_mcs_set      = le_to_host16((p)->vhtop_basic_mcs_set);         \
    } while (0)

#define WLAN_VHT_OP_WRITE(p, b)                                           \
    do {                                                                    \
        ieee80211_vhtop __o  = *(p);                                     \
        __o.vhtop_basic_mcs_set       = host_to_le16(__o.vhtop_basic_mcs_set);          \
        memcpy((b), &__o, sizeof(ieee80211_vhtop));                      \
    } while (0)

#define WLAN_VHT_OP_READ_NO_MCS(p, b)                                       \
    do {                                                                    \
        memcpy((p), (b), offsetof(ieee80211_vhtop, vhtop_basic_mcs_set));   \
    } while (0)

#define WLAN_HE_CAP_READ(p, b)                                              \
    do {                                                                    \
        memcpy((p), (b), sizeof(ieee80211_hecap));                          \
        (p)->he_mac_cap_0_31  = le_to_host32((p)->he_mac_cap_0_31);         \
        (p)->he_mac_cap_32_47 = le_to_host16((p)->he_mac_cap_32_47);        \
        (p)->he_phy_cap_0_31  = le_to_host32((p)->he_phy_cap_0_31);         \
        (p)->he_phy_cap_32_63 = le_to_host32((p)->he_phy_cap_32_63);        \
        (p)->he_phy_cap_64_79 = le_to_host16((p)->he_phy_cap_64_79);        \
    } while (0)

#define WLAN_HE_MCS_READ(p, b)                                              \
    do {                                                                    \
        int __i = 0;                                                        \
        memcpy((p), (b), sizeof(ieee80211_he_mcs_sup_rates));               \
        for(__i=0; __i<(p)->num_sup_rates; __i++) {                         \
            (p)->sup_rate[__i].rx_he_mcs_map =                              \
                le_to_host16((p)->sup_rate[__i].rx_he_mcs_map);             \
            (p)->sup_rate[__i].tx_he_mcs_map =                              \
                le_to_host16((p)->sup_rate[__i].tx_he_mcs_map);             \
        }                                                                   \
    } while (0)

#define WLAN_HE_CAP_WRITE(p, b)                                             \
    do {                                                                    \
        ieee80211_hecap __o  = *(p);                                        \
        __o.he_mac_cap_0_31  = host_to_le32(__o.he_mac_cap_0_31);           \
        __o.he_mac_cap_32_47 = host_to_le16(__o.he_mac_cap_32_47);          \
        __o.he_phy_cap_0_31  = host_to_le32(__o.he_phy_cap_0_31);           \
        __o.he_phy_cap_32_63 = host_to_le32(__o.he_phy_cap_32_63);          \
        __o.he_phy_cap_64_79 = host_to_le16(__o.he_phy_cap_64_79);          \
        memcpy((b), &__o, sizeof(ieee80211_hecap));                         \
    } while (0)

#define WLAN_HE_MCS_WRITE(p, b)                                             \
    do {                                                                    \
        ieee80211_he_mcs_sup_rates __o = *(p);                              \
        int __i = 0;                                                        \
        for (__i=0; __i<__o.num_sup_rates; __i++) {                         \
            __o.sup_rate[__i].rx_he_mcs_map =                               \
                host_to_le16(__o.sup_rate[__i].rx_he_mcs_map);              \
            __o.sup_rate[__i].tx_he_mcs_map =                               \
                host_to_le16(__o.sup_rate[__i].tx_he_mcs_map);              \
        }                                                                   \
        memcpy((b), &__o, sizeof(ieee80211_he_mcs_sup_rates));              \
    } while (0)

#define WLAN_HE_OP_READ(p, b)                                               \
    do {                                                                    \
        memcpy((p), (b), sizeof(ieee80211_heop));                           \
        (p)->heop_params      = le_to_host32((p)->heop_params);             \
        (p)->basic_he_mcs_set = le_to_host16((p)->basic_he_mcs_set);        \
    } while (0)

#define WLAN_HE_OP_WRITE(p, b)                                              \
    do {                                                                    \
        ieee80211_heop __o = *(p);                                          \
        __o.heop_params      = host_to_le32(__o.heop_params);               \
        __o.basic_he_mcs_set = host_to_le16(__o.basic_he_mcs_set);          \
        memcpy((b), &__o, sizeof(ieee80211_heop));                          \
    } while (0)

#define IEEE80211_VHTOP_CHWIDTH_REVSIG_160    1 /* 160 MHz Operating Channel
                                                   (revised signalling) */
#define IEEE80211_VHTOP_CHWIDTH_REVSIG_80_80  1 /* 80 + 80 MHz Operating Channel
                                                   (revised signalling) */
 /*
  * Check if revised signalling is being used for VHT160 in vhtop
  */
#define IS_REVSIG_VHT160(vhtop) (((vhtop)->vht_op_chwidth == IEEE80211_VHTOP_CHWIDTH_REVSIG_160) && \
        ((vhtop)->vht_op_ch_freq_seg2 != 0) && \
        (abs((vhtop)->vht_op_ch_freq_seg2 - (vhtop)->vht_op_ch_freq_seg1) == 8))

#if !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER)
/*
 * Check if revised signalling is being used for VHT80p80 in vhtop
 */
#define IS_REVSIG_VHT80_80(vhtop) (((vhtop)->vht_op_chwidth == IEEE80211_VHTOP_CHWIDTH_REVSIG_80_80) && \
        ((vhtop)->vht_op_ch_freq_seg2 != 0) && \
        (abs((vhtop)->vht_op_ch_freq_seg2 - (vhtop)->vht_op_ch_freq_seg1) > 8))
#endif /* !defined(ARUBA_AVOID_REDEFINITIONS_IN_WLAN_DRIVER) */

/*
 * high-efficiency (802.11ax)
 */

#define WLAN_HE_MAX_STREAMS    8
#define WLAN_HE_MAX_MCS        11

/*
 * MCS 0-11 for SS=1-8
 */
#define WLAN_HE_MCS_MAP_NSS1_MASK       0xFFFC
#define WLAN_HE_MCS_MAP_NSS2_MASK       0xFFF0
#define WLAN_HE_MCS_MAP_NSS3_MASK       0xFFC0
#define WLAN_HE_MCS_MAP_NSS4_MASK       0xFF00
#define WLAN_HE_MCS_MAP_NSS5_MASK       0xFC00
#define WLAN_HE_MCS_MAP_NSS6_MASK       0xF000
#define WLAN_HE_MCS_MAP_NSS7_MASK       0xC000
#define WLAN_HE_MCS_MAP_NSS8_MASK       0x0000

/*  11ax spec states it is mandatory to support MCS 0-7 and NSS=1, Draft2.0 section 28.5 */ 
#define WLAN_HE_BASIC_MCS_MAP           0xFFFC

/*
 * 2-bit max MCS for a spatial stream represented in VHT IE mcs map.
 */
#define WLAN_HE_MAX_MCS_8_SS_0_7            0
#define WLAN_HE_MAX_MCS_8_SS_0_9            1
#define WLAN_HE_MAX_MCS_8_SS_0_11           2
#define WLAN_HE_MAX_MCS_8_SS_UNSUPPORTED    3

// set 2-bit max mcs value r for spatial stream ss
#define WLAN_HE_MAX_MCS_8_SS_SET(map, r, ss)   \
    ((map) = (((map) & ~(3 << (((ss) - 1) << 1))) | ((3 & (r)) << (((ss) - 1) << 1))))

#define WLAN_HE_MAX_MCS_8_SS_GET(map, ss)      (((map) >> (((ss) - 1) << 1)) & 3)

#define WIFI_HE_MCS_CODE_TO_MCS_MAP(mcs_code) \
    ((mcs_code == WLAN_HE_MAX_MCS_8_SS_0_7) ? 0xff : \
     (mcs_code == WLAN_HE_MAX_MCS_8_SS_0_9) ? 0x3ff : \
     (mcs_code == WLAN_HE_MAX_MCS_8_SS_0_11) ? 0xfff : 0) 

#ifdef __KERNEL__
#include <asm/byteorder.h>
#define IEEE80211_FC(type, stype) cpu_to_le16((type << 2) | (stype << 4))
#else
#define IEEE80211_FC(type, stype) host_to_le16((type << 2) | (stype << 4))
#endif

#if 0
// The HFA384X stuff is all Intersil chipset related
/* HFA384X Information frames */
#define HFA384X_INFO_COMMTALLIES 0xF100
#define HFA384X_INFO_SCANRESULTS 0xF101
#define HFA384X_INFO_CHANNELINFORESULTS 0xF102 /* AP f/w only */
#define HFA384X_INFO_HOSTSCANRESULTS 0xF103
#define HFA384X_INFO_LINKSTATUS 0xF200

#ifndef KERNEL_SAP
enum { HFA384X_LINKSTATUS_CONNECTED = 1,
       HFA384X_LINKSTATUS_DISCONNECTED = 2,
       HFA384X_LINKSTATUS_AP_CHANGE = 3,
       HFA384X_LINKSTATUS_AP_OUT_OF_RANGE = 4,
       HFA384X_LINKSTATUS_AP_IN_RANGE = 5,
       HFA384X_LINKSTATUS_ASSOC_FAILED = 6 };

enum { HFA384X_PORTTYPE_BSS = 1, HFA384X_PORTTYPE_WDS = 2,
       HFA384X_PORTTYPE_PSEUDO_IBSS = 3, HFA384X_PORTTYPE_IBSS = 0,
       HFA384X_PORTTYPE_HOSTAP = 6 };

enum { HFA384X_RX_MSGTYPE_NORMAL = 0, HFA384X_RX_MSGTYPE_RFC1042 = 1,
       HFA384X_RX_MSGTYPE_BRIDGETUNNEL = 2, HFA384X_RX_MSGTYPE_MGMT = 4 };

#endif // KERNEL_SAP

#define HFA384X_RATES_1MBPS BIT(0)
#define HFA384X_RATES_2MBPS BIT(1)
#define HFA384X_RATES_5MBPS BIT(2)
#define HFA384X_RATES_11MBPS BIT(3)

#define HFA384X_WEPFLAGS_PRIVACYINVOKED BIT(0)
#define HFA384X_WEPFLAGS_EXCLUDEUNENCRYPTED BIT(1)
#define HFA384X_WEPFLAGS_HOSTENCRYPT BIT(4)
#define HFA384X_WEPFLAGS_HOSTDECRYPT BIT(7)

#define HFA384X_RX_STATUS_MSGTYPE (BIT(15) | BIT(14) | BIT(13))
#define HFA384X_RX_STATUS_PCF BIT(12)
#define HFA384X_RX_STATUS_MACPORT (BIT(10) | BIT(9) | BIT(8))
#define HFA384X_RX_STATUS_UNDECR BIT(1)
#define HFA384X_RX_STATUS_FCSERR BIT(0)


#define HFA384X_TX_CTRL_ALT_RTRY BIT(5)
#define HFA384X_TX_CTRL_802_11 BIT(3)
#define HFA384X_TX_CTRL_802_3 0
#define HFA384X_TX_CTRL_TX_EX BIT(2)
#define HFA384X_TX_CTRL_TX_OK BIT(1)
#endif // 0

#define ARUBA_80211_FRAME_B 0x8210
#define ARUBA_80211_FRAME_A 0x8220

#define MAX_VIRTUAL_AP_SIZE 16

// Aruba specific info on the beacons

#define AP_NAME_SIZE        64
#define HOST_NAME_SIZE      128
#define USER_NAME_SIZE      248
#define WLAN_EID_ARUBA_INFO  0xab
typedef struct _aruba_assoc_info {
  __u8  oui[3];
  __u8  num_channels;
  __u8  variable; // u8 channel1-n
} __attribute__ ((packed)) aruba_assoc_info;

typedef struct aruba_beacon_info {
    __u8  oui[3];
  union {
    __u32 loc  LOCATION_CODE_DEPRECATED;
    struct {
      __u8 b;
      __u8 f;
      __u16 l;
    } __attribute__ ((packed)) bfl;
  } location LOCATION_CODE_DEPRECATED;
  __u32 lms_address;
} __attribute__ ((packed)) aruba_beacon;

int wifi_aruba_beacon_element(void *cfg, char *start, int max_len);

struct aruba_ie {
	__u8	ie; /* 221 decimal */
	__u8	len;
	__u8	oui[3];	/* 0x00, 0x0b, 0x86 */
	__u8	version;
	__u8	type;
	__u8	sub_type;
	__u8	data[0];
} __attribute__ ((packed));

struct aruba_voip_qos_req {
	__u8	ac;
	__u16	time;
	__u8	reason_code;
} __attribute__ ((packed));

#define ARUBA_MESH_IE_VERSION 0

typedef enum _mesh_state {
    // NBB: for backwards compatibility do not modify order.
    MESH_UNKNOWN=0,           // For backwards compatibility before state embedded in mesh-IE.
    MESH_CONNECTED=1,         // Succeeded in assoc, [auth,] and link to controller verified.
    MESH_UPGRADING=2,         // Connected but awaiting or processing an image upgrade.
    MESH_CONNECT_PENDING=3,   // Anywhere between issue assoc-req and contacting controller
    MESH_SCANNING=4,          // Not connected, periodically scanning permitted channels.
    MESH_CONFIG_PENDING=5,    // Waiting for sapd: got assoc-resp and (optional) auth succeeded.
    MESH_DESCENDANT_UPGRAD=6, // Connected. Have descendant(s) performing an image upgrade.
    MESH_STATE_COUNT
} mesh_state_t;

#ifndef __KERNEL__
  #if __BYTE_ORDER == __LITTLE_ENDIAN
    #define MESH_LITTLE_ENDIAN
  #endif
#else
  #if defined(__LITTLE_ENDIAN)
    #define MESH_LITTLE_ENDIAN
  #endif
#endif

struct aruba_mesh_cmn_bits {
#if !defined(MESH_LITTLE_ENDIAN)
    __u8 mstate:3;           // mesh_state
    __u8 no_more_children:1; // max_children and/or hop_count limit reached
    __u8 encrypted:1;        // encrypted if set.
    __u8 recovery:1;         // Mesh-point is in recovery
    __u8 subtree_full:1;     // Set if a subtree_weight limit reached (if configured).
    __u8 sae_enabled:1;
#else
    __u8 sae_enabled:1;
    __u8 subtree_full:1;     // Set if a subtree_weight limit reached (if configured).
    __u8 recovery:1;         // Mesh-point is in recovery
    __u8 encrypted:1;        // encrypted if set.
    __u8 no_more_children:1; // max_children and/or hop_count limit reached
    __u8 mstate:3;           // mesh_state
#endif
} __attribute__ ((packed));

struct aruba_mesh_cmn {
    __u16 path_cost; // routing metric.
#if !defined(MESH_LITTLE_ENDIAN)
    __u8 legacy_disallowed:1; // Turned on by HT nodes that refuse non-HT children. 
    __u8 reserved1:7;        // require 7-bits - meshd uses for reverse-rssi in probe-resp. 
#else
    __u8 reserved1:7;        // require 7-bits - meshd uses for reverse-rssi in probe-resp. 
    __u8 legacy_disallowed:1; // Turned on by HT nodes that refuse non-HT children. 
#endif
    __u8 node_cost;
    struct aruba_mesh_cmn_bits cbits; 
    __u8 hop_count;
} __attribute__ ((packed));

struct aruba_mesh_bits {
#if !defined(MESH_LITTLE_ENDIAN)
    __u8 reserved:6;
    __u8 brandnew_mesh:1; //0 - legacy mesh, 1 - new AnulMesh;
    __u8 portal_reachability:1;
#else
    __u8 portal_reachability:1;
    __u8 brandnew_mesh:1; //0 - legacy mesh, 1 - new AnulMesh;
    __u8 reserved:6;
#endif
} __attribute__ ((packed));

struct aruba_mesh_beacon_element {
  __u8 version;
  struct aruba_mesh_bits mbits;
  __u8 mesh_portal_id[6];
  struct aruba_mesh_cmn cmn;
  __u8 mesh_id_len;
  __u8 mesh_id[0];
} __attribute__ ((packed));

//This data structure seems not used in mesh.
struct aruba_mesh_probe_request_element {
  __u8 mesh_portal_id[6]; // all zeros for broadcast, specific portal ID
  __u8 mesh_id_len;
  __u8 mesh_id[0];
} __attribute__ ((packed));

struct aruba_mesh_heartbeat_element {
  __u16 subtree_weight;
  __u16 portal_mtu;
  struct aruba_mesh_cmn cmn;
  struct aruba_mesh_bits mbits;
  __u32 seq_no;
} __attribute__ ((packed));

struct aruba_mesh_assoc_resp_element {
  __u32 lms_ip;
  __u16 rmp_mpv;
} __attribute__ ((packed));

struct aruba_mesh_assoc_request_element {
  __u32 seq_no;
} __attribute__ ((packed));

struct aruba_mesh_lms_element {
   __u8 mesh_type;
   __u8 len;
  __u32 lms_ip;
} __attribute__ ((packed));

struct aruba_phy_cap_element {
    __u8 ht_enabled;
    ieee80211_ht_cap ht_cap;
    __u8 vht_enabled; 
    ieee80211_vht_cap vht_cap;
    __u8 he_enabled;
    ieee80211_he_cap he_cap;
    ieee80211_he_rates he_rates;
} __attribute__ ((packed));

struct aruba_ieee80211_csa { 
    __u8 id;
    __u8 len;
    __u8 mode;
    __u8 channel;
    __u8 count;
    __u8 ht_offset;
} __attribute__ ((packed));

struct aruba_stm_ie {
    __u8  id;
    __u8  len;
    __u8  oui[3];
    __u8  version;
    __u16 status;
    __u16  aid;               // assoc-req fills in AID if assigned by driver.
    __u8  aruba_in_drvr_mgmt; // Set if driver responded to last auth/assoc.
    __u32 anul_txnid;         //  set and checked in  ANUL layer.
    struct aruba_phy_cap_element phy;
} __attribute__ ((packed));


struct aruba_arm_ie {
  __u8 tx_eirp;
} __attribute__ ((packed));

typedef struct _wpa_ie {
  __u8  oui[3]; //00-50-F2
  __u8  oui_type; //1
  __u16 version;
  __u8  group_cipher[4]; //00-50-F2-??
  __u16 pairwise_cipher_count;
  __u8  variable[0];
} __attribute__ ((packed)) wpa_ie;


typedef struct _wpa2_ie {
  __u16 version;
  __u8  group_cipher[4]; //00-0F-AC-??
  __u16 pairwise_cipher_count;
  __u8  variable[0];
} __attribute__ ((packed)) wpa2_ie;


/* Extended Capabilities WLAN_EID_EXT_CAPABILITIES 127*/
#ifndef __KERNEL__

typedef struct _extended_caps_elem{
#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 event: 1;
    __u8 s_psmp_sup: 1;
    __u8 svc_ival_gran:1;
    __u8 psmp_sup: 1;
    __u8 wave_ind: 1;
    __u8 ext_chswitch: 1;
    __u8 on_dmd_beacon: 1;
    __u8 bss_coex_mgmt: 1; // B0 octet 0
#else
    __u8 bss_coex_mgmt: 1; // B0 octet 0
    __u8 on_dmd_beacon: 1;
    __u8 ext_chswitch: 1;
    __u8 wave_ind: 1;
    __u8 psmp_sup: 1;
    __u8 svc_ival_gran:1;
    __u8 s_psmp_sup: 1;
    __u8 event: 1;
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 geo_loc: 1;
    __u8 civic_loc: 1;
    __u8 coloc_intf_rpt: 1;
    __u8 proxy_arp: 1;
    __u8 fms: 1;
    __u8 loc_track: 1;
    __u8 mc_diags: 1;
    __u8 diags: 1;         // B0 octet 1
#else
    __u8 diags: 1;         // B0 octet 1
    __u8 mc_diags: 1;
    __u8 loc_track: 1;
    __u8 fms: 1;
    __u8 proxy_arp: 1;
    __u8 coloc_intf_rpt: 1;
    __u8 civic_loc: 1;
    __u8 geo_loc: 1;
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 timing_meas: 1;
    __u8 multi_bssid: 1;
    __u8 ac_sta_cnt: 1;
    __u8 qos_traffic: 1;
    __u8 bss_trans: 1;
    __u8 tim_brdcast: 1;
    __u8 wnm_sleep: 1;
    __u8 tfs: 1;           // B0 octet 2
#else
    __u8 tfs: 1;           // B0 octet 2
    __u8 wnm_sleep: 1;
    __u8 tim_brdcast: 1;
    __u8 bss_trans: 1;
    __u8 qos_traffic: 1;
    __u8 ac_sta_cnt: 1;
    __u8 multi_bssid: 1;
    __u8 timing_meas: 1;
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 iwking: 1; 
    __u8 rsvd3: 3;
    __u8 utc_tsf_off: 1;
    __u8 dms: 1;
    __u8 ssid_list: 1;
    __u8 chan_usage: 1;   // B0 octet 3
#else
    __u8 chan_usage: 1;   // B0 octet 3
    __u8 ssid_list: 1;
    __u8 dms: 1;
    __u8 utc_tsf_off: 1;
    __u8 rsvd3: 3;
    __u8 iwking: 1;
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 tdls_cs_prohibited:1,
         tdls_prohibited:1,
         tdls_support:1,
         mscgf_capability:1,
         rsvd_b4_1:1,
         sspn_interface:1,
         ebr:1,
         qos_map:1;
#else
    __u8 qos_map:1,
         ebr:1,
         sspn_interface:1,         
         rsvd_b4_1:1,
         mscgf_capability:1,
         tdls_support:1,
         tdls_prohibited:1,
         tdls_cs_prohibited:1;
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 rsvd_b5_1:1,           /* bit 47: reserved */
         wnm_notification:1,    /* bit 46: WNM Notification */
         uapsd_coex:1,          /* bit 45: uapsd coexistence */
         id_location:1,         /* bit 44: identifier location */
         serv_int_gran:3,       /* bit 41-43: service interval granularity */
         reject_unadm_frame:1;  /* bit 40: reject unadmitted frame */
#else
    __u8 reject_unadm_frame:1,  /* bit 40: reject unadmitted frame */
         serv_int_gran:3,       /* bit 41-43: service interval granularity */
         id_location:1,         /* bit 44: identifier location */
         uapsd_coex:1,          /* bit 45: uapsd coexistence */
         wnm_notification:1,    /* bit 46: WNM Notification */
         rsvd_b5_1:1;           /* bit 47: reserved */
#endif
    __u8 oct6;

#if __BYTE_ORDER == __BIG_ENDIAN
    __u8 oct7_temp: 1;
    __u8 opmode_notif: 1;
    __u8 oct7_remain: 6; // B0 octet 7
#else
    __u8 oct7_remain: 6; // B0 octet 7
    __u8 opmode_notif: 1;
    __u8 oct7_temp: 1;
#endif

} __attribute__ ((packed)) extended_caps_elem;

typedef struct _extended_caps{
  __u8 eid;
  __u8 length;
  extended_caps_elem caps;
} __attribute__ ((packed)) extended_caps;
#endif
/****************************************
 ******** Network Power Save IE's *******
 ****************************************/
#define WLAN_BSS_DEFAULT_IDLE_PERIOD        60

#ifndef __KERNEL__

struct wlan_bss_idle_options {
#if __BYTE_ORDER == __BIG_ENDIAN
    __u8        rsvd:7,                     /* B1-B7 - Reserved */
                prot_ka_reqd:1;             /* B0 - Protected keep-alive required */
#else
    __u8        prot_ka_reqd:1,             /* B0 - Protected Keep-alive required */
                rsvd:7;                     /* B1-B7 - Reserved */
#endif
} __attribute__ ((packed));


typedef struct _wlan_bss_max_idle_ie {
    __u8                            ie;
    __u8                            len;
    __u16                           max_idle_period;
    struct wlan_bss_idle_options   idle_options;
} __attribute__ ((packed)) wlan_bss_max_idle_ie;
#endif

typedef struct _wifi_action {
    __u8    ia_category;
    __u8    ia_action;
} __attribute__ ((packed)) wifi_action;

typedef struct _wifi_ba_seqctrl {
#if ARUBA_IS_BIG_ENDIAN
    __u16 startseqnum   :   12, /* B4-15  starting sequence number */
              fragnum   :   4;  /* B0-3fragment number */
#else
    __u16     fragnum   :   4,  /* B0-3  fragment number */
            startseqnum :   12; /* B4-15  starting sequence number */
#endif
} __attribute__ ((packed)) wifi_ba_seqctrl;

typedef struct _wifi_ba_parameterset {
#if ARUBA_IS_BIG_ENDIAN
    __u16 buffersize    :   10, /* B6-15  buffer size */
                 tid    :   4,  /* B2-5   TID */
            bapolicy    :   1,  /* B1   block ack policy */
        amsdusupported  :   1;  /* B0   amsdu supported */
#else
    __u16 amsdusupported    :   1,  /* B0   amsdu supported */
                bapolicy    :   1,  /* B1   block ack policy */
                     tid    :   4,  /* B2-5   TID */
              buffersize    :   10; /* B6-15  buffer size */
#endif
} __attribute__ ((packed)) wifi_ba_parameterset;

typedef struct _wifi_delba_parameterset {
#if ARUBA_IS_BIG_ENDIAN
    __u16 tid       :   4,  /* B12-15  tid */
          initiator :   1,  /* B11     initiator */
          reserved0 :   11; /* B0-10   reserved */
#else
    __u16 reserved0 :   11, /* B0-10   reserved */
          initiator :   1,  /* B11     initiator */
          tid       :   4;  /* B12-15  tid */
#endif
} __attribute__ ((packed)) wifi_delba_parameterset;

/* BA - ADDBA request */
typedef struct _wifi_action_ba_addbarequest {
    wifi_action             rq_header;
    __u8                    rq_dialogtoken;
    wifi_ba_parameterset    rq_baparamset;
    __u16                   rq_batimeout;   /* in TUs */
    wifi_ba_seqctrl         rq_basequencectrl;
} __attribute__ ((packed)) wifi_action_ba_addbarequest;

/* BA - ADDBA response */
typedef struct _wifi_action_ba_addbaresponse {
    wifi_action             rs_header;
    __u8                    rs_dialogtoken;
    __u16                   rs_statuscode;
    wifi_ba_parameterset    rs_baparamset;
    __u16                   rs_batimeout; /* in TUs */
} __attribute__ ((packed)) wifi_action_ba_addbaresponse;

/* BA - DELBA */
typedef struct _wifi_action_ba_delba {
    wifi_action                 dl_header;
    wifi_delba_parameterset     dl_delbaparamset;
    __u16                       dl_reasoncode;
} __attribute__ ((packed)) wifi_action_ba_delba;


#define ETH_P_ARUBA_MESH_MGMT_TYPE    0x860b
#define ETH_P_ARUBA_RADIO_BONDING    0x860c
#ifndef ETH_P_EAPOL
#define ETH_P_EAPOL 0x888e
#endif
#define MESH_ECHO_SKB_HEADROOM		40 /* space for .11QoS+CCMP hdr */

#define ARUBA_OUI		0x860b00
#define ARUBA_OUI_VERSION	0x01
#define ARUBA_OUI_TYPE_VOIP_CAC	0x1	/* Alcatel+Spectralink */
#define ARUBA_OUI_TYPE_MESH	0x2
#define ARUBA_OUI_TYPE_INFO	0x3
#define ARUBA_OUI_TYPE_ARM	0x4
#define ARUBA_OUI_TYPE_SLB	0x5
#define ARUBA_OUI_TYPE_SJ_LOOP_PROTECT 0x6

#define ARUBA_OUI_STYPE_AP_NAME	0
#define ARUBA_ELEMID_AP_NAME_LEN_MAX    30
#define LOCATION_OUI    0x042234

#define ARUBA_VOIP_CAC_ADMIT		0x0
#define ARUBA_VOIP_CAC_ADMIT_SUGG	0x1
#define ARUBA_VOIP_CAC_DENY_RSRC	0x8
#define ARUBA_VOIP_CAC_DENY_ADMIN	0x9

#define ARUBA_MESH_TYPE_ECHO_REQUEST    16
#define ARUBA_MESH_TYPE_ECHO_RESPONSE   17
#define ARUBA_MESH_TYPE_LOST_CHILD      18
#define ARUBA_MESH_LMS_CHANGE           19

#define MAX_ARUBA_IE_LEN		(6+4*sizeof(struct aruba_voip_qos_req))

#ifndef LE_READ_4
#define LE_READ_4(p)					\
	((__u32)					\
	 ((((const __u8 *)(p))[0]      ) |		\
	  (((const __u8 *)(p))[1] <<  8) |		\
	  (((const __u8 *)(p))[2] << 16) |		\
	  (((const __u8 *)(p))[3] << 24)))
#endif

static inline int
isarubaoui(const __u8 *frm)
{
	return ((frm[0] == WLAN_EID_VENDOR_SPECIFIC_) && (frm[1] > 3) && (LE_READ_4(frm+2) == ((ARUBA_OUI_VERSION<<24)|ARUBA_OUI)));
}

static inline int
is_aruba_voip_cac_ie(__u8 *ie)
{
	return ((ie[1] > 6) && (ie[6] == ARUBA_OUI_TYPE_VOIP_CAC));
}

static inline int
is_aruba_slb_ie(__u8 *ie)
{
	return ((ie[1] > 6) && (ie[6] == ARUBA_OUI_TYPE_SLB));
}

static inline __u8*
wlan_add_stm_ie(__u8 *frm, __u16 status, __u16  aid,
                __u8  aruba_in_drvr_mgmt, __u32 anul_txnid)
{
    struct aruba_stm_ie *aie = (struct aruba_stm_ie *)frm;

    aie->id = WLAN_EID_VENDOR_SPECIFIC_;
    aie->len = sizeof(struct aruba_stm_ie) - 2;
    aie->oui[0] = ARUBA_OUI & 0xff;         //0x00;
    aie->oui[1] = (ARUBA_OUI >> 8) & 0xff;  //0x0b;
    aie->oui[2] = (ARUBA_OUI >> 16) & 0xff; //0x86;
    aie->version = ARUBA_OUI_VERSION;
    aie->status = status;
    aie->aid = aid;
    aie->aruba_in_drvr_mgmt = aruba_in_drvr_mgmt;
    aie->anul_txnid = anul_txnid;
    frm += (aie->len + 2);

    return frm;
}

static inline __u8 *
aruba_find_stm_ie(__u8 *frm, int len)
{
    __u8 *p, *q, *ie = 0;
    __u8 elem;

    p = frm;
    q = frm + len;
    while ( (p+2) < q ) {
        if ( isarubaoui(p) ) {
            ie = p;
            break;
        }

        elem = *(p + 1);
        p += (elem + 2);
    }
    return ie;
}

#define WIFI_ETHERTYPE_EAP 0x888e
#define WIFI_ETHERTYPE_XSEC 0x88d6
#define WIFI_ETHERTYPE_ARUBA1X    0x88ea
#define MAX_IGTK_BYTES			32
#define WPA2_IGTK_SIZE 16
#define WPA3_SUITEB_IGTK_SIZE 32
#define IGTK_TXKEY_INDEX   4

#define ARUBA_SKB_PRIORITY_SET 0xf000

#include "tunnel_id.h"

// 802.11r related
// MDIE
typedef struct _mobility_domain_elem {
    __u8     eid;
    __u8     len;
    __u16    mob_domain_id;
    __u8     ft_cap;
} __attribute__ ((packed)) mobility_domain_elem;


#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
#define BIT(x) (1 << (x))
#endif

#define WIFI_ENCRYPTION_DISABLED       BIT(0)
#define WIFI_ENCRYPTION_STATIC_WEP     BIT(1)
#define WIFI_ENCRYPTION_DYNAMIC_WEP    BIT(2)
#define WIFI_ENCRYPTION_STATIC_TKIP    BIT(3)
#define WIFI_ENCRYPTION_DYNAMIC_TKIP   BIT(4)
#define WIFI_ENCRYPTION_WPA2_PSK       BIT(5)
#define WIFI_ENCRYPTION_WPA2_8021X     BIT(6)
#define WIFI_ENCRYPTION_WPA2_PREAUTH   BIT(7)
#define WIFI_ENCRYPTION_XSEC           BIT(8)
#define WIFI_ENCRYPTION_WPA_AES_PSK    BIT(9)
#define WIFI_ENCRYPTION_WPA_AES_8021X  BIT(10)
#define WIFI_ENCRYPTION_WPA2_TKIP_PSK  BIT(11)
#define WIFI_ENCRYPTION_WPA2_TKIP_8021X     BIT(12)
#define WIFI_ENCRYPTION_WPA2_AES_GCM_128    BIT(13)
#define WIFI_ENCRYPTION_WPA2_AES_GCM_256    BIT(14)
#define WIFI_ENCRYPTION_WPA2_AES_128_CMAC   BIT(15)
#define WIFI_ENCRYPTION_OWE                 BIT(16) // You may see this referred to as 'Enhanced Open' in the code/display
#define WIFI_ENCRYPTION_SAE                 BIT(17)
#define WIFI_ENCRYPTION_WPA3_AES_GCM_256    BIT(18) // For suiteB, you may see this referred to as 'WPA3 CNSA' in the code/display
#define WIFI_ENCRYPTION_WPA3_AES_CCM_128	BIT(19) // Dummy bit For wpa3 enterprise without suiteB
#define WIFI_ENCRYPTION_WPA3_AES_GMAC_256   BIT(20) // Use for setup IGTK to sapd
#define WIFI_ENCRYPTION_MPSK				BIT(21) // Dummy bit used for MPSK in front-end

#if 0 // not being used right now
#ifdef UC_CROSS
#define WIFI_ENCRYPTION_FT             BIT(16)
#endif
#endif

#define WIFI_ENCRYPTION_WEP     (WIFI_ENCRYPTION_STATIC_WEP | WIFI_ENCRYPTION_DYNAMIC_WEP)
#define WIFI_ENCRYPTION_TKIP    (WIFI_ENCRYPTION_STATIC_TKIP | WIFI_ENCRYPTION_DYNAMIC_TKIP | WIFI_ENCRYPTION_WPA2_TKIP_PSK | WIFI_ENCRYPTION_WPA2_TKIP_8021X)
#define WIFI_ENCRYPTION_AES    (WIFI_ENCRYPTION_WPA_AES_PSK | WIFI_ENCRYPTION_WPA_AES_8021X | WIFI_ENCRYPTION_WPA2_PSK | WIFI_ENCRYPTION_WPA2_8021X | WIFI_ENCRYPTION_OWE | WIFI_ENCRYPTION_SAE | WIFI_ENCRYPTION_WPA3_AES_GCM_256 | WIFI_ENCRYPTION_WPA3_AES_CCM_128)
#define WIFI_ENCRYPTION_WPA2    (WIFI_ENCRYPTION_WPA2_PSK | WIFI_ENCRYPTION_WPA2_8021X | WIFI_ENCRYPTION_WPA2_TKIP_PSK | WIFI_ENCRYPTION_WPA2_TKIP_8021X | WIFI_ENCRYPTION_WPA2_AES_GCM_128 | WIFI_ENCRYPTION_WPA2_AES_GCM_256)
#define WIFI_ENCRYPTION_GCM       (WIFI_ENCRYPTION_WPA2_AES_GCM_128 | WIFI_ENCRYPTION_WPA2_AES_GCM_256 | WIFI_ENCRYPTION_WPA3_AES_GCM_256)
#define WIFI_ENCRYPTION_WPA2_AES  (WIFI_ENCRYPTION_WPA2_PSK | WIFI_ENCRYPTION_WPA2_8021X | WIFI_ENCRYPTION_OWE | WIFI_ENCRYPTION_SAE)
#define WIFI_ENCRYPTION_WPA2_TKIP (WIFI_ENCRYPTION_WPA2_TKIP_PSK | WIFI_ENCRYPTION_WPA2_TKIP_8021X)
#define WIFI_ENCRYPTION_WPA_AES (WIFI_ENCRYPTION_WPA_AES_PSK | WIFI_ENCRYPTION_WPA_AES_8021X)

#define WIFI_ENCRYPTION_WPA_TKIP (WIFI_ENCRYPTION_STATIC_TKIP | WIFI_ENCRYPTION_DYNAMIC_TKIP)
#define WIFI_ENCRYPTION_8021X   (WIFI_ENCRYPTION_DYNAMIC_WEP | WIFI_ENCRYPTION_DYNAMIC_TKIP | WIFI_ENCRYPTION_WPA2_8021X | WIFI_ENCRYPTION_WPA_AES_8021X | WIFI_ENCRYPTION_WPA2_TKIP_8021X | WIFI_ENCRYPTION_WPA3_AES_GCM_256 | WIFI_ENCRYPTION_WPA3_AES_CCM_128)
#define WIFI_ENCRYPTION_8021X_ON_CTRL   (WIFI_ENCRYPTION_DYNAMIC_TKIP | WIFI_ENCRYPTION_WPA2_8021X | WIFI_ENCRYPTION_WPA_AES_8021X | WIFI_ENCRYPTION_WPA2_TKIP_8021X)
#define WIFI_ENCRYPTION_8021X_PSK   (WIFI_ENCRYPTION_STATIC_TKIP | WIFI_ENCRYPTION_WPA2_PSK | WIFI_ENCRYPTION_WPA_AES_PSK | WIFI_ENCRYPTION_WPA2_TKIP_PSK | WIFI_ENCRYPTION_SAE)
#define WIFI_ENCRYPTION_WPA2_ENT    (WIFI_ENCRYPTION_WPA2_8021X | WIFI_ENCRYPTION_WPA2_TKIP_8021X | WIFI_ENCRYPTION_WPA2_AES_GCM_128 | WIFI_ENCRYPTION_WPA2_AES_GCM_256 | WIFI_ENCRYPTION_WPA2_AES_128_CMAC)
#define WIFI_ENCRYPTION_WPA3        (WIFI_ENCRYPTION_OWE | WIFI_ENCRYPTION_SAE | WIFI_ENCRYPTION_WPA3_AES_GCM_256 | WIFI_ENCRYPTION_WPA3_AES_CCM_128)
#define WIFI_ENCRYPTION_WPA3_ENT    (WIFI_ENCRYPTION_WPA3_AES_GCM_256 | WIFI_ENCRYPTION_WPA3_AES_CCM_128)
#define WIFI_ENCRYPTION_STATIC (WIFI_ENCRYPTION_STATIC_WEP | WIFI_ENCRYPTION_STATIC_TKIP | WIFI_ENCRYPTION_WPA_AES_PSK | WIFI_ENCRYPTION_WPA2_PSK | WIFI_ENCRYPTION_WPA2_TKIP_PSK)
#define WIFI_ENCRYPTION_GROUP_MGMT (WIFI_ENCRYPTION_WPA2_AES_128_CMAC | WIFI_ENCRYPTION_WPA3_AES_GMAC_256)

#define WIFI_IS_8021X_ON_CTRL(u_encr_alg)    \
    (u_encr_alg & WIFI_ENCRYPTION_8021X)

/* 802.11W */
#define SAQ_ACTION_CATEGORY	8
#define PROTECTED_DUAL_OF_PUBLIC_ACTION_CATEGORY	9
#define SAQ_ACTION_REQUEST	0
#define SAQ_ACTION_RESPONSE	1
#define SAQ_TRANSID_SZ		2

#define WMM_DSCP_MAP_STRSIZE 64

/* JLEE: START: FCS6.4.3 to SC integration
ARM3.0 Client Management 
typedef enum __client_status {
    ARM_CLIENT_OK, //don't ever change this enum 

    ARM_STICKY_CLIENT,
    ARM_LOAD_BALANCE,
    ARM_BAND_STEER,
    ARM_VHT_STEER,
    ARM_CELLULAR_ASSIST,
    ARM_VOICE_ROAM,
    ARM_USER_ACTION,
    ARM_MU_STEER,
    //add new steer reason here
    ARM_STEER_REASON_CNT,

    //other status start here
    ARM_BAND_BALANCE,
    ARM_TEMP_UNST, //temporary pause steering
    ARM_IOS_UNST, //IOS device rate limit steers
    ARM_NO_STEER, //Used for manually added unsteerable clients
    ARM_IOS_DOT11V_UNST, //IOS device dot11v rate limit steers
    //add new status here
    ARM_STATUS_CNT,
} client_status;

*/
/*  Estimate the client's relative view of two APs (c1 - c2) with
 *  APs Rx signel strength from the client and APs EIRP.
 *  Formula: c1 - c2 = ((-sig1) - (-sig2)) + (eirp1/10 - eirp2/10)
 *  Note that in AOS the signal strength is using absolute value, 
 *  and the eirp is 10x actual eirp value.
 *  Normally, the effective signal eff_sig1 = sig1, and
 *  eff_sig2 is derived from sig1 and the estimated client 
 *  relative view. But if eff_sig2 is too strong, and the absolute eff_diff 
 *  is greater than sig1 itself. Then, to avoid negative value, eff_sig2 = sig2, 
 *  and eff_sig1 is derived from the sig2 and the estimated client relative view.
 */
/*
static inline
int 
est_client_rel_view(__u8 sig1, __s32 eirp1, __u8 sig2, __s32 eirp2, __u8* eff_sig1, __u8* eff_sig2)
{
   int eff_diff = (int)(sig2 - sig1) + (int)(eirp1/10 - eirp2/10);
   
   if (eff_diff < 0 && (-1*eff_diff) > sig1){
        // should rarely happen
        *eff_sig2 = sig2;
        *eff_sig1 = sig2 + (-1*eff_diff);
    } else { 
        *eff_sig1 = sig1;
        *eff_sig2 = sig1 + eff_diff;
    }

   return eff_diff;
}

#define BLIST_SHIFT 7
#define BLIST_TIMEOUT_MASK 0x7f
#define BLIST_IS_PS_DEAUTH 0x01
#define BLIST_IS_BC_PRB_REQ 0x02
#define BLIST_IS_SOURCE    0x80
#define BLIST_IS_TARGET    0x40

#ifdef ARM_CLIENT_STATUS_ENUM
char *client_status_desc[] = {
  "OK",
  "Sticky",
  "Load Balance",
  "Band Steer",
  "VHT Steer",
  "Cellular handoff",
  "Voice Roam",
  "User action",
  "",
  "Band Balance",
  "Temporarily unsteerable",
  "IOS Workaround",
  "No Steer",
  "Temp dot11v unst",
  "",
};
#else
extern char* client_status_desc[];
#endif
JLEE: START: FCS6.4.3 to SC integration */

typedef enum 
{
    ASSIGN_DYNAMIC = 0,
    ASSIGN_STATIC = 1,
} key_assignment_t;
static inline
int mesh_upgrading_state(mesh_state_t mstate)
{
    if ((mstate == MESH_UPGRADING) ||
        (mstate == MESH_DESCENDANT_UPGRAD)) {
        return 1;
    }
    return 0;
}

static inline 
int mesh_connected_state(mesh_state_t mstate)
{
    if ((mstate == MESH_CONNECTED) ||
        (mstate == MESH_UPGRADING) ||
        (mstate == MESH_DESCENDANT_UPGRAD)) {
        return 1;
    }
    return 0;
}

#ifndef __KERNEL__
static inline 
struct _extended_caps *wlan_init_extended_caps(__u8 *frm)
{
        struct _extended_caps *e_caps = (struct _extended_caps *)frm;
        e_caps->eid = WLAN_EID_EXT_CAPABILITIES;
        e_caps->length = WLAN_EID_EXT_CAPABILITIES_LEN;
        return e_caps;
}
#endif

// XXX: check if is_11v/is_11k should set e_caps->caps.bss_trans=1
#define WLAN_SET_EXTENDED_CAPS_IE(is_htsta, is_hsp, is_11k, is_11v, e_caps) do { \
        if (is_htsta) {                                                 \
            e_caps->caps.bss_coex_mgmt = 1;                             \
        }                                                               \
        if (is_hsp) {                                                   \
            e_caps->caps.iwking = 1;                                    \
            e_caps->caps.qos_map = 1;                                   \
            /* hotspot2, phase 2 */                                     \
            e_caps->caps.wnm_notification = 1;                          \
        }                                                               \
        if (is_hsp || is_11v) {                                         \
            e_caps->caps.proxy_arp = 1;                                 \
        }                                                               \
        if (is_11v) {                                                   \
            e_caps->caps.tfs = 1;                                       \
            e_caps->caps.wnm_sleep = 1;                                 \
            e_caps->caps.bss_trans = 1;                                 \
        }                                                               \
    } while(0)

#ifndef __KERNEL__
#define WLAN_ADD_BSS_MAX_IDLE_PERIOD_IE(frm, is_prot) do { \
        wlan_bss_max_idle_ie *bss_max_idle_ie = (wlan_bss_max_idle_ie *)frm; \
        __u8 ie_size = ie_size = sizeof(wlan_bss_max_idle_ie);          \
                                                                        \
        bss_max_idle_ie->ie = WLAN_EID_BSS_MAX_IDLE_PERIOD;             \
        bss_max_idle_ie->len = (ie_size - 2);                           \
        bss_max_idle_ie->max_idle_period = host_to_le16(WLAN_BSS_DEFAULT_IDLE_PERIOD); \
        bss_max_idle_ie->idle_options.prot_ka_reqd = is_prot;           \
        bss_max_idle_ie->idle_options.rsvd = 0;                         \
    } while(0)
#endif
/* wifi_map_status_ota():
 * aruba-specific status-codes are used in internal logic 
 * and have to map to standard status code before sending
 * OTA. Currently only WLAN_STATUS_ARUBA_AP_HAS_MAX_STA is mapped 
 * to WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA 
 */
static inline __u16 wifi_map_status_ota(__u16 reason)
{
    __u16 ret;
    switch (reason) {
    case WLAN_STATUS_ARUBA_AP_HAS_MAX_STA:
        ret = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STATION;
        break;
    default:
        ret = reason;	
    }
    return ret;
}

/* 
 * wifi_map_deauth_ota():
 * Innternally we pass aruba-specific reason-codes and assoc-fail status codes as deauth reasons. That
 * causes issues such as Bug 100849: assoc-status==10 interepreted as deuath-reason==10.
 * Since the internal logic may depend on the status codes, map just before sending OTA.
 * For completeness, and to permit use in user-space, AP_AGEOUT_INTERNAL is mapped to SPECIFIC_INTERNAL
 * but neither of those should ever go over the air.
 */
static inline __u16 wifi_map_deauth_ota(__u16 reason)
{
    __u16 ret;

    /* Don't want to remap valid reasons such as CLASS2_FRAME_FROM_NONAUTH_STA.
     * WLAN_STATUS_UNSPECIFIED_FAILURE == WLAN_REASON_UNSPECIFIED == 1.
     * Other assoc status codes used in the code are >= 10 (WLAN_STATUS_CAPS_UNSUPPORTED)
     * All aruba specific reasons are > WLAN_REASON_ARUBA_SPECIFIC_START(21).
     * Default mapping is to WLAN_REASON_DEAUTH_LEAVING(3).
     */

    if (reason < WLAN_STATUS_CAPABILITY_UNSUPPORTED) {
        ret = reason;
    } else if (reason == WLAN_REASON_ARUBA_INVALID_PMKID) {
        ret = WLAN_STD_REASON_INVALID_PMKID;
    } else if (reason == WLAN_REASON_ARUBA_EAP_CHALLENGE_FAIL) {
        ret = WLAN_STD_REASON_DOT1X_AUTH_FAILED;
    } else  {
        switch (reason) {
        case WLAN_REASON_ARUBA_AP_AGEOUT_INTERNAL:
        case WLAN_REASON_ARUBA_SPECIFIC_INTERNAL:
        case WLAN_REASON_ARUBA_ROAMING:
            ret = WLAN_REASON_ARUBA_SPECIFIC_INTERNAL;
            break;
        default:
            ret = WLAN_REASON_DEAUTHENTICATE_LEAVING;
            break;
        }
    }

#if 0
#ifdef __KERNEL__
    if (reason != ret) {
        printk(KERN_DEBUG "%s: remapped status/aruba-reason:%d to reason:%d", 
               __func__, reason, ret);
    }
#endif
#endif

    return ret;
}

// RSSI Frame Type bitmaps for ids profile
#define AM_RSSI_BLOCK_ACKS       BIT(0)
#define AM_RSSI_PROBE_REQ        BIT(1)
#define AM_RSSI_LOW_RATE         BIT(2)
#define AM_RSSI_HIGH_RATE        BIT(3)
#define AM_RSSI_NULL_DATA        BIT(4)
#define AM_RSSI_MGMT             BIT(5)
#define AM_RSSI_CTRL             BIT(6)
#define AM_RSSI_ALL              BIT(7)
#define AM_RSSI_ALL_SEL          0x7f

// RSSI to est throughput logic for traffic steering
// To be improved
#define SNR_MIN 18
#define GOOD_TPUT 100000
#define BAD_TPUT 24000
#define SNR_TO_TPUT(x) \
    (((x) <= SNR_MIN) ? BAD_TPUT : GOOD_TPUT) 
#define GET_EST_TPUT(snr, rate) \
    (((snr) > 25) ?             \
    ((snr) * (rate) * 80/6) :   \
    (((snr) > 20) ?             \
    ((snr) * (rate) * 75/6) :   \
    (((snr) > 15) ?             \
    ((snr) * (rate) * 70/6) :   \
    (((snr) > 10) ?             \
    ((snr) * (rate) * 65/6) :   \
    ((snr) * (rate) * 10)))))

//Generic bitmap operations
#ifndef NBBY
#define NBBY 8
#endif /* NBBY */
#define SETBIT(x, pos)    ((x)[(pos)/NBBY] |= 1<<((pos)%NBBY))
#define CLEARBIT(x, pos)  ((x)[(pos)/NBBY] &= ~(1<<(pos)%NBBY))
#define CHECKBIT(x, pos)  ((x)[(pos)/NBBY] & (1<<((pos)%NBBY)))

#ifdef __KERNEL__
#define CHK_COPY_TO_USER(user_buf, len, size, tmpbuf, tmpsz, fmt, args...) \
{ \
    int l; \
    int n; \
    snprintf(tmpbuf, tmpsz, fmt, ## args) ; \
    l = strlen(tmpbuf) ; \
    if ((len) + l + 1 >= size) goto toobig ; \
    n = copy_to_user(user_buf, tmpbuf, l+1);     \
    (len) += l; \
    (user_buf) += l; \
}
#endif
typedef enum _chan_bw_type {
    CHAN_BW_20MHZ,
    CHAN_BW_40MHZ,
    CHAN_BW_80MHZ,
    CHAN_BW_160MHZ,
    CHAN_BW_INVALID,
    CHAN_BW_TYPE_COUNT,
} chan_bw_type;

typedef enum {
    SAPM_PRELOAD_ADD_KEY_AP_NAME,
    SAPM_PRELOAD_ADD_KEY_AP_MAC,   /*will be used in the future*/
} sapm_preload_key_t;

typedef enum {
    IMAGE_PRELOAD_INIT = 0,  
    IMAGE_PRELOAD_SUCC = 1,
    IMAGE_PRELOAD_NO_MEM,  
    IMAGE_PRELOAD_OTHER_TFTP_FTP_ERR, 
    IMAGE_PRELOAD_INCOMPATIBLE_AP,  
    IMAGE_PRELOAD_NO_IMAGE, 
    IMAGE_PRELOAD_NO_PROCESS, 
    IMAGE_PRELOAD_IMG_HEAD_ERR, 
    IMAGE_PRELOAD_AP_STATE_ERR, 
    IMAGE_PRELOAD_ACTIVATE_ERR, 
    IMAGE_PRELOAD_NOT_SUPPORT, 
    IMAGE_PRELOAD_UNSPE_ERR, 
    IMAGE_PRELOAD_STATUS_MAX, 
} cluster_upg_preload_status_t;

#endif /* WIFI_H */
