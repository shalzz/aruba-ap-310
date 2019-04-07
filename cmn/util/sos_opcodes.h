
#if defined(BUILD_ENUM_STR_DEFN)
#undef __SOS_OPCODES_H__
#endif

#ifndef __SOS_OPCODES_H__
#define __SOS_OPCODES_H__

#include "dense_enum_macro.h"

#define SOS_OPCODE_PREFIX "SOS_MSG_OPCODE_"

/* Please use all lowercase for alphabet(s) in HEX 
 * value introduced for new OPCODE(s).
 */

ARUBA_DENSE_ENUM_DEF_START(sos_opcode, SOS_MSG_OPCODES)

    ARUBA_DENSE_ENUM_ENTRY_W_VAL_N_USRSTR(
        sos_opcode, SOS_MSG_OPCODE_FREE, 0x0, "SOS_MSG_OPCODE_RAW/FREE")
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FLOOD, 0x1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BRIDGE, 0x2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ROUTE, 0x3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SESSION, 0x4)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FORWARD, 0x5)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HELLO, 0x6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_APP, 0x7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ARP, 0x8)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FRAG ,0x9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WIFI, 0xa)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PING1, 0xb)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ALLOC, 0xc)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021X, 0xd)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MOBILITY, 0xe)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACL, 0xf)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACE, 0x10)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_USER, 0x11)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VLAN, 0x12)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MCAST, 0x13)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PORT, 0x14)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MAC, 0x15)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ROUTE_CLEAR, 0x16)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TUNNEL, 0x17)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LAG, 0x18)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BPDU, 0x19)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LAPDU, 0x1a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BWM, 0x1b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VPN, 0x1c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WKEY, 0x1d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_STATION, 0x1e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PPTP, 0x1f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TEST0, 0x20)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TEST1, 0x21)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TEST2, 0x22)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PHY, 0x23)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_CRC, 0x24)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MIC, 0x25)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MICCRC, 0x26)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TKIPPHASE1, 0x27)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WIRED_STATS, 0x28)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LEARN, 0x29)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_NAT, 0x2a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_USER_TMOUT, 0x2b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LOOPRC4, 0x2c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LOOPDDR, 0x2d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LOOPPPTP, 0x2e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MOBILITY_L2, 0x2f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LED, 0x30)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LOOPIPSEC, 0x31)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DHCP, 0x32)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_KERBEROS, 0x33)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_RADIUS, 0x34)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACELOG, 0x35)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACESTATS, 0x36)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DNSLOCALMS, 0x37)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FORWUNENC, 0x38)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PANIC, 0x39)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_CDPDU, 0x3a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FABRIC, 0x3b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LOOP, 0x3c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_STATS, 0x3d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FREECTX, 0x3e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VPN_CLEAR, 0x3f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AUTH, 0x40)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_CPDNS, 0x41)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LOCK, 0x42)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PPTPACK, 0x43)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VRRPDU, 0x44)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BADL2DECRYPT, 0x45)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TKIPKEY, 0x46)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FORWARDDOT1X, 0x47)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FIREWALL, 0x48)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FILE, 0x49)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_GETNEXT, 0x4a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AUTH_L2, 0x4b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TUNNEL_FP, 0x4c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WEPENC, 0x4d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WEPDEC, 0x4e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IPSECENC, 0x4f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IPSECDEC, 0x50)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TKIPENC, 0x51)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TKIPDEC, 0x52)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PPTPENC, 0x53)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PPTPDEC, 0x54)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IPSECSA, 0x55)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESCCMENC, 0x56)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESCCMDEC, 0x57)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ADPDU, 0x58)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_STATION2, 0x59)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DHCPDBG, 0x5a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TUNNEL_TMOUT, 0x5b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SLB, 0x5c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WEPDFRAG, 0x5d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESCCMKEY, 0x5e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IGMP, 0x5f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PIM, 0x60)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PREAUTH, 0x61)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MACH_INFO, 0x62)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESCCMDFRAG, 0x63)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_NETAD, 0x64)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_NETADLOG, 0x65)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SESSIONPPP, 0x66)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_XSECENC, 0x67)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_XSECDEC, 0x68)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_XSECKEY, 0x69)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SERVICE, 0x6a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VOIP, 0x6b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IKETRIGGER, 0x6c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_STATS2, 0x6d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_CARD_IN_SLOT, 0x6e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021XTERM, 0x6f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_EAPWOTLS, 0x70)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_POST, 0x71)
    /* Bug 131446 SOS_MSG_OPCODE_RAND not used Remove when renumbering */
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_RAND, 0x72)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ROUTEIPSEC, 0x73)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DHCPC, 0x74)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PRIO, 0x75)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PPPOE, 0x76)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AMAP, 0x77)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IKEDPD, 0x78)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACE2, 0x79)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACL2, 0x7a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_USER2, 0x7b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_NAT2, 0x7c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SESSMGMT, 0x7d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_EAPKEY, 0x7e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TLSSEND, 0x7f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TLSKEYEXCH, 0x80)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_L2TPHELLO, 0x81)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_NTLM, 0x82)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SRC_BRIDGEUPDATE, 0x83)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HWDROP, 0x84)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DNSPROXY, 0x85)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_L2TP, 0x86)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HWSRCMAC, 0x87)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HWDSTMAC, 0x88)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HWEDA, 0x89)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FWDCP, 0x8a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BWM_QUEUE, 0x8b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PINOT_CUTTHRO, 0x8c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BACKPLANE_DATA, 0x8d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MVC, 0x8e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021XTERM_SMRST, 0x8f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACK_CP, 0x90)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MLD, 0x91)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ACL_PKTTRACE, 0x92)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_OSPF, 0x93)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021XLOOP, 0x94)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_GRENACHE_AM, 0x95)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HOST_DOMAIN_NAME, 0x96)
    /*XLR RSA core is used to perform modular exponential operation
     * NOTE : RSA core is used for DH, RSA sign, RSA decrypt operation*/
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_RSA_MOD_EXP, 0x97)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_IKESOS, 0x98)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_STP, 0x99)
    /* This is to fill a hole due to deletion of DST_BRIDEGE opcode; 
     * On conflict please don't pull this into any release prior to 6.2
     */
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WIFI_STATION, 0x9a)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VIA_SSL_FALLBACK, 0x9b)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FWDNS, 0x9c)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_RTP_CALC, 0x9d)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WIRED_AUTH_TERM, 0x9e)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_UPD_SESSION, 0x9f)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BULK_ACL_ACE, 0xa0)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BULK_ACL, 0xa1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BULK_ACE, 0xa2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BULK_BWM, 0xa3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BULK_USER, 0xa4)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_USER_DP, 0xa5)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021XTERM_SP, 0xa6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_INTF_MAXMACS, 0xa7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SET_ACL_FALLBACK_WIRED_USER,
                                 0xa8)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PORTSEC_LOG, 0xa9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESGCMENC, 0xaa)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESGCMDEC, 0xab)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESGCMKEY, 0xac)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AESGCMDFRAG, 0xad)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_RSA_2048_SIGN, 0xae)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_EHMAP, 0xaf)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IPV6, 0xb0)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ROUTE6, 0xb1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_INTERIM_STATS, 0xb2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_CTR_CHECK, 0xb3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_CTR_CHECK_WINDOW, 0xb4)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_UA_STR_UPDATE, 0xb5)
