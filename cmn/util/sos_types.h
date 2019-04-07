#ifndef _SOS_TYPES_H_
#define _SOS_TYPES_H_

#include "mxc.h"

/*
 * Destination Definition (32 bits)
 * Unknown (slowpath):                0
 * Multicast groups:                  1 - 0x3006
 *                    Vlan:           1 - 4095 (one per VLAN)
 *                Mobility:           4096 - 8197
 *                L3-Mcast:           8198 - (8198 + 4096)
 * Unicast (physical port) dests:     0x3006 - x-1 (x = blades * ports/blade)
 * Drop                               0x4000
 * Tunnel-Groups:                     0x4001 - 0x4400
 * PBR NexthopLists:                  0x4401 - 0x4420
 * PBR Nexthops:                      0x4421 - 0x4620
 * IPSEC map ids:                     0x4621 - 0x7ef0
 * Named-Tunnels (special tunnels):   0x7ff0 - 0x7fff
 * Opcode (redirection opcodes):      0x8000 - 0xffff
 * Tunnels (including trunk groups):  0x10000 - (0x10000 + platfrom tunnel limmit)
 */

#define SOS_DEST_VLANS        (4*1024) /* 802.1Q maximum */
/* #if defined (SOS_MOBILE_IP) */
#define SOS_DEST_MOB_HAD      1   /* special multicast group */
#define SOS_DEST_MMG          255 /*((4*1024)-1)*/ /* Mobility Multicast Groups, Fixme:Size*/
/* #else */
/* #define SOS_DEST_MOB_HAD      0 */
/* #define SOS_DEST_MMG          0 */
/* #endif */

#define SOS_DEST_MOBILITY     (SOS_DEST_MOB_HAD+SOS_DEST_MMG)

#define SOS_DEST_L3_MCAST     (4*1024) /* Maximum no of L3 groups */

#if defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO) || defined(CONFIG_X86_TARGET_PLATFORM)
#define NUM_L3_MCAST_GROUPS   (4*1024)
#else
#define NUM_L3_MCAST_GROUPS   (256)
#endif

