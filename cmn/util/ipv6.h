/* ------------------------------------------------------------------------- 
 * Name        : ipv6.h
 * Description : 
 * ------------------------------------------------------------------------- */

#ifndef IPV6_H
#define IPV6_H
#ifdef ARUBA_IPV6 
#include <assert.h>
#ifndef NETINET_INH
#define NETINET_INH
#undef IPPROTO_ESP
#undef IPPROTO_AH
#undef IPPROTO_HOPOPTS
#undef IPPROTO_IPIP
#undef IPPROTO_ICMP
#undef IPPROTO_TCP
#undef IPPROTO_UDP
#undef IPPROTO_IPV6
#undef IPPROTO_ICMPV6
#undef IPPROTO_ROUTING
#undef IPPROTO_DSTOPTS
#undef IPPROTO_FRAGMENT
#undef IPPROTO_NONE
#include <netinet/in.h>
#endif /* NETINET_INH */
/* IPv6 specific changes for Aruba Networks */
#define IPV6_MAX_BYTELEN    16
#define IPV6_PREFIX_BYTELEN 8
#define IPV6_MAX_BITLEN    128
#define IPV6_MAX_PREFIXLEN 128
#define MAX_IPV6_ADDR_PER_INTF 3

#define IPV6_ADDR_CMP(D,S)   memcmp ((D), (S), IPV6_MAX_BYTELEN)
#define IPV6_ADDR_SAME(D,S)  (memcmp ((D), (S), IPV6_MAX_BYTELEN) == 0)
#define IPV6_ADDR_COPY(D,S)  memcpy ((D), (S), IPV6_MAX_BYTELEN)
#define IPV6_IS_NULL_ADDR(addr) \
      ((((prefixIpv6_t *)(addr))->prefix.s6_addr32[0] == 0) && \
            (((prefixIpv6_t *)(addr))->prefix.s6_addr32[1] == 0) && \
            (((prefixIpv6_t *)(addr))->prefix.s6_addr32[2] == 0) && \
            (((prefixIpv6_t *)(addr))->prefix.s6_addr32[3] == 0) )

#define IPV6_ADDR_PREFIX_SAME(D,S) \
    ((memcmp (&(((prefixIpv6_t *)(D))->prefix),&(((prefixIpv6_t *)(S))->prefix),IPV6_MAX_BYTELEN)== 0)&& \
    ((((prefixIpv6_t *)(D))->prefixLen)==(((prefixIpv6_t *)(S))->prefixLen)))
#define IPV6_PREFIX_MATCH(D,S) \
    (memcmp (&(((prefixIpv6_t *)(D))->prefix),&(((prefixIpv6_t *)(S))->prefix),IPV6_PREFIX_BYTELEN)== 0)
#define IN6_IS_ADDR_GLOBAL(a) \
    ((((const unsigned char *) (a))[0] & 0xe0) == 0x20)


#define IN6_IS_ADDR_UNIQUELOCAL(a) \
    ((((__const uint32_t *) (a))[0] & htonl (0xfe000000)) == htonl (0xfc000000))

//Layer3 Vlan IPv6 Address Commands
typedef struct prefixIpv6_s
{
    unsigned char prefixLen;
    struct in6_addr prefix  __attribute__ ((aligned (8)));
}prefixIpv6_t;

typedef struct ipv6NbrMac_s{
    prefixIpv6_t ipv6Addr;    
    int intIfNum;
    int ifType;  
    unsigned char macAddr[6];
}ipv6NbrMac_t;

typedef struct ipv6NbrEntry_s{
    prefixIpv6_t ipv6Addr;    
    int intIfNum;
    unsigned int vlanId;
    unsigned int ifType;  
    unsigned int flags;  
    unsigned char macAddr[6];
}ipv6NbrEntry_t;

#endif //ARUBA_IPV6

#ifdef AP_PLATFORM
#define IPV6_MAX_BYTELEN    16
#define IPV6_ADDR_COPY(D,S)  memcpy ((D), (S), IPV6_MAX_BYTELEN)
#endif
#endif
