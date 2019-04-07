#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include "ar7240_soc.h"

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
    /* Enable eth Switch LEDs */
    ar7240_reg_wr (AR7240_GPIO_FUNC, (ar7240_reg_rd(AR7240_GPIO_FUNC) | 0xf8));
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
#ifndef COMPRESSED_UBOOT
    ar7240_ddr_tap_init();
#endif
    }
    else {
	ar7240_reg_wr (AR7240_DDR_TAP_CONTROL0, 0x2);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL1, 0x2);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL2, 0x0);
        ar7240_reg_wr (AR7240_DDR_TAP_CONTROL3, 0x0); 
    }

    tap_val1 = ar7240_reg_rd(0xb800001c);
    tap_val2 = ar7240_reg_rd(0xb8000020);

#ifndef COMPRESSED_UBOOT
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

#ifdef COMPRESSED_UBOOT
int checkboard (char *board_string)
{
    if ((is_ar7241() || is_ar7242()))
        strcpy(board_string,"pb9x (ar7241 - Virian) U-boot\n");
    else
        strcpy(board_string,"pb9x (ar7240 - Python) U-boot\n");
    return 0;
}
#else
int checkboard (void)
{
    if ((is_ar7241() || is_ar7242()))
        printf("PB93 (ar7241 - Virian) U-boot\n");
    else
        printf("PB93 (ar7240 - Python) U-boot\n");

    return 0;
}

int
early_board_init(void)
{
    return 0;
}

void
late_board_init(void)
{
#if 0
    extern void ar5416_initialize(void);
    extern void ar5416_install_callback(void *);

    ar5416_install_callback(scapa_radio_callback);
    ar5416_initialize();

    // initialize green radio LED signals
    if (n_radios == 2) {
        // dual-concurrent, drive pin if output is low
        // each GPIO pin has a 2-bit configuration, so shift 10 bits for pin 5
        ar7240_reg_wr(0x1001404c, /*__cpu_to_le32*/(1<<10));    // 5G
        ar7240_reg_wr(0x1000404c, /*__cpu_to_le32*/(1<<10));    // 2.4G

        // turn the LEDs off for now (register 0x4048 on the 922x)
        ar7240_reg_wr(0x10014048, /*__cpu_to_le32*/(0x20));   // 5G
        ar7240_reg_wr(0x10014060, /*__cpu_to_le32*/(0));
        ar7240_reg_wr(0x10004048, /*__cpu_to_le32*/(0x20));   // 2.4G
        ar7240_reg_wr(0x10004060, /*__cpu_to_le32*/(0));
    } else if (n_radios == 1) {
        // XXX; this needs a lot more work once we figure out the single-radio plan
        // single radio, drive pin if output is low
        // each GPIO pin has a 2-bit configuration, so shift 10 bits for pin 5
        ar7240_reg_wr(0x1000404c, /*__cpu_to_le32*/(1<<10));    // 5/2.4G

        // turn the LEDs off for now (register 0x4048 on the 922x)
        ar7240_reg_wr(0x10004048, /*__cpu_to_le32*/(0x20));
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

    return;
#endif
}
#endif
