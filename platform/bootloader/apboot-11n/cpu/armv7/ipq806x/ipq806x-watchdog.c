/* vim:set ts=4 sw=4 expandtab: */
/* this assumes that only one core is running in U-boot */
#include <common.h>
#include <config.h>

#if defined(CONFIG_HW_WATCHDOG) && defined(CONFIG_MACH_IPQ806x)

#include <command.h>
#include <asm/io.h>
#include <asm/arch/iomap.h>

static int pet_the_dog = 1;
static uint64_t ticks_per_second;

#ifdef CONFIG_EIGER
extern void eiger_ext_watchdog_enable(void);
extern void eiger_ext_watchdog_pet(void);
extern void eiger_ext_watchdog_disable(void);
extern int eiger_led_available;
#endif

extern void gpio_out(unsigned, unsigned);

#define WDT_PERIOD_DEFAULT (10 * 0x8000) /* 10 seconds */
#define WDT_BITE_DEFAULT (11 * 0x8000) /* 13 seconds */

static uint64_t
get_timer_from_tb(uint64_t base)
{
    uint64_t tb;

    tb = get_ticks();

    if (base == 0) {
        return tb;
    } else if (tb < base) {
        /* wrapped; this is close enough for our purposes */
        return base - tb;
    } else {
        return tb - base;
    }
}

/* this can be called from anywhere */
void
hw_watchdog_reset_internal(void)
{
    if (pet_the_dog) {
        writel(1, APCS_WDT0_RST);
    }
}

/* 
 * this can be called in places where 'gd' may not be valid; if so,
 * we skip LED and external watchdog poking
 */
void
hw_watchdog_reset(void)
{
    DECLARE_GLOBAL_DATA_PTR;

    if (!gd->reloc_off) {
        /* don't touch 'gd' when running from flash */
        hw_watchdog_reset_internal();
        return;
    }

    if (!gd->watchdog_inited) {
        gd->watchdog_start = get_timer_from_tb(0);
        gd->watchdog_blink = 1;
        gd->watchdog_inited = 1;
        ticks_per_second = get_tbclk();
    }

    hw_watchdog_reset_internal();

#ifdef CONFIG_EIGER
    if (eiger_led_available == 0) {
        return;
    }
    if (gd->watchdog_blink) {
        gpio_out(GPIO_OPERATIONAL_LED, 1);
        if (pet_the_dog) {
            eiger_ext_watchdog_pet();
        }
    } else {
        gpio_out(GPIO_OPERATIONAL_LED, 0);
        if (pet_the_dog) {
            eiger_ext_watchdog_pet();
        }
    }
#else
#ifdef GPIO_GREEN_STATUS_LED
    if (gd->watchdog_blink) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
#ifdef GPIO_EXT_WDOG_POKE
        if (pet_the_dog) {
            gpio_out(GPIO_EXT_WDOG_POKE, 1);
        }
#endif
    } else {
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
#ifdef GPIO_EXT_WDOG_POKE
        if (pet_the_dog) {
            gpio_out(GPIO_EXT_WDOG_POKE, 0);
        }
#endif
    }
#endif
#endif
    if (get_timer_from_tb(gd->watchdog_start) > ticks_per_second) {
        gd->watchdog_start = get_timer_from_tb(0);
        gd->watchdog_blink = !gd->watchdog_blink;
    }
}

void
hw_watchdog_init(void)
{
 	writel(0, APCS_WDT0_EN);
	writel(1, APCS_WDT0_RST);
	writel(WDT_PERIOD_DEFAULT, APCS_WDT0_BARK_TIME);
	writel(WDT_BITE_DEFAULT, APCS_WDT0_BITE_TIME);
	writel(1, APCS_WDT0_EN);
#ifdef CONFIG_EIGER
    eiger_ext_watchdog_enable();
#endif
#ifdef GPIO_WD_DISABLE
    /* enable external watch dog */
    gpio_out(GPIO_WD_DISABLE, 0);
#endif
}

void
hw_watchdog_off(void)
{
#ifdef CONFIG_EIGER
    eiger_ext_watchdog_disable();
#endif
 #ifdef GPIO_WD_DISABLE
    /* enable external watch dog */
    gpio_out(GPIO_WD_DISABLE, 1);
#endif
	writel(0, APCS_WDT0_EN);
}

int
do_dog(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("Watchdog will fire momentarily.\n");

    pet_the_dog = 0;
    return 0;
}

int
do_nodog(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    hw_watchdog_off();
    printf("Watchdog disabled\n");
    return 0;
}

int
do_endog(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("Watchdog enabled\n");
    hw_watchdog_init();
    return 0;
}

U_BOOT_CMD(
    wdog,     1,     1,      do_dog,
    "wdog - stop petting the watchdog\n",
    "- stop petting the watchdog\n"
);

U_BOOT_CMD(
    nodog,     1,     1,      do_nodog,
    "nodog - disable watchdog\n",
    "- disable watchdog\n"
);

U_BOOT_CMD(
    endog,     1,     1,      do_endog,
    "endog - enable watchdog\n",
    "- enable watchdog\n"
);

void
iproc_watchdog_check(void)
{
    //    printf("Reboot caused by watchdog timeout\n");

    /* clear boot status */
}
#endif
