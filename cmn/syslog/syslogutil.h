#ifndef SYSLOGMISC
#define SYSLOGMISC

struct code {
        char       *c_name;
        int        c_val;
};

typedef struct facInfo_s{
        char*        facName; //Keyword in the CLI commands.
        int          facVal;  // A unique Facility ID.
        int          service; // PAPI Service number for the application
        char*        desc;    // Help string used in the CLI.
		int isTrace;
		int isDebug;
		char* confName;
}facInfo_t;

#define MAX_SUB_CAT	32

#define SUBCAT_ALL	0

//Sub categories for network

#define SUBCAT_NETWORK_PACKET_DUMP 1 
#define SUBCAT_NETWORK_MOBILEIP 2 
#define SUBCAT_NETWORK_DHCP 3 
#define SUBCAT_NETWORK_SDN 4 
#define SUBCAT_NETWORK_CLUSTER 5
#define SUBCAT_NETWORK_GP 6
//Sub categories for security
#define SUBCAT_SECURITY_AAA 1 
#define SUBCAT_SECURITY_FIREWALL 2 
#define SUBCAT_SECURITY_PACKET_TRACE 3 
#define SUBCAT_SECURITY_MOBILITY 4 
#define SUBCAT_SECURITY_VPN 5 
#define SUBCAT_SECURITY_DOT1X 6 
#define SUBCAT_SECURITY_IKE 7 
#define SUBCAT_SECURITY_WEBSERVER 8
#define SUBCAT_SECURITY_NTLM 9
#define SUBCAT_SECURITY_KERBEROS 10
#define SUBCAT_SECURITY_IDS 11
#define SUBCAT_SECURITY_IDS_AP 12
#define SUBCAT_SECURITY_CPSEC 13
#define SUBCAT_SECURITY_DB 14
#define SUBCAT_SECURITY_CERTMGR 15
#define SUBCAT_SECURITY_WL_SYNC 16
#define SUBCAT_SECURITY_HA 17
#define SUBCAT_SECURITY_AMON 18
#define SUBCAT_SECURITY_CPNW 19
#define SUBCAT_SECURITY_CLUSTER 20
#define SUBCAT_SECURITY_CERTINIT 21
#define SUBCAT_SECURITY_EST 22
#define SUBCAT_SECURITY_NGINX 23
//Sub categories for system
#define SUBCAT_SYSTEM_CONFIGURATION 1 
#define SUBCAT_SYSTEM_MESSAGES 2 
#define SUBCAT_SYSTEM_SNMP 3 
#define SUBCAT_SYSTEM_WEBSERVER 4 
#define SUBCAT_SYSTEM_UPLINK 5
#define SUBCAT_SYSTEM_AP 6
#define SUBCAT_SYSTEM_AMON 7
#define SUBCAT_SYSTEM_HA 8
#define SUBCAT_SYSTEM_MAPC 10
#define SUBCAT_SYSTEM_PAN 11
#define SUBCAT_SYSTEM_AMON_ALE 12
#define SUBCAT_SYSTEM_AMON_AMP 13
#define SUBCAT_SYSTEM_REG_TBL  14
#define SUBCAT_SYSTEM_GP 15
#define SUBCAT_SYSTEM_CLUSTER 16
#define SUBCAT_SYSTEM_VALIDATION 17
#define SUBCAT_SYSTEM_CPNW 18
#define SUBCAT_SYSTEM_OFC_FLOW_MANAGER 19
#define SUBCAT_SYSTEM_OFC_TOPOLOGY 20
#define SUBCAT_SYSTEM_OFC_TOPOLOGY_DISCOVERY 21
#define SUBCAT_SYSTEM_OFC_ROUTING_SWITCH 22
#define SUBCAT_SYSTEM_OFC_SWITCH_MANAGER 23
#define SUBCAT_SYSTEM_OFC_PACKET_DISPATCHER 24
#define SUBCAT_SYSTEM_OFC_EVENT_DISPATCHER 25
#define SUBCAT_SYSTEM_AP_CONFIG 26
#define SUBCAT_SYSTEM_TUNNELED_NODE_MGR 27
#define SUBCAT_SYSTEM_AP_LICENSE  28
#define SUBCAT_SYSTEM_NGINX 29
#define SUBCAT_SYSTEM_MON 30 
#define SUBCAT_SYSTEM_ANALYTICS 31
//Sub categories for user
#define SUBCAT_USER_CAPTIVE_PORTAL 1 
#define SUBCAT_USER_VPN 2 
#define SUBCAT_USER_DOT1X 3 
#define SUBCAT_USER_RADIUS 4 
#define SUBCAT_USER_ARM_CM 6
#define SUBCAT_USER_MAPC 7
#define SUBCAT_USER_PAN 8
#define SUBCAT_USER_CPNW 9
//Sub categories for user_debug
#define SUBCAT_USER_DEBUG_VLAN 1 
// Sub categories for mvc
#define SUBCAT_MVC_CC		1	/* call control */
#define SUBCAT_MVC_DM		2	/* device management */
#define SUBCAT_MVC_CFG		3	/* configuration */
// Sub categories for arm 
#define SUBCAT_ARM_CM		1	/* client management */
#define SUBCAT_ARM_RM		2	/* radio resource  management */

