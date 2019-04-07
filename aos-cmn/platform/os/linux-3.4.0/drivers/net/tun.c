/*
 *  TUN - Universal TUN/TAP device driver.
 *  Copyright (C) 1999-2002 Maxim Krasnyansky <maxk@qualcomm.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  $Id: tun.c,v 1.15 2002/03/01 02:44:24 maxk Exp $
 */

/*
 *  Changes:
 *
 *  Mike Kershaw <dragorn@kismetwireless.net> 2005/08/14
 *    Add TUNSETLINK ioctl to set the link encapsulation
 *
 *  Mark Smith <markzzzsmith@yahoo.com.au>
 *    Use random_ether_addr() for tap MAC address.
 *
 *  Harald Roelle <harald.roelle@ifi.lmu.de>  2004/04/20
 *    Fixes in packet dropping, queue length setting and queue wakeup.
 *    Increased default tx queue length.
 *    Added ethtool API.
 *    Minor cleanups
 *
 *  Daniel Podlejski <underley@underley.eu.org>
 *    Modifications for 2.3.99-pre5 kernel.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#define DRV_NAME	"tun"
#define DRV_VERSION	"1.6"
#define DRV_DESCRIPTION	"Universal TUN/TAP device driver"
#define DRV_COPYRIGHT	"(C) 1999-2004 Max Krasnyansky <maxk@qualcomm.com>"

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/miscdevice.h>
#include <linux/ethtool.h>
#include <linux/rtnetlink.h>
#include <linux/compat.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_tun.h>
#include <linux/crc32.h>
#include <linux/nsproxy.h>
#include <linux/virtio_net.h>
#include <linux/rcupdate.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
#include <net/rtnetlink.h>
#include <net/sock.h>
#include <net/ip6_route.h>

#include <asm/uaccess.h>
#ifdef CONFIG_ARUBA_AP
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <net/route.h>
#include <linux/in_route.h>
#include <net/dst.h>
#include <linux/crypto.h>
#include <net/xfrm.h>
#include <net/ip.h>

#include "../../../../../cmn/vpn/tun_crypto.h"
#include "../../../../../cmn/wifi/tunnel_id.h"
#include "../../../../../cmn/wifi/asap_cmn.h"

#ifdef DBG_RTNL_LOCK
#define MYLOCKDEV printk("%s:%s:%d Locking rtnl_lock\n", __FILE__, __FUNCTION__, __LINE__);
#define MYUNLOCKDEV printk("%s:%s:%d Unlocking rtnl_lock\n", __FILE__, __FUNCTION__, __LINE__);
#else
#define MYLOCKDEV
#define MYUNLOCKDEV
#endif

#define USE_BRIDGE 1
#ifdef USE_BRIDGE
#include "../../../../../cmn/wifi/atheros_sysctl.h"
#define OUTPUTDEV BRIDGE_DEVNAME
#else
#define OUTPUTDEV "eth0"
#endif
#ifdef __FAT_AP__
int (*ip_asap_firewall_input)(struct sk_buff *skb);
EXPORT_SYMBOL(ip_asap_firewall_input);
#endif
typedef struct cryptotun_s {
    char               dev_name[256];
    struct net_device *output_dev;
    unsigned int       ipid; /* IP ID */
    struct xfrm_state *out_xfrm;
    struct xfrm_state *in_xfrm;
    ipseckeys_t        k;
    l2tpinfo_t         l;
    unsigned int       allowclear;
} cryptotun_t;    

typedef struct esp_hdr_s {
    unsigned int spi;
    unsigned int seq;
} __attribute__ ((packed)) esp_hdr_t;

typedef struct l2tpppp_hdr_s {
    u16 type;
    u16 length;
    u16 tunid;
    u16 sessid;
    u16 pppdata;
} __attribute__ ((packed)) l2tpppp_hdr_t  ;

typedef struct l2tp_hdr_s {
    struct udphdr ludp;
    l2tpppp_hdr_t l2tp;
} __attribute__ ((packed)) l2tp_hdr_t;

#endif

#if defined(CONFIG_ARUBA_AP) && !defined(CONFIG_GRENACHE) && !defined(CONFIG_ARUBA_CONTROLLER)
int mirror_port = 0;
int mirror_ipsec = 0;
struct net_device *mirror_dev = NULL;

void asap_ipsec_mirror_packet_to_dev(struct sk_buff *skb, int xmit_pkt);
void asap_port_mirror_packet_to_dev(struct sk_buff *skb, int xmit_pkt, int is_eth);

EXPORT_SYMBOL(mirror_port);
EXPORT_SYMBOL(mirror_ipsec);
EXPORT_SYMBOL(mirror_dev);
EXPORT_SYMBOL(asap_port_mirror_packet_to_dev);
#endif

/* Uncomment to enable debugging */
/* #define TUN_DEBUG 1 */

#ifdef TUN_DEBUG
static int debug;

