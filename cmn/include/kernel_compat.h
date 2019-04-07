#ifndef __kernel_compat_h__
#define __kernel_compat_h__
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
#include "kernel_skb_compat.h"
#include <linux/inetdevice.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))

/*
 * pre-2.6.22
 */

/* netdevice */
//#define netdev_priv(x)		((x)->priv
#define	DEV_GET_BY_NAME(x)	dev_get_by_name(x)
#define	DEV_GET_BY_INDEX(x)	dev_get_by_index(x)
#define	XFRM_LOOKUP(x, y, z, a, b)	xfrm_lookup(y, z, a, b)
#define	IP6_ROUTE_OUTPUT(x, y, z)	ip6_route_output(y, z)
#define DEV_HAS_HARD_START(d)   ((d)->hard_start_xmit != 0)
#define DEV_HARD_START_XMIT(s, d) (d)->hard_start_xmit(s, d)

/* sysctl */
#define REGISTER_SYSCTL_TABLE(x, y) register_sysctl_table(x, y)

/* proc */
#define NET_NAMESPACE(x)	x

/* net */
#define IP_ROUTE_OUTPUT_KEY(x, y) ip_route_output_key(x, y)

/* notifier */
#define	NOTIFIER_CHAIN_REGISTER		notifier_chain_register
#define	NOTIFIER_CHAIN_UNREGISTER	notifier_chain_unregister
typedef __u32 __bitwise __wsum;

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))

/*
 * 2.6.22 - 2.6.29
 */

/* netdevice */
#define	DEV_GET_BY_NAME(x)	dev_get_by_name(x)
#define	DEV_GET_BY_INDEX(x)	dev_get_by_index(x)
#define DEV_HAS_HARD_START(d)   ((d)->hard_start_xmit != 0)
#define DEV_HARD_START_XMIT(s, d) (d)->hard_start_xmit(s, d)

/* sysctl */
#define REGISTER_SYSCTL_TABLE(x, y) register_sysctl_table(x)

/* proc */
#define NET_NAMESPACE(x)	x

/* net */
#define IP_ROUTE_OUTPUT_KEY(x, y) ip_route_output_key(x, y)

//#define	SET_MODULE_OWNER(x)

/* crypto */
#define crypto_tfm crypto_cipher
#define crypto_alloc_tfm(name,type) crypto_alloc_cipher(name,type, CRYPTO_ALG_ASYNC)
#define crypto_free_tfm(cipher) crypto_free_cipher(cipher)

/* ISR */
#undef SA_SHIRQ
#define SA_SHIRQ IRQF_SHARED

/* RTNL */
#define rtnl_shlock	rtnl_lock
#define rtnl_shunlock	rtnl_unlock

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
/*
 * 2.6.30 - 2.6.34
 */

/* netdevice */
#define	DEV_GET_BY_NAME(x)	dev_get_by_name(&init_net, x)
#define	DEV_GET_BY_INDEX(x)	dev_get_by_index(&init_net, x)
#define	XFRM_LOOKUP(x, y, z, a, b)	xfrm_lookup(x, y, z, a, b)
#define	IP6_ROUTE_OUTPUT(x, y, z)	ip6_route_output(x, y, z)
#define DEV_HAS_HARD_START(d)   ((d)->netdev_ops->ndo_start_xmit != 0)
#define DEV_HARD_START_XMIT(s, d) (d)->netdev_ops->ndo_start_xmit(s, d)

/* sysctl */
#define REGISTER_SYSCTL_TABLE(x, y) register_sysctl_table(x)

/* proc */
#define NET_NAMESPACE(x)	init_net.x

/* net */
#define IP_ROUTE_OUTPUT_KEY(x, y) ip_route_output_key(&init_net, x, y)

//#define	SET_MODULE_OWNER(x)

/* crypto */
#define crypto_tfm crypto_cipher
#define crypto_alloc_tfm(name,type) crypto_alloc_cipher(name,type, CRYPTO_ALG_ASYNC)
#define crypto_free_tfm(cipher) crypto_free_cipher(cipher)

/* ISR */
#define SA_SHIRQ IRQF_SHARED

/* RTNL */
#define rtnl_shlock	rtnl_lock
#define rtnl_shunlock	rtnl_unlock

/* notifier */
#define	NOTIFIER_CHAIN_REGISTER		atomic_notifier_chain_register
#define	NOTIFIER_CHAIN_UNREGISTER	atomic_notifier_chain_unregister

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
/*
 * 2.6.35+
 */

/* netdevice */
#define	DEV_GET_BY_NAME(x)	dev_get_by_name(&init_net, x)
#define	DEV_GET_BY_INDEX(x)	dev_get_by_index(&init_net, x)
#define	XFRM_LOOKUP(x, y, z, a, b)	xfrm_lookup(x, y, z, a, b)
#define	IP6_ROUTE_OUTPUT(x, y, z)	ip6_route_output(x, y, z)
#define DEV_HAS_HARD_START(d)   ((d)->netdev_ops->ndo_start_xmit != 0)
#define DEV_HARD_START_XMIT(s, d) (d)->netdev_ops->ndo_start_xmit(s, d)