// Sub categories for ap-debug
#define SUBCAT_APDEBUG_HA   1   /* HA */
#define SUBCAT_APDEBUG_AP_CONFIG 2
#define SUBCAT_APDEBUG_SDN  3

typedef struct facSubCat_s{
	int fac;
	struct subCatInfo_s{
		char* tag;
		int sId;
		char* desc;
	}subCatList[MAX_SUB_CAT];
}facSubCat_t;

//Search Type Definitions.
typedef enum sKeyTypes_e{
	SEARCH_BY_MAC,
	SEARCH_BY_IP,
	SEARCH_BY_NAME,
	SEARCH_BY_STRING,
	SEARCH_BY_INT,
}sKeyTypes_e;

#define DEBUG_FAC_AP_HID	0	
#define DEBUG_FAC_BSSID_HID	1
#define DEBUG_FAC_ESSID_HID	2
#define DEBUG_FAC_USER_HID	3
typedef struct debugFacInfo_s{
        char*        facName; //Keyword in the CLI commands.
        char*        desc;    // Help string used in the CLI.
        int          facVal;  // A unique Facility ID.
		sKeyTypes_e	 sType;
}debugFacInfo_t;


extern struct code lglvlNames[];
extern facInfo_t   arubaFacilities[];
extern debugFacInfo_t arubaDebugFacilities[];
extern facSubCat_t arubaSubCat[];
#define ARUBA_LOG_FACILITY_START        24  /* The Maximum defined for the standard
                                             * SYSYLOG is 23 */
                   
/*
 * To define a new log facility, and an item to each of the following two
 * enums.
 */

typedef enum {
    _LOG_IKE,
    _LOG_L2TP,
    _LOG_PPTP,
    _LOG_STM,
    _LOG_WMS,
    _LOG_PLATFORM,
    _LOG_NANNY,
    _LOG_ARUBA_AUTH,
    _LOG_DOT1X_PROC,
    _LOG_USERDB,
    _LOG_OFFLDR,
    _LOG_FPAPPS,
    _LOG_SUSER,
    _LOG_INTUSER,
    _LOG_AAA,
    _LOG_TRAFFIC,
    _LOG_DHCPD,
    _LOG_PUBLISHER,
    _LOG_MIP,
    _LOG_ESI,
    _LOG_PIM,
    _LOG_SNMP,
    _LOG_ARUBAFILTER,
    _LOG_LICENSEMGR,
    _LOG_FGT,
    _LOG_HTTPD,
    _LOG_PPPOED,
	_LOG_NETWORK, 
	_LOG_SECURITY, 
	_LOG_SYSTEM, 
	_LOG_WIRELESS,
	_LOG_APDEBUG, 
	_LOG_BSSIDDEBUG,
	_LOG_ESSIDDEBUG,
	_LOG_USERDEBUG,
	_LOG_ARMUSERDEBUG,
       _LOG_UPLOAD_SYNC_MGR,
#ifdef EMS_PLATFORM 
    _LOG_SYSMAP,
    _LOG_STATSMGR,
    _LOG_FAULTMGR,
    _LOG_SPOTMGR,
    _LOG_ZMCGI,
    _LOG_ADMINSERVER,
    _LOG_HAMGR,
    _LOG_CONFIGMGR,
#endif
    _LOG_PROFMGR,
	_LOG_SYSLOGDWRAP,
#ifdef NCFG_TEST
    _LOG_NCFGTEST,
#endif
    _LOG_RFM,
    _LOG_CTS,
    _LOG_CERTMGR,
    _LOG_DBSYNC,
    _LOG_MVC,
    _LOG_ARM,
    _LOG_APPROC,
    _LOG_PPPD,
    _LOG_MISC_PROC,
    _LOG_OSPF,
    _LOG_HWMON,
    _LOG_CMICM,
    _LOG_CMICA,
    _LOG_CMIPHYTASK,
    _LOG_NPPPD,
    _LOG_CPSEC,
    _LOG_SPECTRUM,
    _LOG_PHM,
    _LOG_SAPD,
    _LOG_CLI,
    _LOG_MESHD,
#ifndef CONFIG_GRENACHE
    _LOG_MDNS,
    _LOG_EXTIF_MGR,
#endif
#if defined (CONFIG_GRENACHE) || defined (CONFIG_NEBBIOLO) || defined (CONFIG_OPUSONE)
#else
    _LOG_SURVIVAL,
#endif
    _LOG_IAP_MGR,
    _LOG_RADVD,
    _LOG_UTILPROC,
    _LOG_GSM_MGR,
    _LOG_HA_MGR,
    /*
     * _LOG_MON_MGR,
     */
#ifndef CONFIG_OPUSONE
    _LOG_ACTIVATE,
#endif
    _LOG_UCM,
    _LOG_IPSTM,
    _LOG_ARMD,
    _LOG_LLDP,
    _LOG_DDS,
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_SHUMWAY) || defined (CONFIG_MILAGRO)
    _LOG_CLUSTER_MGR,
    _LOG_TUNNELED_NODE_MGR,
