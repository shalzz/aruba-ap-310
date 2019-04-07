// vim:set ts=4 sw=4 expandtab:
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
#include <exports.h>
#include <linux/ctype.h>
#include <aruba_manuf.h>
#include <watchdog.h>

#define  CVMX_CIU_MBOX_CLRX(offset)                          (CVMX_ADD_IO_SEG(0x0001070000000680ull+((offset)*8)))


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

#define mdio_write(x,y,z) octeon_miiphy_write("en0", x,y,z)
#define mdio_read(x,y,z) octeon_miiphy_read("en0", x,y,z)

/* In order to determine whether the switch is in single-chip or multi-chip mode, we attempt
 * to read the switch identifier register for port 9.
 *
 * This may wind up making some version of the EBH3100 boot up slowly, as it goes through a timeout cycle.
 */
#ifndef CONFIG_PALOMINO
static int marvell_sense_multi_chip_mode(void)
{
    int result;
    uint16_t value;

    result = mdio_read(0x19, 3, &value);
    return (value != 0x1a52);
}
#endif
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

#if (CONFIG_COMMANDS & CFG_CMD_MII)
static void
palomino_phy_init(void)
{
        uint16_t page;
        int idx;

        //
        // initialize PHY LEDs per PRD; data provided by Thomas
        // All variants are supposed to have two ethernets now, so we 
        // now assume two PHYs
        //
        for (idx = 0; idx < 2; idx++) {
            mdio_read(idx, 0x16, &page);
            mdio_write(idx, 0x16, 0x3);
            mdio_write(idx, 0x10, 0x1107);
            mdio_write(idx, 0x16, page);
        }
}
#endif