#define tun_debug(level, tun, fmt, args...)			\
do {								\
	if (tun->debug)						\
		netdev_printk(level, tun->dev, fmt, ##args);	\
} while (0)
#define DBG1(level, fmt, args...)				\
do {								\
	if (debug == 2)						\
		printk(level fmt, ##args);			\
} while (0)
#else
#define tun_debug(level, tun, fmt, args...)			\
do {								\
	if (0)							\
		netdev_printk(level, tun->dev, fmt, ##args);	\
} while (0)
#define DBG1(level, fmt, args...)				\
do {								\
	if (0)							\
		printk(level fmt, ##args);			\
} while (0)
#endif

#define FLT_EXACT_COUNT 8
struct tap_filter {
	unsigned int    count;    /* Number of addrs. Zero means disabled */
	u32             mask[2];  /* Mask of the hashed addrs */
	unsigned char	addr[FLT_EXACT_COUNT][ETH_ALEN];
};

struct tun_file {
	atomic_t count;
	struct tun_struct *tun;
	struct net *net;
};

struct tun_sock;
#ifdef CONFIG_ARUBA_AP
static LIST_HEAD(tun_dev_list);
static spinlock_t tun_list_lock = __SPIN_LOCK_UNLOCKED(tun_list_lock);
#endif

struct tun_struct {
#ifdef CONFIG_ARUBA_AP
	struct list_head        list;
#endif
	struct tun_file		*tfile;
	unsigned int 		flags;
	uid_t			owner;
	gid_t			group;
#ifdef CONFIG_ARUBA_AP
    unsigned long           if_flags;
#endif
	struct net_device	*dev;
	netdev_features_t	set_features;
#define TUN_USER_FEATURES (NETIF_F_HW_CSUM|NETIF_F_TSO_ECN|NETIF_F_TSO| \
			  NETIF_F_TSO6|NETIF_F_UFO)
	struct fasync_struct	*fasync;

	struct tap_filter       txflt;
	struct socket		socket;
	struct socket_wq	wq;

	int			vnet_hdr_sz;

#ifdef TUN_DEBUG
	int debug;
#endif
	spinlock_t lock;
};

struct tun_sock {
	struct sock		sk;
	struct tun_struct	*tun;
};
#ifdef CONFIG_ARUBA_AP
int ip_dst_output(struct sk_buff *skb);
int ip6_dst_output(struct sk_buff *skb);
#endif
static inline struct tun_sock *tun_sk(struct sock *sk)
{
	return container_of(sk, struct tun_sock, sk);
}

static int tun_attach(struct tun_struct *tun, struct file *file)
{
	struct tun_file *tfile = file->private_data;
	int err;

	ASSERT_RTNL();

	netif_tx_lock_bh(tun->dev);

	err = -EINVAL;
	if (tfile->tun)
		goto out;

	err = -EBUSY;
	if (tun->tfile)
		goto out;

	err = 0;
	tfile->tun = tun;
	tun->tfile = tfile;
	tun->socket.file = file;
	netif_carrier_on(tun->dev);
	dev_hold(tun->dev);
#ifdef CONFIG_ARUBA_AP
    this_cpu_inc(*tun->dev->tun_refcnt);
#endif
	sock_hold(tun->socket.sk);
	atomic_inc(&tfile->count);

out:
	netif_tx_unlock_bh(tun->dev);
	return err;
}


#ifdef CONFIG_ARUBA_AP
/*
 * in_cksum --
 *	Checksum routine for Internet Protocol family headers (C Version)
 */
u16 in_cksum(u16 *addr, int len)
{
	register int nleft = len;
	register u16 *w = addr;
	register int sum = 0;
	u16 answer = 0;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(u8 *) (&answer) = *(u8 *) w;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
	sum += (sum >> 16); /* add carry */
	answer = ~sum; /* truncate to 16 bits */
	return (answer);
}

#endif
static void __tun_detach(struct tun_struct *tun)
{
#ifdef CONFIG_ARUBA_AP
	cryptotun_t *ipsec;

	spin_lock_bh(&tun_list_lock);
	list_del(&tun->list);
	spin_unlock_bh(&tun_list_lock);
#endif
	/* Detach from net device */
	netif_tx_lock_bh(tun->dev);
	netif_carrier_off(tun->dev);
	tun->tfile = NULL;
#ifdef CONFIG_ARUBA_AP

	ipsec = (cryptotun_t *)(tun+1);

    if (ipsec->out_xfrm) {
        ipsec->out_xfrm->km.state = XFRM_STATE_DEAD;
        xfrm_state_put(ipsec->out_xfrm);
        ipsec->out_xfrm = NULL;
    }
    if (ipsec->in_xfrm) {
        ipsec->in_xfrm->km.state = XFRM_STATE_DEAD;
        xfrm_state_put(ipsec->in_xfrm);
        ipsec->in_xfrm = NULL;
    }
    if (ipsec->output_dev) {
        dev_put(ipsec->output_dev);
        ipsec->output_dev = NULL;
    }
#endif
	netif_tx_unlock_bh(tun->dev);

	/* Drop read queue */
	skb_queue_purge(&tun->socket.sk->sk_receive_queue);

	/* Drop the extra count on the net device */
	dev_put(tun->dev);
#ifdef CONFIG_ARUBA_AP
    this_cpu_dec(*tun->dev->tun_refcnt);
#endif
}

static void tun_detach(struct tun_struct *tun)
{
	rtnl_lock();
	__tun_detach(tun);
	rtnl_unlock();
}

static struct tun_struct *__tun_get(struct tun_file *tfile)
{
	struct tun_struct *tun = NULL;

	if (atomic_inc_not_zero(&tfile->count))
		tun = tfile->tun;

	return tun;
}

static struct tun_struct *tun_get(struct file *file)
{
	return __tun_get(file->private_data);
}

static void tun_put(struct tun_struct *tun)
{
	struct tun_file *tfile = tun->tfile;

	if (atomic_dec_and_test(&tfile->count))
		tun_detach(tfile->tun);
}

/* TAP filtering */
static void addr_hash_set(u32 *mask, const u8 *addr)
{
	int n = ether_crc(ETH_ALEN, addr) >> 26;
	mask[n >> 5] |= (1 << (n & 31));
}

static unsigned int addr_hash_test(const u32 *mask, const u8 *addr)
{
	int n = ether_crc(ETH_ALEN, addr) >> 26;
	return mask[n >> 5] & (1 << (n & 31));
}

static int update_filter(struct tap_filter *filter, void __user *arg)
{
	struct { u8 u[ETH_ALEN]; } *addr;
	struct tun_filter uf;
	int err, alen, n, nexact;

	if (copy_from_user(&uf, arg, sizeof(uf)))
		return -EFAULT;

	if (!uf.count) {
		/* Disabled */
		filter->count = 0;
		return 0;
	}

	alen = ETH_ALEN * uf.count;
	addr = kmalloc(alen, GFP_KERNEL);
	if (!addr)
		return -ENOMEM;

	if (copy_from_user(addr, arg + sizeof(uf), alen)) {
		err = -EFAULT;
		goto done;
	}

	/* The filter is updated without holding any locks. Which is
	 * perfectly safe. We disable it first and in the worst
	 * case we'll accept a few undesired packets. */
	filter->count = 0;
	wmb();

	/* Use first set of addresses as an exact filter */
	for (n = 0; n < uf.count && n < FLT_EXACT_COUNT; n++)
		memcpy(filter->addr[n], addr[n].u, ETH_ALEN);

	nexact = n;

	/* Remaining multicast addresses are hashed,
	 * unicast will leave the filter disabled. */
	memset(filter->mask, 0, sizeof(filter->mask));
	for (; n < uf.count; n++) {
		if (!is_multicast_ether_addr(addr[n].u)) {
			err = 0; /* no filter */
			goto done;
		}
		addr_hash_set(filter->mask, addr[n].u);
	}

	/* For ALLMULTI just set the mask to all ones.
	 * This overrides the mask populated above. */
	if ((uf.flags & TUN_FLT_ALLMULTI))
		memset(filter->mask, ~0, sizeof(filter->mask));

	/* Now enable the filter */
	wmb();
	filter->count = nexact;

	/* Return the number of exact filters */
	err = nexact;

done:
	kfree(addr);
	return err;
}

/* Returns: 0 - drop, !=0 - accept */
static int run_filter(struct tap_filter *filter, const struct sk_buff *skb)
{
	/* Cannot use eth_hdr(skb) here because skb_mac_hdr() is incorrect
	 * at this point. */
	struct ethhdr *eh = (struct ethhdr *) skb->data;
	int i;

	/* Exact match */
	for (i = 0; i < filter->count; i++)
		if (!compare_ether_addr(eh->h_dest, filter->addr[i]))
			return 1;

	/* Inexact match (multicast only) */
	if (is_multicast_ether_addr(eh->h_dest))
		return addr_hash_test(filter->mask, eh->h_dest);

	return 0;
}

/*
 * Checks whether the packet is accepted or not.
 * Returns: 0 - drop, !=0 - accept
 */
static int check_filter(struct tap_filter *filter, const struct sk_buff *skb)
{
	if (!filter->count)
		return 1;

	return run_filter(filter, skb);
}

/* Network device part of the driver */

static const struct ethtool_ops tun_ethtool_ops;

/* Net device detach from fd. */
static void tun_net_uninit(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);
	struct tun_file *tfile = tun->tfile;

	/* Inform the methods they need to stop using the dev.
	 */
	if (tfile) {
		wake_up_all(&tun->wq.wait);
		if (atomic_dec_and_test(&tfile->count))
			__tun_detach(tun);
	}
}

static void tun_free_netdev(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);

	BUG_ON(!test_bit(SOCK_EXTERNALLY_ALLOCATED, &tun->socket.flags));

	sk_release_kernel(tun->socket.sk);
}

/* Net device open. */
static int tun_net_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

/* Net device close. */
static int tun_net_close(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

#ifdef CONFIG_ARUBA_AP
/* Deprecated: use ip_route_output_key directly */
static int myroute(struct rtable **rp,
                   u32 daddr, u32 saddr, u32 tos, int oif)
{
    struct flowi fl;
    int err = 0;
    memset(&fl, 0, sizeof(fl));
    fl.u.ip4.__fl_common.flowic_oif = oif;
    fl.u.ip4.daddr = daddr;
    fl.u.ip4.saddr = saddr;
    fl.u.ip4.__fl_common.flowic_tos = tos;
    
    *rp = ip_route_output_key(&init_net, &fl.u.ip4);
    if (IS_ERR(*rp)) {
		err = PTR_ERR(*rp);
        *rp = NULL;
	}
    return err;
}


static int mapaln[4] = {2,1,0,3};
static int fourbytemapaln[4] = {0,3,2,1};

extern void copy_skb_header(struct sk_buff *new, const struct sk_buff *old);

/*
 * myskb_copy - copies skb and returns new one with hdroom AND with
 *              newskb->data aligned 2B after 4B alignment.  This is so
 *              after adding PPP 2B header, the frame can go through
 *              HMAC-SHA1 processing of linux kernel at optimal rate.  Without
 *              this change, encryption code is 1/2 has slow.
 *              If "ppp" is not set, then do NOT align 2B after 4B alignment.
 */
static struct sk_buff *
myskb_copy(struct sk_buff *skb, int hdroom, int ppp)
{
    struct sk_buff *n;
    int             hdlen = skb->data - skb->head;
    int             alnoff, nhdrlen;

    if (hdroom > hdlen)
        nhdrlen = hdroom;
    else
        nhdrlen = hdlen;

    n = alloc_skb(skb->end - skb->head + skb->data_len + (nhdrlen-hdlen) + 32,
                  GFP_ATOMIC);
    if (!n)
        return NULL;

    /* this piece copied from skb_copy */
    skb_reserve(n, nhdrlen);
    if (ppp) {
      /* force to be 2B off 4B alignment */
      alnoff = mapaln[((u32)(long)(n->data))%4];
    } else {
      /* force to be 4B aligned */
      alnoff = fourbytemapaln[((u32)(long)(n->data))%4];
    } 
    if (alnoff > 0)
        skb_reserve(n, alnoff);
    /* rest from skb_copy */
    skb_put(n, skb->len);
    n->csum = skb->csum;
    n->ip_summed = skb->ip_summed;
    if (skb_copy_bits(skb, -hdlen, n->data-hdlen, hdlen + skb->len))
        BUG();
    copy_skb_header(n, skb);

    if (ppp) {
      if ( (((u32)(long)(n->data))%4) != 2) {
          printk("%s:%d data not aligned properly at 0x%08x\n",
               __FUNCTION__, __LINE__, (u32)(long)(skb->data));
      }
    } else {
      if ( (((u32)(long)(n->data))%4) != 0) {
        printk("%s:%d data not aligned properly at 0x%08x\n",
               __FUNCTION__, __LINE__, (u32)(long)(skb->data));
      }
    }

    if (skb_headroom(n) < hdroom) {
        printk("%s:%d headroom not enough at %d, needs %d\n",
               __FUNCTION__, __LINE__,
               skb_headroom(n), hdroom);
    }
#if 0
    printk("%s:%d myskb_copy returning data=0x%08x hdroom=%d\n",
           __FUNCTION__, __LINE__, (u32)(n->data), skb_headroom(n));
#endif
    return n;
}

typedef struct {
    u16 flags;
    u16 type;
} ip_gre_t;

#define FRAG_OFFSET  0x1fff
#define FRAG_OFFSET  0x1fff
#define FRAG_OFFSET  0x1fff

/* assumes there's already check on GRE, note that there's no way we can know
   if a fragmented frame was tunneled or split if this AP is setup for tunneled
   with allowclear (no double encrypt) given that we don't track fragments,
   hence we must always double encrypt fragments. */
#define __IP_TO_TYPE(_iph_) (((ip_gre_t *)(((u8 *)(_iph_))+((_iph_)->ihl*4)))->type)
#define __TYPE_IS_SJ(_iph_) (__IP_TO_TYPE((_iph_)) >= TUNNEL_ID_SJ1 && __IP_TO_TYPE((_iph_)) <= TUNNEL_ID_SJ_MAX)
#define SJ_GRE(_iph_) ((((_iph_)->frag_off & FRAG_OFFSET)==0) && __TYPE_IS_SJ((_iph_)))

static void tun_net_xmit2(struct sk_buff *skb)
{
	struct iphdr *oip;
	oip = (struct iphdr *)skb->data;

	/* outer IP length is no longer adjusted in esp_output() */
        if (oip->version == 4) {
	    oip->tot_len = htons(skb->len);
	    ip_send_check(oip);
        } else {
            struct ipv6hdr *oip_ipv6 = (struct ipv6hdr *)skb->data;
            oip_ipv6->payload_len = htons(skb->len - sizeof(struct ipv6hdr));
        }
}

static int tun_net_xmit3(struct sk_buff *skb, struct tun_struct *tun)
{
	struct net_device *dev = tun->dev;
	cryptotun_t *ipsec = (cryptotun_t *)(tun+1);
	struct iphdr *iph;
	struct rtable *rt = NULL;
	__u32  saddr, daddr;
	int    sent = 0;

	if (ipsec->output_dev == NULL) {
		return -1;
	}
	skb->dev = ipsec->output_dev;
	
	/* flag to tell SOS not to loop in case someone
	   misconfigured the AP and user VLANs to be the same. */
	skb->flags |= TUNNEL_SKB_FLAG_IPSECENCAP;

	iph = (struct iphdr *)skb_network_header(skb);
        if (iph->version == 4) {
#if defined(__LINUX_ARM_ARCH__) && (__LINUX_ARM_ARCH__ == 5)
	{
	        ushort *s = (ushort *)&iph->saddr;
	        ushort *d = (ushort *)&saddr;

	        *d++ = *s++;
	        *d++ = *s++;
	        s = (ushort *)&iph->daddr;
	        d = (ushort *)&daddr;
	        *d++ = *s++;
	        *d++ = *s++;
	}
#else
	saddr = iph->saddr;
	daddr = iph->daddr;
#endif
	if (myroute(&rt, daddr, saddr,
		    RT_TOS(iph->tos), skb->dev->iflink) ||
	    (tun->dev == rt->dst.dev)) {
	    return -1;
	}
	dst_release(skb_dst(skb));
	skb_dst_set(skb, &rt->dst);
	//IPCB(skb)->flags |= IPSKB_REROUTED;
	if (skb->flags & TUNNEL_SKB_FLAG_80211) {
	    iph = (struct iphdr *)(skb->data);
	    if ((iph->protocol == IPPROTO_GRE) &&
		(iph->frag_off & htons(IP_DF))) {
		ip_dst_output(skb);
		dev->stats.tx_carrier_errors++;
		sent = 1;
	    }
	}
        } else {
            struct flowi6 fl6 = {};
            struct dst_entry *dst = NULL, *new_dst = NULL;
            static struct net_device *brdev = NULL;
            struct xfrm_state *x = ipsec->out_xfrm;

            ipv6_addr_copy(&fl6.daddr, (struct in6_addr *)&(x->id.daddr.a6));
            fl6.flowi6_proto = IPPROTO_IPV6;

            /* Following is required so that the route lookup doesn't pick-up tun0 interface,
               which will then lead to looping of packets.*/
            brdev = dev_get_by_name(&init_net, "br0");
            if (brdev){
                fl6.flowi6_oif = brdev->ifindex;
            }

            fl6.flowi6_flags = RT6_LOOKUP_F_SADDR_IGNORE;
            dst = (struct dst_entry *)ip6_route_output(&init_net, NULL, &fl6);
            if (tun->dev == dst->dev) {
                printk("tun0 dev is fetched!");
                return -1;
            }
            if (dst->error)
            {
                printk("dst->error\n");
                return -1;
            }
            new_dst = xfrm_lookup(dev_net(dst->dev), dst, flowi6_to_flowi(&fl6), NULL, 0);
            if (!new_dst) {
                printk("%s xfrm_lookup failed",__FUNCTION__);
                return -1;
            }
            if (IS_ERR(new_dst)) {
                printk("error in new_dst. old dst %p new dst %p",dst,new_dst);
                return -1;
            } 
            dst_release(skb_dst(skb));
            skb_dst_set(skb, new_dst);
            if (skb->flags & TUNNEL_SKB_FLAG_80211) {
                struct ipv6hdr *ip6h = (struct ipv6hdr *)(skb->data);
                if ((ip6h->nexthdr == IPPROTO_GRE) && !skb->local_df) {
                    ip6_dst_output(skb);
                    dev->stats.tx_carrier_errors++;
                    sent = 1;
                }
            }
        }
	if (sent == 0) {
	    dst_output(skb);
	    sent = 1;
	}
	return 0;
}
#endif
/* Net device start xmit */
static netdev_tx_t tun_net_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);
#ifdef CONFIG_ARUBA_AP
    int          error;
    cryptotun_t *ipsec = (cryptotun_t *)(tun+1);
    int          statslen = skb->len;
#endif
	tun_debug(KERN_INFO, tun, "tun_net_xmit %d\n", skb->len);

	/* Drop packet if interface is not attached */
	if (unlikely(!tun->tfile))
		goto drop;

	/* Drop if the filter does not like it.
	 * This is a noop if the filter is disabled.
	 * Filter can be enabled only for the TAP devices. */
	if (!check_filter(&tun->txflt, skb))
		goto drop;

	if (tun->socket.sk->sk_filter &&
	    sk_filter(tun->socket.sk, skb))
		goto drop;

#if 0
	if (unlikely(skb_queue_len(&tun->socket.sk->sk_receive_queue) >= dev->tx_queue_len)) {
		if (!(tun->flags & TUN_ONE_QUEUE)) {
			/* Normal queueing mode. */
			/* Packet scheduler handles dropping of further packets. */
			netif_stop_queue(dev);

			/* We won't see all dropped packets individually, so overrun
			 * error is more appropriate. */
			dev->stats.tx_fifo_errors++;
		} else {
			/* Single queue mode.
			 * Driver handles dropping of all packets itself. */
			goto drop;
		}
	}
#endif

	/* Orphan the skb - required as we might hang on to it
	 * for indefinite time. */
	skb_orphan(skb);
 #ifdef CONFIG_ARUBA_AP
	/* check if frame is already encap'd, if yes, we skip tun0 */
	if (likely(skb->len > (sizeof(struct iphdr)+sizeof(struct udphdr)))) {
	    struct iphdr *ip   = (struct iphdr *)(skb->data);
	    struct udphdr *udp = (struct udphdr *)(ip+1);		   
            if (ip->version == 6) {
                struct ipv6hdr *ipv6_hdr   = (struct ipv6hdr *)(skb->data);
                struct udphdr *udp_v6 = (struct udphdr *)(ipv6_hdr+1);
                if (unlikely(ipv6_hdr->nexthdr == IPPROTO_UDP &&
                             udp_v6->dest == ipsec->k.dstNatt &&
                             (ipsec->k.srcNatt == 0 ||
                             (udp_v6->source == ipsec->k.srcNatt &&
                             (memcmp(ipv6_hdr->saddr.s6_addr32,
                                     ipsec->k.srcIP.v6addr32, IPV6_SIZE) == 0) &&
                             (memcmp(ipv6_hdr->daddr.s6_addr32,
                                     ipsec->k.dstIP.v6addr32, IPV6_SIZE) == 0))))) {
                    goto tosend;
                }
            } else {
	    /* route out br0 */
	    if (likely(ip->protocol == ipsec->k.natProto &&
		       udp->dest == ipsec->k.dstNatt &&
		       (ipsec->k.srcNatt == 0 ||
			(udp->source == ipsec->k.srcNatt &&
			 ip->saddr == IPV4(ipsec->k.srcIP) &&
			 ip->daddr == IPV4(ipsec->k.dstIP))))) {
		
		goto tosend;
			
	    }
            }
	}

#if defined(CONFIG_ARUBA_AP) && !defined(CONFIG_GRENACHE) && !defined(CONFIG_ARUBA_CONTROLLER)
    asap_ipsec_mirror_packet_to_dev(skb, 1);
#endif

	if (likely(ipsec->out_xfrm)) {
	    l2tp_hdr_t     *l = 0;
	    struct iphdr   *iip, *oip;
            struct ipv6hdr *iip_ipv6, *oip_ipv6;
	    int             hdrlen, ip_esp_len;
	    struct xfrm_state *x = ipsec->out_xfrm;
	    int		alignoffset;
            int    is_v4 = 0;
	   
            if (x->props.family == AF_INET){
                is_v4 = 1;
            }
 
	    /* allow single encryption only if configured to do so AND
	       it's not secure jack or split */
	    if (unlikely((ipsec->allowclear) &&
			 (skb->flags & TUNNEL_SKB_FLAG_80211) &&
                         is_v4)) {
		struct udphdr *udp;
		u32            fakeSPI;
		
		if (ipsec->k.greClear)
		    goto tosend;

		iip = (struct iphdr *) skb_network_header(skb);
		/* no encryption, just encap and send */
		hdrlen = (sizeof(*oip)+sizeof(struct udphdr)+4/*spi*/)+16;
		if (skb_headroom(skb)<hdrlen || skb_cloned(skb) ||
		    skb_shared(skb)) {
		    /* Note: ok to realloc headroom as we're only adding to
		       hdr */
		    struct sk_buff *new_skb =
			skb_realloc_headroom(skb, hdrlen);
		    if (!new_skb) {
			dev->stats.tx_dropped++;
			dev_kfree_skb(skb);
			return 0;
		    }

		    dev_kfree_skb(skb);
		    skb = new_skb;
		}
		skb_push(skb, sizeof(*oip)+sizeof(struct udphdr)+4/*spi*/);
		skb_reset_network_header(skb);
		
		oip = (struct iphdr *)(skb->data);
		udp = (struct udphdr *)(oip+1);
		udp->source = ipsec->k.srcNatt;
		udp->dest   = ipsec->k.dstNatt;
		udp->len    = htons(ntohs(iip->tot_len) + 4/*spi*/+sizeof(*udp));
		udp->check  = 0;
		fakeSPI     = htonl((ntohl(ipsec->k.outspi) & 0xffffff00)|0x88);
		memcpy(udp+1, &fakeSPI, 4);
		
		/* outer IP header */
		oip->ihl      = 0x5;
		oip->version  = 0x4;
		oip->tos      = iip->tos;
		oip->tot_len  = htons(ntohs(udp->len) + sizeof(*oip));
		oip->id       = ipsec->ipid++;
#ifdef __FAT_AP__
		oip->frag_off = 0; /* frag ok */
#else
		oip->frag_off = htons(0x4000); /* don't frag */
#endif
		oip->ttl      = 64;
		oip->protocol = IPPROTO_UDP;

                /*srcIP and dstIP types are modified from u32 to ip_addr_t to support v6.
                  can't copy saddr and daddr in one shot.*/
#if 0
		/* copy src & dst IP in one shot */
#if defined(__LINUX_ARM_ARCH__) && (__LINUX_ARM_ARCH__ == 5)
		/* 
		 * The compiler optimizes memcmpy() to an 8-byte ldm/stm which
		 * causes 64-bit alignment exceptions.
		 * However, in many cases, doing these as 4-byte
		 * moves still doesn't work because the IP headers are not
		 * aligned, so do them as 2-byte (short) moves.
		 */
		{
			ushort *s = (ushort *)&ipsec->k.srcIP;
			ushort *d = (ushort *)&oip->saddr;

			*d++ = *s++;
			*d++ = *s++;
			*d++ = *s++;
			*d++ = *s++;
		}
#else
		memcpy(&oip->saddr, &ipsec->k.srcIP, 8);
#endif
#endif
                oip->saddr    = IPV4(ipsec->k.srcIP);
                oip->daddr    = IPV4(ipsec->k.dstIP);
		oip->check    = 0;
		oip->check    = in_cksum((u16 *)(oip), sizeof(struct iphdr));
		
		skb_set_transport_header(skb, sizeof(struct iphdr));
		goto tosend;
	    } else if (unlikely((ipsec->allowclear) &&
                    (skb->flags & TUNNEL_SKB_FLAG_80211) && !is_v4)) {

                u32 fakeSPI;
                if (ipsec->k.greClear)
                    goto tosend;

                iip_ipv6 = (struct ipv6hdr *) skb_network_header(skb);
                /* no encryption, just encap and send */
                hdrlen = (sizeof(*oip_ipv6) + 4/*spi*/) + 16;      
                if ((skb_headroom(skb) < hdrlen) || skb_cloned(skb) ||
                    skb_shared(skb)) {
                    /* Note: ok to realloc headroom as we're only adding to hdr */
                    struct sk_buff *new_skb = skb_realloc_headroom(skb, hdrlen);
                    if (!new_skb) {
                        dev->stats.tx_dropped++;
                        dev_kfree_skb(skb);
                        return 0;
                    }
                    dev_kfree_skb(skb);
                    skb = new_skb;
                }
                skb_push(skb, sizeof(*oip_ipv6) + 4/*spi*/);
                skb_reset_network_header(skb);

                oip_ipv6 = (struct ipv6hdr *)skb->data;
                oip_ipv6->version  = 0x6;
                oip_ipv6->payload_len = htons(skb->len - sizeof(*oip_ipv6));
                oip_ipv6->nexthdr = IPPROTO_ESP;
                oip_ipv6->hop_limit = 64;
                ipv6_addr_copy(&oip_ipv6->saddr, (struct in6_addr *)&IPV6(ipsec->k.srcIP));
                ipv6_addr_copy(&oip_ipv6->daddr, (struct in6_addr *)&IPV6(ipsec->k.dstIP));
                fakeSPI = htonl((ntohl(ipsec->k.outspi) & 0xffffff00)|0x88);
                memcpy(oip_ipv6+1, &fakeSPI, 4);

                skb_set_transport_header(skb, sizeof(struct ipv6hdr));
                goto tosend;
            }

	    /* encapsulation case (ESP) */
	    /* (the following comment is taken from net/xfrm.h)
	     * Add encapsulation header.
	     *
	     * On exit, the transport header will be set to the start of the
	     * encapsulation header to be filled in by x->type->output and
	     * the mac header will be set to the nextheader (protocol for
	     * IPv4) field of the extension header directly preceding the
	     * encapsulation header, or in its absence, that of the top IP
	     * header.  The value of the network header will always point
	     * to the top IP header while skb->data will point to the payload.
	     */

	    /* we allocate hdr room for both l2tp and encrypted
	       ESP here so esp4.c doesn't need to do it again */
	    if (ipsec->k.isXauth) {
                if (is_v4) {
                    ip_esp_len = x->props.header_len + sizeof(struct udphdr);
                    alignoffset = 0; // we don't need to do extra, since IPSEC tunnel-mode is aligned
                } else {
                    /*x->props.header_len already contain room for IPV6 header,ESP header and Crypt data*/
                    ip_esp_len = x->props.header_len;
                    alignoffset = 0; /* we don't need to do extra, since IPSEC tunnel-mode is aligned*/
                }
	    } else {
		ip_esp_len = sizeof(l2tp_hdr_t) + x->props.header_len;
		alignoffset = 2; // extra alignment needed due to PPP(2Byte hdr)
	    }
	    // add bytes for alignment
	    hdrlen = ip_esp_len+64;
	    //printk("ip_esp_len %d     hdrlen %d\n", ip_esp_len, hdrlen);
	    
	    if (skb_cloned(skb) || skb_shared(skb) ||
		skb_headroom(skb) < hdrlen ||
		( ((u32)(long)(skb->data)) % 4 ) != alignoffset) {
		/* cannot realloc headroom, need to copy entire SKB as the
		   destination will be overwritten on encrypt. */
		struct sk_buff *new_skb;
		if (ipsec->k.isXauth)
		    new_skb = myskb_copy(skb, hdrlen, 0);
		else
		    new_skb = myskb_copy(skb, hdrlen, 1);

		if (!new_skb) {
		    dev->stats.tx_dropped++;
		    dev_kfree_skb(skb);
		    return 0;
		}
		dev_kfree_skb(skb);
		skb = new_skb;
	    }

            if (is_v4) {
                iip = (struct iphdr *)(skb->data);
                oip = (struct iphdr *)skb_push(skb, ip_esp_len);
            } else {
                iip_ipv6 = (struct ipv6hdr *)(skb->data);
                oip_ipv6 = (struct ipv6hdr *)skb_push(skb, ip_esp_len);
            }

	    /*
	     * skb->transport_header should point to the ESP header.
	     * (note that esp_output() will move it forward past the UDP
	     * header, so have it point just after the IP header, and it
	     * it will be moved later)
	     */
            if (is_v4) {
                skb_set_transport_header(skb, sizeof(*oip));
            } else {
                skb_set_transport_header(skb, sizeof(*oip_ipv6));
            }

	    if (ipsec->k.isXauth == 0) {
		l               = (l2tp_hdr_t *)(((u8 *)oip)+ip_esp_len-
					       sizeof(l2tp_hdr_t));
		
		/* fill out l2tp/ppp */
		l->l2tp.type    = 0x4002;
		l->l2tp.length  = ntohs(iip->tot_len) + sizeof(l->l2tp);
		l->l2tp.tunid   = ipsec->l.l2tp_tunid;
		l->l2tp.sessid  = ipsec->l.l2tp_sessid;
		l->l2tp.pppdata = 0x0021;
		
		/* L2TP UDP */
		l->ludp.source  = ipsec->l.srcl2tp;
		l->ludp.dest    = ipsec->l.dstl2tp;
		l->ludp.len     = l->l2tp.length + sizeof(l->ludp);
		l->ludp.check   = 0;
	    }

	    /* skb->network_header points to the start of the outer IP (top IP) */
	    skb_reset_network_header(skb);

	    /* skb->mac_header should point to the outer IP proto/IPv6 nexthdr field */
            if (is_v4) {
                skb_set_mac_header(skb, offsetof(struct iphdr, protocol));
            } else {
                skb_set_mac_header(skb, offsetof(struct ipv6hdr, nexthdr));
            }

	    /* skb->data points to the payload */
	    if (ipsec->k.isXauth == 0) {
		skb->len      -= (u8 *)l - skb->data;
		skb->data      = (u8 *)(l);
	    } else {
                if (is_v4) {
                    skb->len      -= (u8 *)iip - skb->data;
                    skb->data      = (u8 *)(iip);
                } else {
                    skb->len      -= (u8 *)iip_ipv6 - skb->data;
                    skb->data      = (u8 *)(iip_ipv6);
                }
	    }
	    
            /* outer IP header */
            if (is_v4) {
                oip->ihl      = 0x5;
                oip->version  = 0x4;
                oip->tos      = iip->tos;
                oip->tot_len  = htons(ntohs(iip->tot_len) + ip_esp_len);
                oip->id       = ipsec->ipid++;
#ifdef __FAT_AP__
	        oip->frag_off = 0; /* frag ok */
#else
                oip->frag_off = htons(0x4000); /* don't frag */
#endif
                oip->ttl      = 64;
                if (ipsec->k.isXauth == 0)
                    oip->protocol = IPPROTO_UDP;
                else
                    oip->protocol = IPPROTO_IPIP;
            /*can not copy src and dst ip one shot, since srcIP and dstIP types
              are modified from u32 to ip_addr_t to support v6.*/
#if 0
            /* copy src & dst IP in one shot */
            memcpy(&oip->saddr, &ipsec->k.srcIP, 8);
#endif
                oip->saddr    = IPV4(ipsec->k.srcIP);
                oip->daddr    = IPV4(ipsec->k.dstIP);
                oip->check    = 0;
                oip->check    = in_cksum((u16 *)(oip), sizeof(struct iphdr));
            } else {
                memset(oip_ipv6, 0, sizeof(*oip_ipv6));
                oip_ipv6->version  = 0x6;
                oip_ipv6->payload_len = iip_ipv6->payload_len + ip_esp_len;
                oip_ipv6->hop_limit = 64;
                oip_ipv6->nexthdr = IPPROTO_IPV6;
                ipv6_addr_copy(&oip_ipv6->saddr, (struct in6_addr *)&IPV6(ipsec->k.srcIP));
                ipv6_addr_copy(&oip_ipv6->daddr, (struct in6_addr *)&IPV6(ipsec->k.dstIP));
            }

	    if ( (((u32)(long)(skb->data))%4) != 0) {
		printk("%s:%s:%d data not aligned for optimal performance 0x%08x\n",
			   __FILE__, __FUNCTION__, __LINE__, (u32)(long)(skb->data));
	    }

	    XFRM_SKB_CB(skb)->seq.output.low = ++x->replay.oseq;
	    //dump_buf("l2tp", skb->network_header, skb->len, __LINE__);
	    /* now do ESP encryption w/ UDP encap */
#ifdef CONFIG_ARUBA_AP
        memcpy(&skb->cb[16], &tun, sizeof(struct cryptotun_t *));
#endif
	    error = x->type->output(x, skb);
	    if (error == -EINPROGRESS) {
	        memcpy(&skb->cb[24], &dev, sizeof(struct net_device *));
	    }
	    else if (error == NET_XMIT_DROP || error < 0) {
		dev->stats.tx_errors++;
		dev_kfree_skb(skb);
		return 0;
	    }
	    else {
		tun_net_xmit2(skb);
tosend:
		if (tun_net_xmit3(skb, tun) < 0) {
		    goto drop;
		}
	    }
	    
	    dev->stats.tx_packets++;
	    dev->stats.tx_bytes += statslen;

	    return 0;
	}
#endif

#if 0
	nf_reset(skb);

	/* Enqueue packet */
	skb_queue_tail(&tun->socket.sk->sk_receive_queue, skb);

	/* Notify and wake up reader process */
	if (tun->flags & TUN_FASYNC)
		kill_fasync(&tun->fasync, SIGIO, POLL_IN);
	wake_up_interruptible_poll(&tun->wq.wait, POLLIN |
				   POLLRDNORM | POLLRDBAND);
	return NETDEV_TX_OK;
#endif

drop:
	dev->stats.tx_dropped++;
	kfree_skb(skb);
	return NETDEV_TX_OK;
}
#ifdef CONFIG_ARUBA_AP
int tun_net_xmit_finish(struct sk_buff *skb)
{
	struct tun_struct *tun;

	memcpy(&tun, &skb->cb[16], sizeof(struct tun_struct *));

	tun_net_xmit2(skb);
	if (tun_net_xmit3(skb, tun) < 0)
	{
                tun->dev->stats.tx_dropped++;
		kfree_skb(skb);
		return -1;
	}
	return 0;
}

