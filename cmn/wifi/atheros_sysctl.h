/* sapcfg.xml and net80211/ieee80211_linux.c and ASAP_mod need
 * to have a common definition of these sysctl node names
 */

#ifndef _ATHEROS_SYSCTL_
#define _ATHEROS_SYSCTL_

/* 
 * Sysctls for setting VHT mcs rates and firmware stats
 */
#define VAP_SYSCTL_VHT_MCS              "vhtmcs"
#define VAP_SYSCTL_VHT_STATS            "vht_stats"
#define VAP_SYSCTL_VHT_AMPDU            "vht_ampdu"
#define VAP_SYSCTL_VHT_AMSDU            "vht_amsdu"

#define VAP_SYSCTL_VHT_MPDU_SIZE        "max_vht_mpdu_size"
#define VAP_SYSCTL_TX_AMSDU_COUNT_BE    "max_tx_amsdu_count_be"
#define VAP_SYSCTL_TX_AMSDU_COUNT_BK    "max_tx_amsdu_count_bk"
#define VAP_SYSCTL_TX_AMSDU_COUNT_VI    "max_tx_amsdu_count_vi"
#define VAP_SYSCTL_TX_AMSDU_COUNT_VO    "max_tx_amsdu_count_vo"

/* Sysctl nodes relevant to each Virtual AP */
#define VAP_SYSCTL_ENABLE		"enable"
#define VAP_SYSCTL_BAND			"band"
#define VAP_SYSCTL_FORWARD_MODE		"forward_mode"
#define VAP_SYSCTL_DISABLE_PROBE_RETRY	"dis_probe_retry"
#define VAP_SYSCTL_ESSID		"essid"
#define VAP_SYSCTL_ENCRYPTION		"encryption"
#define VAP_SYSCTL_WPA3_TRANSITION	"wpa3_transition"
#define	VAP_SYSCTL_WPA3_TWIN_ESSID	"wpa3_twin_essid"
#define	VAP_SYSCTL_WPA3_TWIN_BSSID	"wpa3_twin_bssid"
#define VAP_SYSCTL_WPA2_PREAUTH		"wpa2_preauth"
#define VAP_SYSCTL_DTIM_INTERVAL	"dtim_interval"
#define VAP_SYSCTL_MFP_CAPABLE		"mfp_capable"
#define VAP_SYSCTL_MFP_REQUIRED		"mfp_required"
#define VAP_SYSCTL_MAX_CLIENTS		"max_clients"
#define VAP_SYSCTL_WMM			"wmm"
#define VAP_SYSCTL_WMM_UAPSD   "wmm_uapsd"
#define VAP_SYSCTL_WMM_BE_NOACK		"wmm_be_noack"
#define VAP_SYSCTL_WMM_BK_NOACK		"wmm_bk_noack"
#define VAP_SYSCTL_WMM_VI_NOACK		"wmm_vi_noack"
#define VAP_SYSCTL_WMM_VO_NOACK		"wmm_vo_noack"
#define VAP_SYSCTL_WMM_TS_MIN_INACT_INT   "wmm_ts_min_inact_int"
#define VAP_SYSCTL_HIDE_SSID		"hide_ssid"
#define VAP_SYSCTL_BROADCAST_PROBE	"deny_broadcast_probe"
#define VAP_SYSCTL_WEPKEY1		"wepkey1"
#define VAP_SYSCTL_WEPKEY2		"wepkey2"
#define VAP_SYSCTL_WEPKEY3		"wepkey3"
#define VAP_SYSCTL_WEPKEY4		"wepkey4"
#define VAP_SYSCTL_WEPTXKEY		"weptxkey"
#define VAP_SYSCTL_WPA_HEXKEY		"wpa_hexkey"
#define VAP_SYSCTL_WPA_PASSPHRASE	"wpa_passphrase"
#define VAP_SYSCTL_MAX_TX_FAILURES	"max_tx_failures"
#define VAP_SYSCTL_WPS			"wps"
#define VAP_SYSCTL_BASIC_RATES		"rates"
#define VAP_SYSCTL_TX_RATES		"tx_rates"
#define VAP_SYSCTL_AGEOUT		"ageout"
#define VAP_SYSCTL_MAX_RETRIES		"max_retries"
#define VAP_SYSCTL_RTS_THRESHOLD	"rts_threshold"
#define VAP_SYSCTL_SHORT_PREAMBLE	"short_preamble"
#define VAP_SYSCTL_LOCAL_PROBE_RESP	"local_probe_response"
#define VAP_SYSCTL_LOCAL_PROBE_REQ_THRESH "local_probe_req_thresh"
#define VAP_SYSCTL_AUTH_REQ_THRESH "auth_req_thresh"
#define VAP_SYSCTL_COMMIT		"commit"
#define VAP_SYSCTL_STAKEY		"stakey"
#define VAP_SYSCTL_DEAUTH_ALL		"deauth_all"
#define VAP_SYSCTL_BATTERY_BOOST	"battery_boost"
#define VAP_SYSCTL_EDCA_PARAMETERS_STA	"edca_parameters_sta"
#define VAP_SYSCTL_EDCA_PARAMETERS_AP	"edca_parameters_ap"
#define VAP_SYSCTL_MU_EDCA_PARAMETERS   "mu_edca_parameters"
#define VAP_SYSCTL_MCAST_RATE_OPT       "mc_rate"
#define VAP_SYSCTL_EAPOL_RATE_OPT       "eapol_rate"
#define VAP_SYSCTL_STRICT_SVP       "strict_svp"
#define VAP_SYSCTL_HT_ENABLE		"ht_enable"
#define VAP_SYSCTL_40MHZ_ENABLE		"40mhz_enable"
#define VAP_SYSCTL_20MHZ_GI		"20mhz_gi"
#define VAP_SYSCTL_40MHZ_GI		"40mhz_gi"
#define VAP_SYSCTL_80MHZ_GI		"80mhz_gi"
#define VAP_SYSCTL_TXBF		          "tx_beamforming"
#define VAP_SYSCTL_VHT_TXBF		  "vht_tx_beamforming"
#define VAP_SYSCTL_TXBF_COMP_STEERING     "txbf_compressed_steering"
#define VAP_SYSCTL_TXBF_NONCOMP_STEERING  "txbf_noncompressed_steering"
#define VAP_SYSCTL_TXBF_IMMEDIATE_FEEDBACK "txbf_immediate_feedback"
#define VAP_SYSCTL_TXBF_DELAYED_FEEDBACK  "txbf_delayed_feedback"
#define VAP_SYSCTL_STBC_TX_STREAMS		"stbc_tx_streams"
#define VAP_SYSCTL_STBC_RX_STREAMS		"stbc_rx_streams"
#define VAP_SYSCTL_LDPC			"ldpc"
#define VAP_SYSCTL_AMPDU_TX_ENABLE      "ampdu_tx_enable"
#define VAP_SYSCTL_AMPDU_RX_ENABLE      "ampdu_rx_enable"
#define VAP_SYSCTL_AMPDU_TX_MAX         "ampdu_tx_max"
#define VAP_SYSCTL_AMPDU_RX_MAX         "ampdu_rx_max"
#define VAP_SYSCTL_AMPDU_RX_MIN_SPC     "ampdu_rx_min_spc"
#define VAP_SYSCTL_AMSDU_RX_MAX         "amsdu_rx_max"
#define VAP_SYSCTL_LEGACY_ALLOWED       "legacy_allowed"
#define VAP_SYSCTL_SUPPORTED_MCS        "supported_mcs"
#define VAP_SYSCTL_SSID_CONTENT_FILTER	"content_filter"
#define VAP_SYSCTL_VHT_ENABLE		    "vht_enable"
#define VAP_SYSCTL_80MHZ_ENABLE		    "80mhz_enable"
#define VAP_SYSCTL_VHT_MCS_MAP   "vht_mcs_map"
#define VAP_SYSCTL_VHT_TX_MCS_20 "vht_tx_mcs_20"
#define VAP_SYSCTL_VHT_TX_MCS_40 "vht_tx_mcs_40"
#define VAP_SYSCTL_VHT_TX_MCS_80 "vht_tx_mcs_80"
#define VAP_SYSCTL_SSID_CONTENT_FILTER	"content_filter"
#define VAP_SYSCTL_BA_AMSDU_ENABLE      "ba_amsdu"
#define VAP_SYSCTL_SW_RETRY_ENABLE      "temporal_diversity"
#define VAP_SYSCTL_WMM_OVERRIDE_DSCP_ENABLE "wmm_override_dscp_mapping"
#define VAP_SYSCTL_VHT_MU_TXBF		  "vht_mu_tx_beamforming"

#define VAP_SYSCTL_HE_ENABLE              "he_enable"
#define VAP_SYSCTL_DYNAMIC_FRAG_LEVEL     "dynamic_frag_level"
#define VAP_SYSCTL_DYNAMIC_FRAG_MAX_NUM   "dynamic_frag_max_num"
#define VAP_SYSCTL_DYNAMIC_FRAG_MIN_SIZE  "dynamic_frag_min_size"
#define VAP_SYSCTL_DURATION_BASED_RTS     "duration_based_rts"
#define VAP_SYSCTL_HE_AMPDU_MAX_LENGTH    "he_ampdu_max_length"
#define VAP_SYSCTL_INDIVIDUAL_TWT         "individual_twt"
#define VAP_SYSCTL_BROADCAST_TWT          "broadcast_twt"
#define VAP_SYSCTL_HE_TXBF                "he_txbf"
#define VAP_SYSCTL_HE_OMI                 "he_omi"
#define VAP_SYSCTL_MULTI_TID_AMPDU        "multi_tid_ampdu"
#define VAP_SYSCTL_HE_MCS_MAP             "he_mcs_map"
#define VAP_SYSCTL_HE_MU_OFDMA            "he_mu_ofdma"
#define VAP_SYSCTL_HE_MU_MIMO             "he_mu_mimo"
#define VAP_SYSCTL_UL_OFDMA_RANDOM_ACCESS "ul_ofdma_random_access"


