// vim:set ts=4 sw=4 expandtab:
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/console.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <linux/serial.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/serial.h>
#include <asm/setup.h>
#include <asm/hw_breakpoint.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <oem/aptype.h>
#include <cmn/oem/kernel_oemdefs.c>
#include <linux/aruba-ap.h>
#include <cmn/oem/aphw.h>
#include <generated/compile.h>
#include <linux/utsname.h>
#include <linux/ctype.h>
#include <linux/gpio.h>
#include <cmn/util/usb.h>
#include <linux/mtd/mtd.h>
#include <linux/nmi.h>
#include <linux/kmod.h>
#include <mach/msm_iomap.h>
#include "msm_watchdog.h"
#include <linux/kallsyms.h>
#include <asm/unwind.h>
#include <soft-ap/misc-drivers/pm-isl/pm_api.h>

#define ARUBA_DEBUG
int xml_ap_model = AP_148;
int ap_booted_partition = -2;
int ap_flash_variant = 0;
int (*ap_panic_get_radio_sta_info)(char *) = NULL;
int sierra_ap_board = 0;
EXPORT_SYMBOL(xml_ap_model);
EXPORT_SYMBOL(ap_flash_variant);
EXPORT_SYMBOL(ap_booted_partition);
EXPORT_SYMBOL(ap_panic_get_radio_sta_info);

int conn_to_controller;
EXPORT_SYMBOL(conn_to_controller);
int asap_eth_capture_enabled = 0;
EXPORT_SYMBOL(asap_eth_capture_enabled);

static struct work_struct power_work;
static void aruba_enforce_power_policy(void);
static int power_check_timer_started;

extern void touch_nmi_watchdog(void);
extern apflash_t *aruba_get_flash_info_by_name(char *section);
extern int aruba_panic_flash_erase(loff_t);
extern int aruba_panic_flash_write(loff_t, size_t, char *);
#ifdef CONFIG_HAVE_HW_BREAKPOINT
extern void arch_set_watchpoint(void *, unsigned, int);
extern void arch_clear_watchpoint(void);
#endif

#define MTD_DEV_NUM              3

#define CONFIG_ETH_PORTS_NUM 2

enum poe_type { POE_TYPE_UNKNOWN, POE_TYPE_AT, POE_TYPE_AF };

/* From /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies */
#define PM_FREQ_CPU_100 "1400000"
#define PM_FREQ_CPU_75 "1000000"
#define PM_FREQ_CPU_50 "600000"
#define PM_FREQ_CPU_25 "384000"
static int pm_cpu_freq = 100;
static enum pm_status ipm_power_status;


#define _N_RADIOS 4
static void *aruba_radio_ptrs[_N_RADIOS];
static int aruba_chain_mask_max[_N_RADIOS] = {0xf, 0xf, 0xf, 0xf};
static int aruba_chain_mask[_N_RADIOS] = {0xf, 0xf, 0xf, 0xf};
static int aruba_tx_power[_N_RADIOS] = {0xff, 0xff, 0xff, 0xff};
char aruba_boardname[64];
char aruba_bootver[64];
char aruba_variant[64];
unsigned    aruba_flash_base;
unsigned    aruba_flash_size;
unsigned char aruba_wmac[6];
unsigned aruba_wmac_count;
unsigned char aruba_mac[CONFIG_ETH_PORTS_NUM][6];
unsigned aruba_mac_count;
unsigned aruba_board_id = -1;
static int aruba_usb_power_mode = USB_POWER_MODE_AUTO; /* we start with auto usb mode as default*/
static void aruba_usb_power_control(int en);
static int dcpin, afpin;
static enum poe_type last_pol;
static int last_dc;
static int static_power_management;
static unsigned ap_power_mode;

/* usb_power_status : 0 - disable, 1 - enable */
#define USB_POWER_ENABLE 1
#define USB_POWER_DISABLE 0

unsigned usb_power_status = USB_POWER_ENABLE;

/* reboot reason */
#define AP_REBOOT_WARM 0
#define AP_REBOOT_COLD 1
#define AP_REBOOT_WDT  2
#define AP_REBOOT_CPU_WDT  3  /* internal watchdog bite */
#define PHY_DISABLE_WAIT_TIME   (60 * HZ * 2)

#define TOBA_PESUDO_DC_PIN 65

int ap_reboot_reason = AP_REBOOT_WARM;
EXPORT_SYMBOL(ap_reboot_reason);
#define WDT_STS		0xC

char ap_reboot_time[64] = {" "};

#define FL_CACHE_TAG "TheCache"

char buf[65536];

extern int aruba_xtime2date (unsigned long xtime_secs, char *dest_str, int sz);
static char date_str[128];

int
ap_panic_dump_erase(void *v, loff_t o, size_t sz, int spi)
{
    touch_nmi_watchdog();

    return aruba_panic_flash_erase(o);
}
EXPORT_SYMBOL(ap_panic_dump_erase);

int
ap_panic_dump_write(void *v, loff_t o, size_t sz, unsigned char *buf, int spi)
{
    touch_nmi_watchdog();

    return aruba_panic_flash_write(o, sz, buf);
}
EXPORT_SYMBOL(ap_panic_dump_write);

static int
aruba_panic_dump_extend_tag_write(char *buf, char *date_str)
{
    int len = 0;
    char radio_sta_str[128];

    len += sprintf(buf + len, "OS Version:%s\n", ARUBA_VERSION);

    len += sprintf(buf + len, "Uname:%s %s %s %s %s %s\n",
            init_uts_ns.name.sysname,
            init_uts_ns.name.nodename,
            init_uts_ns.name.release,
            init_uts_ns.name.version,
            init_uts_ns.name.machine,
            strcmp(init_uts_ns.name.domainname, "(none)") == 0 
            ? "unknown" : init_uts_ns.name.domainname);
    
    if (ap_panic_get_radio_sta_info) {
        memset(radio_sta_str, 0, sizeof(radio_sta_str)); 
        ap_panic_get_radio_sta_info((char *)radio_sta_str);
        len += sprintf(buf + len, "Client Number: %s\n", radio_sta_str);
    }

    if (date_str && strlen(date_str) > 0) {
        len += sprintf(buf + len, "Date:%s\n", date_str);
    }
    return len;
}

static void
aruba_panic_dump(void)
{
    extern void kernel_log_memcpy(char *buf, int len);
    extern unsigned kernel_get_logged_chars(void);
    extern void kernel_log_memcpy(char *buf, int len);
    extern unsigned kernel_get_logged_chars(void);
    apflash_t *flpanic, *flall;
    unsigned long flash_base, dump_offset, dump_size;
    int rv;
    int err = 0;
    int taglen = strlen(FL_CACHE_TAG);
    int maxlen = 0;
    unsigned buf_size = sizeof(buf);
    int len;
    int exttagsize;

    static int entries = 0;

    /*
     * Prevent repeated calls in case of additional panics/page faults.
     */
    if (++entries > 1) {
        return;
    }
	
    flpanic = aruba_get_flash_info_by_name("panic");
    if (!flpanic) {
        return;
    }

    flall = aruba_get_flash_info_by_name("all");
    if (!flall) {
        return;
    }

    dump_offset = flpanic->af_offset;
    dump_size = flpanic->af_size;
    flash_base = flall->af_start;


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
    {
        struct timeval now_tv;
        do_gettimeofday(&now_tv);
        rv = aruba_xtime2date(now_tv.tv_sec, date_str, sizeof(date_str));
    }
#else
    rv = aruba_xtime2date(xtime.tv_sec, date_str, sizeof(date_str));
#endif
    if (rv > 0) {
        printk("Time %s: Flash erase @ 0x%lx\n", date_str,
                flash_base + dump_offset);
    } else {
        printk("Flash erase @ 0x%lx\n", flash_base + dump_offset);
    }

    touch_nmi_watchdog();

    err = aruba_panic_flash_erase(dump_offset);
    if (err) {
        printk("%s: flash erase err:%d\n", __FUNCTION__, err);
    }

    // if needed, trim dump_size prior to copy
    if (dump_size > buf_size) {
        dump_size = buf_size;
    }
    maxlen = dump_size - taglen - 1;

    strlcpy(buf, FL_CACHE_TAG, taglen + 1);

    exttagsize = aruba_panic_dump_extend_tag_write(&buf[taglen + 1], date_str);

    len = kernel_get_logged_chars();
    if (len > maxlen - exttagsize) {
        len = maxlen - exttagsize;
    }

    kernel_log_memcpy(&buf[taglen + 1 + exttagsize], len);

    touch_nmi_watchdog();

    /* write the crash  info */
    err = aruba_panic_flash_write(dump_offset, len + taglen + 1 + exttagsize, buf);
    if (err) {
        printk("%s: flash write err:%d\n", __FUNCTION__, err);
    }

    return;
}

#if 0
/* this crap is here just to get the 64-bit division support loaded */
unsigned long long
waste_of_time(unsigned long long a, unsigned long long b)
{
    return a / b;
}

EXPORT_SYMBOL(waste_of_time);
#endif

void
platform_get_wireless_mac(u8 *mac)
{
    BUG_ON(1);
}
EXPORT_SYMBOL(platform_get_wireless_mac);

void __init
aruba_set_board_type(void)
{
    int i;

    if (!strcmp(aruba_boardname, "Octomore")) {
        if (!strcmp(aruba_variant, "00")) {
            xml_ap_model = AP_324;
        } else if (!strcmp(aruba_variant, "01")) {
            xml_ap_model = AP_325;
        } else {
            xml_ap_model = AP_325;
            printk("Unknown variant; assuming AP-325\n");
        }
    } else if (!strcmp(aruba_boardname, "Eiger")) {
        xml_ap_model = AP_385;
    } else if (!strcmp(aruba_boardname, "Glenfarclas")) {
        if (!strcmp(aruba_variant, "00")) {
            xml_ap_model = AP_314;
        } else if (!strcmp(aruba_variant, "01")) {
            xml_ap_model = AP_315;
        } else {
            xml_ap_model = AP_315;
            printk("Unknown variant; assuming AP-315\n");
        }
    } else if (!strcmp(aruba_boardname, "Sierra")) {
		sierra_ap_board = 1;
        if (!strcmp(aruba_variant, "74")) {
             xml_ap_model = AP_374; 
        } else if (!strcmp(aruba_variant, "75")) { 
              xml_ap_model = AP_375;
        } else if (!strcmp(aruba_variant, "77")) {
               xml_ap_model = AP_377;
        } else if (!strcmp(aruba_variant, "18")) {
            xml_ap_model = AP_318; 
        } else { 
            xml_ap_model = AP_375;
             printk("Unknown variant; assuming AP-375\n");
        }    
	} else if (!strcmp(aruba_boardname, "Toba")) {
        if (!strcmp(aruba_variant, "00")) {
            xml_ap_model = AP_387;
        } else {
            xml_ap_model = AP_160;
            printk("Unknown variant; assuming AP-160\n");
        }

    } else {
        printk("Unknown board name: %s\n", aruba_boardname);
        BUG_ON(1);
    }
    ap_panic_dump = aruba_panic_dump;

    aruba_panic_init();

    ap_flash_variant = -1;
    for (i = 0; i < __ap_hw_info[xml_ap_model].num_flash_variants; i++)  {
	   if (aruba_flash_size == __ap_hw_info[xml_ap_model].flash_variants[i].afv_size)  {
            ap_flash_variant = i;
            break;
        }
    }
    if (ap_flash_variant < 0)  {
        ap_flash_variant = 0;  /* assume first flash variant is good enough */
        printk("No flash variant matches size for this board: %s\n", aruba_boardname);
    } else {
        printk("Flash variant: %s\n", __ap_hw_info[xml_ap_model].flash_variants[ap_flash_variant].afv_name);
    }

    for (i = 0; i < __ap_hw_info[xml_ap_model].num_radios; i++) {
        aruba_chain_mask_max[i] =
            (1 << __ap_hw_info[xml_ap_model].radios[i].num_antennas) - 1;
        aruba_chain_mask[i] = aruba_chain_mask_max[i];
    }
}

