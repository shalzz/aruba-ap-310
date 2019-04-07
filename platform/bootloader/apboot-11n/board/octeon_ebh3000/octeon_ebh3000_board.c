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


#if defined(CONFIG_PCI)
extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
}
#endif


int octeon_ebh3000_get_board_major_rev(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return(gd->board_desc.rev_major);
}

int octeon_ebh3000_get_board_minor_rev(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    return(gd->board_desc.rev_minor);
}

int checkboard (void)
{
    DECLARE_GLOBAL_DATA_PTR;
    if (octeon_show_info())
    {

        int pal_rev_maj = 0;
        int pal_rev_min = 0;
        int voltage_100ths = 0;
        int voltage_1s = 0;
        int mcu_rev_maj = 0;
        int mcu_rev_min = 0;

        if (octeon_read64_byte(OCTEON_PAL_BASE_ADDR)==0xa5 && octeon_read64_byte(OCTEON_PAL_BASE_ADDR+1)==0x5a)
        {
            pal_rev_maj = octeon_read64_byte(OCTEON_PAL_BASE_ADDR+2);
            pal_rev_min = octeon_read64_byte(OCTEON_PAL_BASE_ADDR+3);
            if ((octeon_read64_byte(OCTEON_PAL_BASE_ADDR+4))>0xf)
            {
                voltage_1s = 0;
                voltage_100ths = (600+(31-octeon_read64_byte(OCTEON_PAL_BASE_ADDR+4))*25);
            }
            else
            {
                voltage_1s = 1;
                voltage_100ths = ((15-octeon_read64_byte(OCTEON_PAL_BASE_ADDR+4))*5);
            }
        }

        if (twsii_mcu_read(0x00)==0xa5 && twsii_mcu_read(0x01)==0x5a)
        {
            gd->mcu_rev_maj = mcu_rev_maj = twsii_mcu_read(2);
            gd->mcu_rev_min = mcu_rev_min = twsii_mcu_read(3);
        }

        printf("PAL rev: %d.%02d, MCU rev: %d.%02d, CPU voltage: %d.%02d\n",
               pal_rev_maj, pal_rev_min, mcu_rev_maj, mcu_rev_min, voltage_1s, voltage_100ths);

    }

    if (octeon_show_info())
    {
        /* Display CPU speed on display */

        if ((octeon_read64_byte(OCTEON_PAL_BASE_ADDR+4))>0xf)
        {
            char tmp[10];
            sprintf(tmp,"%d 0.%.2d ",gd->cpu_clock_mhz,(600+(31-octeon_read64_byte(OCTEON_PAL_BASE_ADDR+4))*25)/10);
            octeon_led_str_write(tmp);
        }
        else
        {
            char tmp[10];
            sprintf(tmp,"%d 1.%.2d ",gd->cpu_clock_mhz,(15-octeon_read64_byte(OCTEON_PAL_BASE_ADDR+4))*5);
            octeon_led_str_write(tmp);
        }
    }
    else
    {
        octeon_led_str_write("Boot    ");
    }

    return 0;
}



int early_board_init(void)
{
    int cpu_ref = 33;

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
        gd->board_desc.board_type = CVMX_BOARD_TYPE_EBH3000;
        gd->board_desc.rev_major = 1;
    }

    if (gd->board_desc.rev_major == 1)
    {
        if (gd->clock_desc.cpu_ref_clock_mhz_x_8)
        {
            cpu_ref = gd->clock_desc.cpu_ref_clock_mhz_x_8/8;
            gd->ddr_clock_mhz = gd->clock_desc.ddr_clock_mhz;
        }
        else
        {
            gd->flags |= GD_FLG_CLOCK_DESC_MISSING;
            /* Default values */
            cpu_ref = 50;
            gd->ddr_clock_mhz = EBH3000_REV1_DEF_DRAM_FREQ;
        }
    }
    else
    {
        /* Even though the CPU ref freq is stored in the clock descriptor, we don't read it here.  The MCU
        ** reads it and configures the clock, and we read how the clock is actually configured.
        ** The bootloader does not need to read the clock descriptor tuple for normal operation on
        ** ebh3000 boards
        */
        cpu_ref = octeon_mcu_read_cpu_ref();
        gd->ddr_clock_mhz = octeon_mcu_read_ddr_clock();
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
    return 0;

}
void octeon_led_str_write(const char *str)
{
    octeon_led_str_write_std(str);
}

