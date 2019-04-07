/* vim:set ts=4 sw=4 expandtab: */
/* this assumes that only one core is running in U-boot */
#include <common.h>
#include <config.h>

#ifdef CONFIG_APBOOT
#pragma GCC optimize("-O0")
#endif

#if defined(CONFIG_HW_WATCHDOG) && defined(CONFIG_IPROC)

#include <asm/arch/socregs.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch/iproc.h>

#include <command.h>

static int pet_the_dog = 1;

extern void gpio_out(unsigned, unsigned);

#define WDT_PERIOD_DEFAULT 25

#if 0
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
#endif


static inline unsigned
get_periph_clock(void)
{
    unsigned periph_clk;
    /* 
     * We read this every time to avoid issues with relocation;  we could 
     * probably stash a variable away after relocation, but I don't think 
     * that the overhead is too high.
     */
#ifdef CONFIG_NORTHSTAR
	{
		unsigned int sku_id;
		sku_id = ((*((unsigned int *)0x1810D500)) >> 2) & 0x03;
		if(sku_id == 1 || sku_id == 2) {
			periph_clk = 400000000;	/* 400 MHz */
		}
		else {
			periph_clk = 500000000; /* 500 MHz */
		}
	}
#else
#error "Unsupported"
#endif

    return periph_clk;
}

/* this can be called from anywhere */
void
hw_watchdog_reset_internal(void)
{
    unsigned periph_clk;

    periph_clk = get_periph_clock();

    if (pet_the_dog) {
        reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_LOAD, periph_clk);
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
        gd->watchdog_start = get_timer(0);
        gd->watchdog_blink = 1;
        gd->watchdog_inited = 1;
//        ticks_per_second = get_tbclk();
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
    if (get_timer(gd->watchdog_start) > CFG_HZ) {
        gd->watchdog_start = get_timer(0);
        gd->watchdog_blink = !gd->watchdog_blink;
    }
}

void
hw_watchdog_init_internal(void)
{
    unsigned periph_clk;

    periph_clk = get_periph_clock();

    /* clear status before enabling watchdog */
    reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_COUNTER, periph_clk);
    reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_LOAD, periph_clk);
    reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_CTRL, 
        (WDT_PERIOD_DEFAULT << 8) | 0x9);
}

void
hw_watchdog_init(void)
{
    hw_watchdog_init_internal();
}

void
hw_watchdog_off(void)
{
    reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_DISABLE, 0x12345678);
    reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_DISABLE, 0x87654321);
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
    hw_watchdog_init_internal();
    printf("Watchdog enabled\n");
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
    if (reg32_read((volatile uint32_t *)IHOST_PTIM_WATCHDOG_STATUS) & 0x1) {
        printf("Reboot caused by watchdog timeout\n");
    }

    /* clear boot status */
    reg32_write((volatile uint32_t *)IHOST_PTIM_WATCHDOG_STATUS, 1);
}
#endif
