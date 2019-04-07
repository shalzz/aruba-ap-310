#ifndef HS2_H
#define HS2_H

#include "anqp.h"

#define QOS_ACTION_CATEGORY        1
#define DLS_ACTION_CATEGORY        2
#define TDLS_ACTION_CATEGORY       12
#define PUBLIC_ACTION_CATEGORY     4

#define GAS_INITIAL_REQUEST        10
#define GAS_INITIAL_RESPONSE       11
#define GAS_COMEBACK_REQUEST       12
#define GAS_COMEBACK_RESPONSE      13

#define WFA_P2P_IE_TYPE            0x09        /* P2P Type - WFA Specs */

#define HS2_IE_TYPE                0x10
#define H2QP_TYPE                  0x11

#define QOS_MAP_CONFIGURE_ACTION   4

#define PATH_TO_ICON_FILE "/tmp"

/* Interworking IE - Access Network Type */
typedef enum _hotspot_acc_nwk_type {
    IEEE80211_ACC_NWK_PRIVATE                   = 0,
    IEEE80211_ACC_NWK_PRIVATE_WITH_GUEST_ACCESS = 1,
    IEEE80211_ACC_NWK_PUBLIC_CHARGEABLE         = 2,
    IEEE80211_ACC_NWK_PUBLIC_FREE               = 3,
    IEEE80211_ACC_NWK_PERSONAL_DEVICE           = 4,
    IEEE80211_ACC_NWK_EMER_SERV_ONLY            = 5,
    /* NwkType 6 - 13 are Reserved */
    IEEE80211_ACC_NWK_TEST                      = 14,
    IEEE80211_ACC_NWK_WILDCARD                  = 15,
} hotspot_acc_nwk_type;

/* Interworking IE - Venue Group */
typedef enum _hotspot_venue_group {
    IEEE80211_VENUE_GROUP_UNSPECIFIED               = 0,
    IEEE80211_VENUE_GROUP_ASSEMBLY                  = 1,
    IEEE80211_VENUE_GROUP_BUSINESS                  = 2,
    IEEE80211_VENUE_GROUP_EDUCATIONAL               = 3,
    IEEE80211_VENUE_GROUP_FACTORY_AND_INDUSTRIAL    = 4,
    IEEE80211_VENUE_GROUP_INSTITUTIONAL             = 5,
    IEEE80211_VENUE_GROUP_MERCANTILE                = 6,
    IEEE80211_VENUE_GROUP_RESIDENTIAL               = 7,
    IEEE80211_VENUE_GROUP_STORAGE                   = 8,
    IEEE80211_VENUE_GROUP_UTILITY_AND_MISC          = 9,
    IEEE80211_VENUE_GROUP_VEHICULAR                 = 10,
    IEEE80211_VENUE_GROUP_OUTDOOR                   = 11,
    IEEE80211_VENUE_GROUP_RESERVED                  = 12,
} hotspot_venue_group;

