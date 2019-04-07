/* ------------------------------------------------------------------------- 
 * Name        : swarm_ioctl_acl.h
 * Description : 
 * ------------------------------------------------------------------------- */

#ifndef SWARM_IOCTL_ACL_H
#define SWARM_IOCTL_ACL_H


#define SWARM_MAX_ACE_PER_ACL_OLD 32

#define SWARM_MAX_ROUTES 32

#define SWARM_MAX_ACE_PER_ACL 1024
#define SWARM_ACL_NAME_LEN    32
#define SWARM_DPI_STR_LEN    31
#define SWARM_DPI_STR_NUM    5
#define SWARM_DPI_URL_LEN    255

#define SWARM_MAGIC_VLAN_ID      3333

#define SWARM_EXTERNAL_CP_PORT   8080
#define SWARM_INTERNAL_CP_PORT   8081
#define SWARM_INTERNAL_HTTPS_CP_PORT   4343
#define SWARM_EXTERNAL_HTTPS_CP_PORT   4343

/* BAD: redefenition - these are actually defined in sos_l3.h */
#define SWARM_IF_NAT_POOL_BASE   65
#define SWARM_CP_NAT_POOL        63
#define SWARM_DRP_NAT_POOL_BASE  2

/* Because __SOS_AP__ can't be seen in this header file, so values related to those
   marcos in that file can't be consistent even include it in this header file.
   This is bug needed to be fixed in future:
        1, define __SOS_AP__
        2, include sos_l3.h
        3, make swarm marcos depend on SOS's marcos.
   Here, we just define these values directly according to those in sos_l3.h.
 */
#define SWARM_RULE_NAT_POOL_BASE 128
#define SWARM_RULE_NAT_POOL_NUM  64
#define SWARM_RULE_NAT_POOL_END  (SWARM_RULE_NAT_POOL_BASE + SWARM_RULE_NAT_POOL_NUM)

#define SWARM_RULE_NAT_IMAGE_SYNC SWARM_RULE_NAT_POOL_END+1
#define SWARM_VLAN_NAT_POOL_BASE SWARM_RULE_NAT_IMAGE_SYNC+1
#define SWARM_VLAN_NAT_POOL_NUM  31
#define SWARM_VLAN_NAT_POOL_END  (SWARM_VLAN_NAT_POOL_BASE + SWARM_VLAN_NAT_POOL_NUM)

#define SWARM_ACL_BASE           100
#define SWARM_ACL_DENY_ALL       (SWARM_ACL_BASE + 0)
#define SWARM_ACL_PERMIT_ALL     (SWARM_ACL_BASE + 1)
#define SWARM_ACL_PERMIT_NAT     (SWARM_ACL_BASE + 2)
#define SWARM_ACL_MAC_AUTH       (SWARM_ACL_BASE + 3)
#define SWARM_ACL_AP             (SWARM_ACL_BASE + 5) /* goes to br0 */
#define SWARM_ACL_MAGIC_VLAN     (SWARM_ACL_BASE + 6) /* goes to bond0 */
#define SWARM_ACL_WIRED_TRUSTED  (SWARM_ACL_BASE + 7) /* for wired trusted port */

#define CLI_MAX_DOWNLINK_AP_PORT    4
#ifdef __EXPERIMENTAL_BUILD__
#define CLI_MAX_SSID_LEN         86
#else
#define CLI_MAX_SSID_LEN         32
#endif
#define CLI_MAX_ACTIVE_SSID      16
#define CLI_MAX_SSID             32
#define CLI_MAX_LOCAL_ACCESS_PROFILES 64
#define CLI_MAX_DLBR__ACCESS_PROFILES 32
/* If you modified CLI_MAX_ACCESS_PROFILES, should take care of ASAP_MAX_ROLE_BWM_CONFIG and TUNNEL_ID_NAT_MAX */
#define CLI_MAX_ACCESS_PROFILES  (CLI_MAX_LOCAL_ACCESS_PROFILES + CLI_MAX_DLBR__ACCESS_PROFILES)
#define CLI_MAX_SERVERS_PER_SSID 3
#define CLI_MAX_SERVERS          (CLI_MAX_SERVERS_PER_SSID * CLI_MAX_ACTIVE_SSID + MAX_MDNS_CPPM_SERVERS + 1) /* +1 for internal */
#define CLI_MAX_LDAP_SERVERS     CLI_MAX_ACTIVE_SSID
#define CLI_MAX_DPI_ERR_URL      8
#define CLI_MAX_DPI_ERR_URL_LEN  256

#define CLI_MAX_SERVER_RULES_PER_SSID 16
#define CLI_MAX_VLAN_RULES_PER_SSID 8

