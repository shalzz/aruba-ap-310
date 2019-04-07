#include <linux/skb_throttle.h>

#ifdef SKB_THROTTLE_ACTIVE
#include <linux/sysctl.h>
#include <linux/module.h>
#include <linux/string.h>
#include <kernel_compat.h>

#define SKBTHR_NODENAME "throttle"
#define SKBTHR_SYSCTL_DEBUG "throttle_debug"
int skbthr_debug = 0;

#ifdef CONFIG_ARUBA_AP
atomic_t skbthr_kernel_wdog = ATOMIC_INIT(0);
atomic_t *skbthr_kwdog_refcnt = &skbthr_kernel_wdog;
EXPORT_SYMBOL(skbthr_kwdog_refcnt);
#endif

#ifdef CONFIG_SYSCTL
#define CTL_AUTO        -2      /* cannot be CTL_ANY or CTL_NONE */
enum {
    SKBTHR_DEBUG           = 1,
} ;
#endif // TODO
static const ctl_table skbthr_sysctl_template[] = {
    { .ctl_name           = SKBTHR_DEBUG,
      .procname           = SKBTHR_SYSCTL_DEBUG,
      .data               = &skbthr_debug,
      .maxlen             = sizeof(skbthr_debug),
      .mode               = 0644,
      .proc_handler       = proc_dointvec
    },
   { 0 }
};
static struct ctl_table *skbthr_sysctls ;
static struct ctl_table_header *skbthr_sysctl_header ;

void
skbthr_sysctl_attach(void)
{
    int space ;

    space = 5*sizeof(struct ctl_table) + sizeof(skbthr_sysctl_template) ;
    skbthr_sysctls = kmalloc(space, GFP_KERNEL);
    if (skbthr_sysctls == NULL) {
	printk("%s: no memory for sysctl table!\n", __func__);
	return;
    }

    /* setup the table */
    memset(skbthr_sysctls, 0, space);
    skbthr_sysctls[0].ctl_name = CTL_NET;
    skbthr_sysctls[0].procname = "net";
    skbthr_sysctls[0].mode = 0555;
    skbthr_sysctls[0].child = &skbthr_sysctls[2];
    /* [1] is NULL terminator */
    skbthr_sysctls[2].ctl_name = CTL_AUTO;
    skbthr_sysctls[2].procname = SKBTHR_NODENAME ;
    skbthr_sysctls[2].mode = 0555;
    skbthr_sysctls[2].child = &skbthr_sysctls[4];
    /* [3] is NULL terminator */
    /* copy in pre-defined data */
    memcpy(&skbthr_sysctls[4], skbthr_sysctl_template,
	   sizeof(skbthr_sysctl_template));
    skbthr_sysctl_header = REGISTER_SYSCTL_TABLE(skbthr_sysctls, 1);
    if (!skbthr_sysctl_header) {
	printk("%s: failed to register sysctls!\n", __FUNCTION__);
	kfree(skbthr_sysctls);
	skbthr_sysctls = NULL;
    }
}

void
skbthr_sysctl_detach(void)
{
    if (skbthr_sysctl_header) {
	unregister_sysctl_table(skbthr_sysctl_header);
	skbthr_sysctl_header = NULL;
    }
    if (skbthr_sysctls) {
	kfree(skbthr_sysctls);
	skbthr_sysctls = NULL;
    }
}

EXPORT_SYMBOL(skbthr_debug);

#endif // CONFIG_MERLOT
