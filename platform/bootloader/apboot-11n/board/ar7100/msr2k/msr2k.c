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

extern int ar7100_ddr_find_size(void);

#if defined(CONFIG_MSR_SUBTYPE)
extern int __second_flash_exist;
extern flash_info_t flash_info_second;

/* RTC I2C addr. Copy from rtc/ds1374.c */
#define AR7100_RTC_I2C_ADDR             0x68
#define AR7100_RTC_CTL_ADDR             0x07
#define AR7100_RTC_CTL_BIT_WACE         (1<<6)

extern uchar i2c_reg_read(uchar i2c_addr, uchar reg);
extern void i2c_reg_write(uchar i2c_addr, uchar reg, uchar val);

/* Disable RTC watchdog function */
void
rtc_wd_off(void)
{
    uchar val;

    val = i2c_reg_read(AR7100_RTC_I2C_ADDR, AR7100_RTC_CTL_ADDR);
    val &= ~AR7100_RTC_CTL_BIT_WACE;
    i2c_reg_write(AR7100_RTC_I2C_ADDR, AR7100_RTC_CTL_ADDR, val);
}
#endif /* CONFIG_MSR_SUBTYPE */

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
    *(volatile unsigned int *)0xb805001c = 0xee;	// 33Mhz PCI
    udelay(10); /* moved PCI setting from below to fix WNC 5% fail */
    *(volatile unsigned int *)0xb8050004 = 0x50C0;
    udelay(10);
    *(volatile unsigned int *)0xb8050018 = 0x1313;
    udelay(10);
    *(volatile unsigned int *)0xb8050010 = 0x01111000;
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
#if defined(CONFIG_MSR_SUBTYPE)
    /* 
    * Disable RTC watchdog function. Because the MeshOS can enable RTC
    * watchdog function, and the command "reboot" in MeshOS cannot reset
    * peripheral chipset, after reboot from MeshOS, the RTC watchdog still
    * run. So diable it when APBoot initialize.
    */
    rtc_wd_off();
#endif
    WATCHDOG_RESET();
#endif
    //ar7100_gpio_config_output(GPIO_RED_STATUS_LED);
    //ar7100_gpio_config_output(GPIO_GREEN_STATUS_LED);
    //ar7100_gpio_config_output(GPIO_FLASH_WRITE);
    //ar7100_gpio_config_output(GPIO_2G_RED_LED);
    //ar7100_gpio_config_output(GPIO_5G_RED_LED);
#ifdef CONFIG_NOT_MSR4K_TYPE
    ar7100_gpio_config_output(GPIO_2G_GREEN_LED);
    ar7100_gpio_config_output(GPIO_5G_GREEN_LED);
#endif
    // drive reset pin high, but don't enable it as an output
    //ar7100_gpio_out_val(GPIO_HW_RESET, 1);

    // turn red/green radio signals off as early as possible
    //ar7100_gpio_out_val(GPIO_5G_RED_LED, 1);
    //ar7100_gpio_out_val(GPIO_2G_RED_LED, 1);
    //ar7100_gpio_out_val(GPIO_5G_GREEN_LED, 1);
    //ar7100_gpio_out_val(GPIO_2G_GREEN_LED, 1);

    return 0;
}

int
early_board_init(void)
{
    // turn off red LED
    //ar7100_gpio_out_val(GPIO_RED_STATUS_LED, 1);
    // turn off radio LEDs
#ifdef CONFIG_NOT_MSR4K_TYPE
    ar7100_gpio_out_val(GPIO_2G_GREEN_LED, 0);
    ar7100_gpio_out_val(GPIO_5G_GREEN_LED, 0);
#endif
	return 0;
}

#ifdef CONFIG_AR5416
int n_radios = 0;

static void
jura_radio_callback(int n)
{
   n_radios = n;
}
#endif

void
late_board_init(void)
{
#ifdef CONFIG_AR5416
    extern void ar5416_initialize(void);
    extern void ar5416_install_callback(void *);

    ar5416_install_callback(jura_radio_callback);
    ar5416_initialize();
#endif

#if 0
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
#endif
    //
    // turn off red/green LED signals to be safe; we already did this, so it's 
    // probably not necessary, but it's also harmless
    //
    //ar7100_gpio_out_val(GPIO_5G_RED_LED, 1);
    //ar7100_gpio_out_val(GPIO_2G_RED_LED, 1);
    //ar7100_gpio_out_val(GPIO_5G_GREEN_LED, 1);
    //ar7100_gpio_out_val(GPIO_2G_GREEN_LED, 1);
#ifdef CONFIG_NOT_MSR4K_TYPE
    ar7100_gpio_out_val(GPIO_5G_GREEN_LED, 0);
    ar7100_gpio_out_val(GPIO_2G_GREEN_LED, 0);
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

#if defined(CONFIG_MSR_SUBTYPE)
    /* Initialize the second flash */
    if (__second_flash_exist) {
       memset(&flash_info_second, 0, sizeof(flash_info_t)); 
       flash_info_second.flash_id = FLASH_M25P64;
       flash_info_second.size = CFG_FLASH_SIZE;
       flash_info_second.sector_count = flash_info_second.size / CFG_FLASH_SECTOR_SIZE;
       
       for (i = 0; i < flash_info_second.sector_count; i++) {
           flash_info_second.start[i] = i * CFG_FLASH_SECTOR_SIZE;
           flash_info_second.protect[i] = 0;
       }
    }
#endif
//    printf ("flash size 8MB, sector count = %d\n", flash_info->sector_count);
    return (flash_info->size);
}
