#ifndef _ADP_H_
#define _ADP_H_

/* defines for Aruba Discovery Protocol */

#define ADP_IP_ADDR_STR    "239.0.82.11"
#define ADP_IP_ADDR         0xef00520b
#define ADP_IP_PROTO        17 /* UDP */
#define ADP_UDP_PORT        8200

#define ADP_VERSION_NUM     1

#define ADP_TYPE_QUERY      1
#define ADP_TYPE_RESPONSE   2

typedef struct adp_hdr {
  unsigned short version;   /* ADP_VERSION_* */
  unsigned short type;      /* ADP_TYPE_* */
  unsigned short id;        /* transaction id */
  unsigned char data[0];
} adp_hdr_t;

#endif /* _ADP_H_ */