int checkboard (void)
{
    extern void hw_watchdog_init(void);
#ifdef OCTEON_PAL_BASE_ADDR
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
#endif


    /* Set up switch complex */   


    /* Force us into RGMII mode, enable interface */
    /* NOTE:  Applications expect this to be set appropriately
    ** by the bootloader, and will configure the interface accordingly */
    cvmx_write_csr (CVMX_GMXX_INF_MODE (0), 0x2);

    /* Enable SMI to talk with the GMII switch */
    cvmx_write_csr (CVMX_SMI_EN, 0x1);
    cvmx_read_csr(CVMX_SMI_EN);

#if (CONFIG_COMMANDS & CFG_CMD_MII)
        palomino_phy_init();
#endif  // Palomino

    hw_watchdog_init();
    WATCHDOG_RESET();
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

#ifndef CONFIG_PALOMINO
    /* Populate global data from eeprom */
    uint8_t ee_buf[OCTEON_EEPROM_MAX_TUPLE_LENGTH];
    int addr;
#endif

#ifdef CONFIG_PALOMINO
    memset((void *)&(gd->clock_desc), 0, sizeof(gd->clock_desc));
    gd->clock_desc.ddr_clock_mhz = 266;
    gd->clock_desc.spi_clock_mhz = 0;  // we are using RGMII
#else
    addr = octeon_tlv_get_tuple_addr(CFG_DEF_EEPROM_ADDR, EEPROM_CLOCK_DESC_TYPE, 0, ee_buf, OCTEON_EEPROM_MAX_TUPLE_LENGTH);
    if (addr >= 0)
    {
        memcpy((void *)&(gd->clock_desc), ee_buf, sizeof(octeon_eeprom_clock_desc_t));
    }
#endif

#ifdef CONFIG_PALOMINO
    gd->flags |= GD_FLG_BOARD_DESC_MISSING;
    gd->board_desc.rev_minor = 0;
    gd->board_desc.board_type = CVMX_BOARD_TYPE_CN3010_EVB_HS5;
    gd->board_desc.rev_major = 1;
    strncpy((char *)(gd->board_desc.serial_str), ARUBA_BOARD_TYPE, SERIAL_LEN);
#else
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
#endif

#ifdef CONFIG_PALOMINO
    // ddr_clock_mhz was set above
    cpu_ref = 33;
    if (__octeon_is_model_runtime__(OCTEON_CN50XX)) {
//        gd->clock_desc.ddr_clock_mhz = 0;   // will be computed later
        cpu_ref = CN5020_FORCED_DDR_AND_CPU_REF_HZ;
    }
#else
    /* Even though the CPU ref freq is stored in the clock descriptor, we don't read it here.  The MCU
    ** reads it and configures the clock, and we read how the clock is actually configured.
    ** The bootloader does not need to read the clock descriptor tuple for normal operation on
    ** ebh3100 boards
    */
    cpu_ref = octeon_mcu_read_cpu_ref();
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
#endif
//XXXSCAXXX
#ifdef CONFIG_PALOMINO
    nvram_manuf_t *manuf = (nvram_manuf_t *)CFG_MANUF_BASE;
    unsigned char checksum = manuf_compute_checksum(manuf);
    if (checksum == manuf->checksum) {
        ushort *u = (ushort *)&manuf->network_address_count[0];
        int i;

        gd->mac_desc.count = *u;
        for (i = 0; i < 6; i++) {
            gd->mac_desc.mac_addr_base[i] = manuf->network_address[i];
        }
        gd->mac_desc.count = 2;
    } else {
        /* Make up some MAC addresses */
        gd->mac_desc.count = 2;
        gd->mac_desc.mac_addr_base[0] = 0x00;
        gd->mac_desc.mac_addr_base[1] = 0xDE;
        gd->mac_desc.mac_addr_base[2] = 0xAD;
        gd->mac_desc.mac_addr_base[3] = (gd->board_desc.rev_major<<4) | gd->board_desc.rev_minor;
        gd->mac_desc.mac_addr_base[4] = gd->board_desc.serial_str[0];
        gd->mac_desc.mac_addr_base[5] = 0x00;
    }
#else
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
#endif


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

    // adjust clock to 400 if this is the single-radio 5010-based variant
    if (__octeon_is_model_runtime__(OCTEON_CN50XX)
        && (octeon_gpio_value(GPIO_NOT_3005) == 0)) {
        gd->cpu_clock_mhz = 400;
    }

    if (gd->cpu_clock_mhz == 499) {
        gd->cpu_clock_mhz = 500;
    } else if (gd->cpu_clock_mhz == 399) {
        gd->cpu_clock_mhz = 400;
    }

    // HW sect, 7.6
    octeon_gpio_cfg_output(GPIO_RED_STATUS_LED); /* red status LED */
    octeon_gpio_cfg_output(GPIO_GREEN_STATUS_LED); /* green status LED */
    octeon_gpio_cfg_output(GPIO_PHY_0_RESET); /* PHY 0 reset */
    octeon_gpio_cfg_output(GPIO_PHY_1_RESET); /* PHY 1 reset */
    octeon_gpio_cfg_output(GPIO_RADIO_0_RESET); /* a/b/g/n reset */
    octeon_gpio_cfg_output(GPIO_RADIO_1_RESET); /* b/g/n reset */

    // put radios and PHYs in reset for a brief time
    octeon_gpio_clr(GPIO_PHY_0_RESET);
    octeon_gpio_clr(GPIO_PHY_1_RESET);
    octeon_gpio_clr(GPIO_RADIO_0_RESET);
    octeon_gpio_clr(GPIO_RADIO_1_RESET);

    udelay(30 * 1000);

    // take PHYs out of reset
    octeon_gpio_set(GPIO_PHY_0_RESET);
    octeon_gpio_set(GPIO_PHY_1_RESET);

    //
    // release radios
    //
    octeon_gpio_set(GPIO_RADIO_0_RESET);
    octeon_gpio_set(GPIO_RADIO_1_RESET);

    // turn status led to green
    octeon_gpio_set(GPIO_RED_STATUS_LED);

    return 0;

}

