#include "../util/ipv6_cmn.h"

#ifndef _TUN_CRYPTO_H_
#define _TUN_CRYPTO_H_

// Values supported for encrAlgo
#define TUN_ENCRALGO_NULL       (0)
#define TUN_ENCRALGO_3DES       (1)
#define TUN_ENCRALGO_AES        (2)
#define IPSEC_ENCALG_BLOWFISH   (3)
#define IPSEC_ENCALG_AES        (4)
#define IPSEC_ENCALG_AES_CTR    (5)
#define IPSEC_ENCALG_AES_GCM    (6)
#define IPSEC_ENCALG_AES_GMAC   (7)


typedef struct ipseckeys_s {
#ifndef CONFIG_MERLOT
    unsigned char  inkey[64];  /* 24B for 3DES, 32B for AES */
#else
    unsigned char  inkey[32];  /* 24B for 3DES, 32B for AES */
#endif
    unsigned char  inhash[32]; /* 16 for MD5, 20 for SHA1 */
#ifndef CONFIG_MERLOT
    unsigned char  outkey[64];  /* 24B for 3DES, 32B for AES */
#else
    unsigned char  outkey[32];  /* 24B for 3DES, 32B for AES */
#endif
    unsigned char  outhash[32]; /* 16 for MD5, 20 for SHA1 */
    unsigned int   inspi, outspi;
    unsigned short srcNatt, dstNatt;
    ip_addr_t 		srcIP, dstIP;			
    ip_addr_t   inner_ip;				
    unsigned int   seq_snd, seq_rcv; /* IPSEC ESP sequence numbers */
    int            alg;  /* we do either 3des w/ HMAC-SHA1 or
                               AES-CBC-256/AES-CBC-128 w/ HMAC-SHA1 */
    int		   keylen;
    unsigned short natProto; /* in case we ever move to TCP */
    unsigned short isXauth;  /* Flag to distinguish between XAUTH(1) and L2TP(0) */
    unsigned int   greClear;
#ifndef CONFIG_MERLOT
    unsigned short keyLength;
    unsigned short implicitNonceLength;
#endif
} __attribute__ ((packed)) ipseckeys_t;

typedef struct l2tpinfo_s {
    unsigned short srcl2tp,     dstl2tp; /* port numbers */
    unsigned short l2tp_tunid,  l2tp_tunid_peer;
    unsigned short l2tp_sessid, l2tp_sessid_peer;
} __attribute__ ((packed)) l2tpinfo_t;    

#define SIOCDEVIPSECKEYS   (SIOCDEVPRIVATE+1)
#define SIOCDEVL2TP        (SIOCDEVPRIVATE+2)
#define SIOCDEVSET         (SIOCDEVPRIVATE+3)
#define SIOCDEVSINGLECRYPT (SIOCDEVPRIVATE+4)

#ifdef __KERNEL__
extern int tun_recv_esp(struct sk_buff *skb);
#endif

#endif /* _TUN_CRYPTO_H_ */

