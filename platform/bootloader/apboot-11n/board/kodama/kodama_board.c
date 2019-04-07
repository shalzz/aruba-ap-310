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
void kodama_set_power_led(int onoff) {
    octeon_gpio_cfg_output(5);

    if (onoff)
	octeon_gpio_clr(5);
    else
	octeon_gpio_set(5);
}

/* "Alarm" is on GPIO 6. */
void kodama_set_alarm_led(int onoff) {
    octeon_gpio_cfg_output(6);

    if (onoff)
	octeon_gpio_clr(6);
    else
	octeon_gpio_set(6);
}

/* "Status" is on GPIO 7. */
void kodama_set_status_led(int onoff) {
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

    /* Configure switch and phys on the Kodama board */

    udelay(100000);	/* Give the switch/phy complex a full .1s to come back after the reset */

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
    /* Enable MDIO */
    cvmx_write_csr(CVMX_SMI_EN, 1);

    uint16_t val;
    int port;

    /* Set WAN PHY TX LED to be link, blink with activity */
    val = miiphy_read_wrapper(0x4, 24);
    miiphy_write(0x4, 24, val | 1);

    /* Soft reset the WAN PHY */
    val = miiphy_read_wrapper(0x4, 0);
    val |= 1 << 15;
    miiphy_write(0x4, 0, val);
    udelay(1000);

    /* Set SGMII autonegotiate off on switch PHY */
    miiphy_write(0x1d, 22, 1);
    val = miiphy_read_wrapper(0x1d, 0);
    val &= ~(1 << 12);	/* Autoneg bit */
    val |= 1 << 15;	/* Reinit SGMII interface */
    miiphy_write(0x1d, 0, val);
    miiphy_write(0x1d, 22, 0);

    /* Soft reset the switch PHY */
    val = miiphy_read_wrapper(0x1d, 0);
    val |= 1 << 15;
    miiphy_write(0x1d, 0, val);
    udelay(1000);

    /* Do workaround for SGMII errata.
    ** Documented in Marvel "RGMII to SGMII conversion using Marvell PHYs"
    ** Doc. number MV-s300635-00
    */
    miiphy_write(0x1d, 29, 0x001b);
    miiphy_write(0x1d, 30, 0x418f);
    miiphy_write(0x1d, 30, 0);

    /* Disable the switch PHY Polling Unit (PPU) (Belt & suspenders - it should already be off) */
    val = miiphy_read_wrapper(0x1B, 0);
    if ((val >> 14) != 0) {		/* If the PPU is running... */
	val = miiphy_read_wrapper(0x1B, 4);
	val &= ~(1 << 14);		/* Turn it off. */
	miiphy_write(0x1B, 4, val);

	/* Wait for the PPU to stop */
	do {
	    udelay(1000);
	    val = miiphy_read_wrapper(0x1B, 0);
	} while ((val >> 14) == 3);
    }

    /* Init the quad PHY<->switch connection */
    for (port = 0; port < 4; port++) {
	/* Set SGMII autonegotiate off */
	miiphy_write(port, 22, 1);
	val = miiphy_read_wrapper(port, 0);
	val &= ~(1 << 12);	/* Autoneg bit */
	val |= 1 << 15;	/* Reinit SGMII interface */
	miiphy_write(port, 0, val);
	udelay(1000);
	miiphy_write(port, 22, 0);
    }

    /* Init the switch */

    /* Clear MGMII mode on all ports */
    for (port = 0x10; port < 0x14; port++) {
	val = miiphy_read_wrapper(port, 0);
	val &= ~(1 << 6);
	miiphy_write(port, 0, val);
	/* Clear out SGMII/PCS autonegotiation state, it might be odd. */
	miiphy_write(port, 1, 3);
    }

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

    /* NOTE: this is early in the init process, so the serial port is not yet configured */

    env_str = getenv("serial#");
    if (env_str)
    {
	strncpy((char *)(gd->board_desc.serial_str), env_str, SERIAL_LEN);
    }
    else
    {
	strncpy((char *)(gd->board_desc.serial_str), "unknown", SERIAL_LEN);
    }
    /* Determine board type/rev */

    gd->flags = 0;
    gd->board_desc.board_type = CVMX_BOARD_TYPE_KODAMA;
    gd->board_desc.rev_major = 2;
    gd->board_desc.rev_minor = 0;
    gd->board_desc.chip_type = CVMX_CHIP_TYPE_OCTEON_SAMPLE;       
    gd->board_desc.chip_rev_major = 2;
    gd->board_desc.chip_rev_minor = 1;  


    /* Read MAC address base/count from env */
    if (!(env_str = getenv("octeon_mac_base")) || !ether_aton(env_str, (uint8_t *)(gd->mac_desc.mac_addr_base)))
    {
        gd->mac_desc.mac_addr_base[0] = 0x00;  /* Special value allows silent overrides */
        gd->mac_desc.mac_addr_base[1] = 0xba;
        gd->mac_desc.mac_addr_base[2] = 0xdd;
    }
    gd->mac_desc.count = 2;  // Kodama always has 2

    /* Default values */
    cpu_ref = 50;
    if ((octeon_get_proc_id() == OCTEON_CN38XX_PASS1))
        gd->ddr_clock_mhz = 200;   /* All boards with pass 1 are 200 Mhz */
    else
        gd->ddr_clock_mhz = OCTEON_DDR_CLOCK_MHZ;

    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;

    gd->cpu_clock_mhz = data * cpu_ref;

    kodama_set_power_led(1);
    kodama_set_status_led(0);
    kodama_set_alarm_led(1);

    return 0;
}

int late_board_init(void) {


    kodama_set_alarm_led(0);
#if CONFIG_OCTEON_FAILSAFE
    kodama_set_status_led(0);
#else
    kodama_set_status_led(1);
#endif

    octeon_led_str_write("Kodama");

    return(0);
}
void octeon_led_str_write(const char *str)
{
    octeon_led_str_write_std(str);
}
