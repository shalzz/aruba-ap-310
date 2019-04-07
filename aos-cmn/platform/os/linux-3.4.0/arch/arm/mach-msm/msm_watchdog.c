/* Copyright (c) 2010-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/pm.h>
#include <linux/mfd/pmic8058.h>
#include <linux/jiffies.h>
#include <linux/suspend.h>
#include <linux/percpu.h>
#include <linux/interrupt.h>
#include <linux/reboot.h>
#include <asm/fiq.h>
#include <asm/hardware/gic.h>
#include <mach/msm_iomap.h>
#include <asm/mach-types.h>
#include <asm/cacheflush.h>
#include <mach/scm.h>
#include <mach/socinfo.h>
#include "msm_watchdog.h"
#include "timer.h"

#ifdef CONFIG_ARUBA_AP
#include <linux/smp.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/watchdog.h>
#include <cmn/oem/aphw.h>
#include <linux/mman.h>
#include <linux/kmod.h>
#endif

#define MODULE_NAME "msm_watchdog"

#define TCSR_WDT_CFG	0x30

#define WDT_RST		0x0
#define WDT_EN		0x8
#define WDT_STS		0xC
#define WDT_BARK_TIME	0x14
#define WDT_BITE_TIME	0x24

#define WDT_HZ		32768

#ifdef CONFIG_ARUBA_AP
static int wdog_pin = -1;
static int wdog_pin_val = 0;
extern int aruba_get_gpio_pin(char *);
#ifdef CONFIG_EIGER
extern void aruba_ext_wdt_enable(void);
extern void aruba_ext_wdt_disable(void);
extern int aruba_ext_wdt_refresh(int core);
#endif

#define CONFIG_WATCHDOG_NOWAYOUT
/*
 * refresh the watchdog timer for this many seconds in kernel
 * before letting the watchdog process take over.
 */
static int wdt_blink_led = 1;
static int green_led_pin = -1;
static int amber_led_pin = -1;
extern void aruba_gpio_out(int, unsigned);

#define QCA_WATCHDOG_DUMP_MEM  0x41400000
#define QCA_WATCHDOG_DUMP_MAGIC  0x44434151
struct aruba_tz_dump *aruba_tz_dump_data;
int qca_watchdog_barked = 0;

// state maintained to track which CPU petted the watchdog
#define WDG_DBG_HISTORY 60
char wdg_dbg_core_history[WDG_DBG_HISTORY+1];
extern char *kernel_log_buf;
extern unsigned *kernel_log_idx;

char * argv[] = { "/bin/sh", "-c", "/aruba/bin/wdcp", NULL };
#define WDG_BACKUP_LOG_COUNT 25
#define WDG_PANIC_COUNT 20
#endif

struct msm_watchdog_dump msm_dump_cpu_ctx;

static void __iomem *msm_wdt_base;

static unsigned long delay_time;
static unsigned long bark_time;
static unsigned long long last_pet;
static bool has_vic;
static unsigned int msm_wdog_irq;

/*
 * On the kernel command line specify
 * msm_watchdog.enable=1 to enable the watchdog
 * By default watchdog is turned on
 */
static int enable = 1;
module_param(enable, int, 0);

/*
 * Watchdog bark reboot timeout in seconds.
 * Can be specified in kernel command line.
 */
static int reboot_bark_timeout = 22;
module_param(reboot_bark_timeout, int, 0644);
/*
 * If the watchdog is enabled at bootup (enable=1),
 * the runtime_disable sysfs node at
 * /sys/module/msm_watchdog/runtime_disable
 * can be used to deactivate the watchdog.
 * This is a one-time setting. The watchdog
 * cannot be re-enabled once it is disabled.
 */
static int runtime_disable;
static DEFINE_MUTEX(disable_lock);
static int wdog_enable_set(const char *val, struct kernel_param *kp);
module_param_call(runtime_disable, wdog_enable_set, param_get_int,
			&runtime_disable, 0644);

/*
 * On the kernel command line specify msm_watchdog.appsbark=1 to handle
 * watchdog barks in Linux. By default barks are processed by the secure side.
 */
static int appsbark = 1;
module_param(appsbark, int, 0);

static int appsbark_fiq;

/*
 * Use /sys/module/msm_watchdog/parameters/print_all_stacks
 * to control whether stacks of all running
 * processes are printed when a wdog bark is received.
 */
static int print_all_stacks = 1;
module_param(print_all_stacks, int,  S_IRUGO | S_IWUSR);
#if CONFIG_ARUBA_AP
/*
 *  enable aruba proc file for watchdog control and track.
 */
static int enable_aruba_watchdog_proc = 1;
module_param(enable_aruba_watchdog_proc, int,  S_IRUGO | S_IWUSR);
#endif
#ifndef CONFIG_ARUBA_AP
/* Area for context dump in secure mode */
static void *scm_regsave;
#endif

static struct msm_watchdog_pdata __percpu **percpu_pdata;

static void pet_watchdog_work(struct work_struct *work);
static void init_watchdog_work(struct work_struct *work);
#if CONFIG_ARUBA_AP
static void init_aruba_watchdog_work(struct work_struct *work);
static DECLARE_WORK(init_aruba_dogwork_struct, init_aruba_watchdog_work);
#endif
static DECLARE_DELAYED_WORK(dogwork_struct, pet_watchdog_work);
static DECLARE_WORK(init_dogwork_struct, init_watchdog_work);

/* Called from the FIQ bark handler */
void msm_wdog_bark_fin(void)
{
	flush_cache_all();
	pr_crit("\nApps Watchdog bark received - Calling Panic\n");
	panic("Apps Watchdog Bark received\n");
}

static int msm_watchdog_suspend(struct device *dev)
{
	if (!enable)
		return 0;

	__raw_writel(1, msm_wdt_base + WDT_RST);
	__raw_writel(0, msm_wdt_base + WDT_EN);
	mb();
	return 0;
}