static void
aruba_adj_mac(__u8 *mac, int adj)
{
    unsigned tmp;

    tmp = (mac[3] << 16) | (mac[4] << 8) | mac[5];
    tmp &= 0xffffff;

    tmp += adj;

    mac[3] = (tmp >> 16) & 0xff;
    mac[4] = (tmp >> 8) & 0xff;
    mac[5] = tmp & 0xff;
}
void aruba_get_env_mac(uint8_t *mac)
{
    char p[128];
    char *ep;
    int i;
    unsigned char env_mac[6];

    ep = aruba_get_apboot_env("ethaddr",p,sizeof(p));
    if(ep)
    {
        for(i=0; i<6;i++)
        {
            p[i*3 + 2]=0;
            env_mac[i]= (unsigned char)(simple_strtol(&p[i*3],0,16) & 0xff);
        
        }
        memcpy(mac, env_mac, sizeof(env_mac));
        printk("apboot env ethaddr is %02X:%02X:%02X:%02X:%02X:%02X\n",env_mac[0],env_mac[1],env_mac[2],env_mac[3],env_mac[4],env_mac[5]); 
        
    }
    else
    {
        printk("get env ethaddr from env failed\n");
    }
}
static int __init 
parse_tag_aruba_uboot(const struct tag *tag)
{
    int i = 0;

#ifdef CONFIG_ARUBA_DEBUG
    printk("Using Aruba parameters structure\n");
#endif

    strlcpy(aruba_boardname, tag->u.aruba_uboot.au_boardname, sizeof(aruba_boardname));
    strlcpy(aruba_bootver, tag->u.aruba_uboot.au_bootver, sizeof(aruba_bootver));
    strlcpy(aruba_variant, tag->u.aruba_uboot.au_variant, sizeof(aruba_variant));
    aruba_mac_count = tag->u.aruba_uboot.au_mac_count;
    for (i = 0; i < aruba_mac_count; i++) {
        if (i == 0) {
            memcpy(aruba_mac[i], tag->u.aruba_uboot.au_mac, 6);
        } else {
            memcpy(aruba_mac[i], aruba_mac[0], 6);
            aruba_adj_mac(aruba_mac[i], i);
        }
    }

#ifdef CONFIG_ARUBA_DEBUG
    printk("boardname '%s'\n", aruba_boardname);
    printk("bootver '%s'\n", aruba_bootver);
    printk("variant '%s'\n", aruba_variant);
    printk("mac: %02x:%02x:%02x:%02x:%02x:%02x; count %u\n",
        aruba_mac[0][0], 
        aruba_mac[0][1], 
        aruba_mac[0][2], 
        aruba_mac[0][3], 
        aruba_mac[0][4], 
        aruba_mac[0][5],
        aruba_mac_count);
#endif

    aruba_flash_base = tag->u.aruba_uboot.au_flash.start;
    aruba_flash_size = tag->u.aruba_uboot.au_flash.size;
    ap_booted_partition = tag->u.aruba_uboot.au_booted_partition;
    if (tag->u.aruba_uboot.au_spare1 != 0) {
        aruba_board_id = tag->u.aruba_uboot.au_spare1;
    }
#ifdef CONFIG_SMP
    setup_max_cpus = tag->u.aruba_uboot.au_num_cores;
#endif

#ifdef CONFIG_ARUBA_DEBUG
#ifdef CONFIG_SMP
    printk("CPUS: %u\n", setup_max_cpus);
#endif
    printk("flash: %u @ 0x%x\n", aruba_flash_size / (1024 * 1024), aruba_flash_base);
//    printk("boardID = %u\n", aruba_board_id);
#endif

    aruba_set_board_type();
//    aruba_panic_init();
    return 0;
}
__tagtable(ATAG_ARUBA_UBOOT, parse_tag_aruba_uboot);

static int __init 
parse_tag_aruba_wmac(const struct tag *tag)
{
#ifdef CONFIG_ARUBA_DEBUG
    printk("Using Aruba WMAC structure\n");
#endif

    memcpy(aruba_wmac, tag->u.aruba_wmac.aw_wmac, 6);
    aruba_wmac_count = tag->u.aruba_wmac.aw_count;
#ifdef CONFIG_ARUBA_DEBUG
    printk("wmac: %02x:%02x:%02x:%02x:%02x:%02x; count %u\n",
        aruba_wmac[0], 
        aruba_wmac[1], 
        aruba_wmac[2], 
        aruba_wmac[3], 
        aruba_wmac[4], 
        aruba_wmac[5],
        aruba_wmac_count);
#endif
    return 0;
}
__tagtable(ATAG_ARUBA_WMAC, parse_tag_aruba_wmac);

int
aruba_get_gpio_pin(char *name)
{
    int i;
    for (i = 0; i < __ap_hw_info[xml_ap_model].num_gpio_pins; i++) {
        if (!strcmp(__ap_hw_info[xml_ap_model].gpio_pins[i].ag_name, name)) {
            return __ap_hw_info[xml_ap_model].gpio_pins[i].ag_pin;
        }
    }
    return -1;
}
EXPORT_SYMBOL(aruba_get_gpio_pin);

unsigned
aruba_get_num_i2c_busses(void)
{
    return __ap_hw_info[xml_ap_model].num_i2c_busses;
}
EXPORT_SYMBOL(aruba_get_num_i2c_busses);

void
aruba_get_i2c_bus_info(unsigned num, unsigned *clock, unsigned *data)
{
    int i;
    for (i = 0; i < __ap_hw_info[xml_ap_model].num_i2c_busses; i++) {
        if (__ap_hw_info[xml_ap_model].i2c_busses[i].ib_num == num) {
            *clock = __ap_hw_info[xml_ap_model].i2c_busses[i].ib_clock;
            *data = __ap_hw_info[xml_ap_model].i2c_busses[i].ib_data;
            return;
        }
    }
}
EXPORT_SYMBOL(aruba_get_i2c_bus_info);

int
aruba_get_i2c_bus_for_device(char *name)
{
    int i;
    for (i = 0; i < __ap_hw_info[xml_ap_model].num_i2c_devices; i++) {
        if (!strcmp(__ap_hw_info[xml_ap_model].i2c_devices[i].id_name, name)) {
            return __ap_hw_info[xml_ap_model].i2c_devices[i].id_bus;
        }
    }
    return -1;
}
EXPORT_SYMBOL(aruba_get_i2c_bus_for_device);

ap_led_t *
aruba_get_led_info(char *name)
{
    int i;
    for (i = 0; i < __ap_hw_info[xml_ap_model].num_leds; i++) {
        if (!strcmp(__ap_hw_info[xml_ap_model].leds[i].led_name, name)) {
            return &__ap_hw_info[xml_ap_model].leds[i];
        }
    }
    return 0;
}
EXPORT_SYMBOL(aruba_get_led_info);

ap_phy_led_t *
aruba_get_phy_led_info(char *name)
{
    int i;
    ap_led_t *led;

    led = aruba_get_led_info("phy");
    if (!led) {
        return 0;
    }
    for (i = 0; i < led->led_num_led_modes; i++) {
        if (!strcmp(led->led_u.led_phy[i].pl_name, name)) {
            return &led->led_u.led_phy[i];
        }
    }
    return 0;
}
EXPORT_SYMBOL(aruba_get_phy_led_info);

__u32
aruba_reg_rd(__u32 addr)
{
#if 0
    return MV_REG_READ(addr);
#else
    return 0;
#endif
}

void
aruba_reg_wr(__u32 addr, __u32 val)
{
#if 0
    MV_REG_WRITE(addr, val);
#endif
}

void
aruba_reg_rmw_set(__u32 addr, __u32 mask)
{
#if 0
    MV_REG_BIT_SET(addr, mask);
#endif
}

/* 'bytes' can currently be 1, 2, 4, or 8.  Any access >= v
* && < (v + bytes) will be trapped by current CPU.
*
* a == 1: trap on stores
* a == 2: trap on loads
* a == 3: trap on any
*/
void
aruba_set_watch_cpu(void *v, unsigned bytes, int a)
{
#ifdef CONFIG_HAVE_HW_BREAKPOINT
   /* check range */
    switch (bytes) {
        case 1:
        case 2:
        case 4:
        case 8:
            break;
        default:
            BUG_ON(1);
    }

    /* check access */
    switch (a) {
        case 1: //store
        case 2: //load
        case 3: //any
            break;
        default:
            BUG_ON(1);
    }
    arch_set_watchpoint(v, bytes, a);
#endif
}

void
aruba_clear_watch_cpu(void)
{
#ifdef CONFIG_HAVE_HW_BREAKPOINT
    arch_clear_watchpoint();
#endif
}

struct aruba_watch_param{
    void *addr;
    unsigned bytes;
    int type;
};

static void
__aruba_set_watch(void *p)
{
    struct aruba_watch_param *param = (struct aruba_watch_param *)p;
    aruba_set_watch_cpu(param->addr, param->bytes, param->type);
}

/* 'bytes' can currently be 1, 2, 4, or 8.  Any access >= v
* && < (v + bytes) will be trapped by all CPU.
*
* a == 1: trap on stores
* a == 2: trap on loads
* a == 3: trap on any
*/
void
aruba_set_watch(void *v, unsigned bytes, int a)
{
    int cpu;
    int ret;
    struct aruba_watch_param param;

    param.addr = v;
    param.bytes = bytes;
    param.type = a;
 
    preempt_disable();
    for_each_online_cpu(cpu){
        ret = smp_call_function_single(cpu, __aruba_set_watch,
                &param, 1);
        WARN_ON(ret);
    }
    preempt_enable();
}
EXPORT_SYMBOL(aruba_set_watch);

/* 'bytes' can currently be 1, 2, 4, or 8.  Any access >= v
* && < (v + bytes) will be trapped by CPU.
*
* a == 1: trap on stores
* a == 2: trap on loads
* a == 3: trap on any
*/
void
aruba_set_watch_on_cpu(void *v, unsigned bytes, int a, int cpu)
{
    int ret;
    struct aruba_watch_param param;

    param.addr = v;
    param.bytes = bytes;
    param.type = a;
 
    preempt_disable();
    ret = smp_call_function_single(cpu, __aruba_set_watch,
            &param, 1);
    WARN_ON(ret);
    preempt_enable();
}
EXPORT_SYMBOL(aruba_set_watch_on_cpu);

static void
__aruba_clear_watch(void *p)
{
    aruba_clear_watch_cpu();
}

void aruba_clear_watch(void)
{
    int cpu;
    int ret;
    preempt_disable();
    for_each_online_cpu(cpu){
        ret = smp_call_function_single(cpu, __aruba_clear_watch,
                NULL, 1);
        WARN_ON(ret);
    }
    preempt_enable();
}
EXPORT_SYMBOL(aruba_clear_watch);

void aruba_clear_watch_on_cpu(int cpu)
{
    int ret;
    preempt_disable();
    ret = smp_call_function_single(cpu, __aruba_clear_watch,
            NULL, 1);
    WARN_ON(ret);
    preempt_enable();
}
EXPORT_SYMBOL(aruba_clear_watch_on_cpu);

void
aruba_reg_rmw_clear(__u32 addr, __u32 mask)
{
#if 0
    MV_REG_BIT_RESET(addr, mask);
#endif
}

/*
 * Unlike PPC, iProc adds the GPIO chip with a base of 0, so 
 * no offset is needed before calling gpiolib.
 */
int
aruba_gpio_in(int pin)
{
	if (pin == -1)
	{
        return -1;
    }
	/*Since Toba don't support DC power supply, 
	  to avoid involve too many changes on current power subsytem,
	  assue pesudo dc pin always return 0 on Toba P1 board
	*/
	if (xml_ap_model == AP_387 && pin == TOBA_PESUDO_DC_PIN)
		return 0;
	if (pin < 152)
		return gpio_get_value(pin);
	else
		return gpio_get_value_cansleep(pin); /*For sierra,if GPIO number > 152, it means using io expander. It can sleep*/
}
EXPORT_SYMBOL(aruba_gpio_in);

void
aruba_gpio_out(int pin, unsigned val)
{
    if (pin != -1) {
		if (pin < 152 )
			gpio_set_value(pin, val);
		else
			gpio_set_value_cansleep(pin, val); /*For sierra,if GPIO number > 152, it means using io expander. It can sleep*/
    }
}
EXPORT_SYMBOL(aruba_gpio_out);

void
aruba_gpio_config_output(unsigned pin, unsigned value)
{
    gpio_direction_output(pin, value);  /* 'value' is initial value */
}
EXPORT_SYMBOL(aruba_gpio_config_output);

int
aruba_gpio_request(unsigned gpio, unsigned long flags, const char *label)
{
    return gpio_request_one(gpio, flags, label);
}
EXPORT_SYMBOL(aruba_gpio_request);
/*
 * From GIPIO configuration register,get the direction of gpio via GPIO CONFIGURATION registe bit9 GPIO_OE, not need gpio request
*/
int
aruba_gpio_get_direction(unsigned gpio)
{
    int val;
    val = readl((MSM_TLMM_BASE + 0x1000 + (0x10 * gpio)));

    /*GPIO_OE 1:output enable,0:output disable*/  
    return(val & 0x200);       
}
EXPORT_SYMBOL(aruba_gpio_get_direction);

