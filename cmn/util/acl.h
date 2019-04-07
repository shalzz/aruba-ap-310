#ifndef _CMN_ACL_H_
#define _CMN_ACL_H_

/*
 * Access Control Lists: This module defines the ACL data structure
 * and access routines.  This module supports most ACLs, including:
 * Standard, Extended, MAC Address, Etype, and Reflexive.  Each ACL
 * can contain an unlimited number of entries (ACEs).  Entries are
 * processed in the same order as configured.  The first entry that
 * matches the frame is applied, ending the search.
 *
 * Cisco uses the following convention for numbering:
 *
 *   Standard ACLs:    1-99, 1300-1999
 *   Extended ACLs:    100-199, 2000-2699
 *   Ether Type ACLs:  200-299
 *   DECNet ACLs:      300-399  (not supported) 
 *   XNS ACLs:         400-599  (not supported)
 *   AppleTalk ACLs:   600-699  (not supported)
 *   MAC Address ACLs: 700-799, 1100-1199
 *   IPX ACLs:         800-1099, 1200-1299 (not supported)
 * 
 * and the following syntax for configuration:
 *
 * standard access lists (only supports source IP addresses):
 *   access-list 1-99 {permit|deny} source mask
 *
 * extended access lists (supports dest, ports, protocols, etc.):
 *   access-list 100-199 [dynamic name [timeout minutes]] {permit|deny}
 *     protocol source mask dest mask [precedence 0-7] [tos 0-15] [log]
 *
 * access-list 100-199 [dynamic name [timeout minutes]] {permit|deny}
 *   tcp source mask [operator port [port]]  (operator: eq, lt, gt, neq, range)
 *       dest   mask [operator port [port]]
 *       [established] [precedence 0-7] [tos 0-15] [log]
 *
 * access-list 100-199 [dynamic name [timeout minutes]] {permit|deny}
 *   udp source mask [operator port [port]]
 *       dest   mask [operator port [port]]
 *       [precedence 0-7] [tos 0-15] [log]
 *
 * access-list 100-199 [dynamic name [timeout minutes]] {permit|deny}
 *   icmp source mask dest mask [type [code]|icmp-message]
 *       [precedence 0-7] [tos 0-15] [log]
 *
 * access-list 100-199 [dynamic name [timeout minutes]] {permit|deny}
 *   igmp source mask dest mask [type]
 *       [precedence 0-7] [tos 0-15] [log]
 *
 * access-list 100-199 [dynamic name [timeout minutes]] {permit|deny}
 *   ip source mask dest mask
 *       [precedence 0-7] [tos 0-15] [log]
 *  
 * named access lists:
 *   ip access-list standard name
 *     permit 1.2.3.4
 *     deny any
 *
 *   ip access-list extended name
 *     permit tcp any any eq telnet
 *
 * ether type access lists:
 *   access-list 200-299 {permit|deny} ether-type mask
 *
 * MAC address access lists:
 *   access-list 700-799 {permit|deny} mac-sa mask
 */

/* we partition ACLs for ones controlled by auth and ones by CPFW (in PIM) */
#define AUTH_ACLS         2700
#define AUTH_ACL_START    0                 /* AUTH assumes this is 0 */
#define AUTH_ACL_END      (AUTH_ACLS-AUTH_ACL_START-1)
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE) || defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
#define CP_ACLS           4                 /* starts after AUTH_ACLS */
#else
#define CP_ACLS           5                 /* starts after AUTH_ACLS */
#endif
#define CP_ACL_START      (AUTH_ACL_END+1)
#define CP_ACL_END        (CP_ACL_START+CP_ACLS-1)

#define ACLS              (AUTH_ACLS+CP_ACLS)  /* see above */
#if defined (CONFIG_OPUSONE) || defined (CONFIG_GRAPPA) || defined (CONFIG_ACE_16K)
#define ACL_SHIFT         14
#else
#define ACL_SHIFT         13
#endif
#define ACL_ENTRIES       (1 << ACL_SHIFT)  /* total entries */
#define ACL_INVALID_INDEX ( 2*ACL_ENTRIES + 1 ) /* used by asap */
#define AUTH_ACE_START    0                 /* AUTH assumes this is 0 */
#define AUTH_ACE_END      (ACL_ENTRIES - 512 - 1)
#define AUTH_ACE_ENTRIES  (AUTH_ACE_END-AUTH_ACE_START+1)
/* 512 ACE entries go to CP firewall, we swap 256 in/out for whitelisting feature */
#define CP_ACE_START      (AUTH_ACE_END+1)  /* AUTH assumes this is 0 */
#if defined(CONFIG_NEBBIOLO) || defined(CONFIG_GRENACHE) || defined(CONFIG_PORFIDIO) || defined(X86_TARGET_PLATFORM)  || defined(CONFIG_MILAGRO)
#define CP_ACE_END        (CP_ACE_START + 512)
#else
#define CP_ACE_END        (CP_ACE_START + 128)
#endif