#ifdef __FAT_AP__
#define VAP_SYSCTL_SSID_CONTENT_FILTER	"content_filter"
#define VAP_SYSCTL_ENFORCE_DHCP         "enforce_dhcp"
#define VAP_SYSCTL_BYPASS_CP_DOMAIN     "bypass_cp_domain"
#define VAP_SYSCTL_AUTH_SURV            "auth_surv"
#define VAP_SYSCTL_DHCP_ROLE_ENABLE     "dhcp_role_enable"
#define VAP_SYSCTL_VAP_SUBNET            "vap_subnet"
#define VAP_SYSCTL_APPLY_BLACK_LIST     "apply_black_list"
#define VAP_SYSCTL_11V_ENABLE		     "11v"
#define VAP_SYSCTL_DENY_INTER_USR_BRIDGING "deny_usr_bridging"
#define VAP_SYSCTL_DENY_LOCAL_ROUTING      "deny_local_routing"
#define VAP_SYSCTL_ZONE                    "vap_zone"
#endif

#define VAP_SYSCTL_WMM_DSCP_MAPPING      "wmm_dscp_mapping"
#define VAP_SYSCTL_WMM_VO_DSCP           "wmm_vo_dscp"
#define VAP_SYSCTL_WMM_VI_DSCP           "wmm_vi_dscp"
#define VAP_SYSCTL_WMM_BE_DSCP           "wmm_be_dscp"
#define VAP_SYSCTL_WMM_BK_DSCP           "wmm_bk_dscp"
#define VAP_SYSCTL_WMM_EAP_AC            "wmm_eap_ac"
#define VAP_SYSCTL_BAND_STEERING         "band_steering"
#define VAP_SYSCTL_ALLOC_BCAST_KEYSLOT  "bcast_keyslot"
#define VAP_SYSCTL_ALLOC_MFP_KEYSLOT	"mfp_clr_keyslot"
#define VAP_SYSCTL_BAND_STEERING_MODE    "band_steering_mode"
#define VAP_SYSCTL_11K_ENABLE		     "11k"
#define VAP_SYSCTL_BCN_MEASUREMENT_MODE  "bcn_measurement_mode"
#define VAP_SYSCTL_BEACON_RATE           "beacon_rate"
#define VAP_SYSCTL_MULTICAST_RATE        "multicast_rate"
#define VAP_SYSCTL_HANDOVER_TRIGGER		 "handover_trigger"
#define VAP_SYSCTL_MONITORED_CLIENT      "monitored_client"
#define VAP_SYSCTL_QBSS_LOAD_ENABLE		 "qbss_load_enable"
#define VAP_SYSCTL_DOS_PREV              "dos_prevention"

#define VAP_SYSCTL_HOTSPOT_ENABLE                "hotspot_enable"
#define VAP_SYSCTL_HOTSPOT_OSEN_ENABLE           "hotspot_osen"
#define VAP_SYSCTL_HOTSPOT_ASRA                  "hotspot_asra"
#define VAP_SYSCTL_HOTSPOT_INTERNET              "hotspot_internet"
#define VAP_SYSCTL_HOTSPOT_QUERY_RSP_LEN_LIMIT   "query_response_len_limit"
#define VAP_SYSCTL_HOTSPOT_ACC_NWK_TYPE          "acc_nwk_type"
#define VAP_SYSCTL_HOTSPOT_VENUE_GROUP           "venue_group"
#define VAP_SYSCTL_HOTSPOT_VENUE_TYPE            "venue_type"
#define VAP_SYSCTL_HOTSPOT_INDICATION_TYPE       "indication_type"
#define VAP_SYSCTL_HOTSPOT_ROAM_CONS_OI_1        "roam_cons_oi_1"
#define VAP_SYSCTL_HOTSPOT_ROAM_CONS_OI_2        "roam_cons_oi_2"
#define VAP_SYSCTL_HOTSPOT_ROAM_CONS_OI_3        "roam_cons_oi_3"
#define VAP_SYSCTL_HOTSPOT_HESSID                "hessid"
#define VAP_SYSCTL_HOTSPOT_ADDTL_RC_OI           "additional_roam_cons_ois"
#define VAP_SYSCTL_HOTSPOT_PAME_BI               "pame_bi"
#define VAP_SYSCTL_HOTSPOT_DGAF_DISABLE          "dgaf_disable"
#define VAP_SYSCTL_HOTSPOT_RELEASE_NUMBER        "hs_release_number"
#define VAP_SYSCTL_HOTSPOT_ANQP_DOMAIN_ID        "anqp_domain_id"
#define VAP_SYSCTL_HOTSPOT_TIME_ZONE             "time_zone"
#define VAP_SYSCTL_HOTSPOT_TIME_ADVT_CAP         "time_advt_cap"
#define VAP_SYSCTL_HOTSPOT_TIME_ERROR            "time_error"
#define VAP_SYSCTL_HOTSPOT_ADVT_PROT             "advt_prot"
#define VAP_SYSCTL_HOTSPOT_P2P_DEV_MGMT          "p2p_dev_mgmt"
#define VAP_SYSCTL_HOTSPOT_P2P_CROSS_CONNECT     "p2p_cross_connect"
#define VAP_SYSCTL_HOTSPOT_SUB_REM_SERVER_URL    "sub_rem_server_url"
#define VAP_SYSCTL_HOTSPOT_SESS_INFO_URL         "sess_info_url"
#define VAP_SYSCTL_HOTSPOT_SUB_DEAUTH_REASON_URL "sub_deauth_reason_url"
#define VAP_SYSCTL_HOTSPOT_QOS_MAP_EXCEPTIONS    "qos_map_exceptions"
#define VAP_SYSCTL_HOTSPOT_QOS_MAP_RANGES        "qos_map_ranges"
#define VAP_SYSCTL_HOTSPOT_QOS_MAP               "qos_map"
#define VAP_SYSCTL_HOTSPOT_QBSS_LOAD             "qbss_load"
#define VAP_SYSCTL_HOTSPOT_QBSS_STA_COUNT        "qbss_sta_count"
#define VAP_SYSCTL_HOTSPOT_QBSS_AVAIL_CAP        "qbss_avail_cap"
#define VAP_SYSCTL_LOCATION_INFORMATION          "location"

#define VAP_SYSCTL_DMO_ENABLE            "dmo_enable"

#define VAP_SYSCTL_DMO_THRESH            "dmo_thresh"
#ifdef __FAT_AP__  // NO_AP_IGMP
#define VAP_SYSCTL_DMO_UTILIZATION       "dmo_utilization"
#define SYSCTL_STA_IDLE_TIMEOUT          "ni_inact_reload"
#endif // NO_AP_IGMP

#define VAP_SYSCTL_MSFT_WISPR_INFORMATION "msft_wispr"
#define VAP_SYSCTL_AP_NAME_INFORMATION "advt_ap_name"

#define VAP_SYSCTL_APCR_CHAN_11A         "apcr_chan_11a"
#define VAP_SYSCTL_APCR_CHAN_11BG        "apcr_chan_11bg"
#define VAP_SYSCTL_BR_CHAN_11A           "br_chan_11a"
#define VAP_SYSCTL_BR_CHAN_11BG          "br_chan_11bg"
#define VAP_SYSCTL_BCN_REQ_TIME          "bcn_req_time"
#define VAP_SYSCTL_LM_REQ_TIME           "lm_req_time"
#define VAP_SYSCTL_TSM_REQ_TIME          "tsm_req_time"

// for rrm-ie-prof 
#define VAP_SYSCTL_RRM_ENABLED_CAP_IE    "rrm_enabled_cap_ie"
#define VAP_SYSCTL_RRM_COUNTRY_IE        "rrm_country_ie"
#define VAP_SYSCTL_RRM_PWR_CONSTR_IE     "rrm_pwr_constr_ie"
#define VAP_SYSCTL_RRM_TPC_REPORT_IE     "rrm_tpc_report_ie"
#define VAP_SYSCTL_RRM_QBSS_LOAD_IE      "rrm_qbss_load_ie"
#define VAP_SYSCTL_RRM_BSS_AAC_IE        "rrm_bss_aac_ie"
#define VAP_SYSCTL_RRM_QUIET_IE          "rrm_quiet_ie"

#define VAP_SYSCTL_CLIENT_OPERATE        "client_operate"

#define WIF_SYSCTL_MAX_EIRP              "max_eirp"
#define WIF_SYSCTL_NONOCCUPANCY_CHECK    "non_occupancy_check"
#define WIF_SYSCTL_ACTUAL_EIRP           "actual_eirp"
#define WIF_SYSCTL_NUM_CLIENTS           "num_clients"
#define WIF_SYSCTL_MIN_EIRP              "min_eirp"

// for mesh
#define VAP_SYSCTL_MESH_STA_DISASSOC	"m_sta_disassoc"

#define ASAP_SYSCTL_VAP_BCAST_FILTER_ALL    "broadcast_filter_all"
#define ASAP_SYSCTL_VAP_BCAST_FILTER_ARP    "broadcast_filter_arp"
// for swatch
#define ASAP_SYSCTL_VAP_BCAST_FILTER        "broadcast_filter"
// for 802.11r
#define VAP_SYSCTL_DOT11R_KNOB           "11r"
#define VAP_SYSCTL_MOBILITY_DOMAIN_ID    "mdid"

