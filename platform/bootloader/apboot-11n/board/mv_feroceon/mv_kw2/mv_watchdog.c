// vim:set ts=4 sw=4 expandtab:
#include <config.h>

#ifdef CONFIG_HW_WATCHDOG

#include "../mv_hal/cntmr/mvCntmr.h"
#include "../mv_hal/cntmr/mvCntmrRegs.h"
#include "../mv_hal/gpp/mvGpp.h"
#include "../mv_kw2/kw2_family/cpu/mvCpu.h"
#include "../mv_kw2/kw2_family/ctrlEnv/sys/mvCpuIfRegs.h"

#include <common.h>
#include <command.h>

static int pet_the_dog = 1;
extern void gpio_out(unsigned, unsigned);

// this gives ~21 seconds with a 200 MHz TClk, or ~25 with a 166 MHz TClk
#define WDOG_VAL    0xffffffff

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
        MV_REG_WRITE(WDOG_VAL_REG, WDOG_VAL);
    }
#ifdef GPIO_GREEN_STATUS_LED
    if (gd->watchdog_blink) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
    } else {
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
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
    // set timer value
    MV_REG_WRITE(WDOG_VAL_REG, WDOG_VAL);

    // enable watchdog (bit 4)
    MV_REG_BIT_SET(CNTMR_CTRL_REG, BIT4);
    // disable watchdog reload (bit 5)
    MV_REG_BIT_RESET(CNTMR_CTRL_REG, BIT5);

    // enable reset on watchdog
    MV_REG_BIT_SET(CPU_RSTOUTN_MASK_REG, BIT1);
}

void
hw_watchdog_off(void)
{
    MV_REG_BIT_RESET(CNTMR_CTRL_REG, BIT4); // disable watchdog (bit 4)
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
mv_watchdog_check(void)
{
}
#endif