/* get Ethernet MAC address */
unsigned char *
aruba_get_mac_addr(int idx)
{

    if (idx >= aruba_mac_count) {
        return NULL;
    }
    return aruba_mac[idx];
}
EXPORT_SYMBOL(aruba_get_mac_addr);

#ifdef CONFIG_OCTOMORE
extern int aruba_spi_flash_read(char *, loff_t, loff_t, size_t);
#endif
/* get APBoot environment parameter */
char *
aruba_get_apboot_env(char *s, char *buf, int buf_size)
{
    int len, count, copy_byte;
    char *ep, *tmp_ep;
    apflash_t * fl;
    struct mtd_info *mtd_info;

    len = strlen(s);
    if (memcmp(s, "et0macaddr", len) == 0) {
        snprintf(buf, buf_size, "%02x:%02x:%02x:%02x:%02x:%02x",
                 aruba_mac[0][0], aruba_mac[0][1], aruba_mac[0][2], 
                 aruba_mac[0][3], aruba_mac[0][4], aruba_mac[0][5]);
        return buf;
    } else if ((memcmp(s, "et1macaddr", len) == 0) && (__ap_hw_info[xml_ap_model].num_enets > 1)) {
        snprintf(buf, buf_size, "%02x:%02x:%02x:%02x:%02x:%02x",
                 aruba_mac[1][0], aruba_mac[1][1], aruba_mac[1][2], 
                 aruba_mac[1][3], aruba_mac[1][4], aruba_mac[1][5]);
        return buf;
    } else if (memcmp(s, "aruba_mac_count", len) == 0) {
        if ((__ap_hw_info[xml_ap_model].num_enets > 1)) {
            snprintf(buf, buf_size, "%d", aruba_mac_count);
        } else {
            snprintf(buf, buf_size, "%d", 1);
        }
        return buf;
    }

    fl = aruba_get_flash_info_by_name("env");
    if (!fl) {
        return NULL;
    }
    
    mtd_info = get_mtd_device(NULL, MTD_DEV_NUM);
    if (mtd_info) {
        ep = kmalloc(fl->af_size, GFP_TEMPORARY);
        if (!ep) {
            pr_err("%s: Can't allocate memory of 0x%x bytes\n", __func__, fl->af_size);
            return NULL;
        }
        mtd_read(mtd_info, fl->af_offset, fl->af_size, &count, ep);
        put_mtd_device(mtd_info);
        if (count != fl->af_size) {
            printk("%s: read of %s failed count:%d offfset:%d\n",
                   __func__, s, count, fl->af_offset);
            kfree(ep);
            return NULL;
        }
    } else {
        printk("%s: no such mtd device\n", __func__);
        return NULL;
    }

    /* skip env crc */
    tmp_ep = ep + 4;
    len = strlen(s);
    while (tmp_ep < ep + fl->af_size) {
        if (memcmp(s, tmp_ep, len) == 0 && tmp_ep[len] == '=') {
            if (ep + fl->af_size - tmp_ep > buf_size) {
                copy_byte = buf_size;
            } else {
                copy_byte = ep + fl->af_size - tmp_ep;
            }
            tmp_ep = tmp_ep + len + 1;
            strncpy(buf, tmp_ep, copy_byte);
            kfree(ep);
            return buf;
        }
        tmp_ep++;
    }
    kfree(ep);
    return NULL;
}
EXPORT_SYMBOL(aruba_get_apboot_env);

unsigned lldp_power = 0;

static void
aruba_usb_power_control(int en)
{
    int usb_power_pin = -1;

    usb_power_pin = aruba_get_gpio_pin("usb-power");

    if (usb_power_pin == -1) {
        return;
    }

    if (en) {
        if (usb_power_status == USB_POWER_DISABLE) {
            aruba_gpio_out(usb_power_pin, 0);
            printk("Enabling USB power\n");
            usb_power_status = USB_POWER_ENABLE;
        }
    } else {
        if (usb_power_status == USB_POWER_ENABLE) {
            aruba_gpio_out(usb_power_pin, 1);
            printk("Disabling USB power\n");
            usb_power_status = USB_POWER_DISABLE;
        }
    }
}

/* create usb_power proc file so that user app can control usb power */
static int
aruba_proc_usb_power_mode_read(char *page, char **start, off_t off,
                          int count, int *eof, void *data)
{
    char *p = page;
    int len;

    p += sprintf(p, "%d\n", aruba_usb_power_mode);
    len = p - page;

    if (off >= len) {
        *eof = 1;
        return 0;
    }

    len -= off;
    if (len > count)
        len = count;

    *start = page + off;

    return len;

}

static ssize_t
aruba_proc_usb_power_mode_write(struct file *filp, 
    const char __user *buff, unsigned long len, void *data)
{
    char value[8];
    int i;
    unsigned olen = len;
    int dc;

    if (dcpin == -1) {
        return 1;
    }

    dc = aruba_gpio_in(dcpin);

    if (len >= sizeof(value))
        return -EINVAL;

    if (copy_from_user(value, buff, len))
        return -EFAULT;

    if (value[len - 1] == '\n') {
        /* for debugging from the shell, trim a newline */
        len--;
    }
    value[len] = '\0';

    for (i = 0; i < len; i++) {
        if (!isdigit(value[i]))
            return -EINVAL;
    }

    aruba_usb_power_mode = simple_strtoul(value, NULL, 0);

    if (static_power_management) {
        switch (aruba_usb_power_mode) {
            case USB_POWER_MODE_AUTO:
                /* if AP is in low power mode without DC or we have AF power */
                if ((ap_power_mode == 1 && !dc ) || last_pol != POE_TYPE_AT) {
                    /* force this to disable USB */
                    usb_power_status = USB_POWER_ENABLE;
                    /* disable the USB */
                    aruba_usb_power_control(0);
                } else {
                    /* enable the usb */
                    aruba_usb_power_control(1);
                }
                break;
            case USB_POWER_MODE_ENABLE:
                /* enable the usb */
                aruba_usb_power_control(1);
                break;
            case USB_POWER_MODE_DISABLE:
                usb_power_status = USB_POWER_ENABLE;
                /* disable the USB */
                aruba_usb_power_control(0);
                break;
            default:
                break;

#if 0
                if (aruba_usb_power_override)
                    aruba_usb_power_control(1);
#ifndef __FAT_AP__
                else if (ap_power_mode || last_pol != POE_TYPE_AT)
#else
                    //At the very beginning, last_pol is unkown without initilization.
                    //IAP will hit the condition by bug128079 and cause modem unwork.
                else if (ap_power_mode || last_pol == POE_TYPE_AF)
#endif
                    aruba_usb_power_control(0);
#endif
        }
    }

    return olen;
}


/* create usb_power proc file so that user app can control usb power */
static int
aruba_proc_usb_power_read(char *page, char **start, off_t off,
                          int count, int *eof, void *data)
{
    char *p = page;
    int len;

    p += sprintf(p, "%d\n", usb_power_status);
    len = p - page;

    if (off >= len) {
        *eof = 1;
        return 0;
    }

    len -= off;
    if (len > count)
        len = count;

    *start = page + off;

    return len;

}

static char *
lldp_avail_power(char *b, size_t l)
{
    unsigned lldp_w = lldp_power / 10;
    unsigned lldp_fw = lldp_power % 10;

    snprintf(b, l, "%u.%uW", lldp_w, lldp_fw);

    return b;
}

static ssize_t
aruba_proc_usb_power_write(struct file *filp, 
    const char __user *buff, unsigned long len, void *data)
{
    char value[8];
    int i;
    unsigned olen = len;

    if (len >= sizeof(value))
        return -EINVAL;

    if (copy_from_user(value, buff, len))
        return -EFAULT;

    if (value[len - 1] == '\n') {
        /* for debugging from the shell, trim a newline */
        len--;
    }
    value[len] = '\0';

    for (i = 0; i < len; i++) {
        if (!isdigit(value[i]))
            return -EINVAL;
    }

    /* assumes that all ports are the same; should move this to the AP itself */
    if (!__ap_hw_info[xml_ap_model].enets[0].dot3at)
        return olen;

    lldp_power = simple_strtoul(value, NULL, 0);

    if (power_check_timer_started && (static_power_management == 1))
        schedule_work(&power_work);

    return olen;
}

static int __init
aruba_usb_init(void)
{
    struct proc_dir_entry *entry = create_proc_entry("usb_power", 0644, NULL);
    if (!entry) {
        return -1;
    }

    entry->read_proc = aruba_proc_usb_power_read;
    entry->write_proc = aruba_proc_usb_power_write;

    return 0;
}

static int __init
aruba_usb_power_mode_init(void)
{
    struct proc_dir_entry *entry = create_proc_entry("usb_power_mode", 0644, NULL);
    if (!entry) {
        return -1;
    }

    entry->read_proc = aruba_proc_usb_power_mode_read;
    entry->write_proc = aruba_proc_usb_power_mode_write;

    return 0;
}

static void
aruba_usb_cleanup(void)
{
    remove_proc_entry("usb_power", NULL);
    remove_proc_entry("usb_power_mode", NULL);
}



//
// For production code, this should be 0.
// Set it to any valid configuration number to
// override what the GPIO pins report.
//
int aruba_power_test_mode = 0;
EXPORT_SYMBOL(aruba_power_test_mode);

static void __init
aruba_hw_load_power_pins(void)
{
    dcpin = aruba_get_gpio_pin("dc");
    afpin = aruba_get_gpio_pin("af-at");
}

/*
 * Return the power profile number from the list
 * provided by the HW team.
 *  1. DC or 802.3at
 *  2. 802.3af
 */
static enum poe_type
aruba_hw_power_status(void)
{
    int dc, at;

    if (dcpin == -1 || afpin == -1) {
        return POE_TYPE_AT;
    }

    dc = aruba_gpio_in(dcpin);
    at = !aruba_gpio_in(afpin);

    if (aruba_power_test_mode) {
        return aruba_power_test_mode;
    }

    if (dc || at) {
        /* DC or 802.3at */
        return POE_TYPE_AT;
    }
    /* 802.3af */
    return POE_TYPE_AF;
}

static unsigned
aruba_last_poll_pwr_status(void)
{
    if (static_power_management)
        return (unsigned)last_pol;
    else
        return aruba_gpio_in(afpin) ? POE_TYPE_AF : POE_TYPE_AT;
}

/* 0 => unrestricted, 1 => restricted */
unsigned
aruba_power_status(void)
{
    if (static_power_management)
        return last_pol > POE_TYPE_AT;
    else
        return ipm_power_status;
}
EXPORT_SYMBOL(aruba_power_status);

static int
aruba_get_number_of_chains(int mask)
{
    int number_of_chains = 0;

    while (mask) {
        number_of_chains++;
        mask >>= 1;
    }

    return number_of_chains;
}

static void
aruba_power_save_description(char buf[])
{
    char *p;
    char restrictions_2ghz[100], restrictions_5ghz[100], restrictions_60ghz[100],restrictions_cpu[20];

    if (usb_power_status == USB_POWER_DISABLE)
        strcat(buf, "USB power disabled; ");

    strcpy(restrictions_2ghz, "2.4GHz radio: No restrictions");
    strcpy(restrictions_5ghz, "5GHz radio: No restrictions");
	strcpy(restrictions_60ghz, "60GHz radio: No restrictions");
	

    /* See if power restrictions are in place */
    /* Since AP387 dose not have radio1, it doesnt do any radio operaiton */
    if(xml_ap_model != AP_387) {
        if ((aruba_tx_power[1] != 0xff) || (aruba_chain_mask[1] != aruba_chain_mask_max[1])) {
            strcpy(restrictions_2ghz, "2.4GHz radio:");
            if (aruba_chain_mask[1] != aruba_chain_mask_max[1]) {
                int chains = aruba_get_number_of_chains(aruba_chain_mask[1]);
                p = restrictions_2ghz + strlen(restrictions_2ghz);
                sprintf(p, " %dx%d", chains, chains);
            }
            if (aruba_tx_power[1] != 0xff) {
                p = restrictions_2ghz + strlen(restrictions_2ghz);
                sprintf(p, " %d%sdBm",
                    aruba_tx_power[1]/2, aruba_tx_power[1] & 0x1 ? ".5" : "");
            }
        }
    }

    if ((aruba_tx_power[0] != 0xff) || (aruba_chain_mask[0] != aruba_chain_mask_max[0])) {
        strcpy(restrictions_5ghz, "5GHz radio:");
        if (aruba_chain_mask[0] != aruba_chain_mask_max[0]) {
            int chains = aruba_get_number_of_chains(aruba_chain_mask[0]);
            p = restrictions_5ghz + strlen(restrictions_5ghz);
            sprintf(p, " %dx%d ", chains, chains);
        }
        if (aruba_tx_power[0] != 0xff) {
            p = restrictions_5ghz + strlen(restrictions_5ghz);
            sprintf(p, " %d%sdBm",
                aruba_tx_power[0]/2, aruba_tx_power[0] & 0x1 ? ".5" : "");
        }
    }

	if (xml_ap_model != AP_387)
		strcat(buf, restrictions_2ghz);
	else
		strcat(buf, restrictions_60ghz);
    strcat(buf, ", ");
    strcat(buf, restrictions_5ghz);

    if (pm_cpu_freq != 100) {
        sprintf(restrictions_cpu, ", CPU at %d%%", pm_cpu_freq);
        strcat(buf, restrictions_cpu);
    }
}

