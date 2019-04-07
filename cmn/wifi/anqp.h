#ifndef _ANQP_H_
#define _ANQP_H_

#include <asm/types.h>
#include <linux/version.h>
#ifdef __KERNEL__
#include <linux/if_ether.h>
#endif

#ifndef __KERNEL__
#define BIT(x) (1 << (x))
#endif

#ifndef __KERNEL__
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define le_to_host16(n) (n)
#define host_to_le16(n) (n)
#define le_to_host32(n) (n)
#define host_to_le32(n) (n)
#else
#include <byteswap.h>
#define le_to_host16(n) bswap_16(n)
#define host_to_le16(n) bswap_16(n)
#define le_to_host32(n) bswap_32(n)
#define host_to_le32(n) bswap_32(n)
#endif
#endif

typedef enum _anqp_nwk_auth_type {
    ANQP_NWK_AUTH_ACCEPTANCE_TERMS_CONDITIONS   = 0,
    ANQP_NWK_AUTH_ONLINE_ENROLLMENT             = 1,
    ANQP_NWK_AUTH_HTTP_S_REDIRECTION            = 2,
    ANQP_NWK_AUTH_DNS_REDIRECTION               = 3
} anqp_nwk_auth_type;

typedef enum _anqp_roam_cons_oi_len {
    ANQP_ROAM_CONS_OI_LEN_0                    = 0,
    ANQP_ROAM_CONS_OI_LEN_3                    = 3,
    ANQP_ROAM_CONS_OI_LEN_5                    = 5,
} anqp_roam_cons_oi_len;

typedef enum _anqp_auth_param_type {
    ANQP_AUTH_PARAM_RESERVED = 0,
    ANQP_AUTH_PARAM_EXPANDED_EAP,
    ANQP_AUTH_PARAM_NON_EAP_INNER_AUTH,
    ANQP_AUTH_PARAM_EAP_INNER_AUTH,
    ANQP_AUTH_PARAM_EXPANDED_INNER_EAP,
    ANQP_AUTH_PARAM_CREDENTIAL_TYPE,
    ANQP_AUTH_PARAM_TUNNELED_EAP_CREDENTIAL_TYPE,
} anqp_auth_param_type;

typedef enum _anqp_eap_method {
    ANQP_EAP_METHOD_IDENTITY                = 1,
    ANQP_EAP_METHOD_NOTIFICATION,
    ANQP_EAP_METHOD_ONE_TIME_PASSWORD       = 3,
    ANQP_EAP_METHOD_GENERIC_TOKEN_CARD      = 4,
    ANQP_EAP_METHOD_EAP_TLS                 = 13,
    ANQP_EAP_METHOD_EAP_SIM                 = 18,
    ANQP_EAP_METHOD_EAP_TTLS                = 21,
    ANQP_EAP_METHOD_EAP_AKA                 = 23,
    ANQP_EAP_METHOD_PEAP                    = 25,
    ANQP_EAP_METHOD_CRYPTOCARD              = 28,
    ANQP_EAP_METHOD_PEAP_MSCHAPV2           = 29,
    ANQP_EAP_METHOD_NONE                    = 255,
} anqp_eap_method;

#define ANQP_AUTH_VALUE_OFFSET          40

typedef enum _anqp_auth_non_eap_param {
#define ANQP_AUTH_NON_EAP_PARAM_OFFSET  100
    ANQP_AUTH_NON_EAP_RSVD      = (ANQP_AUTH_VALUE_OFFSET *ANQP_AUTH_PARAM_NON_EAP_INNER_AUTH),
    ANQP_AUTH_NON_EAP_PAP,
    ANQP_AUTH_NON_EAP_CHAP,
    ANQP_AUTH_NON_EAP_MSCHAP,
    ANQP_AUTH_NON_EAP_MSCHAPV2,
} anqp_auth_non_eap_param;

typedef enum _anqp_auth_cred_param {
#define ANQP_AUTH_CRED_PARAM_OFFSET     200
    ANQP_AUTH_CRED_NA = (ANQP_AUTH_VALUE_OFFSET * ANQP_AUTH_PARAM_CREDENTIAL_TYPE),
    ANQP_AUTH_CRED_SIM,
    ANQP_AUTH_CRED_USIM,
    ANQP_AUTH_CRED_NFC,
    ANQP_AUTH_CRED_HW_TOKEN,
    ANQP_AUTH_CRED_SOFTOKEN,
    ANQP_AUTH_CRED_CERT,
    ANQP_AUTH_CRED_USERPASS,
    ANQP_AUTH_CRED_NONE,
    ANQP_AUTH_CRED_RSVD,
    ANQP_AUTH_CRED_VENSPEC,
} anqp_auth_cred_param;

typedef enum _anqp_auth_tun_cred_param {
#define ANQP_AUTH_TUN_CRED_PARAM_OFFSET     300
    ANQP_AUTH_TUN_CRED_NA = (ANQP_AUTH_VALUE_OFFSET * ANQP_AUTH_PARAM_TUNNELED_EAP_CREDENTIAL_TYPE),
    ANQP_AUTH_TUN_CRED_SIM,
    ANQP_AUTH_TUN_CRED_USIM,
    ANQP_AUTH_TUN_CRED_NFC,
    ANQP_AUTH_TUN_CRED_HW_TOKEN,
    ANQP_AUTH_TUN_CRED_SOFTOKEN,
    ANQP_AUTH_TUN_CRED_CERT,
    ANQP_AUTH_TUN_CRED_USERPASS,
    ANQP_AUTH_TUN_CRED_RSVD,
    ANQP_AUTH_TUN_CRED_ANONYMOUS,
    ANQP_AUTH_TUN_CRED_VENSPEC,
} anqp_auth_tun_cred_param;



#define ANQP_ADDR_AVAIL_IPV4_OFFSET     2
typedef enum _anqp_ipv6_addr_field {
    ANQP_IPV6_ADDR_NA  = 0,
    ANQP_IPV6_ADDR_AVAILABLE,
    ANQP_IPV6_ADDR_AVAILABILITY_UNKNOWN,
    ANQP_IPV6_ADDR_RESERVED,
} anqp_ipv6_addr_field;

typedef enum _anqp_ipv4_addr_field {
    ANQP_IPV4_ADDR_TYPE_NA  = 0,
    ANQP_IPV4_ADDR_PUBLIC,
    ANQP_IPV4_ADDR_PORT_RESTRICTED,
    ANQP_IPV4_ADDR_SINGLE_NATED_PRIVATE,
    ANQP_IPV4_ADDR_DOUBLE_NATED_PRIVATE,
    ANQP_IPV4_ADDR_PORT_RESTRICTED_SINGLE_NATED,
    ANQP_IPV4_ADDR_PORT_RESTRICTED_DOUBLE_NATED,
    ANQP_IPV4_ADDR_AVAILABILITY_UNKNOWN,
    ANQP_IPV4_ADDR_RESERVED,
} anqp_ipv4_addr_field;

typedef enum _hotspot_cfg_OSU_method_number {
    HS_OSU_METHOD_OMA_DM     = 0,
    HS_OSU_METHOD_SOAP_XML     = 1,
    HS_OSU_METHOD_RSVD    = 2,
} hotspot_cfg_OSU_method_number;

#endif

