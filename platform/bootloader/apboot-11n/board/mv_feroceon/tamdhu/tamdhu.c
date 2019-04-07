/* vim:set ts=4 sw=4 expandtab: */
#include <config.h>
#include <common.h>
#include <command.h>
#include <miiphy.h>
#include <exports.h>
#include <linux/ctype.h>
#include <aruba_manuf.h>
#include <watchdog.h>
#ifdef CONFIG_MARVELL_6172
#include "../../../drivers/m6172.h"
#endif
#include "../mv_hal/uart/mvUart.h"

extern void gpio_set_as_input(unsigned);
extern void gpio_set_as_output(unsigned);
extern void gpio_out(unsigned, unsigned);
extern unsigned gpio_value(unsigned);
#ifdef CONFIG_LTC_4266
extern void poeInit(void);
#endif
int
checkboard(void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif
    /* drive reset pin high, but don't enable it as an output */
    gpio_out(GPIO_HW_RESET, 1);

    return 0;
}

#if defined(CONFIG_RESET_BUTTON)
static void
flash_status_led(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    int i;

    for (i = 0; i < 6; i++) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
#ifdef CONFIG_HW_WATCHDOG
        gd->watchdog_blink = 1;
#endif
        mdelay(100);
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
#ifdef CONFIG_HW_WATCHDOG
        gd->watchdog_blink = 0;
#endif
        WATCHDOG_RESET();
        mdelay(100);
    }
}

static void
check_reset_button(void)
{
    MV_UART_PORT *p0, *p1;
    char *reset_env_var = 0;

    p0 = mvUartBase(0);
    p1 = mvUartBase(1);

    if (!gpio_value(GPIO_CONFIG_CLEAR)) {
        /* unleash hell */
#if 0 /* XXX; this doesn't work pre relocation */
        reset_env_var = getenv("disable_reset_button");
#endif
        if (reset_env_var && (!strcmp(reset_env_var, "yes")))  {
            p0->scr = 'D';
            p1->scr = 'D';
        } else {
            p0->scr = 'R';
            p1->scr = 'B';
            flash_status_led();
        }
    } else {
        /* no config clear */
        p0->scr = 'N';
        p1->scr = 'R';
    }
}
#endif

int
early_board_init(void)
{
    /* turn off red LED */
    gpio_set_as_output(GPIO_RED_STATUS_LED);
    gpio_out(GPIO_RED_STATUS_LED, 1);

    gpio_set_as_output(GPIO_GREEN_STATUS_LED);
#ifdef GPIO_FLASH_WRITE
    gpio_set_as_output(GPIO_FLASH_WRITE);
#endif
    gpio_set_as_output(GPIO_2G_RED_LED);
    gpio_set_as_output(GPIO_5G_RED_LED);
    gpio_set_as_output(GPIO_2G_GREEN_LED);
    gpio_set_as_output(GPIO_5G_GREEN_LED);
    gpio_set_as_output(GPIO_2G_RADIO_RESET);
    gpio_set_as_output(GPIO_5G_RADIO_RESET);

    gpio_set_as_output(GPIO_PSE1_RED_LED);
    gpio_set_as_output(GPIO_PSE2_RED_LED);
    gpio_set_as_output(GPIO_PSE1_GREEN_LED);
    gpio_set_as_output(GPIO_PSE2_GREEN_LED);

    gpio_set_as_input(GPIO_CONFIG_CLEAR);

    /* turn red/green radio signals off as early as possible */
    gpio_out(GPIO_5G_RED_LED, 1);
    gpio_out(GPIO_2G_RED_LED, 1);
    gpio_out(GPIO_5G_GREEN_LED, 1);
    gpio_out(GPIO_2G_GREEN_LED, 1);

    gpio_out(GPIO_PSE1_RED_LED, 1);
    gpio_out(GPIO_PSE2_RED_LED, 1);
    gpio_out(GPIO_PSE1_GREEN_LED, 1);
    gpio_out(GPIO_PSE2_GREEN_LED, 1);

    /* disable USB power supply */
    gpio_out(GPIO_USB_POWER, 0);  

    /* put radios and PHYs in reset */
    gpio_out(GPIO_2G_RADIO_RESET, 0);
    gpio_out(GPIO_5G_RADIO_RESET, 0);

    mdelay(100);

    /* take switch out of reset */
#ifdef GPIO_SWITCH_RESET
    gpio_set_as_output(GPIO_SWITCH_RESET);
    gpio_out(GPIO_SWITCH_RESET, 1);
#endif

#if 0
    /* drive PA at 4.2v */
    gpio_set_as_output(GPIO_PA_VOLTAGE);
    gpio_out(GPIO_PA_VOLTAGE, 0);
#endif

    /* take radios out of reset */
    gpio_out(GPIO_2G_RADIO_RESET, 1);
    gpio_out(GPIO_5G_RADIO_RESET, 1);

    check_reset_button();
    return 0;
}