EXPORT_SYMBOL(tun_net_xmit_finish);
#endif

static void tun_net_mclist(struct net_device *dev)
{
	/*
	 * This callback is supposed to deal with mc filter in
	 * _rx_ path and has nothing to do with the _tx_ path.
	 * In rx path we always accept everything userspace gives us.
	 */
}

#define MIN_MTU 68
#define MAX_MTU 65535

static int
tun_net_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < MIN_MTU || new_mtu + dev->hard_header_len > MAX_MTU)
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}

static netdev_features_t tun_net_fix_features(struct net_device *dev,
	netdev_features_t features)
{
	struct tun_struct *tun = netdev_priv(dev);

	return (features & tun->set_features) | (features & ~TUN_USER_FEATURES);
}
#ifdef CONFIG_NET_POLL_CONTROLLER
static void tun_poll_controller(struct net_device *dev)
{
	/*
	 * Tun only receives frames when:
	 * 1) the char device endpoint gets data from user space
	 * 2) the tun socket gets a sendmsg call from user space
	 * Since both of those are syncronous operations, we are guaranteed
	 * never to have pending data when we poll for it
	 * so theres nothing to do here but return.
	 * We need this though so netpoll recognizes us as an interface that
	 * supports polling, which enables bridge devices in virt setups to
	 * still use netconsole
	 */
	return;
}
#endif
static const struct net_device_ops tun_netdev_ops = {
	.ndo_uninit		= tun_net_uninit,
	.ndo_open		= tun_net_open,
	.ndo_stop		= tun_net_close,
	.ndo_start_xmit		= tun_net_xmit,
	.ndo_change_mtu		= tun_net_change_mtu,
	.ndo_fix_features	= tun_net_fix_features,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= tun_poll_controller,
#endif
};