void octeon_led_str_write(const char *str)
{
    octeon_led_str_write_std(str);
}

#if (CONFIG_COMMANDS & CFG_CMD_I2C)
int cli_i2c_access(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint8_t val;
    int len = 1, dev;
    int i, ret;
    uint32_t addr;

    addr = 0;        
    if (argv[1][0] == 'r') {
        if (argc < 4) {    
            printf ("Usage: i2c <r or w> <dev> <addr> <data> <len>\n"); 
            return 0;
        }
        if (argc == 5) { 
            len = simple_strtoul(argv[4],NULL,0);
        } else {
            len = 1;
        }
        addr = simple_strtoul(argv[3],NULL,0);
        dev = simple_strtoul(argv[2],NULL,0); // DEVID
        for (i = 0 ; i< len; i ++) { 
            ret = i2c_read(dev, addr, 1, &val, 1);
            if(ret == 1) {
                printf("i2c ERROR: No ACK received\n");
                break;
            }
            printf ("0x%x 0x%x \n",addr,val);
            addr++;
        }
    } else if (argv[1][0] == 'w') {
        if (argc < 5) {    
            printf ("Usage: i2c <r or w> <dev> <addr> <data> <len>\n"); 
            return 0;
        }
        dev = simple_strtoul(argv[2],NULL,0);
        addr = simple_strtoul(argv[3],NULL,0);
        val = simple_strtoul(argv[4],NULL,0);
        for (i = 0 ; i< len; i ++) {             
            ret = i2c_write(dev, addr, 1, &val, 1);
            if(ret == 1) {
                printf("i2c ERROR: No ACK received\n");
                break;
            }
            addr++;
        }
    } else {
        printf ("Usage: i2c <r or w> <dev> <addr> <data> <len>\n"); 
    }
    return 0;
}

U_BOOT_CMD(
	i2c,    7,    1,     cli_i2c_access,
	"i2c    - generic i2c access\n",                   
	" Usage: i2c <r or w> <devid> <addr> <data> <len>\n"
);

int cli_clock_drvr(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint8_t val;
    int len = 1, dev;
    int i, ret;
    uint32_t addr;

    addr = 0;        
    if (argv[1][0] == 'r') {
        if (argc < 3) {    
            printf ("Usage: cdce w <addr>\n"); 
            return 0;
        }
        len = 1;
        addr = simple_strtoul(argv[2],NULL,0);
        addr |= 0x80; // byte read/write
        dev = 0x69; // DEVID
        for (i = 0 ; i< len; i ++) { 
            ret = i2c_read(dev, addr, 1, &val, 1);
            if(ret == 1) {
                printf("i2c ERROR: No ACK received\n");
                break;
            }
            printf ("0x%x 0x%x \n",addr,val);
            addr++;
        }
    } else if (argv[1][0] == 'w') {
        if (argc < 4) {    
            printf ("Usage: cdce w <addr> <data>\n"); 
            return 0;
        }
        dev = 0x69;
        addr = simple_strtoul(argv[2],NULL,0);
        addr |= 0x80; // byte read/write
        val = simple_strtoul(argv[3],NULL,0);
        for (i = 0 ; i< len; i ++) {             
            ret = i2c_write(dev, addr, 1, &val, 1);
            if(ret == 1) {
                printf("i2c ERROR: No ACK received\n");
                break;
            }
            addr++;
        }
    } else {
        printf ("Usage: cdce r <addr>\n"); 
        printf ("     : cdce w <addr> <data>\n"); 
    }
    return 0;
}

U_BOOT_CMD(
	cdce,    6,    1,     cli_clock_drvr,
	"cdce    - access CDCE906 clock driver i2c registers\n",                   
	" Usage: cdce w <addr> <data>\n"
	" Usage: cdce r <addr>\n"
);
#endif

int __pal_on_dc;
int __pal_voltage_sense_0;
int __pal_voltage_sense_1;
int __pal_radio_board;
int __pal_digital_board;

