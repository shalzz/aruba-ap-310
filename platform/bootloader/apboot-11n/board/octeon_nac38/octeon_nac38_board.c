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





#define VIA_IDE_NATIVE_MODE     1
#define VIA_PMIO_BASE_ADDR      0x480L


void via_ide_init(void)
{
   pci_dev_t dev;
   uint8_t value;
   uint32_t via_ide_port[3];

   dev=pci_find_device(0x1106, 0x0571, 0);	// find Via IDE
   if ( dev >= 0 )
   {
      //printf("IDE ");
      pci_write_config_word(dev, 0x04, 0x0087);	// Enable Bus Master I/O space,Memory space
      pci_write_config_byte(dev, 0x0d, 0x40);	// Latency Timer
      pci_read_config_byte(dev, 0x40, &value);
      pci_write_config_byte(dev, 0x40, value|0x02);// Enable Primary Channel
      pci_write_config_byte(dev, 0x41, 0xC2);	// Primary IDE Read Prefetch Buffer
      pci_write_config_byte(dev, 0x43, 0x05);	// FIFO configuration 1/2

      pci_write_config_byte(dev, 0x3c, 0x0e); 	// Interrupt Line
      pci_write_config_byte(dev, 0x44, 0x1c); 	// Miscellaneous Control
      pci_write_config_byte(dev, 0x45, 0x00); 	// Miscellaneous Control

      pci_write_config_byte(dev, 0x46, 0xc0); 	// Enable DMA FIFO flush  
      pci_write_config_dword(dev, 0x48, 0x2022a8a8);	// Drive Timing Control, PIO Mode 4(0x20)
      pci_write_config_byte(dev, 0x4c, 0xff);		// Address Setup Time
      pci_write_config_byte(dev, 0x54, 0x04);		// UltraDMA FIFO control


      pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &via_ide_port[0]); 
      pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &via_ide_port[1]); 
      pci_read_config_dword(dev, PCI_BASE_ADDRESS_1, &via_ide_port[2]);

      /* Mask of non-address bits at bottom of BAR registers */
      via_ide_port[0] = via_ide_port[0] & 0xfffffff8;
      via_ide_port[1] = via_ide_port[1] & 0xfffffff8;
      via_ide_port[2] = via_ide_port[2] & 0xfffffff8;
      int dev;
      dev = ide_register_device(IF_TYPE_IDE, BLOCK_QUIRK_IDE_VIA, 0, OCTEON_PCI_IOSPACE_BASE + via_ide_port[0], OCTEON_PCI_IOSPACE_BASE + via_ide_port[1], OCTEON_PCI_IOSPACE_BASE + via_ide_port[2]);
      dev = ide_register_device(IF_TYPE_IDE, BLOCK_QUIRK_IDE_VIA, 1, OCTEON_PCI_IOSPACE_BASE + via_ide_port[0], OCTEON_PCI_IOSPACE_BASE + via_ide_port[1], OCTEON_PCI_IOSPACE_BASE + via_ide_port[2]);

      //printf("(ideport 0x%x-0x%x, 0x%x, ", via_ide_port[0], via_ide_port[0]+0x08, via_ide_port[2]+6);
      
   }

}










extern void init_octeon_pci (void);

void pci_init_board (void)
{
	init_octeon_pci();
//        via_ide_init();
}
#endif

int late_board_init(void)
{
    return 0;
}
int checkboard (void)
{
    return 0;
}


int early_board_init(void)
{
    int cpu_ref = NAC38_REV1_DEF_REF_CLOCK_FREQ;
    int cpu_rev = octeon_get_proc_id() & 0xff;

    DECLARE_GLOBAL_DATA_PTR;

    memset((void *)&(gd->mac_desc), 0x0, sizeof(octeon_eeprom_mac_addr_t));
    memset((void *)&(gd->clock_desc), 0x0, sizeof(octeon_eeprom_clock_desc_t));
    memset((void *)&(gd->board_desc), 0x0, sizeof(octeon_eeprom_board_desc_t));

    /* NOTE: this is early in the init process, so the serial port is not yet configured */

    /* Populate global data from eeprom */
    uint8_t ee_buf[OCTEON_EEPROM_MAX_TUPLE_LENGTH];
    int addr;


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
        gd->board_desc.board_type = CVMX_BOARD_TYPE_NAC38;
        gd->board_desc.rev_major = 1;       
        gd->board_desc.rev_minor = 0;       
        gd->board_desc.chip_type = CVMX_CHIP_TYPE_OCTEON_SAMPLE;       
        gd->board_desc.chip_rev_major = 1 + (octeon_get_proc_id() & 0xff); /* 0 = pass1, 1 = pass2 */
        gd->board_desc.chip_rev_minor = (gd->board_desc.chip_rev_major == 0) ? 3 : 0;
    }

    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_CLOCK_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->clock_desc), ee_buf, sizeof(octeon_eeprom_clock_desc_t));
    }
    else
    {
        gd->flags |= GD_FLG_CLOCK_DESC_MISSING;
        /* Default values */
        cpu_ref = NAC38_REV1_DEF_REF_CLOCK_FREQ;
        gd->ddr_clock_mhz = NAC38_REV1_DEF_DRAM_FREQ;
    }

    /* Some sanity checks */
    if (cpu_ref <= 0)
    {
        cpu_ref = NAC38_REV1_DEF_REF_CLOCK_FREQ;
    }
    if (gd->ddr_clock_mhz <= 0)
    {
        gd->ddr_clock_mhz = NAC38_REV1_DEF_DRAM_FREQ;
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


    if (cpu_rev != 0)        /* Dynamically determine the DDR clock */
        gd->ddr_clock_mhz = 0;  /* DRAM init code will measure clock */


    /* Enable the LAN ports that are on the bypass relays */
    cvmx_write_csr(CVMX_GPIO_BIT_CFGX(5), 1);
    cvmx_write_csr(CVMX_GPIO_TX_SET, 0x20);
    return 0;

}

void octeon_led_str_write(const char *str)
{
    return;
}
