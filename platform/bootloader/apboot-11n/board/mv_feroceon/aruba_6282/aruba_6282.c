// vim:set ts=4 sw=4 expandtab:
#include <config.h>
#include <common.h>
#include <command.h>
#include <miiphy.h>
#include <exports.h>
#include <linux/ctype.h>
#include <aruba_manuf.h>
#include <watchdog.h>

int
checkboard(void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif
    return 0;
}

int
early_board_init(void)
{
    return 0;
}

int n_radios = 0;

static void
aruba_6282_radio_callback(int n)
{
   n_radios = n;
}

void
late_board_init(void)
{
    extern void ar5416_initialize(void);
    extern void ar5416_install_callback(void *);

    ar5416_install_callback(aruba_6282_radio_callback);
    ar5416_initialize();
    return;
}