#define SIZE_ACL_NAME     64

#define SIZE_HOST_NAME 64
#define MAX_NETDEST_ENTRY 256
#define MAX_NETDEST_ENTRY_PER_UPD 32
#define MAX_NETDESTINATION_ENTRIES 1024

/*
 * Priorities for port ACE for use with SOS.
 */
#define ACL_GRE_HB_SOS_PRIORITY 3
#define ACL_PAPI_HB_SOS_PRIORITY 7
#define ACL_PAPI_CONF_SOS_PRIORITY 7
#define ACL_SIP_SOS_PRIORITY 8
#define ACL_IKE_SOS_PRIORITY 8 
#define ACL_PPTP_SOS_PRIORITY 8 
#define ACL_L2TP_SOS_PRIORITY 8 
#define ACL_DHCP_REPLY_SOS_PRIORITY 9
#define ACL_DNS_PROXY_SOS_PRIORITY 9
#define ACL_HTTP_REDIRECT_SOS_PRIORITY 9 
#define ACL_NATT_SOS_PRIORITY 9 
#define ACL_NTP_SOS_PRIORITY 10
#define ACL_SNMP_SOS_PRIORITY 10
#define ACL_TFTP_SOS_PRIORITY 11
#define ACL_FTP_SOS_PRIORITY 11
#define ACL_SSH_SOS_PRIORITY 11 
#define ACL_HTTP_WEBUI_SOS_PRIORITY 11
#define ACL_HTTPS_WEBUI_SOS_PRIORITY 11
#define ACL_TELNET_SOS_PRIORITY 11
#define ACL_SYSLOG_SOS_PRIORITY 12

/*
 * Switched the order of two ACL below so that two
 * ACL can be allocated per role (including logon)
 */
#define ACL_LOGON_ID      2                /* ACL 1 is the global sacl */
#define ACL_VALID_USER_ID ACL_LOGON_ID + 1

#ifdef __FAT_AP__ 
#define ACL_SDN_ID        104 
#else 
#define ACL_SDN_ID        (ACL_VALID_USER_ID+1) 
#endif 


#define ACL_FLAG_PERMIT        0x00000001
#define ACL_FLAG_LOG           0x00000002
#define ACL_FLAG_EST           0x00000004
#define ACL_FLAG_MAC           0x00000008
#define ACL_FLAG_SNAT          0x00000010
#define ACL_FLAG_DNAT          0x00000020
#define ACL_FLAG_FORREDIR      0x00000040
#define ACL_FLAG_REVREDIR      0x00000080
#define ACL_FLAG_ETYPE         0x00000100
#define ACL_FLAG_INVERTSA      0x00000200
#define ACL_FLAG_INVERTDA      0x00000400
#define ACL_FLAG_HIGHPRIO      0x00000800
#define ACL_FLAG_TOS           0x00001000
#define ACL_FLAG_PRIORITY      0x00002000
#define ACL_FLAG_RANGESA       0x00004000
#define ACL_FLAG_RANGEDA       0x00008000
#define ACL_FLAG_DENYRESP      ACL_FLAG_TOS
#define ACL_FLAG_REDIR         (ACL_FLAG_FORREDIR|ACL_FLAG_REVREDIR)
#define ACL_FLAG_NAT           (ACL_FLAG_DNAT|ACL_FLAG_SNAT)
#define ACL_FLAG_MIRROR        ACL_FLAG_EST
#define ACL_FLAG_DEN_NOTDNAT   0x00010000 /* deny if not through DNAT     */
#define ACL_FLAG_DEN_NOTMASTER 0x00020000 /* deny if not master/local VPN */
#define ACL_FLAG_DISSCAN       0x00040000 /* do not ARM scan */

/*
 * RKOMMULA: the reason for having both IPv4 and IPv6 flags is, we can
 * share an ACL between these two protocols.
 */