/* Nodes relevant to ASAP_mod */
#define ASAP_SYSCTL_BRIDGE_INIT         "br_init"
#define ASAP_SYSCTL_LMSIP               "lmsip"
#define ASAP_SYSCTL_AMSIP               "amsip"
#define ASAP_SYSCTL_GRE_TYPE            "gre_type"
#define ASAP_SYSCTL_GRE_ALL_SLAVE       "gre_slave_all"
#define  ASAP_SYSCTL_GRE_USE_VC_IP      "gre_use_vc_ip"
#define  ASAP_SYSCTL_GRE_OUTSIDE        "gre_outside"
#define ASAP_SYSCTL_FLUSH_USER_TABLE    "flush_user_table"
#define ASAP_SYSCTL_MTU                 "mtu"
#define ASAP_SYSCTL_RAP_MTU             "rap_mtu"
#define ASAP_SYSCTL_RMP_MPV		"mpv"
#define ASAP_SYSCTL_MTU_SWITCH          "mtu_switch"
#define ASAP_SYSCTL_MTU_ALL             "mtu_all"
#define ASAP_SYSCTL_MTU_ICMP            "mtu_icmp"
#define ASAP_SYSCTL_TIMESTAMP           "timestamp"
#define ASAP_SYSCTL_CAPTURE             "capture"
#define ASAP_SYSCTL_LPCAP_FLOW          "lpcap_flow"
#define ASAP_SYSCTL_PCAPOO              "pcapoo"
#define ASAP_SYSCTL_FFT_CAPTURE         "fft_capture"
#define ASAP_SYSCTL_FORWARD_MODE        "forward_mode"
#define ASAP_SYSCTL_VLAN                "vlan"
#ifdef __FAT_AP__
#define ASAP_SYSCTL_STATE               "state"
#define ASAP_SYSCTL_REBOOTME_ACK        "rebootme_ack"
#endif
#define ASAP_SYSCTL_NATIVE_VLAN         "native_vlan"
#define ASAP_SYSCTL_VLAN_DISCOVERY      "vlan_discovery"
#define ASAP_SYSCTL_HEARTBEAT_INTERVAL  "heartbeat_interval"
#define ASAP_SYSCTL_BRIDGE              "bridge"
#define ASAP_SYSCTL_MODE                "mode"
#define ASAP_SYSCTL_BRIDGE_IF_DOWN	"bridge_down"
#define ASAP_SYSCTL_AM_BUF_OVERFLOWS    "am_buf_overflows"
#define ASAP_SYSCTL_SPECTRUM_OVERFLOWS    "spectrum_overflows"
#define ASAP_SYSCTL_BOARD_TEMP          "board_temp"
#define ASAP_SYSCTL_HB_DSCP             "hb_dscp"
#define ASAP_SYSCTL_MGMT_DSCP           "mgmt_dscp"
#define ASAP_SYSCTL_DSCP_TO_DOT1P_PRIORITY_MAPPING "dscp_to_dot1p_priority_mapping"
#define ASAP_SYSCTL_ELECTION_STATE      "election_state"
#define ASAP_SYSCTL_PPP                "ppp"
#define ASAP_SYSCTL_MESH_VAP		"mesh_mode"
#define ASAP_SYSCTL_MESH_ENET0_MODE	"mesh_enet0"
#define ASAP_SYSCTL_STANDBYIP           "standbyip"
#define ASAP_SYSCTL_SET_STANDBY_AS_ACTIVE  "set_standby_as_active"
#define ASAP_SYSCTL_VAP_GONE            "vap_gone"
#define ASAP_SYSCTL_GRE_STRIPING_IP    	"gre_striping_ip"
#define ASAP_SYSCTL_CLR_GRE_RX_COUNTERS "gre_rx_clear"
#define ASAP_SYSCTL_GRE_OFFLOAD_INFO         "gre_offload_info"
#define ASAP_SYSCTL_LAG_DESTINATION_LIST "lag_dest_list"
#define ASAP_SYSCTL_LAG_STA_SHOW_COUNTERS   "lag_sta_counters"
#define ASAP_SYSCTL_LAG_STA_RESET_COUNTERS  "lag_sta_clear"

#define ASAP_SYSCTL_OPENFLOW_ENABLE     "of_enable"
#define ASAP_SYSCTL_MULT_TX_REPLAY_CNTR "mult_tx_replay"

#define ASAP_SYSCTL_BK_VAP_ENABLE       "bk_vap"
#define ASAP_SYSCTL_BK_ENABLE           "bk_enable"

#define ASAP_SYSCTL_ETH_CAPTURE         "eth_capture"

#define ASAP_SYSCTL_FIREWALL_DEVICE     "firewall_dev"
#define ASAP_SYSCTL_FIREWALL_UPDATE_BOND_FIREWALL     "bond_firewall_update"

#define ASAP_SYSCTL_DPA_HBT_DEBUG              "dpa_hbt_debug"

#if 0
#define ASAP_SYSCTL_MESH_ENET1_MODE	"mesh_enet1"
#endif
#define ASAP_SYSCTL_MESH_BYTES 		"mesh_bytes"
#define ASAP_SYSCTL_WIF_MESH_MODE	"mesh_mode"
#define ASAP_SYSCTL_WIF_SENSOR   	"sensor"
#if 0
#define ASAP_SYSCTL_MESH_ENET0_DISABLE	"mesh_enet0_disable"
#define ASAP_SYSCTL_MESH_ENET1_DISABLE	"mesh_enet1_disable"
#endif
#define ASAP_SYSCTL_SNAKE_TEST		"snake_test"
#define ASAP_SYSCTL_PORT_MONITOR	"port_monitor"
#define ASAP_SYSCTL_DOUBLE_ENCRYPT	"double_encrypt"
#define ASAP_SYSCTL_FIPS_DISABLE	"fips_disable"
#define ASAP_SYSCTL_AM_FILTER      	"am_filter"
#define ASAP_SYSCTL_ALLOW_BAND_STEERING "allow_band_steering"
#define ASAP_SYSCTL_DATA_SEC_MODE      	"data_sec_mode"
#define ASAP_SYSCTL_MCAST_AGGR      "mcast_aggr"
#define ASAP_SYSCTL_MCAST_AGGR_VLAN      "mcast_aggr_vlan"

#ifdef CONFIG_GRENACHE
#define ASAP_SYSCTL_SOS_LINK_ADD	"asap_sos"
#define ASAP_SYSCTL_SET_CPU		"set_cpu"
#define ASAP_SYSCTL_SKB_QLEN		"gren_skbqlen"
#endif

#define ASAP_SYSCTL_WIF_MESH_PORTAL	1
#define ASAP_SYSCTL_WIF_MESH_POINT	2

#define ASAP_SYSCTL_SJ_TAGGED           "tagged"
#define ASAP_SYSCTL_SJ_NATIVE_VLAN      "native_vlan"
#define ASAP_SYSCTL_SJ_FORWARD_MODE     "forward_mode"
#define ASAP_SYSCTL_SJ_ADD_ALLOWED_VLAN "add_allowed_vlan"
#define ASAP_SYSCTL_SJ_ALLOW_ALL_VLAN   "all"
#define ASAP_SYSCTL_SJ_ALLOW_NO_VLAN    "none"
#define ASAP_SYSCTL_SJ_ADD_DEV          "add"
#define ASAP_SYSCTL_SJ_DEL_DEV          "del"
#define ASAP_SYSCTL_SJ_DEL_ALLOWED_VLAN "del_allowed_vlan"
#define ASAP_SYSCTL_SJ_CLEAR_ALLOWED_VLAN "clear_allowed_vlan"
#define ASAP_SYSCTL_SJ_LMS_ADDRESS       "lms_address"
#define ASAP_SYSCTL_SJ_STATUS            "status"
#define ASAP_SYSCTL_SJ_MTU_PROBES        "mtu"
#define ASAP_SYSCTL_SJ_SHUTDOWN          "shutdown"
#define ASAP_SYSCTL_SJ_FALLBACK          "fallback"
#define ASAP_SYSCTL_SJ_AUTH_TIMEOUT      "auth_timeout"
#define ASAP_SYSCTL_SJ_L2_AUTH		 "l2_auth"
#define ASAP_SYSCTL_SJ_TRUSTED           "trusted"
#define ASAP_SYSCTL_SJ_ADD_ALLOWED_AND_NATIVE_VLAN "add_allowed_and_native_vlan"
#ifdef __FAT_AP__
#define ASAP_SYSCTL_SJ_BLOCK_USER        "block_user"
#define ASAP_SYSCTL_SJ_STP               "stp"
#define ASAP_SYSCTL_SJ_DHCP_ROLE_ENABLE  "dhcp_role_enable"
#endif
#define ASAP_SYSCTL_SJ_LOOP_PROTECT             "loop_protect"
#define ASAP_SYSCTL_SJ_LOOP_PROTECT_PORT_ERR    "loop_protect_port_err"
#define ASAP_SYSCTL_SJ_LOOP_PROTECT_STATS       "loop_protect_stats"
#define ASAP_SYSCTL_SJ_STATS                    "stats"
#define ASAP_SYSCTL_SJ_MESH_SEND_PACKET         "mesh_auto_send_packet"
#ifdef __FAT_AP__
#define ASAP_SYSCTL_SJ_MESH_AUTO_ENET0_BRG      "mesh_auto_enet0_bridging"
#endif

/* 802.3az */
#define ASAP_SYSCTL_DOT3AZ_ENABLE	"enable"
#define ASAP_SYSCTL_DOT3AZ_STATUS	"status"
#define ASAP_SYSCTL_DOT3AZ_DEBUG	"debug"

/* 802.3bz */
#define ASAP_SYSCTL_DOT3BZ_ENABLE	"enable"
#define ASAP_SYSCTL_DOT3BZ_STATUS	"status"
#define ASAP_SYSCTL_DOT3BZ_SET_ENABLE   "set_enable"

/* PSE */
#define ASAP_SYSCTL_PSE_ENABLE	"enable"
#define ASAP_SYSCTL_PSE_STATUS	"status"
#define ASAP_SYSCTL_PSE_DEBUG	"debug"

