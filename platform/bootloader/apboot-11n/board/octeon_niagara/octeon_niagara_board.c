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



int checkboard (void)
{
    return 0;
}

int early_board_init(void)
{
    DECLARE_GLOBAL_DATA_PTR;

    gd->board_desc.board_type = CVMX_BOARD_TYPE_NIAGARA;
    gd->board_desc.rev_major = 1;       
    gd->board_desc.rev_minor = 0;       
    gd->board_desc.chip_type = CVMX_CHIP_TYPE_OCTEON_SAMPLE;       
    gd->board_desc.chip_rev_major = 1;  
    gd->board_desc.chip_rev_minor = 3;  

    /* Default values */
    gd->ddr_clock_mhz = 266;  /* set to 266 even for 200 Mhz clock */

    /* Read CPU clock multiplier */
    uint64_t data = octeon_read64((uint64_t)0x80011F00000001E8ull);
    data = data >> 18;
    data &= 0x1f;


    gd->cpu_clock_mhz = data * 33.33;
    return 0;
}

void octeon_led_str_write(const char *str)
{
    return;
}