#define IEEE80211_VENUE_TYPE_MULT_OFFSET            20
/* #define IEEE80211_VENUE_GROUP_OFFSET                20 */
#define IEEE80211_VENUE_TYPE_ASSEMBLY_OFFSET        20
#define IEEE80211_VENUE_TYPE_BUSINESS_OFFSET        40
#define IEEE80211_VENUE_TYPE_EDUCATIONAL_OFFSET     60
#define IEEE80211_VENUE_TYPE_FACTORY_OFFSET         80
#define IEEE80211_VENUE_TYPE_INSTITUTIONAL_OFFSET  100
#define IEEE80211_VENUE_TYPE_MERCANTILE_OFFSET     120
#define IEEE80211_VENUE_TYPE_RESIDENTIAL_OFFSET    140
#define IEEE80211_VENUE_TYPE_STORAGE_OFFSET        160
#define IEEE80211_VENUE_TYPE_UTILITY_OFFSET        180
#define IEEE80211_VENUE_TYPE_VEHICULAR_OFFSET      200
#define IEEE80211_VENUE_TYPE_OUTDOOR_OFFSET        220

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_assembly_type {
    IEEE80211_VENUE_TYPE_ASSEMBLY_UNSPECIFIED = IEEE80211_VENUE_TYPE_ASSEMBLY_OFFSET,
    IEEE80211_VENUE_TYPE_ASSEMBLY_ARENA,
    IEEE80211_VENUE_TYPE_ASSEMBLY_STADIUM,
    IEEE80211_VENUE_TYPE_ASSEMBLY_PASSENGER_TERM,
    IEEE80211_VENUE_TYPE_ASSEMBLY_AMPHITHEATER,
    IEEE80211_VENUE_TYPE_ASSEMBLY_AMUSEMENT_PARK,
    IEEE80211_VENUE_TYPE_ASSEMBLY_WORSHIP_PLACE,
    IEEE80211_VENUE_TYPE_ASSEMBLY_CONVENTION_CTR,
    IEEE80211_VENUE_TYPE_ASSEMBLY_LIBRARY,
    IEEE80211_VENUE_TYPE_ASSEMBLY_MUSEUM,
    IEEE80211_VENUE_TYPE_ASSEMBLY_RESTAURANT,
    IEEE80211_VENUE_TYPE_ASSEMBLY_THEATER,
    IEEE80211_VENUE_TYPE_ASSEMBLY_BAR,
    IEEE80211_VENUE_TYPE_ASSEMBLY_COFFEE_SHOP,
    IEEE80211_VENUE_TYPE_ASSEMBLY_ZOO,
    IEEE80211_VENUE_TYPE_ASSEMBLY_EMER_COORD_CTR,
} hotspot_venue_assembly_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_business_type {
    IEEE80211_VENUE_TYPE_BUSINESS_UNSPECIFIED = IEEE80211_VENUE_TYPE_BUSINESS_OFFSET,
    IEEE80211_VENUE_TYPE_BUSINESS_DOCTOR,
    IEEE80211_VENUE_TYPE_BUSINESS_BANK,
    IEEE80211_VENUE_TYPE_BUSINESS_FIRE_STATION,
    IEEE80211_VENUE_TYPE_BUSINESS_POLICE_STATION,
    IEEE80211_VENUE_TYPE_BUSINESS_POST_OFFICE = 46,
    IEEE80211_VENUE_TYPE_BUSINESS_PROF_OFFICE,
    IEEE80211_VENUE_TYPE_BUSINESS_R_AND_D,
    IEEE80211_VENUE_TYPE_BUSINESS_ATTORNEY,
} hotspot_venue_business_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_educational_type {
    IEEE80211_VENUE_TYPE_EDUCATIONAL_UNSPECIFIED = IEEE80211_VENUE_TYPE_EDUCATIONAL_OFFSET,
    IEEE80211_VENUE_TYPE_EDUCATIONAL_PRIM_SCHOOL,
    IEEE80211_VENUE_TYPE_EDUCATIONAL_SEC_SCHOOL,
    IEEE80211_VENUE_TYPE_EDUCATIONAL_UNIVERSITY,
} hotspot_venue_educational_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_industrial_type {
    IEEE80211_VENUE_TYPE_INDUSTRIAL_UNSPECIFIED = IEEE80211_VENUE_TYPE_FACTORY_OFFSET,
    IEEE80211_VENUE_TYPE_INDUSTRIAL_FACTORY,
} hotspot_venue_industrial_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_institutional_type {
    IEEE80211_VENUE_TYPE_INSTITUTIONAL_UNSPECIFIED = IEEE80211_VENUE_TYPE_INSTITUTIONAL_OFFSET,
    IEEE80211_VENUE_TYPE_INSTITUTIONAL_HOSPITAL,
    IEEE80211_VENUE_TYPE_INSTITUTIONAL_TERM_CARE,
    IEEE80211_VENUE_TYPE_INSTITUTIONAL_DRUG_REHAB,
    IEEE80211_VENUE_TYPE_INSTITUTIONAL_GROUP_HOME,
    IEEE80211_VENUE_TYPE_INSTITUTIONAL_PRISON,
} hotspot_venue_institutional_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_mercantile_type {
    IEEE80211_VENUE_TYPE_MERCANTILE_UNSPECIFIED = IEEE80211_VENUE_TYPE_MERCANTILE_OFFSET,
    IEEE80211_VENUE_TYPE_MERCANTILE_RETAIL,
    IEEE80211_VENUE_TYPE_MERCANTILE_GROCERY,
    IEEE80211_VENUE_TYPE_MERCANTILE_AUTOMOTIVE_SS,
    IEEE80211_VENUE_TYPE_MERCANTILE_SHOPPING_MALL,
    IEEE80211_VENUE_TYPE_MERCANTILE_GAS_STATION,
} hotspot_venue_mercantile_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_residential_type {
    IEEE80211_VENUE_TYPE_RESIDENTIAL_UNSPECIFIED = IEEE80211_VENUE_TYPE_RESIDENTIAL_OFFSET,
    IEEE80211_VENUE_TYPE_RESIDENTIAL_PRIVATE,
    IEEE80211_VENUE_TYPE_RESIDENTIAL_HOTEL,
    IEEE80211_VENUE_TYPE_RESIDENTIAL_DORMITORY,
    IEEE80211_VENUE_TYPE_RESIDENTIAL_BOARDING_HOUSE,
} hotspot_venue_residential_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_utility_type {
    IEEE80211_VENUE_TYPE_UTILITY_UNSPECIFIED = IEEE80211_VENUE_TYPE_UTILITY_OFFSET,
} hotspot_venue_utility_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_vehicular_type {
    IEEE80211_VENUE_TYPE_VEHICULAR_UNSPECIFIED = IEEE80211_VENUE_TYPE_VEHICULAR_OFFSET,
    IEEE80211_VENUE_TYPE_VEHICULAR_AUTOMOBILE,
    IEEE80211_VENUE_TYPE_VEHICULAR_AIRPLANE,
    IEEE80211_VENUE_TYPE_VEHICULAR_BUS,
    IEEE80211_VENUE_TYPE_VEHICULAR_FERRY,
    IEEE80211_VENUE_TYPE_VEHICULAR_SHIP,
    IEEE80211_VENUE_TYPE_VEHICULAR_TRAIN,
    IEEE80211_VENUE_TYPE_VEHICULAR_MOTOR_BIKE,
} hotspot_venue_vehicular_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_outdoor_type {
    IEEE80211_VENUE_TYPE_OUTDOOR_UNSPECIFIED = IEEE80211_VENUE_TYPE_OUTDOOR_OFFSET,
    IEEE80211_VENUE_TYPE_OUTDOOR_MUNI_MESH_NETWORK,
    IEEE80211_VENUE_TYPE_OUTDOOR_CITY_PARK,
    IEEE80211_VENUE_TYPE_OUTDOOR_REST_AREA,
    IEEE80211_VENUE_TYPE_OUTDOOR_TRAFFIC_CONTROL,
    IEEE80211_VENUE_TYPE_OUTDOOR_BUS_STOP,
    IEEE80211_VENUE_TYPE_OUTDOOR_KIOSK,
} hotspot_venue_outdoor_type;