void
late_board_init(void)
{
#ifdef CONFIG_AR5416
    extern int ar5416_initialize(void);
#endif

    // figure out what we have
    __pal_on_dc = octeon_gpio_value(GPIO_POE_ACTIVE);
    // range is 45-48 on dual-radio, 41-44 on single-radio
    __pal_voltage_sense_0 = !octeon_gpio_value(GPIO_VSENSE_0);
    __pal_voltage_sense_1 = !octeon_gpio_value(GPIO_VSENSE_1);

    __pal_radio_board = octeon_gpio_value(GPIO_CARD_ID_2)  
                        | octeon_gpio_value(GPIO_CARD_ID_1) 
                        | octeon_gpio_value(GPIO_CARD_ID_0);
    __pal_digital_board = octeon_gpio_value(GPIO_NOT_3005);

#if 0
printf("6=%u, 7=%u, 12=%u, 13=%u, 14=%u, 15=%u, 16=%u\n", 
octeon_gpio_value(GPIO_VSENSE_0),
octeon_gpio_value(GPIO_VSENSE_1),
octeon_gpio_value(GPIO_POE_ACTIVE),
octeon_gpio_value(GPIO_CARD_ID_0),
octeon_gpio_value(GPIO_CARD_ID_1),
octeon_gpio_value(GPIO_CARD_ID_2),
octeon_gpio_value(GPIO_NOT_3005));

printf("DC %x, V0 %x, V1 %x, RB %x, DB %x\n",
__pal_on_dc,
__pal_voltage_sense_0,
__pal_voltage_sense_1,
__pal_radio_board,
__pal_digital_board);
#endif
#ifdef CONFIG_AR5416
    ar5416_initialize();

    //
    // drive radio GPIO outputs; 
    // If single-radio board, drive 2/3 as outputs on
    // 5ghz radio; see register 0x404c on the 5416.
    // See section "AR5416 GPIO" of the functional specification.
    //
    if (__pal_radio_board == 0) {
        // dual-concurrent
        cvmx_write64_uint32(0x90011b008000404cULL, __cpu_to_le32(0xf));    // 0/1
        cvmx_write64_uint32(0x90011b008001404cULL, __cpu_to_le32(0xf0));   // 2/3

        // turn the LEDs off for now (register 0x4048 on the 5416)
        cvmx_write64_uint32(0x90011b0080004048ULL, __cpu_to_le32(0x0)); // 5ghz off
        cvmx_write64_uint32(0x90011b0080004060ULL, __cpu_to_le32(0x0)); // GPIO output
        cvmx_write64_uint32(0x90011b0080014048ULL, __cpu_to_le32(0x0)); // 2.4ghz off
        cvmx_write64_uint32(0x90011b0080014060ULL, __cpu_to_le32(0x0)); // GPIO output
    } else if (__pal_radio_board == 1) {
        // single radio
        cvmx_write64_uint32(0x90011b008000404cULL, __cpu_to_le32(0xff));    // 0-3

        // turn the LEDs off for now (register 0x4048 on the 5416)
        cvmx_write64_uint32(0x90011b0080004048ULL, __cpu_to_le32(0x0));
        cvmx_write64_uint32(0x90011b0080004060ULL, __cpu_to_le32(0x0)); // GPIO output
    } else {
        printf("Unknown radio board ID: %u; LEDs not initialized\n", 
            __pal_radio_board);
    }
#endif
}

DECLARE_GLOBAL_DATA_PTR;
extern volatile uchar	PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
int octeon_eth_send(struct eth_device *dev, volatile void *packet, int length);
int octeon_etest_recv(struct eth_device *dev,  uchar *pkt); /* Check for received packets	*/

