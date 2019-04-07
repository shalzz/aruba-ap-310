#ifndef __kernel_skb_compat_h__
#define __kernel_skb_compat_h__

/*
 * Copyright (C) 2002-2013 by Aruba Networks, Inc.
 * All Rights Reserved.
 * 
 * This software is an unpublished work and is protected by copyright and 
 * trade secret law.  Unauthorized copying, redistribution or other use of 
 * this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */

#include <linux/version.h>
#include <linux/skbuff.h>


#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) )
typedef unsigned char *sk_buff_data_t;

/*
 * pre-2.6.22
 */

/* sk_buff */
#define skb_dst(s)              (s)->dst
#define skb_dst_set(s, d)	(s)->dst = (d)
#define udp_hdr(s)        (s)->h.uh

#define SKB_DST_DROP(s)		dst_release(skb_dst((s)))

#define skb_csum_unnecessary(skb) (skb->ip_summed & CHECKSUM_UNNECESSARY)
#ifndef CSUM_MANGLED_0
#define CSUM_MANGLED_0 ((__force __u16)0xffff)
#endif

/* Don't change this without changing skb_csum_unnecessary! */
#define CHECKSUM_COMPLETE 2

#define skb_reset_mac_header(skb) skb->mac.raw = skb->data
#define skb_reset_transport_header(skb)    skb->h.raw = skb->data
#define skb_reset_network_header(skb)      skb->nh.raw = skb->data
#define skb_set_transport_header(skb, off) skb->h.raw = skb->data + off
#define skb_tail_pointer(skb)           skb->tail
#define skb_reset_tail_pointer(skb)     skb->tail = skb->data
#define skb_set_tail_pointer(skb, _off) skb->tail = skb->data + _off
#define skb_end_pointer(skb)            skb->end

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))

/*
 * 2.6.22 - 2.6.29
 */

/* sk_buff */
#define __skb_append(o,n,l)	__skb_queue_after(l,o,n)
#define skb_dst(s)              (s)->dst
#define skb_dst_set(s, d)	(s)->dst = (d)

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
/*
 * 2.6.30 - 2.6.34
 */

#define __skb_append(o,n,l)	__skb_queue_after(l,o,n)
#define SKB_DST_DROP(s)		dst_release(skb_dst((s)))

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
/*
 * 2.6.35+
 */

/* sk_buff */
#define __skb_append(o,n,l)	__skb_queue_after(l,o,n)
#define SKB_DST_DROP(s)		skb_dst_drop((s))

#else

/* sk_buff */
#define __skb_append(o,n,l)	__skb_queue_after(l,o,n)
#define SKB_DST_DROP(s)		skb_dst_drop((s))

#endif

#endif