#ifdef CONFIG_MSWITCH
#define SOS_DEST_SLOTS         3  /* maximum GMACs */
#define SOS_DEST_CPUS          2
#define SOS_DEST_TUNNELS      (8*1024)
#elif defined(CONFIG_NEBBIOLO)
#define SOS_DEST_SLOTS         4  /* maximum GMACs */
#define SOS_DEST_CPUS          2
#define SOS_DEST_TUNNELS      (32*1024)
#elif defined(CONFIG_GRENACHE)
#define SOS_DEST_SLOTS         6  /* maximum GMACs */
#define SOS_DEST_CPUS          2
#define SOS_DEST_TUNNELS      (8*1024)
#elif defined(CONFIG_CORVINA)
#define SOS_DEST_SLOTS         8  /* maximum slots */
#define SOS_DEST_CPUS          2
#define SOS_DEST_TUNNELS      (8*1024)
#elif defined(X86_TARGET_PLATFORM)
#define SOS_DEST_SLOTS         1  /* maximum slots */
#define SOS_DEST_CPUS          2
#define SOS_DEST_TUNNELS      (sosdplimit.tunnel_table_entry) /*pow of 2*/
#define SOS_DEST_TUNNELS_DBG      (sosdplimit.tunnel_table_entry) /*Only for debuggin purpose*/
#elif defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
#define SOS_DEST_SLOTS         2  /* maximum slots */
#define SOS_DEST_CPUS          2
#if defined(VARIANT_porfidio_7030)
#define SOS_DEST_TUNNELS      (10240) //Only for debugging purpose
#define SOS_DEST_TUNNELS_DBG  (10240) //Only for debugging purpose
#elif defined(VARIANT_porfidio_7010)
#define SOS_DEST_TUNNELS      (5120) //Only for debugging purpose
#define SOS_DEST_TUNNELS_DBG  (5120) //Only for debugging purpose
#elif defined(VARIANT_porfidio_7024)
#define SOS_DEST_TUNNELS      (5120) //Only for debugging purpose
#define SOS_DEST_TUNNELS_DBG  (5120) //Only for debugging purpose
#elif defined(VARIANT_porfidio_7210) // start variant
#define SOS_DEST_TUNNELS      (32*1024) // (24*1024) PER PRD, bump up for Power of 2
#define SOS_DEST_TUNNELS_DBG      (24*1024) //Only for debugging purpose
#elif defined(VARIANT_porfidio_7240)
#define SOS_DEST_TUNNELS      (128*1024)// (96*1024) PER PRD, bump up for Power of 2
#define SOS_DEST_TUNNELS_DBG      (96*1024) //Only for debugging purpose
#elif defined(VARIANT_porfidio_7240xm)
#define SOS_DEST_TUNNELS      (128*1024)// (96*1024) PER PRD, bump up for Power of 2
#define SOS_DEST_TUNNELS_DBG      (96*1024) //Only for debugging purpose
#elif defined(VARIANT_porfidio_7220)
#define SOS_DEST_TUNNELS      (64*1024) // (48*1024) PER PRD, bump up for Power of 2
#define SOS_DEST_TUNNELS_DBG      (48*1024) //Only for debugging purpose
#elif defined(CONFIG_MILAGRO)
#define SOS_DEST_TUNNELS      (128*1024)// (96*1024) PER PRD, bump up for Power of 2
#define SOS_DEST_TUNNELS_DBG      (96*1024) //Only for debugging purpose
#else
#define SOS_DEST_TUNNELS      (64*1024) // (48*1024) PER PRD, bump up for Power of 2
#define SOS_DEST_TUNNELS_DBG      (48*1024) //Only for debugging purpose
#endif                             // end variant
#elif defined(CONFIG_ARUBA_NET)	
#ifdef CONFIG_KILCHOMAN
#define SOS_DEST_SLOTS         3  /* three since 96 devs needed */
#elif defined(__FAT_AP__)
#define SOS_DEST_SLOTS         3  /* three since 96 devs needed */
#else
#define SOS_DEST_SLOTS         2  /* two since 64 devs needed */
#endif
#define SOS_DEST_CPUS          1
#define SOS_DEST_TUNNELS       0  /* tunnels are "ports" on AP */
#else /* CONFIG_MSWITCH */
#define SOS_DEST_SLOTS         2  /* maximum GMACs */
#define SOS_DEST_CPUS          1
#define SOS_DEST_TUNNELS      (8*1024)
#endif /* CONFIG_MSWITCH */

#if defined(CONFIG_CORVINA)
#define SOS_DEST_SLOT_PORTS    64 /* max ports per line card */
#define SOS_DEST_DEVICES       2  /* max devices per line card */
#define SOS_DEST_DEVICE_PORTS  32 /* max port per device */
#define SOS_DEST_MODULES       2  /* max modules per line card */
#define SOS_DEST_MODULE_PORTS  48 /* max ports per module */
#else /* CONFIG_CORVINA */

#define SOS_DEST_SLOT_PORTS    32 /* max ports per line card */
#define SOS_DEST_MCASTS        (SOS_DEST_VLANS+SOS_DEST_MOBILITY+\
                               SOS_DEST_L3_MCAST)
#define SOS_DEST_DEVICES       1  /* max devices per line card */
#define SOS_DEST_DEVICE_PORTS  32 /* max ports per device */
#define SOS_DEST_MODULES       1  /* max modules per line card */
#define SOS_DEST_MODULE_PORTS  SOS_DEST_SLOT_PORTS 
#endif

#define SOS_DEST_PORT_MASK     (SOS_DEST_SLOT_PORTS - 1) 

