#include "../dnsmasq-2.55/dnsmasq.h"
#include <linux/if_packet.h>
#ifndef __OEM_siemens__
#include "cmn/util/sos_msg.h"
#include "cmn/wifi/atheros_sysctl.h"
#endif

#define RAW_DEV_NAME BRIDGE_DEVNAME
int retry_send(void)
{
   struct timespec waiter;
   if (errno == EAGAIN)
     {
       waiter.tv_sec = 0;
       waiter.tv_nsec = 10000;
       nanosleep(&waiter, NULL);
       return 1;
     }
   
   if (errno == EINTR)
     return 1;

   return 0;
}

static u16 ip_cksum(u16 *addr, u32 count)
{
    register u32 sum = 0;
  
    while (count-- > 0) {
        //UTRACE(TRDEF_FramXsum, 0, addr, *addr, count, sum);
        sum += *addr++;
    }
    //UTRACE(TRDEF_FramXsum, 0, 0, 0, 0, sum);
    sum  = (sum >> 16) + (sum & 0xFFFF);  /* This can only wrap twice */
    return ~((sum >> 16) + (sum & 0xFFFF));  /* (work it out) */
}


#define FRAME_HDR_LEN  8
static u16 udp_cksum(struct ip *pip, int len, char *data, __u32 src, __u32 dst)
{
    register __u32 sum = 0;
    register __u16 *addr;
    register __u32 count;

    if (ntohs(pip->ip_len) < sizeof(struct ip) + sizeof(struct udphdr)) {
        return 0;
    }

    // UDP payload
    addr = (u16 *) data;
    count = (len - sizeof(struct udphdr))/2;
    while (count-- > 0) {
        sum += *addr++;
    }
    if (len & 0x1) {
        sum += (*addr) & htons(0xff00);
    }

    // UDP header
    addr = (u16 *) (((char*) pip) + (pip->ip_hl * 4));
    count = (sizeof(struct udphdr))/2;
    while (count-- > 0) {
        sum += *addr++;
    }

    // IP source and dest
    sum += src >> 16;
    sum += src & 0xffff;
    sum += dst >> 16;
    sum += dst & 0xffff;

    // IP payload length and protocol
    sum += htons((ntohs(pip->ip_len) - (pip->ip_hl * 4))) + htons(IPPROTO_UDP);

    sum  = (sum >> 16) + (sum & 0xFFFF);  /* This can only wrap twice */
    return ~((sum >> 16) + (sum & 0xFFFF));  /* (work it out) */
}


/* Send a raw ethernet packet */
int raw_send(int fd, char *header, size_t hlen, 
             char *packet, size_t len)
{
  struct iovec iov[2];
  typedef struct {
      struct ether_header ethernet;
      struct ip ip __attribute__ ((__packed__));
  } frame_t;
  frame_t *frame;
  struct udphdr *udp;
  printf("into raw_send\n");
  frame = (frame_t*) header;
  udp = (struct udphdr*) (header + (sizeof(struct ether_header) + 
      (frame->ip.ip_hl * 4)));
  udp->len = htons(sizeof(struct udphdr) + len);
  frame->ip.ip_len = htons((frame->ip.ip_hl * 4) + ntohs(udp->len));
  frame->ip.ip_sum = 0;
  frame->ip.ip_sum = ip_cksum((__u16*) &frame->ip, (frame->ip.ip_hl * 4) / 2);
  udp->check = 0;
  udp->check = udp_cksum(&frame->ip, ntohs(udp->len), packet, 
          (__u32) frame->ip.ip_src.s_addr, (__u32) frame->ip.ip_dst.s_addr);
#if 0
  {
    unsigned  int i;
    printf("Sending raw frame: ");
    for (i = 0; i < hlen; i++) {
      printf("%x ", (unsigned char) header[i]);
    }
    for (i = 0; i < len; i++) {
      printf("%x ", (unsigned char) packet[i]);
    }
    printf("\n");
  }
#endif

  iov[0].iov_base = header;
  iov[0].iov_len = hlen;
  iov[1].iov_base = packet;
  iov[1].iov_len = len;

  while (writev(fd, iov, 2) == -1 && retry_send());
  printf("raw send success\n");
  return 0;
}
          
#ifdef FORWARD_RAW

extern char raw_mac_addr[ETH_ALEN];
extern int raw_sock;

