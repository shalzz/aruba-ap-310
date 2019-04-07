#ifndef __SKB_THROTTLE_H__
#define __SKB_THROTTLE_H__

/*
 * Description: generic throttling that drops packets received on a given
 * interface when the outstanding packet count reaches a certain level. When
 * the count returns to the original ring-buffer size, no more packets are
 * dropped. Control packets are never dropped and priority packets dropped 
 * preferentially. When load is high kernel watchdog refresh is enabled.
 *
 * Operation:
 * rxcnt == skbthr_rx_ringsz + skbthr_delta/3: do kernel watchdog refresh.
 * rxcnt >= skbthr_rx_ringsz+skbthr_delta: drop low prio packets.
 * rxcnt >= skbthr_rx_ringsz+(skbthr_delta*6/5): drop all but control packets.
 * rxcnt <= skbthr_rx_ringsz: stop kernel watchdog refresh.
 * 
 * Each throttled netdevice has it's own boolean, skbthr_do_krefresh, to track
 * if its load is enough to require kernel watchdog refresh. The global
 * watchdog ref-counter, skbthr_kwdog_refcnt, is incremented when set and
 * decremented when unset.
 *
 * Do the following to add support to a driver (e.g. drivers/net/ar2313.c):
 *     1) Call skbthr_init_netdev() soon after the device is allocated from 
 *        alloc_netdev/alloc_etherdev. Arg3, throttle_delta should be selected carefully:
 *        large enough to allow full speed, but not so large as to make OOM likely.
 *     2) Whenever an skb has been allocated, including filling the ring-buffer,
 *        increment the count of outstanding packets by calling skbthr_inc_rxcnt().
 *        The refcnt is decremented in skbuff.c:__kfree_skb(), which calls skbthr_dec_rxcnt().
 *     3) When a packet is received and before calling dev_alloc_skb(), call skbthr_rxdrop() 
 *        with suitable priority-check function. If TRUE: 
 *        (a) As appropriate either do dev_kfree_skb(), or restore the skb from the ring-buffer 
 *        for reallocation (skb->tail = skb->data = skb->head; skb->len = 0; etc.), and
 *        (b) Increment the relevant rx_dropped stats field for display in /proc/net/dev.
 *
 * Debug: 
 * Throttling counter:, use sysctl -w net.throttle.throttle_debug=1 [0 to disable.]
 * Watchdog uses KERN_DEBUG: echo 8 > /proc/sys/kernel/printk [7 to disable].
 * /proc/net/dev: displays net_device_stats  for each device including rx_dropped count.
 *
 */

#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <asm/atomic.h>

#if defined(CONFIG_ARUBA_AP)
#define SKB_THROTTLE_ACTIVE 1

extern atomic_t *skbthr_kwdog_refcnt; // The relevant watchdog must define this.
extern int skbthr_debug;

typedef enum {
    SKBTHR_LOWPRIO=0, // MUST be ZERO.
    SKBTHR_VIVOPRIO=1, 
    SKBTHR_CTRLPRIO=2, // MUST be 2.
} skbthr_prio_t;
typedef int (*skbthr_is_prio_f)(u8 *skbdata);

extern void skbthr_sysctl_attach(void);
extern void skbthr_sysctl_detach(void);

// Debug macro to periodically print cntr with msg every modv messages
#define SKBTHR__DEBUG(...) do {                                   \
        if(!skbthr_debug) break;                                  \
        printk(__VA_ARGS__);                                      \
    } while (0)

#define SKBTHR_PKT_COUNTER(cntr, modv, msg) do {         \
        static unsigned int cntr = 0;                                    \
        if(!skbthr_debug) break;                                \
        if(0 == cntr%modv) {                                    \
            printk("%s: %s: %d\n", __func__, msg, cntr+1);      \
        }                                                       \
        ++cntr;                                                 \
    } while(0);

static __inline int skbthr_is_ethprio(u8 *skbdata)
{
    struct ethhdr *ethh = (struct ethhdr *)skbdata;
    struct iphdr *iph=(struct iphdr *)(skbdata+sizeof(struct ethhdr));
    int rval=SKBTHR_LOWPRIO;

    if (ethh->h_proto == ETH_P_ARP) {
        rval=SKBTHR_CTRLPRIO;
    } else if (iph->protocol == IPPROTO_GRE) {
        u16 eth_proto = ntohs(*((u16 *)(skbdata+14+20+4+12))); //eth+ip+gre+(eth mac)
        if (eth_proto == 0x8211) {
            u16 typ = ntohs(*((u16 *)(skbdata+14+20+4+14))); //eth+ip+gre+(eth hdr)
            if (typ == 5) { //WLAN_FC_STYPE_SAPCP_HEARTBEAT
                SKBTHR_PKT_COUNTER(cntgre, 10, "control_gre");
                rval=SKBTHR_CTRLPRIO;
            }
        }
    } else if (iph->protocol == IPPROTO_UDP) {
        struct udphdr *uh = (struct udphdr *)(skbdata+sizeof(struct ethhdr)+(sizeof(struct iphdr)));
        if ((uh->source == ntohs(8211)) && (uh->dest == ntohs(8211))) { // PAPI-UDP port
            SKBTHR_PKT_COUNTER(cntpapi, 10, "control_papi");
            rval=SKBTHR_CTRLPRIO;
        }
    }
    if(!rval) { // TODO: figure out this "magic" copied from asap_firewall:wmm_classify()
        switch  (iph->tos){
        case 0x28:				/* Video */
        case 0xa0:
            rval = SKBTHR_VIVOPRIO;
          break;
        case 0x30:				/* Voice */
        case 0xe0:
        case 0x88:				/* XXX UPSD */
        case 0xb8:
            rval = SKBTHR_VIVOPRIO;
          break;
        default:
          break;
        }
        if(rval) {
            SKBTHR_PKT_COUNTER(cnttosp, 200, "iph->tos-vivo-priority");
        }
    }
    return rval;
}

