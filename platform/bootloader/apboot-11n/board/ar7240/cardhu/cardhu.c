#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include <watchdog.h>
#include "ar7240_soc.h"

/* scratch-pad address used for Cardhu  */
/* (aka, last word of 32k On-Chip SRAM) */
#define CARDHU_SCRATCH_PAD 0xbd007ffc

extern int wasp_ddr_initial_config(uint32_t refresh);
extern int ar7240_ddr_find_size(void);

#ifdef COMPRESSED_UBOOT
#	define prmsg(...)
#else
#	define prmsg	printf
#endif

void
wasp_usb_initial_config(void)
{
#define unset(a)	(~(a))

	if ((ar7240_reg_rd(WASP_BOOTSTRAP_REG) & WASP_REF_CLK_25) == 0) {
		ar7240_reg_wr_nf(AR934X_SWITCH_CLOCK_SPARE,
			ar7240_reg_rd(AR934X_SWITCH_CLOCK_SPARE) |
			SWITCH_CLOCK_SPARE_USB_REFCLK_FREQ_SEL_SET(2));
	} else {
		ar7240_reg_wr_nf(AR934X_SWITCH_CLOCK_SPARE,
			ar7240_reg_rd(AR934X_SWITCH_CLOCK_SPARE) |
			SWITCH_CLOCK_SPARE_USB_REFCLK_FREQ_SEL_SET(5));
	}

	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) |
		RST_RESET_USB_PHY_SUSPEND_OVERRIDE_SET(1));
	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) &
		unset(RST_RESET_USB_PHY_RESET_SET(1)));
	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) &
		unset(RST_RESET_USB_PHY_ARESET_SET(1)));
	udelay(1000);
	ar7240_reg_wr(AR7240_RESET,
		ar7240_reg_rd(AR7240_RESET) &
		unset(RST_RESET_USB_HOST_RESET_SET(1)));
	udelay(1000);
	if ((ar7240_reg_rd(AR7240_REV_ID) & 0xf) == 0) {
		/* Only for WASP 1.0 */
		ar7240_reg_wr(0xb8116c84 ,
			ar7240_reg_rd(0xb8116c84) & unset(1<<20));
	}
}

void wasp_gpio_config(void)
{
#if 0
	/* Disable clock obs */
	ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) & 0xffe7e0ff));
	/* Enable eth Switch LEDs */
#ifdef CONFIG_K31
	ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) | 0xd8));
#else
	ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) | 0xfa));
#endif
#endif
}

int
wasp_mem_config(void)
{
	unsigned int type, reg32;

	type = wasp_ddr_initial_config(CFG_DDR_REFRESH_VAL);

	/* Take WMAC out of reset */
	reg32 = ar7240_reg_rd(AR7240_RESET);
	reg32 = reg32 &  ~AR7240_RESET_WMAC;
	ar7240_reg_wr_nf(AR7240_RESET, reg32);

	/* Switching regulator settings */
	ar7240_reg_wr_nf(0x18116c40, 0x633c8176); /* AR_PHY_PMU1 */
#if !defined(CONFIG_ATH_NAND_FL)
	if (ar7240_reg_rd(AR7240_REV_ID) & 0xf) {
		if (type == 2) {
			// ddr1
			ar7240_reg_wr_nf(0x18116c44, 0x10000000); /* AR_PHY_PMU2 */
		} else {
			// ddr2 & sdram
			ar7240_reg_wr_nf(0x18116c44, 0x10380000); /* AR_PHY_PMU2 */
		}
	} else {
		ar7240_reg_wr_nf(0x18116c44, 0x10380000); /* AR_PHY_PMU2 */
	}
#endif

	wasp_usb_initial_config();

	wasp_gpio_config();

	reg32 = ar7240_ddr_find_size();

	return reg32;
}

long int initdram(int board_type)
{
	return (wasp_mem_config());
}