#endif
    _LOG_SC_REPLICATION_MGR,
    _LOG_RSYNC,
    _LOG_WEB_CC,
    _LOG_DHCPPROXY,
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_OPUSONE) || defined(CONFIG_NEBBIOLO) || defined (CONFIG_MILAGRO)
    _LOG_BOC_MGR,
#endif
    _LOG_HCM,
    _LOG_CERT_DWNLD,
#ifndef CONFIG_GRENACHE
    _LOG_RESOLVE,

#endif
    _LOG_BLE_RELAY,
#if defined (CONFIG_OPUSONE) || defined(CONFIG_SHUMWAY)
    _LOG_RNG_MGR,
#endif
#ifndef CONFIG_OPUSONE 
    _LOG_OFA,
    _LOG_IP_FLOW_EXPORT,
#endif
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_OPUSONE) || defined (CONFIG_MILAGRO)
    _LOG_AMON_RECVR,
#endif
#ifdef CONFIG_OPUSONE
    _LOG_MCELL,
    _LOG_MON_SERV,
    _LOG_MON_SERV_FWV,
    _LOG_NBAPI_HELPER_PROC,
    _LOG_OPENFLOW_CONTROLLER,
    _LOG_LOGFWD,
    _LOG_CLUSTER_UPGRADE_MGR,
#endif
    _LOG_WEB_DAEMON,
    _LOG_CFGDIST,
    _LOG_UPGRADEMGR,
    _LOG_DPAGENT,
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_OPUSONE) || defined (CONFIG_SHUMWAY) || defined (CONFIG_MILAGRO)
    _LOG_APPRF,
#endif

#ifndef CONFIG_OPUSONE    
    _LOG_RTPA,
#endif    
    _LOG_AMON_SENDER,
    _LOG_CERTINIT,

#ifdef __FAT_AP__
    _LOG_DPIMGR,
    _LOG_LHM,
#endif
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_MILAGRO) || defined (CONFIG_SHUMWAY)
    _LOG_CENTRAL,
#endif
    _LOG_MSGHH,
    _LOG_CTAMON,
#ifdef CONFIG_OPUSONE
    _LOG_SDWAN,
#endif
    _LOG_UTILD,
#ifndef CONFIG_OPUSONE    
    _LOG_POLICYMGR,
#endif
    _LOG_VRRP,
    _LOG_LAGM,
    _LOG_UV,
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_MILAGRO)
    _LOG_APIMAGEMGR,
#endif
#ifdef CONFIG_OPUSONE
    _LOG_IM_HELPER,
#endif
    _LOG_WPA3_SAE,
    /*
     * Add new values above this comment.
     */
    LOG_NARUBAFACILITIES
} aruba_log_fac_index_t;