static const struct net_device_ops tap_netdev_ops = {
	.ndo_uninit		= tun_net_uninit,
	.ndo_open		= tun_net_open,
	.ndo_stop		= tun_net_close,
	.ndo_start_xmit		= tun_net_xmit,
	.ndo_change_mtu		= tun_net_change_mtu,
	.ndo_fix_features	= tun_net_fix_features,
	.ndo_set_rx_mode	= tun_net_mclist,
	.ndo_set_mac_address	= eth_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= tun_poll_controller,
#endif
};

/* Initialize net device. */
static void tun_net_init(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);

	switch (tun->flags & TUN_TYPE_MASK) {
	case TUN_TUN_DEV:
		dev->netdev_ops = &tun_netdev_ops;

		/* Point-to-Point TUN Device */
		dev->hard_header_len = 192; /*14B ETH + 40B V6 + 4B ESP + 40B V6 + 64B sbeth*/ 
		dev->addr_len = 0;
		dev->mtu = 1500;

		/* Zero header length */
		dev->type = ARPHRD_NONE;
		dev->flags = IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
#ifdef CONFIG_ARUBA_AP
        /* something small or else the encryption will take up all the CPU cycle preventing watchdog process from kicking in. */
        dev->tx_queue_len = 64;
#else
		dev->tx_queue_len = TUN_READQ_SIZE;  /* We prefer our own queue length */
#endif
       

		break;

	case TUN_TAP_DEV:
		dev->netdev_ops = &tap_netdev_ops;
		/* Ethernet TAP Device */
		ether_setup(dev);
		dev->priv_flags &= ~IFF_TX_SKB_SHARING;

		eth_hw_addr_random(dev);

		dev->tx_queue_len = TUN_READQ_SIZE;  /* We prefer our own queue length */
		break;
	}
}

/* Character device part */

/* Poll */
static unsigned int tun_chr_poll(struct file *file, poll_table * wait)
{
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun = __tun_get(tfile);
	struct sock *sk;
	unsigned int mask = 0;

	if (!tun)
		return POLLERR;

	sk = tun->socket.sk;

	tun_debug(KERN_INFO, tun, "tun_chr_poll\n");

	poll_wait(file, &tun->wq.wait, wait);

	if (!skb_queue_empty(&sk->sk_receive_queue))
		mask |= POLLIN | POLLRDNORM;

	if (sock_writeable(sk) ||
	    (!test_and_set_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags) &&
	     sock_writeable(sk)))
		mask |= POLLOUT | POLLWRNORM;

	if (tun->dev->reg_state != NETREG_REGISTERED)
		mask = POLLERR;

	tun_put(tun);
	return mask;
}

/* prepad is the amount to reserve at front.  len is length after that.
 * linear is a hint as to how much to copy (usually headers). */
static struct sk_buff *tun_alloc_skb(struct tun_struct *tun,
				     size_t prepad, size_t len,
				     size_t linear, int noblock)
{
	struct sock *sk = tun->socket.sk;
	struct sk_buff *skb;
	int err;

	sock_update_classid(sk);

	/* Under a page?  Don't bother with paged skb. */
	if (prepad + len < PAGE_SIZE || !linear)
		linear = len;

	skb = sock_alloc_send_pskb(sk, prepad + linear, len - linear, noblock,
				   &err);
	if (!skb)
		return ERR_PTR(err);

	skb_reserve(skb, prepad);
	skb_put(skb, linear);
	skb->data_len = len - linear;
	skb->len += len - linear;

	return skb;
}

/* Get packet from user space buffer */
static ssize_t tun_get_user(struct tun_struct *tun,
			    const struct iovec *iv, size_t count,
			    int noblock)
{
	struct tun_pi pi = { 0, cpu_to_be16(ETH_P_IP) };
	struct sk_buff *skb;
	size_t len = count, align = NET_SKB_PAD;
	struct virtio_net_hdr gso = { 0 };
	int offset = 0;
#ifdef CONFIG_ARUBA_AP
    cryptotun_t *ipsec = (cryptotun_t *)(tun+1);
#endif
	if (!(tun->flags & TUN_NO_PI)) {
		if (len < sizeof(pi))
			return -EINVAL;
		len -= sizeof(pi);

		if (memcpy_fromiovecend((void *)&pi, iv, 0, sizeof(pi)))
			return -EFAULT;
		offset += sizeof(pi);
	}

	if (tun->flags & TUN_VNET_HDR) {
		if (len < tun->vnet_hdr_sz)
			return -EINVAL;
		len -= tun->vnet_hdr_sz;

		if (memcpy_fromiovecend((void *)&gso, iv, offset, sizeof(gso)))
			return -EFAULT;

		if ((gso.flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) &&
		    gso.csum_start + gso.csum_offset + 2 > gso.hdr_len)
			gso.hdr_len = gso.csum_start + gso.csum_offset + 2;

		if (gso.hdr_len > len)
			return -EINVAL;
		offset += tun->vnet_hdr_sz;
	}

	if ((tun->flags & TUN_TYPE_MASK) == TUN_TAP_DEV) {
		align += NET_IP_ALIGN;
		if (unlikely(len < ETH_HLEN ||
			     (gso.hdr_len && gso.hdr_len < ETH_HLEN)))
			return -EINVAL;
	}

	skb = tun_alloc_skb(tun, align, len, gso.hdr_len, noblock);
	if (IS_ERR(skb)) {
		if (PTR_ERR(skb) != -EAGAIN)
			tun->dev->stats.rx_dropped++;
		return PTR_ERR(skb);
	}

	if (skb_copy_datagram_from_iovec(skb, 0, iv, offset, len)) {
		tun->dev->stats.rx_dropped++;
		kfree_skb(skb);
		return -EFAULT;
	}

	if (gso.flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) {
		if (!skb_partial_csum_set(skb, gso.csum_start,
					  gso.csum_offset)) {
			tun->dev->stats.rx_frame_errors++;
			kfree_skb(skb);
			return -EINVAL;
		}
	}

	switch (tun->flags & TUN_TYPE_MASK) {
	case TUN_TUN_DEV:
		if (tun->flags & TUN_NO_PI) {
			switch (skb->data[0] & 0xf0) {
			case 0x40:
				pi.proto = htons(ETH_P_IP);
				break;
			case 0x60:
				pi.proto = htons(ETH_P_IPV6);
				break;
			default:
				tun->dev->stats.rx_dropped++;
				kfree_skb(skb);
				return -EINVAL;
			}
		}

		skb_reset_mac_header(skb);
		skb->protocol = pi.proto;
		skb->dev = tun->dev;
		break;
	case TUN_TAP_DEV:
		skb->protocol = eth_type_trans(skb, tun->dev);
		break;
	}

	if (gso.gso_type != VIRTIO_NET_HDR_GSO_NONE) {
		pr_debug("GSO!\n");
		switch (gso.gso_type & ~VIRTIO_NET_HDR_GSO_ECN) {
		case VIRTIO_NET_HDR_GSO_TCPV4:
			skb_shinfo(skb)->gso_type = SKB_GSO_TCPV4;
			break;
		case VIRTIO_NET_HDR_GSO_TCPV6:
			skb_shinfo(skb)->gso_type = SKB_GSO_TCPV6;
			break;
		case VIRTIO_NET_HDR_GSO_UDP:
			skb_shinfo(skb)->gso_type = SKB_GSO_UDP;
			break;
		default:
			tun->dev->stats.rx_frame_errors++;
			kfree_skb(skb);
			return -EINVAL;
		}

		if (gso.gso_type & VIRTIO_NET_HDR_GSO_ECN)
			skb_shinfo(skb)->gso_type |= SKB_GSO_TCP_ECN;

		skb_shinfo(skb)->gso_size = gso.gso_size;
		if (skb_shinfo(skb)->gso_size == 0) {
			tun->dev->stats.rx_frame_errors++;
			kfree_skb(skb);
			return -EINVAL;
		}

		/* Header must be checked, and gso_segs computed. */
		skb_shinfo(skb)->gso_type |= SKB_GSO_DODGY;
		skb_shinfo(skb)->gso_segs = 0;
	}
#ifdef CONFIG_ARUBA_AP
    /* masquerade source as br0 */
    if (ipsec->output_dev)
        skb->dev = ipsec->output_dev;
    else
        skb->dev = tun->dev;
#endif
	netif_rx_ni(skb);

	tun->dev->stats.rx_packets++;
	tun->dev->stats.rx_bytes += len;

	return count;
}

static ssize_t tun_chr_aio_write(struct kiocb *iocb, const struct iovec *iv,
			      unsigned long count, loff_t pos)
{
	struct file *file = iocb->ki_filp;
	struct tun_struct *tun = tun_get(file);
	ssize_t result;

	if (!tun)
		return -EBADFD;

	tun_debug(KERN_INFO, tun, "tun_chr_write %ld\n", count);

	result = tun_get_user(tun, iv, iov_length(iv, count),
			      file->f_flags & O_NONBLOCK);

	tun_put(tun);
	return result;
}

/* Put packet to the user space buffer */
static ssize_t tun_put_user(struct tun_struct *tun,
			    struct sk_buff *skb,
			    const struct iovec *iv, int len)
{
	struct tun_pi pi = { 0, skb->protocol };
	ssize_t total = 0;

	if (!(tun->flags & TUN_NO_PI)) {
		if ((len -= sizeof(pi)) < 0)
			return -EINVAL;

		if (len < skb->len) {
			/* Packet will be striped */
			pi.flags |= TUN_PKT_STRIP;
		}

		if (memcpy_toiovecend(iv, (void *) &pi, 0, sizeof(pi)))
			return -EFAULT;
		total += sizeof(pi);
	}

	if (tun->flags & TUN_VNET_HDR) {
		struct virtio_net_hdr gso = { 0 }; /* no info leak */
		if ((len -= tun->vnet_hdr_sz) < 0)
			return -EINVAL;

		if (skb_is_gso(skb)) {
			struct skb_shared_info *sinfo = skb_shinfo(skb);

			/* This is a hint as to how much should be linear. */
			gso.hdr_len = skb_headlen(skb);
			gso.gso_size = sinfo->gso_size;
			if (sinfo->gso_type & SKB_GSO_TCPV4)
				gso.gso_type = VIRTIO_NET_HDR_GSO_TCPV4;
			else if (sinfo->gso_type & SKB_GSO_TCPV6)
				gso.gso_type = VIRTIO_NET_HDR_GSO_TCPV6;
			else if (sinfo->gso_type & SKB_GSO_UDP)
				gso.gso_type = VIRTIO_NET_HDR_GSO_UDP;
			else {
				pr_err("unexpected GSO type: "
				       "0x%x, gso_size %d, hdr_len %d\n",
				       sinfo->gso_type, gso.gso_size,
				       gso.hdr_len);
				print_hex_dump(KERN_ERR, "tun: ",
					       DUMP_PREFIX_NONE,
					       16, 1, skb->head,
					       min((int)gso.hdr_len, 64), true);
				WARN_ON_ONCE(1);
				return -EINVAL;
			}
			if (sinfo->gso_type & SKB_GSO_TCP_ECN)
				gso.gso_type |= VIRTIO_NET_HDR_GSO_ECN;
		} else
			gso.gso_type = VIRTIO_NET_HDR_GSO_NONE;

		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			gso.flags = VIRTIO_NET_HDR_F_NEEDS_CSUM;
			gso.csum_start = skb_checksum_start_offset(skb);
			gso.csum_offset = skb->csum_offset;
		} else if (skb->ip_summed == CHECKSUM_UNNECESSARY) {
			gso.flags = VIRTIO_NET_HDR_F_DATA_VALID;
		} /* else everything is zero */

		if (unlikely(memcpy_toiovecend(iv, (void *)&gso, total,
					       sizeof(gso))))
			return -EFAULT;
		total += tun->vnet_hdr_sz;
	}

	len = min_t(int, skb->len, len);

	skb_copy_datagram_const_iovec(skb, 0, iv, total, len);
	total += skb->len;

	tun->dev->stats.tx_packets++;
	tun->dev->stats.tx_bytes += len;

	return total;
}

