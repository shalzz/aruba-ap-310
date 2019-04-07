/*
 * (C) Copyright 2004,2005
 * Cavium Networks
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/au1x00.h>
#include <asm/mipsregs.h>
#include "octeon_boot.h"
#include "lib_octeon.h"
#include <lib_octeon_shared.h>
#include <pci.h>
#include <net.h>
#include <miiphy.h>


#if defined(CONFIG_PCI)
extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
}
#endif


/* LED control.  "Power" LED is on GPIO 5. */
void hikari_set_power_led(int onoff) {
    octeon_gpio_cfg_output(5);

    if (onoff)
	octeon_gpio_clr(5);
    else
	octeon_gpio_set(5);
}

/* "Alarm" is on GPIO 6. */
void hikari_set_alarm_led(int onoff) {
    octeon_gpio_cfg_output(6);

    if (onoff)
	octeon_gpio_clr(6);
    else
	octeon_gpio_set(6);
}

/* "Status" is on GPIO 7. */
void hikari_set_status_led(int onoff) {
    octeon_gpio_cfg_output(7);

    if (onoff)
	octeon_gpio_clr(7);
    else
	octeon_gpio_set(7);
}

int checkboard (void)
{
    /* Reset the phys/switches on gpio 8 - 11 */

    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(8), 0x1);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(9), 0x1);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(10), 0x1);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(11), 0x1);
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(8), 0x1);

    cvmx_write_csr(CVMX_GPIO_TX_CLR, 0xf00);
    udelay(15000);  /* Minimum reset pulse is 10ms */
    cvmx_write_csr(CVMX_GPIO_TX_SET, 0xf00);

    /* Configure switch and phys on the Hikari board */

    udelay(100000);	/* Give the switch/phy complex a full .1s to come back after the reset */

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)

    uint64_t val;

    /* Force us into GMII mode, enable interface */
    cvmx_write_csr (CVMX_GMXX_INF_MODE (0), 0x3);

    /* Enable SMI to talk with the GMII switch */
    cvmx_write_csr(CVMX_SMI_EN, 0x1);

    /* Set WAN PHY TX LED to be link, blink with activity */
    val = miiphy_read_wrapper(0x4, 24);
    miiphy_write(0x4, 24, val | 1);

    /* Soft reset the WAN PHY */
    val = miiphy_read_wrapper(0x4, 0);
    val |= 1 << 15;
    miiphy_write(0x4, 0, val);
    udelay(1000);

    /* Clear MGMII mode on port 9 */
    val = miiphy_read_wrapper(0x19, 0);
    val &= ~(1 << 6);
    miiphy_write(0x19, 0, val);

    /* Force port 9 to 1Gbps FDX, link up, flow control disabled */
    miiphy_write(0x19, 1, 0x3E);

    /* Turn on the PHY polling unit (PPU) so it'll read port states from the attached PHYs */
    val = miiphy_read_wrapper(0x1B, 4);
    val |= 1 << 15 | 1 << 14;	/* SWReset | PPUEn */
    miiphy_write(0x1B, 4, val);

#endif

    /* End switch/phy configuration */

    return 0;
}


int early_board_init(void)
{
    char *env_str;
    int cpu_ref = 50;

    DECLARE_GLOBAL_DATA_PTR;

    memset((void *)&(gd->mac_desc), 0x0, sizeof(octeon_eeprom_mac_addr_t));
    memset((void *)&(gd->clock_desc), 0x0, sizeof(octeon_eeprom_clock_desc_t));
    memset((void *)&(gd->board_desc), 0x0, sizeof(octeon_eeprom_board_desc_t));
#if CONFIG_OCTEON_EEPROM_TUPLES
    uint8_t ee_buf[OCTEON_EEPROM_MAX_TUPLE_LENGTH];
    int addr;
#endif   /* CONFIG_OCTEON_EEPROM_TUPLES */
    /* NOTE: this is early in the init process, so the serial port is not yet configured */

    gd->flags = 0;
    gd->board_desc.board_type = CVMX_BOARD_TYPE_HIKARI;
    gd->board_desc.rev_major = 2;
    gd->board_desc.rev_minor = 0;
    gd->board_desc.chip_type = 0;       
    gd->board_desc.chip_rev_major = 0;
    gd->board_desc.chip_rev_minor = 0;  

    /* Determine board type/rev */
    strncpy((char *)(gd->board_desc.serial_str), "unknown", SERIAL_LEN);

#if CONFIG_OCTEON_EEPROM_TUPLES
    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_BOARD_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->board_desc), ee_buf, sizeof(octeon_eeprom_board_desc_t));
    }
    else
    {
#if 0
        /* Be silent on missing desc */
        gd->flags |= GD_FLG_BOARD_DESC_MISSING;
#endif
    }
#endif  /* CONFIG_OCTEON_EEPROM_TUPLES */
    /* Allow serial# env variable to override board desc */
    env_str = getenv("serial#");
    if (env_str)
    {
	strncpy((char *)(gd->board_desc.serial_str), env_str, SERIAL_LEN);
    }

#if CONFIG_OCTEON_EEPROM_TUPLES
    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_CLOCK_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->clock_desc), ee_buf, sizeof(octeon_eeprom_clock_desc_t));
        cpu_ref = gd->clock_desc.cpu_ref_clock_mhz_x_8/8;
    }
    else
#endif /* CONFIG_OCTEON_EEPROM_TUPLES */
    {
#if 0 && CONFIG_OCTEON_EEPROM_TUPLES
        /* be silent on missing desc */
        gd->flags |= GD_FLG_CLOCK_DESC_MISSING;
#endif
        /* Default values */
        cpu_ref = 50;
        gd->ddr_clock_mhz = OCTEON_DDR_CLOCK_MHZ;
    }


    /* Set default MAC address in case empty eeprom and env. */
    gd->mac_desc.mac_addr_base[0] = 0x00;  /* Special value allows silent overrides */
    gd->mac_desc.mac_addr_base[1] = 0xba;
    gd->mac_desc.mac_addr_base[2] = 0xdd;
    gd->mac_desc.count = 2;  // Hikari always has 2
#if CONFIG_OCTEON_EEPROM_TUPLES
    /* Get MAC addresses from eeprom if present */
    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_MAC_ADDR_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->mac_desc), ee_buf, sizeof(octeon_eeprom_mac_addr_t));
    }
#endif /* CONFIG_OCTEON_EEPROM_TUPLES */

    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;

    gd->cpu_clock_mhz = data * cpu_ref;

    hikari_set_power_led(1);
    hikari_set_status_led(0);
    hikari_set_alarm_led(1);

    return 0;
}

int late_board_init(void) {


    hikari_set_alarm_led(0);
#if CONFIG_OCTEON_FAILSAFE
    hikari_set_status_led(0);
#else
    hikari_set_status_led(1);
#endif

    octeon_led_str_write("Hikari");

    return(0);
}
void octeon_led_str_write(const char *str)
{
    octeon_led_str_write_std(str);
}
