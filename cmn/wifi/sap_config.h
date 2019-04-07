/* ------------------------------------------------------------------------- 
 * Name        : sap_config.h
 * Description : 
 * ------------------------------------------------------------------------- */

#ifndef SAP_CONFIG_H
#define SAP_CONFIG_H
// public section
#include <asm/types.h>
#include <wifi/wifi.h>
#include <wifi/hs2.h> 

typedef enum _wifi_card_type {
  NO_CARD,
  INTERSIL,
  ATHEROS_A,
  ATHEROS_BG,
  ATHEROS_A_AND_BG,
  AR5212_BASE = 8,
  AR5212_A,
  AR5212_BG,
  AR5212_ABG,
  MADWIFI = 16,
  BRCM = 17,
  CARD_TYPE_END
} wifi_card_type;

#define IS_AR5212(x) (((x) == ATHEROS_BG) || (((x) & ~3) == AR5212_BASE))

#define MAX_BSSIDS 8
#define IS_BASE_BSSID(bssid) (((bssid[5] & (__u8)(MAX_BSSIDS - 1)) == 0))

#define WIFI_PROC_ITF_TYPE        "itf_type"
#define WIFI_PROC_TUNNEL_ID       "tunnel_id"
#define WIFI_PROC_MTU             "mtu"
#define WIFI_PROC_RAPMTU          "rap_mtu"
#define WIFI_PROC_DSCP            "dscp"
#define WIFI_PROC_HEARTBEAT       "heartbeat"
#define WIFI_PROC_AGEOUT          "ageout"
#define WIFI_PROC_CALIBRATE       "calibrate"
#define WIFI_PROC_WALKABOUT       "walkabout"
#define WIFI_PROC_CHANNEL         "channel"
#define WIFI_PROC_MODE            "mode"
#define WIFI_PROC_TXPOWER         "txpower"
#define WIFI_PROC_SENSITIVITY     "sensitivity"
#define WIFI_PROC_ESSID           "essid"
#define WIFI_PROC_BSSID           "bssid"
#define WIFI_PROC_FRAG_THRESHOLD  "frag_threshold"
#define WIFI_PROC_RTS_THRESHOLD   "rts_threshold"
#define WIFI_PROC_RETRY           "retry"
#define WIFI_PROC_SHORT_PREAMBLE  "short_preamble"
#define WIFI_PROC_BEACON_INTERVAL "beacon_interval"
#define WIFI_PROC_BEACON_REGULATE "beacon_regulate"
#define WIFI_PROC_POWER_MGMT      "power_mgmt"
#define WIFI_PROC_RATES           "rates"
#define WIFI_PROC_TXRATES         "txrates"
#define WIFI_PROC_LOAD_BALANCE    "load_balance"
#define WIFI_PROC_DTIM_PERIOD     "dtim_period"
#define WIFI_PROC_LMS_ADDRESS     "lms_address"
#define WIFI_PROC_AM_ACK          "am_ack"
#define WIFI_PROC_ENCRYPTION      "encryption"
#define WIFI_PROC_STATUS          "status"
#define WIFI_PROC_DEV_LED         "dev_led"
#define WIFI_PROC_AP_LED          "ap_led"
#define WIFI_PROC_LOCATION        "location"
#define WIFI_PROC_HIDE_SSID       "hide_ssid"
#define WIFI_PROC_DENY_BCAST      "deny_bcast"
#define WIFI_PROC_B_G_MODE        "b_g_mode"
#define WIFI_PROC_TX_PKTS_MODE    "tx_pkts_mode"
#define WIFI_PROC_LINK_MON_DATA_MODE   "link_mon_data_mode"
#define WIFI_PROC_LINK_MON_MGMT_MODE   "link_mon_mgmt_mode"
#define WIFI_PROC_REGDOMAIN       "reg_domain"
#define WIFI_PROC_COUNTRY_CODE    "country"
#define WIFI_PROC_CWMIN           "cwmin"
#define WIFI_PROC_CAPTURE         "capture"
#define WIFI_PROC_RESET_STATS     "if_stats"
#define WIFI_PROC_MRR     	      "mrr"
#define WIFI_PROC_UPSD     	      "upsd"
#define WIFI_PROC_WMM     	      "wmm"
#define WIFI_PROC_WMM_UAPSD		  "wmm_uapsd"
#define WIFI_PROC_STRICT_SVP	  "strict_svp"
#define WIFI_PROC_WMM_NO_ACK      "wmm_no_ack"
// individual procs defined below in addition to generic no-ack
// proc per Andy's request, makes SAPM less complicated
#define WIFI_PROC_WMM_BE_NO_ACK   "wmm_be_no_ack"
#define WIFI_PROC_WMM_BK_NO_ACK   "wmm_bk_no_ack"
#define WIFI_PROC_WMM_VI_NO_ACK   "wmm_vi_no_ack"
#define WIFI_PROC_WMM_VO_NO_ACK   "wmm_vo_no_ack"
#define WIFI_PROC_WMM_TS_MIN_INACT_INT  "wmm_ts_min_inact_int"


