#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include <watchdog.h>
#include "ar7240_soc.h"

#if defined(CONFIG_RESET_BUTTON)
#define REG_OFFSET             4
#define OFS_SCRATCH_PAD        (7*REG_OFFSET)
#define UART16550_READ(y)      ar7240_reg_rd((AR7240_UART_BASE+y))
#define UART16550_WRITE(x, z)  ar7240_reg_wr((AR7240_UART_BASE+x), z)
#endif

extern void ar7240_ddr_initial_config(uint32_t refresh);
extern int ar7240_ddr_find_size(void);

void
ar7240_usb_initial_config(void)
{
    ar7240_reg_wr_nf(AR7240_USB_PLL_CONFIG, 0x0a04081e);
    ar7240_reg_wr_nf(AR7240_USB_PLL_CONFIG, 0x0804081e);
}

void ar7240_gpio_config(void)
{
    /* Disable clock obs */
    ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) & 0xffe7e0ff));
#if 0
    /* Enable eth Switch LEDs */
    ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) | 0xf8));
#endif
}

int
ar7240_mem_config(void)
{
    unsigned int tap_val1, tap_val2;
    ar7240_ddr_initial_config(CFG_DDR_REFRESH_VAL);

    /* Default tap values for starting the tap_init*/
    if (!(is_ar7241() || is_ar7242()))  {
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL0, 0x8);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL1, 0x9);
        ar7240_ddr_tap_init();
    }
    else {
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL0, 0x2);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL1, 0x2);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL2, 0x0);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL3, 0x0);
    }

    tap_val1 = ar7240_reg_rd(0xb800001c);
    tap_val2 = ar7240_reg_rd(0xb8000020);

#if 0
    printf("#### TAP VALUE 1 = 0x%x, 2 = 0x%x [0x%x: 0x%x]\n",
                tap_val1, tap_val2, *(unsigned *)0x80500000,
                *(unsigned *)0x80500004);
#endif
    ar7240_usb_initial_config();
    ar7240_gpio_config();

    return (ar7240_ddr_find_size());
}

long int initdram(int board_type)
{
    return (ar7240_mem_config());
}

int
checkboard (void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif

#ifdef CONFIG_SCAPA_H
    /* set GPIO output-enable and function registers to something sane */
    ar7240_reg_wr(AR7240_GPIO_OE, 0x0);  /* set all gpio pins to inputs */
    ar7240_reg_wr(AR7240_GPIO_FUNC, 0x48002);  /* spi_en + uart_en */
#endif

    ar7240_gpio_config_output(GPIO_RED_STATUS_LED);
    ar7240_gpio_config_output(GPIO_GREEN_STATUS_LED);
    ar7240_gpio_config_output(GPIO_FLASH_WRITE);
    ar7240_gpio_config_output(GPIO_2G_RED_LED);
    ar7240_gpio_config_output(GPIO_5G_RED_LED);
    ar7240_gpio_config_output(GPIO_5G_GREEN_LED);

    // drive reset pin high, but don't enable it as an output
    ar7240_gpio_out_val(GPIO_HW_RESET, 1);

    // turn red/green radio signals off as early as possible
    ar7240_gpio_out_val(GPIO_5G_RED_LED, 1);
    ar7240_gpio_out_val(GPIO_2G_RED_LED, 1);
    ar7240_gpio_out_val(GPIO_5G_GREEN_LED, 1);

    return 0;
}

#if defined(CONFIG_RESET_BUTTON)
static void
flash_status_led(void)
{
    int i;

    ar7240_gpio_config_output(GPIO_RED_STATUS_LED);
    ar7240_gpio_config_output(GPIO_GREEN_STATUS_LED);
    for (i = 0; i < 10; i++) {
        ar7240_gpio_out_val(GPIO_GREEN_STATUS_LED, 1);
        mdelay(100);
        ar7240_gpio_out_val(GPIO_GREEN_STATUS_LED, 0);
        mdelay(100);
    }
}