#ifndef CONFIG_STAGE1
int do_traffic_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	struct eth_device* en_xmit = eth_get_dev_by_name("en0");
	struct eth_device* en_recv = eth_get_dev_by_name("en1");
	bd_t *bd = gd->bd;
	static uchar mac[6];
	volatile IP_t *ip;
	volatile ushort *s;
	volatile uchar *pkt;
    int     i, pkt_xmit = 0, pkt_recv = 0, tst_seq_num = 0;
    int     pkt_cnt = 1, rec_len;
    int     run_forever = 0, direction = 0;
    ulong   tst_src_ip, tst_dst_ip, timeout;
    uchar   rec_buf[2048];
    int c;
        
    optind = 1;
    optreset = 1;
    while ((c = getopt (argc, argv, "d:fn:hH")) != -1)
    {
        switch (c)
        {
        case 'h':
        case 'H':
            //mtest_usage();
            return 0;
            
        case 'd':
            direction = simple_strtoul(optarg,NULL,0);
            if(direction) {
                printf("Test: en1->en0\n");
                en_xmit = eth_get_dev_by_name("en1");
                en_recv = eth_get_dev_by_name("en0");
            } else {
                printf("Test: en0->en1\n");
                en_xmit = eth_get_dev_by_name("en0");
                en_recv = eth_get_dev_by_name("en1");
            }
            break;
 
        case 'f':
            run_forever = 1;
            break;
            
        case 'n':
            pkt_cnt = simple_strtoul(optarg,NULL,0);
            break;
 
        case '?':
            if (isprint (optopt))
              fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
              fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;

        default:
            break;
        }
    }
    
    //index = argc - optind + 1;

    tst_src_ip = 0x11223344;
    tst_dst_ip = 0x69696969;
    // set up the packet buffers
	NetTxPacket = NULL;
	if (!NetTxPacket) {
		/*
		 *	Setup packet buffers, aligned correctly.
		 */
		NetTxPacket = &PktBuf[0] + (PKTALIGN - 1);
		NetTxPacket -= (ulong)NetTxPacket % PKTALIGN;
		for (i = 0; i < PKTBUFSRX; i++) {
			NetRxPackets[i] = NetTxPacket + (i+1)*PKTSIZE_ALIGN;
		}
	}
    
    // init both en0 and en1
    if (en_xmit->init(en_xmit, bd)) {
		en_xmit->state = ETH_STATE_ACTIVE;
	} else {
        printf("en0 failed to initialize\n");
    }
    if (en_recv->init(en_recv, bd)) {
		en_recv->state = ETH_STATE_ACTIVE;
	} else {
        printf("en1 failed to initialize\n");
    }
    
    // for a start, send on 0 - recv on 1
	memcpy(mac, NetEtherNullAddr, 6);
	pkt = NetTxPacket;
	pkt += NetSetEther(pkt, mac, PROT_IP);

	ip = (volatile IP_t *)pkt;

	/*
	 *	Construct an IP and ICMP header.  (need to set no fragment bit - XXX)
	 */
	ip->ip_hl_v  = 0x45;		/* IP_HDR_SIZE / 4 (not including UDP) */
	ip->ip_tos   = 0;
	ip->ip_len   = htons(IP_HDR_SIZE_NO_UDP + 8);
	ip->ip_id    = htons(NetIPID++);
	ip->ip_off   = htons(0x4000);	/* No fragmentation */
	ip->ip_ttl   = 255;
	ip->ip_p     = 0x01;		/* ICMP */
	ip->ip_sum   = 0;
	NetCopyIP((void*)&ip->ip_src, &tst_src_ip); /* already in network byte order */
	NetCopyIP((void*)&ip->ip_dst, &tst_dst_ip);	   /* - "" - */
	ip->ip_sum   = ~NetCksum((uchar *)ip, IP_HDR_SIZE_NO_UDP / 2);

	s = &ip->udp_src;              /* XXX ICMP starts here */
	s[0] = htons(0x0800);          /* echo-request, code   */
	s[1] = 0;                      /* checksum             */
	s[2] = 0;                      /* identifier           */
	s[3] = htons(tst_seq_num++);   /* sequence number      */
	s[1] = ~NetCksum((uchar *)s, 8/2);
         
    i = 0;
    while((pkt_xmit < pkt_cnt) || (run_forever)) {
        octeon_eth_send(en_xmit, pkt, 512);
        pkt_xmit++;
        timeout = 0x100;
        do {
            if ((rec_len = octeon_etest_recv(en_recv, rec_buf)) != 0) {
                //printf("got one!\n");
                if(memcmp((void*) pkt, rec_buf, 512) != 0) {
                    printf("Packet data miscompares...\n");
                }
                pkt_recv++;
            }
            WATCHDOG_RESET();
		    if (ctrlc()) {
			    puts ("\nAbort\n");
                run_forever = 0; pkt_cnt = 0;
			    break;
		    }
        } while((rec_len == 0) && timeout--);
        if(timeout == 0) printf("Timeout waiting for packet!\n");
		if (ctrlc()) {
			puts ("\nAbort\n");
            run_forever = 0; pkt_cnt = 0;
			break;
		}
        WATCHDOG_RESET();
    }
    
    if(pkt_xmit > pkt_recv) {
        timeout = 0x100;
        do {
            if ((rec_len = octeon_etest_recv(en_recv, rec_buf)) != 0) {
                //printf("got one!\n");
                if(memcmp((void*) pkt, rec_buf, 512) != 0) {
                    printf("Packet data miscompares...\n");
                }
                pkt_recv++;
            }
            WATCHDOG_RESET();
		    if (ctrlc()) {
			    puts ("\nAbort\n");
                run_forever = 0; pkt_cnt = 0;
			    break;
		    }
        } while((rec_len == 0) && timeout--);
        if(timeout == 0) printf("Timeout waiting for packet!\n");
    }
    
    printf("Sent %d packets, Received %d packets\n",pkt_xmit,pkt_recv);
    
    return 0;
}