static int msm_watchdog_resume(struct device *dev)
{
	if (!enable)
		return 0;

	__raw_writel(1, msm_wdt_base + WDT_EN);
	__raw_writel(1, msm_wdt_base + WDT_RST);
	mb();
	return 0;
}

static int panic_wdog_handler(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	if (panic_timeout == 0) {
		__raw_writel(0, msm_wdt_base + WDT_EN);
		mb();
	} else {
		__raw_writel(WDT_HZ * (panic_timeout + 4),
				msm_wdt_base + WDT_BARK_TIME);
		__raw_writel(WDT_HZ * (panic_timeout + 4),
				msm_wdt_base + WDT_BITE_TIME);
		__raw_writel(1, msm_wdt_base + WDT_RST);
	}
	return NOTIFY_DONE;
}

static struct notifier_block panic_blk = {
	.notifier_call	= panic_wdog_handler,
};

#define get_sclk_hz(t_ms) ((t_ms / 1000) * WDT_HZ)
#define get_reboot_bark_timeout(t_s) ((t_s * MSEC_PER_SEC) < bark_time ? \
		get_sclk_hz(bark_time) : get_sclk_hz(t_s * MSEC_PER_SEC))

static int msm_watchdog_reboot_notifier(struct notifier_block *this,
		unsigned long code, void *unused)
{

	u64 timeout = get_reboot_bark_timeout(reboot_bark_timeout);
	__raw_writel(timeout, msm_wdt_base + WDT_BARK_TIME);
	__raw_writel(timeout + 3 * WDT_HZ,
			msm_wdt_base + WDT_BITE_TIME);
	__raw_writel(1, msm_wdt_base + WDT_RST);

	return NOTIFY_DONE;
}

static struct notifier_block msm_reboot_notifier = {
	.notifier_call = msm_watchdog_reboot_notifier,
};

struct wdog_disable_work_data {
	struct work_struct work;
	struct completion complete;
};

static void wdog_disable_work(struct work_struct *work)
{
	struct wdog_disable_work_data *work_data =
		container_of(work, struct wdog_disable_work_data, work);
	__raw_writel(0, msm_wdt_base + WDT_EN);
	mb();
	if (has_vic) {
		free_irq(msm_wdog_irq, 0);
	} else {
		disable_percpu_irq(msm_wdog_irq);
		if (!appsbark_fiq) {
			free_percpu_irq(msm_wdog_irq,
					percpu_pdata);
			free_percpu(percpu_pdata);
		}
	}
	enable = 0;
	atomic_notifier_chain_unregister(&panic_notifier_list, &panic_blk);
	unregister_reboot_notifier(&msm_reboot_notifier);
	cancel_delayed_work(&dogwork_struct);
	/* may be suspended after the first write above */
	__raw_writel(0, msm_wdt_base + WDT_EN);
	complete(&work_data->complete);
	pr_info("MSM Watchdog deactivated.\n");
}

static int wdog_enable_set(const char *val, struct kernel_param *kp)
{
	int ret = 0;
	int old_val = runtime_disable;
	struct wdog_disable_work_data work_data;

	mutex_lock(&disable_lock);
	if (!enable) {
		printk(KERN_INFO "MSM Watchdog is not active.\n");
		ret = -EINVAL;
		goto done;
	}

	ret = param_set_int(val, kp);
	if (ret)
		goto done;

	if (runtime_disable == 1) {
		if (old_val)
			goto done;
		init_completion(&work_data.complete);
		INIT_WORK_ONSTACK(&work_data.work, wdog_disable_work);
		schedule_work_on(0, &work_data.work);
		wait_for_completion(&work_data.complete);
	} else {
		runtime_disable = old_val;
		ret = -EINVAL;
	}
done:
	mutex_unlock(&disable_lock);
	return ret;
}

unsigned min_slack_ticks = UINT_MAX;
unsigned long long min_slack_ns = ULLONG_MAX;

void pet_watchdog(void)
{
	int slack;
	unsigned long long time_ns;
	unsigned long long slack_ns;
	unsigned long long bark_time_ns = bark_time * 1000000ULL;

	if (!enable)
		return;

	slack = __raw_readl(msm_wdt_base + WDT_STS) >> 3;
	slack = ((bark_time*WDT_HZ)/1000) - slack;
	if (slack < min_slack_ticks)
		min_slack_ticks = slack;
	__raw_writel(1, msm_wdt_base + WDT_RST);
	time_ns = sched_clock();
	slack_ns = (last_pet + bark_time_ns) - time_ns;
	if (slack_ns < min_slack_ns)
		min_slack_ns = slack_ns;
	last_pet = time_ns;
}

static void pet_watchdog_work(struct work_struct *work)
{
	pet_watchdog();

	if (enable)
		schedule_delayed_work_on(1, &dogwork_struct, delay_time);
}

static irqreturn_t wdog_bark_handler(int irq, void *dev_id)
{
	unsigned long nanosec_rem;
	unsigned long long t = sched_clock();
	struct task_struct *tsk;

	nanosec_rem = do_div(t, 1000000000);
	printk(KERN_INFO "Watchdog bark! Now = %lu.%06lu\n", (unsigned long) t,
		nanosec_rem / 1000);

	nanosec_rem = do_div(last_pet, 1000000000);
	printk(KERN_INFO "Watchdog last pet at %lu.%06lu\n", (unsigned long)
		last_pet, nanosec_rem / 1000);

	if (print_all_stacks) {

		/* Suspend wdog until all stacks are printed */
		msm_watchdog_suspend(NULL);

		printk(KERN_INFO "Stack trace dump:\n");

		for_each_process(tsk) {
			printk(KERN_INFO "\nPID: %d, Name: %s\n",
				tsk->pid, tsk->comm);
			show_stack(tsk, NULL);
		}

		msm_watchdog_resume(NULL);
	}

	panic("Apps watchdog bark received!");
	return IRQ_HANDLED;
}

