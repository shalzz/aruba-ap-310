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


int checkboard (void)
{

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
	env_str = getenv("serial#");
	if (env_str)
	{
	    strncpy((char *)(gd->board_desc.serial_str), env_str, SERIAL_LEN);
	}

	gd->flags |= GD_FLG_BOARD_DESC_MISSING;
	gd->board_desc.board_type = CVMX_BOARD_TYPE_KBP;
	gd->board_desc.rev_major = 1;
	gd->board_desc.rev_minor = 0;
    }

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
	    gd->mac_desc.mac_addr_base[0] = 0x00;  /* Special value allows silent overrides */
	    gd->mac_desc.mac_addr_base[1] = 0xba;
	    gd->mac_desc.mac_addr_base[2] = 0xdd;
	    gd->mac_desc.mac_addr_base[3] = (gd->board_desc.rev_major<<4) | gd->board_desc.rev_minor;
	    gd->mac_desc.mac_addr_base[4] = gd->board_desc.serial_str[0];
	}
	gd->mac_desc.count = 32;  // Could be SPI
    }

    /* Default values */
    cpu_ref = gd->clock_desc.cpu_ref_clock_mhz_x_8 / 8;

    if (cpu_ref <= 20 || cpu_ref > 60) {
	cpu_ref = 50;
    }

    if ((octeon_get_proc_id() == OCTEON_PASS1))
        gd->ddr_clock_mhz = 200;   /* All boards with pass 1 are 200 Mhz */
    else
        gd->ddr_clock_mhz = OCTEON_DDR_CLOCK_MHZ;

    if (gd->ddr_clock_mhz <= 100) {
	gd->ddr_clock_mhz = 200;
    }

    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;

    gd->cpu_clock_mhz = data * cpu_ref;

    return 0;
}

int late_board_init(void) {

    return(0);
}

void octeon_led_str_write(const char *str)
{
//    octeon_led_str_write_std(str);
// No diag display
}