#ifdef CONFIG_KILCHOMAN
#define SOS_DEST_UCASTS       96
#elif defined(CONFIG_ASAP_MOD) && defined(CONFIG_GRENACHE)
#define SOS_DEST_UCASTS       64
#else
#define SOS_DEST_UCASTS       (SOS_DEST_SLOTS*SOS_DEST_SLOT_PORTS)
#endif

/* PORF_SCALE_REVISIT: Enable 32-bit support for all Aruba platforms */
#if !defined(AOS_32BIT_TUNNEL_INDEX_SUPPORT)
#define AOS_32BIT_TUNNEL_INDEX_SUPPORT
#endif

/* This destination can be used for packets that need to be dropped.
 * They will be dropped in fp_frame() forward */
#define SOS_DEST_DROP               0x4000

#define SOS_DEST_TUNNEL_GROUPS      1024
#define SOS_DEST_TUNNEL_GROUP_START 0x4001
#define SOS_DEST_TUNNEL_GROUP_END   (SOS_DEST_TUNNEL_GROUP_START+\
                                    SOS_DEST_TUNNEL_GROUPS-1)

/*
 * Nexthop List dests for PBR
 * Indexing starts from SOS_DEST_PBR_NHL_START + 1 to
 * keep it non-zero 
 */
#define SOS_DEST_PBR_NEXTHOP_LISTS  32
#define SOS_DEST_PBR_NHL_START      SOS_DEST_TUNNEL_GROUP_END
#define SOS_DEST_PBR_NHL_END        (SOS_DEST_PBR_NHL_START + \
                                     SOS_DEST_PBR_NEXTHOP_LISTS)

/* special NULL dest for empty inactive nexthop-lists */
#define SOS_DEST_PBR_NULL_DEST SOS_DEST_PBR_NHL_START

/*
 * Nexthop dests for PBR
 * Indexing starts from SOS_DEST_PBR_NH_START + 1 to
 * keep it non-zero
 */
#define SOS_MAX_NH_PER_PBR_NHL      16
#define SOS_DEST_PBR_NEXTHOPS       (SOS_MAX_NH_PER_PBR_NHL * \
                                     SOS_DEST_PBR_NEXTHOP_LISTS - 1)
#define SOS_DEST_PBR_NH_START       SOS_DEST_PBR_NHL_END
#define SOS_DEST_PBR_NH_END         (SOS_DEST_PBR_NH_START + \
                                     SOS_DEST_PBR_NEXTHOPS)

#if defined(AOS_32BIT_TUNNEL_INDEX_SUPPORT)
#define SOS_DEST_TUNNEL_BASE        0x10000
#else
#define SOS_DEST_TUNNEL_BASE        (SOS_DEST_MCASTS+SOS_DEST_UCASTS)
#endif
#define SOS_DEST_TUNNEL_OLD_BASE    (SOS_DEST_MCASTS+SOS_DEST_UCASTS)


/* 
 * ipsec-map id to tunnel dest mapping
 * Indexing starts from SOS_DEST_IPSEC_ID_START + 1 to
 * keep it non-zero
 */
#define SOS_DEST_IPSEC_ID           14500
#define SOS_DEST_IPSEC_ID_START     SOS_DEST_PBR_NH_END
#define SOS_DEST_IPSEC_ID_END       (SOS_DEST_IPSEC_ID_START + \
                                     SOS_DEST_IPSEC_ID)

/*==================================================================
* Support named tunnels (like css-tunnel) - A special range of Dest
*==================================================================*/
#define SOS_DEST_MGMT_PORT          0x6000
#define SOS_DEST_NAMED_TUNNEL_START (0x7FF0)
#define SOS_DEST_NAMED_TUNNEL_END   (0x7FFF)
#define SOS_DEST_NAMED_TUNNELS      (16)
#define SOS_DEST_CSS_TUNNEL_START   SOS_DEST_NAMED_TUNNEL_START
#define SOS_DEST_CSS_TUNNEL_END     SOS_DEST_CSS_TUNNEL_START
#define SOS_DEST_CSS_TUNNELS        (1)