/* Interworking IE - Venue Type */
typedef enum _hotspot_venue_type {
    IEEE80211_VENUE_TYPE_UNSPECIFIED                = 0,
} hotspot_venue_type;

typedef enum _hotspot_advt_prot {
    IEEE80211_ADVT_PROT_ANQP        = 0,
    IEEE80211_ADVT_PROT_MIH_INFO    = 1,
    IEEE80211_ADVT_PROT_MIH_CMD     = 2,
    IEEE80211_ADVT_PROT_EAS         = 3,
    IEEE80211_ADVT_PROT_RSVD        = 4,
} hotspot_advt_prot;

/* Hotspot 2.0 Indication IE - Time Advertisement Capability */
typedef enum _hotspot_time_advt_cap_type {
    IEEE80211_TIME_ADVT_CAP_NO_EXT_TIME_SRC     = 0,
    IEEE80211_TIME_ADVT_CAP_UTC_OFFSET          = 1,
    IEEE80211_TIME_ADVT_CAP_RSVD                = 2,
} hotspot_time_advt_cap_type;

/* Hotspot 2.0 Indication IE - Hotspot Configuration field - Release Number */
typedef enum _hotspot_cfg_release_number {
    IEEE80211_HS_CFG_RELEASE_ONE             = 0,
    IEEE80211_HS_CFG_RELEASE_TWO             = 1,
    IEEE80211_HS_CFG_RELEASE_RSVD            = 2,
} hotspot_cfg_release_number;

#define MAX_ROAM_CONS_OI           5       
#define MAX_TIME_ZONE_LEN          64      
#define MAX_HESSID_LEN                 6 
#define UP_NUM			   8
#define DSCP_EXCEP_MAX             21   
struct dot11u_profile {
    __u8        dot11u_enable;
    /* Modify the IEEE80211_ELEMID_ROAM_CONS_VARIABLE_LEN in ieee80211.h as well */
    __u8        asra;
    __u8        internet;
    __u16       network_type;
    __u8        venue_group;
    __u8        venue_type;
    __u8        query_rsp_len_limit;
    __u8        advt_prot;
    __u8        roam_cons_len_1;
    __u8        roam_cons_oi_1[MAX_ROAM_CONS_OI];
    __u8        roam_cons_len_2;
    __u8        roam_cons_oi_2[MAX_ROAM_CONS_OI];
    __u8        roam_cons_len_3;
    __u8        roam_cons_oi_3[MAX_ROAM_CONS_OI];
    __u8        hessid[MAX_HESSID_LEN];
    __u8        addtl_rc_ois;
    __u8        pame_bi;
    __u8        dgaf_disable;
    __u8        time_zone[MAX_TIME_ZONE_LEN];
    __u8        time_advt_cap;
    __u16       time_error;
    __u8        p2p_dev_mgmt;
    __u8        p2p_cross_connect;
    __u16       anqp_domain_id;