#define WIFI_PROC_ANTENNA      	"antenna"
#define WIFI_PROC_INT_ANTENNA   "int_ant_mode"
#define WIFI_PROC_READ_REG     	"rd"
#define WIFI_PROC_WRITE_REG    	"wr"
#define WIFI_PROC_SCAN          "scan"
#define WIFI_PROC_TIMESTAMP    	"timestamp"
#define WIFI_PROC_B2BTX         "back_to_back_tx"
#define WIFI_PROC_JAMTX         "jam_tx"
#define WIFI_PROC_COL_STATS    	"collect_stats"
#define WIFI_PROC_RESET_ITF     "reset"
#define WIFI_PROC_AP_SILENT     "ap_silent"
#define WIFI_PROC_VLAN          "vlan"
#define WIFI_PROC_NATIVE_VLAN   "native_vlan"


#define WIFI_PROC_NOISE_FLOOR  "noise_floor"
#define WIFI_PROC_LOCAL_PROBE_RESP "probe_resp"
#define WIFI_PROC_LOCAL_PROBE_REQ_THRESH "probe_req_thresh"
#define WIFI_PROC_AUTH_REQ_THRESH "auth_req_thresh"

#define WIFI_PROC_ANTENNA_GAIN_24 "ant_gain_24"
#define WIFI_PROC_ANTENNA_GAIN_5  "ant_gain_5"

#define WIFI_PROC_FORWARD_MODE "forward_mode"
#define WIFI_PROC_WIRELESS_MODE "wireless_mode"
#define WIFI_PROC_NUM_PACKETS   "num_packets"

#define WIFI_PROC_TX_FAIL_LIMIT   "tx_fail_limit"
#define WIFI_PROC_WPS		"wps" // Windows Provisioning
#define WIFI_PROC_VOICE_AWARE_SCAN "voice_aware_scan"
#define WIFI_PROC_PS_AWARE_SCAN "ps_aware_scan"
#define WIFI_PROC_PERF_BOOST "perf_boost"

#define WIFI_PROC_TX_QUEUE_LENGTH "tx_queue_length"
#define WIFI_PROC_BATTERY_BOOST "battery_boost"

enum _dev_led_state {
#ifdef __FAT_AP__
  WIFI_DEV_LED_OFF, WIFI_DEV_LED_ON, WIFI_DEV_LED_BLINK, WIFI_DEV_LED_DUAL5G, WIFI_DEV_LED_ON_AMBER
#else
  WIFI_DEV_LED_OFF, WIFI_DEV_LED_ON, WIFI_DEV_LED_BLINK, WIFI_DEV_LED_DUAL5G
#endif
};

enum _ap_led_state {
  WIFI_AP_LED_OFF, WIFI_AP_LED_ON, WIFI_AP_LED_ERROR, WIFI_AP_LED_BLINK,
  WIFI_AP_LED_ERROR_BLINK
};

typedef enum _sap_wifi_mode {
    WIFI_MODE_MASTER, WIFI_MODE_ADHOC, WIFI_MODE_MONITOR, WIFI_MODE_DISABLED,
    WIFI_MODE_SPECTRUM
} sap_wifi_mode;

typedef enum {
    SAP_GLOBAL_LED_MODE_NORMAL,
    SAP_GLOBAL_LED_MODE_OFF,
    SAP_GLOBAL_LED_MODE_ON
} sap_ap_global_led_mode_t;
 
#define WIFI_ESSID_SIZE       32
#define WIFI_BSSID_SIZE       6

#ifndef MAX_COUNTRY_NAME
#define MAX_COUNTRY_NAME 20
#endif


#define MAX_SAP_CONFIG_COMMAND_SIZE   1024


#define WIFI_RATE_1MBPS       BIT(0)
#define WIFI_RATE_2MBPS       BIT(1)
#define WIFI_RATE_5MBPS       BIT(2)
#define WIFI_RATE_11MBPS      BIT(3)
#define WIFI_RATE_6MBPS       BIT(4)
#define WIFI_RATE_9MBPS       BIT(5)
#define WIFI_RATE_12MBPS      BIT(6)
#define WIFI_RATE_18MBPS      BIT(7)
#define WIFI_RATE_24MBPS      BIT(8)
#define WIFI_RATE_36MBPS      BIT(9)
#define WIFI_RATE_48MBPS      BIT(10)
#define WIFI_RATE_54MBPS      BIT(11)

#define WIFI_B_ONLY_RATES    (WIFI_RATE_1MBPS \
			      | WIFI_RATE_2MBPS \
			      | WIFI_RATE_5MBPS \
			      | WIFI_RATE_11MBPS)

