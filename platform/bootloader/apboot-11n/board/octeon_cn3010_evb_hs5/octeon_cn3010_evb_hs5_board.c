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
#include "octeon_hal.h"
#include <pci.h>
#include <lib_octeon_shared.h>
#include <lib_octeon.h>
#include <miiphy.h>
#ifdef CONFIG_HW_WATCHDOG
#include <watchdog.h>
#endif




typedef union {
  uint16_t  u16;
  struct {
    uint16_t
            busy  : 1,
            rsv   : 2,
            mode  : 1,
            op    : 2,
            device: 5,
            reg   : 5;
  } s;
} smi_cmd;

typedef union {
  uint16_t  u16;
  struct {
    uint16_t
            busy  : 1,
            op    : 3,
            mode  : 2,
            rsv   : 4,
            ptr   : 6;
  } s;
} marvel_status_cmd;

#if (CONFIG_COMMANDS & CFG_CMD_MII)
extern int octeon_miiphy_write(char *, unsigned char,
    unsigned char, unsigned short);
extern int  octeon_miiphy_read(char *, unsigned char, 
    unsigned char, unsigned short *);

#define mdio_write(x,y,z) octeon_miiphy_write("octeth0", x,y,z)
#define mdio_read(x,y,z) octeon_miiphy_read("octeth0", x,y,z)

/* In order to determine whether the switch is in single-chip or multi-chip mode, we attempt
 * to read the switch identifier register for port 9.
 *
 * This may wind up making some version of the EBH3100 boot up slowly, as it goes through a timeout cycle.
 */
static int marvell_sense_multi_chip_mode(void)
{
    int result;
    uint16_t value;

    result = mdio_read(0x19, 3, &value);
    return (value != 0x1a52);
}
#endif /* MII */

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

int checkboard (void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);
#endif
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


    /* Set up switch complex */   


    /* Force us into GMII mode, enable interface */
    /* NOTE:  Applications expect this to be set appropriately
    ** by the bootloader, and will configure the interface accordingly */
    cvmx_write_csr (CVMX_GMXX_INF_MODE (0), 0x3);

    /* Enable SMI to talk with the GMII switch */
    cvmx_write_csr (CVMX_SMI_EN, 0x1);
    cvmx_read_csr(CVMX_SMI_EN);



#if (CONFIG_COMMANDS & CFG_CMD_MII)

    uint16_t value;

    if (marvell_sense_multi_chip_mode())
    {
        printf("\n\nERROR: marvell switch in multi-chip mode!!!!!\n\n");

    }
    else
    {
        /* Force a reset of the switch */
        mdio_read  (0x1B, 4, &value);
        mdio_write (0x1B, 4, value | 0x8000);
        do
        {
            mdio_read  (0x1B, 4, &value);   
        } while (value & 0x8000);

        /* Force Port9 link speed and state. */
        mdio_write (0x19, 1, 0x003E);

        /* Turn off PPU. */
        mdio_read  (0x1B, 4, &value);
        mdio_write (0x1B, 4, value & ~0x4000);

        /* Clear MGMII mode */
        mdio_read  (0x10, 0x00, &value);
        mdio_write (0x10, 0x00, value & ~0x0040);

        mdio_read  (0x11, 0x00, &value);
        mdio_write (0x11, 0x00, value & ~0x0040);

        mdio_read  (0x12, 0x00, &value);
        mdio_write (0x12, 0x00, value & ~0x0040);

        mdio_read  (0x13, 0x00, &value);
        mdio_write (0x13, 0x00, value & ~0x0040);

        /* Clear all port statistics maintained by the switch. */
//        mdio_clear_statistics ();
        //device = 0x19;
        //for (reg = 0; reg < 20; reg= reg + 1) {
        //    if (reg >= 8 && reg <= 15) continue;
        //    mdio_read (device, reg, &value);
        //}

        /* Turn PPU back on. */
        mdio_read  (0x1B, 4, &value);
        mdio_write (0x1B, 4, value | 0x4000);
        cvmx_wait (0x40000);

    }



#endif  /* MII */

#ifdef CONFIG_HW_WATCHDOG
    hw_watchdog_init();
    WATCHDOG_RESET();
#endif

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
        gd->board_desc.board_type = CVMX_BOARD_TYPE_CN3010_EVB_HS5;
        gd->board_desc.rev_major = 1;
    }
#ifdef ARUBA_BOARD_TYPE
    strncpy((char *)(gd->board_desc.serial_str), ARUBA_BOARD_TYPE, SERIAL_LEN);
#endif


    /* Even though the CPU ref freq is stored in the clock descriptor, we don't read it here.  The MCU
    ** reads it and configures the clock, and we read how the clock is actually configured.
    ** The bootloader does not need to read the clock descriptor tuple for normal operation on
    ** ebh3100 boards
    */
    cpu_ref = CN5020_FORCED_DDR_AND_CPU_REF_HZ;;
    gd->ddr_clock_mhz = octeon_mcu_read_ddr_clock();


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

#ifdef CFG_LATE_BOARD_INIT
void
late_board_init(void)
{
#if 0
    DECLARE_GLOBAL_DATA_PTR;
    uint8_t mac[6];
#endif
#ifdef CONFIG_AR5416
    extern int ar5416_initialize(void);

    ar5416_initialize();
#endif

#ifdef notyet
    // this will initialize the switch, if it is present
    memcpy(mac, (uint8_t *)&(gd->mac_desc.mac_addr_base[0]), sizeof(mac));
    mac[5] += 1;
    m6063_announce(mac);
#endif
}
#endif
