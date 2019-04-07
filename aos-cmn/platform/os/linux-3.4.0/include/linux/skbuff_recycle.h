/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 *      Definitions for the skb recycler functions
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_SKBUFF_RECYCLE_H
#define _LINUX_SKBUFF_RECYCLE_H

#include <linux/module.h>
#include <linux/types.h>
#include <linux/cpu.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/kmemcheck.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#ifdef CONFIG_NET_CLS_ACT
#include <net/pkt_sched.h>
#endif
#include <linux/string.h>
#include <linux/skbuff.h>
#include <linux/splice.h>
#include <linux/init.h>
#include <linux/prefetch.h>
#include <linux/if.h>


/* Got the best performance for the settings below
 * Free mem is 125MB with 4096 skbs in recycle queue
 * Do cat /proc/net/skb_recycle for stats
 */
#define SKB_RECYCLE_SIZE	2560
#define SKB_RECYCLE_MIN_SIZE	SKB_RECYCLE_SIZE
#define SKB_RECYCLE_MAX_SIZE	8192
#define SKB_RECYCLE_MAX_SKBS	4096

#define SKB_RECYCLE_SPARE_MAX_SKBS		256
#define SKB_RECYCLE_MAX_SHARED_POOLS		8
#define SKB_RECYCLE_MAX_SHARED_POOLS_MASK	(SKB_RECYCLE_MAX_SHARED_POOLS - 1)

struct recycle_stats {
        unsigned long alloc_recycle;
        unsigned long recy_qempty;
        unsigned long recycled;
        unsigned long recy_qfull;
        unsigned long norecy_min;
        unsigned long norecy_max;
        unsigned long norecy_irq;
        unsigned long norecy_frags;
        unsigned long norecy_linear;
        unsigned long norecy_other;
        int sz;
        int norecy_len;
        int max_qlen;
        unsigned long alloc_fail;
        int alloc_fail_sz;
        unsigned long frag_list_recycled;
        unsigned long frag_list_recycled_fail;
        atomic_t recyq_mem;
};

extern struct recycle_stats g_skb_recycle_stats;
#ifdef CONFIG_SKB_RECYCLER_MULTI_CPU
struct global_recycler {
	/* Global circular list which holds the shared skb pools */
	struct sk_buff_head pool[SKB_RECYCLE_MAX_SHARED_POOLS];
	uint8_t head;		/* head of the circular list */
	uint8_t tail;		/* tail of the circular list */
	spinlock_t lock;
};
#endif

static inline void zero_struct(void *v, int size)
{
	uint32_t *s = (uint32_t *)v;

	/* We assume that size is word aligned; in fact, it's constant */
	BUG_ON((size & 3) != 0);

	/*
	 * This looks odd but we "know" size is a constant, and so the
	 * compiler can fold away all of the conditionals.  The compiler is
	 * pretty smart here, and can fold away the loop, too!
	 */
	while (size > 0) {
		if (size >= 4)
			s[0] = 0;
		if (size >= 8)
			s[1] = 0;
		if (size >= 12)
			s[2] = 0;
		if (size >= 16)
			s[3] = 0;
		if (size >= 20)
			s[4] = 0;
		if (size >= 24)
			s[5] = 0;
		if (size >= 28)
			s[6] = 0;
		if (size >= 32)
			s[7] = 0;
		if (size >= 36)
			s[8] = 0;
		if (size >= 40)
			s[9] = 0;
		if (size >= 44)
			s[10] = 0;
		if (size >= 48)
			s[11] = 0;
		if (size >= 52)
			s[12] = 0;
		if (size >= 56)
			s[13] = 0;
		if (size >= 60)
			s[14] = 0;
		if (size >= 64)
			s[15] = 0;
		size -= 64;
		s += 16;
	}
}

static inline bool consume_skb_can_recycle(const struct sk_buff *skb,
					   int min_skb_size, int max_skb_size)
{
        int sz;
        if (unlikely(irqs_disabled())) {
                g_skb_recycle_stats.norecy_irq++;
		return false;
        }

	if (unlikely(skb_shinfo(skb)->tx_flags & SKBTX_DEV_ZEROCOPY)) {
                g_skb_recycle_stats.norecy_other++;
		return false;
        }
        if (skb->destructor)
                return false;
        if (skb_shinfo(skb)->nr_frags) {
                g_skb_recycle_stats.norecy_frags++;
		return false;
        }
	if (unlikely(skb_is_nonlinear(skb))) {
                g_skb_recycle_stats.norecy_linear++;
		return false;
        }
	if (unlikely(skb->fclone != SKB_FCLONE_UNAVAILABLE)) {
                g_skb_recycle_stats.norecy_other++;
		return false;
        }
	min_skb_size = SKB_DATA_ALIGN(min_skb_size + NET_SKB_PAD);
	if (unlikely(skb_end_pointer(skb) - skb->head < min_skb_size)) {
                g_skb_recycle_stats.norecy_min++;
		return false;
        }
	max_skb_size = SKB_DATA_ALIGN(max_skb_size + NET_SKB_PAD);
	if (unlikely((sz = (skb_end_pointer(skb) - skb->head)) > max_skb_size)) {
                g_skb_recycle_stats.sz = sz;
                g_skb_recycle_stats.norecy_len = skb->len;
                g_skb_recycle_stats.norecy_max++;
		return false;
        }
	if (skb->cloned || skb_shared(skb) || skb_cloned(skb) || skb_header_cloned(skb)) {
                g_skb_recycle_stats.norecy_other++;
		return false;
        }
        g_skb_recycle_stats.recycled++;
	return true;
}

#ifdef CONFIG_SKB_RECYCLER

void __init skb_recycler_init(void);
#if defined(CONFIG_ARUBA_SKB_LEAK_DEBUGGING)
struct sk_buff *skb_recycler_alloc(struct net_device *dev, unsigned int length, const char *caller);
#else
struct sk_buff *skb_recycler_alloc(struct net_device *dev, unsigned int length);
#endif
bool skb_recycler_consume(struct sk_buff *skb);

#else

#define skb_recycler_init()  {}
#define skb_recycler_alloc(dev, len) NULL
#define skb_recycler_consume(skb) false

#endif
#endif