#define ARUBA_LOG_FAC_DEF(fac)  \
    fac = ((ARUBA_LOG_FACILITY_START + _##fac) << 3)

typedef enum {
    ARUBA_LOG_FAC_DEF(LOG_IKE),
    ARUBA_LOG_FAC_DEF(LOG_L2TP),
    ARUBA_LOG_FAC_DEF(LOG_PPTP),
    ARUBA_LOG_FAC_DEF(LOG_STM),
    ARUBA_LOG_FAC_DEF(LOG_WMS),
    ARUBA_LOG_FAC_DEF(LOG_PLATFORM),
    ARUBA_LOG_FAC_DEF(LOG_NANNY),
    ARUBA_LOG_FAC_DEF(LOG_ARUBA_AUTH),
    ARUBA_LOG_FAC_DEF(LOG_DOT1X_PROC),
    ARUBA_LOG_FAC_DEF(LOG_USERDB),
    ARUBA_LOG_FAC_DEF(LOG_OFFLDR),
    ARUBA_LOG_FAC_DEF(LOG_FPAPPS),
    ARUBA_LOG_FAC_DEF(LOG_SUSER),
    ARUBA_LOG_FAC_DEF(LOG_INTUSER),
    ARUBA_LOG_FAC_DEF(LOG_AAA),
    ARUBA_LOG_FAC_DEF(LOG_TRAFFIC),
    ARUBA_LOG_FAC_DEF(LOG_DHCPD),
    ARUBA_LOG_FAC_DEF(LOG_PUBLISHER),
    ARUBA_LOG_FAC_DEF(LOG_MIP),
    ARUBA_LOG_FAC_DEF(LOG_ESI),
    ARUBA_LOG_FAC_DEF(LOG_PIM),
    ARUBA_LOG_FAC_DEF(LOG_SNMP),
    ARUBA_LOG_FAC_DEF(LOG_ARUBAFILTER),
    ARUBA_LOG_FAC_DEF(LOG_LICENSEMGR),
    ARUBA_LOG_FAC_DEF(LOG_FGT),
    ARUBA_LOG_FAC_DEF(LOG_HTTPD),
    ARUBA_LOG_FAC_DEF(LOG_PPPOED),
	ARUBA_LOG_FAC_DEF(LOG_NETWORK),
	ARUBA_LOG_FAC_DEF(LOG_SECURITY),
	ARUBA_LOG_FAC_DEF(LOG_SYSTEM),
	ARUBA_LOG_FAC_DEF(LOG_WIRELESS),
	ARUBA_LOG_FAC_DEF(LOG_APDEBUG),
	ARUBA_LOG_FAC_DEF(LOG_BSSIDDEBUG),
	ARUBA_LOG_FAC_DEF(LOG_ESSIDDEBUG),
	ARUBA_LOG_FAC_DEF(LOG_USERDEBUG),
	ARUBA_LOG_FAC_DEF(LOG_ARMUSERDEBUG),
        ARUBA_LOG_FAC_DEF(LOG_UPLOAD_SYNC_MGR),
#ifdef EMS_PLATFORM 
    ARUBA_LOG_FAC_DEF(LOG_SYSMAP),
    ARUBA_LOG_FAC_DEF(LOG_STATSMGR),
    ARUBA_LOG_FAC_DEF(LOG_FAULTMGR),
    ARUBA_LOG_FAC_DEF(LOG_SPOTMGR),
    ARUBA_LOG_FAC_DEF(LOG_ZMCGI),
    ARUBA_LOG_FAC_DEF(LOG_ADMINSERVER),
    ARUBA_LOG_FAC_DEF(LOG_HAMGR),
    ARUBA_LOG_FAC_DEF(LOG_CONFIGMGR),
#endif
    ARUBA_LOG_FAC_DEF(LOG_PROFMGR),
    ARUBA_LOG_FAC_DEF(LOG_SYSLOGDWRAP),
#ifdef NCFG_TEST
    ARUBA_LOG_FAC_DEF(LOG_NCFGTEST),
#endif
    ARUBA_LOG_FAC_DEF(LOG_RFM),
    ARUBA_LOG_FAC_DEF(LOG_CTS),
    ARUBA_LOG_FAC_DEF(LOG_CERTMGR),
    ARUBA_LOG_FAC_DEF(LOG_DBSYNC),
    ARUBA_LOG_FAC_DEF(LOG_MVC),
    ARUBA_LOG_FAC_DEF(LOG_ARM),
    ARUBA_LOG_FAC_DEF(LOG_APPROC), 
    ARUBA_LOG_FAC_DEF(LOG_PPPD), 
    ARUBA_LOG_FAC_DEF(LOG_MISC_PROC), 
    ARUBA_LOG_FAC_DEF(LOG_OSPF), 
    ARUBA_LOG_FAC_DEF(LOG_HWMON), 
    ARUBA_LOG_FAC_DEF(LOG_CMICM), 
    ARUBA_LOG_FAC_DEF(LOG_CMICA), 
    ARUBA_LOG_FAC_DEF(LOG_CMIPHYTASK), 
    ARUBA_LOG_FAC_DEF(LOG_NPPPD), 
    ARUBA_LOG_FAC_DEF(LOG_CPSEC), 
    ARUBA_LOG_FAC_DEF(LOG_SPECTRUM), 
    ARUBA_LOG_FAC_DEF(LOG_PHM), 
    ARUBA_LOG_FAC_DEF(LOG_SAPD), 
    ARUBA_LOG_FAC_DEF(LOG_CLI), 
    ARUBA_LOG_FAC_DEF(LOG_MESHD), 
#ifndef CONFIG_GRENACHE
    ARUBA_LOG_FAC_DEF(LOG_MDNS),
    ARUBA_LOG_FAC_DEF(LOG_EXTIF_MGR), 
#endif
#if defined (CONFIG_GRENACHE) || defined (CONFIG_NEBBIOLO) || defined (CONFIG_OPUSONE)
#else
    ARUBA_LOG_FAC_DEF(LOG_SURVIVAL),
#endif
    ARUBA_LOG_FAC_DEF(LOG_IAP_MGR), 
    ARUBA_LOG_FAC_DEF(LOG_RADVD),
    ARUBA_LOG_FAC_DEF(LOG_UTILPROC),
    ARUBA_LOG_FAC_DEF(LOG_GSM_MGR),
    ARUBA_LOG_FAC_DEF(LOG_HA_MGR),
    /*
     * ARUBA_LOG_FAC_DEF(LOG_MON_MGR),
     */
#ifndef CONFIG_OPUSONE
    ARUBA_LOG_FAC_DEF(LOG_ACTIVATE),
#endif
    ARUBA_LOG_FAC_DEF(LOG_UCM),
    ARUBA_LOG_FAC_DEF(LOG_IPSTM),
    ARUBA_LOG_FAC_DEF(LOG_ARMD),
    ARUBA_LOG_FAC_DEF(LOG_LLDP),
    ARUBA_LOG_FAC_DEF(LOG_DDS),
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_SHUMWAY) || defined (CONFIG_MILAGRO)
    ARUBA_LOG_FAC_DEF(LOG_CLUSTER_MGR),
    ARUBA_LOG_FAC_DEF(LOG_TUNNELED_NODE_MGR),
#endif
    ARUBA_LOG_FAC_DEF(LOG_SC_REPLICATION_MGR),
    ARUBA_LOG_FAC_DEF(LOG_RSYNC),
    ARUBA_LOG_FAC_DEF(LOG_WEB_CC),
    ARUBA_LOG_FAC_DEF(LOG_DHCPPROXY),
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_OPUSONE) || defined (CONFIG_MILAGRO)
    ARUBA_LOG_FAC_DEF(LOG_AMON_RECVR),
#endif
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_OPUSONE) || defined(CONFIG_NEBBIOLO) || defined (CONFIG_MILAGRO)
    ARUBA_LOG_FAC_DEF(LOG_BOC_MGR),