static void
aruba_power_status_description(enum poe_type pol, char buf[])
{
    int dc, at;

    if (dcpin == -1 || afpin == -1) {
        strcpy(buf, "Not applicable");
        return;
    }

    dc = aruba_gpio_in(dcpin);
    at = !aruba_gpio_in(afpin);

    if (pol == POE_TYPE_UNKNOWN) {
        pol = aruba_last_poll_pwr_status();
    } else {
        /* Power state as determined by Hardware of AP for ardmore and tomatin */
        if (!dc) {
            switch (pol) {
                case POE_TYPE_AT:
                    if (static_power_management)
                        strcpy(buf, "POE-AT: No restrictions");
                    else {
                        strcpy(buf, "POE-AT: ");
                        aruba_power_save_description(buf);
                    }
                    break;
                case POE_TYPE_AF:
                    if ((xml_ap_model == AP_374) || (xml_ap_model == AP_375) || (xml_ap_model == AP_377) || (xml_ap_model == AP_318)) {
                        strcpy(buf, "POE-AF: All radios disabled;");
                        return;
                    } else{
                        strcpy(buf, "POE-AF: ");
                    }
                    aruba_power_save_description(buf);
                    break;
                default:
                    /*
                     * very early, this can be 0 because things are not set up yet, so
                     * just return 'unknown'
                     */
                    strcpy(buf, "Unknown");
                    break;
            }
            return;
        }
    }

    if (dc && !aruba_power_test_mode) {
        strcpy(buf, "DC");
        return;
    }

	switch (pol) {
    case POE_TYPE_AT:
        if (static_power_management) {
            if (ap_power_mode == 1 && (dc != 1))
                sprintf(buf, "POE-AT: User Overriden: USB port %s",
                    (usb_power_status == USB_POWER_ENABLE) ?
                    "enabled" : "disabled");
            else
                strcpy(buf, "POE-AT: No restrictions");
        } else {
            strcpy(buf, "POE-AT: ");
            aruba_power_save_description(buf);
        }

        break;
    case POE_TYPE_AF:
        if ((xml_ap_model == AP_374) || (xml_ap_model == AP_375) || (xml_ap_model == AP_377) || (xml_ap_model == AP_318)) {
            strcpy(buf, "POE-AF: All radios disabled;");
            return;
        } else{
            strcpy(buf, "POE-AF: ");
        }
        aruba_power_save_description(buf);
        break;

    default:
        /*
         * very early, this can be 0 because things are not set up yet, so
         * just return 'unknown'
         */
        strcpy(buf, "Unknown");
        break;
    }
}

char *
aruba_usb_status_desc(void)
{
    switch (aruba_usb_power_mode) {
        case USB_POWER_MODE_AUTO:
            return "Auto";

        case USB_POWER_MODE_ENABLE:
            return "Enable";

        case USB_POWER_MODE_DISABLE:
            return "Disable";

        default:
            return "Unknown";
    }
}

static int 
aruba_info_show(struct seq_file *m, void *v)
{
    char buf[128];
    char *lldp_desc = "";
    unsigned dot3at;
    static int new_sbl2 = -3;

    dot3at = __ap_hw_info[xml_ap_model].enets[0].dot3at;
    
    if ( ((lldp_power >= dot3at) && (last_pol == POE_TYPE_AT)) ||
         ((lldp_power < dot3at) && (last_pol > POE_TYPE_AT) &&
            (aruba_hw_power_status() == POE_TYPE_AT)))
        lldp_desc = " (Overridden by LLDP)";

    seq_printf(m, "board name: %s\n", aruba_boardname);
    if (new_sbl2 < 0) {
        if (aruba_get_apboot_env("NEW_SBL2", buf, sizeof(buf))) {
            new_sbl2 = 1;
        } else {
            new_sbl2++;
        }
    }
    if (new_sbl2 > 0) {
        seq_printf(m, "boot version: %s/SBL\n", aruba_bootver);
    } else {
        seq_printf(m, "boot version: %s\n", aruba_bootver);
    }
    seq_printf(m, "flash: 0x%x @ 0x%x\n", aruba_flash_size, aruba_flash_base);

    aruba_power_status_description(POE_TYPE_UNKNOWN, buf);
    seq_printf(m, "power status: %s%s\n", buf, lldp_desc);
    aruba_power_status_description(aruba_hw_power_status(), buf);
    seq_printf(m, "HW POE status: %s\n", buf);
    seq_printf(m, "LLDP power: %s\n", lldp_avail_power(buf, sizeof(buf)));
    seq_printf(m, "AP Power mode Knob: %s\n",
                 (ap_power_mode) ? "Low Power Mode" : "Full Power Mode");
    seq_printf(m, "AP USB Knob: %s\n", aruba_usb_status_desc());
    return 0;
}

typedef void (*aruba_eth_callback_ptr_t)(void *, int);
static aruba_eth_callback_ptr_t aruba_eth_callback = 0;

void aruba_eth_reset_phy(struct net_device *dev, int onoff)
{
    if (aruba_eth_callback) {
        (*aruba_eth_callback)(dev, onoff);
    }
}

void aruba_set_eth_callback(aruba_eth_callback_ptr_t f) 
{
    if (aruba_eth_callback == NULL) {
        aruba_eth_callback = f;
    }
}

EXPORT_SYMBOL(aruba_set_eth_callback);

void
aruba_shutdown_phy(int pol, int onoff)
{
    struct net_device *dev0 = 0, *dev1 = 0, *dev = 0;
    int p0 = 0, p1 = 0;
    char *n;
    int pin;

    /* Glenfarclas etc. */
    if (__ap_hw_info[xml_ap_model].num_enets == 1)
        return;
	/*Sierra AP don't need shutdown eth1 phy when poe-af power supply*/
	if (sierra_ap_board)
		return;

    dev0 = dev_get_by_name(&init_net, "eth0");
    if (dev0) {
        p0 = ethtool_op_get_link(dev0);
    }
    dev1 = dev_get_by_name(&init_net, "eth1");
    if (dev1) {
        p1 = ethtool_op_get_link(dev1);
    }

    if (p0 && p1) {
        /* both ports are up, stay with port 0 */
        n = "eth1";
        dev = dev1;
        pin = 2;
    } else if (p1) {
        /* turn off p0 */
        n = "eth0";
        dev = dev0;
        pin = 3;
    } else {
        /* turn off p1 */
        n = "eth1";
        dev = dev1;
        pin = 2;
    }

    if (onoff == 0) {
        printk("Shutting down %s due to insufficient POE voltage [power profile %u]\n", 
             n, pol);
    } else {
        printk("Enabling %s due to power change [power profile %u]\n", n, pol);
    }

    if (dev) {
        aruba_eth_reset_phy(dev, onoff);
    }

    if (dev0) {
        dev_put(dev0);
    }
    if (dev1) {
        dev_put(dev1);
    }
}


typedef void (*aruba_radio_callback_ptr_t)(void *, int, int);
static aruba_radio_callback_ptr_t aruba_radio_callback = 0;

void
aruba_set_radio_callback(aruba_radio_callback_ptr_t f, void *v)
{
    int i;
    aruba_radio_callback = f;

    for (i = 0; i < _N_RADIOS; i++) {
        if (aruba_radio_ptrs[i] == 0) {
            aruba_radio_ptrs[i] = v;
            return;
        }
    }
    BUG_ON(1);
}

void
aruba_set_radio_callback_with_index(aruba_radio_callback_ptr_t f, int index, void *v)
{
    aruba_radio_callback = f;

    if (index >= _N_RADIOS) {
        BUG_ON(1);
    } else {
        if (aruba_radio_ptrs[index] == 0) {
            aruba_radio_ptrs[index] = v;
        }
    }
}

void
aruba_reset_radio_callback(void)
{
    int i;

    aruba_radio_callback = NULL;

    for (i = 0; i < _N_RADIOS; i++) {
        aruba_radio_ptrs[i] = NULL;
    }

    return;
}

int
aruba_get_allowed_chain_mask(int index)
{
    return aruba_chain_mask[index];
}

int
aruba_get_allowed_tx_power(int index)
{
    return aruba_tx_power[index];
}


extern struct aruba_tz_dump *aruba_tz_dump_data;
extern int qca_watchdog_barked;

static int
aruba_update_radios(enum pm_radios radio)
{
    if (aruba_radio_callback) {
        void *v;
        int i;

        for (i = 0; i < _N_RADIOS; i++)
            if ((radio == i) || (radio == RADIO_ALL))
                if ((v = aruba_radio_ptrs[i]))
                    (*aruba_radio_callback)(v, aruba_chain_mask[i],
                                         aruba_tx_power[i]);
        return 0;
    } else {
        return -1;    // error
    }
}

enum 
{
    PHY_DISABLE_WAIT = 0,
    PHY_DISABLE_DO,
    PHY_DISABLE_DONE,
};