int n_radios = 0;

static void
tamdhu_radio_callback(int n)
{
   n_radios = n;
}

#if defined(CONFIG_RESET_BUTTON)
static void
execute_config_clear(void)
{
    extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
    char *av[3];
    MV_UART_PORT *p0, *p1;
    char *reset_env_var = 0;

    p0 = mvUartBase(0);
    p1 = mvUartBase(1);

    WATCHDOG_RESET();

    if (p0->scr == 'N' && p1->scr == 'R') {
        return;
    } else if (p0->scr == 'D' && p1->scr == 'D') {
        printf("**** Reset Request Disabled by Environment ****\n");
    } else if (p0->scr != 'R' || p1->scr != 'B') {
        printf("**** Unrecognized Reset State (0x%02x/0x%02x) -- Ignoring ****\n", 
            p0->scr, p1->scr);
        return;
    }

    reset_env_var = getenv("disable_reset_button");
    if (reset_env_var && !strcmp(reset_env_var, "yes")) {
        printf("**** Reset Request Disabled by Environment ****\n");
        return;
    }

    printf("**** Configuration Reset Requested by User ****\n");

    av[0] = "factory_reset";
    av[1] = 0;
    do_factory_reset(0, 0, 1, av);
    flash_status_led();
}
#endif

void
late_board_init(void)
{
    extern void ar5416_initialize(void);
    extern void ar5416_install_callback(void *);
    extern void aruba_6560_ge_phy_init(void);
#ifdef CONFIG_MARVELL_6172
    unsigned char mac[6];
    char *p;
#endif

    WATCHDOG_RESET();

    ar5416_install_callback(tamdhu_radio_callback);
    ar5416_initialize();

    aruba_6560_ge_phy_init();

    /*
     * turn off red/green LED signals to be safe; we already did this, so it's 
     * probably not necessary, but it's also harmless
     */
    gpio_out(GPIO_5G_RED_LED, 1);
    gpio_out(GPIO_2G_RED_LED, 1);
    gpio_out(GPIO_5G_GREEN_LED, 1);
    gpio_out(GPIO_2G_GREEN_LED, 1);

#ifdef CONFIG_MARVELL_6172
    p = getenv("ethaddr");
    if (p) {
        mvMacStrToHex(p, (MV_U8 *)mac);
        mac[5] += 1;  /* this is not correct, but it won't be used in APBoot */
    }
    m6172_announce(mac);
#endif
#ifdef CONFIG_LTC_4266
    poeInit();
#endif
    execute_config_clear();
}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("Config reset:              %s\n", gpio_value(GPIO_CONFIG_CLEAR) ? "inactive" : "pending");
    printf("Config usb power interrupt %s\n", gpio_value(GPIO_USB_POWER_INT)? "inactive" : "pending");
    printf("Config poe interrupt       %s\n", gpio_value(GPIO_POE_INT)? "inactive" : "pending");
    return 0;
}

U_BOOT_CMD(
    gpio_pins,    1,    1,     do_gpio_pins,
    "gpio_pins    - show GPIO pin status\n",                   
    " Usage: gpio_pins \n"
);