// BW mgmt stuff
#define ASAP_SYSCTL_BW_MGMT_STATS_PERIOD          "bw_stats_period"
#define ASAP_SYSCTL_BW_MGMT_TOKEN_UPDATE_PERIOD   "bw_token_period"
#define ASAP_SYSCTL_BW_MGMT_RESET_STATS           "bw_reset_stats"
#define ASAP_SYSCTL_BW_MGMT_STATS                 "bw_stats"
#define ASAP_SYSCTL_BW_MGMT_STATS_VERBOSE         "bw_stats_v"

#define ASAP_SYSCTL_BW_MGMT_VAP_SHARE              "bw_share"
#define ASAP_SYSCTL_BW_MGMT_VAP_HARD_LIMIT         "bw_hard_limit"
#define ASAP_SYSCTL_BW_MGMT_VAP_STATS              "bw_stats"
#define ASAP_SYSCTL_BW_MGMT_VAP_IGMP_STATS         "igmp_stats"
#define ASAP_SYSCTL_BW_MGMT_POLICY                 "shaping-policy"
#define ASAP_SYSCTL_BW_MGMT_VAP_STATS_VERBOSE      "bw_stats_v"
#define ASAP_SYSCTL_BW_MGMT_VAP_SHAPING_TABLE      "shaping-table"
#define ASAP_SYSCTL_BW_MGMT_VAP_LIMIT              "bw_vap_limit"
#define ASAP_SYSCTL_BW_MGMT_VAP_USER_LIMIT         "bw_vap_user_limit"

#define ASAP_SYSCTL_BW_MGMT_VAP_PER_AC_SHARE       "per_ac_share"
#define ASAP_SYSCTL_BW_MGMT_CLEAR_COUNTERS	   "clear_bwm"
#define ASAP_SYSCTL_STA_DATA_READY                 "sta_data_ready"

/* SOS Firewall defines */
#define ASAP_SYSCTL_FIREWALL_BRIDGE_TABLE          "bridge_table"
#define ASAP_SYSCTL_FIREWALL_SESSION_TABLE         "session_table"
#define ASAP_SYSCTL_FIREWALL_USER_TABLE            "user_table"
#define ASAP_SYSCTL_FIREWALL_SERVICE_TABLE         "service_table"
#define ASAP_SYSCTL_FIREWALL_VLAN_TABLE            "vlan_table"
#define ASAP_SYSCTL_FIREWALL_MCAST_TABLE           "mcast_table"
#ifdef __FAT_AP__  // NO_AP_IGMP
#define ASAP_SYSCTL_FIREWALL_IGMP_STATION          "igmp_station"
#define ASAP_SYSCTL_FIREWALL_IGMP_GROUP_TABLE      "igmp_group_table"
#define ASAP_SYSCTL_FIREWALL_IGMP_GROUPS           "igmp_groups"
#define ASAP_SYSCTL_FIREWALL_STATION_MCAST_GROUPS  "station_mcast_groups"
#define ASAP_SYSCTL_FIREWALL_IP_MCAST_GROUPS       "ip_mcast_groups"
#define ASAP_SYSCTL_FIREWALL_IP_MCAST_MADDR        "ip_mcast_maddr"
#define ASAP_SYSCTL_FIREWALL_IP_MCAST_IGMP_STATS   "ip_mcast_igmp_stats"
#define ASAP_SYSCTL_FIREWALL_MCAST_ACLS            "mcast_acls"
#endif // NO_AP_IGMP
#define ASAP_SYSCTL_FIREWALL_STATS                 "stats"
#define ASAP_SYSCTL_FIREWALL_SJ_STATS              "sj_stats"  
#define ASAP_SYSCTL_FIREWALL_ACLS                  "acls"
#define ASAP_SYSCTL_FIREWALL_ACL_DOWNLOADED        "acl_downloaded"
#define ASAP_SYSCTL_FIREWALL_NETDEST               "netdest"
#define ASAP_SYSCTL_FIREWALL_CLEAR_NETDEST         "clear_netdest"
#define ASAP_SYSCTL_FIREWALL_NAT_POOLS             "nat_pools"
#define ASAP_SYSCTL_FIREWALL_DEL_NAT_POOL          "nat_pool_del"
#define ASAP_SYSCTL_FIREWALL_FRAG_TABLE            "frag_table"
#define ASAP_SYSCTL_FIREWALL_ROUTE_TABLE           "route_table"
#define ASAP_SYSCTL_FIREWALL_DNS_DOMAINS           "dns_domains"
#define ASAP_SYSCTL_FIREWALL_DNS_ID_MAP            "dns_id_map"
#define ASAP_SYSCTL_FIREWALL_RESET_ROLE_BWM_TABLE  "reset_role_bwm_table"
#define ASAP_SYSCTL_FIREWALL_ROLE_BWM_CONFIG       "role_bwm_table"
#define ASAP_SYSCTL_FIREWALL_HEARTBEAT             "heartbeat"
#define ASAP_SYSCTL_FIREWALL_HEARTBEAT_STATS       "heartbeat_stats"
#define ASAP_SYSCTL_FIREWALL_HEARTBEAT_TOTAL_STATS "heartbeat_total_stats"
#define ASAP_SYSCTL_FIREWALL_HEARTBEAT_MTUSTATS    "heartbeat_mtustats"
#define ASAP_SYSCTL_FIREWALL_HEARTBEAT_BOOSTER     "heartbeat_booster"
#define ASAP_SYSCTL_FIREWALL_HBT_SEQNUM_INFO	   "heartbeat_seqnum_info"
#define ASAP_SYSCTL_FIREWALL_HBT_CLEAR	           "heartbeat_clear"
#define ASAP_SYSCTL_FIREWALL_STANDBY_HEARTBEAT     "standby_heartbeat"
#define ASAP_SYSCTL_FIREWALL_STANDBY_HEARTBEAT_STATS  "standby_heartbeat_stats"
#define ASAP_SYSCTL_FIREWALL_CONFIG                "config"
#define ASAP_SYSCTL_FIREWALL_AP_GROUP              "ap_group"
#define ASAP_SYSCTL_FIREWALL_SESSION_ACL           "session_acl"
#define ASAP_SYSCTL_FIREWALL_DHCP_VLAN             "dhcp_vlan"
#define ASAP_SYSCTL_FIREWALL_ALWAYS_VLAN           "always_vlan"
#define ASAP_SYSCTL_FIREWALL_PORT_TABLE            "port_table"
#define ASAP_SYSCTL_FIREWALL_PPPOE_SID             "pppoe_sid"
#define ASAP_SYSCTL_FIREWALL_USER_IDLE_TIMEOUT     "idle_timeout"
#define ASAP_SYSCTL_FIREWALL_WIRED_CLIENTS         "wired_clients"
#define ASAP_SYSCTL_FIREWALL_TRACE                 "firewall_trace"
#define ASAP_SYSCTL_FIREWALL_UPLINK_BW_RESV        "uplink_bw_reservation"
#define ASAP_SYSCTL_FIREWALL_LOCAL_NETWORK_ACCESS  "local_network_access"
#define ASAP_SYSCTL_FIREWALL_L2USER_ADD            "l2user_add"
#define ASAP_SYSCTL_FIREWALL_L2USER_REM            "l2user_rem"
#define ASAP_SYSCTL_FIREWALL_ADD_MCAST             "mcast_add"
#define ASAP_SYSCTL_FIREWALL_REM_MCAST             "mcast_rem"
#define ASAP_SYSCTL_FIREWALL_ELECTED_MASTER        "elected_master"
#define ASAP_SYSCTL_FIREWALL_SWARM_IP              "swarm_ip"
#define ASAP_SYSCTL_FIREWALL_VC_VLAN_MASK_GW       "vc_vlan_mask_gw"
#define ASAP_SYSCTL_FIREWALL_DRP_IP_VLAN_MASK_GW   "drp_ip_vlan_mask_gw"
#define ASAP_SYSCTL_FIREWALL_DNS_PROXY             "dns_proxy"
#define ASAP_SYSCTL_FIREWALL_MASTER_MAC            "master_mac"
#define ASAP_SYSCTL_FIREWALL_DEFAULT_IP            "default_ip"
#define ASAP_SYSCTL_FIREWALL_RTR_ADVT_FLAGS        "rtr_advt_flags"
#define ASAP_SYSCTL_FIREWALL_DHCP_RELAY            "dhcp_relay"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82_ALU     "dhcp_option82_alu"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82         "dhcp_option82"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82_AGENT   "dhcp_option82_agent"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82_CIRCUITID_STR "dhcp_option82_circuitid_str"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82_REMOTEID_STR "dhcp_option82_remoteid_str"
#define ASAP_SYSCTL_FIREWALL_MASTER_ELECTION       "election_statistics"
#define ASAP_SYSCTL_FIREWALL_AUTH_SURV             "auth_surv"
#define ASAP_SYSCTL_FIREWALL_G_MCAST_ENABLE_DISABLE "guest_mdns_mcast_enable"
#define ASAP_SYSCTL_FIREWALL_TELNET                "telnet"
#define ASAP_SYSCTL_FIREWALL_L2TPV3_TUNNEL_IP      "l2tpv3_tunnel_ip"
#define ASAP_SYSCTL_FIREWALL_CLIENT_PKT_TRACE      "client_pkt_trace"
#define ASAP_SYSCTL_FIREWALL_OFFLINE_VAP_ACE       "offline_vap_ace"
#define ASAP_SYSCTL_FIREWALL_MOBILITY_ALT_IP       "alt_ip"