#ifdef CONFIG_ARUBA_AP
void
aruba_watchdog_stop_blinking(void)
{
	wdt_blink_led = 0;
}
EXPORT_SYMBOL(aruba_watchdog_stop_blinking);
#endif

#define SCM_SET_REGSAVE_CMD 0x2

static long configure_bark_dump(void *param)
{
	int ret;
	struct {
		unsigned addr;
		int len;
	} cmd_buf;

	if (!appsbark) {
#ifdef CONFIG_ARUBA_AP
        void __iomem *regsave_vm_base = 0;

        regsave_vm_base = ioremap_nocache(QCA_WATCHDOG_DUMP_MEM, sizeof(struct aruba_tz_dump));

        if (regsave_vm_base) {
            struct qca_tz_watchdog_dump *tz_dump;
            struct aruba_tz_dump *ap_tz_dump;

            tz_dump = (struct qca_tz_watchdog_dump *)regsave_vm_base;

            aruba_tz_dump_data = (struct aruba_tz_dump *)kmalloc(sizeof(struct aruba_tz_dump), GFP_KERNEL);
            if (unlikely(!aruba_tz_dump_data)) {
                printk(KERN_ERR "not enough free pages for QCA watchdog dump\n");
            } else {
                memcpy_fromio(aruba_tz_dump_data, regsave_vm_base, sizeof(struct aruba_tz_dump));
            }

            if ((tz_dump->magic == QCA_WATCHDOG_DUMP_MAGIC) && 
                    (tz_dump->reg_dump[0].magic || tz_dump->reg_dump[1].magic)) {

                if (aruba_tz_dump_data) {
                    qca_watchdog_barked = 1;
                    printk("found a watchdog dump\n");
                }
            }

			cmd_buf.addr = QCA_WATCHDOG_DUMP_MEM;
			cmd_buf.len  = PAGE_SIZE;

			ret = scm_call(SCM_SVC_UTIL, SCM_SET_REGSAVE_CMD,
				       &cmd_buf, sizeof(cmd_buf), NULL, 0);
			if (ret)
				pr_err("Setting register save address failed.\n"
				       "Registers won't be dumped on a dog "
				       "bite\n");

            /* fill Aruba fields */
            ap_tz_dump = (struct aruba_tz_dump *)regsave_vm_base;
            iowrite32(__pa(&jiffies_64), &ap_tz_dump->jiffies64_addr);
            iowrite32(__pa(kernel_log_idx), &ap_tz_dump->kernel_log_idx_addr);
            iowrite32(__pa(kernel_log_buf), &ap_tz_dump->kernel_log_addr);
            iowrite32(__pa(wdg_dbg_core_history), &ap_tz_dump->wdg_history_addr);

            /* clear jiffies64 field  */
            memset_io(&ap_tz_dump->jiffies64, 0, sizeof(ap_tz_dump->jiffies64));

            iounmap(regsave_vm_base);
        }

#else
		scm_regsave = (void *)__get_free_page(GFP_KERNEL);

		if (scm_regsave) {
			cmd_buf.addr = __pa(scm_regsave);
			cmd_buf.len  = PAGE_SIZE;

			ret = scm_call(SCM_SVC_UTIL, SCM_SET_REGSAVE_CMD,
				       &cmd_buf, sizeof(cmd_buf), NULL, 0);
			if (ret)
				pr_err("Setting register save address failed.\n"
				       "Registers won't be dumped on a dog "
				       "bite\n");
		} else {
			pr_err("Allocating register save space failed\n"
			       "Registers won't be dumped on a dog bite\n");
			/*
			 * No need to bail if allocation fails. Simply don't
			 * send the command, and the secure side will reset
			 * without saving registers.
			 */
		}
#endif
	}
    return ret;
}

struct fiq_handler wdog_fh = {
	.name = MODULE_NAME,
};

static void init_watchdog_work(struct work_struct *work)
{
	u64 timeout = (bark_time * WDT_HZ)/1000;
	void *stack;
	int ret;

	if (has_vic) {
		ret = request_irq(msm_wdog_irq, wdog_bark_handler, 0,
				  "apps_wdog_bark", NULL);
		if (ret)
			return;
	} else if (appsbark_fiq) {
		claim_fiq(&wdog_fh);
		set_fiq_handler(&msm_wdog_fiq_start, msm_wdog_fiq_length);
		stack = (void *)__get_free_pages(GFP_KERNEL, THREAD_SIZE_ORDER);
		if (!stack) {
			pr_info("No free pages available - %s fails\n",
					__func__);
			return;
		}

		msm_wdog_fiq_setup(stack);
		gic_set_irq_secure(msm_wdog_irq);
	} else {
		percpu_pdata = alloc_percpu(struct msm_watchdog_pdata *);
		if (!percpu_pdata) {
			pr_err("%s: memory allocation failed for percpu data\n",
					__func__);
			return;
		}

		/* Must request irq before sending scm command */
		ret = request_percpu_irq(msm_wdog_irq,
			wdog_bark_handler, "apps_wdog_bark", percpu_pdata);
		if (ret) {
			free_percpu(percpu_pdata);
			return;
		}
	}

	configure_bark_dump(NULL);

	__raw_writel(timeout, msm_wdt_base + WDT_BARK_TIME);
	__raw_writel(timeout + 3*WDT_HZ, msm_wdt_base + WDT_BITE_TIME);

	schedule_delayed_work_on(1, &dogwork_struct, delay_time);

	atomic_notifier_chain_register(&panic_notifier_list,
				       &panic_blk);

	ret = register_reboot_notifier(&msm_reboot_notifier);
	if (ret)
		pr_err("Failed to register reboot notifier\n");

	__raw_writel(1, msm_wdt_base + WDT_EN);
	__raw_writel(1, msm_wdt_base + WDT_RST);
	last_pet = sched_clock();

	if (!has_vic)
		enable_percpu_irq(msm_wdog_irq, IRQ_TYPE_EDGE_RISING);

	printk(KERN_INFO "MSM Watchdog Initialized\n");

	return;
}