static void
aruba_enforce_power_policy(void)
{
    extern void aruba_save_crash_reason(char *);
    extern void kernel_restart(char *);
    enum poe_type pol = aruba_hw_power_status();
    int dc, at;
    static int notified = 4;
    unsigned dot3at;
    static char disable_phy = PHY_DISABLE_WAIT;
    static unsigned long up_time = 0;

    dc = aruba_gpio_in(dcpin);
    at = !aruba_gpio_in(afpin);
    
    if (ap_power_mode && __ap_hw_info[xml_ap_model].enets[0].lldp_overriden_power) {
        dot3at = __ap_hw_info[xml_ap_model].enets[0].lldp_overriden_power;
    } else {
        dot3at = __ap_hw_info[xml_ap_model].enets[0].dot3at;
    }

    /* if we connected to controller and timer has not
     * started yet, we record the current time */
    if (conn_to_controller && up_time == 0) {
        up_time = jiffies;
    } else if (up_time && disable_phy == PHY_DISABLE_WAIT && 
                         up_time + PHY_DISABLE_WAIT_TIME <= jiffies) {
        /* if 1) we already start the timer,  and 
         *    2) we are still in wait state,  and 
         *    3) we pass PHY_DISABLE_WAIT_TIME seconds in the wait state 
         *
         *    then move the disable_phy to PHY_DISABLE_DO state
         */
        disable_phy = PHY_DISABLE_DO;
    }

    if (pol == last_pol) {
        /* if there is no power state change, we check if disable_phy is moved to
         * PHY_DISABLE_DO state. if yes, we go ahead to disable the phy and
         * update the state to PHY_DISABLE_DONE then finish.
         */
        if (disable_phy == PHY_DISABLE_DO && last_pol == POE_TYPE_AF) {
            goto enforce;
        } 
    }

    /* check if DC showed up so that we can reboot */
    if ((lldp_power >= dot3at) && !dc) {
        /* enable more functionality since LLDP says its OK */
        pol = POE_TYPE_AT;
        if (pol != last_pol) {
            goto enforce;
        }
    }
    
    if (lldp_power && (lldp_power < dot3at) && !dc && !at) {
        /* disable functionality since LLDP says its not OK */
        pol = POE_TYPE_AF;
        if (last_pol == POE_TYPE_UNKNOWN) {
            goto enforce;
        }
    }


    if (pol < last_pol) {
        char buf[128];
        /*
         * NOTE: LLDP can override this logic
         * For the time being,  we never re-enable functionality even
         * if conditions improve, but let's be nice and inform the customer
         * about it
         */
        if (pol < notified) {
            aruba_power_status_description(pol,buf);
            printk(KERN_NOTICE "POE power conditions have improved: the new condition would be '%s'\n", buf);
            notified = pol;
        }

        /* reboot AP if the power condition has changed from POE to DC */
        if (dc) {
            snprintf(buf, sizeof(buf), "Rebooting AP since power has changed from POE to DC\n");
            printk(KERN_NOTICE "%s", buf);
            aruba_save_crash_reason(buf);
            kernel_restart(NULL);
        }
        return;
    }
    if (pol == last_pol && dc == last_dc) {
        /*
         * status has not changed, so nothing to do
         */
        return;
    }
enforce:
    switch (pol) {
    case POE_TYPE_AT:
        /* lower power mode and we have POE AT power */
        if (ap_power_mode == 1 && !dc) {
            if (aruba_usb_power_mode == USB_POWER_MODE_DISABLE || aruba_usb_power_mode == USB_POWER_MODE_AUTO) {
                /* disable USB since we are in low power mode or USB knob is disabled*/
                aruba_usb_power_control(0);
            } else if (aruba_usb_power_mode == USB_POWER_MODE_ENABLE) {
                /* Reduced Power Mode however USB global knob says to enable USB */
                aruba_usb_power_control(1);
            }
        } else {
            /* if global usb knob is auto or on we enable the USB */
            if (aruba_usb_power_mode != USB_POWER_MODE_DISABLE) {
                /* enable USB */
                aruba_usb_power_control(1);
            }
        }

        aruba_shutdown_phy(pol, 1);

        aruba_chain_mask[0] = aruba_chain_mask_max[0];
        aruba_chain_mask[1] = aruba_chain_mask_max[1];
        aruba_tx_power[0] = 0xff;  /* 0xff means full tx power */
        aruba_tx_power[1] = 0xff;
        break;
    case POE_TYPE_AF:
        /* global usb-knob is not enabled so disable USB due to 
         * lack of power */
        if (aruba_usb_power_mode != USB_POWER_MODE_ENABLE) {
            /* disable USB */
            aruba_usb_power_control(0);
        }

        /* disable Ethernet port */
        if (disable_phy) {
            aruba_shutdown_phy(pol, 0);
            disable_phy = PHY_DISABLE_DONE;
        }

        /* power save mode */
        aruba_chain_mask[0] = ((0x1 << __ap_hw_info[xml_ap_model].radios[0].max_tx_chains_under_af) - 1);
        aruba_chain_mask[1] = ((0x1 << __ap_hw_info[xml_ap_model].radios[1].max_tx_chains_under_af) -1);
        aruba_tx_power[0] = __ap_hw_info[xml_ap_model].radios[0].max_tx_pwr_under_af;
        aruba_tx_power[1] = __ap_hw_info[xml_ap_model].radios[1].max_tx_pwr_under_af;
        break;
    default:
        break;
    }
   
    if (aruba_update_radios(RADIO_ALL) < 0) {
        /* Radio update not happened. Do not update policy number */
        last_pol = POE_TYPE_UNKNOWN;
    }
    else {
        last_pol = pol;
        last_dc = dc;
    }
}

#define ARUBA_POWER_CHECK_TIME 60    /* seconds */

static struct timer_list aruba_power_timer;

static void 
aruba_periodic_power_check(unsigned long arg)
{
    if (!static_power_management)
        return;

    /* 
     * schedule workqueue to enforce power policy change
     * the ethernet mdio port cannot access within timer interrupt
     */
    schedule_work(&power_work);
    mod_timer(&aruba_power_timer, jiffies + (ARUBA_POWER_CHECK_TIME * HZ));
}

void
aruba_start_power_check(void)
{
    char *p;
    int dc;
    char buf[20];

    if (!static_power_management)
        return;

    p = aruba_get_apboot_env("ap_power_mode", buf, sizeof(buf));
    if (p) {
        ap_power_mode = simple_strtol(p, 0, 0);
    } else {
        ap_power_mode = 0;
    }

    if (dcpin == -1) {
        return;
    }

    dc = aruba_gpio_in(dcpin);

    aruba_enforce_power_policy();

    if (!power_check_timer_started) {
        power_check_timer_started = 1;
        init_timer(&aruba_power_timer);
        aruba_power_timer.data = 0;
        aruba_power_timer.function = aruba_periodic_power_check;
    }

    mod_timer(&aruba_power_timer, jiffies + (ARUBA_POWER_CHECK_TIME * HZ));
}

/* Dynamic Power Management Functions */

static int
aruba_set_usb(enum pm_state state)
{
    aruba_usb_power_control(state);
    return 0;
}

static void
aruba_set_radio_2ghz_power(enum pm_radio_power power)
{
    switch (power) {
    case PM_RADIO_POWER_FULL:
        aruba_tx_power[1] = 0xff;
        break;
    case PM_RADIO_REDUCED_3DB:
        aruba_tx_power[1] = __ap_hw_info[xml_ap_model].radios[1].max_tx_pwr_under_af;
        break;
    case PM_RADIO_REDUCED_6DB :
        aruba_tx_power[1] = __ap_hw_info[xml_ap_model].radios[1].max_tx_pwr_under_af - 6;
        break;
    }
}

static void
aruba_set_radio_5ghz_power(enum pm_radio_power power)
{
    switch (power) {
    case PM_RADIO_POWER_FULL:
        aruba_tx_power[0] = 0xff;
        break;
    case PM_RADIO_REDUCED_3DB:
        /*
         * max_tx_power_under_af is actually set to the max so we need to
         * subtract another 3dB
         */
        aruba_tx_power[0] = __ap_hw_info[xml_ap_model].radios[0].max_tx_pwr_under_af - 6;
        break;
    case PM_RADIO_REDUCED_6DB :
        aruba_tx_power[0] = __ap_hw_info[xml_ap_model].radios[0].max_tx_pwr_under_af - 12;
        break;
    }
}

static void
aruba_set_radio_2ghz_chain(enum pm_radio_chains chains)
{
    aruba_chain_mask[1] = (1 << (chains+1)) - 1;
}

static void
aruba_set_radio_5ghz_chain(enum pm_radio_chains chains)
{
    aruba_chain_mask[0] = (1 << (chains+1)) - 1;
}

static int
aruba_throttle_cpu(char *speed)
{
    char tcmd[100];
    char *sh_cmd[4];
    int ret;

    /* CPU 0 */
    sprintf(tcmd,
    "/bin/echo %s > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed",
     speed);

    sh_cmd[0] = "/bin/sh";
    sh_cmd[1] = "-c";
    sh_cmd[2] = tcmd;
    sh_cmd[3] = NULL;

    ret = call_usermodehelper(sh_cmd[0], sh_cmd, NULL, UMH_WAIT_PROC);

    if (ret)
        return ret;

    /* CPU 1 */
    sprintf(tcmd,
    "/bin/echo %s > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed",
     speed);

    return call_usermodehelper(sh_cmd[0], sh_cmd, NULL, UMH_WAIT_PROC);
}

static int
aruba_throttle_cpu_100(void)
{
    pm_cpu_freq = 100;
    return aruba_throttle_cpu(PM_FREQ_CPU_100);
}

static int
aruba_throttle_cpu_75(void)
{
    pm_cpu_freq = 75;
    return aruba_throttle_cpu(PM_FREQ_CPU_75);
}

static int
aruba_throttle_cpu_50(void)
{
    pm_cpu_freq = 50;
    return aruba_throttle_cpu(PM_FREQ_CPU_50);
}

static int
aruba_throttle_cpu_25(void)
{
    pm_cpu_freq = 25;
    return aruba_throttle_cpu(PM_FREQ_CPU_25);
}

static int
aruba_get_power_threshold(void)
{
    int at;

    if (aruba_gpio_in(dcpin))
        return PM_POWER_DC;

    /*
     * Some POE switches respond in the Power over MDI TLV with the current
     * power usage instead of the maximum allowed usage. Ignore any value
     * less than the POE AF maximum power
     */
    if (lldp_power > POE_AF_MAX_POWER)
        return lldp_power;

    at = !aruba_gpio_in(afpin);

    if (at) {
        if (ap_power_mode && __ap_hw_info[xml_ap_model].enets[0].lldp_overriden_power)
            return __ap_hw_info[xml_ap_model].enets[0].lldp_overriden_power;
        else
            return __ap_hw_info[xml_ap_model].enets[0].dot3at;
    }

    return POE_AF_MAX_POWER;
}

static int
aruba_start_static_power_management(void)
{
    static_power_management = 1;
    last_pol = POE_TYPE_UNKNOWN;
    aruba_throttle_cpu_100();
    aruba_start_power_check();
    return 0;
}

static int
aruba_stop_static_power_management(void)
{
    static_power_management = 0;
    cancel_work_sync(&power_work);
    return 0;
}

#define M_SCALE_FACTOR       1000

static uint32_t
aruba_get_eff(uint32_t power)
{
    /* TODO: need H/W team measurements, assume 85% */
    return 850;
}

static uint32_t
aruba_get_poe_power(uint32_t measured_power)
{
    return (measured_power * EFF_SCALE_FACTOR)/aruba_get_eff(measured_power);
}

static uint32_t
aruba_get_measured_power(uint32_t poe_power)
{
    return (poe_power * aruba_get_eff(poe_power))/EFF_SCALE_FACTOR;
}

static void
aruba_set_power_status(enum pm_status status)
{
    ipm_power_status = status;
}

struct pm_functions ap_pm_functions = {
    .set_usb = aruba_set_usb,
    .set_radio_2ghz_power = aruba_set_radio_2ghz_power,
    .set_radio_5ghz_power = aruba_set_radio_5ghz_power,
    .set_radio_2ghz_chain = aruba_set_radio_2ghz_chain,
    .set_radio_5ghz_chain = aruba_set_radio_5ghz_chain,
    .update_radios = aruba_update_radios,
    .throttle_cpu_100 = aruba_throttle_cpu_100,
    .throttle_cpu_75 = aruba_throttle_cpu_75,
    .throttle_cpu_50 = aruba_throttle_cpu_50,
    .throttle_cpu_25 = aruba_throttle_cpu_25,
    .get_power_threshold = aruba_get_power_threshold,
    .get_poe_power = aruba_get_poe_power,
    .get_measured_power = aruba_get_measured_power,
    .start_static_power_management = aruba_start_static_power_management,
    .stop_static_power_management = aruba_stop_static_power_management,
    .set_power_status = aruba_set_power_status
};

const struct pm_functions *aruba_pm_functions_get(void)
{
    int radio, antenna;
    int num_radios, num_antennas;

    num_radios = __ap_hw_info[xml_ap_model].num_radios;
    BUG_ON(num_radios > PM_MAX_RADIOS);

    for (radio = 0; radio < num_radios; radio++) {
        num_antennas = __ap_hw_info[xml_ap_model].radios[radio].num_antennas;
        BUG_ON(num_antennas > PM_MAX_ANTENNAS);

        for (antenna = 0;  antenna < num_antennas; antenna++)
            if (aruba_chain_mask_max[radio] & 1<<antenna)
                ap_pm_functions.available_radio_chain_masks[radio][antenna] = 1;
            else
                ap_pm_functions.available_radio_chain_masks[radio][antenna] = 0;
    }

    return &ap_pm_functions;
}

EXPORT_SYMBOL(aruba_set_radio_callback);
EXPORT_SYMBOL(aruba_set_radio_callback_with_index);
EXPORT_SYMBOL(aruba_reset_radio_callback);
EXPORT_SYMBOL(aruba_start_power_check);
EXPORT_SYMBOL(aruba_get_allowed_chain_mask);
EXPORT_SYMBOL(aruba_get_allowed_tx_power);
EXPORT_SYMBOL(aruba_pm_functions_get);

