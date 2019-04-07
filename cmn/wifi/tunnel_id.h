#ifndef _TUNNEL_ID_H_
#define _TUNNEL_ID_H_


/* WARNING!!!  WARNING!!!  WARNING!!! */
/* NOTE: everytime you add or modify these, please make sure to update all
         instances in code including drivers/tun.c and stm/enet_mgmt.c to
         make sure single and double encryption is followed properly.  In
         general, ONLY wireless traffic in tunnel mode is allowed to go
         single encryption when configured to do so.  All other cases must go
         through the double encryption (ie, IPSEC encrypted) code path */
#define TUNNEL_ID_CORP_ACCESS   0x0001
#define TUNNEL_ID_SPLIT         0x8000
#define TUNNEL_ID_ASAP_SOS	0x8080 // only for grenache

#define TUNNEL_ID_L3_MOB_MIN        0x8800
#define TUNNEL_ID_L3_MOB_MIN_DATA   0x8800
#define TUNNEL_ID_L3_MOB_MAX_DATA   0x8880
#define TUNNEL_ID_L3_MOB_HB         0x88ff
#define TUNNEL_ID_L3_MOB_MAX        0x8900

#define TUNNEL_ID_NAT_MIN           0x8e00
#define TUNNEL_ID_NAT_MAX           0x8f5f /*  wo now support to max 64 + 32 role,
                                               the acl number max should be 146 + 2 * 96 (0x152)
                                               so this number at least should be 0x8e00+0x0152 (0x8f52)
                                           */

// these constants should only be used for initialization
#define TUNNEL_ID_SJ0           0x8100      // eth0 on mesh points
#define TUNNEL_ID_SJ1           0x8110      // eth1
#define TUNNEL_ID_SJ2           0x8120      // eth2
#define TUNNEL_ID_SJ3           0x8130      // eth3
#define TUNNEL_ID_SJ4           0x8140      // eth4
#define TUNNEL_ID_SJ5           0x8150      // eth5
#define TUNNEL_ID_SJ6           0x8160      // eth6
#define TUNNEL_ID_SJ7           0x8170      // eth7

#define	TUNNEL_ID_SJ_MAX TUNNEL_ID_SJ7

#define TUNNEL_ID_THIN  0x8200
#define TUNNEL_ID_HBT   0x9000

#define	TUNNEL_ID_SJ(tunnel_id)                 \
            ((tunnel_id) >= TUNNEL_ID_SJ0 && (tunnel_id) <= TUNNEL_ID_SJ_MAX)
#define TUNNEL_ID_8023(tunnel_id) ((tunnel_id) < TUNNEL_ID_THIN)
#define TUNNEL_ID_VAP(ti) ((ti) >= TUNNEL_ID_THIN && (ti) < TUNNEL_ID_HBT)

#define TUNNEL_ID_DOTG(ti) ((ti & 0x300) == 0x300)
#define TUNNEL_ID_DOTA(ti) ((ti & 0x300) == 0x200)

#define TUNNEL_ID_LAG_SEQ_B0_B3_MASK     0x000F
#define TUNNEL_ID_LAG_SEQ_B0_B3_SHIFT    0
#define TUNNEL_ID_LAG_SEQ_B4_B5_MASK     0x0C00
#define TUNNEL_ID_LAG_SEQ_B4_B5_SHIFT    6
#define TUNNEL_ID_LAG_SEQ_B6_B7_MASK     0x6000
#define TUNNEL_ID_LAG_SEQ_B6_B7_SHIFT    7
#define TUNNEL_ID_LAG_SEQ_MASK           0x6C0F
#define TUNNEL_ID_SET_LAG_SEQ(t, s)                                                     \
    do {                                                                                \
        (t) |= (((s) << TUNNEL_ID_LAG_SEQ_B0_B3_SHIFT) & TUNNEL_ID_LAG_SEQ_B0_B3_MASK) |\
               (((s) << TUNNEL_ID_LAG_SEQ_B4_B5_SHIFT) & TUNNEL_ID_LAG_SEQ_B4_B5_MASK) |\
               (((s) << TUNNEL_ID_LAG_SEQ_B6_B7_SHIFT) & TUNNEL_ID_LAG_SEQ_B6_B7_MASK); \
    } while (0) 
#define TUNNEL_ID_CLR_LAG_SEQ(t)  ((t) &= ~TUNNEL_ID_LAG_SEQ_MASK)
    