static int msm_watchdog_probe(struct platform_device *pdev)
{
	struct msm_watchdog_pdata *pdata = pdev->dev.platform_data;

	if (!enable || !pdata || !pdata->pet_time || !pdata->bark_time) {
		printk(KERN_INFO "MSM Watchdog Not Initialized\n");
		return -ENODEV;
	}

	bark_time = pdata->bark_time;
	/* reboot_bark_timeout (in seconds) might have been supplied as
	 * module parameter.
	 */
	if ((reboot_bark_timeout * MSEC_PER_SEC) < bark_time)
		reboot_bark_timeout = (bark_time / MSEC_PER_SEC);

	has_vic = pdata->has_vic;

	if (!pdata->has_secure) {
		appsbark = 1;
		appsbark_fiq = pdata->use_kernel_fiq;
	}

	msm_wdt_base = pdata->base;
	msm_wdog_irq = platform_get_irq(pdev, 0);

	if (pdata->needs_expired_enable)
		__raw_writel(0x1, MSM_CLK_CTL_BASE + 0x3820);

	delay_time = msecs_to_jiffies(pdata->pet_time);
	schedule_work_on(0, &init_dogwork_struct);
	return 0;
}

static const struct dev_pm_ops msm_watchdog_dev_pm_ops = {
	.suspend_noirq = msm_watchdog_suspend,
	.resume_noirq = msm_watchdog_resume,
};

static struct platform_driver msm_watchdog_driver = {
	.probe = msm_watchdog_probe,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
		.pm = &msm_watchdog_dev_pm_ops,
	},
};

#ifdef CONFIG_ARUBA_AP
/* WDT0 for core0, WDT1 for core1.*/
static void aruba_watchdog_timer_fn_core0(struct work_struct *work);
static void aruba_watchdog_timer_fn_core1(struct work_struct *work);
static DECLARE_DELAYED_WORK(aruba_dogwork_struct_core0, aruba_watchdog_timer_fn_core0);
static DECLARE_DELAYED_WORK(aruba_dogwork_struct_core1, aruba_watchdog_timer_fn_core1);
static DEFINE_SPINLOCK(aruba_wdt_lock);
/*
 * Functions for reading/writing to file /proc/wdg_dbg_params
 */
#define MAX_PROC_SIZE 200
#define WDT_COUNT (10*60)
#define WDG_DBG_PRINT_THRESH     41
/*
 bite time is 'bark time + WDT_BITE_DELAY' seconds
 bite time should not exceed external WDT period of ~100 seconds
 bark time is taken from aruba_wdt_period
*/
#define WDT_BITE_DELAY 20

typedef struct wdt_state_t {
	unsigned long wdt_is_active;
	struct delayed_work *dwork;
	int wdt_enabled;
	int wdt_count;
    void __iomem *msm_wdt_base;
} wdt_state_t;

/* Bug 128509, bark limit is much larger than believed originally.
Put twice the time needed to print at 9600, the largest bug report seen which is 30 seconds */
u32 aruba_wdt_period = 50000; /* bark time in millisecs */
atomic_t wdt_open_count;
u32 aruba_wdt_enabled = 0;
static unsigned wdt_stop_intr = 1;
// counter per core for wdg debugging, increments each time wdg is petted
static uint8_t wdg_dbg_print_ctr[2] = {0, 0}; 
int wdg_dbg_interval = 0;
int wdg_dbg_en = 0;
uint8_t wdg_dbg_consec = 0;
static unsigned wdt_ignore = 0;
/* wdt_is_active stores wether or not the /dev/watchdog device is opened */


static wdt_state_t wdt_state[NR_CPUS];
static char proc_data[MAX_PROC_SIZE];
static char str1[WDG_DBG_HISTORY], str2[WDG_DBG_HISTORY];
static struct proc_dir_entry *proc_write_entry;
static int hist_wp=0, hist_wrap=0, missing=0, prev_core=0;

int read_proc_wdg_dbg(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    int len=0;
    wdg_dbg_core_history[WDG_DBG_HISTORY] = '\0';
    if (hist_wrap) {
            char save = wdg_dbg_core_history[hist_wp];
            snprintf(str1, sizeof(str1), "%s", &wdg_dbg_core_history[hist_wp]);
            wdg_dbg_core_history[hist_wp] = '\0';
            snprintf(str2, sizeof(str2), "%s", wdg_dbg_core_history);
            wdg_dbg_core_history[hist_wp] = save;
    } else {
            snprintf(str1, sizeof(str1), "%s", wdg_dbg_core_history);
            str2[0] = '\0';
    }
    len = sprintf(buf,"Input[%s] Consecutive[%d] Debug[%d] Interval[%d] %d %d History[%s%s]\n", 
                  proc_data, missing, wdg_dbg_en, wdg_dbg_interval, hist_wrap, hist_wp, str1, str2);
    return len;
}

static long dead_loop_test(void *arg)
{
    int irq_on = *(int *)arg;

    if (!irq_on) {
        local_irq_disable();
    }
    while(1);
    return 0;
}

static long dead_lock_test(void *arg)
{
    static DEFINE_SPINLOCK(debug_lock);
    int irq_on = *(int *)arg;

    printk(KERN_DEBUG "lock debug_lock on core %d with irq on %d\n", smp_processor_id(), irq_on);
    if (!irq_on) {
        spin_lock_irq(&debug_lock);
    } else {
        spin_lock(&debug_lock);
    }
    return 0;
}

