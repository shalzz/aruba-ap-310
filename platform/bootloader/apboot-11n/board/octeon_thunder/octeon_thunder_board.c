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
#include <net.h>
#include <lib_octeon_shared.h>
#include <lib_octeon.h>


#if defined(CONFIG_PCI)
extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
}
#endif




int checkboard (void)
{
    return 0;
}

int early_board_init(void)
{
    DECLARE_GLOBAL_DATA_PTR;

    char *env_str;
    int cpu_ref;

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
        gd->board_desc.board_type = CVMX_BOARD_TYPE_THUNDER;
        /* Try to determine board rev by looking at PAL */
        gd->board_desc.rev_major = 1;

    }

    if (gd->clock_desc.cpu_ref_clock_mhz_x_8)
    {
        cpu_ref = gd->clock_desc.cpu_ref_clock_mhz_x_8/8;
        gd->ddr_clock_mhz = gd->clock_desc.ddr_clock_mhz;
    }
    else
    {
        gd->flags |= GD_FLG_CLOCK_DESC_MISSING;
        /* Default values */
        cpu_ref = OCTEON_CPU_REF_MHZ;
        gd->ddr_clock_mhz = OCTEON_DDR_CLOCK_MHZ;
    }

    /* Some sanity checks */
    if (cpu_ref <= 0)
        cpu_ref = OCTEON_CPU_REF_MHZ;

    if (gd->ddr_clock_mhz <= 0)
        gd->ddr_clock_mhz = OCTEON_DDR_CLOCK_MHZ;

    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_MAC_ADDR_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->mac_desc), ee_buf, sizeof(octeon_eeprom_mac_addr_t));
    }
    else
    {
        /* Read MAC address base/count from env */
        if (!(env_str = getenv("octeon_mac_base")) || !ether_aton(env_str, (uint8_t *)(gd->mac_desc.mac_addr_base)))
        {
            gd->mac_desc.count = 14;
            /* Make up some MAC addresses */
            gd->mac_desc.count = 255;
            gd->mac_desc.mac_addr_base[0] = 0x00;
            gd->mac_desc.mac_addr_base[1] = 0xDE;
            gd->mac_desc.mac_addr_base[2] = 0xAD;
            gd->mac_desc.mac_addr_base[3] = (gd->board_desc.rev_major<<4) | gd->board_desc.rev_minor;
            gd->mac_desc.mac_addr_base[4] = gd->board_desc.serial_str[0];
            gd->mac_desc.mac_addr_base[5] = 0x00;
        }
    }



    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;


    gd->cpu_clock_mhz = data * cpu_ref;


    /* Set the LAN bypass defaults for Thunder */
    octeon_gpio_cfg_output(6); /* GPIO 6 controls the bypass enable */
    octeon_gpio_cfg_output(7); /* GPIO 7 controls the watchdog clear */
    octeon_gpio_cfg_input(5);  /* GPIO 5 tells you the bypass status */
    
    /* Startup with bypass disabled and watchdog cleared */
    octeon_gpio_clr(6);
    octeon_gpio_set(7);

    return 0;

}