#define ACL_FLAG_IPv4          0x00080000 /* Entry is used for IPv4 */
#define ACL_FLAG_IPv6          0x00100000 /* Entry is used for IPv6 */

#define ACL_FLAG_PKTTRACE       0x00200000 /* For enabling datapath pkt trace */
#define ACL_FLAG_BWM_CONTRACT   0x00400000 /* For applying bw contracts */
#define ACL_FLAG_NAMESA         0x00800000 /* domain name */
#define ACL_FLAG_NAMEDA         0x01000000 /* domain name */
#define ACL_FLAG_LOCALIP        0x02000000 /* For indicating local-ips on a device */
#define ACL_FLAG_DEN_NOTSECURE  0x04000000 /* deny if didn't come in secured */
#define ACL_FLAG_DPI            0x08000000 /* App-based ACE for DPI PEF */
#define ACL_FLAG_CLASSIFY_MEDIA 0x10000000 // To Identify it's a media flow voip/video.
#define ACL_FLAG_WEBCC          0x20000000 /* Web CC ctgy/score based ACE*/
#define ACL_FLAG_ROUTE_NEXTHOP  0x80000000 /* For next hop ACLs */
#define ACL_FLAG_BLACKLIST      0x40000000 /*To Identify blacklist */
#define ACL_FLAG_QINQ           0x80000000 /* QinQ ACL */
#define ACL_FLAG_OPENFLOW_ACT   0x0000000100000000ULL /* To Identify it's an openflow */
#define ACL_FLAG_SRC_ROLE       0x0000000200000000ULL /* To Identify src user role */
#define ACL_FLAG_DST_ROLE       0x0000000400000000ULL /* To Identify dest user role */
#define ACL_FLAG_SRC_ALIAS      0x0000000800000000ULL /* To Identify src alias used in ACL policy */
#define ACL_FLAG_DST_ALIAS      0x0000001000000000ULL /* To Identify dst alias used in ACL policy */
#define ACL_FLAG_DEN_NOTIAP     0x0000002000000000ULL /* Deny if not from IAP Tunnel */

#define ACL_FLAG_GEO            0x40000000 //GEO ACE 
#define ACL_FLAG_OPENFLOW_ACT   0x0000000100000000ULL // To Identify it's an openflow 

#ifdef __FAT_AP__
#define ACL_EX_FLAG_AP_IP        0x00000001
#define ACL_EX_FLAG_AP_NETWORK   0x00000002
#define ACL_EX_FLAG_MASTER_IP    0x00000004
#define ACL_EX_FLAG_TUNNEL_ONLY  0x00000008
#define ACL_EX_FLAG_SSL_THROTTLE 0x00000010
#define ACL_EX_FLAG_TIME_BASED   0x00000020
#define ACL_EX_FLAG_DPI_ONLY     0x00000040
#define ACL_EX_FLAG_CACHEDA      0x00000080 // for fat-ap, https error page
#endif

#define ACL_INDEX(Table, Entry) (((Table) << ACL_SHIFT) | (Entry))

#define ACL_KEYWORD_ANY			0x00000000		/* 'any' */
#define ACL_KEYWORD_USER		0xFFFFFFFF		/* 'user' */
#define ACL_KEYWORD_LOCALIP            0x00000001              /* 'localip' */

#ifdef __FAT_AP__
#define ACL_KEYWORD_APIP         0x00000001
#define ACL_KEYWORD_APNETWORK    0x00000002
#define ACL_KEYWORD_MASTERIP     0x00000003
#endif

/* acl_flags array definition in SOS is 16-bit entity */
#define ACL_GLOBAL_FLAG_DPI_DISABLE        0x00000001
#define ACL_GLOBAL_FLAG_DPI_METADATA       0x00000002
#define ACL_GLOBAL_FLAG_DPI_ANONYMIZE      0x00000004
#define ACL_GLOBAL_FLAG_WEBCC_DISABLE      0x00000008
#define ACL_GLOBAL_FLAG_DPI_BWM_ENF        0x00000010
#define ACL_GLOBAL_FLAG_WEBCC_BWM_ENF      0x00000020
#define ACL_GLOBAL_FLAG_YOUTUBE_EDU        0x00000040
#define ACL_GLOBAL_FLAG_AV_DISABLE         0x00000080
#define ACL_GLOBAL_FLAG_IPCLASSIFY_DISABLE 0x00000100
#define ACL_GLOBAL_FLAG_OPENFLOW_ENABLE    0x00000200


