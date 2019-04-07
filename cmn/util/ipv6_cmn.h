#ifndef _IPv6_CMN_H_
#define _IPv6_CMN_H_
#if defined (SWITCH_PLATFORM)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <util/ipv6.h>
char *inet_ntoa_hton(struct in_addr);
#endif

#if 0
//v6todo: really need to sort this out
#ifdef __KERNEL__
#include <linux/inet.h>
#else
#include <arpa/inet.h>
#endif
#endif
#include <asm/types.h>

// Generic IP address structure and associated utils
struct sockaddr_in6;
typedef struct {
    __u16           af;
    union {
        __u32       addr4;
        __u8        addr6[16];
	__u16		addr6_16[8];
	__u32		addr6_32[4];
    } addr;
} ip_addr_t;
#define v4addr		addr.addr4
#define v6addr		addr.addr6
#define v6addr16	addr.addr6_16
#define v6addr32	addr.addr6_32

extern __u8 ipv6_loopback[16];
extern __u8 ipv6_zeros[16];
extern __u8 ipv6_v4MapPrefix[12];

char* aruba_ip_to_str(ip_addr_t addr);
char* aruba_ip_to_str_hton(ip_addr_t addr);
char* aruba_ip_to_str_r(ip_addr_t addr, char* buffer, int len);
char* aruba_ipv6_to_str(__u8* ip);
char* aruba_sin_to_str(struct sockaddr_in6 *sin);


#define IPAF(x) (x).af
#define IPV4(x) (x).addr.addr4
#define IPV6(x) (x).addr.addr6
#define IPV6_16(x) (x).v6addr16
#define IPV6_SIZE	(sizeof(__u8) * 16)
#define IPV4_SIZE	sizeof(__u32)
#define SAMEIP(x,y) (IPAF(x) == IPAF(y) && 	\
					((IPAF(x) == AF_INET) ?	\
					 (IPV4(x) == IPV4(y)) :	\
					 !memcmp(IPV6(x), IPV6(y), sizeof(IPV6(x)))))
#define CLEARIP(x)									\
		do {										\
			memset(&x, 0, sizeof(ip_addr_t));		\
			IPAF(x) = AF_UNSPEC;					\
		} while(0)
#define IS_V4(x) ((IPAF(x) == AF_INET) ?1:0)
#define IS_V6(x) ((IPAF(x) == AF_INET6)?1:0)
#define IS_V6_ZERO(x)	(memcmp(IPV6(x), IN6ADDR_ZERO, IPV6_SIZE) ? 0 : 1)
#define IS_V6_LOOPBACK(x)	(memcmp(IPV6(x), IN6ADDR_LOOPBACK, IPV6_SIZE) ? 0 : 1)
#define IS_V4_ZERO(x)	(IPV4(x) == 0)
#define IS_V4_LOOPBACK(x)	(IPV4(x) == htonl(INADDR_LOOPBACK))
#define ZEROIP(x)	((IPAF(x) == AF_UNSPEC) || (IS_V6(x) ?	IS_V6_ZERO(x) : IS_V4_ZERO(x)))
#define IPV6_CMP(x,y)   memcmp(IPV6(x), IPV6(y), sizeof(IPV6(x)))
#define SETV4(x,y)									\
		do {										\
			CLEARIP(x);								\
			IPAF(x) = AF_INET;						\
			IPV4(x) = (y);							\
		} while(0)
#define SETV6(x,y)									\
		do {										\
			CLEARIP(x);								\
			IPAF(x) = AF_INET6;						\
			memcpy(IPV6(x), (y), IPV6_SIZE);		\
		} while(0)
#define SETV4_NTOHL(x,y)									\
		do {										\
			CLEARIP(x);								\
			IPAF(x) = AF_INET;						\
			IPV4(x) = ntohl(y);							\
		} while(0)
#define SETV4_HTONL(x,y)									\
		do {										\
			CLEARIP(x);								\
			IPAF(x) = AF_INET;						\
			IPV4(x) = htonl(y);							\
		} while(0)
#define PB_TO_IPADDR(x,y)							\
		do {										\
			if ((y)->af_type == AF_INET6) { 		\
				SETV6((x), (y)->ip_v6.data);		\
			} else {								\
				SETV4((x), (y)->ip);				\
			}										\
		} while(0)
#define IPADDR_TO_PB(x,y)							\
		do {										\
			(x)->af_type = IPAF(y);					\
			if ((x)->af_type == AF_INET6) { 		\
				(x)->ip_v6.len = IPV6_SIZE; 		\
				(x)->ip_v6.data = IPV6(y); 			\
				(x)->has_ip_v6 = TRUE;				\
			} else {								\
				(x)->ip = IPV4(y);					\
				(x)->has_ip = TRUE;					\
			}										\
		} while(0)

            
