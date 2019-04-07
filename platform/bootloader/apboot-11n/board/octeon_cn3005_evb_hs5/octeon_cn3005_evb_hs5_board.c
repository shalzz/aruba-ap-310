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
#include <pci.h>
#include <lib_octeon_shared.h>
#include <lib_octeon.h>

#include "vcs7385.h"


#if defined(CONFIG_PCI)
extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
}
#endif


int octeon_get_board_major_rev(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return(gd->board_desc.rev_major);
}

int octeon_get_board_minor_rev(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return(gd->board_desc.rev_minor);
}

/* "Status" is on GPIO 1. */
void cn3005_evb_hs5_status_led(int onoff) {
    octeon_gpio_cfg_output(1);

    if (onoff)
	octeon_gpio_set(1);     /* Turn LED ON */
    else
	octeon_gpio_clr(1);     /* Turn LED OFF */
}

/* WAN PHY Reset is on GPIO 5 */
void cn3005_evb_hs5_wan_phy_reset(int onoff) {
    octeon_gpio_cfg_output(5);

    if (onoff)
	octeon_gpio_clr(5);     /* Assert Reset */
    else
	octeon_gpio_set(5);     /* Deassert Reset */
}

/* VCS Reset is on GPIO 4 */
void cn3005_evb_hs5_vcs_reset(int onoff) {
    octeon_gpio_cfg_output(4);

    if (onoff)
	octeon_gpio_clr(4);     /* Assert Reset */
    else
	octeon_gpio_set(4);     /* Deassert Reset */
}

/* VCS Chip Select is on GPIO 3 */
void cn3005_evb_hs5_vcs_chip_select(int onoff) {
    octeon_gpio_cfg_output(3);

    if (onoff)
	octeon_gpio_set(3);     /* CS = 1 */
    else
	octeon_gpio_clr(3);     /* CS = 0 */
}

/* SLIC Reset is on GPIO 2 */
void cn3005_evb_hs5_slic_reset(int onoff) {
    octeon_gpio_cfg_output(2);

    if (onoff)
	octeon_gpio_clr(2);     /* Assert Reset */
    else
	octeon_gpio_set(2);     /* Deassert Reset */
}


int checkboard (void)
{

    cn3005_evb_hs5_wan_phy_reset(0); /* Deassert Reset */
    cn3005_evb_hs5_vcs_reset(0); /* Deassert Reset */
    cn3005_evb_hs5_slic_reset(0); /* Deassert Reset */

    /* Force us into RGMII mode, enable interface */
    /* NOTE:  Applications expect this to be set appropriately
    ** by the bootloader, and will configure the interface accordingly */
    cvmx_write_csr (CVMX_GMXX_INF_MODE (0), 0x2);


    mpi_init();

    /* Initialize switch and load code into internal 8051 cpu */   
    vitesse_init();

    return 0;
}



int early_board_init(void)
{
    int cpu_ref = 50;

    DECLARE_GLOBAL_DATA_PTR;

    memset((void *)&(gd->mac_desc), 0x0, sizeof(octeon_eeprom_mac_addr_t));
    memset((void *)&(gd->clock_desc), 0x0, sizeof(octeon_eeprom_clock_desc_t));
    memset((void *)&(gd->board_desc), 0x0, sizeof(octeon_eeprom_board_desc_t));

    /* NOTE: this is early in the init process, so the serial port is not yet configured */


    /* Populate global data from eeprom */
    uint8_t ee_buf[OCTEON_EEPROM_MAX_TUPLE_LENGTH];
    int addr;

    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_CLOCK_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->clock_desc), ee_buf, sizeof(octeon_eeprom_clock_desc_t));
    }


    /* Determine board type/rev */
    strncpy((char *)(gd->board_desc.serial_str), "unknown", SERIAL_LEN);
    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_BOARD_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->board_desc), ee_buf, sizeof(octeon_eeprom_board_desc_t));
    }
    else
    {
        gd->flags |= GD_FLG_BOARD_DESC_MISSING;
        gd->board_desc.rev_minor = 0;
        gd->board_desc.board_type = CVMX_BOARD_TYPE_CN3005_EVB_HS5;
        gd->board_desc.rev_major = 1;
    }
    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_CLOCK_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->clock_desc), ee_buf, sizeof(octeon_eeprom_clock_desc_t));
        cpu_ref = gd->clock_desc.cpu_ref_clock_mhz_x_8/8;
        gd->ddr_clock_mhz = gd->clock_desc.ddr_clock_mhz;
    }
    else
    {
        /* Clocks are hardwired on the board, so these should always be good*/
        gd->ddr_clock_mhz = 266;
        cpu_ref = 50;
    }



    /* Some sanity checks */
    if (cpu_ref <= 0)
    {
        /* Default to 50 */
        cpu_ref = 50;
    }
    if (gd->ddr_clock_mhz <= 0)
    {
        /* Default to 200 */
        gd->ddr_clock_mhz = 200;
    }

    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_MAC_ADDR_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->mac_desc), ee_buf, sizeof(octeon_eeprom_mac_addr_t));
    }
    else
    {
        /* Make up some MAC addresses */
        gd->mac_desc.count = 255;
        gd->mac_desc.mac_addr_base[0] = 0x00;
        gd->mac_desc.mac_addr_base[1] = 0xDE;
        gd->mac_desc.mac_addr_base[2] = 0xAD;
        gd->mac_desc.mac_addr_base[3] = (gd->board_desc.rev_major<<4) | gd->board_desc.rev_minor;
        gd->mac_desc.mac_addr_base[4] = gd->board_desc.serial_str[0];
        gd->mac_desc.mac_addr_base[5] = 0x00;
    }



    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;


    gd->cpu_clock_mhz = data * cpu_ref;

    /* adjust for 33.33 Mhz clock */
    if (cpu_ref == 33)
        gd->cpu_clock_mhz += (data)/4 + data/8;

    if (gd->cpu_clock_mhz < 100 || gd->cpu_clock_mhz > 600)
    {
        gd->cpu_clock_mhz = DEFAULT_ECLK_FREQ_MHZ;
    }

    cn3005_evb_hs5_vcs_chip_select(1);
    cn3005_evb_hs5_status_led(0); /* Status LED Off */

    cn3005_evb_hs5_wan_phy_reset(1); /* Assert Reset */
    cn3005_evb_hs5_vcs_reset(1); /* Assert Reset */
    cn3005_evb_hs5_slic_reset(1); /* Assert Reset */

    return 0;
}

int late_board_init(void) {

#if CONFIG_OCTEON_FAILSAFE
    cn3005_evb_hs5_status_led(0); /* Status LED Off */
#else
    cn3005_evb_hs5_status_led(1); /* Status LED On */
#endif

    return(0);
}
void octeon_led_str_write(const char *str)
{
    octeon_led_str_write_std(str);
}