#endif
    ARUBA_LOG_FAC_DEF(LOG_HCM),
    ARUBA_LOG_FAC_DEF(LOG_CERT_DWNLD),
#ifndef CONFIG_GRENACHE
    ARUBA_LOG_FAC_DEF(LOG_RESOLVE),
#endif
    ARUBA_LOG_FAC_DEF(LOG_BLE_RELAY),
#ifndef CONFIG_OPUSONE 
    ARUBA_LOG_FAC_DEF(LOG_OFA),
    ARUBA_LOG_FAC_DEF(LOG_IP_FLOW_EXPORT),
#endif
#ifdef CONFIG_OPUSONE
    ARUBA_LOG_FAC_DEF(LOG_MCELL),
    ARUBA_LOG_FAC_DEF(LOG_MON_SERV),
    ARUBA_LOG_FAC_DEF(LOG_MON_SERV_FWV),
    ARUBA_LOG_FAC_DEF(LOG_NBAPI_HELPER_PROC),
    ARUBA_LOG_FAC_DEF(LOG_OPENFLOW_CONTROLLER),
    ARUBA_LOG_FAC_DEF(LOG_LOGFWD),
    ARUBA_LOG_FAC_DEF(LOG_CLUSTER_UPGRADE_MGR),
#endif
#if defined (CONFIG_OPUSONE) || defined (CONFIG_SHUMWAY)
    ARUBA_LOG_FAC_DEF(LOG_RNG_MGR),
