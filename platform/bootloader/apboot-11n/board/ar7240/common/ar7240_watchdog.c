#include <config.h>

#ifdef CONFIG_HW_WATCHDOG

#include <common.h>
#include <command.h>
#include <ar7240_soc.h>

static int pet_the_dog = 1;
#ifdef CONFIG_CARDHU
#define WDOG_VAL    0x20000000  // this gives ~20 seconds at a ref clk of 25Mhz
#elif defined(CONFIG_DALMORE) || defined(CONFIG_HAZELBURN)
#define WDOG_VAL    0x33333333  // this gives ~20 seconds at a ref clk of 40Mhz
#else
#define WDOG_VAL    0xffffffff  // this gives ~25 seconds at a bus speed of 170Mhz
#endif

void
hw_watchdog_reset(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (!gd->watchdog_inited) {
		gd->watchdog_start = get_timer(0);
		gd->watchdog_blink = 1;
		gd->watchdog_inited = 1;
	}
	if (pet_the_dog) {
		ar7240_reg_wr(AR7240_WATCHDOG_TMR, WDOG_VAL);
	}
#ifdef GPIO_GREEN_STATUS_LED
	if (gd->watchdog_blink) {
		ar7240_gpio_out_val(GPIO_GREEN_STATUS_LED, 1);
#ifdef GPIO_AMBER_STATUS_LED
		ar7240_gpio_out_val(GPIO_AMBER_STATUS_LED, 1);
#endif
	} else {
		ar7240_gpio_out_val(GPIO_GREEN_STATUS_LED, 0);
#ifdef GPIO_AMBER_STATUS_LED
		ar7240_gpio_out_val(GPIO_AMBER_STATUS_LED, 0);
#endif
	}
#endif
	if (get_timer(gd->watchdog_start) > CFG_HZ) {
		gd->watchdog_start = get_timer(0);
		gd->watchdog_blink = !gd->watchdog_blink;
	}
}

void
hw_watchdog_init(void)
{
	ar7240_reg_wr(AR7240_WATCHDOG_TMR, WDOG_VAL);
	ar7240_reg_wr(AR7240_WATCHDOG_TMR_CONTROL, 3);
}

void
hw_watchdog_off(void)
{
	ar7240_reg_wr(AR7240_WATCHDOG_TMR_CONTROL, 0);
}

int
do_dog(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf("Watchdog will fire momentarily.\n");
	pet_the_dog = 0;
	return 0;
}

U_BOOT_CMD(
	wdog,     1,     1,      do_dog,
	"wdog - stop petting the watchdog\n",
	"- stop petting the watchdog\n"
);

void
ar7240_watchdog_check(void)
{
#if 0
// doesn't seem to work on 7242
	if (ar7240_reg_rd(AR7240_WATCHDOG_TMR_CONTROL) & 0x80000000) {
		printf("\n*** Reboot caused by watchdog timeout. ***\n");
	}
#endif
}
#endif