#define TUNNEL_ID_GET_LAG_SEQ(t)                                                  \
        (((t) & TUNNEL_ID_LAG_SEQ_B0_B3_MASK) >> TUNNEL_ID_LAG_SEQ_B0_B3_SHIFT) | \
        (((t) & TUNNEL_ID_LAG_SEQ_B4_B5_MASK) >> TUNNEL_ID_LAG_SEQ_B4_B5_SHIFT) | \
        (((t) & TUNNEL_ID_LAG_SEQ_B6_B7_MASK) >> TUNNEL_ID_LAG_SEQ_B6_B7_SHIFT)

/**************************************
 * skb->flags is 16 bit
 **************************************/
#define TUNNEL_SKB_FLAG_UP_BW_CONTROL   0x0080

#define TUNNEL_SKB_FLAG_80211           0x0800 /* used to signal /dev/tun that frame needs encryption */

#define TUNNEL_SKB_FLAG_IPSECENCAP      0x0800 /* tun device marks frame as encap'd through IPSEC already.  
                                                * We are overloading this flag with FLAG_80211 */
#define TUNNEL_SKB_FLAG_SOS_ROUTE     /*0x0100*/SOS_MSG_FLAG_ROUTE /*net/ipv4/route.c*/
#define TUNNEL_SKB_SPLIT_CORP           0x0200
#define TUNNEL_SKB_SPLIT_NO_CORP        0x0400
// if both flags are set, we think this is an L2 frame
#define TUNNEL_SKB_SPLIT_L2             (TUNNEL_SKB_SPLIT_CORP|TUNNEL_SKB_SPLIT_NO_CORP)

#define TUNNEL_SKB_FLAG_UP_BW_Q1        0x1000
#define TUNNEL_SKB_FLAG_UP_BW_Q2        0x2000
#define TUNNEL_SKB_FLAG_UP_BW_Q3        0x4000
#define TUNNEL_SKB_FLAG_UP_BW_Q4        0x8000
/* for STP */
#define TUNNEL_SKB_FLAG_STP_DONE        SOS_MSG_FLAG_STP_DONE
/*for RAP-NG*/
#define TUNNEL_SKB_FLAG_PKTFROMTUN      0x10000
#define TUNNEL_SKB_FLAG_IS_MTU_PROBE_PKT 0x100000
#define TUNNEL_SKB_FLAG_IS_HBT_PKT	0x200000
#define TUNNEL_SKB_FLAG_IS_STANDBY_HBT	0x400000
#define TUNNEL_SKB_FLAG_IS_OPENFLOW_DEV	0x800000
#define TUNNEL_SKB_FLAG_PKTFROMWIFI     0x04000000
#define TUNNEL_SKB_FLAG_GRE_RADIOG      0x08000000
#define TUNNEL_SKB_FLAG_GRE_RADIOA      0x10000000

#define TUNNEL_SKB_FLAG_ETH_CAP_PKT     0x20000000

#define TUNNEL_SKB_FLAG_DMO_CONVERTED   0x40000000

#if (!TUNNEL_ID_8023(TUNNEL_ID_SPLIT))
#error "(!TUNNEL_ID_8023(TUNNEL_ID_SPLIT))"
#endif
#if (!TUNNEL_ID_8023(TUNNEL_ID_SJ_MAX))
#error "(!TUNNEL_ID_8023(TUNNEL_ID_SJ_MAX))"
#endif
#if (!TUNNEL_ID_8023(TUNNEL_ID_SJ1))
#error "(!TUNNEL_ID_8023(TUNNEL_ID_SJ1))"
#endif
#if (!TUNNEL_ID_8023(TUNNEL_ID_SJ0))
#error "(!TUNNEL_ID_8023(TUNNEL_ID_SJ0))"
#endif


#define GET_VAP_TUNNEL_ID(phy_num, vap_id) (TUNNEL_ID_THIN + ((phy_num)*0x100) \
                                            + ((vap_id)*0x10))
#define TUNNEL_ID_TO_PHY(tunnel_id) ((((tunnel_id) - TUNNEL_ID_THIN) & 0x100) >> 8)
#define TUNNEL_ID_TO_VAP(tunnel_id) ((((tunnel_id) - TUNNEL_ID_THIN) & 0xf0) >> 4)
#define TUNNEL_ID_TO_SJ(tunnel_id)  ((((tunnel_id) - TUNNEL_ID_SJ0) & 0xf0) >> 4)
#define GET_SJ_TUNNEL_ID(enet_id)   (TUNNEL_ID_SJ0 + ((enet_id) * 0x10))

/*If a new tunnel type is added and is maintained by STM, update this
 * macro as well if you want your tunnel to be cleaned up in the event of an
 * stm crash and its eventual restart by nanny */
#define IS_STM_TUNNEL(tun_id) (tun_id >= 0x8000 && tun_id <= 0x8400) \
                                         || (tun_id == TUNNEL_ID_HBT)

#endif