#endif
    ARUBA_LOG_FAC_DEF(LOG_WEB_DAEMON),
    ARUBA_LOG_FAC_DEF(LOG_CFGDIST),
    ARUBA_LOG_FAC_DEF(LOG_UPGRADEMGR),
    ARUBA_LOG_FAC_DEF(LOG_DPAGENT),
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_OPUSONE) || defined (CONFIG_SHUMWAY) || defined (CONFIG_MILAGRO)
    ARUBA_LOG_FAC_DEF(LOG_APPRF),
#endif
#ifndef CONFIG_OPUSONE    
    ARUBA_LOG_FAC_DEF(LOG_RTPA),
#endif    
    ARUBA_LOG_FAC_DEF(LOG_AMON_SENDER),
    ARUBA_LOG_FAC_DEF(LOG_CERTINIT),
#ifdef __FAT_AP__
    ARUBA_LOG_FAC_DEF(LOG_DPIMGR),
    ARUBA_LOG_FAC_DEF(LOG_LHM),
#endif
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_MILAGRO) || defined (CONFIG_SHUMWAY)
    ARUBA_LOG_FAC_DEF(LOG_CENTRAL),
#endif
    ARUBA_LOG_FAC_DEF(LOG_MSGHH),
    ARUBA_LOG_FAC_DEF(LOG_CTAMON),
#ifdef CONFIG_OPUSONE    
    ARUBA_LOG_FAC_DEF(LOG_SDWAN),
#endif
    ARUBA_LOG_FAC_DEF(LOG_UTILD),
#ifndef CONFIG_OPUSONE    
    ARUBA_LOG_FAC_DEF(LOG_POLICYMGR),
#endif
    ARUBA_LOG_FAC_DEF(LOG_VRRP),
    ARUBA_LOG_FAC_DEF(LOG_LAGM),
    ARUBA_LOG_FAC_DEF(LOG_UV),    
#if defined (CONFIG_PORFIDIO) || defined (CONFIG_MILAGRO)
    ARUBA_LOG_FAC_DEF(LOG_APIMAGEMGR),
#endif
#ifdef CONFIG_OPUSONE
    ARUBA_LOG_FAC_DEF(LOG_IM_HELPER),
#endif
    ARUBA_LOG_FAC_DEF(LOG_WPA3_SAE),

} aruba_log_fac_t;

/* Debug ID's for FPCLI */
#define DEBUG_FPCLI              1
#define DEBUG_FPCLI_EMWEB        2

/* Debug ID's for FPAPPS */
#define DEBUG_FPAPPS_NIM         1
#define DEBUG_FPAPPS_LAG         2
#define DEBUG_FPAPPS_VRRP        3
#define DEBUG_FPAPPS_DOT1D       4
#define DEBUG_FPAPPS_DOT1Q       5

#define DEBUG_PLATFORM_ALL       1
#define DEBUG_USER_ALL           1
#define DEBUG_INTUSER_ALL        1
#define DEBUG_AAA_ALL            1
#define DEBUG_SNMP_ALL           1
#define DEBUG_NANNY_ALL          1
#define DEBUG_PUBLISHER_ALL      1
#define DEBUG_VRRPD_ALL          1

/* Debug ID's for Mobility */
#define DEBUG_DHCP      1
#define DEBUG_DATAPATH  2
#define DEBUG_MESSAGES  3
#define DEBUG_STATES    4
#define DEBUG_TIMERS    5
#define DEBUG_API       6
#define DEBUG_INFO      7
#define DEBUG_FLAG      8
#define DEBUG_USERS     9
#define DEBUG_PACKETS   10
#define DEBUG_CONFIG    11
#define DEBUG_REG       12
#define DEBUG_QUERIES   13
#define DEBUG_CACHE     14