/* sysctl */
#define REGISTER_SYSCTL_TABLE(x, y) register_sysctl_table(x)

/* proc */
#define NET_NAMESPACE(x)	init_net.x

/* net */
#define IP_ROUTE_OUTPUT_KEY(x, y) ip_route_output_key(&init_net, x, y)

//#define	SET_MODULE_OWNER(x)

/* crypto */
#define crypto_tfm crypto_cipher
#define crypto_alloc_tfm(name,type) crypto_alloc_cipher(name,type, CRYPTO_ALG_ASYNC)
#define crypto_free_tfm(cipher) crypto_free_cipher(cipher)

/* ISR */
#define SA_SHIRQ IRQF_SHARED

/* RTNL */
#define rtnl_shlock	rtnl_lock
#define rtnl_shunlock	rtnl_unlock

/* notifier */
#define	NOTIFIER_CHAIN_REGISTER		atomic_notifier_chain_register
#define	NOTIFIER_CHAIN_UNREGISTER	atomic_notifier_chain_unregister

#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4,4,60))
/*
 * 3.4.0+
 */

/* netdevice */
#define	DEV_GET_BY_NAME(x)	dev_get_by_name(&init_net, x)
#define	DEV_GET_BY_INDEX(x)	dev_get_by_index(&init_net, x)
#define	XFRM_LOOKUP(x, y, z, a, b)	xfrm_lookup(x, y, z, a, b)
#define DEV_HAS_HARD_START(d)   ((d)->netdev_ops->ndo_start_xmit != 0)
#define DEV_HARD_START_XMIT(s, d) (d)->netdev_ops->ndo_start_xmit(s, d)

/* sysctl */
#define REGISTER_SYSCTL_TABLE(x, y) register_sysctl_table(x)

/* proc */
#define NET_NAMESPACE(x)	init_net.x

/* net */
#define IP_ROUTE_OUTPUT_KEY(y) ip_route_output_key_aruba(&init_net, y.u.ip4)

//#define	SET_MODULE_OWNER(x)

/* crypto */
#define crypto_tfm crypto_cipher
#define crypto_alloc_tfm(name,type) crypto_alloc_cipher(name,type, CRYPTO_ALG_ASYNC)
#define crypto_free_tfm(cipher) crypto_free_cipher(cipher)

/* ISR */
#define SA_SHIRQ IRQF_SHARED

/* RTNL */
#define rtnl_shlock	rtnl_lock
#define rtnl_shunlock	rtnl_unlock

/* notifier */
#define	NOTIFIER_CHAIN_REGISTER		atomic_notifier_chain_register
#define	NOTIFIER_CHAIN_UNREGISTER	atomic_notifier_chain_unregister

#else
/*
 * 4.4.60+
 */

/* netdevice */
#define	DEV_GET_BY_NAME(x)	dev_get_by_name(&init_net, x)
#define	DEV_GET_BY_INDEX(x)	dev_get_by_index(&init_net, x)
#define	XFRM_LOOKUP(x, y, z, a, b)	xfrm_lookup(x, y, z, a, b)
#define DEV_HAS_HARD_START(d)   ((d)->netdev_ops->ndo_start_xmit != 0)
#define DEV_HARD_START_XMIT(s, d) (d)->netdev_ops->ndo_start_xmit(s, d)

/* sysctl */
#define REGISTER_SYSCTL_TABLE(x, y) register_sysctl_table(x)

/* proc */
#define NET_NAMESPACE(x)	init_net.x

/* net */
#define IP_ROUTE_OUTPUT_KEY(y) ip_route_output_key_aruba(&init_net, y.u.ip4)

//#define	SET_MODULE_OWNER(x)

/* crypto */
#define crypto_alloc_tfm(name,type) crypto_alloc_cipher(name,type, CRYPTO_ALG_ASYNC)

/* ISR */
#define SA_SHIRQ IRQF_SHARED

/* RTNL */
#define rtnl_shlock	rtnl_lock
#define rtnl_shunlock	rtnl_unlock

/* notifier */
#define	NOTIFIER_CHAIN_REGISTER		atomic_notifier_chain_register
#define	NOTIFIER_CHAIN_UNREGISTER	atomic_notifier_chain_unregister

#endif	/* 3.4.0 */

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17) )
#ifndef NET_SKB_PAD
#define NET_SKB_PAD 16
#endif
#endif /*  < 2.6.17 */

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) )
#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"
#endif /*  >= 2.6.36 */

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0) )
#include <linux/proc_fs.h>

#define PDE_DATA(inode) PDE(inode)->data

static inline struct proc_dir_entry *PROC_CREATE_DATA(const char *name,
        mode_t mode, struct proc_dir_entry *parent,
        const struct file_operations *proc_fops, void *data)
{
    struct proc_dir_entry *pde = create_proc_entry(name, mode, parent);

    if (pde != NULL) {
        pde->proc_fops = proc_fops;
        pde->data = data;
    }
    return pde;
}