int
raw_get_hwaddr()
{
    int s;
    struct ifreq ifr;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("socket failed: %s", strerror(errno));
        return 0;
    }

    strcpy(ifr.ifr_name, RAW_DEV_NAME);
    if (ioctl(s, SIOCGIFHWADDR, &ifr) >= 0) {
        memcpy(raw_mac_addr, ifr.ifr_hwaddr.sa_data, 6);
    }

    close(s);
    return 1;
}
#if 1
int
raw_socket(unsigned short Opcode)
{
    unsigned short protocol;
    struct sockaddr_ll my_ll;
    struct ifreq ifr;
    int rc;

    if((Opcode <= SOS_MSG_OPCODE_FREE) ||  
       (Opcode >= SOS_MSG_OPCODES)) {
        goto error;
    }

    protocol = SOS_MSG_PROTO(Opcode);
    protocol = htons(protocol);

    raw_sock = socket(PF_PACKET, SOCK_RAW, protocol); 
    if (raw_sock < 0) {
        goto error;
    }

    /* get ethernet index */
    memset(&ifr, 0, sizeof (ifr));
    snprintf(ifr.ifr_name, sizeof (ifr.ifr_name), "%s", RAW_DEV_NAME);
    rc = ioctl(raw_sock, SIOCGIFINDEX, &ifr);
    if (rc < 0) {
        goto error;
    }

    memset((char*)&my_ll,0,sizeof(my_ll));
    my_ll.sll_family   = AF_PACKET;
    my_ll.sll_protocol = protocol;
    my_ll.sll_ifindex  = ifr.ifr_ifindex;
    my_ll.sll_hatype   = ARPHRD_ETHER;
    my_ll.sll_pkttype  = PACKET_HOST;
    my_ll.sll_halen    = ETH_ALEN;

    if (bind(raw_sock,(struct sockaddr*)&my_ll,sizeof(my_ll))) {
        goto error;
    }

    return raw_sock;

error:
    if (raw_sock > 0) {
        close(raw_sock);
        raw_sock = 0;
    }
    return -1;
}
#endif 

#if 0
static __u32
raw_ppp_src_addr (void)
{
    int                  s;
    struct ifreq         ifr;
    int                  retval;
    struct sockaddr_in * saddr;

    s = socket (PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (s < 0)
        return (0);

    memset  (&ifr, 0, sizeof(ifr));
    strncpy (ifr.ifr_name, "ppp0", sizeof (ifr.ifr_name));
    retval = ioctl (s, SIOCGIFADDR, &ifr);
    if (retval < 0) {
        syslog(LOG_ERR, "%s: ioctl ppp0 failed", __FUNCTION__);
        close(s);
        return (0);
    }

    close (s);
    saddr = (struct sockaddr_in *) &ifr.ifr_addr;

    return (saddr->sin_addr.s_addr);
}

struct in_addr
raw_src_addr(void)
{
    FILE *uplink;
    char s[10];
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, RAW_DEV_NAME);
    ifr.ifr_addr.sa_family = AF_INET;

    uplink = fopen("/tmp/uplink", "r");
    if(uplink) {
        fgets(s, 10, uplink);    
        if(s[0] == 'p') {
            __u32 ppp_ip;
            ppp_ip = raw_ppp_src_addr();
            if (ppp_ip == 0) {
                syslog(LOG_ERR, "%s: ioctl %s failed", __FUNCTION__, RAW_DEV_NAME);
            }
            ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr = ppp_ip;
        } else {
            ioctl(raw_sock, SIOCGIFADDR, &ifr);
        }
        fclose(uplink);
    } else {
        ioctl(raw_sock, SIOCGIFADDR, &ifr);
    }
    return ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr;
}
#endif

int
raw_udp_send(char *header, int plen, 
             struct in_addr src, struct in_addr dst,
             u_int16_t sport, u_int16_t dport,
             char *dmac)
{
    typedef struct {
        struct ether_header ethernet;
        struct ip ip __attribute__ ((__packed__));
        struct udphdr udp __attribute__ ((__packed__));
    } frame_t;
    frame_t frame;
    unsigned char tmp_dmac[6];
    int i;
    printf("into raw_udp_send\n");
    for(i = 0; i < 6; i++)
    {
       tmp_dmac[i] = dmac[i];
    }
    memset(&frame, 0, sizeof(frame));
    memcpy(frame.ethernet.ether_dhost, (unsigned char *)tmp_dmac, ETH_ALEN);
    memcpy(frame.ethernet.ether_shost, raw_mac_addr, ETH_ALEN);
    frame.ethernet.ether_type = htons(ETHERTYPE_IP);
    frame.ip.ip_hl = sizeof(struct ip) / 4;
    frame.ip.ip_v = IPVERSION;
    frame.ip.ip_p = IPPROTO_UDP;
    frame.ip.ip_ttl = IPDEFTTL;
    frame.ip.ip_src = src;
    frame.ip.ip_dst = dst;
    frame.udp.source = sport, 
    frame.udp.dest = dport;
    printf("dmac is %x:%x:%x:%x:%x:%x\n", dmac[0], dmac[1], tmp_dmac[2],tmp_dmac[3], tmp_dmac[4], tmp_dmac[5]);
    printf("raw_sock is %d\n", raw_sock);
    printf("port is %d and %d\n", sport, dport);
    printf("@@in raw udp send\n");   
    return raw_send(raw_sock, (char*) &frame, sizeof(frame), header, plen);
}

#endif /* FORWARD_RAW */
