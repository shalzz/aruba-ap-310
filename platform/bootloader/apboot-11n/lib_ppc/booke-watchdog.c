/* vim:set ts=4 sw=4 expandtab: */
/* this assumes that only one core is running in U-boot */
#include <config.h>

#if defined(CONFIG_HW_WATCHDOG) && defined(CONFIG_BOOKE)

#include <asm/processor.h>

#include <common.h>
#include <command.h>

#define WDTP(x)                ((((x)&0x3)<<30)|(((x)&0x3c)<<15))
#define WDTP_MASK        (WDTP(0x3f))

static int pet_the_dog = 1;
static uint64_t ticks_per_second;

extern void gpio_out(unsigned, unsigned);

#define WDT_PERIOD_DEFAULT 35        /* at 50MHz time base, this is 20 seconds */

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
        mtspr(SPRN_TSR, TSR_ENW|TSR_WIS);
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

#ifdef GPIO_GREEN_STATUS_LED
    if (gd->watchdog_blink) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
#ifdef GPIO_EXT_WDOG_POKE
        gpio_out(GPIO_EXT_WDOG_POKE, 1);
#endif
    } else {
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
#ifdef GPIO_EXT_WDOG_POKE
        gpio_out(GPIO_EXT_WDOG_POKE, 0);
#endif
    }
#endif
    if (get_timer_from_tb(gd->watchdog_start) > ticks_per_second) {
        gd->watchdog_start = get_timer_from_tb(0);
        gd->watchdog_blink = !gd->watchdog_blink;
    }
}

void
hw_watchdog_init(void)
{
    u32 val;

    /* clear status before enabling watchdog */
    mtspr(SPRN_TSR, TSR_ENW|TSR_WIS);
    val = mfspr(SPRN_TCR);
    val &= ~WDTP_MASK;
    /* first time out takes an exception, on second time out, 
       reset the whole chip */
    val |= (TCR_WIE|TCR_WRC(WRC_CHIP)|WDTP(WDT_PERIOD_DEFAULT));

    mtspr(SPRN_TCR, val);
}

void
hw_watchdog_off(void)
{
    u32 val;

    val = mfspr(SPRN_TCR);
    val &= ~(TCR_WIE | TCR_WRC_MASK | WDTP_MASK);
    mtspr(SPRN_TCR, val);
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

// no support for this that I can find yet
void
booke_watchdog_check(void)
{
}
#endif
