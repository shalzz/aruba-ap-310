#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include <atheros.h>
#include <watchdog.h>

//#undef CONFIG_JTAG_DISABLE

/* scratch-pad address used for Ardbeg */
/* (aka, last word of 32k On-Chip SRAM) */
#define ARDBEG_SCRATCH_PAD 0xbd007ffc

extern int ath_ddr_initial_config(uint32_t refresh);
extern int ath_ddr_find_size(void);

#ifdef COMPRESSED_UBOOT
#	define prmsg(...)
#	define args		char *s
#	define board_str(a)	do {			\
	char ver[] = "0";				\
	strcpy(s, a " - Scorpion 1.");			\
	ver[0] += ath_reg_rd(RST_REVISION_ID_ADDRESS)	\
						& 0xf;	\
	strcat(s, ver);					\
} while (0)
#else
#	define prmsg	printf
#	define args		void
#	define board_str(a)				\
	printf(a " - Scorpion 1.%d", ath_reg_rd		\
			(RST_REVISION_ID_ADDRESS) & 0xf)
#endif

void
ath_usb1_initial_config(void)
{
#define unset(a)	(~(a))

	ath_reg_wr_nf(SWITCH_CLOCK_SPARE_ADDRESS,
		ath_reg_rd(SWITCH_CLOCK_SPARE_ADDRESS) |
		SWITCH_CLOCK_SPARE_USB_REFCLK_FREQ_SEL_SET(5));
	udelay(1000);

	ath_reg_rmw_set(RST_RESET_ADDRESS,
				RST_RESET_USB_PHY_SUSPEND_OVERRIDE_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_PHY_RESET_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_PHY_ARESET_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_HOST_RESET_SET(1));
	udelay(1000);

	ath_reg_rmw_clear(RST_RESET_ADDRESS, RST_RESET_USB_PHY_PLL_PWD_EXT_SET(1));
	udelay(10);

	ath_reg_rmw_set(RST_RESET2_ADDRESS, RST_RESET2_USB1_EXT_PWR_SEQ_SET(1));
	udelay(10);
}

void
ath_usb2_initial_config(void)
{
	if (is_drqfn()) {
		return;
	}

	ath_reg_rmw_set(RST_RESET2_ADDRESS, RST_RESET2_USB2_MODE_SET(1));
	udelay(10);
	ath_reg_rmw_set(RST_RESET2_ADDRESS,
				RST_RESET2_USB_PHY2_SUSPEND_OVERRIDE_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET2_ADDRESS, RST_RESET2_USB_PHY2_RESET_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET2_ADDRESS, RST_RESET2_USB_PHY2_ARESET_SET(1));
	udelay(1000);
	ath_reg_rmw_clear(RST_RESET2_ADDRESS, RST_RESET2_USB_HOST2_RESET_SET(1));
	udelay(1000);

	ath_reg_rmw_clear(RST_RESET2_ADDRESS, RST_RESET2_USB_PHY2_PLL_PWD_EXT_SET(1));
	udelay(10);

	ath_reg_rmw_set(RST_RESET2_ADDRESS, RST_RESET2_USB2_EXT_PWR_SEQ_SET(1));
	udelay(10);
}

void ath_gpio_config(void)
{
#ifdef CONFIG_JTAG_DISABLE
	ath_reg_wr(GPIO_FUNCTION_ADDRESS, 0x00000002);        /* JTAG & CLK obs (disable jtag) */
//	ath_reg_wr(GPIO_OE_ADDRESS, 0x00021b03);          /* set pin 3 as an output for LED_5GHZ_RED */
#else
	ath_reg_wr(GPIO_FUNCTION_ADDRESS, 0x00000000);        /* JTAG & CLK obs */
#endif
#if 0
	/* disable the CLK_OBS on GPIO_4 and set GPIO4 as input */
	ath_reg_rmw_clear(GPIO_OE_ADDRESS, (1 << 4));
	ath_reg_rmw_clear(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_MASK);
	ath_reg_rmw_set(GPIO_OUT_FUNCTION1_ADDRESS, GPIO_OUT_FUNCTION1_ENABLE_GPIO_4_SET(0x80));
	ath_reg_rmw_set(GPIO_OE_ADDRESS, (1 << 4));
#endif
	/* pin 4 is a GPIO; disable clock observation */
	ath_reg_rmw_clear(GPIO_OUT_FUNCTION1_ADDRESS, 0xff);

	/* drive CS1 correctly; fix from Jingong */
	ath_gpio_config_output(GPIO_SPI1_CS1_L);
	ath_gpio_out_val(GPIO_SPI1_CS1_L, 1);
}