/*for RAP-NG to receieve ipsec status*/
#define ASAP_SYSCTL_FIREWALL_TUNNEL_INFO           "tunnel_info"
#define ASAP_SYSCTL_FIREWALL_IPSEC_STATUS          "ipsec_status"
#define ASAP_SYSCTL_FIREWALL_CLEAR_VPN_STATUS      "clear_vpn_status"
#define ASAP_SYSCTL_FIREWALL_L2_CORP_VLANS         "l2_corp_vlans"
#define ASAP_SYSCTL_FIREWALL_DHCP_RELAY_VLANS      "dhcp_relay_vlans"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82_VLANS   "dhcp_option82_vlans"
#define ASAP_SYSCTL_FIREWALL_DHCP_OPTION82_ALU_VLANS "dhcp_option82_alu_vlans"
#define ASAP_SYSCTL_FIREWALL_L2_CORP_VLANS_IN_SLAVE "l2_corp_vlan_in_slave"
#define ASAP_SYSCTL_FIREWALL_DHCP_VLAN_LIST        "dhcp_vlan_list"
#define ASAP_SYSCTL_FIREWALL_GRE_HEALTH_CHECK_PING "gre_ping_health_check"
#define ASAP_SYSCTL_FIREWALL_DEBUG_PKT             "debug_pkt"
#define ASAP_SYSCTL_FIREWALL_L3_GW_IP              "l3_gw_ip"
#define ASAP_SYSCTL_FIREWALL_PEER_TUNL_IP          "peer_tunl_ip"
#define ASAP_SYSCTL_FIREWALL_MDNS_ENABLE_DISABLE   "mdns_enable"
#ifdef __FAT_AP__
#define ASAP_SYSCTL_FIREWALL_FULL_TUNNEL_VLANS     "full_tunnel_vlans"
#define ASAP_SYSCTL_FIREWALL_LOCAL_L2_VLANS        "local_l2_vlans"
#define ASAP_SYSCTL_FIREWALL_SSDP_ENABLE_DISABLE   "ssdp_enable"
#define ASAP_SYSCTL_FIREWALL_UPLINK_CONFIGURED      "fatap_pppoe_uplink_configured"
#define ASAP_SYSCTL_FIREWALL_FILTERED_DOMAIN        "filtered_domain"
#define ASAP_SYSCTL_FIREWALL_CP_DOMAIN              "captive_portal_domain"
#define ASAP_SYSCTL_ALLOWED_AP                      "allowed_ap"
#define ASAP_SYSCTL_FIREWALL_ALLOW_PAPI             "allow_papi"
#define ASAP_SYSCTL_FIREWALL_FACTORY_SSID           "factory_ssid"
#define ASAP_SYSCTL_FIREWALL_FACTORY_ESSID          "factory_essid"
#endif
#if 0
#define ASAP_SYSCTL_FIREWALL_ALG                   "alg"
#endif
#define ASAP_SYSCTL_FIREWALL_SCCP_DISABLE          "sccp_disable"
#define ASAP_SYSCTL_FIREWALL_SIP_DISABLE           "sip_disable"
#define ASAP_SYSCTL_FIREWALL_UA_DISABLE            "ua_disable"
#define ASAP_SYSCTL_FIREWALL_VOCERA_DISABLE        "vocera_disable"
#define ASAP_SYSCTL_FIREWALL_AP_IP                 "ap_ip"

#define ASAP_SYSCTL_FIREWALL_ENABLE_MOBILITY        "enable_mobility"
#define ASAP_SYSCTL_FIREWALL_CLEAR_CONFIG          "clear_fw_config"
#define ASAP_SYSCTL_FIREWALL_AP_CLASS              "ap_class"
#define ASAP_SYSCTL_FIREWALL_DEBUG_MASTER_BEACON   "debug_master_beacon"
#define ASAP_SYSCTL_FIREWALL_DEBUG_MOBILITY        "debug_mobility"
#define ASAP_SYSCTL_FIREWALL_BOND0_ARP_STUCK_TIME  "bond0_stuck_time"
#define ASAP_SYSCTL_FIREWALL_STANDALONE_MODE       "standalone_mode"
#define ASAP_SYSCTL_FIREWALL_WIFI_UPLINK           "wifi_uplink"
#define ASAP_SYSCTL_FIREWALL_3G_UPLINK             "3g_uplink"
#define ASAP_SYSCTL_FIREWALL_ENFORCE_ETH_UPLINK    "enforce_eth_uplink"
#define ASAP_SYSCTL_FIREWALL_IAP_MASTER            "iap_master"
#define ASAP_SYSCTL_FIREWALL_IAP_MASTER_WAIT_TICK  "iap_master_wait_tick"
#define ASAP_SYSCTL_FIREWALL_IAP_FACTORY_SSID      "iap_factory_ssid"
#define ASAP_SYSCTL_FIREWALL_ELECTION_UPLINK_FLAPS "election_uplink_flaps"
#define ASAP_SYSCTL_FIREWALL_ELECTION_BEACON_MISS  "election_beacon_miss"
#define ASAP_SYSCTL_FIREWALL_STATIC_GOTIP           "asap_static_gotip"
#define ASAP_SYSCTL_FIREWALL_IAP_L2SWITCH_MODE     "asap_l2switch_mode"

#define ASAP_SYSCTL_FIREWALL_L3_MOB_STATUS         "l3_mob_status"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_ADD_HOME       "l3_mob_add_home"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_DEL_HOME       "l3_mob_del_home"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_SHOW_HOME      "l3_mob_show_home"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_ADD_FOREIGN    "l3_mob_add_foreign"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_DEL_FOREIGN    "l3_mob_del_foreign"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_SHOW_FOREIGN   "l3_mob_show_foreign"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_ADD_TUNNEL     "l3_mob_add_tunnel"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_DEL_TUNNEL     "l3_mob_del_tunnel"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_SHOW_TUNNELS   "l3_mob_show_tunnels"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_RESET_FLAGS    "l3_mob_reset_flags"
#define ASAP_SYSCTL_FIREWALL_L3_MOB_SET_CLIENT_ACL "l3_mob_set_client_acl"
#define ASAP_SYSCTL_FIREWALL_MCAST_GROUP           "mcast_group"
#define ASAP_SYSCTL_FIREWALL_ACL_SHOW_ALLOC        "acl_show_alloc"
#define ASAP_SYSCTL_FIREWALL_3G_UPLINK_PRESENT     "3g_uplink_present"

#define ASAP_SYSCTL_FIREWALL_ADD_VLAN_DEV          "add_vlan_dev"
#define ASAP_SYSCTL_FIREWALL_REM_VLAN_DEV          "rem_vlan_dev"

#define ASAP_SYSCTL_FIREWALL_FAST_SWITCH           "fast_switch"
#define ASAP_SYSCTL_FIREWALL_MGMT_PLANE_PROTECTION "mgmt_plane_protection"
#define ASAP_SYSCTL_FIREWALL_ALLOW_CORP_VLANS_ON_UPLINK "allow_corp_vlans_on_uplink"
#define ASAP_SYSCTL_FIREWALL_SYSCTL_SUBNET_SHOW    "show_dhcp_subnets"

#define ASAP_SYSCTL_FIREWALL_DROP_BAD_ARP_ENABLE  "drop_bad_arp_enable"
#define ASAP_SYSCTL_FIREWALL_FIX_DHCP_ENABLE      "fix_dhcp_enable"
#define ASAP_SYSCTL_FIREWALL_POISON_CHECK_ENABLE  "poison_check_enable"
#define ASAP_SYSCTL_FIREWALL_ATTACK_STATS         "attack_stats"
#define ASAP_SYSCTL_FIREWALL_EXTENDED_ESSID_ENABLE "extended_essid_enable"
#define ASAP_SYSCTL_FIREWALL_INACTIVITY_AGEOUT     "inactivity_ageout"
#define ASAP_SYSCTL_FIREWALL_CALEA_SERVER_IP       "calea_ip"
#define ASAP_SYSCTL_FIREWALL_CALEA_CLIENT_MAC      "calea_client_mac"
#define ASAP_SYSCTL_FIREWALL_CALEA_GRE_TYPE        "calea_gre_type"
#define ASAP_SYSCTL_FIREWALL_CALEA_IP_MTU          "calea_ip_mtu"
#define ASAP_SYSCTL_FIREWALL_CALEA_STATS           "calea_stats"

#define ASAP_SYSCTL_FIREWALL_CLEAR_DP_SESSION       "clear_dp_session"
#define ASAP_SYSCTL_FIREWALL_CLEAR_DP_STATISTICS    "clear_dp_statistics"
#define ASAP_SYSCTL_FIREWALL_MCAST_GROUP           "mcast_group"
#define ASAP_SYSCTL_FIREWALL_UPD_NAMED_TUNNEL       "named_tunnel"
#define ASAP_SYSCTL_FIREWALL_PORT_MIRROR            "port_mirror"
#define ASAP_SYSCTL_FIREWALL_LMS                    "lms"
#define ASAP_SYSCTL_SET_CERT_CAP                   "cert_cap"
#ifdef __FIPS_BUILD__
#define ASAP_SYSCTL_OPEN_OCSP_PORTS                   "ocsp_ports"
#define ASAP_SYSCTL_CLOSE_OCSP_PORTS                  "ocsp_ports_close"
#endif
#define ASAP_SYSCTL_OPEN_EST_PORTS                    "est_ports"
#define ASAP_SYSCTL_CLOSE_EST_PORTS                   "est_ports_close"
#define ASAP_SYSCTL_FIREWALL_ARP_ATTACK_PROTECTION      "arp_attack_protection"
#define ASAP_SYSCTL_GRE_OFFLOAD_ENABLE              "gre_offload_enable"
#define ASAP_SYSCTL_BR_OFFLOAD_ENABLE               "br_offload_enable"
#define ASAP_SYSCTL_UOL_CONFIG                             "uol_config"
#define ASAP_SYSCTL_NSS_CRASHED                     "nss_crashed"
#define ASAP_SYSCTL_ACTIVE_UPLINK                  "active_uplink"

#define ASAP_SYSCTL_BLE_RESET                      "ble_reset"