static int __init
aruba_get_reboot_reason(void)
{
    int power_status_pin;
    int wdt_status_pin;
    int power_clr_pin;
    int wdt_clr_pin;

    if (xml_ap_model == AP_385) {
        return 0;
    }

    power_status_pin = aruba_get_gpio_pin("por-sts");
    wdt_status_pin = aruba_get_gpio_pin("wdt-sts");
    power_clr_pin = aruba_get_gpio_pin("por-sts-clr");
    wdt_clr_pin = aruba_get_gpio_pin("wdt-sts-clr");

    if (power_status_pin == -1 || wdt_status_pin == -1 
            || power_clr_pin == -1 || wdt_clr_pin == -1)
    {
        printk("Can't get the gpio pins for reboot reason\n");
        return -1;
    }

    if (aruba_gpio_in(wdt_status_pin)) {
        ap_reboot_reason = AP_REBOOT_WDT;
        /* clear wdt reset status - plus low */
        aruba_gpio_out(wdt_clr_pin, 0);
        udelay(500);
        aruba_gpio_out(wdt_clr_pin, 1);

    } else if(!aruba_gpio_in(power_status_pin)) {
        ap_reboot_reason = AP_REBOOT_COLD;
        /* clear power reset status */
        aruba_gpio_out(power_clr_pin, 1);
    } else {
        /* read WDT0 STS register to check RST_STS(bit 0) 
         * bit 0: 1 - wdt(the last system reset was due to WDT0 
         *        0 - reset(the last system reset was not due to WDT0
         */
        if ((__raw_readl(MSM_TMR0_BASE + WDT0_OFFSET + WDT_STS)) & 0x1) {
            ap_reboot_reason = AP_REBOOT_CPU_WDT;
        }
    }

    /* check watchdog dump data */
    if ((ap_reboot_reason != AP_REBOOT_COLD) && qca_watchdog_barked) {
        ap_reboot_reason = AP_REBOOT_CPU_WDT;
    }

    return 0;
}

static int 
aruba_reboot_reason_show(struct seq_file *m, void *v)
{
    switch (ap_reboot_reason) {
        case AP_REBOOT_COLD:
            seq_printf(m, "AP rebooted caused by cold HW reset(power loss) %s\n", ap_reboot_time);
            break;
        case AP_REBOOT_WDT:
            seq_printf(m, "AP rebooted caused by external watchdog reset %s\n", ap_reboot_time);
            break;
        case AP_REBOOT_CPU_WDT:
            seq_printf(m, "AP rebooted caused by internal watchdog reset\n");
            break;
        default:
            seq_printf(m, "AP rebooted caused by warm reset %s\n", ap_reboot_time);
            break;
    }
    
    return 0;
}

static int 
aruba_reboot_reason_open(struct inode *inode, struct file *file)
{
    return single_open(file, aruba_reboot_reason_show, NULL);
}

static struct file_operations aruba_reboot_reason_operations = {
    .open       = aruba_reboot_reason_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int 
aruba_watchdog_dump_show(struct seq_file *m, void *v)
{
    if (AP_REBOOT_CPU_WDT == ap_reboot_reason) {
        int i;
        struct qca_tz_watchdog_dump *qca_dump_data = (struct qca_tz_watchdog_dump *)aruba_tz_dump_data;
        unsigned int *p = (unsigned int*)qca_dump_data;

        seq_printf(m, "AP rebooted caused by internal watchdog reset\n");
        seq_printf(m, "OS Version:%s\n", ARUBA_VERSION);

        seq_printf(m, "Uname:%s %s %s %s %s %s\n",
                init_uts_ns.name.sysname,
                init_uts_ns.name.nodename,
                init_uts_ns.name.release,
                init_uts_ns.name.version,
                init_uts_ns.name.machine,
                strcmp(init_uts_ns.name.domainname, "(none)") == 0 
                ? "unknown" : init_uts_ns.name.domainname);

        if (unlikely(!qca_dump_data)) {
            seq_printf(m, "Dump Data:\n");
            return 0;
        }

        seq_printf(m, "Dump Data:\n");
        for (i = 0; i < sizeof(struct qca_tz_watchdog_dump)/4; i++, p++) {
            seq_printf(m, "%08x ", *p);
            if (i%4 == 3) {
                seq_printf(m, "\n");
            }
        }

        seq_printf(m, "\n");
        if (aruba_tz_dump_data->jiffies64) {
            seq_printf(m, "jiffies64 = %lld\n", aruba_tz_dump_data->jiffies64);
            seq_printf(m, "watchdog pet history: %s\n", aruba_tz_dump_data->wdg_history);
            seq_printf(m, "kernel log index:  %d\n", aruba_tz_dump_data->kernel_log_idx);
            seq_printf(m, "kernel log:  \n %s\n", aruba_tz_dump_data->kernel_log);
        }

        /* check watchdog dump data */
        if (qca_watchdog_barked) {
            for (i = 0; i < CONFIG_NR_CPUS; i++) {
                struct pt_regs regs;
                //char buf[4096];
                regs.ARM_pc = qca_dump_data->reg_dump[i].magic;
                regs.ARM_lr = qca_dump_data->reg_dump[i].svc_r14;
                regs.ARM_r0 = qca_dump_data->reg_dump[i].usr_r0;
                regs.ARM_r1 = qca_dump_data->reg_dump[i].usr_r1;
                regs.ARM_r2 = qca_dump_data->reg_dump[i].usr_r2;
                regs.ARM_r3 = qca_dump_data->reg_dump[i].usr_r3;
                regs.ARM_r4 = qca_dump_data->reg_dump[i].usr_r4;
                regs.ARM_r5 = qca_dump_data->reg_dump[i].usr_r5;
                regs.ARM_r6 = qca_dump_data->reg_dump[i].usr_r6;
                regs.ARM_r7 = qca_dump_data->reg_dump[i].usr_r7;
                regs.ARM_r8 = qca_dump_data->reg_dump[i].usr_r8;
                regs.ARM_r9 = qca_dump_data->reg_dump[i].usr_r9;
                regs.ARM_r10 = qca_dump_data->reg_dump[i].usr_r10;
                regs.ARM_fp = qca_dump_data->reg_dump[i].usr_r11;
                regs.ARM_ip = qca_dump_data->reg_dump[i].usr_r12;
                regs.ARM_sp = qca_dump_data->reg_dump[i].svc_r13;
                if (aruba_tz_dump_data->jiffies64) {
                    regs.ARM_sp = (unsigned int)aruba_tz_dump_data->stacks[i] + (regs.ARM_sp - (regs.ARM_sp & ~(THREAD_SIZE - 1)));
                }
                regs.ARM_cpsr = qca_dump_data->reg_dump[i].curr_cpsr;

                seq_printf(m, "CPU %d:\n", i);
                
                sprint_symbol(buf, regs.ARM_pc);
                seq_printf(m, "PC is at %s\n", buf);
                sprint_symbol(buf, regs.ARM_lr);
                seq_printf(m, "LR is at %s\n", buf);
                seq_printf(m, "pc : [<%08lx>]    lr : [<%08lx>]    psr: %08lx\n"
                        "sp : %08lx  ip : %08lx  fp : %08lx\n",
                        regs.ARM_pc, regs.ARM_lr, regs.ARM_cpsr,
                        regs.ARM_sp, regs.ARM_ip, regs.ARM_fp);
                seq_printf(m, "r10: %08lx  r9 : %08lx  r8 : %08lx\n",
                        regs.ARM_r10, regs.ARM_r9,
                        regs.ARM_r8);
                seq_printf(m, "r7 : %08lx  r6 : %08lx  r5 : %08lx  r4 : %08lx\n",
                        regs.ARM_r7, regs.ARM_r6,
                        regs.ARM_r5, regs.ARM_r4);
                seq_printf(m, "r3 : %08lx  r2 : %08lx  r1 : %08lx  r0 : %08lx\n",
                        regs.ARM_r3, regs.ARM_r2,
                        regs.ARM_r1, regs.ARM_r0);

                if (aruba_tz_dump_data->jiffies64) {
#ifdef CONFIG_ARM_UNWIND
                    sprint_unwind_backtrace_regs(&regs, NULL, buf, sizeof(buf));
#else
                    sprint_backtrace_regs(regs, NULL, buf, sizeof(buf));
#endif
                    seq_printf(m, "Backstrace: \n%s\n", buf);
                }
            }
        }
    }
    
    return 0;
}

static int 
aruba_watchdog_dump_open(struct inode *inode, struct file *file)
{
    return single_open(file, aruba_watchdog_dump_show, NULL);
}

static struct file_operations aruba_watchdog_dump_operations = {
    .open       = aruba_watchdog_dump_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int arubacmd_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "OK\n");
    return 0;
}

static int arubacmd_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, arubacmd_proc_show, NULL);
}

void    cn_dump_memory(void *buf, int size)
{
    unsigned char *bp = buf;
    unsigned char *prev = NULL;
    int    i, len;
    int repeat=0;

    while (size)
    {
        len = size > 16 ? 16 : size;

        if (size>16 && prev) /* handle repetitions */
        {
            if (memcmp(prev, bp, 16)==0)
            {
                repeat++;
                bp += 16;
                size -= len;
                continue;
            }
        }
        if (repeat)
            printk("<repeated %x times>\n", repeat);
        repeat = 0;
        prev = bp;

        printk("%4x ", (int)(bp - (unsigned char *)buf));
        for (i=0 ; i<len ; i++)
            printk(" %02x", *bp++);

        for (i=len ; i<16 ; i++)
            printk("   ");

        printk(" ");
        for (i=0, bp-=len ; i<len ; i++, bp++)
            if (*bp>=32 && *bp<127)
                printk("%c", *bp);
            else
                printk(".");

        printk("\n");
        size -= len;
    }
}

static struct timer_list aruba_writeint_timer;

static void 
aruba_writeint_timer_handler(unsigned long arg)
{
    int *p=(int *)arg;

    printk("Kernel writing at %lx cpu%d\n", arg, smp_processor_id());
    *p = 0;
}


/* circular buffer to trace programs and save more panic/softdog info */
#define COLUBRIS_ZONE_BUF_SIZE  8192
char cn_zone_backtrace_buf[COLUBRIS_ZONE_BUF_SIZE/2];
static int force_stack_magic = 0;
static int force_stack_overflow = 0;

/*
add nul terminated string to colubris zone
can be called from interrupt context
*/
void    cn_zone_str(char *str)
{
    printk( "%s\n", str);
}

void    cn_zone_print_backtrace_regs(struct pt_regs *regs, struct task_struct *tsk)
{
    cn_zone_backtrace_buf[0] = 0;
#ifdef CONFIG_ARM_UNWIND
    sprint_unwind_backtrace_regs(regs, tsk, cn_zone_backtrace_buf, sizeof(cn_zone_backtrace_buf));
#else
    sprint_backtrace_regs(regs, tsk, cn_zone_backtrace_buf, sizeof(cn_zone_backtrace_buf));
#endif
    cn_zone_str(cn_zone_backtrace_buf);
}

unsigned long cn_board_get_sp(void)
{
    unsigned long sp;

#if defined(CONFIG_PPC)
    asm("mr %0,1" : "=r" (sp) :);
#elif defined(CONFIG_X86)
    sp = (unsigned long)&sp;
#elif defined(CONFIG_MIPS)
    sp = (unsigned long)&sp;
#elif defined(CONFIG_ARM)
    asm("mov %0, sp" : "=r"(sp) : );
#endif
    return sp;
}

void    cn_board_check_stack_magic(void)
{
    static int cstack_corrupted;
    unsigned long flags;
    struct task_struct *tsk = current;
    unsigned long stack = (unsigned long)tsk->stack;
    unsigned long stack_hi = stack + THREAD_SIZE;
    unsigned long stack_lo = stack + sizeof(struct thread_info);
    unsigned long sp = cn_board_get_sp();
    unsigned long *stackend = end_of_stack(tsk); /* see fork.c */

    if (tsk->pid==0)
        return;

    if ( (*stackend != STACK_END_MAGIC) || (force_stack_magic))
    {
        if (cstack_corrupted==0) /* do not repeat */
        {
            static spinlock_t check_stack_buffer_lock = __SPIN_LOCK_UNLOCKED(check_stack_buffer_lock);
            static char buffer[128];

            spin_lock_irqsave(&check_stack_buffer_lock, flags);
            cstack_corrupted = 1;
        
            sprintf(buffer, "Kernel stack corrupted: process %.16s, pid %d\n"
                   " stack_lo %08lx sp %08lx stack_hi %08lx\n",
                   current->comm, current->pid,
                   stack_lo, sp, stack_hi);

            printk("%s", buffer);
            spin_unlock_irqrestore(&check_stack_buffer_lock, flags);

            #ifdef CONFIG_ARM
                cn_zone_print_backtrace_regs(NULL, current);
            #else
                cn_zone_print_backtrace_addr(tsk, (void *)sp);
            #endif
        
            panic("%s", buffer);
        }
    }
}