#define SOS_DEST_CP                 0
#define SOS_DEST_OPCODE             0x8000
#define SOS_DEST_OPCODE_START       SOS_DEST_OPCODE 
#define SOS_DEST_OPCODE_END         0xFFFF

/* Range of TUNNELS created for OpenFlow in AP */
#define SOS_DEST_OFTUNNEL_START     0x3E80
#define SOS_DEST_OFTUNNEL_COUNT     128
#define SOS_DEST_OFTUNNEL_END       (SOS_DEST_OFTUNNEL_START + \
                                    SOS_DEST_OFTUNNEL_COUNT)
#define SOS_DEST_TO_OFTUNNEL(Dest)  ((Dest) - (SOS_DEST_OFTUNNEL_START))
#define SOS_DEST_FROM_OFTUNNEL(Tun) ((Tun) + (SOS_DEST_OFTUNNEL_START))
#define SOS_DEST_IS_OFTUNNEL(Dest)  \
                                    ((Dest) >= (SOS_DEST_OFTUNNEL_START) && \
                                    (Dest) <= (SOS_DEST_OFTUNNEL_END))


#define SOS_DEST_IS_MCAST(Dest) \
    ((Dest) < SOS_DEST_MCASTS)
#define SOS_DEST_IS_UCAST(Dest) \
    ((((__u32)Dest) - SOS_DEST_MCASTS) < SOS_DEST_UCASTS)
/* PORF_SCALE_REVISIT: Need to change the isTunnel check to start from 64k onwards */
#define SOS_DEST_IS_TUNNEL(Dest) \
    ((Dest) >= (SOS_DEST_TUNNEL_BASE))
#define SOS_DEST_IS_NONPC_TUNNEL(Dest) \
    ((Dest) >= (SOS_DEST_TUNNEL_BASE) + (MXC_PORT_CHANNELS))
#define SOS_DEST_IS_NAMED_TUNNEL(Dest) \
    ((Dest) >= (SOS_DEST_NAMED_TUNNEL_START) && \
     (Dest) <= (SOS_DEST_NAMED_TUNNEL_END))
#define SOS_DEST_IS_CSS_TUNNEL(Dest) \
    ((Dest) >= (SOS_DEST_CSS_TUNNEL_START) && \
     (Dest) <= (SOS_DEST_CSS_TUNNEL_END))
#define SOS_DEST_IS_OPCODE(Dest) \
    ((Dest) >= (SOS_DEST_OPCODE_START) && \
     (Dest) <= (SOS_DEST_OPCODE_END))
#define SOS_DEST_IS_DROP(Dest) \
    ((Dest) == SOS_DEST_DROP)
#define SOS_DEST_IS_TUNNEL_GROUP(Dest) \
    ((Dest) >= (SOS_DEST_TUNNEL_GROUP_START) && \
     (Dest) <= (SOS_DEST_TUNNEL_GROUP_END))
#define SOS_DEST_IS_PORTCHANNEL(Dest) \
    (SOS_DEST_IS_TUNNEL(Dest) && (SOS_DEST_TO_TUNNEL(Dest) <= MXC_PORT_CHANNELS))

#define SOS_DEST_IS_WIFI_TUNNEL(Dest) \
    (SOS_DEST_IS_TUNNEL((Dest)) && \
    (TUNNEL_TO_ENTRY(SOS_DEST_TO_TUNNEL((Dest)))->flags & SOS_TUNNEL_FLAGS_WIFI))

#define SOS_DEST_IS_MUX_TUNNEL(Dest) \
    (SOS_DEST_IS_TUNNEL((Dest)) && \
    ((TUNNEL_TO_ENTRY(SOS_DEST_TO_TUNNEL((Dest)))->flags & \
     (SOS_TUNNEL_FLAG_ETHER |SOS_TUNNEL_FLAG_WIRED |SOS_TUNNEL_FLAG_PERMANENT))\
     == (SOS_TUNNEL_FLAG_ETHER |SOS_TUNNEL_FLAG_WIRED |SOS_TUNNEL_FLAG_PERMANENT)))

