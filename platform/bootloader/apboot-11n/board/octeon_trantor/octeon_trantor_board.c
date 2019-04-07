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


#if defined(CONFIG_PCI)
extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
}
#endif


void octeon_led_str_write(const char *str)
{
    return;
}

int checkboard (void)
{
    return 0;
}

int early_board_init(void)
{
    int cpu_ref;

    DECLARE_GLOBAL_DATA_PTR;

    /* NOTE: this is early in the init process, so the serial port is not yet configured */

    strncpy((char *)(gd->board_desc.serial_str), "unknown", SERIAL_LEN);
    /* Determine board type/rev */

    gd->flags = 0;
    gd->board_desc.board_type = CVMX_BOARD_TYPE_TRANTOR;
    gd->board_desc.rev_major = 2;       
    gd->board_desc.rev_minor = 0;       
    gd->board_desc.chip_type = CVMX_CHIP_TYPE_OCTEON_SAMPLE;       
    gd->board_desc.chip_rev_major = 1;  
    gd->board_desc.chip_rev_minor = 3;  

    gd->mac_desc.mac_addr_base[0] = 0x00;
    gd->mac_desc.mac_addr_base[1] = 0xba;
    gd->mac_desc.mac_addr_base[2] = 0xdd;
    gd->mac_desc.count = 4;


// HACK for Trantor
    /* Default values */
    cpu_ref = 33;
    gd->ddr_clock_mhz = 266;

    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;

    gd->cpu_clock_mhz = data * cpu_ref;

    if (gd->cpu_clock_mhz < 100 || gd->cpu_clock_mhz > 600)
    {
        gd->cpu_clock_mhz = DEFAULT_ECLK_FREQ_MHZ;
    }


    return 0;

}