int
ath_mem_config(void)
{
	unsigned int type, reg32, *tap;
	extern uint32_t *ath_ddr_tap_cal(void);

#if !defined(CONFIG_ATH_EMULATION)

#if !defined(CONFIG_ATH_NAND_BR)
	type = ath_ddr_initial_config(CFG_DDR_REFRESH_VAL);

	tap = ath_ddr_tap_cal();
//	prmsg("tap = 0x%p\n", tap);

	tap = (uint32_t *)0xbd007f10;
//	prmsg("Tap (low, high) = (0x%x, 0x%x)\n", tap[0], tap[1]);

	tap = (uint32_t *)TAP_CONTROL_0_ADDRESS;
//	prmsg("Tap values = (0x%x, 0x%x, 0x%x, 0x%x)\n",
//		tap[0], tap[2], tap[2], tap[3]);

	/* Take WMAC out of reset */
	reg32 = ath_reg_rd(RST_RESET_ADDRESS);
	reg32 = reg32 & ~RST_RESET_RTC_RESET_SET(1);
	ath_reg_wr_nf(RST_RESET_ADDRESS, reg32);
#endif

	ath_usb1_initial_config();
	ath_usb2_initial_config();

	ath_gpio_config();
#endif /* !defined(CONFIG_ATH_EMULATION) */

	return ath_ddr_find_size();
}

long int initdram(int board_type)
{
	return (ath_mem_config());
}

int	checkboard(args)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif


    // specifically map LEDs
    ath_gpio_config_output(GPIO_GREEN_STATUS_LED);
#ifdef CONFIG_JTAG_DISABLE
    ath_gpio_config_output(GPIO_RED_STATUS_LED);
    ath_gpio_config_output(GPIO_2G_AMBER_LED);
    ath_gpio_config_output(GPIO_5G_GREEN_LED);
#endif
    ath_gpio_config_output(GPIO_2G_GREEN_LED);
    ath_gpio_config_output(GPIO_5G_AMBER_LED);
#ifdef GPIO_CONFIG_CLEAR
    ath_gpio_config_input(GPIO_CONFIG_CLEAR);
#endif
#ifdef GPIO_POWER_POE_PLUS
    ath_gpio_config_input(GPIO_POWER_POE_PLUS);
#endif
#ifdef GPIO_POWER_DC
    ath_gpio_config_input(GPIO_POWER_DC);
#endif
#ifdef GPIO_POWER_AT
    ath_gpio_config_input(GPIO_POWER_AT);
#endif
#ifdef GPIO_PHY_INT
    ath_gpio_config_input(GPIO_PHY_INT);
#endif

    // drive reset pin high, but don't enable it as an output
#ifdef GPIO_HW_RESET
    ath_gpio_out_val(GPIO_HW_RESET, 1);
#endif

    // turn red/green radio signals off as early as possible
    ath_gpio_out_val(GPIO_2G_GREEN_LED, 1);
#ifdef CONFIG_JTAG_DISABLE
    ath_gpio_out_val(GPIO_2G_AMBER_LED, 1);
    ath_gpio_out_val(GPIO_5G_AMBER_LED, 1);
    ath_gpio_out_val(GPIO_5G_GREEN_LED, 1);

    ath_gpio_out_val(GPIO_RED_STATUS_LED, 1);
#endif
#ifdef GPIO_PHY_RESET
    ath_gpio_config_output(GPIO_PHY_RESET);
    ath_gpio_out_val(GPIO_PHY_RESET, 1);
#endif

#ifdef GPIO_USB_CONTROL
    ath_gpio_config_output(GPIO_USB_CONTROL);
    ath_gpio_out_val(GPIO_USB_CONTROL, 0);
#endif
    return 0;
}

#if defined(CONFIG_RESET_BUTTON)
static void
flash_status_led(void)
{
    int i;

#ifdef CONFIG_JTAG_DISABLE
    ath_gpio_config_output(GPIO_RED_STATUS_LED);
#endif
    ath_gpio_config_output(GPIO_GREEN_STATUS_LED);
    for (i = 0; i < 10; i++) {
        ath_gpio_out_val(GPIO_GREEN_STATUS_LED, 1);
        mdelay(100);
        ath_gpio_out_val(GPIO_GREEN_STATUS_LED, 0);
        mdelay(100);
    }
}

void
scratch_write(unsigned char byte)
{
    ath_reg_wr(ARDBEG_SCRATCH_PAD, byte);
}