#define WIFI_G_RATES    	(WIFI_RATE_6MBPS \
			      | WIFI_RATE_9MBPS \
			      | WIFI_RATE_12MBPS \
			      | WIFI_RATE_18MBPS \
			      | WIFI_RATE_24MBPS \
			      | WIFI_RATE_36MBPS \
			      | WIFI_RATE_48MBPS \
			      | WIFI_RATE_54MBPS)
#define ARUBA_BEACON_ELEM_ID 0xab
#define WIFI_SSN_ELEM_ID 0xdd
#define WIFI_SSN_VERSION 1

#define WIFI_CALIBRATION_START    1
#define WIFI_CALIBRATION_END      0

typedef struct _sap_config {
    __u32          calibrate; // 0 means not in progress, 1 means in progress
    __u8           walkabout; // 0 means not in progress, 1 means in progress
    __u8           channel;  // 1-11 for .b 1-12 for .a
    sap_wifi_mode  mode;
    __u8           txpower; // in dbm
    __s32          sensitivity; // in dbm
    __u8           essid[WIFI_ESSID_SIZE+1];
    __u8           bssid[WIFI_BSSID_SIZE];
    __u16          frag_threshold;
    __u16          rts_threshold;
    __u8           retry;
    __u8           short_preamble;
    __u16          beacon_interval;
    __u8           power_mgmt;
    __u8           load_balance;
    __u16          rates; // bit mask of supported rates
    __u16          txrates; // bit mask of supported rates
    __u16          dtim_period;
    __u32          lms_address; // ip in NBO
    __u16          encryption;
    __u16          max_clients;
    __u8           am_ack;
    __u8           status;
    __u16          ageout; // in seconds
    __u32          mtu;
  union {
    __u32          loc;
    struct {
      __u8         b;
      __u8         f;
      __u16        l;
    } __attribute__ ((packed)) bfl;
  } location LOCATION_CODE_DEPRECATED;
  __u8             hide_ssid;
  __u8             deny_bcast;
  __u8             b_g_mode;
  wifi_card_type   card_type;
  __u8             tx_pkts_mode;
  __u32            regDomain;
  __u8             countryCode[MAX_COUNTRY_NAME]; // isoName
  __u8             cwmin;
  __u8             if_stats;
  __s16            noise_floor;
  __u8             local_probe_resp;
  __u8 		   collect_stats;
  __s8             ant_gain_24;
  __s8             ant_gain_5;
  __u8             forward_mode;
  __u8             apSilent;
  int              tx_fail_limit;
  int		   wps;
  int              voice_aware_scan;
  int              tx_queue_length;
  __u16            vlan;
  __u16            native_vlan;
  int              ps_aware_scan;
  __u8             mrr; // multi rate retry
  __u8             upsd;
  __u8             wmm;
  __u8             wmm_be_no_ack;
  __u8             wmm_bk_no_ack;
  __u8             wmm_vi_no_ack;
  __u8             wmm_vo_no_ack;
}sap_config;

#if 0
typedef enum _bss_fw_mode {
    FORWARD_TUNNEL_ENCRYPTED,  // cli value - tunnel
    FORWARD_BRIDGE,            // cli value - bridge
    FORWARD_TUNNEL_DECRYPTED   // cli value - decrypt-tunnel
} bss_fw_mode;
#endif

enum {
  B_G_MIXED,
  B_ONLY,
  G_ONLY
};

struct ssn_beacon_info {
  __u8 elem_id;  // 0xdd
  __u8 elem_len;
  __u8 oui[3];   // 00:50:f2
  __u8 oui_type;
  __u16 version; // 01:00
  __u8 mcast_suite[4];
  __u8 variable[0];
  // 2 bytes of unicast_suite count, 
  // followed by 4*n bytes of unicast suite list
  // 2 bytes of authenticated key mgmt suite count
  // followed by 4*n bytes of suite list
} __attribute__ ((packed));

struct ssn_suite_list {
  __u8 oui[3];
  __u8 type;
} __attribute__ ((packed));

typedef enum {
    percent,
    kbps
} rap_uplink_bw_reservation_unit_t;

/********** REMOTE AP **************/

#define WIFI_KEY_PROC_INTERFACE "/proc/net/keys"
// key type is defined in stm_message.h

/*
 * key <mac> <index> <key> <type>
 * mac - BSSID or STA MAC
 * index - 0 to 3, 0 always for STA, 0 to 3 for AP
 *
 * txkey <index>
 *
 * Static WEP - Key is configured always for STA on index 0 
 *            - BSSID key is on index i 
 * Dynamic WEP - 
*/
  

#ifdef SAP_CONFIG_IMP
// private section

#endif


#ifdef SAP_CONFIG_INIT
// initialization section


#endif

#endif