#ifdef COMPRESSED_UBOOT
int	checkboard(char *s)
#else
int	checkboard(void)
#endif
{
#if 0
#ifdef COMPRESSED_UBOOT
#if CONFIG_AP123
	strcpy(s, "U-boot AP123\n");
#elif CONFIG_MI124
	strcpy(s, "U-boot MI124\n");
#else
	strcpy(s, "U-boot DB120\n");
#endif
#endif
#if CONFIG_AP123
	prmsg("U-boot AP123\n");
#elif CONFIG_MI124
	prmsg("U-boot MI124\n");
#else
	prmsg("U-boot DB120\n");
#endif
#endif

#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif

#ifdef CONFIG_CARDHU
    /* set GPIO output-enable and function registers to something sane */
#if CONFIG_PSE_SHUTDOWN
    ar7240_reg_wr(AR7240_GPIO_OE, 0x00031b01);  /* set appropriate gpio direction (keep GPIO1 & GPIO3 outputs) */
#else
    ar7240_reg_wr(AR7240_GPIO_OE, 0x00031b0b);  /* set appropriate gpio direction bits */
#endif
    ar7240_reg_wr(AR7240_GPIO_SET, 0x00701800);  /* bits 11, 12, 20, 21, & 22 high */
    ar7240_reg_wr(AR7240_GPIO_INT_ENABLE, 0x00010000);   /* bit 16 = usb fault */
    ar7240_reg_wr(AR7240_GPIO_INT_TYPE, 0x00010000);     /* level-sensitive */
    ar7240_reg_wr(AR7240_GPIO_INT_POLARITY, 0x00000000); /* active low */
    ar7240_reg_wr(AR7240_GPIO_INT_MASK, 0x00010000);     /* pass interrupt */
    ar7240_reg_wr(AR934X_GPIO_IN_ETH_SWITCH_LED, 0x000fffff);  /* route all LED signals (does this do anything?) */
    ar7240_reg_wr(AR934X_GPIO_OUT_FUNCTION0, 0x00000000);  /* (3-0) = TMS, TDO, TDI, TCK (if jtag enabled) - 0, 0, 0, 0 */
    ar7240_reg_wr(AR934X_GPIO_OUT_FUNCTION1, 0x0b0a0900);  /* (7-4) = SPI_MOSI (0x0b), SPI_CLK (0x0a), SPI_CS_0 (0x09), 0 */
    ar7240_reg_wr(AR934X_GPIO_OUT_FUNCTION2, 0x00180000);  /* (11-8) = 0, UART_SOUT (0x18), 0, 0 */
    ar7240_reg_wr(AR934X_GPIO_OUT_FUNCTION3, 0x00000000);  /* (15-12) = 0, 0, 0, 0 */
    ar7240_reg_wr(AR934X_GPIO_OUT_FUNCTION4, 0x2f2e0000);  /* (19-16) = GPIO19_Ch1 (0x2f), GPIO18_Ch0 (0x2e), 0, 0 */
    ar7240_reg_wr(AR934X_GPIO_OUT_FUNCTION5, 0x002c2d2b);  /* (22-20) = 0x00, LINK[3] (0x2c), LINK[4] (0x2d), LINK[2] (0x2b) */
    ar7240_reg_wr(AR934X_GPIO_IN_ENABLE0, 0x00000908);  /* UART0_SIN = 0x9, SPI_DATA_IN = 0x8 */
    ar7240_reg_wr(AR934X_GPIO_IN_ENABLE1, 0x00000000);  /* I2S Interface */
    ar7240_reg_wr(AR934X_GPIO_IN_ENABLE2, 0x00000000);  /* ETH_RX */
    ar7240_reg_wr(AR934X_GPIO_IN_ENABLE3, 0x00000000);  /* External MDIO */
    ar7240_reg_wr(AR934X_GPIO_IN_ENABLE4, 0x00000000);  /* SLIC Interface */
    ar7240_reg_wr(AR934X_GPIO_IN_ENABLE9, 0x00000000);  /* UART1 */
#if CONFIG_JTAG_DISABLE
    ar7240_reg_wr(AR934X_GPIO_FUNC, 0x00000002);        /* JTAG & CLK obs (disable jtag) */
#else
    ar7240_reg_wr(AR934X_GPIO_FUNC, 0x00000000);        /* JTAG & CLK obs */
#endif
#endif
    // specifically map LEDS
    ar7240_gpio_config_output(GPIO_RED_STATUS_LED);
    ar7240_gpio_config_output(GPIO_GREEN_STATUS_LED);
    ar7240_gpio_config_output(GPIO_2G_RED_LED);
    ar7240_gpio_config_output(GPIO_2G_GREEN_LED);

    // drive reset pin high, but don't enable it as an output
#ifdef GPIO_HW_RESET
    ar7240_gpio_out_val(GPIO_HW_RESET, 1);
#endif

    // turn red/green radio signals off as early as possible
    ar7240_gpio_out_val(GPIO_2G_RED_LED, 1);
    ar7240_gpio_out_val(GPIO_2G_GREEN_LED, 1);

    // turn off red status led
    ar7240_gpio_out_val(GPIO_RED_STATUS_LED, 1);

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

void
scratch_write(unsigned char byte)
{
    ar7240_reg_wr(CARDHU_SCRATCH_PAD, byte);
}

unsigned char
scratch_read(void)
{
    return (ar7240_reg_rd(CARDHU_SCRATCH_PAD) & 0xff);
}

static void
check_reset_button(void)
{
    char *reset_env_var;
    scratch_write(0xa5);  /* default (unset value) */
    if (!ar7240_get_bit(AR7240_GPIO_IN, GPIO_CONFIG_CLEAR)) {
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
    // turn off red LED
    ar7240_gpio_out_val(GPIO_RED_STATUS_LED, 1);
#ifdef CONFIG_RESET_BUTTON
    ar7240_gpio_config_input(GPIO_CONFIG_CLEAR);
    check_reset_button();
#endif
    return 0;
}

#ifdef CONFIG_ATH_WMAC
int n_radios = 0;

static void
cardhu_radio_callback(int n)
{
   n_radios = n;
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

    ath_wmac_install_callback(cardhu_radio_callback);
    ath_wmac_initialize(0);
#endif

#if 0
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
    ar7240_gpio_out_val(GPIO_2G_RED_LED, 1);
    ar7240_gpio_out_val(GPIO_2G_GREEN_LED, 1);
#endif

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

void
ath_set_tuning_caps(void)
{
}