U_BOOT_CMD(
	tt,    7,    1,     do_traffic_test,
	"tt    - eth0 to eth1 traffic test\n",                   
	" Usage: tt -[dfn] \n"
    "           -d <0|1> \"direction\" from 0 to 1 or from 1 to 0\n"
    "           -f run \"forever\", ctrl-C to exit\n"
    "           -n <num> Send <num> test packets\n"
);
#endif

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int base_vin = 45;
    int max_vin = 48;

    if (__pal_radio_board) {
        base_vin = 41;
        max_vin = 44;
    }
    printf("Power: %s [voltage sense %u/%u/%u]\n", __pal_on_dc ? "DC" : "POE",
        __pal_voltage_sense_0, __pal_voltage_sense_1, __pal_on_dc);
    if (!__pal_on_dc) {
        // 1 == high
        if (__pal_voltage_sense_1) {
            printf("Voltage: > %uV\n", max_vin);
        } else if (__pal_voltage_sense_0) {
            printf("Voltage: %uV-%uV\n", base_vin, max_vin);
        } else {
            printf("Voltage: < %uV\n", base_vin);
        }
    }
    printf("Digital ID: %s\n", __pal_digital_board ? "124/5" : "120/1");
    switch (__pal_radio_board) {
    case 0:
        printf("Radio ID: Fino-Dual\n");
        break;
    case 1:
        printf("Radio ID: Fino-Single\n");
        break;
    default:
        printf("Unknown ID: %u\n", __pal_radio_board);
        break;
    }

    printf("Thermal Interrupt: %s\n", octeon_gpio_value(GPIO_THERMAL_INT) ? "inactive" : "pending");
    printf("TPS Interrupt: %s\n", octeon_gpio_value(GPIO_TPS_INT) ? "inactive" : "pending");
    return 0;
}

U_BOOT_CMD(
	gpio_pins,    1,    1,     do_gpio_pins,
	"gpio_pins    - show GPIO pin status\n",                   
	" Usage: gpio_pins \n"
);




