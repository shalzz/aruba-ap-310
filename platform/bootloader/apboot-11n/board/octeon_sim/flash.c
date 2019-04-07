/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

#include "octeon_boot.h"
#include "octeon_hal.h"
flash_info_t flash_info[CFG_MAX_FLASH_BANKS];	/* info for FLASH chips */

/*-----------------------------------------------------------------------
 * flash_init()
 *
 * sets up flash_info and returns size of FLASH (bytes)
 */
unsigned long flash_init (void)
{
    uint64_t flash_size = (256*1024*1024);
    cvmx_mio_boot_reg_cfgx_t reg_cfg;
    printf("Configuring boot bus for full 256meg access\n");
    reg_cfg.u64 = 0;
    reg_cfg.s.en = 1;
    reg_cfg.s.size = ((flash_size) >> 16) - 1;  /* In 64k blocks, + 4MByte alias of low 4Mbytes of flash */
    reg_cfg.s.base = ((0x10000000 >> 16) & 0x1fff);  /* Mask to put physical address in boot bus window */
    octeon_write64(CVMX_MIO_BOOT_REG_CFG0, reg_cfg.u64);
    return flash_size;
}

int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	printf ("write_buff not implemented\n");
	return (-1);
}