void    cn_board_check_for_stack_overflow(char *name)
{
    static int cstack_overflowed;
    unsigned long flags;
    struct task_struct *tsk = current;
    unsigned long stack = (unsigned long)tsk->stack;
    unsigned long stack_hi = stack + THREAD_SIZE;
    unsigned long stack_lo = stack + sizeof(struct thread_info);
    unsigned long sp = cn_board_get_sp();

    if (tsk->pid==0)
        return;

    if ( (sp > stack_hi || sp < stack_lo + 1536) || (force_stack_overflow))
    {
        if (cstack_overflowed==0)   /* do not repeat myself */
        {
            static spinlock_t check_stack_buffer_lock = __SPIN_LOCK_UNLOCKED(check_stack_buffer_lock);
            static char buffer[128];

            spin_lock_irqsave(&check_stack_buffer_lock, flags);
            cstack_overflowed = 1;

            sprintf(buffer, "Kernel stack overflow at %s: process %.16s, pid %d\n"
                   " stack_lo %08lx sp %08lx stack_hi %08lx\n",
                   name,
                   current->comm, current->pid,
                   stack_lo, sp, stack_hi);

            cn_zone_str(buffer);
            printk("%s", buffer);
            spin_unlock_irqrestore(&check_stack_buffer_lock, flags);

            #ifdef CONFIG_ARM
                cn_zone_print_backtrace_regs(NULL, current);
            #else
                cn_zone_print_backtrace_addr(tsk, (void *)sp);
            #endif

            panic("%s", buffer);
        }
    }
}

int sprintf_dump_backtrace_entry(char *buf, unsigned long where, unsigned long from, unsigned long frame)
{
    int len=0;
#ifdef CONFIG_KALLSYMS
    char sym1[KSYM_SYMBOL_LEN], sym2[KSYM_SYMBOL_LEN];
    sprint_symbol(sym1, where);
    sprint_symbol(sym2, from);
    len += sprintf(buf+len, "[<%08lx>] (%s) from [<%08lx>] (%s)\n", where, sym1, from, sym2);
#else
    len += sprintf(buf+len, "Function entered at [<%08lx>] from [<%08lx>]\n", where, from);
#endif
    return len;
}

void    cn_board_print_sp_position(char *name)
{
    struct task_struct *tsk = current;
    unsigned long stack = (unsigned long)tsk->stack;
    unsigned long stack_hi = stack + THREAD_SIZE;
    unsigned long stack_lo = stack + sizeof(struct thread_info);
    unsigned long sp;

    if (tsk->pid==0)
        return;

    sp = cn_board_get_sp();
    printk("Kernel stack at %s: process %.16s, pid %d stack_lo %08lx sp %08lx stack_hi %08lx use %ld%%\n",
           name,
           current->comm, current->pid,
           stack_lo, sp, stack_hi, (stack_hi-sp)*100/(stack_hi-stack_lo));
}

void    small_recursive_loop(int depth)
{
    unsigned long i_time = jiffies;
    char buffer[4];

    /* TEST ONLY */
    buffer[sizeof(buffer)-1] = 0xee;
    if ((depth & 0xff)==0)
    {
        printk("depth %d\n", depth);
        while (jiffies==i_time) /* wait a jiffy for stack detection */
            ;
    }
    small_recursive_loop(depth+1);
    if ((depth & 0xfff)==0) /* do not let compiler optimize stack away */
    {
        printk("depth %d returned\n", depth);
    }
}

int big_recursive_loop(void *start, int depth)
{
    unsigned long big_start = (unsigned long)start;
    char buffer[0x100];
    int ret = 0;

    /* TEST ONLY */
    buffer[sizeof(buffer)-1] = 0xee;
    if (big_start - (unsigned long)&big_start < depth)
    {
//      printk("Depth %lx\n", big_start - (unsigned long)&big_start);
        cn_board_print_sp_position((char *)__FUNCTION__);
        cn_board_check_for_stack_overflow((char *)__FUNCTION__);
        ret += big_recursive_loop(start, depth);
    }
    else
    {
        unsigned long i_time = jiffies;

        printk("Waiting 5 secs\n");
        while (jiffies-i_time < 5*HZ)
            ;
        printk("Exiting\n");
    }
    if ((depth & 0xfff)==0) /* do not let compiler optimize stack away */
    {
        printk("depth %d returned\n", depth);
    }
    return ret;
}

#pragma GCC diagnostic push
/* GB2014, Dont tell me that the stack frame is big, this is intended... */
#pragma GCC diagnostic ignored "-Wframe-larger-than="
void    big_stack_16K(void)
{
    char buffer[0x4000];

    /* TEST ONLY */
    buffer[sizeof(buffer)-1] = 0xee;
    {
        unsigned long i_time = jiffies;

        printk("Waiting 5 secs, buffer[%p-%p], last byte %02x\n",
            buffer, &buffer[sizeof(buffer)-1], buffer[sizeof(buffer)-1]);
        while (jiffies-i_time < 5*HZ)
            ;
        printk("Exiting\n");
    }
}
#pragma GCC diagnostic pop

static struct timer_list heartbeat_timer;

static void board_heartbeat_timer(unsigned long data)
{
    cn_board_check_stack_magic();    /* check stack of current process */
    cn_board_check_for_stack_overflow("tick");
    mod_timer(&heartbeat_timer, jiffies + 1);
}

void __init
board_setup_heartbeat(void)
{
    init_timer(&heartbeat_timer);
    heartbeat_timer.function = board_heartbeat_timer;
    mod_timer(&heartbeat_timer, jiffies + 1);
}

#define  ARUBACMDLEN       64
struct arubacmdwork {
    struct work_struct work;    /* must be first to allow typecast */
    char buffer[ARUBACMDLEN];
    int count;
};

static ssize_t
arubacmd_proc_write(struct file *file, const char __user *userbuffer,
                    size_t count, loff_t *pos);

static void
arubacmd_do_work(struct work_struct *work)
{
    struct arubacmdwork *cmdwork = (struct arubacmdwork *)work;

    arubacmd_proc_write(NULL, cmdwork->buffer, cmdwork->count, 0);
    kfree(work);
}

static ssize_t
arubacmd_proc_write(struct file *file, const char __user *userbuffer,
                    size_t count, loff_t *pos)
{
    char buffer[ARUBACMDLEN];
    int readcount = count > sizeof(buffer)-1 ? sizeof(buffer)-1 : count;

    if (file)
    {
        if (copy_from_user(buffer, userbuffer, readcount))
            return -EFAULT;
    }
    else /* re-entered from do_work */
        memcpy(buffer, userbuffer, readcount);

    buffer[readcount] = 0;
#if 0
    printk("cpu%d cmd: %s\n", smp_processor_id(), buffer);
#endif
    if (memcmp(buffer,"oncpu",5) == 0)    /* oncpuN:COMMAND */
    {
        char *ecp=NULL;
        int cpu = simple_strtoul(buffer+5, &ecp, 16);

        if (ecp && *ecp==':')
        {
            struct arubacmdwork *cmdwork = kmalloc(sizeof(*cmdwork), GFP_ATOMIC);
            if (cmdwork)
            {
                int consumed = ecp+1-buffer;

                INIT_WORK(&cmdwork->work, arubacmd_do_work);
                memcpy(cmdwork->buffer, buffer+consumed, count-consumed);
                cmdwork->count = count-consumed;
                schedule_work_on(cpu, &cmdwork->work);
            }
        }
        return count;
    }

    if(memcmp(buffer,"setwatch",8) == 0)    /* setwatchADDR[,SIZE,[1(w)2(r)3(rw)]] */
    {
        char *ecp;
        int addr = 0;
        int size = 1;
        int type = 1;

        addr = simple_strtoul(buffer+8, &ecp, 16);
        if (ecp && *ecp==',')
        {
            size = simple_strtoul(ecp+1, &ecp, 16);
            if (*ecp == ',')
            {
                type = simple_strtoul(ecp+1, NULL, 16);
            }
        }

        printk("cpu%d set_watch(%x, %x, %x)\n", smp_processor_id(), addr, size, type);
        aruba_set_watch((void *)addr, size, type);
        return count;
    }

    if(memcmp(buffer,"clrwatch",8) == 0)    /* clrwatch */
    {
        printk("cpu%d clear_watch\n", smp_processor_id());
        aruba_clear_watch();
        return count;
    }

    if(memcmp(buffer,"writeint",8) == 0)    /* writeintADDR -- write at ADDR from interrupt timer context */
    {
        int addr = 0;

        addr = simple_strtoul(buffer+8, NULL, 16);
        init_timer(&aruba_writeint_timer);
        aruba_writeint_timer.data = addr;
        aruba_writeint_timer.function = aruba_writeint_timer_handler;
        mod_timer(&aruba_writeint_timer, jiffies + 1);
        return count;
    }

    if(memcmp(buffer,"panicnow",8) == 0)
    {
        panic("panicnow: testing the panic mechanism");
        return count;
    }

    if(memcmp(buffer,"loopuser",8) == 0)    /* loopuser[SECONDS] */
    {
        /* Send user code into an infinite loop.
        SOFTDOG timer should trigger.
        */
        long seconds = simple_strtoul(buffer+8, NULL, 16);

        if (seconds==0)
        {
            printk("Looping user code cpu%d\n", smp_processor_id());
            for ( ; ; )
                ;
        }
        else
        {
            unsigned long itime = jiffies;

            printk("Looping user code cpu%d for %ld seconds\n", smp_processor_id(), seconds);
            for ( ; (jiffies - itime)/HZ < seconds; )
                ;
            printk("End of loop\n");
        }
        return count;
    }

    if(memcmp(buffer,"loopintoff",10) == 0)
    {
        /* Send user code into infinite loop with interrupts off.
        HW watchdog should trigger.
        */
        printk("Looping user code with interrupts off cpu%d\n", smp_processor_id());
        local_irq_disable();
        for ( ; ; )
            ;
        return count;
    }

    if (memcmp(buffer,"dm",2) == 0)    /* dmADDR[,LEN] */
    {
        char *ecp = NULL;
        int *addr = (void *)simple_strtoul(buffer+2, &ecp, 16);
        int cpu = smp_processor_id();

        if (ecp && *ecp==',')
        {
            int len = 16;

            len = simple_strtoul(ecp+1, NULL, 16);
            printk("cpu%d dm %08x,%08x\n", cpu, (int)addr, len);
            cn_dump_memory(addr, len);
            return count;
        }
        printk("cpu%d dm %08x\n", cpu, (int)addr);
        printk("cpu%d dm %08x: %08x\n", cpu, (int)addr, *addr);
        return count;
    }

    if (memcmp(buffer,"mm",2) == 0)    /* mmADDR=VAL[,LEN] */
    {
        char *cp;
        char *ecp = NULL;
        int addr = simple_strtoul(buffer+2, &cp, 16);
        uint8_t *addr8;
        uint16_t *addr16;
        uint32_t *addr32;
        int val = 0;
        int len = sizeof(int);
        int cpu = smp_processor_id();

        if (*cp == '=')
            val = simple_strtoul(cp+1, &ecp, 16);

        if (ecp && *ecp==',')
            len = simple_strtoul(ecp+1, NULL, 16);

        printk("cpu%d mm %08x=%x,%x\n", cpu, addr, val, len);
        switch(len)
        {
        case 1:
            addr8 = (uint8_t *)addr;
            *addr8 = val;
            printk("cpu%d mm %08x: %02x\n", cpu, (int)addr8, *addr8);
            break;
        case 2:
            addr16 = (uint16_t *)(addr & ~1);
            *addr16 = val;
            printk("cpu%d mm %08x: %04x\n", cpu, (int)addr16, *addr16);
            break;
        default:
            addr32 = (uint32_t *)(addr & ~3);
            *addr32 = val;
            printk("cpu%d mm %08x: %08x\n", cpu, (int)addr32, *addr32);
            break;
        }
        return count;
    }

    if (memcmp(buffer,".", 1)==0)    /* .text : printk(text) */
    {
        int len = count-1;
        printk("%*.*s", len, len, buffer+1);
        return count;
    }

    if (memcmp(buffer, "kmalloc", 7)==0) /* kmallocSIZE[,FILL[,ECOUNT] */
    {
        int size = 0;
        int fill = 0;
        int ecount = 1;
        char *ecp;
        void *buf;
        int i, elem;

        size = simple_strtoul(buffer+7, &ecp, 16);
        if (ecp && *ecp==',')
        {
            fill = simple_strtoul(ecp+1, &ecp, 16);
            if (*ecp == ',')
            {
                ecount = simple_strtoul(ecp+1, NULL, 16);
            }
        }
        for (i=0, elem=0 ; i<ecount ; i++)
        {
            buf = kmalloc(size, GFP_KERNEL);
            printk(" %8x", (int)buf);
            if (++elem == 8)
            {
                printk("\n");
                elem = 0;
            }
            if (fill && buf)
                memset(buf, fill, size);
        }
        if (elem)
            printk("\n");
        return count;
    }

    if (memcmp(buffer, "kfree", 5)==0) /* kfreeADDR */
    {
        int val = 0;

        val = simple_strtoul(buffer+5, NULL, 16);
        kfree((void *)val);
        return count;
    }


    if (memcmp(buffer,"bug",3)==0)    /* bug : BUG() */
    {
        BUG();
        return count;
    }

    if (memcmp(buffer,"warn_on",7)==0)    /* warn_on: WARN_ON(1) */
    {
        WARN_ON(1);
        return count;
    }

    if (memcmp(buffer,"cpuid", 5)==0)    /* cpuid : show my cpu id */
    {
        printk("%d\n", smp_processor_id());
        return count;
    }

    if (memcmp(buffer,"phy_dm",6) == 0)    /* phy_dm[.]ADDR[,LEN] */
    {
        char *cp = buffer+6;
        int big_endian = 0;
        char *ecp = NULL;
        void *map_base;
        int *virt_addr;
        phys_addr_t phy_addr;

        if (*cp == '.') {
            cp++;
            big_endian = 1;
        }
        if (sizeof(phys_addr_t) == 4)
            phy_addr = simple_strtoul(cp, &ecp, 16);
        else
            phy_addr = simple_strtoull(cp, &ecp, 16);

        phy_addr &= ~3;
        map_base = ioremap_nocache(phy_addr & ~((phys_addr_t)PAGE_SIZE-1), PAGE_SIZE);

        if (!map_base)
            return count;
        virt_addr = (int *)((unsigned int)map_base + ((unsigned int)phy_addr & (PAGE_SIZE-1)));
        if (ecp && *ecp==',')
        {
            uint32_t len = 16;
            uint32_t max_len;

            max_len = PAGE_SIZE-((unsigned int)virt_addr & (PAGE_SIZE-1));
            len = simple_strtoul(ecp+1, NULL, 16);
            if (len > max_len)
                len = max_len;
            cn_dump_memory(virt_addr, len);
        }
        else if (sizeof(phys_addr_t) == 4)
            printk("phy_dm %08x: %08x\n", (uint32_t)phy_addr, big_endian ? ioread32be(virt_addr): ioread32(virt_addr));
        else
            printk("phy_dm %08llx: %08x\n", (uint64_t)phy_addr, big_endian ? ioread32be(virt_addr) : ioread32(virt_addr));

        iounmap(map_base);
        return count;
    }

    if (memcmp(buffer,"phy_mm",6) == 0)    /* phy_mm[.]ADDR=VAL */
    {
        char *cp = buffer+6;
        int big_endian = 0;
        char *ecp = NULL;
        uint32_t val = 0;
        void *virt_addr;
        phys_addr_t phy_addr;

        if (*cp == '.') {
            cp++;
            big_endian = 1;
        }
        if (sizeof(phys_addr_t) == 4)
            phy_addr = simple_strtoul(cp, &ecp, 16);
        else
            phy_addr = simple_strtoull(cp, &ecp, 16);

        if (*ecp == '=')
        {
            val = simple_strtoul(ecp+1, NULL, 16);
            phy_addr &= ~3;
            virt_addr = ioremap_nocache(phy_addr, 4);
            if (!virt_addr)
                return count;
            if (big_endian)
                iowrite32be(val, virt_addr);
            else
                iowrite32(val, virt_addr);
            if (sizeof(phys_addr_t) == 4)
                printk("phy_mm %08x: %08x\n", (uint32_t)phy_addr, big_endian ? ioread32be(virt_addr) : ioread32(virt_addr));
            else
                printk("phy_mm %08llx: %08x\n", (uint64_t)phy_addr, big_endian ? ioread32be(virt_addr) : ioread32(virt_addr));
            iounmap(virt_addr);
        }
        return count;
    }

    if (memcmp(buffer, "accessok", 8)==0)    /* accesssokADDR */
    {
        unsigned long addr = simple_strtoul(buffer+8, NULL, 16);

        printk("access_ok(%lx): %d\n", addr, access_ok(VERIFY_READ, addr, 1));
        return count;
    }

    if(memcmp(buffer,"stack_magic",11) == 0)
    {
        unsigned long i_time = jiffies;
        force_stack_magic = 1;
        printk("Waiting 5 secs\n");
        while (jiffies-i_time < 5*HZ)
            ;
        printk("Exiting\n");
        return count;
    }
    
    if(memcmp(buffer,"stack_overflow",14) == 0)
    {
        unsigned long i_time = jiffies;
        force_stack_overflow = 1;
        printk("Waiting 5 secs\n");
        while (jiffies-i_time < 5*HZ)
            ;
        printk("Exiting\n");
        return count;
    }

    if (memcmp(buffer, "SOV",3) == 0)   /* SOV[depth] default depth 0x4000 */
    {
        /* cause a big stack push of a given size 'depth' */
        int depth = 0;

        depth = simple_strtoul(buffer+3, NULL, 16);
        if (depth==0)
            big_stack_16K();
        else if (depth==1)
            small_recursive_loop(0);
        else
            big_recursive_loop(&pos, depth);   /* stack size is 0x2000, limit set to 0x800 free */
        return count;
    }
    if(memcmp(buffer,"__memzero_crash",15) == 0)    /* __memzero */
    {
        printk("memzero crashing on cpu%d\n", smp_processor_id());
        __memzero(0, 1);
        return count;
    }

    printk("no command: %s\n", buffer);

    return count;
}