int write_proc_wdg_dbg(struct file *file,  const char __user *buf, unsigned long count, void *data)
{
    int int_data = -1;
    int cpu = 0;
    int irq_on = 0;

    if(count >= MAX_PROC_SIZE)
        count = MAX_PROC_SIZE-1;
    if(copy_from_user(proc_data, buf, count))
        return -EFAULT;

    proc_data[count] = '\0';
    sscanf(proc_data,"%d %d %d",&int_data, &cpu, &irq_on);
    //int_data2 = simple_strtol(proc_data, NULL, 10);
    if (int_data == -1) {
        /* trigger a crash */
        BUG_ON(1);
    }
    if (int_data == -2) {
        /* trigger a crash with interrupt disabled */
        local_irq_disable();
        BUG_ON(1);
    }

    if (int_data == -3) {
        //smp_call_function_single(cpu, dead_loop_test, (void *)&irq_on, 0);
        work_on_cpu(cpu, dead_loop_test, (void *)&irq_on);
    }

    if (int_data == -4) {
        //smp_call_function_single(cpu, dead_lock_test, (void *)&irq_on, 0);
        work_on_cpu(cpu, dead_lock_test, (void *)&irq_on);
    }

    return count;
    if (int_data >= 1) {
        wdg_dbg_interval = int_data;
        wdg_dbg_en = 1;
    } else {
        wdg_dbg_interval = 0;
        wdg_dbg_en = 0;
    }
    proc_data[strlen(proc_data) - 1] = '\0';
    printk(KERN_CRIT "\nInput[%s][%d], Debug Enabled[%d], Debug Output Interval[%d] epochs.\n", 
            proc_data, strlen(proc_data), wdg_dbg_en, wdg_dbg_interval);
    return count;
}

void wdg_create_new_proc_entry(void)
{
    proc_write_entry = create_proc_entry("wdg_dbg_params",0666,NULL);
    if(!proc_write_entry)
    {
        printk(KERN_CRIT "%s: Error creating proc entry", __FUNCTION__);
    }
    proc_write_entry->read_proc = read_proc_wdg_dbg ;
    proc_write_entry->write_proc = write_proc_wdg_dbg;
}

int wdg_proc_init (void) {
    wdg_create_new_proc_entry();
    return 0;
}

void wdg_proc_cleanup(void) {
    printk(KERN_INFO "%s\n", __FUNCTION__);
    remove_proc_entry("wdg_dbg_params", NULL);
}
/*
 * End of functions for /proc/wdg_dbg_params
 */
/* if spin dead lock is detected, call panic when watchdog timeout,
 * otherwise let watchdog bite to dump CPU registers */
int is_spin_lockup = 0;

static inline void update_core_history(int core)
{
    if (core < NR_CPUS) {
        wdg_dbg_core_history[hist_wp] = '0'+core;
        hist_wp = (hist_wp + 1) % WDG_DBG_HISTORY;
        if (0 == hist_wp)
            hist_wrap = 1;
        if (core == prev_core) {
            ++missing;
        } else {
            missing = 0;
        }
        prev_core = core;

        wdg_dbg_print_ctr[core] = missing; 
        // Check if we have some historial data for CPUs petting the watchdog
        if (wdg_dbg_print_ctr[core] >= WDG_DBG_PRINT_THRESH) {
            if ((wdg_dbg_consec == 0) && (jiffies > 45000)) {
                wdg_dbg_core_history[WDG_DBG_HISTORY] = '\0';
                if (hist_wrap) {
                    char save = wdg_dbg_core_history[hist_wp];
                    snprintf(str1, sizeof(str1), "%s", &wdg_dbg_core_history[hist_wp]);
                    wdg_dbg_core_history[hist_wp] = '\0';
                    snprintf(str2, sizeof(str2), "%s", wdg_dbg_core_history);
                    wdg_dbg_core_history[hist_wp] = save;
                } else {
                    snprintf(str1, sizeof(str1), "%s", wdg_dbg_core_history);
                    str2[0] = '\0';
                }
                
                /* In the case it means other CPU hangs at the time, just call panic... */
                if (is_spin_lockup) {
                    panic("Watchdog petted by same CPU %d for %d times", core, wdg_dbg_print_ctr[core]-1);
                }

                flush_cache_all();
            }
            if (++wdg_dbg_consec > WDG_DBG_HISTORY) {
                wdg_dbg_consec = 0;
            }
        } else {
            wdg_dbg_consec = 0;
        }
    } else {
        printk(KERN_EMERG "Invalid core %d\n", core);
    }
}

static void __aruba_wdt_enable(void *data)
{

	int core = smp_processor_id();
    wdt_state_t *wdt = &wdt_state[core];
    u64 timeout = (bark_time * WDT_HZ)/1000;
    __raw_writel(timeout, wdt->msm_wdt_base + WDT_BARK_TIME);
    __raw_writel(timeout + WDT_BITE_DELAY * WDT_HZ, wdt->msm_wdt_base + WDT_BITE_TIME);
	__raw_writel(1, wdt->msm_wdt_base + WDT_EN);
	__raw_writel(1, wdt->msm_wdt_base + WDT_RST);
	mb();
    last_pet = sched_clock();
#ifdef CONFIG_ARUBA_DEBUG
    printk("%s: enable watchdog on core %d wdt->msm_wdt_base %p\n", __FUNCTION__, core,wdt->msm_wdt_base);
#endif
}

static void __aruba_wdt_disable(void *data)
{
	u32 val;
	int core = smp_processor_id();
    wdt_state_t *wdt = &wdt_state[core];
	__raw_writel(1, wdt->msm_wdt_base + WDT_RST);
	__raw_writel(0, wdt->msm_wdt_base + WDT_EN);
	mb();
#ifdef CONFIG_ARUBA_DEBUG
    printk("%s: disable watchdog on core %d wdt->msm_wdt_base %p\n", __FUNCTION__, core,wdt->msm_wdt_base);
#endif
}

DEFINE_PER_CPU(unsigned long, gpio_tstamp);
static int blink = 1;