static ssize_t tun_do_read(struct tun_struct *tun,
			   struct kiocb *iocb, const struct iovec *iv,
			   ssize_t len, int noblock)
{
	DECLARE_WAITQUEUE(wait, current);
	struct sk_buff *skb;
	ssize_t ret = 0;

	tun_debug(KERN_INFO, tun, "tun_chr_read\n");

	if (unlikely(!noblock))
		add_wait_queue(&tun->wq.wait, &wait);
	while (len) {
		current->state = TASK_INTERRUPTIBLE;

		/* Read frames from the queue */
		if (!(skb=skb_dequeue(&tun->socket.sk->sk_receive_queue))) {
			if (noblock) {
				ret = -EAGAIN;
				break;
			}
			if (signal_pending(current)) {
				ret = -ERESTARTSYS;
				break;
			}
			if (tun->dev->reg_state != NETREG_REGISTERED) {
				ret = -EIO;
				break;
			}

			/* Nothing to read, let's sleep */
			schedule();
			continue;
		}
		netif_wake_queue(tun->dev);

		ret = tun_put_user(tun, skb, iv, len);
		kfree_skb(skb);
		break;
	}

	current->state = TASK_RUNNING;
	if (unlikely(!noblock))
		remove_wait_queue(&tun->wq.wait, &wait);

	return ret;
}

static ssize_t tun_chr_aio_read(struct kiocb *iocb, const struct iovec *iv,
			    unsigned long count, loff_t pos)
{
	struct file *file = iocb->ki_filp;
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun = __tun_get(tfile);
	ssize_t len, ret;

	if (!tun)
		return -EBADFD;
	len = iov_length(iv, count);
	if (len < 0) {
		ret = -EINVAL;
		goto out;
	}

	ret = tun_do_read(tun, iocb, iv, len, file->f_flags & O_NONBLOCK);
	ret = min_t(ssize_t, ret, len);
	if (ret > 0)
		iocb->ki_pos = ret;
out:
	tun_put(tun);
	return ret;
}

static void tun_setup(struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);

	tun->owner = -1;
	tun->group = -1;

	dev->ethtool_ops = &tun_ethtool_ops;
	dev->destructor = tun_free_netdev;
}

/* Trivial set of netlink ops to allow deleting tun or tap
 * device with netlink.
 */
static int tun_validate(struct nlattr *tb[], struct nlattr *data[])
{
	return -EINVAL;
}

static struct rtnl_link_ops tun_link_ops __read_mostly = {
	.kind		= DRV_NAME,
	.priv_size	= sizeof(struct tun_struct),
	.setup		= tun_setup,
	.validate	= tun_validate,
};

static void tun_sock_write_space(struct sock *sk)
{
	struct tun_struct *tun;
	wait_queue_head_t *wqueue;

	if (!sock_writeable(sk))
		return;

	if (!test_and_clear_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags))
		return;

	wqueue = sk_sleep(sk);
	if (wqueue && waitqueue_active(wqueue))
		wake_up_interruptible_sync_poll(wqueue, POLLOUT |
						POLLWRNORM | POLLWRBAND);

	tun = tun_sk(sk)->tun;
	kill_fasync(&tun->fasync, SIGIO, POLL_OUT);
}

static void tun_sock_destruct(struct sock *sk)
{
	free_netdev(tun_sk(sk)->tun->dev);
}

static int tun_sendmsg(struct kiocb *iocb, struct socket *sock,
		       struct msghdr *m, size_t total_len)
{
	struct tun_struct *tun = container_of(sock, struct tun_struct, socket);
	return tun_get_user(tun, m->msg_iov, total_len,
			    m->msg_flags & MSG_DONTWAIT);
}

static int tun_recvmsg(struct kiocb *iocb, struct socket *sock,
		       struct msghdr *m, size_t total_len,
		       int flags)
{
	struct tun_struct *tun = container_of(sock, struct tun_struct, socket);
	int ret;
	if (flags & ~(MSG_DONTWAIT|MSG_TRUNC))
		return -EINVAL;
	ret = tun_do_read(tun, iocb, m->msg_iov, total_len,
			  flags & MSG_DONTWAIT);
	if (ret > total_len) {
		m->msg_flags |= MSG_TRUNC;
		ret = flags & MSG_TRUNC ? ret : total_len;
	}
	return ret;
}

static int tun_release(struct socket *sock)
{
	if (sock->sk)
		sock_put(sock->sk);
	return 0;
}

/* Ops structure to mimic raw sockets with tun */
static const struct proto_ops tun_socket_ops = {
	.sendmsg = tun_sendmsg,
	.recvmsg = tun_recvmsg,
	.release = tun_release,
};

static struct proto tun_proto = {
	.name		= "tun",
	.owner		= THIS_MODULE,
	.obj_size	= sizeof(struct tun_sock),
};

static int tun_flags(struct tun_struct *tun)
{
	int flags = 0;

	if (tun->flags & TUN_TUN_DEV)
		flags |= IFF_TUN;
	else
		flags |= IFF_TAP;

	if (tun->flags & TUN_NO_PI)
		flags |= IFF_NO_PI;

	if (tun->flags & TUN_ONE_QUEUE)
		flags |= IFF_ONE_QUEUE;

	if (tun->flags & TUN_VNET_HDR)
		flags |= IFF_VNET_HDR;

	return flags;
}

static ssize_t tun_show_flags(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct tun_struct *tun = netdev_priv(to_net_dev(dev));
	return sprintf(buf, "0x%x\n", tun_flags(tun));
}

static ssize_t tun_show_owner(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct tun_struct *tun = netdev_priv(to_net_dev(dev));
	return sprintf(buf, "%d\n", tun->owner);
}

static ssize_t tun_show_group(struct device *dev, struct device_attribute *attr,
			      char *buf)
{
	struct tun_struct *tun = netdev_priv(to_net_dev(dev));
	return sprintf(buf, "%d\n", tun->group);
}

static DEVICE_ATTR(tun_flags, 0444, tun_show_flags, NULL);
static DEVICE_ATTR(owner, 0444, tun_show_owner, NULL);
static DEVICE_ATTR(group, 0444, tun_show_group, NULL);

static int tun_set_iff(struct net *net, struct file *file, struct ifreq *ifr)
{
	struct sock *sk;
	struct tun_struct *tun;
	struct net_device *dev;
	int err;
#ifdef CONFIG_ARUBA_AP
    cryptotun_t *ipsec;
#endif
	dev = __dev_get_by_name(net, ifr->ifr_name);
	if (dev) {
		const struct cred *cred = current_cred();

		if (ifr->ifr_flags & IFF_TUN_EXCL)
			return -EBUSY;
		if ((ifr->ifr_flags & IFF_TUN) && dev->netdev_ops == &tun_netdev_ops)
			tun = netdev_priv(dev);
		else if ((ifr->ifr_flags & IFF_TAP) && dev->netdev_ops == &tap_netdev_ops)
			tun = netdev_priv(dev);
		else
			return -EINVAL;

		if (((tun->owner != -1 && cred->euid != tun->owner) ||
		     (tun->group != -1 && !in_egroup_p(tun->group))) &&
		    !capable(CAP_NET_ADMIN))
			return -EPERM;
		err = security_tun_dev_attach(tun->socket.sk);
		if (err < 0)
			return err;

		err = tun_attach(tun, file);
		if (err < 0)
			return err;
	}
	else {
		char *name;
		unsigned long flags = 0;

		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		err = security_tun_dev_create();
		if (err < 0)
			return err;

		/* Set dev type */
		if (ifr->ifr_flags & IFF_TUN) {
			/* TUN device */
			flags |= TUN_TUN_DEV;
			name = "tun%d";
		} else if (ifr->ifr_flags & IFF_TAP) {
			/* TAP device */
			flags |= TUN_TAP_DEV;
			name = "tap%d";
		} else
			return -EINVAL;

		if (*ifr->ifr_name)
			name = ifr->ifr_name;
#ifdef CONFIG_ARUBA_AP
		/* increase the struct to hold:
		   IPSEC keys
		   L2TP tunid and sessid
		   whether or not to double encrypt
		   src & dest IPSEC NAT-T port
		   src & dest L2TP ports */
		dev = alloc_netdev(sizeof(struct tun_struct) + sizeof(cryptotun_t), name,
 				   tun_setup);
#else
		dev = alloc_netdev(sizeof(struct tun_struct), name,
				   tun_setup);
#endif
		if (!dev)
			return -ENOMEM;

		dev_net_set(dev, net);
		dev->rtnl_link_ops = &tun_link_ops;

		tun = netdev_priv(dev);
		tun->dev = dev;
		tun->flags = flags;
#ifdef CONFIG_ARUBA_AP
        tun->if_flags = 0;
#endif
		tun->txflt.count = 0;
		tun->vnet_hdr_sz = sizeof(struct virtio_net_hdr);
		set_bit(SOCK_EXTERNALLY_ALLOCATED, &tun->socket.flags);

                spin_lock_init(&tun->lock);

		err = -ENOMEM;
		sk = sk_alloc(&init_net, AF_UNSPEC, GFP_KERNEL, &tun_proto);
		if (!sk)
			goto err_free_dev;

		sk_change_net(sk, net);
		tun->socket.wq = &tun->wq;
		init_waitqueue_head(&tun->wq.wait);
		tun->socket.ops = &tun_socket_ops;
		sock_init_data(&tun->socket, sk);
		sk->sk_write_space = tun_sock_write_space;
		sk->sk_sndbuf = INT_MAX;

		tun_sk(sk)->tun = tun;

		security_tun_dev_post_create(sk);
#ifdef CONFIG_ARUBA_AP
        /* initial values */
        ipsec = (cryptotun_t *)(tun+1);
        memset(ipsec, 0, sizeof(*ipsec));
        /* initialize to default def */
        strcpy(ipsec->dev_name, OUTPUTDEV);
        ipsec->output_dev = dev_get_by_name(net, ipsec->dev_name);
        ipsec->k.dstNatt = (ifr->ifr_flags & IFF_TUN_IPV6) ? htons(500) : htons(4500);
        ipsec->k.natProto = IPPROTO_UDP;
        ipsec->k.seq_snd  = 0;
        ipsec->k.seq_rcv  = 1;
#endif
		tun_net_init(dev);

		dev->hw_features = NETIF_F_SG | NETIF_F_FRAGLIST |
			TUN_USER_FEATURES;
		dev->features = dev->hw_features;

		err = register_netdevice(tun->dev);
		if (err < 0)
			goto err_free_sk;

		if (device_create_file(&tun->dev->dev, &dev_attr_tun_flags) ||
		    device_create_file(&tun->dev->dev, &dev_attr_owner) ||
		    device_create_file(&tun->dev->dev, &dev_attr_group))
			pr_err("Failed to create tun sysfs files\n");

		sk->sk_destruct = tun_sock_destruct;

		err = tun_attach(tun, file);
		if (err < 0)
			goto failed;
#ifdef CONFIG_ARUBA_AP
		spin_lock_bh(&tun_list_lock);
        list_add(&tun->list, &tun_dev_list);
		spin_unlock_bh(&tun_list_lock);
#endif
	}

	tun_debug(KERN_INFO, tun, "tun_set_iff\n");

	if (ifr->ifr_flags & IFF_NO_PI)
		tun->flags |= TUN_NO_PI;
	else
		tun->flags &= ~TUN_NO_PI;

	if (ifr->ifr_flags & IFF_ONE_QUEUE)
		tun->flags |= TUN_ONE_QUEUE;
	else
		tun->flags &= ~TUN_ONE_QUEUE;

	if (ifr->ifr_flags & IFF_VNET_HDR)
		tun->flags |= TUN_VNET_HDR;
	else
		tun->flags &= ~TUN_VNET_HDR;

	/* Make sure persistent devices do not get stuck in
	 * xoff state.
	 */
	if (netif_running(tun->dev))
		netif_wake_queue(tun->dev);

	strcpy(ifr->ifr_name, tun->dev->name);
	return 0;

 err_free_sk:
	tun_free_netdev(dev);
 err_free_dev:
	free_netdev(dev);
 failed:
	return err;
}
#ifdef CONFIG_ARUBA_AP
static int
is_l2tp_data(struct udphdr *udp, int *offset)
{
    /* we're the client, so only check source.  Different from switch
       code. */
    if (udp->len > 18 &&
        udp->source == htons(1701)) {
        if (((u16 *)((udp)+1))[0] == 0x4002 /* L2TP Data w/ len */ &&
            ((u16 *)((udp)+1))[4] == 0x0021) {
            *offset = 18;
            return 1;
        }  else if(((u16 *)((udp)+1))[0] == 0x0002 /* L2TP Data no len */ &&
                   ((u16 *)((udp)+1))[3] == 0x0021) {
            *offset = 16;
            return 1;
        }
    }
    return 0;
}

static int tun_recv_esp3(struct sk_buff *skb, cryptotun_t *ipsec)
{
	if (ipsec->k.isXauth == 0)
	{
	    struct iphdr *iph = (struct iphdr *)skb_network_header(skb);
	    int offset;

	    if (iph->protocol == IPPROTO_UDP &&
		is_l2tp_data((struct udphdr *)skb_transport_header(skb), &offset)) {
		
		
		/* trim everything by offset bytes */
		skb_pull(skb, offset);
		skb->network_header += offset;
		skb->transport_header = (skb->network_header + iph->ihl*4);
	    } else {
		/* need to push back the IP header */
		skb_push(skb, skb->transport_header - skb->network_header);
	    }
        }
	return 0;
}