#define PRINTPAPISRCIP(hdr) (IN6_IS_ADDR_UNSPECIFIED(hdr->SrcIp6Addr)? \
                            ip_to_str(hdr->SrcIpAddr): (char *)aruba_ipv6_to_str(hdr->SrcIp6Addr)) 
#define PRINTPAPIDSTIP(hdr) (IN6_IS_ADDR_UNSPECIFIED(hdr->DestIp6Addr)? \
                            ip_to_str(hdr->DestIpAddr): (char *)aruba_ipv6_to_str(hdr->DestIp6Addr)) 
#define COPYIP(x,y) ((x) = (y))
#define IN6ADDR_LOOPBACK	ipv6_loopback
#define IN6ADDR_ZERO		ipv6_zeros
#define SETV6_LOOPBACK(x)	SETV6((x), IN6ADDR_LOOPBACK)

#define MAP_IP_4TO6(u32_nbo, ipv6)                                      \
    do {                                                                \
        memcpy((__u8*)ipv6, ipv6_v4MapPrefix, sizeof(ipv6_v4MapPrefix));\
        memcpy((__u8*)ipv6 + sizeof(ipv6_v4MapPrefix),                  \
               (__u8*)&u32_nbo, IPV6_SIZE - sizeof(ipv6_v4MapPrefix));  \
    } while(0)

//v6todo: must be a better way for IS_ALL_FF
#define IS_ALL_FF(x) 	((IPAF(x) == AF_INET) ?								\
						 (IPV4(x) == 0xFFFFFFFF) :							\
						 (*((__u32 *)IPV6(x)) == 0xFFFFFFFF && 				\
						  *((__u32 *)IPV6(x) + 1) == 0xFFFFFFFF && 			\
						  *((__u32 *)IPV6(x) + 2) == 0xFFFFFFFF && 			\
						  *((__u32 *)IPV6(x) + 3) == 0xFFFFFFFF))

#define SETV6_ALL_FF(x)      \
		do {										\
			CLEARIP(x);								\
			IPAF(x) = AF_INET6;						\
			*((__u32 *)IPV6(x))	= 0xFFFFFFFF;						\
			*((__u32 *)IPV6(x) + 1)	= 0xFFFFFFFF;					\
			*((__u32 *)IPV6(x) + 2)	= 0xFFFFFFFF;					\
			*((__u32 *)IPV6(x) + 3)	= 0xFFFFFFFF;					\
		} while(0)

#define COPY_V4V6_IP(dst, src)	\
{								\
	if (IS_V6(src)) {			\
    	SETV6(dst, src.v6addr);	\
	} else if(IS_V4(src)) {		\
    	SETV4(dst, src.v4addr);	\
	}			                \
}\

#define SAPD_COPY_V4V6_IP(dst, src)	\
{								\
	if (IS_V6(src)) {			\
    	SETV6(dst, src.v6addr);	\
	} else if(IS_V4(src)) {		\
    	SETV4(dst, src.v4addr);	\
	}							\
	else {\
		SAPD_DEBUG_LOG("%s(%d): address family not present ", __FUNCTION__,__LINE__);			  \
      	SAPD_DEBUG_TO_LOG_FILE("%s(%d): address family not present  ", __FUNCTION__,__LINE__); \
	}																			\
}
#define RAPPER_COPY_V4V6_IP(dst, src) \
{								\
	if (IS_V6(src)) {			\
    	SETV6(dst, src.v6addr);	\
	} else if(IS_V4(src)) {		\
    	SETV4(dst, src.v4addr);	\
	}							\
	 else {					\
	log_ap_mocana("%s(%d): address family not present ", __FUNCTION__, __LINE__); \
	}								\
}

int aruba_plen_to_netmask(__u8 plen, ip_addr_t *pip);
int aruba_netmask_to_plen(ip_addr_t mask);
char *aruba_inet_ntop(ip_addr_t addr, char *buffer);
int aruba_inet_pton(const char *src, ip_addr_t *dst);
#define aruba_str_to_ip		aruba_inet_pton
int ipv6_addr_mask_to_plen(ip_addr_t ip_mask6);
void fix_zero_and_all_1_ip_addr(__u16 af);

extern ip_addr_t zero_ip_addr, all_1_ip_addr;
#define ZERO_IP_ADDR_T zero_ip_addr
#define ALL_1_IP_ADDR_T all_1_ip_addr

extern void format_ipv6_with_colon(char *in, char *out);
extern int is_ipv6_link_local_address(ip_addr_t addr);
extern int is_ipv6_site_local_address(ip_addr_t addr);
int validate_and_assign_ip_addr(char *str, ip_addr_t *dst);
extern int check_valid_ipv6_addr_mask(ip_addr_t ipv6addr, __u8 pref_len);

#if defined (SWITCH_PLATFORM)
extern int str2PrefixIpv6(const unsigned char* str, prefixIpv6_t *ipv6Addr);
extern int prefixIpv6ToStr (prefixIpv6_t *ipv6Addr, char *str, int size);
#endif
#endif /* _IPv6_CMN_H_ */