static void
aruba_pet_watchdog(int core)
{
    int slack;
	unsigned long long time_ns;
	unsigned long long slack_ns;
	unsigned long long bark_time_ns = bark_time * 1000000ULL;
    wdt_state_t *wdt = &wdt_state[core];
    static int last_core = 0;

    /* 
     * if only one core pets the watchdog, it turns out another core is dead.
     * don't reset watchdog from now, we expect the watchdog barks in the case
     */
    if (last_core == core) {
        return;
    }
    last_core = core;
#ifdef CONFIG_ARUBA_DEBUG
    {
        unsigned long nanosec_rem;
        unsigned long long t = sched_clock();
        nanosec_rem = do_div(t, 1000000000);
        printk(KERN_INFO "aruba watchdog Pet on CPU %u! Now = %lu.%06lu\n",smp_processor_id(), (unsigned long) t,
            nanosec_rem / 1000);
    }
#endif
	slack = __raw_readl(wdt->msm_wdt_base + WDT_STS) >> 3;
	slack = ((bark_time*WDT_HZ)/1000) - slack;
	if (slack < min_slack_ticks)
		min_slack_ticks = slack;
	__raw_writel(1, wdt->msm_wdt_base + WDT_RST);
	time_ns = sched_clock();
	slack_ns = (last_pet + bark_time_ns) - time_ns;
	if (slack_ns < min_slack_ns)
		min_slack_ns = slack_ns;
	last_pet = time_ns;
}

static void
aruba_wdt_refresh(int from_kernel)
{
    int core = smp_processor_id();
    static int missing[2] = {0,0};
#ifdef CONFIG_EIGER
    spin_lock_bh(&aruba_wdt_lock);
#endif
    /* Pet Watchdog.*/
    aruba_pet_watchdog(core);
    
    /* Blink LED */
	if (core == 0) {
		if (wdt_blink_led && green_led_pin != -1 && amber_led_pin != -1) {
			aruba_gpio_out(green_led_pin, blink);
			blink = !blink;
		}
	}
        /* Pet external watchdog */
	if ((wdog_pin != -1)) {
        unsigned long other_tstamp;
        /* wait long enough after other cpu has written to the GPIO pin */
        other_tstamp = per_cpu(gpio_tstamp, 1-core);
        //while ((jiffies > other_tstamp) && (jiffies - other_tstamp) < 15) {}
        if ((jiffies > other_tstamp) && (jiffies - other_tstamp) < 15) { 
            missing[core] = 1;
            return;
        }
        missing[core] = 0;
        aruba_gpio_out(wdog_pin, core);
        /* record when this CPU wrote to GPIO pin */
        if (!missing[1-core]) {
            get_cpu_var(gpio_tstamp) = jiffies;
        }
        put_cpu_var(gpio_tstamp);
        if (!from_kernel) {
            update_core_history(core);
        }
    }
#ifdef CONFIG_EIGER
    else if (xml_ap_model == AP_385) {
        aruba_ext_wdt_refresh(core);
        /* record when this CPU peted the dog */
        get_cpu_var(gpio_tstamp) = jiffies;
        put_cpu_var(gpio_tstamp);
        update_core_history(core);
    }
    spin_unlock_bh(&aruba_wdt_lock);
#endif
}
void 
aruba_watchdog_disable_all(void)
{
	printk("Disabling watchdog\n");
	on_each_cpu(__aruba_wdt_disable, NULL, 0);
#ifdef CONFIG_EIGER
	if (xml_ap_model == AP_385) {
		aruba_ext_wdt_disable();
	}
#endif

}

static void 
aruba_watchdog_enable_core(void *v)
{
	printk("Enabling watchdog on CPU %u\n", smp_processor_id());
	__aruba_wdt_enable(NULL);
}

void 
aruba_watchdog_enable_all(void)
{
	printk("Enabling watchdog on all CPUs\n");
	on_each_cpu(aruba_watchdog_enable_core, NULL, 0);
#ifdef CONFIG_EIGER
	if (xml_ap_model == AP_385) {
		aruba_ext_wdt_enable();
	}
#endif
}

void
touch_nmi_watchdog(void)
{
    touch_softlockup_watchdog();
    return aruba_wdt_refresh(1);
}
EXPORT_SYMBOL(touch_nmi_watchdog);

static void 
__aruba_watchdog_timer_fn(struct work_struct *work)
{
	int core = smp_processor_id();
	wdt_state_t *wdt = &wdt_state[core];
    static int trigger_bug = 0;
    static int log_backup = 0;

#ifdef CONFIG_EIGER
	spin_lock_bh(&aruba_wdt_lock);
#endif
	if (!wdt_ignore) {
        /* PET */
        aruba_pet_watchdog(core);
    }
    /* Blink LED */
	if (core == 0) {
		if (wdt_blink_led && green_led_pin != -1 && amber_led_pin != -1) {
			aruba_gpio_out(green_led_pin, blink);
			blink = !blink;
		}
	}
        /* Pet external watchdog */
	if ((core == 0) && (wdog_pin != -1)) {
		aruba_gpio_out(wdog_pin, wdog_pin_val);
		wdog_pin_val = !wdog_pin_val;
	}
#ifdef CONFIG_EIGER
	else if (xml_ap_model == AP_385) {
		aruba_ext_wdt_refresh(core);
		/* record when this CPU peted the dog */
		get_cpu_var(gpio_tstamp) = jiffies;
		put_cpu_var(gpio_tstamp);
		update_core_history(core);
	}
	spin_unlock_bh(&aruba_wdt_lock);
#endif

	if (--wdt->wdt_count >= 0) {
        if (wdt->wdt_count == WDG_BACKUP_LOG_COUNT) {
            if (!log_backup) {
                log_backup = 1;
                call_usermodehelper(argv[0], argv, NULL, UMH_WAIT_EXEC);
            }
        }
        if (wdt->wdt_count <= WDG_PANIC_COUNT) {
            if (!trigger_bug) {
                trigger_bug = 1;
                BUG();
            }
        }
        schedule_delayed_work_on(core, wdt->dwork, msecs_to_jiffies(1000));
	} else {
		printk(KERN_INFO "Kernel watchdog refresh ended on core %u.\n",core);
	}
}

static void 
aruba_watchdog_timer_fn_core0(struct work_struct *work)
{
        __aruba_watchdog_timer_fn(work); 
}