int tun_recv_esp2_prep(struct sk_buff *skb, void *arg, int gre_flags)
{
    struct tun_struct *tun = (struct tun_struct *)arg;
    struct net *net;
    struct iphdr *iph;
    cryptotun_t *ipsec = (cryptotun_t *)(tun+1);

#if defined(CONFIG_ARUBA_AP) && !defined(CONFIG_GRENACHE) && !defined(CONFIG_ARUBA_CONTROLLER)
    asap_ipsec_mirror_packet_to_dev(skb, 0);
#endif

    /* reset a few things */
    if (gre_flags)
        skb_reset_mac_header(skb);
    iph = (struct iphdr *)skb->data;
    if (iph->version == 4) {
        skb->protocol = __constant_htons(ETH_P_IP);
    } else {
        skb->protocol = __constant_htons(ETH_P_IPV6); 
    }
    skb->pkt_type = PACKET_HOST;
    if (skb->sp) {
        secpath_reset(skb);
    }

    if (tun->flags & TUN_NOCHECKSUM)
        skb->ip_summed = CHECKSUM_UNNECESSARY;
    if (skb_dst(skb)) {
        skb_dst_drop(skb);
        skb_dst_set(skb, NULL);
    }
#ifdef __FAT_AP__
    skb->dev = tun->dev;
#else
    /* masquerade source as br0 */
    if (ipsec->output_dev)
        skb->dev = ipsec->output_dev;
    else
        skb->dev = tun->dev;
#endif
    skb_reset_network_header(skb);
    skb_reset_transport_header(skb);

    net = dev_net(skb->dev);
    iph = (struct iphdr *)skb->data;
    /* now send to networking stack */
    nf_reset(skb);

    tun->dev->last_rx = jiffies;
    tun->dev->stats.rx_packets++;
    tun->dev->stats.rx_bytes += skb->len;

    return 0;
}
EXPORT_SYMBOL(tun_recv_esp2_prep);

static int tun_recv_esp2(struct sk_buff *skb, struct tun_struct *tun, int gre_flags)
{
    tun_recv_esp2_prep(skb, (void *)tun, gre_flags);
#ifdef __FAT_AP__
    /* Send to firewall processing first */
    if (!ip_asap_firewall_input || !ip_asap_firewall_input(skb)) {
        netif_rx(skb);
    }
#else
    /* now send to networking stack */
    netif_rx(skb);
#endif
    return 0;
}

/*
 * On successful lookup, the returned @tun will increace its refcnt,
 * the caller function needs to call tun_lookup_tun_put after use.
 */
void *
tun_lookup_tun(struct sk_buff *skb)
{
    struct tun_struct *tun = NULL, *nxt;
    cryptotun_t *ipsec = NULL;
    struct iphdr *iptmp = 0;
    struct udphdr *udp = 0;
    int    found = 0;
    struct ipv6hdr *ipv6_hdr   = NULL;

    iptmp = (struct iphdr *)skb_network_header(skb);
    if (iptmp->version == 4) {
        udp = (struct udphdr *)skb_transport_header(skb);
    } else if (iptmp->version == 6) {
        ipv6_hdr = (struct ipv6hdr *)skb_network_header(skb);
    } else {
        /*Invalid IP packet. Drop it!*/
        return NULL;
    }

    spin_lock_bh(&tun_list_lock);
    list_for_each_entry_safe(tun, nxt, &tun_dev_list, list) {
#ifdef CONFIG_ARDMORE
    /* OCTOMORE: TBR */
    __tun_get(tun->tfile);
#endif
	ipsec = (cryptotun_t *)(tun+1);
        if (iptmp->version == 4) {
            if (ipsec->in_xfrm && iptmp->saddr == IPV4(ipsec->k.dstIP) &&
                iptmp->daddr == IPV4(ipsec->k.srcIP) &&
                udp->source == ipsec->k.dstNatt &&
                udp->dest   == ipsec->k.srcNatt) {
                found = 1;
                break;
            }
        } else {
            if (ipsec->in_xfrm &&
                (memcmp(ipv6_hdr->saddr.s6_addr32,
                        ipsec->k.dstIP.v6addr32, IPV6_SIZE) == 0) &&
                (memcmp(ipv6_hdr->daddr.s6_addr32,
                        ipsec->k.srcIP.v6addr32, IPV6_SIZE) == 0)) {
                found = 1;
                break;
            }
        }
#ifdef CONFIG_ARDMORE
    /* OCTOMORE: TBR */
	tun_put(tun);
#endif
    }
    spin_unlock_bh(&tun_list_lock);
    if (found) return (void *)tun;
    else return NULL;
}
EXPORT_SYMBOL(tun_lookup_tun);

#ifdef CONFIG_ARDMORE
/* OCTOMORE: TBR */
void tun_lookup_tun_put(void *tun)
{
    tun_put((struct tun_struct *)tun);
}
EXPORT_SYMBOL(tun_lookup_tun_put);
#endif

int
tun_recv_esp(struct sk_buff *skb)
{
    struct tun_struct *tun = NULL;
    cryptotun_t *ipsec = NULL;

#ifdef CONFIG_ARDMORE
    /* OCTOMORE: TBR */
    if (NULL == (tun = (struct tun_struct *)tun_lookup_tun(skb)))
        goto drop;

        ipsec = (cryptotun_t *)(tun+1);
#else
    int    found = 0;

    /* to keep this frame, we need to:
       - call skb_get(skb) (to increment reference count)
       - return 0...caller will then call kfree_skb as we're telling it that
         we're eating the frame.
	*/
    if (NULL != (tun = (struct tun_struct *)tun_lookup_tun(skb))) {
		ipsec = (cryptotun_t *)(tun+1);
		found = 1;
	}

	if (found) {
#endif
    struct xfrm_state *x = ipsec->in_xfrm;
	int gre_flags = 0;
        int err = 0;
        struct iphdr *iptmp = (struct iphdr *)skb_network_header(skb);
        if (iptmp->version == 4) { 
	    /*
	     * need to pull skb to the beginning of ESP, esp4.c expects that.
	     * however, it also expects skb->network_header and 
	     * skb->transport_header to remain where they were.  He will do 
	     * further push/pull to adjust back.  Note that when we got in here, 
	     * the IP header was already pulled for us by the udp.c code
	     */
            skb_pull(skb, sizeof(struct udphdr));
        } else {
            /*for v6 packets, skb is already pulled to point to ESP header.*/
        }

        /* handle single encryption first */
        if (ipsec->allowclear  && iptmp->version == 4) {
            __u32 spi = ntohl(*(__u32 *)skb->data);
            if ((spi & 0xff) == 0x88) {
            //printk("tun_recv_esp fake SPI %x\n", spi);
                struct iphdr *greip = (struct iphdr *)(skb->data+4);
                if (greip->protocol != IPPROTO_GRE ||
                    SJ_GRE(greip)) {
                    /* only allow GRE in single encryption AND never allow
                       it for securejack */
		    // XXX; note that this excludes port 0 on the assumption
		    // that it is only used in SJ over mesh (may not be true
		    // for Moscato)
                    skb->dev->stats.rx_dropped++;
            	    //printk("tun_recv_esp dropping GRE proto %d SPI %x\n", greip->protocol, spi);

#ifdef CONFIG_ARDMORE
                    /* OCTOMORE: TBR */
                    goto free;
#else
                    // We need to free the skb here, since unlike 2.6.16, the
                    // caller doesn't free the skb on return 0.
                    kfree_skb(skb);
                    return 0;
#endif
                }
                /* trim esp, IP, and aruba gre(4B) */
                skb_pull(skb, 4);
		gre_flags = 1;
                goto sendout;
            }
        } else if (ipsec->allowclear && iptmp->version == 6) {
            u32 spi = ntohl(*(__u32 *)skb->data);
            if ((spi & 0xff) == 0x88) {
                struct ipv6hdr *iip;
                iip = (struct ipv6hdr *)(skb->data + 4/*spi*/);
                if (iip->nexthdr == NEXTHDR_FRAGMENT) {
                    struct frag_hdr *fh = (struct frag_hdr *)(iip + 1);
                    if ((fh->frag_off & FRAG_OFFSET) == 0) {
                        /* only allow GRE in single encryption AND never allow 
                         * it for securejack 
                         * XXX; note that this excludes port 0 on the assumption
                         * that it is only used in SJ over mesh (may not be true
                         * for Moscato)
                         */
                        skb->dev->stats.rx_dropped++;
                        printk("tun_recv_esp dropping GRE proto %d SPI %x\n", iip->nexthdr, spi);

                        // We need to free the skb here, since unlike 2.6.16, the
                        // caller doesn't free the skb on return 0.
                        kfree_skb(skb);
                        return 0;
                    }
                }
                /* Trim esp hdr 4B SPI*/
                skb_pull(skb, 4);
                gre_flags = 1;
                goto sendout;
            }
        }
        
	/* Allocate new secpath or COW existing one. */
	if (!skb->sp || atomic_read(&skb->sp->refcnt) != 1) {
		struct sec_path *sp;
		struct net *net = dev_net(skb->dev);

		sp = secpath_dup(skb->sp);
		if (!sp) {
			XFRM_INC_STATS(net, LINUX_MIB_XFRMINERROR);
#ifdef CONFIG_ARDMORE
            /* OCTOMORE: TBR */
            tun_lookup_tun_put(tun);
#endif
			goto drop;
		}
		if (skb->sp)
			secpath_put(skb->sp);
		skb->sp = sp;
	}

        spin_lock_bh(&tun->lock);
        x = ipsec->in_xfrm;
        if (!x) {
            spin_unlock_bh(&tun->lock);
#ifdef CONFIG_ARDMORE
            /* OCTOMORE: TBR */
            tun_lookup_tun_put(tun);
#endif
            goto drop;
        }
	xfrm_state_hold(x);
        spin_unlock_bh(&tun->lock);

        skb->sp->xvec[skb->sp->len++] = x;

#ifdef CONFIG_ARUBA_AP
    memcpy(&skb->cb[16], &tun, sizeof(struct tun_struct *));
#endif
	err = x->type->input(x, skb);
	if (err == -EINPROGRESS) {
        tun_debug(KERN_INFO, tun, "%s, Decryption in progress\n", __func__);
	}

	else if ((x->props.family == AF_INET && err != IPPROTO_IPIP) ||
                 (x->props.family == AF_INET6 && err != IPPROTO_IPV6)) { 
            //dump_buf("decryptERROR", skb->data, skb->len, __LINE__);
            skb->dev->stats.rx_errors++;
            skb->dev->stats.rx_frame_errors++;
#ifdef CONFIG_ARUBA_AP
            if((err != -EBUSY) && net_ratelimit())
#endif
            printk(KERN_DEBUG "%s:%s:%d decryption failed: len %u\n", __FILE__, __FUNCTION__, __LINE__, skb->len);
//          printk("%s:%s:%d decryption failed: len %u\n", __FILE__, __FUNCTION__, __LINE__, skb->len);

#ifdef CONFIG_ARDMORE
            /* OCTOMORE: TBR */
            goto free;
#else
            // We need to free the skb here, since unlike 2.6.16, the
            // caller doesn't free the skb on return 0.
            kfree_skb(skb);
            return 0;
#endif
        }

//bdump(skb->data, 32, "data");
        //dump_buf("recvipafterdecrypt", skb->data, skb->len, __LINE__);
	else {
		tun_recv_esp3(skb, ipsec);
sendout:
		tun_recv_esp2(skb, tun, gre_flags);
        }

#ifdef CONFIG_ARDMORE
    /* OCTOMORE: TBR */
    tun_lookup_tun_put(tun);
	return 0;

free:
    tun_lookup_tun_put(tun);
    // We need to free the skb here, since unlike 2.6.16, the
    // caller doesn't free the skb on return 0.
    kfree_skb(skb);
    return 0;
#else
	return 0;
	}
#endif
drop:
    skb->dev->stats.rx_errors++;
    return -1;
}
EXPORT_SYMBOL(tun_recv_esp);

int tun_recv_esp_finish(struct sk_buff *skb)
{
	struct tun_struct *tun;
	cryptotun_t *ipsec;

	memcpy(&tun, &skb->cb[16], sizeof(struct tun_struct *));
	ipsec = (cryptotun_t *)(tun+1);

	tun_recv_esp3(skb, ipsec);
	tun_recv_esp2(skb, tun, 0);

	return 0;
}
EXPORT_SYMBOL(tun_recv_esp_finish);
#endif
static int tun_get_iff(struct net *net, struct tun_struct *tun,
		       struct ifreq *ifr)
{
	tun_debug(KERN_INFO, tun, "tun_get_iff\n");

	strcpy(ifr->ifr_name, tun->dev->name);

	ifr->ifr_flags = tun_flags(tun);

	return 0;
}

/* This is like a cut-down ethtool ops, except done via tun fd so no
 * privs required. */
static int set_offload(struct tun_struct *tun, unsigned long arg)
{
	netdev_features_t features = 0;

	if (arg & TUN_F_CSUM) {
		features |= NETIF_F_HW_CSUM;
		arg &= ~TUN_F_CSUM;

		if (arg & (TUN_F_TSO4|TUN_F_TSO6)) {
			if (arg & TUN_F_TSO_ECN) {
				features |= NETIF_F_TSO_ECN;
				arg &= ~TUN_F_TSO_ECN;
			}
			if (arg & TUN_F_TSO4)
				features |= NETIF_F_TSO;
			if (arg & TUN_F_TSO6)
				features |= NETIF_F_TSO6;
			arg &= ~(TUN_F_TSO4|TUN_F_TSO6);
		}

		if (arg & TUN_F_UFO) {
			features |= NETIF_F_UFO;
			arg &= ~TUN_F_UFO;
		}
	}

	/* This gives the user a way to test for new features in future by
	 * trying to set them. */
	if (arg)
		return -EINVAL;

	tun->set_features = features;
	netdev_update_features(tun->dev);

	return 0;
}

#ifdef CONFIG_ARUBA_AP
/*
 * init_xfrm - build a xfrm for encryption or decryption.  This function may
 *             need to change from time to time based on linux kernel releases.
 *             Please compare to xfrm_user.c to see exactly which bits need
 *             to be modified.  There's discussion of changing the mode and
 *             flags in the structure.  We don't use the flags as they
 *             currently don't affect anything in transport mode which may
 *             change in the future.
 *             Returns xfrm is success or NULL if failed.
 */