#define ASAP_SYSCTL_GRE_OFFLOAD_DEBUG                      "gre_offload_debug"
#define ASAP_SYSCTL_UOL_IPR_STATS                  "uol_ipr_stats"
#ifdef __FAT_AP__
#define ASAP_SYSCTL_FIREWALL_DPI                   "enable_dpi"
#define ASAP_SYSCTL_FIREWALL_DPI_CONTROL           "allowed_dpi_sessions"
#define ASAP_SYSCTL_FIREWALL_DPI_HTTP_DENY         "enable_http_deny"
#define ASAP_SYSCTL_FIREWALL_TCP_MSS               "firewall_tcp_mss"
#define ASAP_SYSCTL_FIREWALL_DPI_HTTPS_DIP_CACHE   "https_dip_cache" 
#define ASAP_SYSCTL_FIREWALL_DEBUG_VOIP            "debug_voip"
#define ASAP_SYSCTL_FIREWALL_CS_MAX_CPU            "cs_max_cpu_limit"
#endif

#ifdef __FAT_AP__  // DPI_ERROR_PAGE
#define ASAP_SYSCTL_FIREWALL_ROLE_DPI_ERR_URL      "role_dpi_err_url"
#endif // DPI_ERROR_PAGE

#ifdef __FAT_AP__  // CLARITY LIVE
#define ASAP_SYSCTL_FIREWALL_CLARITYLIVE_EVENT      "claritylive_event"
#endif 

#ifdef __FAT_AP__  // NO_AP_IGMP
#define ASAP_SYSCTL_FIREWALL_CLEAR_IGMP_GROUPS     "clear_fw_igmp_groups"
#define ASAP_SYSCTL_FIREWALL_CLEAR_IGMP_STATS      "clear_fw_igmp_stats"
#define ASAP_SYSCTL_FIREWALL_ETH_LOOP_DETECTED     "eth_loop_detected"
#define ASAP_SYSCTL_FIREWALL_MOB_SESS_REQUEST      "mob_sess_request"
#define ASAP_SYSCTL_FIREWALL_CLEAR_USER            "clear_user"
#define ASAP_SYSCTL_FIREWALL_SSL_THROTTLE          "ssl_throttle"
#endif // NO_AP_IGMP

#define ASAP_SYSCTL_AMS_ACTIVE_VAP               "ams_active_vap"
#define ASAP_SYSCTL_HW_OPMODE                   "hw_opmode"

/* Per AP nodes managed by ANUL under ARUBA_ANUL_NAME */
#define ANUL_SYSCTL_AP_CONTROLLER_MAC              "controller_mac"
#define ANUL_SYSCTL_AP_DOS_STA_ADD_MAC             "dos_sta_add_mac"
#define ANUL_SYSCTL_AP_DOS_STA_REMOVE_MAC          "dos_sta_remove_mac"
#define ANUL_SYSCTL_AP_DOS_STA_MAC_PRESENT         "dos_sta_mac_present"
#define ANUL_SYSCTL_AP_DOS_STA_PURGE               "dos_sta_purge"

/* Per AP nodes for output under proc/net */
#define ANUL_NET_AP_DOS_STA_DUMP                   "anul_dos_sta_dump"
#define ANUL_NET_AP_STATS                          "anul_ap_stats"

#define PROC_MAX_EIRP_PER_CHAN                     "max_eirp_per_chan"
#define PROC_MIN_EIRP_PER_CHAN                     "min_eirp_per_chan"

/* Nodes relevant to Radio Specific Parameters */
#ifdef __FAT_AP__
#define WIF_SYSCTL_DFS_CHAN_EMPTY       "dfs_chan_empty"
#endif 
#define WIF_SYSCTL_ENABLE               "enable"
#define WIF_SYSCTL_CHANNEL              "channel"
#define WIF_SYSCTL_NCHANNEL             "nchannel"
#define WIF_SYSCTL_BEACON_PERIOD        "beacon_period"
#define WIF_SYSCTL_BEACON_REGULATE      "beacon_regulate"
#define WIF_SYSCTL_TX_POWER             "tx_power"
#define WIF_SYSCTL_TPC_POWER            "tpc_power"
#define WIF_SYSCTL_ANTENNA              "antenna"
#define WIF_SYSCTL_INT_ANTENNA          "int_ant_mode"
#define WIF_SYSCTL_ANTENNA_GAIN_24      "ant_gain_24"
#define WIF_SYSCTL_ANTENNA_GAIN_5       "ant_gain_5"
#define WIF_SYSCTL_ANTENNA_GAIN_DUAL5G  "ant_gain_dual5g"
#define WIF_SYSCTL_COUNTRY              "country"
#define WIF_SYSCTL_COMMIT               "commit"
#define WIF_SYSCTL_SCAN                 "scan"
#define WIF_SYSCTL_SCAN_OVERRIDE        "scan_override"
#define WIF_SYSCTL_KEYTABLE				"keytable"
#define WIF_SYSCTL_MODE					"mode"
#define WIF_SYSCTL_RFPROTECT            "rfprotect"
#define WIF_SYSCTL_PS_AWARE_SCAN        "ps_aware_scan"
#define WIF_SYSCTL_CLIENT_BALANCING     "client_balancing"
#define WIF_SYSCTL_CLIENT_BALANCING_STATE     "client_balancing_state"
#define WIF_SYSCTL_ACTIVE_VOICE_CLIENT     "active_voice_client"
#define WIF_SYSCTL_ADD_BLIST_MAC         "add_blist_mac"
#define WIF_SYSCTL_CLIENT_BALANCING_MODE     "client_balancing_mode"
#define WIF_SYSCTL_CLIENT_BALANCING_UPDATE_INTVL "client_balancing_update_intvl"
#define WIF_SYSCTL_CLIENT_BALANCING_THRESHOLD "client_balancing_threshold"
#define WIF_SYSCTL_CLIENT_MANAGEMENT    "client_management"
#define WIF_SYSCTL_CLIENT_MANAGEMENT_UPDATE_INTERVAL "client_management_update_intvl"
#define WIF_SYSCTL_CHANNEL_REUSE        "channel_reuse"
#define WIF_SYSCTL_CHANNEL_REUSE_THRESHOLD "channel_reuse_threshold"
#define WIF_SYSCTL_VOICE_AWARE_SCAN     "voice_aware_scan"
#define WIF_SYSCTL_VIDEO_AWARE_SCAN     "video_aware_scan"
#define WIF_SYSCTL_LOAD_AWARE_SCAN_THRESHOLD     "load_aware_scan_threshold"
#define WIF_SYSCTL_ACTIVE_SCAN          "active_scan"
#define WIF_SYSCTL_OTA_UPDATES          "ota_updates"
#define WIF_SYSCTL_UAPSD_STATS          "uapsd_stats"
#define WIF_SYSCTL_CSA_ENABLE		"csa_enable"
#define WIF_SYSCTL_CSA_COUNT		"csa_count"
#define WIF_SYSCTL_11H_ENABLE		"11h"
#define WIF_SYSCTL_OVERRIDE_MAXEIRP		"override_maxeirp"
#define WIF_SYSCTL_BSS_AAC_VOICE            "bss_aac_voice"
#define WIF_SYSCTL_QBSS_ALLOW		"qbss_allow"
#define WIF_SYSCTL_VOIP_PCT_REACHED     "voip_pct_reached"
#define WIF_SYSCTL_THROTTLE_INTERVAL	"mgmt_throt_intvl"
#define WIF_SYSCTL_THROTTLE_LIMIT	"mgmt_throt_limit"
#define WIF_SYSCTL_PERF_BOOST        "perf_boost"
#define WIF_SYSCTL_RADIO_TYPE 		"radio_type"
#define WIF_SYSCTL_MAX_DISTANCE               "max_distance"
#define WIF_SYSCTL_VERIWAVE_HACK	"veriwave_hack"
#define WIF_SYSCTL_AP_CHANNEL_LIST      "ap_channel_list"
#define WIF_SYSCTL_VOICE_AVLBL_BW  "voice_avlbl_bw"
#define WIF_SYSCTL_HT_ENABLE		"ht_enable"
#define WIF_SYSCTL_MUTE_ENABLE		"mute_enable"
#define WIF_SYSCTL_VHT_ENABLE		"vht_enable"
#define WIF_SYSCTL_HE_ENABLE        "he_enable"
#define WIF_SYSCTL_TURBOQAM_ENABLE      "turboqam_enable"
#define WIF_SYSCTL_40MHZ_ENABLE		"40mhz_enable"
#define WIF_SYSCTL_80MHZ_ENABLE		"80mhz_enable"
#define WIF_SYSCTL_SECONDARY_CHANNEL	"sec_chan"
#define WIF_SYSCTL_40MHZ_INTOLERANT	"40mhz_intol"
#define WIF_SYSCTL_HONOR_40MHZ_INTOLERANT "honor_40mhz_intol"
#define WIF_SYSCTL_ARUBA_FILTER         "aruba_mac_filter"
#define WIF_SYSCTL_MESH_ID              "mesh_id"
#define WIF_SYSCTL_MESH_MBITS           "mesh_mbits"
#define WIF_SYSCTL_MESH_CMN_STRUCT      "mesh_cmn_struct"
#define WIF_SYSCTL_MESH_PORTAL_ID       "mesh_portal_id"
#define WIF_SYSCTL_DOT11B_PROT          "dot11b_prot"
#define WIF_SYSCTL_SPECTRUM_CHANNEL     "spectrum_channel"
#define WIF_SYSCTL_SPECTRUM_TEST     	"spectrum_test"
#define WIF_SYSCTL_SINGLE_CHAIN_LEGACY	"single_chain_legacy"
#define WIF_SYSCTL_INTF_IMMUNITY	"intf_immunity"
#define WIF_SYSCTL_CHECK_DFS        "check_dfs"
#define WIF_SYSCTL_NOISE_FLOOR      "noise_floor"
#define WIF_SYSCTL_SPECTRUM_CH_USAGE    "spectrum_ch_usage"
#define WIF_SYSCTL_TXCHAINMASK          "txchainmask"
#define WIF_SYSCTL_BACK_TO_BACK_TX	"back_to_back_tx"
#define WIF_SYSCTL_RATE			"rate"
#define WIF_SYSCTL_MTU			"mtu"
#define WIF_SYSCTL_NUM_PKTS		"num_pkts"
#define WIF_SYSCTL_JAM_TX		"jam_tx"
#define WIF_SYSCTL_TX_PKTS_MODE		"tx_pkts_mode"
#define WIF_SYSCTL_EXTRA_AGGR_PAD	"aggr_pad" /* For grenache to avoid underrun */
#define WIF_SYSCTL_TXBYTESCUR		"txbytescur"
#define WIF_SYSCTL_RXBYTESCUR		"rxbytescur"
#define WIF_SYSCTL_TXBYTESPERSEC	"txbytespersec"
#define WIF_SYSCTL_RXBYTESPERSEC	"rxbytespersec"
#define WIF_SYSCTL_FRAME_STEERING   "steer" /* Enable Frame steering for a specific client */
#define WIF_SYSCTL_STA_RTP_PARAMS    "sta_rtp_params"
#define WIF_SYSCTL_STA_RTP_STOP    "sta_rtp_stop"
#define WIF_SYSCTL_SPECTRUM_CHAN_TIME  "spectrum_chan_time"
#define WIF_SYSCTL_SPECTRUM_AP_FFT  "spectrum_ap_fft"
#define WIF_SYSCTL_SPECTRUM_AP_FFT_TIME  "spectrum_ap_fft_time"
#define WIF_SYSCTL_SPECTRUM_FFT_FORMAT  "spectrum_fft_format"
#ifdef SPECTRUM_BRCM_WIFI_WORKAROUND
#define WIF_SYSCTL_SPECTRUM_SM_FFT  "spectrum_sm_fft"
#endif
#define WIF_SYSCTL_RX_WIFI_SENSITIVITY "rx_sensitivity_wifi"
#define WIF_SYSCTL_RX_INTERFERENCE_SENSITIVITY "rx_sensitivity_interference"
#define WIF_SYSCTL_TX_ED_THRESHOLD "tx_ed_threshold"
#define WIF_SYSCTL_FIXED_NOISE_FLOOR "fixed_noise_floor"
#define WIF_SYSCTL_REG_INFO_UPD      "reg_info"
#define WIF_SYSCTL_AP_ASSERT      "ap_assert"
#define WIF_SYSCTL_AP_FW_HANG    "ap_fw_hang"
#define WIF_SYSCTL_BUF_RES_BE    "buf_res_be"
#define WIF_SYSCTL_BUF_RES_VI    "buf_res_vi"
#define WIF_SYSCTL_BUF_RES_VO    "buf_res_vo"
#define WIF_SYSCTL_BUF_RES_HIQ   "buf_res_hiq"
#define WIF_SYSCTL_FREE_NI        "free_ni" /* For manual free_node, ni leak dbg */
#define WIF_SYSCTL_WIGIG_BOARD_DATA  "board_data"