static void 
aruba_watchdog_timer_fn_core1(struct work_struct *work)
{
        __aruba_watchdog_timer_fn(work);
}

static void
aruba_watchdog_setup_timer(void *data)
{
	int core = smp_processor_id();
	wdt_state_t *wdt = &wdt_state[core];
    wdt->dwork = (core?&aruba_dogwork_struct_core1:&aruba_dogwork_struct_core0);
    schedule_delayed_work_on(core, wdt->dwork, msecs_to_jiffies(1000));
    printk(KERN_INFO "enable timer on core %u\n",smp_processor_id());
}

static ssize_t aruba_wdt_write(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	int core = smp_processor_id();

	if (count >= 2) {
		if (!strncmp(buf, "off", 3)) {
			aruba_watchdog_disable_all();
		}
		if (!strncmp(buf, "on", 2)) {
			aruba_watchdog_enable_all();
		}
		if (!strncmp(buf, "stop", 4)) {
			wdt_stop_intr = 1;
		}
		if (!strncmp(buf, "start", 5)) {
			wdt_stop_intr = 0;
		}
		if (!strncmp(buf, "unignore", 7)) {
			wdt_ignore = 0;
		}
		if (!strncmp(buf, "ignore", 6)) {
			wdt_ignore = 1;
		}
		if (!strncmp(buf, "crash", 5)) {
			panic("we suck");
		}
	}

	if (count) {
       //Pet Watchdog here.
#ifdef CONFIG_ARUBA_DEBUG
        printk(KERN_INFO "%s at core %u\n",__func__,core);
#endif
        aruba_wdt_refresh(0); 
	}
	return count;
}


static struct watchdog_info ident = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.identity = "Aruba MSM Watchdog",
};

static long aruba_wdt_ioctl(struct file *file,
				unsigned int cmd, unsigned long arg)
{
	u32 tmp = 0;
	u32 __user *p = (u32 __user *)arg;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		if (copy_to_user((void *)arg, &ident, sizeof(ident)))
			return -EFAULT;
	case WDIOC_GETSTATUS:
		return put_user(0, p);
	case WDIOC_GETBOOTSTATUS:
		/* TBR for Octomore */
		tmp = 0;
		/* returns CARDRESET if last reset was caused by the WDT */
		return (tmp ? WDIOF_CARDRESET : 0);
	case WDIOC_SETOPTIONS:
		if (get_user(tmp, p))
			return -EINVAL;
		if (tmp == WDIOS_ENABLECARD) {
            touch_nmi_watchdog();
			break;
		} else
			return -EINVAL;
		return 0;
	case WDIOC_KEEPALIVE:
		/* booke_wdt_ping(); */
        touch_nmi_watchdog();
		return 0;
	case WDIOC_SETTIMEOUT:
		if (get_user(tmp, p))
			return -EFAULT;
		aruba_wdt_period = tmp;
		//;
		return 0;
	case WDIOC_GETTIMEOUT:
		return put_user(aruba_wdt_period, p);
	default:
		return -ENOTTY;
	}

	return 0;
}

static int aruba_wdt_open(struct inode *inode, struct file *file)
{

	int core = smp_processor_id();
	wdt_state_t *wdt = &wdt_state[core];
#ifdef CONFIG_ARUBA_DEBUG
    printk(KERN_INFO "%s at core %u\n",__func__,core);
#endif
	spin_lock_bh(&aruba_wdt_lock);
	/* /dev/watchdog can only be opened once per core */
	if (test_and_set_bit(0, &wdt->wdt_is_active)) {
#ifdef CONFIG_ARUBA_DEBUG
        printk("watchdog has been opened for core %u,wdt_open_count %d\n",core,atomic_read(&wdt_open_count));
#endif
    }

	if (wdt->wdt_enabled == 0) {
		wdt->wdt_enabled = 1;
		on_each_cpu(__aruba_wdt_enable, NULL, 0);
#ifdef CONFIG_ARUBA_DEBUG
		printk(KERN_INFO "Aruba MSM Watchdog Timer Enabled (wdt_period=%d)\n",aruba_wdt_period);
#endif
	}

	wdt->wdt_count = 0;
	atomic_add(1, &wdt_open_count);
	spin_unlock_bh(&aruba_wdt_lock);

	return nonseekable_open(inode, file);
}

static int aruba_wdt_release(struct inode *inode, struct file *file)
{
	int core = smp_processor_id();
	wdt_state_t *wdt = &wdt_state[core];
#ifdef CONFIG_ARUBA_DEBUG
    printk(KERN_INFO "%s at core %u\n",__func__,core);
#endif
	spin_lock_bh(&aruba_wdt_lock);
	if (atomic_sub_return(1, &wdt_open_count) == 0) {
		/* 
		 * if this is the last close, make sure all 'is_active' bits
		 * are cleared and other state is set correctly; SIGKILL may 
		 * result in close() not running on the right core; XXX this 
		 * is ugly
		 */
		int i;
		for (i = 0; i < NR_CPUS; i++) {
			wdt_state_t *wdt2 = &wdt_state[i];
			if (wdt2->wdt_is_active) {
				clear_bit(0, &wdt2->wdt_is_active);
				wdt2->wdt_count = WDT_COUNT;
				schedule_delayed_work_on(i,wdt2->dwork,msecs_to_jiffies(1000));
#ifdef CONFIG_ARUBA_DEBUG
                printk("Restart Kernel wdg petter for core %u\n",i);
#endif
			}
		}
	}
	spin_unlock_bh(&aruba_wdt_lock);
	return 0;
}

static const struct file_operations aruba_wdt_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.write = aruba_wdt_write,
	.unlocked_ioctl = aruba_wdt_ioctl,
	.open = aruba_wdt_open,
	.release = aruba_wdt_release,
};

static struct miscdevice aruba_wdt_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name = "watchdog",
	.fops = &aruba_wdt_fops,
};