#define ACL_QINQ_ACTION_NONE       0
#define ACL_QINQ_ACTION_POP        1
#define ACL_QINQ_ACTION_SWAP       2

typedef struct acl_entry_s {
    __u64  flags;    /* ACL_FLAG_* */
    sos_dst_t dest_idx;     /* redirect destination, eg, tunnel */
    __u16  vlan;     /* redirect VLAN */
    __u16  fsport;   /* first/lower TCP/UDP source port */
    __u16  lsport;   /* last/upper TCP/UDP source port */
    __u16  fdport;   /* first/lower TCP/UDP destation port */
    __u16  ldport;   /* last/upper TCP/UDP destination port */
    __u8   proto;    /* IP protocol */
    __u8   pool;     /* NAT pool */
    __u16  ap_group; /* AP group ID */
    __u8   tos;      /* type of service */
    __u8   tos_match; /* type of service for matching */
    __u8   priority; /* 802.1p priority */
    union {
        struct {
            ip4_t   sip;  /* source IP address, Etype/Emask, or MAC address */
            ip4_t   smask;/* source IP mask or MAC address cont. */
            ip4_t   dip;  /* destination IP address or MAC mask */
            ip4_t   dmask;/* destination IP mask or MAC mask cont. */
        } v4;
        struct {
            ip6_t   sip;  /* source IP address, Etype/Emask, or MAC address */
            ip6_t   smask;/* source IP mask or MAC address cont. */
            ip6_t   dip;  /* destination IP address or MAC mask */
            ip6_t   dmask;/* destination IP mask or MAC mask cont. */
        } v6;
        struct {
            __u8    Id;
            __u8    type;
            __u8    dir;
            __u64   countrybitmap[4];
        } geo;
    } u;
    __u16  up_contract; /* Upstream bandwidth contract (0 means none) */
    __u16  down_contract; /* Downstream bandwidth contract (0 means none) */
    __u16  vernum;
    __u8   ehtype; /* IPv6 Extended Header type */
    __u8   sos_internal_prio; /*Internal priority used by SOS to classify */
    __u8   fWebCCCtgID;  /* first WebCC Category ID */
    __u8   lWebCCCtgID;  /* last WebCC Category ID */
    __u8   fWebCCRepID;  /* lower WebCC Reputation Index */
    __u8   lWebCCRepID;  /* upper WebCC Reputation Index */
    __u16  appID;
    __u16  appGroupID;
#ifdef __FAT_AP__
    __u32 ex_flags;
#endif
    __u32  action_index;
} acl_entry_t;       /* Access Control Entry (88B) */

/* Note to make the struct 64-bit aligned */
typedef struct {
    __u16 index;   /* ACL_INDEX() */
    __u16 pad;
    __u16 hits[2]; /* number of matches */
     char aclname[SIZE_ACL_NAME];
    __u8  acltype;  
    __u8  unused[7];  
    acl_entry_t entry;
} acl_entry_msg_t;

#define DESTINATION_TYPE_HOST	0
#define DESTINATION_TYPE_SUBNET	1
#define DESTINATION_TYPE_RANGE	2
#define DESTINATION_TYPE_NAME	3
#define DESTINATION_TYPE_L_OVERRIDE	4 // Vlan-based Local Override
#define DESTINATION_TYPE_L_OVERRIDE_FAILED	5 // Vlan-based Local Override has failed,
                                              // SOS should ignore this netdestination item

typedef struct {
   	__u8 type;		/* DESTINATION_TYPE_* */
    union {
        struct {
	        ip4_t 	addr;
	        ip4_t 	mask;	/* also last address of range */
        } v4;
        struct {
	        ip6_t 	addr;
	        ip6_t 	mask;	/* also last address of range */
        } v6;
		//char hostname[SIZE_HOST_NAME];
    } u;
    int vrrpid; 
} netdest_entry_t;

typedef struct {
    __u16 netdest_id;   /* netdest id() */
    __u16  count;
    __u16 start_idx;
    __u8 v6_dest;
    netdest_entry_t entry[MAX_NETDEST_ENTRY_PER_UPD];
    __u8 more;
    __u16 curr_count;
} acl_netdest_msg_t;

#define MAX_HITS_PER_MSG 256