static struct xfrm_state *
init_xfrm(
	ip_addr_t saddr, ip_addr_t daddr, u16 srcport, u16 dstport, int alg, u8 *cipherkey, 
	u8 *hashkey, u32 spi
)
{
    struct xfrm_state     *x = xfrm_state_alloc(&init_net);
    int                    len=0, klen;
    int			   ret;
    int                    is_v4;

    if (!x) {
        printk("%s:%s:%d init_xfrm out of memory\n",
               __FILE__, __FUNCTION__, __LINE__);
        goto err;
    }
    if (IS_V4(saddr)) {
        is_v4 = 1;
    } else if (IS_V6(saddr)) {
        is_v4 = 0;
    } else {
        printk("%s:%s:%d Invalid saddr family!\n",
               __FILE__, __FUNCTION__, __LINE__);
        return NULL;
    }
    /* fill in props */
    x->props.reqid  = 0; /* not used */

    /*
     * note that we used to use transport mode, but tighter checks in
     * xfrm_init_state() preclude that, so we ask for tunnel mode now;
     * it's academic, since we control both the encap and decap ourselves
     */
    x->props.mode   = XFRM_MODE_TUNNEL;
    x->props.replay_window = 0; /* doesn't matter on trans */
    x->props.aalgo  = SADB_AALG_SHA1HMAC;
    // ealgo done below
    x->props.flags  = 0; /* nothing for transport */
    if (is_v4) {
        x->props.family = AF_INET;
        x->props.saddr.a4 = IPV4(saddr);
    } else {
        x->props.family = AF_INET6;
        memcpy(x->props.saddr.a6, IPV6(saddr), IPV6_SIZE);
    }
#ifdef CONFIG_OCTEON_NATIVE_IPSEC
    // cavium crypto init needs this to be set, see get_esp_fn_name()
    if (is_v4) {
        x->id.daddr.a4 = IPV4(daddr);
    } else {
       memcpy(x->id.daddr.a6, IPV6(daddr), IPV6_SIZE);
    }
#endif

    /* fill in proto */
    x->id.proto = IPPROTO_ESP;
    x->id.spi   = spi;

    // xfrm_selector...we assume it's not needed here
    
    /* ealgo */
    if (0 == alg) {                         // IPSEC_ENCALG_ANY
        x->props.ealgo = SADB_EALG_NULL;
        klen = 0;
    } else if (2 == alg) {                  // IPSEC_ENCALG_3DES
        x->props.ealgo = SADB_EALG_3DESCBC;
        klen = 24;
    } else {                                // IPSEC_ENCALG_AES
        x->props.ealgo = SADB_X_EALG_AESCBC;
        klen = 32;
    }
                
    /*  encryption alg */
    len = sizeof(*(x->ealg))+klen+16;
    x->ealg = kzalloc(len, GFP_KERNEL);
    if (x->ealg == NULL) {
        printk("%s:%d kmalloc of ealg failed.  Out of memory?\n",
               __FILE__, __LINE__);
        goto err;
    }
    /* we only support 3DES w/ HMAC-SHA1 or AES256 w/ HMAC-SHA1
       or NULL w/ HMAC-SHA1 */
    if (0 == alg) {                             // IPSEC_ENCALG_ANY
        memcpy(x->ealg->alg_name, "cipher_null", 12);
    } else if (2 == alg) {                      // IPSEC_ENCALG_3DES
        strlcpy(x->ealg->alg_name, "cbc(des3_ede)", sizeof(x->ealg->alg_name));
    } else {                                    // IPSEC_ENCALG_AES
        strlcpy(x->ealg->alg_name, "cbc(aes)",  sizeof(x->ealg->alg_name));
    }

    x->ealg->alg_key_len = klen*8; // in bits
    memcpy(x->ealg->alg_key, cipherkey, klen);
    
    /* hash alg, always assume HMAC-SHA1 which is 20B */
    len = sizeof(*(x->aalg))+20+16;
    x->aalg = kzalloc(len, GFP_KERNEL);
    
    if (x->aalg == NULL) {
        printk("%s:%d kmalloc of aalg failed, out of memory?\n",
               __FILE__, __LINE__);
        goto err;
    }
    strlcpy(x->aalg->alg_name, "hmac(sha1)", sizeof(x->aalg->alg_name));
    x->aalg->alg_key_len = 20*8; // in bits
    /* new for 2.6.35 */
    x->aalg->alg_trunc_len = 96;	/* assumes HMAC(SHA-1) only */
    memcpy(x->aalg->alg_key, hashkey, 20);

    /* note that xfrm_init_state calls xfrm type init_state for us. */
    ret = xfrm_init_state(x);
    if (ret != 0) {
        printk("xfrm_init_state failed, %d\n", ret);
	goto err;
    }

    /*
     * allocate encap after initializing state, due to constraints in tunnel 
     * stuff (it will fail initialization if x->encap is set)
     */
    /* fill in encap tmpl */
    x->encap = kzalloc(sizeof(*(x->encap)), GFP_KERNEL);
    if (x->encap == NULL) {
        printk("%s:%d kmalloc of xfrm encap failed.  Out of memory?\n",
               __FILE__, __LINE__);
	goto err;
    }
    if (is_v4) {
        x->encap->encap_type  = UDP_ENCAP_ESPINUDP;
    }
    x->encap->encap_sport = srcport;
    x->encap->encap_dport = dstport;

    return x;

err:
    if (x) {
	x->km.state = XFRM_STATE_DEAD; 
	xfrm_state_put(x);
    }
    return NULL;
}
#endif
static long __tun_chr_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg, int ifreq_len)
{
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun;
	void __user* argp = (void __user*)arg;
	struct sock_fprog fprog;
	struct ifreq ifr;
	int sndbuf;
	int vnet_hdr_sz;
	int ret;
#ifdef CONFIG_ARUBA_AP
    ipseckeys_t  tmpk;
    l2tpinfo_t   tmpl;
    cryptotun_t *ipsec;
    char         tmpname[sizeof(ipsec->dev_name)];
    int          val;
#endif
#ifdef CONFIG_ANDROID_PARANOID_NETWORK
	if (cmd != TUNGETIFF && !capable(CAP_NET_ADMIN)) {
		return -EPERM;
	}
#endif

	if (cmd == TUNSETIFF || _IOC_TYPE(cmd) == 0x89) {
		if (copy_from_user(&ifr, argp, ifreq_len))
			return -EFAULT;
	} else {
		memset(&ifr, 0, sizeof(ifr));
	}
	if (cmd == TUNGETFEATURES) {
		/* Currently this just means: "what IFF flags are valid?".
		 * This is needed because we never checked for invalid flags on
		 * TUNSETIFF. */
		return put_user(IFF_TUN | IFF_TAP | IFF_NO_PI | IFF_ONE_QUEUE |
				IFF_VNET_HDR,
				(unsigned int __user*)argp);
	}

	rtnl_lock();

	tun = __tun_get(tfile);

#ifdef CONFIG_ARUBA_AP
        if (tun) {
	    ipsec = (cryptotun_t *)(tun+1);
            switch (cmd) {
            case SIOCDEVSINGLECRYPT:
                if (copy_from_user(&val, argp, sizeof val)) {
                    ret = -EFAULT;
		    goto unlock;
		}
                
                ipsec->allowclear = val;
#if 0
                if (ipsec->allowclear)
                printk("%s single-encrypt on\n", __FUNCTION__);
                else
                printk("%s double-encrypt off\n", __FUNCTION__);
#endif
		ret = 0;
                goto unlock;
            case SIOCDEVIPSECKEYS:
    
                if (copy_from_user(&tmpk, argp, sizeof(tmpk))) {
                    ret = -EFAULT;
		    goto unlock;
		}
                /* TODO: sanitize values? */
                memcpy(&ipsec->k, &tmpk, sizeof(ipsec->k));
    
#if 0
                printk("Got new keys, srcNatt=%d, dstNatt=%d\n",
                       tmpk.srcNatt, tmpk.dstNatt);
#endif
    
#ifndef FORCE_USERSPACE
                xfrm_probe_algs();
#ifndef FORCEENCRYPTUSER
                if (ipsec->out_xfrm) {
                    ipsec->out_xfrm->km.state = XFRM_STATE_DEAD;
                    xfrm_state_put(ipsec->out_xfrm);
                    ipsec->out_xfrm = NULL;
                }
                ipsec->out_xfrm = init_xfrm(ipsec->k.srcIP, ipsec->k.dstIP,
                                            ipsec->k.srcNatt, ipsec->k.dstNatt,
                                            ipsec->k.alg,
                                            ipsec->k.outkey, ipsec->k.outhash,
                                            ipsec->k.outspi);
                if (ipsec->out_xfrm == NULL) {
                    printk("%s:%d init_xfrm failed for encryption xfrm.  Encryption done in user space instead.  Check kernel compilation config.\n", __FILE__, __LINE__);
                    ret = -ENOTTY;
		    goto unlock;
                }
#endif /* FORCE_ENCRYPTUSER */
    
#ifndef FORCE_DECRYPTUSER
                spin_lock_bh(&tun->lock);
                if (ipsec->in_xfrm) {
                    ipsec->in_xfrm->km.state = XFRM_STATE_DEAD;
                    xfrm_state_put(ipsec->in_xfrm);
                    ipsec->in_xfrm = NULL;
                }
                spin_unlock_bh(&tun->lock);
                    ipsec->in_xfrm = init_xfrm(ipsec->k.dstIP, ipsec->k.srcIP,
                                               ipsec->k.dstNatt, ipsec->k.srcNatt,
                                               ipsec->k.alg,
                                               ipsec->k.inkey, ipsec->k.inhash,
                                               ipsec->k.inspi);
                    
                    if (ipsec->in_xfrm == NULL) {
                        printk("%s:%d init_xfrm failed for decryption xfrm.  Encryption done in user space instead.  Check kernel compilation config.\n", __FILE__, __LINE__);
                        if (ipsec->out_xfrm) {
                            ipsec->out_xfrm->km.state = XFRM_STATE_DEAD;
                            xfrm_state_put(ipsec->out_xfrm);
                            ipsec->out_xfrm = NULL;
                        }
                        ret = -ENOTTY;
			goto unlock;
                    }
#endif /* FORCE_DECRYPTUSER*/
                
#endif /* FORCE_USERSPACE */
		ret = 0;
                goto unlock;
            case SIOCDEVL2TP:
                if (copy_from_user(&tmpl, argp, sizeof(tmpl))) {
                    ret = -EFAULT;
		    goto unlock;
		}
                /* TODO: sanitize values? */
                memcpy(&ipsec->l, &tmpl, sizeof(ipsec->l));
		ret = 0;
		goto unlock;
            case SIOCDEVSET:
                if (copy_from_user(tmpname, argp, sizeof(tmpname))) {
                    ret = -EFAULT;
		    goto unlock;
		}
                /* NULL terminate */
                tmpname[sizeof(tmpname)-1] = '\0';
                
                // Taylor: please remove this when you're done...
                if (strcmp(tmpname, ipsec->dev_name) != 0) {
                    struct net_device *newdev = dev_get_by_name(&init_net, tmpname);
                    if (newdev) {
                        strcpy(ipsec->dev_name, tmpname);
                        if (ipsec->output_dev) {
                            dev_put(ipsec->output_dev);
                        }
                        ipsec->output_dev = newdev;
                    } else {
                        ret = -EFAULT;
			goto unlock;
                    }
                }
		ret = 0;
                goto unlock;
            }
        }
#endif
	if (cmd == TUNSETIFF && !tun) {
		ifr.ifr_name[IFNAMSIZ-1] = '\0';

		ret = tun_set_iff(tfile->net, file, &ifr);

		if (ret)
			goto unlock;

		if (copy_to_user(argp, &ifr, ifreq_len))
			ret = -EFAULT;
		goto unlock;
	}

	ret = -EBADFD;
	if (!tun)
		goto unlock;

	tun_debug(KERN_INFO, tun, "tun_chr_ioctl cmd %d\n", cmd);

	ret = 0;
	switch (cmd) {
	case TUNGETIFF:
		ret = tun_get_iff(current->nsproxy->net_ns, tun, &ifr);
		if (ret)
			break;

		if (copy_to_user(argp, &ifr, ifreq_len))
			ret = -EFAULT;
		break;

	case TUNSETNOCSUM:
		/* Disable/Enable checksum */

		/* [unimplemented] */
		tun_debug(KERN_INFO, tun, "ignored: set checksum %s\n",
			  arg ? "disabled" : "enabled");
		break;

	case TUNSETPERSIST:
		/* Disable/Enable persist mode */
		if (arg)
			tun->flags |= TUN_PERSIST;
		else
			tun->flags &= ~TUN_PERSIST;

		tun_debug(KERN_INFO, tun, "persist %s\n",
			  arg ? "enabled" : "disabled");
		break;

	case TUNSETOWNER:
		/* Set owner of the device */
		tun->owner = (uid_t) arg;

		tun_debug(KERN_INFO, tun, "owner set to %d\n", tun->owner);
		break;

	case TUNSETGROUP:
		/* Set group of the device */
		tun->group= (gid_t) arg;

		tun_debug(KERN_INFO, tun, "group set to %d\n", tun->group);
		break;

	case TUNSETLINK:
		/* Only allow setting the type when the interface is down */
		if (tun->dev->flags & IFF_UP) {
			tun_debug(KERN_INFO, tun,
				  "Linktype set failed because interface is up\n");
			ret = -EBUSY;
		} else {
			tun->dev->type = (int) arg;
			tun_debug(KERN_INFO, tun, "linktype set to %d\n",
				  tun->dev->type);
			ret = 0;
		}
		break;

#ifdef TUN_DEBUG
	case TUNSETDEBUG:
		tun->debug = arg;
		break;
#endif
	case TUNSETOFFLOAD:
		ret = set_offload(tun, arg);
		break;

	case TUNSETTXFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		ret = update_filter(&tun->txflt, (void __user *)arg);
		break;

	case SIOCGIFHWADDR:
		/* Get hw address */
		memcpy(ifr.ifr_hwaddr.sa_data, tun->dev->dev_addr, ETH_ALEN);
		ifr.ifr_hwaddr.sa_family = tun->dev->type;
		if (copy_to_user(argp, &ifr, ifreq_len))
			ret = -EFAULT;
		break;

	case SIOCSIFHWADDR:
		/* Set hw address */
		tun_debug(KERN_DEBUG, tun, "set hw address: %pM\n",
			  ifr.ifr_hwaddr.sa_data);

		ret = dev_set_mac_address(tun->dev, &ifr.ifr_hwaddr);
		break;

	case TUNGETSNDBUF:
		sndbuf = tun->socket.sk->sk_sndbuf;
		if (copy_to_user(argp, &sndbuf, sizeof(sndbuf)))
			ret = -EFAULT;
		break;

	case TUNSETSNDBUF:
		if (copy_from_user(&sndbuf, argp, sizeof(sndbuf))) {
			ret = -EFAULT;
			break;
		}

		tun->socket.sk->sk_sndbuf = sndbuf;
		break;

	case TUNGETVNETHDRSZ:
		vnet_hdr_sz = tun->vnet_hdr_sz;
		if (copy_to_user(argp, &vnet_hdr_sz, sizeof(vnet_hdr_sz)))
			ret = -EFAULT;
		break;

	case TUNSETVNETHDRSZ:
		if (copy_from_user(&vnet_hdr_sz, argp, sizeof(vnet_hdr_sz))) {
			ret = -EFAULT;
			break;
		}
		if (vnet_hdr_sz < (int)sizeof(struct virtio_net_hdr)) {
			ret = -EINVAL;
			break;
		}

		tun->vnet_hdr_sz = vnet_hdr_sz;
		break;

	case TUNATTACHFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		ret = -EFAULT;
		if (copy_from_user(&fprog, argp, sizeof(fprog)))
			break;

		ret = sk_attach_filter(&fprog, tun->socket.sk);
		break;

	case TUNDETACHFILTER:
		/* Can be set only for TAPs */
		ret = -EINVAL;
		if ((tun->flags & TUN_TYPE_MASK) != TUN_TAP_DEV)
			break;
		ret = sk_detach_filter(tun->socket.sk);
		break;

	default:
		ret = -EINVAL;
		break;
	}

unlock:
	rtnl_unlock();
	if (tun)
		tun_put(tun);
	return ret;
}