#define SOS_DEST_IS_IP_MOBILITY_TUNNEL(Dest) \
    (SOS_DEST_IS_TUNNEL((Dest)) && \
    ((TUNNEL_TO_ENTRY(SOS_DEST_TO_TUNNEL((Dest))))->flags & SOS_TUNNEL_FLAG_L2GRE_MOB))
#define SOS_DEST_IS_TUNNEL_FLAG_IP_MOB(Dest) \
    ((TUNNEL_TO_ENTRY(SOS_DEST_TO_TUNNEL((Dest))))->flags & SOS_TUNNEL_FLAG_L2GRE_MOB)

#define SOS_DEST_IS_PBR_NHL(Dest) \
    ((Dest) > (SOS_DEST_PBR_NHL_START) && \
     (Dest) <= (SOS_DEST_PBR_NHL_END))
#define SOS_DEST_IS_PBR_NH(Dest) \
    ((Dest) > (SOS_DEST_PBR_NH_START) && \
     (Dest) <= (SOS_DEST_PBR_NH_END))
#define SOS_DEST_IS_IPSECMAP(Dest) \
    ((Dest) > (SOS_DEST_IPSEC_ID_START) && \
    (Dest) <= (SOS_DEST_IPSEC_ID_END))
/*
 * update this if the dest index range after tunnels is allocated
 */
#define SOS_DEST_IS_INVALID(Dest) \
    ((Dest) > (SOS_DEST_TUNNEL_BASE + SOS_DEST_TUNNELS))

#define SOS_DEST_FROM_SLOT_INTERNAL(Slot) \
    ((Slot) * SOS_DEST_SLOT_PORTS + SOS_DEST_MCASTS)
#define SOS_DEST_FROM_SLOTPORT_INTERNAL(Slot, Port) \
    ((Slot) * SOS_DEST_SLOT_PORTS + SOS_DEST_MCASTS + (Port))

#define SOS_DEST_FROM_SLOTMODPORT(Slot, Module, Port) \
    ((Slot) * SOS_DEST_SLOT_PORTS + SOS_DEST_MCASTS + \
    (Module) * SOS_DEST_MODULE_PORTS + Port)

#define SOS_DEST_FROM_TUNNEL(Tun) \
    ((Tun) + (SOS_DEST_TUNNEL_BASE))
#define SOS_DEST_FROM_TRUNK(Tr) \
    SOS_DEST_FROM_TUNNEL(Tr)
#define SOS_DEST_FROM_OPCODE(Opcode) \
    ((Opcode) | SOS_DEST_OPCODE)

#define SOS_DEST_TO_MCAST(Dest) \
    (Dest)
#define SOS_DEST_TO_SLOT_INTERNAL(Dest) \
    (((Dest) - SOS_DEST_MCASTS)/SOS_DEST_SLOT_PORTS)
#define SOS_DEST_TO_PORT_INTERNAL(Dest)  \
    ((Dest) & (SOS_DEST_SLOT_PORTS-1))
#define SOS_DEST_TO_INT_PORT_INTERNAL(Dest)  \
    ((Dest) & (SOS_DEST_SLOT_PORTS-1))

#define SOS_DEST_TO_TUNNEL(Dest) \
    ((Dest) - (SOS_DEST_TUNNEL_BASE))
#define SOS_DEST_TO_TRUNK(Tr) \
    SOS_DEST_TO_TUNNEL(Tr)
#define SOS_DEST_TO_NAMED_TUNNEL(Dest) \
    ((Dest) - (SOS_DEST_NAMED_TUNNEL_START))
#define SOS_DEST_TO_CSS_TUNNEL(Dest) \
    ((Dest) - (SOS_DEST_CSS_TUNNEL_START))
#define SOS_DEST_TO_OPCODE(Dest) \
    ((Dest) & ~SOS_DEST_OPCODE)