    /* hs2.0 OSE related elements */
    __u64       up_dscp_map[UP_NUM];
    __u16       dscp_excep[DSCP_EXCEP_MAX];
    __u16       qbss_sta_count;
    __u16       qbss_avail_cap;
    __u8        dscp_excep_range[DSCP_EXCEP_MAX*2];
    __u8        up_dscp_range[UP_NUM*2];
    __u8        qbss_load;
    __u8        dscp_excep_num;
    __u8        up_dscp_valid;
    __u8        hs2_osen_enable;
    __u8        rel_num;
    __u8        qos_map_enable;
};

/* aruba-specific 11u interworking ie */
typedef struct _aruba_11u_iwking_ie {
    __u8        eid;                    // 107 
    __u8        len;                    // max. 11
#ifdef IL_BIGENDIAN
    __u8        uesa            : 1,    /* B7 - Unauthentication emergency services reachable */
                esr             : 1,    /* B6 - emergency services reachable */
                asra            : 1,    /* B5 - Additional Steps required for access */
                internet        : 1,    /* B4 - Internet */
                ant             : 4;    /* B0-B3 - access network type */
#else
    __u8        ant             : 4,    /* B0-B3 - access network type */
                internet        : 1,    /* B4 - Internet */
                asra            : 1,    /* B5 - Additional Steps required for access */
                esr             : 1,    /* B6 - emergency services reachable */
                uesa            : 1;    /* B7 - Unauthentication emergency services reachable */
#endif
    __u8        venue_group;            // 1
    __u8        venue_type;             // 1
    __u8        hessid[MAX_HESSID_LEN];     
} __attribute__ ((packed)) aruba_11u_iwking_ie_t;

/* aruba-specific 11p time advertisement ie */
/***** Time Advertisement Definitions *****/
enum {
    ARUBA_TIME_ADVT_CAP_NO_STD_EXT_TS = 0,
    ARUBA_TIME_ADVT_CAP_UTC_OFFSET    = 1,
    ARUBA_TIME_ADVT_CAP_RSVD                  /* 2 - 255 */
};
#define ARUBA_11P_TIME_ADVT_LEN     32
typedef struct _aruba_11p_time_advt_ie {
    __u8        eid;
    __u8        len;
    __u8        timing_cap;
    __u8        time_advt_pyld[0];
} __attribute__ ((packed)) aruba_11p_time_advt_ie_t;

/* 11u Advertisement Tuple definition */
typedef struct _aruba_advt_tuple {
#ifdef IL_BIGENDIAN
    __u8        pame_bi: 1,                 // B7 - PAME-BI
                query_rsp_len_limit: 7;     // B0-B6 - query response length limit
#else
    __u8        query_rsp_len_limit: 7,     // B0-B6 - query response length limit
                pame_bi: 1;                 // B7 - PAME-BI
#endif
	__u8        prot_id;                    // B8 - B15 - advertisement Protocl Id
} __attribute__ ((packed)) aruba_advt_tuple_t;

/* aruba-specific 11u advertisement protocol ie */
typedef struct _aruba_11u_advt_prot_ie {
    __u8                  eid; //108 
    __u8                  len; 
    aruba_advt_tuple_t    at;
} __attribute__ ((packed)) aruba_11u_advt_prot_ie_t;

/* aruba-specific 11u roaming consortium ie */
typedef struct _aruba_11u_roam_cons_ie {
    __u8        eid;                    // 111 
    __u8        len;                    // 4 + variable 
    __u8        num_anqp_oi;            //  
    __u8        oi_len;                 //  11 
    __u8        oi[0];                  // max. 11 
} __attribute__ ((packed)) aruba_11u_roam_cons_ie_t;

/***** Time Zone Definitions *****/
/* Time Zone Element  - 802.11v */
#define ARUBA_MAX_TZ_LEN      64
typedef struct _aruba_11v_time_zone_ie {
    __u8        eid;
    __u8        len;
    __u8        tz[ARUBA_MAX_TZ_LEN];
} __attribute__ ((packed)) aruba_11v_time_zone_ie_t;