#define GET_PCPU_VAR(var)  percpu_read(var)

#define IPH_DA_TUN(rt, fl) (rt->rt_dst)

#define GET_SA_TUN(rt, fl) (rt->rt_src)

#else /*  < 3.10.0 */
#include <linux/proc_fs.h>

static inline struct proc_dir_entry *PROC_CREATE_DATA(const char *name,
        mode_t mode, struct proc_dir_entry *parent,
        const struct file_operations *proc_fops, void *data)
{
    return proc_create_data(name, mode, parent, proc_fops, data);
}

#define GET_PCPU_VAR(var)  this_cpu_read(var)

#define IPH_DA_TUN(rt, fl) (fl->u.ip4.daddr)

#define GET_SA_TUN(rt, fl) (fl->u.ip4.saddr)

#endif /*  < 3.10.0 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
#define VLAN_HWACCEL_PUT_TAG(skb, vlan_dev, vlan_tci)\
    ({                                          \
        skb->dev = vlan_dev;                    \
        __vlan_hwaccel_put_tag(skb, vlan_tci);  \
    })
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
#define VLAN_HWACCEL_PUT_TAG(skb, vlan_dev, vlan_tci)\
    __vlan_hwaccel_put_tag(skb, vlan_tci)
#else
#define VLAN_HWACCEL_PUT_TAG(skb, vlan_dev, vlan_tci)\
    __vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vlan_tci)
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,11,0))
static inline  struct net_device *NETDEV_NOTIFIER_INFO_TO_DEV(void *ptr)
{
    return (struct net_device *)ptr;
}
#else  /*  < 3.11.0 */
static inline  struct net_device *NETDEV_NOTIFIER_INFO_TO_DEV(void *ptr)
{
    return netdev_notifier_info_to_dev(ptr);
}
#endif /*  < 3.11.0 */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0))
/* Explanation of new parameter 'name_assign_type' is here:
   https://patchwork.kernel.org/patch/4526491/
   Basically, this is only used for sysfs with the new pseudo-file 
   /sys/class/net/IFACE/name_assign_type. Legacy behavior is of type UNKNOWN
*/
#define NET_NAME_UNKNOWN 0
#define ALLOC_NETDEV(sizeof_priv, name, name_assign_type, setup) \
        alloc_netdev(sizeof_priv, name, setup)
#else
#define ALLOC_NETDEV alloc_netdev
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4,1,0))
extern int ip_dst_output(struct sk_buff *skb);
extern int ip6_dst_output(struct sk_buff *skb);

#define NF_HOOK_MACRO(pf, hook, net, sk, skb, indev, outdev, okfn) \
    NF_HOOK(pf, hook, skb, indev, outdev, okfn)
#define NF_HOOK_OKFN_ARGS struct sk_buff *skb
#define IP_DST_OUTPUT(net, sk, skb) ip_dst_output(skb)
#define IP6_DST_OUTPUT(net, sk, skb) ip6_dst_output(skb)
#define DST_OUTPUT(net, sk, skb) dst_output(skb)
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4,4,0))
extern int ip_dst_output(struct sock *sk, struct sk_buff *skb);
extern int ip6_dst_output(struct sock *sk, struct sk_buff *skb);

#define NF_HOOK_MACRO(pf, hook, net, sk, skb, indev, outdev, okfn) \
    NF_HOOK(pf, hook, sk, skb, indev, outdev, okfn)
#define NF_HOOK_OKFN_ARGS struct sock *sk, struct sk_buff *skb
#define IP_DST_OUTPUT(net, sk, skb) ip_dst_output(sk, skb)
#define IP6_DST_OUTPUT(net, sk, skb) ip6_dst_output(sk, skb)
#define DST_OUTPUT(net, sk, skb) dst_output(skb)
#else
#define NF_HOOK_MACRO NF_HOOK
extern int ip_dst_output(struct net *net, struct sock *sk, struct sk_buff *skb);
extern int ip6_dst_output(struct net *net, struct sock *sk, struct sk_buff *skb);

#define NF_HOOK_OKFN_ARGS struct net *net, struct sock *sk, struct sk_buff *skb
#define IP_DST_OUTPUT ip_dst_output
#define IP6_DST_OUTPUT ip6_dst_output
#define DST_OUTPUT dst_output
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))
#define cpumask_clear(dstp) cpus_clear(*(dstp))
#define cpumask_set_cpu(cpu, dstp) cpu_set(cpu, *(dstp))
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
typedef struct ctl_table ctl_table;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,19))
/* Starting from kernel 4.1.19, ip6_route_output is GPL symbol and 
   cannot be called from ASAP(proprietary): we need to roll our own copy :-( */
#define	IP6_ROUTE_OUTPUT(x, y, z)	ip6_route_output_aruba(x, y, z)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#define	IP6_ROUTE_OUTPUT(x, y, z)	ip6_route_output(x, y, z)
#endif

#endif /* __kernel_compat_h__ */