#define WIF_SYSCTL_CLR_STA_MFP    	"clr_sta_mfp"
#define WIF_SYSCTL_SWITCH_MAC           "switch_mac"  
#define WIF_SYSCTL_LATITUDE           "latitude" 
#define WIF_SYSCTL_LONGITUDE          "longitude"
#define WIF_SYSCTL_ALTITUDE           "altitude"
#define WIF_SYSCTL_BUF_RES_BE    "buf_res_be"
#define WIF_SYSCTL_BUF_RES_VI    "buf_res_vi"
#define WIF_SYSCTL_BUF_RES_VO    "buf_res_vo"
#define WIF_SYSCTL_BUF_RES_HIQ   "buf_res_hiq"
#define WIF_SYSCTL_FREE_NI        "free_ni" /* For manual free_node, ni leak dbg */
#define WIF_SYSCTL_AP_NAME        "ap_name"
#define WIF_SYSCTL_PERF_PROMISC   "perf_promisc"
#define WIF_SYSCTL_LOCATION_FEED  "location-feed"
#define WIF_SYSCTL_CHANNEL_OVERRIDE "channeloverride"
#define WIF_SYSCTL_TRANSMIT "transmit"

#define WIF_SYSCTL_TIME             "ntime"
#define WIF_SYSCTL_CHANNEL_OVERRIDE "channeloverride"
#define WIF_SYSCTL_TXANTENNA        "txantenna" 
#define WIF_SYSCTL_RXANTENNA        "rxantenna"
#define WIF_SYSCTL_FAKE_RADAR       "fakeradar"
#define WIF_SYSCTL_AMS_MODE       "ams_mode"
#define WIF_SYSCTL_AMS_FWD_MODE       "ams_fwd_mode"
#define WIF_SYSCTL_AMS_CAPTURE       "ams_capture"

#define WIF_SYSCTL_RX_CYCLIC_POWER_THRESHOLD "rx_cyclic_power_thr"
#define WIF_SYSCTL_MAX_CLIENTS               "max_clients"
#define WIF_SYSCTL_VHT_BW_SIGNALING          "vht_bw_signaling"
#define WIF_SYSCTL_SMARTANT                  "smart_ant"
#define WIF_SYSCTL_VHT_TXBF_SOUNDING_INTERVAL "txbf_sounding_interval"
#define WIF_SYSCTL_RSDB_MODE                "rsdb_mode"
#define WIF_SYSCTL_FLASH_EIRP_LIMIT         "flash_eirp_limit"
#define WIF_SYSCTL_RECOVERY_MODE            "recovery_mode"
#define WIF_SYSCTL_RECOVERY_EVENTS          "recovery_events"
#define WIF_SYSCTL_RECOVERY_START           "recovery_start"
#define WIF_SYSCTL_RECOVERY_END             "recovery_end"
#define WIF_SYSCTL_RECOVERY_PC              "recovery_pc"
#define WIF_SYSCTL_DUAL5G_MODE              "dual5g_mode"

#define WIF_SYSCTL_BSS_COLOR                "bss_color"
#define WIF_SYSCTL_AM_COPY_OPT              "am_copy_opt"
#define WIF_SYSCTL_COEX_SES_ESL             "coex_ses_esl"

#define WIF_SYSCTL_ASSOC_BOOST              "assoc_boost"
#define WIF_SYSCTL_MGMT_RETRY_LIMIT         "mgmt_retry_limit"
#define WIF_SYSCTL_MGMT_SW_RETRY            "mgmt_sw_retry"
#define WIF_SYSCTL_ASSOC_AWARE_SCAN         "assoc_aware_scan"

#define WIF_COMMIT_APPLY_CHANGES	1 /* Only apply if radio params change */
#define WIF_COMMIT_APPLY_ALWAYS		2 /* always apply all radio params */

#ifdef CONFIG_EIGER
#define ASAP_SYSCTL_FAULT_FINDME_LED     "fault_findme_led"
#define ASAP_SYSCTL_CABLE_MODEM_LED      "cable_modem_led"
#endif
#define ASAP_SYSCTL_POWER_LED           "power_led"
#define ASAP_SYSCTL_WLANA_LED           "wlanA_led"
#define ASAP_SYSCTL_WLANB_LED           "wlanB_led"
#define ASAP_SYSCTL_DEV_LED             "dev_led"
#define ASAP_SYSCTL_DEV_HT              "dev_ht"
#define ASAP_SYSCTL_FAT_AP		"fat_ap"

#define ASAP_SYSCTL_GLOBAL_LED_MODE     "global_led_mode"
#define ASAP_SYSCTL_K2_LED_OVERRIDE      "k2_led_override"
#define ASAP_SYSCTL_K2_LED_FAULT_ACTION "k2_led_fault_action"
#define ASAP_SYSCTL_BLINK_ALL_LEDS	"blink_all_leds"
#define ASAP_SYSCTL_WIF_MESH_ALLOWED_VLANS "mesh_allowed_vlans"
#define ASAP_GTS_NAME			"asap_gretun_stats"
#ifdef __FAT_AP__
#define ASAP_SYSCTL_FIREWALL_IPV6_FW_ENABLE "firewall_ipv6_fw_enable"
#define ASAP_SYSCTL_FIREWALL_SESSION_VOIP_TIMEOUT   "session_voip_timeout"
#endif

#define MVETH_SYSCTL_ENABLE             "enable"
#define MVETH_SYSCTL_COUNTERS           "counters"
#define MVETH_SYSCTL_MESH_SSID          "mesh_id"
#define MVETH_SYSCTL_MESH_RSSID         "mesh_id_recovery"
#define MVETH_SYSCTL_MESH_CMN           "mesh_cmn_struct"
#define MVETH_SYSCTL_MESH_BITS          "mesh_mbits"
#define MVETH_SYSCTL_PORTAL             "portal_id"
#define MVETH_SYSCTL_LMSIP              "lms_ip"
#define MVETH_SYSCTL_RMPMPV             "rmp_mpv"
#define MVETH_SYSCTL_SEQNO              "seqno"
#define MVETH_SYSCTL_SUBTREE_WEIGHT     "subtree_weight"
#define MVETH_SYSCTL_PORTAL_MTU         "portal_mtu"
#define MVETH_SYSCTL_ENSLAVE            "enslave"
#define MVETH_SYSCTL_DESLAVE            "deslave"
#define MVETH_SYSCTL_SLAVES             "slaves"
#define MVETH_SYSCTL_LINKTBL            "link_table"
#define MVETH_SYSCTL_LINKDEL            "link_del"
#define MVETH_SYSCTL_FWRDTBL            "fwrd_table"