static void
check_reset_button(void)
{
    char *reset_env_var;
    UART16550_WRITE(OFS_SCRATCH_PAD, 0xa5);  /* default value */
    if (!ar7240_get_bit(AR7240_GPIO_IN, GPIO_CONFIG_CLEAR)) {
        // unleash hell
        UART16550_WRITE(OFS_SCRATCH_PAD, 'R');  /* reset pressed */
        reset_env_var = getenv("disable_reset_button");
        if (reset_env_var && (!strcmp(reset_env_var, "yes")))  {
            UART16550_WRITE(OFS_SCRATCH_PAD, 'D');  /* disabled by env */
        } else {
            flash_status_led();
        }
    } else {
        // no config clear
        UART16550_WRITE(OFS_SCRATCH_PAD, 'N');  /* no reset */
    }
}
#endif

int
early_board_init(void)
{
    // turn off red LED
    ar7240_gpio_out_val(GPIO_RED_STATUS_LED, 1);
#ifdef CONFIG_RESET_BUTTON
    if (GPIO_CONFIG_CLEAR == 1)  {
        ar7240_reg_rmw_clear (AR7240_GPIO_FUNC, 1 << 14);
        ar7240_reg_rmw_clear (AR7240_GPIO_FUNC, 1 << 19);
    }
    ar7240_reg_rmw_clear (AR7240_GPIO_OE, 1 << GPIO_CONFIG_CLEAR);
    check_reset_button();
#endif
    return 0;
}

int n_radios = 0;

static void
scapa_radio_callback(int n)
{
   n_radios = n;
}

#if defined(CONFIG_RESET_BUTTON)
static void
execute_config_clear(void)
{
    extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
    char *av[3];
    unsigned char reset_state = UART16550_READ(OFS_SCRATCH_PAD) & 0xff;

    if (reset_state == 'N')  {  /* "No" config reset requested */
        return;
    }  else if  (reset_state == 'D')  {  /* "Disabled" config request */
        printf("**** Reset Request Disabled by Environment ****\n");
        return;
    }  else if  (reset_state != 'R')  {  /* unrecognized "Reset" indicator */
        printf("**** Unrecognized Reset State (%02x) -- Ignoring ****\n", reset_state);
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

    ar5416_install_callback(scapa_radio_callback);
    ar5416_initialize();

    // initialize radio LED signals
    if (n_radios >= 2) {
    	printf("got %u radios; not expected\n", n_radios);
    } else if (n_radios == 1) {
        // each GPIO pin has a 2-bit configuration, so shift 2 bits for pin 1
        ar7240_reg_wr(0x1000404c, /*__cpu_to_le32*/(1<<2));

        // turn the LEDs off for now (register 0x4048 on the 928x)
        ar7240_reg_wr(0x10004048, /*__cpu_to_le32*/(0x2));
        ar7240_reg_wr(0x10004060, /*__cpu_to_le32*/(0));
    } else {
        printf("No radios found\n");
    }

    //
    // turn off red LED signals to be safe; we already did this, so it's probably not
    // necessary, but it's also harmless
    //
    ar7240_gpio_out_val(GPIO_5G_RED_LED, 1);
    ar7240_gpio_out_val(GPIO_2G_RED_LED, 1);
    ar7240_gpio_out_val(GPIO_5G_GREEN_LED, 1);

#if defined(CONFIG_RESET_BUTTON)
    execute_config_clear();
#endif

    return;
}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("Reset button: %s\n", !ar7240_get_bit(AR7240_GPIO_IN, GPIO_CONFIG_CLEAR) ? "pressed" :
        "not pressed");

    return 0;
}

U_BOOT_CMD(
       gpio_pins,    1,    1,     do_gpio_pins,
       "gpio_pins    - show GPIO pin status\n",
       " Usage: gpio_pins \n"
);