static void __exit aruba_wdt_exit(void)
{
	misc_deregister(&aruba_wdt_miscdev);
    free_percpu_irq(msm_wdog_irq,percpu_pdata);
    free_percpu(percpu_pdata);
	wdg_proc_cleanup();
}

static irqreturn_t aruba_wdog_bark_handler(int irq, void *dev_id)
{
	unsigned long nanosec_rem;
	unsigned long long t = sched_clock();
	struct task_struct *tsk;
    int core = smp_processor_id();
	nanosec_rem = do_div(t, 1000000000);
	printk(KERN_INFO "Watchdog bark! Now = %lu.%06lu\n", (unsigned long) t,
		nanosec_rem / 1000);

	nanosec_rem = do_div(last_pet, 1000000000);
	printk(KERN_INFO "Watchdog last pet at %lu.%06lu\n", (unsigned long)
		last_pet, nanosec_rem / 1000);

	panic("Aruba watchdog bark interrupt received on core %u!",core);
	return IRQ_HANDLED;
}

static void init_aruba_watchdog_work(struct work_struct *work)
{
	u64 timeout = (bark_time * WDT_HZ)/1000;
	void *stack;
	int ret;

	if (has_vic) {
		ret = request_irq(msm_wdog_irq, wdog_bark_handler, 0,
				  "apps_wdog_bark", NULL);
		if (ret)
			return;
	} else if (appsbark_fiq) {
		claim_fiq(&wdog_fh);
		set_fiq_handler(&msm_wdog_fiq_start, msm_wdog_fiq_length);
		stack = (void *)__get_free_pages(GFP_KERNEL, THREAD_SIZE_ORDER);
		if (!stack) {
			pr_info("No free pages available - %s fails\n",
					__func__);
			return;
		}

		msm_wdog_fiq_setup(stack);
		gic_set_irq_secure(msm_wdog_irq);
	} else {
		percpu_pdata = alloc_percpu(struct msm_watchdog_pdata *);
		if (!percpu_pdata) {
			pr_err("%s: memory allocation failed for percpu data\n",
					__func__);
			return;
		}

		/* Must request irq before sending scm command */
		ret = request_percpu_irq(msm_wdog_irq,
			aruba_wdog_bark_handler, "apps_wdog_bark", percpu_pdata);
		if (ret) {
			free_percpu(percpu_pdata);
			return;
		}
	}

	work_on_cpu(0, configure_bark_dump, NULL);

	__raw_writel(timeout, msm_wdt_base + WDT_BARK_TIME);
	__raw_writel(timeout + WDT_BITE_DELAY * WDT_HZ, msm_wdt_base + WDT_BITE_TIME);

    if (!enable_aruba_watchdog_proc) {
        schedule_delayed_work_on(1, &dogwork_struct, delay_time); 
    } else {
        on_each_cpu(aruba_watchdog_setup_timer, NULL, 0);         
    }

	__raw_writel(1, msm_wdt_base + WDT_EN);
	__raw_writel(1, msm_wdt_base + WDT_RST);
	last_pet = sched_clock();

	if (!has_vic)
		enable_percpu_irq(msm_wdog_irq, IRQ_TYPE_EDGE_RISING);

	printk(KERN_INFO "MSM Watchdog Initialized\n");

    return; 
}

static int 
aruba_watchdog_init(void)
{
	extern ap_led_t *aruba_get_led_info(char *);
	ap_led_t *led;
	int ret = 0;
    int i  = 0;
    int cpu  = 0;
#ifdef CONFIG_ARUBA_DEBUG
	printk(KERN_INFO "Aruba MSM Watchdog Timer Loaded\n");
#endif
    bark_time = aruba_wdt_period;
    delay_time = msecs_to_jiffies(aruba_wdt_period/2);;
    msm_wdog_irq = WDT0_ACCSCSSNBARK_INT;
    msm_wdt_base = MSM_TMR0_BASE + WDT0_OFFSET;
    appsbark = 0;
    has_vic = 0;
    if (enable_aruba_watchdog_proc) {
        ret = misc_register(&aruba_wdt_miscdev); 
        if (ret) {
            printk(KERN_CRIT "Cannot register miscdev on minor=%d: %d\n",
                    WATCHDOG_MINOR, ret);
            return ret;
        }

        // procfs for debugging silent reboot
        wdg_proc_init();
        memset(proc_data, 0, MAX_PROC_SIZE);
        wdg_dbg_core_history[0] = '\0';

        spin_lock_bh(&aruba_wdt_lock);

        for (i = 0; i < NR_CPUS; i++) {
            wdt_state_t *wdt = &wdt_state[i];
            wdt->wdt_count = WDT_COUNT;
            /* 
             * Only watchdog0 is avaliable for user. 
             * So core0 and core1 pet the same watchdog actually. 
             */
            wdt->msm_wdt_base = MSM_TMR0_BASE + WDT0_OFFSET;
        }

        spin_unlock_bh(&aruba_wdt_lock);
    }

	if (xml_ap_model == AP_385) {
	}
	else {
	led = aruba_get_led_info("status");
	if (led) {
		green_led_pin = led->led_u.led_gpio.gl_pin;
	}
	led = aruba_get_led_info("status-amber");
	if (led) {
		amber_led_pin = led->led_u.led_gpio.gl_pin;
	}

	wdog_pin = aruba_get_gpio_pin("wdog");
	}

    __raw_writel(0x1, MSM_CLK_CTL_BASE + 0x3820); //WDT0_CPU0
    schedule_work_on(0, &init_aruba_dogwork_struct);
    return ret; 
}
#endif

static int init_watchdog(void)
{
#ifdef CONFIG_ARUBA_AP
    return aruba_watchdog_init();
#else
	return platform_driver_register(&msm_watchdog_driver);
#endif
}

device_initcall(init_watchdog);
#ifdef CONFIG_ARUBA_AP
MODULE_DESCRIPTION("ARUBA MSM Watchdog Driver");
#else
MODULE_DESCRIPTION("MSM Watchdog Driver");
#endif
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL v2");