unsigned char
scratch_read(void)
{
    return (ath_reg_rd(ARDBEG_SCRATCH_PAD) & 0xff);
}

static void
check_reset_button(void)
{
    char *reset_env_var;
    scratch_write(0xa5);  /* default (unset value) */
    if (!ath_get_bit(GPIO_IN_ADDRESS, GPIO_CONFIG_CLEAR)) {
        // unleash hell
        scratch_write('R');  /* reset pressed */
        reset_env_var = getenv("disable_reset_button");
        if (reset_env_var && (!strcmp(reset_env_var, "yes")))  {
            scratch_write('D');  /* disabled by env */
        } else {
            flash_status_led();
        }
    } else {
        // no config clear
        scratch_write('N');  /* no reset */
    }
}
#endif

int
early_board_init(void)
{
#ifdef CONFIG_JTAG_DISABLE
    ath_gpio_config_output(GPIO_RED_STATUS_LED);
    ath_gpio_out_val(GPIO_RED_STATUS_LED, 0);
#endif
#ifdef CONFIG_RESET_BUTTON
    ath_gpio_config_input(GPIO_CONFIG_CLEAR);
    check_reset_button();
#endif
    return 0;
}


#if defined(CONFIG_ATH_WMAC) || defined(CONFIG_AR5416)
int n_radios = 0;

static void
ardbeg_radio_callback(int n)
{
   n_radios += n;
}
#endif

#if defined(CONFIG_RESET_BUTTON)
static void
execute_config_clear(void)
{
    extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
    char *av[3];
    unsigned char reset_state = scratch_read();

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
#ifdef CONFIG_ATH_WMAC
    extern void ath_wmac_initialize(int);
    extern void ath_wmac_install_callback(void *);
#endif
#ifdef CONFIG_AR5416
    extern void ar5416_initialize(void);
    extern void ar5416_install_callback(void *);
#endif

#ifdef CONFIG_AR5416
    ar5416_install_callback(ardbeg_radio_callback);
    ar5416_initialize();
#endif
#ifdef CONFIG_ATH_WMAC
    ath_wmac_install_callback(ardbeg_radio_callback);
    ath_wmac_initialize(n_radios);
#endif

#if defined(CONFIG_RESET_BUTTON)
    execute_config_clear();
#endif
    return;
}

/*
 * From Thomas:
 * POE_AT_DETECT_L         DC_POWER        Description
 * -------------------------------------------------------
 * High (not active)       Low             Powered by PoE_AF (low power)
 * Low  (active)           Low             Powered by PoE AT (high power)
 * Low                     High (active)   Powered by DC-jack (high power)
 * High                    High            Invalid (high power)
 *
 * If both DC and POE are available, DC is used
 */
void
board_print_power_status(void)
{
    int dc = ath_get_bit(GPIO_IN_ADDRESS, GPIO_POWER_DC);
    int at = !ath_get_bit(GPIO_IN_ADDRESS, GPIO_POWER_AT);
    int poe_plus = !ath_get_bit(GPIO_IN_ADDRESS, GPIO_POWER_POE_PLUS);

    if (dc) {
        printf("Power: DC\n");
    } else if (!at) {
        if (poe_plus) {
            printf("Power: 802.3af POE+\n");
        } else {
            printf("Power: 802.3af POE\n");
        }
    } else if (at) {
        printf("Power: 802.3at POE\n");
    } else {
        printf("Power: Unknown\n");
    }
}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("Reset button: %s\n", !ath_get_bit(GPIO_IN_ADDRESS, GPIO_CONFIG_CLEAR) ? "pressed" :
        "not pressed");
    printf("PHY int     : %s\n", ath_get_bit(GPIO_IN_ADDRESS, GPIO_PHY_INT) ? "high" : "low");
    printf("AT          : %s\n", ath_get_bit(GPIO_IN_ADDRESS, GPIO_POWER_AT) ? "high" : "low");
#ifdef GPIO_POWER_POE_PLUS
    printf("POE+        : %s\n", ath_get_bit(GPIO_IN_ADDRESS, GPIO_POWER_POE_PLUS) ? "high" : "low");
#endif
#ifdef GPIO_POWER_DC
    printf("DC          : %s\n", ath_get_bit(GPIO_IN_ADDRESS, GPIO_POWER_DC) ? "high" : "low");
#endif

    return 0;
}

U_BOOT_CMD(
       gpio_pins,    1,    1,     do_gpio_pins,
       "gpio_pins    - show GPIO pin status\n",
       " Usage: gpio_pins \n"
);