#define PROCSYSNET                      "/proc/sys/net/"
#define PROCSYSDEV                      "/proc/sys/dev/"
#define PROCSYSKERNEL                   "/proc/sys/kernel/"
#define ASAP_SJ_DEVNAME                 "sj"
#define ASAP_DOT3AZ_DEVNAME             "dot3az"
#define ASAP_DOT3BZ_DEVNAME             "dot3bz"
#define ASAP_PSE_DEVNAME                "pse"
#define ETH_DEVNAME                     "eth"
#define ETH_DEVNUM_FMT                  "%1d"
#define VAP_DEVNAME                     "aruba"
#define VAP_DEVNUM_FMT                  "%1d%02d"
#define WIFI_DEVNAME                    "wifi"
#define WIFI_11AD_DEVNAME                "wifi"
#define WIFI_DEVNUM_FMT                 "%1d"
#define ASAP_DEVNAME                    "aruba_asap"
#define ARUBA_ANUL_NAME                 "aruba_anul"
#define ASAP_SWITCH_DEVNAME             "asap_switch"
#define BRIDGE_DEVNAME                  "br0"
#define FW_DEVNAME                      "fw"
#define MESH_DEVNAME                    "mesh0"
#define MESH_11AD_DEVNAME               "radiobond0"
#define MESH_MGMT_DEVNAME               "meshmgmt"

#define ASAP_OPENFLOW_DEVNAME		    "oflow"
#define ARUBA_KDUMP_READY               "aruba_kdump_ready"
#define SAPD_SYSCTL_BUF_SIZE            512
#define SAPD_SYSCTL_NAME_SIZE           256

#define ARUBA_KDUMP_READY_NODENAME      PROCSYSKERNEL ARUBA_KDUMP_READY

/*Anyspot*/
#define VAP_SYSCTL_ANYSPOT_ENABLE                   "anyspot"
#define VAP_SYSCTL_ANYSPOT_EXCLUDE_LIST             "anyspot_exclude"
#define VAP_SYSCTL_ANYSPOT_CLEAR_EXCLUDE_LIST       "anyspot_e_clear"
#define VAP_SYSCTL_ANYSPOT_WILDCARD_EXCLUDE_LIST    "anyspot_wildcard_exclude"
#define VAP_SYSCTL_ANYSPOT_CLEAR_W_EXCLUDE_LIST     "anyspot_w_clear"
#define VAP_SYSCTL_ANYSPOT_INCLUDE_LIST             "anyspot_include"
#define VAP_SYSCTL_ANYSPOT_CLEAR_INCLUDE_LIST       "anyspot_i_clear"
#define ASAP_SYSCTL_FIREWALL_CLEAR_SBR_TABLE        "clear_sbr_table"

typedef enum _mgmt_plane_protect_action {
    PROTECTION_ACTION_THROTTLE,
    PROTECTION_ACTION_TYPE_SET,
    PROTECTION_ACTION_CLEAR_STATS
} mgmt_plane_protect_action;

typedef enum _mgmt_plane_protect_type {
    PROTECTION_TYPE_DISABLE,
    PROTECTION_TYPE_ENABLE,
    PROTECTION_TYPE_AUTO
} mgmt_plane_protect_type;

typedef enum _fast_switch_mode {
    FAST_SWITCH_DISABLE,
    FAST_SWITCH_AFTER_QUEUE,
    FAST_SWITCH_BEFORE_QUEUE,
    FAST_SWITCH_AUTO_QUEUE
} fast_switch_mode;

typedef enum _bss_fw_mode {
    FORWARD_TUNNEL_ENCRYPTED,  // cli value - tunnel
    FORWARD_BRIDGE,            // cli value - bridge
    FORWARD_TUNNEL_DECRYPTED,  // cli value - tunnel-decrypted
    FORWARD_SPLIT_TUNNEL       // cli value - split-tunnel
} bss_fw_mode;

typedef enum _arm_wids_override {
    ARM_WIDS_OVERRIDE_OFF = 0,
    ARM_WIDS_OVERRIDE_ON,
    ARM_WIDS_OVERRIDE_DYNAMIC
} arm_wids_override;

typedef enum _bcn_measurement_mode {
    BCN_RPT_MMODE_PASSIVE = 0,
    BCN_RPT_MMODE_ACTIVE_ALL_CHAN,
    BCN_RPT_MMODE_ACTIVE_CHAN_RPT,
    BCN_RPT_MMODE_TABLE,
} bcn_measurement_mode;


typedef enum _tsm_request_mode {
    TSM_REQUEST_MODE_NORMAL = 0,
    TSM_REQUEST_MODE_TRIGGERED,
} tsm_request_mode;

// HACK ALERT: define the max descriptors here so it can be used by asap too

#if defined(ARUBA_AP_HAS_HT_SUPPORT)
#ifdef CONFIG_ARRAN
#define ARUBA_MAX_TX_DESCRIPTORS 4096
#else
#define ARUBA_MAX_TX_DESCRIPTORS 512
#endif
#else
#define ARUBA_MAX_TX_DESCRIPTORS 200
#endif

// Adjust ack/cts timeouts for max_distance[meters] configured > SHORTEST_MAX_DISTANCE, otherwise defaults used.
#define SHORTEST_MAX_DISTANCE 600
#define OUTDOOR_DFLT_MAX_DISTANCE 16000
#define LIMIT_MAX_DISTANCE 100000   // Means initialize to maximum for outdoor MP mesh-radios (config will reduce).
#define USE_MAX_ACK_CTS_TMOUT (~0U) // NB: if changed also updating ar5212SetAckTimeout/ar5416SetAckTimeout
#define MAXDIST_TO_TIMEOUT(mint, d, t) do {                                \
        if (d > LIMIT_MAX_DISTANCE)                                     \
            d = LIMIT_MAX_DISTANCE;                                     \
            t = 18 + (d/150) + ((d%150)?1:0) + 3;                       \
            if (t < mint) t = mint;                                     \
    } while(0);
#define  TIMEOUT_TO_MAXDIST(t,d) do {           \
        d = (t? 150*(t-(18+3)): 0);             \
    } while(0);
#define INIT_OUTDOOR_MAXDIST(_ic, _is_outdoor) do { \
        if(_is_outdoor) { \
            if ((MESH_POINT == _ic->aruba_radio_type)) { \
                _ic->des_config.max_distance = LIMIT_MAX_DISTANCE; \
            } else if ((MESH_PORTAL == _ic->aruba_radio_type)) { \
                _ic->des_config.max_distance = OUTDOOR_DFLT_MAX_DISTANCE; \
            } \
        } \
    } while (0);

/* keep the first two flags as first three bits */
#define ASAP_BW_MGMT_FLAGS_INITIAL_COST    0x0001
#define ASAP_BW_MGMT_FLAGS_OVERFLOW        0x0002
#define ASAP_BW_MGMT_FLAGS_SHAPE           0x0004
#define ASAP_BW_MGMT_STA_FLAGS_CCK         0x0008
#define ASAP_BW_MGMT_STA_FLAGS_HT          0x0010
#define ASAP_BW_MGMT_FLAGS_RATE_TIME       0x0020
#define ASAP_BW_MGMT_STA_FLAGS_ACTIVE      0x0040
#define ASAP_BW_MGMT_STA_FLAGS_VHT         0x0080 

enum _am_opt_flags {
    AM_OPT_FLAG_SUCCESS          =     0x00,
    AM_OPT_FLAG_NOBUF            =     0x01,
    AM_OPT_FLAG_CPU_OVRLD        =     0x02,
    AM_OPT_FLAG_ZEROBUF          =     0X04,
    AM_OPT_FLAG_FILTER_FAIL      =     0x10,
    AM_OPT_FLAG_MISC_FAIL        =     0x20
};

#ifdef __KERNEL__
#define CHK_PRN(buf, len, size, tmpbuf, tmpsz, fmt, args...) \
{ \
    int l; \
    snprintf(tmpbuf, tmpsz, fmt, ## args) ; \
    l = strlen(tmpbuf) ; \
    if ((len) + l + 1 >= size) goto toobig ; \
    strlcpy((buf), tmpbuf, l + 1) ; \
    (len) += l; \
    (buf) += l; \
}
#endif /* __KERNEL__ */

/* junk value that means use the minimum rate */
#define ARUBA_DEFAULT_BEACON_RATE_VAL	65535

/* junk value that means use the minimum rate */
#define ARUBA_DEFAULT_MCAST_RATE_VAL  65535

#ifdef __FAT_AP__
static __inline int aruba_dmo_ap_fwd_mode(bss_fw_mode mode)
{
    return (mode == FORWARD_BRIDGE);
}
#else
static __inline int aruba_dmo_ap_fwd_mode(bss_fw_mode mode)
{
    return ((mode == FORWARD_TUNNEL_DECRYPTED) || 
            (mode == FORWARD_SPLIT_TUNNEL));
}
#endif

#define WFA_OUI             0x506F9A
#define WFA_OUI_LEN         3       /* 3 Bytes */
#define WFA_OUI_0           ((WFA_OUI >> 16) & 0xFF)
#define WFA_OUI_1           ((WFA_OUI >> 8) & 0xFF)
#define WFA_OUI_2           (WFA_OUI & 0xFF)

#define CMP_WFA_OI(oi, match)   \
{                                           \
    if (oi[0] == WFA_OUI_0) {          \
        if (oi[1] == WFA_OUI_1) {      \
            if (oi[2] == WFA_OUI_2) {  \
               match = 1;                   \
            }                               \
        }                                   \
    } else {                                \
       match = 0;                           \
    }                                       \
}

#define SET_WFA_OI(oi)      \
{                           \
    oi[0] = WFA_OUI_0; \
    oi[1] = WFA_OUI_1; \
    oi[2] = WFA_OUI_2; \
}

#define P2P_SET_OI(oi)      SET_WFA_OI(oi)
#define HS2_SET_OI(oi)      SET_WFA_OI(oi)

#define HS2_CMP_OI(oi,match)      CMP_WFA_OI(oi,match)

/*Mesh Virtual Device related sysctl defines*/
enum client_operate_t {
    CLIENT_OP_STOP,
    CLIENT_OP_CONNECT_SSID,
    CLIENT_OP_CONNECT_BSSID,
};

#endif