#define SOS_DEST_TO_PBR_NHL(Dest) \
    ((Dest) - (SOS_DEST_PBR_NHL_START))
#define SOS_DEST_TO_PBR_NH(Dest) \
    ((Dest) - (SOS_DEST_PBR_NH_START))
#define SOS_DEST_TO_IPSECMAP_ID(Dest) \
    ((Dest) - (SOS_DEST_IPSEC_ID_START))

#define SOS_DEST_PORT_IS_BLOCKED(Dest) \
     (pt.port[SOS_DEST_TO_SLOT_INTERNAL(Dest)][SOS_DEST_TO_INT_PORT_INTERNAL(Dest)].flags & \
      SOS_PORT_FLAG_BLOCKED)

#define INTER_CARD_VLAN_ID  4094
#define INTER_CARD_PORT_NUM 0x1E

#include "sos_slot_conv.h"
#define SOS_DEST_TYPE_PORT          0x01
#define SOS_DEST_TYPE_PC            0x02
#define SOS_DEST_TYPE_WIFI_TUNNEL   0x03
#define SOS_DEST_TYPE_L2GRE_TUNNEL  0x04
#define SOS_DEST_TYPE_L3GRE_TUNNEL  0x05
#define SOS_DEST_TYPE_IPSEC_TUNNEL  0x06
#define SOS_DEST_TYPE_IAP_TUNNEL    0x07
#define SOS_DEST_TYPE_OF_TUNNEL     0x08
#define SOS_DEST_TYPE_CSS_TUNNEL    0x09
#define SOS_DEST_TYPE_NAMED_TUNNEL  0x0a

typedef __u16 slotid_t;
typedef __u16 moduleid_t;
typedef __u16 portid_t;
typedef __u16 ip_t[2]; 
typedef __u16 ip4_t[2];
typedef __u16 ip6_t[8];

typedef union {
        __u16 mac[3];
        ip_t v4;     /* ip address */
        ip6_t v6;
} mac_ip_u_t;

typedef union {
    ip_t v4;
    ip6_t v6;
} ip_u_t;

#if defined(AOS_32BIT_TUNNEL_INDEX_SUPPORT)
typedef    __u32    sos_dst_t;
#define sosdst_hton(d)  htonl(d)
#define sosdst_ntoh(d)  ntohl(d)
#else
typedef    __u16    sos_dst_t;
#define sosdst_hton(d)  htons(d)
#define sosdst_ntoh(d)  ntohs(d)
#endif

typedef enum {
    NH_TYPE_INVALID,
    NH_TYPE_IP,
    NH_TYPE_GRE,
    NH_TYPE_IPSEC,
} nh_type_t;

/* 
 * union of different sos nexthop types:
 * IP or L3 tunnels (GRE, IPSEC)
 */
typedef union {
    ip_u_t     ipu;
    sos_dst_t  dest;
} nhu_t;

typedef struct {
    nhu_t       nhu;          /* nh union dataset */
    __u16       nh_idx;       /* nexthop idx */
    __u16       nh_vlan;      /* NH vlan */
    __u8        nh_vernum;    /* version numbering to identify updates */
    __u8        nh_priority;  /* nexthop priority */
    __u16       pad;
    nh_type_t   nh_type;      /* type of NH - Tunnel or IP */
} nh_t;

typedef struct {
    __u16       nh_idx;
    __u16       nh_nhl_vernum;
    __u8        nhl_idx;
    __u8        pad_1;
    __u16       pad_2;
} session_nh_t;

/* To make structure aligned to a double-word boundary */
#define SOS_STRUCT_DWORD_ALIGN        (sizeof(sos_dst_t)/sizeof(__u32))
/* To pad a structure in order make it aligned at a cacheline boundary */
#define SOS_STRUCT_DWORD_PAD          (sizeof(__u32)/sizeof(sos_dst_t))

#endif /* _SOS_TYPES_H_ */