typedef struct {
    __u16 hit_idx;
    __u16 count;
    __u32 hits[MAX_HITS_PER_MSG];
} acl_hits_msg_t;

// message structure for auth->sos, and stm netdest update
typedef struct {
    __u16 action;            // add or del
    __u16 index;            // index on the netdest entry table
    __u16 num_entries;
    __u16 netdest_id;
    __u8 v6_dest;
    netdest_entry_t entries[MAX_NETDEST_ENTRY_PER_UPD]; 
    __u8  curr_count;       // number of entries in this msg
    __u8  more;             // more field  
    __u16 index_offset;     // index offset
} netdest_msg_t;

// message structure for sapm->sapd updates
typedef struct {
    __u16 action;            // add or del
    __u16 index;            // index on the netdest entry table
    __u16 num_entries;
    __u16 netdest_id;
    __u8 v6_dest;
    netdest_entry_t entries[MAX_NETDEST_ENTRY]; 
} ap_netdest_msg_t;

typedef struct {
    __u16 action; /* Add or del */
    __u16 acl;
    __u16 index;   /* ACL_INDEX() */
    __u16 entries;
    __u16 flags;
} acl_msg_t;

typedef struct {
    __u16 action;
    __u16 acl;
    __u16 flags;
    __u8  cookie_str[64];
} acl_youtubeedu_msg_t;

typedef struct {
    __u16 index;   /* ACL_INDEX() */
    __u16 hits[2]; /* number of matches */
} acl_stats_msg_t;

typedef struct {
    __u16 acl;
    __u16 index;
    __u32 tmask[2];    
    __u16 enable;
    __u16 unused;
} acl_pkttrace_msg_t;

/* messaging between APPS and "pim" for controlling CP firewall settings */
typedef struct cpfw_msg_s {
    __u16 type;
    __u16 enable;   /* enable/disable    */
    /* for CPFW_TYPE_FW */
    __u16 proto;    /* proto, TCP or UDP */
    __u16 port;     /* port number       */
    /* these are for CPFW_TYPE_DNAT */
    __u32 addr;
    __u32 dnataddr;
    __u8 is_delete; /*Delete the NAT*/
} cpfw_msg_t;

#define BULK_MSG_MAX_ACLS 50
#define BULK_MSG_MAX_ACES 200
#define MAX_BULK_MSG_SIZE (PAPI_MAX_PACKET_SIZE - 196)
#define MAX_SOS_BULK_MSG_SIZE 1500
#define INIT_BULK_MSG_SIZE (4 + BULK_MSG_MAX_ACLS*sizeof(acl_msg_t))

typedef struct {
    __u16 acls;
    __u16 aces;
    acl_msg_t aclMsg[BULK_MSG_MAX_ACLS];
    acl_entry_msg_t aceMsg[BULK_MSG_MAX_ACES];
} bulk_acl_msg_t;

typedef struct {
    __u32 ehmap[8];
} exthdr_msg_t;


typedef struct {
    __u16 id;			/* index to IP list table */
	int usecount;	/* count of names sharing id */
	char name[64]; /* SOS_DNS_SIZE_NAME */
} hostname_t; 

#define CPFW_TYPE_REPROGRAM 1
#define CPFW_TYPE_FW        2
#define CPFW_TYPE_DNAT      3
#define CPFW_TYPE_USERIDLE  4
#define CPFW_TYPE_REMOVE    5

#define	AP_WIRED_MAGIC	100
#define	AP_WIRED_BRIDGE_MAGIC	101

/* pubsub message uses ACL_REM as 1 and ACL_ADD as 2
 * for publishing ACL name which is published to 
 * prof manager and STM
 * hence defining the value
 */
#define ACL_ACTION_DEL            1
#define ACL_ACTION_ADD            2
#define ACL_ACTION_UPDATE_VER     3
#define ACL_ACTION_UPDATE_FLAGS   4
#define ACL_ACTION_YOUTUBE_EDU    5
#define ACL_ACTION_GEOLOCATION    6

#define ACL_ACTION_CFG_RACL       7
#define ACL_ACTION_SYS_RACL       8

#define ACL_ACTION_NETDEST_ADD    9
#define ACL_ACTION_NETDEST_DEL    10

#define IPPROTO_V6ANY 255 /* Used for proto 'any' in v6 ACLs */

#define MASTER_BOC_TRAFFIC_ACL_NAME   "master-boc-traffic"
#endif /* _CMN_ACL_H_ */