#if 0
    /* Do no comment like this will cause crash as the ID string will not be
     * found*/
    /* 0xb6 was used for SOS_MSG_OPCODE_DHCP_RESP_AUTH which is removed now.
     * this can be reused now for some other messges.
     */
#endif    
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PING_IPV6, 0xb6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TCPDIFF_FREE, 0xb7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BACK_TO_FP, 0xb8)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SET_EPOCH, 0xb9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_QINQ_MODE, 0xba)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_GLOBAL_PKTTRACE, 0xbb)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BLK_MAC, 0xbc)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FW_CP_DMA_COMPLETE, 0xbd)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FW_DMA_ADDR, 0xbe)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_FW_AGG_SESS_EXPORT, 0xbf)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AUTH_STATS, 0xc0)
    /* MDNS Proxy is an independent user space process */
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MDNS, 0xc1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IPSEC_VOLREKEY, 0xc2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MOBILITY_HA_DISC, 0xc3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TRAP_RA_RS, 0xc4)                           
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PKTCAPTURE, 0xc5)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TUNNEL_GROUP, 0xc6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_HA_LITE, 0xc7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_MOBILITY_HA_HAT, 0xc8)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode, SOS_MSG_OPCODE_FIPS_ECDSA_VECTOR_TEST, 0xc9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AP_ROUTE, 0xca)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SET_FEATURE, 0xcb)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DPI, 0xcc)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SESSION_FROM_DPIFP, 0xcd)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DPI_ENF_SESSION, 0xce)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_LLDP, 0xcf)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_FORWARD_WITH_EGR_STATS_UPD, 0xd0)    
    /* using this for AUTH to send the client dataready */
    /* check bug 90851 for more details*/
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_CLIENT_DATAREADY, 0xd1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_JABBER_SRV, 0xd2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IP_FLOW_CACHE_CP_DMA_COMPLETE, 0xd3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IP_FLOW_CACHE_DMA_ADDR, 0xd4)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IP_FLOW_CACHE_EXPORT, 0xd5)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
	                         SOS_MSG_OPCODE_DHCPPROXY, 0xd6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
	                         SOS_MSG_OPCODE_PBR, 0xd7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WEB_CC, 0xd8)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WEB_CC_SESSION, 0xd9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_SET_ACL_WIRELESS_CONSOLE_USER, 0xda)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_OPENFLOW, 0xdb)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_UNUSED1, 0xdc)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_X86_CONSOLE, 0xdd)
	ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
				 SOS_MSG_OPCODE_IKE_IPCOMP, 0xde)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_EGR_BWM, 0xdf)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_INGR_BWM, 0xe0)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PORT_PHY_BWM, 0xe1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
	                         SOS_MSG_OPCODE_AMON_SRV_CFG, 0xe2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ROUTE_NH, 0xe3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DNS, 0xe4)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_AMON_MSG_REGISTER, 0xe5)

    /* This is to be used to program DP for other Traffic Selectors in a single Crypto-map */
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DPI_APPRF, 0xe6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_DPI_APPRF_FILE, 0xe7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WIFICALL_DNS_ADDR, 0xe8)
    /* This is to be used to program Secondary Traffic Selectors to datapath. */
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_VPN_OTHER, 0xe9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_UCM_DNS_PATTERN, 0xea)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                SOS_MSG_OPCODE_UPLINK_LB_UPDATE, 0xeb)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_UPDATE_SESSION_NEXTHOP, 0xec)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_EGR_TX, 0xed)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode, SOS_MSG_OPCODE_UDP_PROBE, 0xee)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode, SOS_MSG_OPCODE_UDP_JITTER, 0xef)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IKESOS_PBR, 0xf0)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_ANTI_VIRUS, 0xf1)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WIFICALL_IP_ADDR_DEL, 0xf2)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_PAPISEC_KEY, 0xf3)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_IPSEC_UPDATE_SA, 0xf4)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_LC_CLUSTER, 0xf5)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_OPENFLOW_AUX, 0xf6)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_OPENFLOW_CP_REPLY, 0xf7)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TNL_STATS_CP_DMA_COMPLETE, 0xf8)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TNL_STATS_DMA_ADDR, 0xf9)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TNL_STATS_EXPORT, 0xfa)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_TUNNELED_NODE_TMOUT, 0xfb)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021XTERM_DOT1X, 0xfc)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_8021X_EAPOL_START, 0xfd)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WAN_POLICY_ENTRY, 0xfe)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_BULK_WAN_POLICY_ENTRY, 0xff)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_WAN_THRESHOLD_PROFILE_ENTRY, 0x100)
    ARUBA_DENSE_ENUM_ENTRY_W_VAL(sos_opcode,
                                 SOS_MSG_OPCODE_APM_POLICY_MANAGER_ENTRY, 0x101)

/* Following entry has to be the last entry */
    ARUBA_DENSE_ENUM_ENTRY(sos_opcode,
                           SOS_MSG_OPCODES)
ARUBA_DENSE_ENUM_DEF_END(sos_opcode,                
                         SOS_MSG_OPCODES,           
                         "SOS_MSG_OPCODE_UNKNOWN",  
                         SOS_OPCODE_PREFIX)


#endif /* __SOS_OPCODES_H__ */