static __inline int skbthr_rxdrop(struct net_device *dev, struct sk_buff *skb, skbthr_is_prio_f isprio_f)
{
    if(likely(dev->skbthr_delta)) {
        if(likely(skb->rxdev)) {
            int rxcnt = atomic_read(&dev->skbthr_rxcnt);
            if(unlikely(rxcnt >= (dev->skbthr_rx_ringsz+dev->skbthr_delta))) {
                int prio = (*isprio_f)(skb->data);
                if(likely(SKBTHR_LOWPRIO == prio)) {
                    SKBTHR_PKT_COUNTER(rxdrop, (20*(dev->skbthr_rx_ringsz+dev->skbthr_delta)), "lowprio-drop"); 
                    return 1;
                } else if (SKBTHR_VIVOPRIO == prio && (rxcnt >= (dev->skbthr_rx_ringsz+(dev->skbthr_delta*6)/5))) {
                    SKBTHR_PKT_COUNTER(prio_drop, dev->skbthr_rx_ringsz, "prio-drop");
                    return 1;
                } else {
                    SKBTHR_PKT_COUNTER(ctrl_nodrop, dev->skbthr_rx_ringsz, "ctrl-nodrop");
                }
            }
        } /*else {
            SKBTHR_PKT_COUNTER(rxdrop, 8*dev->skbthr_delta, "nonrx-nodrop");
            } */
    }
    return 0;
}

static __inline void skbthr_inc_rxcnt(struct sk_buff *skb, struct net_device *dev) 
{              
    if(unlikely(!dev->skbthr_delta)) // If delta==0, then throttling not in effect.
        return;

    skb->rxdev = dev;  
    if(unlikely((atomic_inc_return(&skb->rxdev->skbthr_rxcnt) == 
                 (skb->rxdev->skbthr_rx_ringsz + skb->rxdev->skbthr_delta/3)))) {
        if(!atomic_read(&skb->rxdev->skbthr_do_krefresh)) {
            atomic_set(&skb->rxdev->skbthr_do_krefresh, 1);
            atomic_inc(skbthr_kwdog_refcnt);
            SKBTHR__DEBUG("%s: dev: %s, skbthr_kwdog_refcnt: %d, rxcnt: %d\n", 
                   __func__, skb->rxdev->name, atomic_read(skbthr_kwdog_refcnt), 
                   atomic_read(&skb->rxdev->skbthr_rxcnt));
        }
    }             
} 

static __inline void skbthr_dec_rxcnt(struct sk_buff *skb)
{
    BUG_ON(!skb->rxdev);
    if(unlikely((atomic_dec_return(&skb->rxdev->skbthr_rxcnt) <0))) {
        SKBTHR_PKT_COUNTER(nvecntr, 500, "dec -> <0"); 
        atomic_set(&skb->rxdev->skbthr_rxcnt, 0);
    }
    if((atomic_read(&skb->rxdev->skbthr_rxcnt) <= (skb->rxdev->skbthr_rx_ringsz))) {
        if(atomic_read(&skb->rxdev->skbthr_do_krefresh)) {
            atomic_set(&skb->rxdev->skbthr_do_krefresh, 0);
            if(unlikely(atomic_dec_return(skbthr_kwdog_refcnt) <0)) {
                atomic_set(skbthr_kwdog_refcnt, 0);
            }
            SKBTHR__DEBUG("%s: dev: %s, skbthr_kwdog_refcnt: %d, rxcnt: %d\n", 
                   __func__, skb->rxdev->name, atomic_read(skbthr_kwdog_refcnt), 
                   atomic_read(&skb->rxdev->skbthr_rxcnt));
        }
    }
    skb->rxdev = NULL;
}

static __inline void skbthr_init_netdev(struct net_device *dev, int rx_ringsz, unsigned int throttle_delta)
{
    dev->skbthr_rx_ringsz = rx_ringsz;
    dev->skbthr_delta = throttle_delta;
    atomic_set(&dev->skbthr_rxcnt, 0);
    // printk("%s: dev:%s\n", __func__, dev->name);
}
#else
#define skbthr_sysctl_attach()
#define skbthr_sysctl_detach()
#define skbthr_is_ethprio(skbdata) 1
#define skbthr_rxdrop(dev, skb, devtype) 0
#define skbthr_inc_rxcnt(skb, dev)
#define skbthr_dec_rxcnt(skb)
#define skbthr_init_netdev(dev, rx_ringsz, throttle_delta)
#define SKBTHR__DEBUG(...)
#define SKBTHR_PKT_COUNTER(cntr, modv, msg)
#endif // CONFIG_MERLOT

#endif // __SKB_THROTTLE_H__