/* Debug ID's for VPDN - L2TP and PPTP */
#define DEBUG_VPDN_PACKETS     1
#define DEBUG_PPP_PACKETS      2

/* Debug ID's for VPDN - L2TP and PPTP */
#define DEBUG_ISAKMP_PACKETS      1
#define DEBUG_IPSEC_PACKETS       2

/* Debug ID's for DHCPD */
#define DEBUG_DHCPD_PKTS     1
#define DEBUG_DHCPD_RELAY    2

/* debug ID's for Auth */
#define DEBUG_AUTH_ACL                1
#define DEBUG_AUTH_AAL                2
#define DEBUG_AUTH_MOB                3
#define DEBUG_AUTH_MSG                4
#define DEBUG_AUTH_CFG                5
#define DEBUG_AUTH_USER               6
#define DEBUG_AUTH_DOT1X_SM           7
#define DEBUG_AUTH_DOT1X_PKTTRACE     8
#define DEBUG_AUTH_DOT1X_SAPM_MSG     9
#define DEBUG_AUTH_DOT1X_EAPOL        10
#define DEBUG_AUTH_DOT1X_RADIUS       11
#define DEBUG_AUTH_DOT1X_SAP_MGT      12
#define DEBUG_AUTH_DOT1X_KEY_MGT      13
#define DEBUG_AUTH_DOT1X_CFG          14
#define DEBUG_AUTH_KRB_PACKETS        15
#define DEBUG_AUTH_SNMP               16
#define DEBUG_AUTH_RADIUS             17
#define DEBUG_AUTH_TACACS             18
#define DEBUG_AUTH_DHCP               19

/* debug id for stm */
#define DEBUG_STM_ALL              1

/* debug id for esi */
#define DEBUG_ESI_ALL              1

/* debug id for pim */
#define DEBUG_PIM_ALL              1

/* debug id for userdb */
#define DEBUG_USERDB_ALL           1

/* debug id for license */
#define DEBUG_LICENSEMGR_ALL       1

/* debug id for wms */
#define DEBUG_WMS_ALL              1

#ifdef ARUBA_ENABLE_CERTMGR
/* debug id for cert */
#define DEBUG_CERT_ALL             1
#endif

/* debug id for SAPM */
#define DEBUG_SAPM_ALL             1

/* debug id for dbsync */
#define DEBUG_DBSYNC_ALL           1

/* debug id for MIP */
#define DEBUG_MIP_HA_PROTOCOL         1
#define DEBUG_MIP_FA_PROTOCOL         2
#define DEBUG_MIP_STATES              3
#define DEBUG_MIP_SECURITY            4
#define DEBUG_MIP_MESSAGES            5
#define DEBUG_MIP_CONFIG              6
#define DEBUG_MIP_PROXY               7
#define DEBUG_MIP_RRV_PROTOCOL        8
#define DEBUG_MIP_PROXY_DHCP          9
#define DEBUG_MIP_DATAPATH            10
#define DEBUG_MIP_MOBILEIP            11
#define DEBUG_MIP_MOBILEIP_PROTOCOL   12

/* debug id for Aruba Filter */
#define DEBUG_ARUBAFILTER_ALL         1

#define DEBUG_SYSMAP_ALL             1
#define DEBUG_STATSMGR_ALL           1
#define DEBUG_FAULTMGR_ALL           1
#define DEBUG_SPOTMGR_ALL 	     1
#define DEBUG_ZMCGI_ALL 	     1
#define DEBUG_ADMINSERVER_ALL           1
#define DEBUG_HAMGR_ALL           1
#define DEBUG_CONFIGMGR_ALL           1

/* debug id for apache */
#define DEBUG_HTTPD_ALL           1

/* debug id for Profile Manager */
#define DEBUG_PROFMGR_ALL            1

#ifdef NCFG_TEST
#define DEBUG_NCFGTEST_ALL              1
#endif

/* debug id for RF Manager */
#define DEBUG_RFM_ALL            1
#define DEBUG_CTS_ALL            1

/* debug id for mdns proxy */
#ifndef CONFIG_GRENACHE
#define DEBUG_MDNS_ALL           1
#endif
#define DEBUG_UCM_ALL              1
extern void makeFacNameList(void);
extern char* convertCase(char* data);
#endif /* SYSLOGMISC */