#define CLI_MAX_AAA_MOD_STATIC_VALUE_SIZE 128
#define CLI_MAX_RADIUS_MODIFIERS    (CLI_MAX_SERVERS * 2)
#define CLI_MAX_RADIUS_MODIFIER_ATTR_NUM 64

#define CLI_MAX_RADIUS_ATTRIBUTES 128

#define SWARM_ACL_SSID_PRE_AUTH_BASE      (SWARM_ACL_BASE + 9) /* goes to vapX */

#define SWARM_ACL_POST_AUTH_ROLE_BASE      (SWARM_ACL_SSID_PRE_AUTH_BASE + CLI_MAX_SSID + CLI_MAX_DOWNLINK_AP_PORT + 1)

#define SWARM_ACL_ACE_BASE       1

#define ROLE_FLAG_VOICE      1
#define ROLE_FLAG_EXPANSION_SIZE 2

#define SWARM_ACL_MAX (SWARM_ACL_POST_AUTH_ROLE_BASE + ROLE_FLAG_EXPANSION_SIZE * CLI_MAX_ACCESS_PROFILES)

#define SWARM_ROLE_IDX_INVALID  (0)
#define SWARM_IS_ROLE_IDX_VALID(role_idx) ((role_idx) > 0)
#define SWARM_POST_AUTH_ROLE_IDX_AND_FLAGS_TO_ACL(role_idx, ssid_flag) (SWARM_ACL_POST_AUTH_ROLE_BASE +  ( ( (role_idx)-1) * ROLE_FLAG_EXPANSION_SIZE + (ssid_flag) ) )
#define SWARM_ACL_IS_POST_AUTH(acl) ((acl) >= SWARM_ACL_POST_AUTH_ROLE_BASE)
#define SWARM_POST_AUTH_ACL_TO_ROLE_IDX(acl) ( ((acl) - SWARM_ACL_POST_AUTH_ROLE_BASE) / (ROLE_FLAG_EXPANSION_SIZE))

#define SWARM_SSID_IDX_TO_PRE_AUTH_ACL(ssid_idx) (SWARM_ACL_SSID_PRE_AUTH_BASE + (ssid_idx))
#define SWARM_ACL_IS_PRE_AUTH(acl) ((acl) >= SWARM_ACL_SSID_PRE_AUTH_BASE && (acl) < SWARM_ACL_POST_AUTH_ROLE_BASE)

#define SWARM_IOCTL_ACL_SET        1
#define SWARM_IOCTL_NAT_POOL_SET   2
#define SWARM_IOCTL_CP_USER_SET    3
#define SWARM_IOCTL_CP_USER_GET    4
#define SWARM_IOCTL_CP_USER_GET_CHK_DNAT 5
#define SWARM_IOCTL_NAT_POOL_GET   6
#define SWARM_IOCTL_MCAST_ACL_SET  7
#define SIOCSWARMIOCTL  (SIOCDEVPRIVATE + 2)

/* need padding because extra acl has to be added sometime */
#define SWARM_IOCTL_ACL_PAD        8

/*fix bug 52501*/
#define SIOCSWARMACLIOCTL (SIOCDEVPRIVATE + 3)
#define SIOCSWARMDMOIOCTL (SIOCDEVPRIVATE + 4)
#define SIOCAPPDPIMSG     (SIOCDEVPRIVATE + 8)
#define SIOCDPIMGRIOCTL   (SIOCDEVPRIVATE + 9)
#define SWARM_IOCTL_GETNEXT_SSID_ACL     9
#define SWARM_IOCTL_GETNEXT_ALL_ACL      10
#define SWARM_IOCTL_SET_MGRP             11
#define SWARM_IOCTL_CP_USER_SESSION_GET  12
#define SWARM_IOCTL_HTTPS_USER_SESSION_GET  13

#define SWARM_ACL_POOL_TO_MASTER         0xff

#define SWARM_CAPTIVE_PORTAL_DOAMIN       "/proc/sys/net/fw/captive_portal_domain"
#define SWARM_DNS_FILTERED_DOMAIN         "/proc/sys/net/fw/filtered_domain"

#define SWARM_ECP_NAME_LEN 128
#define SWARM_ECP_SERVER_LEN 128
#define SWARM_ECP_URL_LEN 256
#define SWARM_ECP_REDIRECT_URL_LEN 1024
#define ECP_AUTH_TOKEN_ENVP "EXTERNAL_CP_AUTH_TOKEN"
#define ECP_FACEBOOK_TOKEN "FACEBOOK_AUTH_TOKEN"

typedef struct {
    __u8  valid;
    __u8  mac[6];
    char  essid[CLI_MAX_SSID_LEN + 1];
    __u32 v4_ip;
    __u32 v4_dip;
    __u32 acl;
    char  app[SWARM_DPI_STR_LEN + 1];
    char  rep[SWARM_DPI_STR_LEN + 1];
    char  web[SWARM_DPI_STR_NUM][SWARM_DPI_STR_LEN + 1];
    char  error_url[SWARM_DPI_URL_LEN + 1];
    char  role[SWARM_ACL_NAME_LEN + 1];
} swarm_info_user_t;