static long tun_chr_ioctl(struct file *file,
			  unsigned int cmd, unsigned long arg)
{
	return __tun_chr_ioctl(file, cmd, arg, sizeof (struct ifreq));
}

#ifdef CONFIG_COMPAT
static long tun_chr_compat_ioctl(struct file *file,
			 unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case TUNSETIFF:
	case TUNGETIFF:
	case TUNSETTXFILTER:
	case TUNGETSNDBUF:
	case TUNSETSNDBUF:
	case SIOCGIFHWADDR:
	case SIOCSIFHWADDR:
		arg = (unsigned long)compat_ptr(arg);
		break;
	default:
		arg = (compat_ulong_t)arg;
		break;
	}

	/*
	 * compat_ifreq is shorter than ifreq, so we must not access beyond
	 * the end of that structure. All fields that are used in this
	 * driver are compatible though, we don't need to convert the
	 * contents.
	 */
	return __tun_chr_ioctl(file, cmd, arg, sizeof(struct compat_ifreq));
}
#endif /* CONFIG_COMPAT */

#ifdef CONFIG_64BIT
static long tun_chr_ioctl_compat(struct file *file, 
			 unsigned int cmd, unsigned long arg)
{
	return (long)tun_chr_ioctl(0, file, cmd, arg);
}
#endif

static int tun_chr_fasync(int fd, struct file *file, int on)
{
	struct tun_struct *tun = tun_get(file);
	int ret;

	if (!tun)
		return -EBADFD;

	tun_debug(KERN_INFO, tun, "tun_chr_fasync %d\n", on);

	if ((ret = fasync_helper(fd, file, on, &tun->fasync)) < 0)
		goto out;

	if (on) {
		ret = __f_setown(file, task_pid(current), PIDTYPE_PID, 0);
		if (ret)
			goto out;
		tun->flags |= TUN_FASYNC;
	} else
		tun->flags &= ~TUN_FASYNC;
	ret = 0;
out:
	tun_put(tun);
	return ret;
}

static int tun_chr_open(struct inode *inode, struct file * file)
{
	struct tun_file *tfile;

	DBG1(KERN_INFO, "tunX: tun_chr_open\n");

	tfile = kmalloc(sizeof(*tfile), GFP_KERNEL);
	if (!tfile)
		return -ENOMEM;
	atomic_set(&tfile->count, 0);
	tfile->tun = NULL;
	tfile->net = get_net(current->nsproxy->net_ns);
	file->private_data = tfile;
	return 0;
}

static int tun_chr_close(struct inode *inode, struct file *file)
{
	struct tun_file *tfile = file->private_data;
	struct tun_struct *tun;

	tun = __tun_get(tfile);
	if (tun) {
		struct net_device *dev = tun->dev;

		tun_debug(KERN_INFO, tun, "tun_chr_close\n");
#ifdef CONFIG_ARUBA_AP
        /* Before detaching, wait for others to release the count.
         * refcnt 2: one for tun_attach, one for __tun_get. */
        while (atomic_read(&tfile->count) > 2)
            ;
#endif
		__tun_detach(tun);

		/* If desirable, unregister the netdevice. */
		if (!(tun->flags & TUN_PERSIST)) {
			rtnl_lock();
			if (dev->reg_state == NETREG_REGISTERED)
				unregister_netdevice(dev);
			rtnl_unlock();
		}
	}

	tun = tfile->tun;
	if (tun)
		sock_put(tun->socket.sk);

	put_net(tfile->net);
	kfree(tfile);

	return 0;
}

static const struct file_operations tun_fops = {
	.owner	= THIS_MODULE,
	.llseek = no_llseek,
	.read  = do_sync_read,
	.aio_read  = tun_chr_aio_read,
	.write = do_sync_write,
	.aio_write = tun_chr_aio_write,
	.poll	= tun_chr_poll,
	.unlocked_ioctl	= tun_chr_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = tun_chr_compat_ioctl,
#endif
	.open	= tun_chr_open,
	.release = tun_chr_close,
	.fasync = tun_chr_fasync
};

static struct miscdevice tun_miscdev = {
	.minor = TUN_MINOR,
	.name = "tun",
	.nodename = "net/tun",
	.fops = &tun_fops,
};

/* ethtool interface */

static int tun_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	cmd->supported		= 0;
	cmd->advertising	= 0;
	ethtool_cmd_speed_set(cmd, SPEED_10);
	cmd->duplex		= DUPLEX_FULL;
	cmd->port		= PORT_TP;
	cmd->phy_address	= 0;
	cmd->transceiver	= XCVR_INTERNAL;
	cmd->autoneg		= AUTONEG_DISABLE;
	cmd->maxtxpkt		= 0;
	cmd->maxrxpkt		= 0;
	return 0;
}

static void tun_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct tun_struct *tun = netdev_priv(dev);

	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_VERSION, sizeof(info->version));

	switch (tun->flags & TUN_TYPE_MASK) {
	case TUN_TUN_DEV:
		strlcpy(info->bus_info, "tun", sizeof(info->bus_info));
		break;
	case TUN_TAP_DEV:
		strlcpy(info->bus_info, "tap", sizeof(info->bus_info));
		break;
	}
}

static u32 tun_get_msglevel(struct net_device *dev)
{
#ifdef TUN_DEBUG
	struct tun_struct *tun = netdev_priv(dev);
	return tun->debug;
#else
	return -EOPNOTSUPP;
#endif
}

static void tun_set_msglevel(struct net_device *dev, u32 value)
{
#ifdef TUN_DEBUG
	struct tun_struct *tun = netdev_priv(dev);
	tun->debug = value;
#endif
}

static const struct ethtool_ops tun_ethtool_ops = {
	.get_settings	= tun_get_settings,
	.get_drvinfo	= tun_get_drvinfo,
	.get_msglevel	= tun_get_msglevel,
	.set_msglevel	= tun_set_msglevel,
	.get_link	= ethtool_op_get_link,
};


static int __init tun_init(void)
{
	int ret = 0;
#ifdef CONFIG_ARUBA_DEBUG
	pr_info("%s, %s\n", DRV_DESCRIPTION, DRV_VERSION);
	pr_info("%s\n", DRV_COPYRIGHT);
#endif
	ret = rtnl_link_register(&tun_link_ops);
	if (ret) {
		pr_err("Can't register link_ops\n");
		goto err_linkops;
	}

	ret = misc_register(&tun_miscdev);
	if (ret) {
		pr_err("Can't register misc device %d\n", TUN_MINOR);
		goto err_misc;
	}
	return  0;
err_misc:
	rtnl_link_unregister(&tun_link_ops);
err_linkops:
	return ret;
}

static void tun_cleanup(void)
{
#ifdef CONFIG_ARUBA_AP
    struct tun_struct *tun, *nxt;
#endif
	misc_deregister(&tun_miscdev);
#ifdef CONFIG_ARUBA_AP
	rtnl_lock();
	spin_lock_bh(&tun_list_lock);
	list_for_each_entry_safe(tun, nxt, &tun_dev_list, list) {
#ifdef CONFIG_ARUBA_DEBUG
		printk(KERN_INFO "%s cleaned up\n", tun->dev->name);
#endif
#ifdef DBG_RTNL_LOCK
		printk("%s:%s:%d unregistering netdevice\n",
		       __FILE__, __FUNCTION__, __LINE__);
#endif
//		unregister_netdevice(tun->dev);
	}
	spin_unlock_bh(&tun_list_lock);
	rtnl_unlock();
#endif
	rtnl_link_unregister(&tun_link_ops);
}

/* Get an underlying socket object from tun file.  Returns error unless file is
 * attached to a device.  The returned object works like a packet socket, it
 * can be used for sock_sendmsg/sock_recvmsg.  The caller is responsible for
 * holding a reference to the file for as long as the socket is in use. */
struct socket *tun_get_socket(struct file *file)
{
	struct tun_struct *tun;
	if (file->f_op != &tun_fops)
		return ERR_PTR(-EINVAL);
	tun = tun_get(file);
	if (!tun)
		return ERR_PTR(-EBADFD);
	tun_put(tun);
	return &tun->socket;
}
EXPORT_SYMBOL_GPL(tun_get_socket);

#if defined(CONFIG_ARUBA_AP) && !defined(CONFIG_GRENACHE) && !defined(CONFIG_ARUBA_CONTROLLER)
void asap_ipsec_mirror_packet_to_dev(struct sk_buff *skb, int xmit_pkt)
{
    struct sk_buff *skbc;

    if (!mirror_ipsec || !mirror_dev) {
        return;
    }
    skbc = skb_copy(skb, GFP_ATOMIC);
    if (!skbc) {
        printk(KERN_ALERT "Low on memory, skb_copy failure. Line %d\n", __LINE__);
        return;
    }
    if (skb_headroom(skbc) < ETH_HLEN) {
        struct sk_buff *new_skb = skb_realloc_headroom(skbc, ETH_HLEN);
        if (new_skb) {
            dev_kfree_skb(skbc);
            skbc = new_skb; 
        } else {
            printk(KERN_ALERT "Low on memory, skb_realloc_headroom failure. Line %d\n", __LINE__);
            return;
        }
    }
    skb_push(skbc, ETH_HLEN);
    skbc->network_header = skbc->data;
    if (xmit_pkt) {
        memset(skbc->network_header, 0xff, 6);
        memset(&skbc->network_header[6], 0, 6);
    } else {
        memset(skbc->network_header, 0, 6);
        memset(&skbc->network_header[6], 0xff, 6);
    }
    if (skbc->protocol == htons(ETH_P_IP)) {
        skbc->network_header[12] = 0x08;
        skbc->network_header[13] = 0x00;
    } else {
        skbc->network_header[12] = 0x86;
        skbc->network_header[13] = 0xDD;
    }

    skbc->tail = skbc->data + skbc->len;

    skbc->dev = mirror_dev;
    dev_queue_xmit(skbc);
    return;
}

void asap_port_mirror_packet_to_dev(struct sk_buff *skb, int xmit_pkt, int is_eth)
{
    struct sk_buff *skbc;

    if (!mirror_port || !mirror_dev) {
        return;
    }
    skbc = skb_copy(skb, GFP_ATOMIC);
    if (!skbc) {
        printk(KERN_ALERT "Low on memory, skb_copy failure. Line %d\n", __LINE__);
        return;
    }
    if (((!xmit_pkt) && (is_eth)) || ((xmit_pkt) && (!is_eth))) {
        if (skb_headroom(skbc) < ETH_HLEN) {
            struct sk_buff *new_skb = skb_realloc_headroom(skbc, ETH_HLEN);
            if (new_skb) {
                dev_kfree_skb(skbc);
                skbc = new_skb; 
            } else {
                printk(KERN_ALERT "Low on memory, skb_realloc_headroom failure. Line %d\n", __LINE__);
                return;
            }
        }
        skb_push(skbc, ETH_HLEN);
    }
    skbc->network_header = skbc->data;
    skbc->tail = skbc->data + skbc->len;

    skbc->dev = mirror_dev;
    dev_queue_xmit(skbc);
    return;
}
#endif

module_init(tun_init);
module_exit(tun_cleanup);
MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR(DRV_COPYRIGHT);
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(TUN_MINOR);
MODULE_ALIAS("devname:net/tun");
