// vim:set ts=4 sw=4 expandtab:
/*****************************************************************************/
/*! file ap94.c
** /brief Boot support for AP94 board
**    
**  This provides the support code required for the AP94 board in the U-Boot
**  environment.  This board is a Hydra based system with two Merlin WLAN
**  interfaces.
**
**  Copyright (c) 2008 Atheros Communications Inc.  All rights reserved.
**
*/

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <config.h>
#include <version.h>
#include <watchdog.h>
#include "ar7100_soc.h"

#if defined(CONFIG_RESET_BUTTON)
#define REG_OFFSET             4
#define OFS_SCRATCH_PAD        (7*REG_OFFSET)
#define         UART16550_READ(y)   ar7100_reg_rd((AR7100_UART_BASE+y))
#define         UART16550_WRITE(x, z)  ar7100_reg_wr((AR7100_UART_BASE+x), z)
#endif

extern int ar7100_ddr_find_size(void);

/******************************************************************************/
/*!
**  \brief ar7100_mem_config
**
**  This is a "C" level implementation of the memory configuration interface
**  for the AP94.  
**
**  \return RAM size in bytes
*/

int
ar7100_mem_config(void)
{

    /* XXX - should be set based board configuration */
    *(volatile unsigned int *)0xb8050004 = 0x50C0;
    udelay(10);
    *(volatile unsigned int *)0xb8050018 = 0x1313;
    udelay(10);
    *(volatile unsigned int *)0xb805001c = 0x67;	// 66Mhz PCI
    udelay(10);
    *(volatile unsigned int *)0xb8050010 = 0x1099;
    udelay(10);

    return (ar7100_ddr_find_size());
}

/******************************************************************************/
/*!
**  \brief ar7100_usb_initial_config
**
**  -- Enter Detailed Description --
**
**  \param param1 Describe Parameter 1
**  \param param2 Describe Parameter 2
**  \return Describe return value, or N/A for void
*/

long int initdram(int board_type)
{
//    printf("b8050000: 0x%x\n",* (unsigned long *)(0xb8050000));
    return (ar7100_mem_config());
}

/******************************************************************************/
/*!
**  \brief ar7100_usb_initial_config
**
**  -- Enter Detailed Description --
**
**  \param param1 Describe Parameter 1
**  \param param2 Describe Parameter 2
**  \return Describe return value, or N/A for void
*/

int checkboard (void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif
    ar7100_gpio_config_output(GPIO_RED_STATUS_LED);
    ar7100_gpio_config_output(GPIO_GREEN_STATUS_LED);
    ar7100_gpio_config_output(GPIO_2G_RED_LED);
    ar7100_gpio_config_output(GPIO_5G_RED_LED);

    // drive reset pin high, but don't enable it as an output
    ar7100_gpio_out_val(GPIO_HW_RESET, 1);

    // turn red radio signals off as early as possible
    ar7100_gpio_out_val(GPIO_5G_RED_LED, 1);
    ar7100_gpio_out_val(GPIO_2G_RED_LED, 1);

    return 0;
}

#if defined(CONFIG_RESET_BUTTON)
static void
flash_status_led(void)
{
    unsigned int i;

    ar7100_gpio_config_output(GPIO_RED_STATUS_LED);
    ar7100_gpio_config_output(GPIO_GREEN_STATUS_LED);
    for (i = 0; i < 10; i++) {
        ar7100_gpio_out_val(GPIO_GREEN_STATUS_LED, 0);
        mdelay(100);
        ar7100_gpio_out_val(GPIO_GREEN_STATUS_LED, 1);
        mdelay(100);
    }
}

static void
check_reset_button(void)
{
    char *reset_env_var;
    UART16550_WRITE(OFS_SCRATCH_PAD, 0xa5);  /* default value */
    if (!ar7100_get_bit(AR7100_GPIO_IN, GPIO_CONFIG_CLEAR)) {
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
    ar7100_gpio_out_val(GPIO_RED_STATUS_LED, 1);
#ifdef CONFIG_RESET_BUTTON
    ar7100_reg_rmw_clear (AR7100_GPIO_OE, 1 << GPIO_CONFIG_CLEAR);
    check_reset_button();
#endif

	return 0;
}

int n_radios = 0;

static void
talisker_radio_callback(int n)
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
    extern void s35390_reset(void);

    ar5416_install_callback(talisker_radio_callback);
    ar5416_initialize();

    // initialize green radio LED signals
    if (n_radios == 2) {
        // dual-concurrent, drive pin if output is low
        // each GPIO pin has a 2-bit configuration, so shift 10 bits for pin 5
        ar7100_reg_wr(0x1001404c, /*__cpu_to_le32*/(1<<10));    // 5G
        ar7100_reg_wr(0x1000404c, /*__cpu_to_le32*/(1<<10));    // 2.4G

        // turn the LEDs off for now (register 0x4048 on the 922x)
        ar7100_reg_wr(0x10014048, /*__cpu_to_le32*/(0x20));   // 5G
        ar7100_reg_wr(0x10014060, /*__cpu_to_le32*/(0));
        ar7100_reg_wr(0x10004048, /*__cpu_to_le32*/(0x20));   // 2.4G
        ar7100_reg_wr(0x10004060, /*__cpu_to_le32*/(0));
    } else if (n_radios == 1) {
        // XXX; this needs a lot more work once we figure out the single-radio plan
        // single radio, drive pin if output is low
        // each GPIO pin has a 2-bit configuration, so shift 10 bits for pin 5
        ar7100_reg_wr(0x1000404c, /*__cpu_to_le32*/(1<<10));    // 5/2.4G

        // turn the LEDs off for now (register 0x4048 on the 922x)
        ar7100_reg_wr(0x10004048, /*__cpu_to_le32*/(0x20));
        ar7100_reg_wr(0x10004060, /*__cpu_to_le32*/(0));
    } else {
        printf("No radios found\n");
    }

    //
    // turn off red LED signals to be safe; we already did this, so it's probably not
    // necessary, but it's also harmless
    //
    ar7100_gpio_out_val(GPIO_5G_RED_LED, 1);
    ar7100_gpio_out_val(GPIO_2G_RED_LED, 1);

    s35390_reset();
#if defined(CONFIG_RESET_BUTTON)
    execute_config_clear();
#endif

	return;
}

/*
 * sets up flash_info and returns size of FLASH (bytes)
 */
unsigned long 
flash_get_geom (flash_info_t *flash_info)
{
    int i;
    
    /* XXX this is hardcoded until we figure out how to read flash id */

    flash_info->flash_id  = FLASH_M25P64;
    flash_info->size = CFG_FLASH_SIZE; /* bytes */
    flash_info->sector_count = flash_info->size/CFG_FLASH_SECTOR_SIZE;

    for (i = 0; i < flash_info->sector_count; i++) {
        flash_info->start[i] = CFG_FLASH_BASE + (i * CFG_FLASH_SECTOR_SIZE);
        flash_info->protect[i] = 0;
    }

//    printf ("flash size 8MB, sector count = %d\n", flash_info->sector_count);
    return (flash_info->size);

}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("Reset button: %s\n", !ar7100_get_bit(AR7100_GPIO_IN, GPIO_CONFIG_CLEAR) ? "pressed" :
        "not pressed");

    return 0;
}

U_BOOT_CMD(
	gpio_pins,    1,    1,     do_gpio_pins,
	"gpio_pins    - show GPIO pin status\n",                   
	" Usage: gpio_pins \n"
);