typedef struct {
    __u8 valid;
    union {
        struct {
            ip4_t sip;
            ip4_t dip;
        }v4;
        struct {
            ip6_t sip;
            ip6_t dip;
        }v6;
    }u;
    __u16 sport;
    __u16 dport;
}swarm_info_session_t;

typedef enum _captive_portal_type {
    CAPTIVE_PORTAL_DISABLE = 0, CAPTIVE_PORTAL_INTERNAL, CAPTIVE_PORTAL_EXTERNAL,
    CAPTIVE_PORTAL_FACEBOOK
} captive_portal_type;

#ifdef SWARM_IOCTL_IMP
/* private section */

#endif


#ifdef SWARM_IOCTL_INIT
// initialization section


#endif

// This is for bcast-filter specific code

#define SWARM_MAX_MACE_PER_ACL          32
#define SWARM_MCAST_ACL_MAX_PROFILE     4

typedef enum _wlan_bcast_filter_type {
    WLAN_BCAST_FILTER_NONE              = 0,
    WLAN_BCAST_FILTER_ALL               = 1,
    WLAN_BCAST_FILTER_ARP               = 2,
    WLAN_BCAST_FILTER_ARP_ONLY          = 3,
    WLAN_BCAST_FILTER_CONFIGURED_BASE   = 4,
    WLAN_BCAST_FILTER_CONFIGURED_MAX    = (WLAN_BCAST_FILTER_CONFIGURED_BASE 
                                            + SWARM_MCAST_ACL_MAX_PROFILE -1)

} wlan_bcast_filter_type;



enum {
    BCAST_DENY      = 1,
    BCAST_ALLOW     = 2,
    BCAST_UNICAST   = 3,
    BCAST_OPTIMIZE  = 4,       
    BCAST_INVALID   = 5       
};

enum {
    MCAST_ACL_L2_MAC_ENTRY   = 1,
    MCAST_ACL_L2_ETYPE_ENTRY = 2,
    MCAST_ACL_L3_IPV4_ENTRY  = 3,
    MCAST_ACL_L3_IPV6_ENTRY  = 4,
    MCAST_ACL_L4_PORT_ENTRY  = 5,
    MCAST_ACL_CUSTOM_ENTRY   = 6,
    MCAST_ACL_ANY_ENTRY      = 7
};

enum {
    MCAST_CUSTOM_PROTO_NONE       = 0,
    MCAST_CUSTOM_PROTO_DHCP       = 1,
    MCAST_CUSTOM_PROTO_ARP        = 2,
    MCAST_CUSTOM_PROTO_RTR_ADV    = 3,
    MCAST_CUSTOM_PROTO_RTR_SOL    = 4,
    MCAST_CUSTOM_PROTO_NBR_ADV    = 5,
    MCAST_CUSTOM_PROTO_NBR_SOL    = 6,
    MCAST_CUSTOM_PROTO_V4_STREAM  = 7,
    MCAST_CUSTOM_PROTO_V6_STREAM  = 8,
    MCAST_CUSTOM_PROTO_MDNS       = 9,
    MCAST_CUSTOM_PROTO_SSDP       = 10,
    MCAST_CUSTOM_PROTO_NETBIOS    = 11,

    MCAST_CUSTOM_PROTO_MAX
};

typedef struct _swarm_mcast_ace_t {
    union {
        __u8 custom;
        struct {
            __u8 mac[6];
            __u8 len;
        } l2;
        struct {
            ip_t ip;
            ip_t mask;
        } v4;
        struct {
            ip6_t ip;
            __u8  plen;
        } v6;
        __u16 port;
        __u16 etype;
    } u;
    __u8 type;
    __u8 action;
} swarm_mcast_ace_t;

typedef struct _swarm_acl_t {
    __u8         cmd;
    __u8         role_based:1,
                 permit_arp:1,
                 permit_l2:1,
                 permit_all:1,
                 captive_portal:1,
                 bits_unused:3;
    __u16         acl;
    __u16        num;
    char         rule_idx;
#ifndef __FAT_AP__
    char         name[SWARM_ACL_NAME_LEN + 1];
#else
    char         name[SWARM_ACL_NAME_LEN + 3];
#endif
    acl_entry_t  ace[SWARM_MAX_ACE_PER_ACL];
    swarm_info_user_t user;
    swarm_info_session_t session;
    swarm_mcast_ace_t mace[SWARM_MAX_MACE_PER_ACL];
} swarm_acl_t;

#endif