/* Vendor HS2 IE */
typedef struct _vndr_hs_2_ie {
    __u8        eid;
    __u8        len;
    __u8        oui[3];
    __u8        type;

#ifdef IL_BIGENDIAN
        
__u8        release_number          : 4,  /* B7-B4 - release number to identify HS2.0 Release Capability */
            reserved                : 1,  /* B3 - reserved for future use */
            anqp_domainid_present   : 1,  /* B2 - ANQP Domain ID Present flag */
            pps_moid_present        : 1,  /* B1 - PPS MO ID Present flag */
            dgaf_disabled           : 1;  /* B0 - DGAF Capability flag */    

#else
__u8        dgaf_disabled           : 1,  /* B0 - DGAF Capability flag */
            pps_moid_present        : 1,  /* B1 - PPS MO ID Present flag */    
            anqp_domainid_present   : 1,  /* B2 - ANQP Domain ID Present flag */
            reserved                : 1,  /* B3 - reserved */
            release_number          : 4;  /* B4-B7 - release number to identify HS2.0 Release Capability*/
#endif
} __attribute__ ((packed)) aruba_vndr_hs_2_ie_t;

/***** P2P Definitions *****/
typedef enum _hotspot_p2p_attr_type {
    P2P_ATTR_STATUS = 0,
    P2P_ATTR_MINOR_REASON_CODE,
    P2P_ATTR_P2P_CAP,
    P2P_ATTR_P2P_DEV_ID,
    P2P_ATTR_GRP_OWNER_INTENT,    
    P2P_ATTR_CFG_TIMEOUT,
    P2P_ATTR_LISTEN_CHANNEL,
    P2P_ATTR_GRP_BSSID,
    P2P_ATTR_EXT_LISTEN_TIMING,
    P2P_ATTR_INT_INTF_ADDR,
    P2P_ATTR_MANAGEABILITY,
    P2P_ATTR_CHANNEL_LIST,
    P2P_ATTRR_NOTICE_OF_ABSENCE,
    P2P_ATTR_DEV_INFO,
    P2P_ATTR_GRP_INFO,
    P2P_ATTR_GRP_ID,
    P2P_ATTR_INTF,
    P2P_ATTR_OP_CHANNEL,
    P2P_ATTR_INV_FLAGS,
    P2P_ATTR_RSVD_1,
    P2P_ATTR_VSA = 221,
    P2P_ATTR_RSVD_2
}hotspot_p2p_attr_type;

/* P2P Attribute - Capability Payload */
typedef struct _vndr_p2p_cap_payload {
    __u8        dev_cap;
    __u8        grp_cap;
} __attribute__ ((packed)) aruba_vndr_p2p_cap_payload_t;

/* P2P Attribute - Device ID Payload */
typedef struct _vndr_p2p_dev_id_payload {
    __u8        addr[6];
} __attribute__ ((packed)) aruba_vndr_p2p_dev_id_payload_t;

/* P2P Attribute - Device Manageability Payload */
typedef struct _vndr_p2p_mgab_payload {
#ifdef IL_BIGENDIAN
    __u8        rsvd:5,                     /* B3-B7 - Reserved */
                coexistence_opt:1,          /* B2 - Coexistence Optional */
                cross_conn_permitted:1,     /* B1 - Cross Connection Permitted */
                dev_mgmt:1;                 /* B0 - P2P Device Mgmt */
#else
    __u8        dev_mgmt:1,                 /* B0 - P2P Device Mgmt */
                cross_conn_permitted:1,     /* B1 - Cross Connection Permitted */
                coexistence_opt:1,          /* B2 - Coexistence Optional */
                rsvd:5;                     /* B3-B7 - Reserved */
#endif
} __attribute__ ((packed)) aruba_vndr_p2p_mgab_payload_t;

typedef struct _vndr_p2p_attr {
    __u8        id;
    __u16       len;
    __u8        value[0];
} __attribute__ ((packed)) aruba_vndr_p2p_attr_t;
    
/* Vendor P2P IE */
typedef struct _vndr_p2p_ie {
    __u8        eid;
    __u8        len;
    __u8        oui[3];
    __u8        type;
    __u8        attr[0];
} __attribute__ ((packed)) aruba_vndr_p2p_ie_t;

/* QOS_MAP IE */
typedef struct _qos_map_ie {
    __u8        eid;
    __u8        len;
    __u8        dscp[(DSCP_EXCEP_MAX + UP_NUM)*2];
} __attribute__ ((packed)) aruba_qos_map_ie_t;

/* QOS_MAP_ELEM */
typedef struct _hs_qos_map_elem {
    __u8   category;
    __u8   action;
    __u8   qos_map_set[0];
} __attribute__ ((packed)) hs_qos_map_elem;
#endif