static const struct file_operations arubacmd_operations = {
    .open    = arubacmd_proc_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
    .write   = arubacmd_proc_write,
};

static int __init 
arubacmd_init(void)
{
    struct proc_dir_entry *entry = create_proc_entry("arubacmd", 0, NULL);
    if (!entry) {
        return -1;
    }

    entry->proc_fops = &arubacmd_operations;
    return 0;
}

static void __exit 
arubacmd_cleanup(void)
{
    remove_proc_entry("arubacmd", NULL);
}

static int 
aruba_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, aruba_info_show, NULL);
}

static struct file_operations aruba_info_operations = {
    .open       = aruba_info_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int __init 
aruba_info_init(void)
{
    struct proc_dir_entry *entry;

    INIT_WORK(&power_work, (void *)aruba_enforce_power_policy);

    entry = create_proc_entry("aruba_info", 0, NULL);
    if (!entry) {
        return -1;
    }

    entry->proc_fops = &aruba_info_operations;

    entry = create_proc_entry("reboot_reason", 0, NULL);
    if (!entry) {
        return -1;
    }

    entry->proc_fops = &aruba_reboot_reason_operations;

    entry = create_proc_entry("watchdog_dump", 0, NULL);
    if (!entry) {
        return -1;
    }

    entry->proc_fops = &aruba_watchdog_dump_operations;

    aruba_hw_load_power_pins();

    static_power_management = 1;

    return 0;
}

static void __exit 
aruba_info_cleanup(void)
{
    remove_proc_entry("aruba_info", NULL);
}
static void aruba_tpm_reset(void)
{
    int tpm_rst_pin = -1;
         
    tpm_rst_pin = aruba_get_gpio_pin("tpm-rst");
    if (tpm_rst_pin != -1)
    {
        aruba_gpio_out(tpm_rst_pin,0);
        mdelay(200);
        aruba_gpio_out(tpm_rst_pin,1);
    }

}
static void sierra_gpio_init(void)
{
	/*int sfp_pwr_en_pin = aruba_get_gpio_pin("sfp_pwr_en");
	int ble_rst_pin    = aruba_get_gpio_pin("ble_rst");
	int fiber_rst_pin  = aruba_get_gpio_pin("fiber_rst") ;
	int tpm_ready_pin  = aruba_get_gpio_pin("tpm_ready");
	int sfp_tx_dis_pin = aruba_get_gpio_pin("sfp_tx_dis");
	int sfp_txfault_pin= aruba_get_gpio_pin("sfp_tx_fault") ;
	int sfp_los_pin    = aruba_get_gpio_pin("sfp_los");
	int sfp_prst_pin   = aruba_get_gpio_pin("sfp_prst");
	int tpm_rst_pin    = aruba_get_gpio_pin("tpm_rst");
	int g5_pwr_en_pin  = aruba_get_gpio_pin("5g_pwr_en");
	int g2_pwr_en_pin  = aruba_get_gpio_pin("2g_pwr_en");*/

	int sfp_pwr_en_pin = 157;
	int ble_rst_pin    = 162;
	int fiber_rst_pin  = 161;
	int tpm_ready_pin  = 164;
	int sfp_tx_dis_pin = 153;
	int sfp_txfault_pin= 152 ;
	int sfp_los_pin    = 155;
	int sfp_prst_pin   = 156;
	int tpm_rst_pin    = 158;
	int g5_pwr_en_pin  = 160;
	int g2_pwr_en_pin  = 159;

	if (sfp_pwr_en_pin != -1)
	{
		if (aruba_gpio_request(sfp_pwr_en_pin,GPIOF_OUT_INIT_HIGH,"sfp_pwr_en"))
		{
			sfp_pwr_en_pin = -1;
		}
	}
	mdelay(100);
	if (tpm_rst_pin != -1)
	{
		if (aruba_gpio_request(tpm_rst_pin,GPIOF_OUT_INIT_HIGH,"tpm_reset"))
		{
			tpm_rst_pin = -1;
		}
	}
	if (sfp_tx_dis_pin != -1)
	{
		if (aruba_gpio_request(sfp_tx_dis_pin,GPIOF_OUT_INIT_LOW,"sfp_tx_dis"))
		{
			sfp_tx_dis_pin = -1;
		}
	}
	if (fiber_rst_pin != -1)
	{
		if (aruba_gpio_request(fiber_rst_pin,GPIOF_OUT_INIT_HIGH,"fiber_rst"))
		{
			fiber_rst_pin = -1;
		}
	}
	if (ble_rst_pin != -1)
	{
		if (aruba_gpio_request(ble_rst_pin,GPIOF_OUT_INIT_HIGH,"ble-reset"))
		{
			ble_rst_pin = -1;
		}
	}
	if (g5_pwr_en_pin != -1)
	{
		if (aruba_gpio_request(g5_pwr_en_pin,GPIOF_OUT_INIT_HIGH,"5g_pwr_en"))
		{
			g5_pwr_en_pin = -1;
		}
	}
	if (g2_pwr_en_pin != -1)
	{
		if (aruba_gpio_request(g2_pwr_en_pin,GPIOF_OUT_INIT_HIGH,"2g_pwr_en"))
		{
			g2_pwr_en_pin = -1;
		}
	}
	if (tpm_ready_pin != -1)
	{
		if (aruba_gpio_request(tpm_ready_pin,GPIOF_IN,"tpm_ready"))
		{
			tpm_ready_pin = -1;
		}
	}
	if (sfp_txfault_pin != -1)
	{
		if (aruba_gpio_request(sfp_txfault_pin,GPIOF_IN,"sfp_tx_fault"))
		{
			sfp_txfault_pin = -1;
		}
	}
	if (sfp_los_pin != -1)
	{
		if (aruba_gpio_request(sfp_los_pin,GPIOF_IN,"sfp_los"))
		{
			sfp_los_pin = -1;
		}
	}
	if (sfp_prst_pin != -1)
	{
		if (aruba_gpio_request(sfp_prst_pin,GPIOF_IN,"sfp_prst"))
		{
			sfp_prst_pin = -1;
		}
	}
	/*tpm reset*/
	aruba_gpio_out(tpm_rst_pin,0);
	mdelay(200);
	aruba_gpio_out(tpm_rst_pin,1);
  }

unsigned aruba_avail_cpus = NR_CPUS;
EXPORT_SYMBOL(aruba_avail_cpus);
static int __init 
aruba_hw_init(void)
{
    aruba_info_init();
    aruba_usb_init();
    arubacmd_init();
    aruba_get_reboot_reason();
    aruba_usb_power_mode_init();
    board_setup_heartbeat();
    if (xml_ap_model == AP_387)
    {
        aruba_tpm_reset();
    }
	if (xml_ap_model == AP_374 || xml_ap_model == AP_375 || xml_ap_model == AP_377 || xml_ap_model == AP_318)
	{
		sierra_gpio_init();
	}
    aruba_avail_cpus = num_online_cpus();
    
    return 0;
}

static void __exit 
aruba_hw_cleanup(void)
{
    aruba_info_cleanup();
    aruba_usb_cleanup();
    arubacmd_cleanup();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aruba Networks <sca@arubanetworks.com>");
MODULE_DESCRIPTION("ARM support for Aruba APs");
module_init(aruba_hw_init);
module_exit(aruba_hw_cleanup);
