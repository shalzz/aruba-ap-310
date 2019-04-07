/* vim: set ts=4 sw=4 expandtab: */
/*
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <hwconfig.h>
#include <pci.h>
#include <i2c.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/cache.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_pci.h>
#include <asm/fsl_ddr_sdram.h>
#include <asm/io.h>
#include <asm/fsl_law.h>
#include <asm/fsl_lbc.h>
#include <asm/mp.h>
#include <miiphy.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <tsec.h>
#include <ioports.h>
#include <exports.h>
#include <asm/fsl_serdes.h>
//#include <netdev.h>
#include <watchdog.h>
#include <linux/ctype.h>
#include <usb/ehci-fsl.h>

extern unsigned gpio_value(unsigned);
extern void gpio_set_as_input(unsigned);
extern void gpio_set_as_output(unsigned);
extern void gpio_out(unsigned, unsigned);

void tomatin_power_down_usb(void)
{
    struct usb_ehci *ehci;
    ehci = (struct usb_ehci *)CONFIG_SYS_FSL_USB_ADDR;

    /* Enable interface. */
    setbits_be32(&ehci->control, USB_EN);

    /*
     * disable driving VBUS via the ULPI viewport register
     * - write to PHY register 0xa and clear 0x40 and 0x20; see
     *   the 3300 data sheet for details
     */
    out_le32(&ehci->ulpi_viewpoint, 0x600a0007);

    /* Disable interface. */
    clrbits_be32(&ehci->control, USB_EN);
}

void board_gpio_init(void)
{

    volatile ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
	ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR); // wnc, 20140129_M_Ellis

	setbits_be32(&gur->pmuxcr, MPC85xx_PMUXCR_SPI_GPIO); // wnc, 20140129_M_Ellis: Default signal is SPI(00), it needs to change(10) for GPIO[6:9] pins (pcie reset are 6 and 7)
	setbits_be32(&gur->pmuxcr2, MPC85xx_PMUXCR2_UART_GPIO); // wnc, 20140129_M_Ellis: Default signal is UART(00), it needs to change(01) for GPIO[10:11] pins (reset button is 11)

    gpio_set_as_output(GPIO_RED_STATUS_LED);
    /* turn off red LED */
    gpio_out(GPIO_RED_STATUS_LED, 1);

    gpio_set_as_output(GPIO_GREEN_STATUS_LED);
    gpio_set_as_output(GPIO_PCI_RST_2G);
    gpio_set_as_output(GPIO_PCI_RST_5G);
    gpio_set_as_output(GPIO_USB_PWR);
    gpio_out(GPIO_USB_PWR, 1);

    gpio_set_as_input(GPIO_POWER_DC);
    gpio_set_as_input(GPIO_POWER_AT);
    gpio_set_as_input(GPIO_POWER_POE_PLUS);
    gpio_set_as_input(GPIO_CONFIG_CLEAR);

    out_be32(&pgpio->gpier, 0xffff0000); /* clear all pending GPIO interrupts */
    out_be32(&pgpio->gpimr, 0x00000000); /* disable all GPIO interrupts */
    out_be32(&pgpio->gpicr, 0xffff0000); /* detect only on a high->low change */
}

int board_early_init_f(void)
{
    ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

    /* disable SDHC */
    clrbits_be32(&gur->pmuxcr,
                    (MPC85xx_PMUXCR_SDHC_CD | MPC85xx_PMUXCR_SDHC_WP));
    clrbits_be32(&gur->sdhcdcr, SDHCDCR_CD_INV);

    /* disable SD */
    clrbits_be32(&gur->pmuxcr, MPC85xx_PMUXCR_SD_DATA);

    /* disable TDM */
    clrbits_be32(&gur->pmuxcr, MPC85xx_PMUXCR_TDM_ENA);

    board_gpio_init();

    return 0;
}

#define __FSL_BIT(n) ((31 - n))

/* disable the small number of blocks that we are not using */
static void
tomatin_disable_unused_blocks(void)
{
    ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
    unsigned val;

    val = 0;

    val |= (1 << __FSL_BIT(3));   /* TDM */
    val |= (1 << __FSL_BIT(9));   /* USB2 */
    val |= (1 << __FSL_BIT(10));  /* SDHC */
    val |= (1 << __FSL_BIT(28));  /* SPI */

    setbits_be32(&gur->devdisr, val);
}

void tomatin_phy_init(void)
{
	unsigned short reg;

	/* Change Page Number */
	miiphy_write("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_PAGE, 0x0002);
	/* Delay RGMII TX and RX */
	miiphy_read("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_MAC_SPEC_CONTROL2, &reg);
	reg |= (1 << MIIM_88E1510_RGMII_RX_TIMING_CONTROL_MASK) |
			(1 << MIIM_88E1510_RGMII_TX_TIMING_CONTROL_MASK);
	miiphy_write("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_MAC_SPEC_CONTROL2, reg);

	/* Change Page Number */
	miiphy_write("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_PAGE, 0x0003);
	/* Adjust LED control */
	/* Polarity LED0: On High, Off Low. Polarity LED1: On High, Off Low. */
	miiphy_write("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_LED_POLARITY_CONTROL, 0x4435);
	/* LED0: 1000Mbps Link, Else Off. LED1: Link Blink, Else Off. LED2: Don't Care. */
	miiphy_write("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_LED_FUNCTION_CONTROL, 0x0c17);
	/* Change Page Number */
	miiphy_write("eth0", MDIO_DEVAD_NONE, MIIM_88E1510_PHY_PAGE, 0x0000);
}

#if defined(CONFIG_RESET_BUTTON)
static void
flash_status_led(void)
{
    DECLARE_GLOBAL_DATA_PTR;
    int i;

    for (i = 0; i < 6; i++) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
#ifdef CONFIG_HW_WATCHDOG
        gd->watchdog_blink = 1;
#endif
        udelay(100 * 1000);
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
#ifdef CONFIG_HW_WATCHDOG
        gd->watchdog_blink = 0;
#endif
        WATCHDOG_RESET();
        udelay(100 * 1000);
    }
}

static void
check_reset_button(void)
{
    volatile ccsr_duart_t *pduart = (void *)(CONFIG_SYS_MPC85xx_DUART_ADDR);
    char *reset_env_var = 0;

    if (!gpio_value(GPIO_CONFIG_CLEAR)) {
        /* unleash hell */
#if 0 /* XXX; this doesn't work pre relocation */
        reset_env_var = getenv("disable_reset_button");
#endif
        if (reset_env_var && (!strcmp(reset_env_var, "yes")))  {
            pduart->uscr1 = 'D';
            pduart->uscr2 = 'D';
        } else {
            pduart->uscr1 = 'R';
            pduart->uscr2 = 'B';
            flash_status_led();
        }
    } else {
        /* no config clear */
        pduart->uscr1 = 'N';
        pduart->uscr2 = 'R';
    }
}

static void
execute_config_clear(void)
{
    extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
    char *av[3];
    char *reset_env_var = 0;
    volatile ccsr_duart_t *pduart = (void *)(CONFIG_SYS_MPC85xx_DUART_ADDR);

    WATCHDOG_RESET();

    if (pduart->uscr1 == 'N' && pduart->uscr2 == 'R') {
        return;
    } else if (pduart->uscr1 == 'D' && pduart->uscr2 == 'D') {
        printf("**** Reset Request Disabled by Environment ****\n");
    } else if (pduart->uscr1 != 'R' || pduart->uscr2 != 'B') {
        printf("**** Unrecognized Reset State (0x%02x/0x%02x) -- Ignoring ****\n",
            pduart->uscr1, pduart->uscr2);
        return;
    }

    reset_env_var = getenv("disable_reset_button");
    if (reset_env_var && !strcmp(reset_env_var, "yes")) {
        printf("**** Reset Request Disabled by Environment ****\n");
        return;
    }

    printf("**** Configuration Reset Requested by User ****\n");

    av[0] = "factory_reset";
    av[1] = 0;
    do_factory_reset(0, 0, 1, av);
    flash_status_led();
}
#endif

int checkboard(void)
{
    extern void hw_watchdog_init(void);

    tomatin_power_down_usb();

#ifdef CONFIG_PHYS_64BIT
    puts("(36-bit addrmap) ");
#endif

    udelay(10 * 1000);

    hw_watchdog_init();
    WATCHDOG_RESET();

#ifdef CONFIG_RESET_BUTTON
    check_reset_button();
#endif

    return 0;
}

#ifdef CONFIG_PCI
void pci_init_board(void)
{
    fsl_pcie_init_board(0);
}
#endif

int board_early_init_r(void)
{
    const unsigned int flashbase = CONFIG_SYS_FLASH_BASE;
    const u8 flash_esel = find_tlb_idx((void *)flashbase, 1);

    /*
     * Remap Boot flash region to caching-inhibited
     * so that flash can be erased properly.
     */

    /* Flush d-cache and invalidate i-cache of any FLASH data */
    flush_dcache();
    invalidate_icache();

    /* invalidate existing TLB entry for flash */
    disable_tlb(flash_esel);

    set_tlb(1, flashbase, CONFIG_SYS_FLASH_BASE_PHYS, /* tlb, epn, rpn */
        MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,/* perms, wimge */
        0, flash_esel, BOOKE_PAGESZ_64M, 1);/* ts, esel, tsize, iprot */

    /* reset radios */
    gpio_out(GPIO_PCI_RST_2G, 0);
    gpio_out(GPIO_PCI_RST_5G, 0);
    udelay(80 * 1000);
    gpio_out(GPIO_PCI_RST_2G, 1);
    gpio_out(GPIO_PCI_RST_5G, 1);
    
    return 0;
}

int board_eth_init(bd_t *bis)
{
    struct tsec_info_struct tsec_info[4];
    ccsr_gur_t *gur __attribute__((unused)) =
            (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);
    int num = 0;

#ifdef CONFIG_TSEC1
    SET_STD_TSEC_INFO(tsec_info[num], 1);
    num++;
#endif
#ifdef CONFIG_TSEC2
    SET_STD_TSEC_INFO(tsec_info[num], 2);
    if (is_serdes_configured(SGMII_TSEC2)) {
//                printf("eTSEC2 is in sgmii mode.\n");
            tsec_info[num].flags |= TSEC_SGMII;
    }
    num++;
#endif
#ifdef CONFIG_TSEC3
    SET_STD_TSEC_INFO(tsec_info[num], 3);
    num++;
#endif

    if (!num) {
        printf("No TSECs initialized\n");
        return 0;
    }

    tsec_eth_init(bis, tsec_info, num);

    return 0; //pci_eth_init(bis);
}

#ifdef CONFIG_OF_BOARD_SETUP
void ft_board_setup(void *blob, bd_t *bd)
{
    phys_addr_t base;
    phys_size_t size;

    ft_cpu_setup(blob, bd);

    base = getenv_bootm_low();
    size = getenv_bootm_size();

    fdt_fixup_memory(blob, (u64)base, (u64)size);

    FT_FSL_PCI_SETUP;

    fdt_fixup_dr_usb(blob, bd);
}
#endif

#ifdef CFG_LATE_BOARD_INIT
int n_radios = 0;

static void
tomatin_radio_callback(int n)
{
   n_radios = n;
}

void
late_board_init(void)
{
    extern void ar5416_install_callback(void *);
    extern int ar5416_initialize(void);

#if (CONFIG_COMMANDS & CFG_CMD_MII)
    /*
     * we need the network devices attached for this to work, so we can't
     * do it in checkboard()
     */
    tomatin_phy_init();
#endif
    ar5416_install_callback(tomatin_radio_callback);
    ar5416_initialize();

#ifdef CONFIG_RESET_BUTTON
    execute_config_clear();
#endif

    tomatin_disable_unused_blocks();
}
#endif

int allow_usb;

/*
 * From Thomas:
 * POE_AT_DETECT_L         DC_POWER        Description
 * -------------------------------------------------------
 * High                    Low             Powered by PoE_AF
 * Low                     Low             Powered by PoE AT
 * Low                     High            Powered by DC-jack
 * High                    High            Invalid
 */
void
board_print_power_status(void)
{
#ifdef GPIO_POWER_DC
    int dc = gpio_value(GPIO_POWER_DC);                 /* active high */
    int at = !gpio_value(GPIO_POWER_AT);                /* active low */
    int poe_plus = !gpio_value(GPIO_POWER_POE_PLUS);    /* active low */
    
    allow_usb = 0;

    if (dc) {
        printf("Power: DC\n");
        allow_usb = 1;
    } else if (!at) {
        if (poe_plus) {
            printf("Power: 802.3af POE+\n");
            allow_usb = 1;
        } else {
            printf("Power: 802.3af POE\n");
        }
    } else if (at) {
        printf("Power: 802.3at POE\n");
        allow_usb = 1;
    } else {
        printf("Power: Unknown\n");
    }
#endif
}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#ifdef GPIO_POWER_DC
    printf("DC:              %s\n", gpio_value(GPIO_POWER_DC) ? "high" : "low");
    printf("POE+:            %s\n", gpio_value(GPIO_POWER_POE_PLUS) ? "high" : "low");
    printf("AT:              %s\n", gpio_value(GPIO_POWER_AT) ? "high" : "low");
#endif
#ifdef GPIO_CONFIG_CLEAR
    printf("Config reset:    %s\n", gpio_value(GPIO_CONFIG_CLEAR) ? "inactive" : "pending");
#endif
#ifdef GPIO_PHY_0_INT
    printf("PHY interrupt:   %s\n", gpio_value(GPIO_PHY_0_INT) ? "inactive" : "pending");
#endif
    return 0;
}

U_BOOT_CMD(
    gpio_pins,    1,    1,     do_gpio_pins,
    "gpio_pins    - show GPIO pin status\n",
    " Usage: gpio_pins \n"
);

DECLARE_GLOBAL_DATA_PTR;
extern volatile uchar PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
int tsec_send(struct eth_device *dev, volatile void *packet, int length);
int tsec_recv_test(struct eth_device *dev,  uchar *pkt); /* Check for received packets */

#if 0
#define PKT_LEN 1024

char sbuf[PKT_LEN], dbuf[PKT_LEN];

int
do_traffic_test(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    struct eth_device* en_xmit = eth_get_dev_by_name("eth0");
    struct eth_device* en_recv = eth_get_dev_by_name("eth1");
    bd_t *bd = gd->bd;
    static uchar mac[6];
    volatile IP_t *ip;
    volatile ushort *s;
    volatile uchar *pkt;
    int     i, pkt_xmit = 0, pkt_recv = 0;
//    int  tst_seq_num = 0;
    int     pkt_cnt = 1, rec_len;
    int     run_forever = 0, direction = 0;
    ulong   tst_src_ip, tst_dst_ip, timeout;
    uchar   rec_buf[2048];
    int c;

    optind = 1;
    optreset = 1;
    while ((c = getopt (argc, argv, "d:fn:hH")) != -1) {
        switch (c)
        {
        case 'h':
        case 'H':
            //mtest_usage();
            return 0;

        case 'd':
            direction = simple_strtoul(optarg,NULL,0);
            if (direction) {
                printf("Test: eth1->eth0\n");
                en_xmit = eth_get_dev_by_name("eth1");
                en_recv = eth_get_dev_by_name("eth0");
            } else {
                printf("Test: eth0->eth1\n");
                en_xmit = eth_get_dev_by_name("eth0");
                en_recv = eth_get_dev_by_name("eth1");
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
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;

        default:
            break;
        }
    }

    //index = argc - optind + 1;

    tst_src_ip = 0x0;
    tst_dst_ip = 0xffffffff;
    // set up the packet buffers
    NetTxPacket = NULL;
    if (!NetTxPacket) {
        /*
         *    Setup packet buffers, aligned correctly.
         */
        NetTxPacket = &PktBuf[0] + (PKTALIGN - 1);
        NetTxPacket -= (ulong)NetTxPacket % PKTALIGN;
        for (i = 0; i < PKTBUFSRX; i++) {
            NetRxPackets[i] = NetTxPacket + (i+1)*PKTSIZE_ALIGN;
        }
    }

    // init both eth0 and eth1
    if (en_xmit->init(en_xmit, bd)) {
        en_xmit->state = ETH_STATE_ACTIVE;
    } else {
        printf("%s failed to initialize\n", en_xmit->name);
        return 0;
    }
    if (en_recv->init(en_recv, bd)) {
        en_recv->state = ETH_STATE_ACTIVE;
    } else {
        printf("%s failed to initialize\n", en_recv->name);
        return 0;
    }

    memcpy(mac, NetBcastAddr, 6);
    pkt = NetTxPacket;
    memcpy(NetOurEther, en_xmit->enetaddr, 6);
    pkt += NetSetEther(pkt, mac, PROT_IP);

    ip = (volatile IP_t *)pkt;

    /*
     *    Construct an IP and ICMP header.
     */
    ip->ip_hl_v  = 0x45;        /* IP_HDR_SIZE / 4 (not including UDP) */
    ip->ip_tos   = 0;
    ip->ip_len   = htons(PKT_LEN - ETHER_HDR_SIZE);
    ip->ip_id    = htons(NetIPID++);
    ip->ip_off   = htons(0x4000);    /* No fragmentation */
    ip->ip_ttl   = 0x2;
    ip->ip_p     = IPPROTO_UDP;
    ip->ip_sum   = 0;
    NetCopyIP((void*)&ip->ip_src, &tst_src_ip); /* already in network byte order */
    NetCopyIP((void*)&ip->ip_dst, &tst_dst_ip);       /* - "" - */
    ip->ip_sum   = ~NetCksum((uchar *)ip, IP_HDR_SIZE_NO_UDP / 2);

    s = &ip->udp_src;              /* UDP starts here */
    s[0] = htons(0x0044);          /* src port */
    s[1] = htons(0x0044);          /* dst port */
    s[2] = (PKT_LEN - IP_HDR_SIZE_NO_UDP - ETHER_HDR_SIZE - 8); /* length */
    s[3] = 0;   /* checksum not present */

    memset((uchar *)&s[4], 0xa5, PKT_LEN - IP_HDR_SIZE - ETHER_HDR_SIZE);
//    s[3] = ~NetCksum((uchar *)s, (PKT_LEN - IP_HDR_SIZE_NO_UDP - ETHER_HDR_SIZE) / 2);

    i = 0;
    while ((pkt_xmit < pkt_cnt) || (run_forever)) {
        tsec_send(en_xmit, NetTxPacket, PKT_LEN);
        pkt_xmit++;
        timeout = 0x100;
        do {
            if ((rec_len = tsec_recv_test(en_recv, rec_buf)) != 0) {
                //printf("got one!\n");
                if (memcmp((void*) NetTxPacket, rec_buf, PKT_LEN) != 0) {
                    memcpy(sbuf, (char *)NetTxPacket, PKT_LEN);
                    memcpy(dbuf, rec_buf, PKT_LEN);
                    printf("#1 Packet data miscompares... %p %p\n", &sbuf[0], &dbuf[0]);
                }
                pkt_recv++;
            }
            WATCHDOG_RESET();
            if (ctrlc()) {
                puts ("\nAbort\n");
                run_forever = 0; pkt_cnt = 0;
                break;
            }
        } while ((rec_len == 0) && timeout--);
        if (timeout == 0) {
            printf("Timeout waiting for packet!\n");
        }
        if (ctrlc()) {
            puts ("\nAbort\n");
            run_forever = 0; pkt_cnt = 0;
            break;
        }
        WATCHDOG_RESET();
    }

    if (pkt_xmit > pkt_recv) {
        timeout = 0x100;
        do {
            if ((rec_len = tsec_recv_test(en_recv, rec_buf)) != 0) {
                //printf("got one!\n");
                if (memcmp((void*) NetTxPacket, rec_buf, PKT_LEN) != 0) {
                    printf("#2 Packet data miscompares...\n");
                }
                pkt_recv++;
            }
            WATCHDOG_RESET();
            if (ctrlc()) {
                puts ("\nAbort\n");
                run_forever = 0; pkt_cnt = 0;
                break;
            }
        } while ((rec_len == 0) && timeout--);
        if (timeout == 0) {
            printf("Timeout waiting for packet!\n");
        }
    }

    printf("Sent %d packets, Received %d packets\n",pkt_xmit,pkt_recv);

    en_xmit->halt(en_xmit);
    en_recv->halt(en_recv);
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

static int
do_phy_temp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint16_t page, val;
    int idx;

    /* retrieve PHY temperatures */
    for (idx = 0; idx < 1; idx++) {
        miiphy_read("eth0", idx, 0x16, &page);
        miiphy_write("eth0", idx, 0x16, 0x0006);
        miiphy_read("eth0", idx, 0x1b, &val);
        /* see 154x datasheet, register page 6, register 27 */
        val &= 0xff;
        printf("PHY address 0x%x temperature is %d C\n", idx, val - 25);
        /* restore page to its previous value */
        miiphy_write("eth0", idx, 0x16, page);
    }
    return 0;
}

U_BOOT_CMD(
    phyTemp,    7,    1,     do_phy_temp,
    "phyTemp    - display PHY temperature\n",
    " Usage: phyTemp \n"
);

extern int i2c_write_raw(u8, u8 *, int);
extern int i2c_read_raw(u8, u8 *, int);
int
tpm_i2c_read_bb(int bus, int dev, unsigned char *buf, int count)
{
	int bnum = i2c_get_bus_num();
	int r;

	i2c_set_bus_num(0);
	r = i2c_read_raw(dev, buf, count);
//	r = i2c_read(dev, 0, 0, buf, count);
	i2c_set_bus_num(bnum);
	return r;
}
int
tpm_i2c_write_bb(int bus, int dev, unsigned char *buf, int count)
{
	int bnum = i2c_get_bus_num();
	int r;

	i2c_set_bus_num(0);
//	r = i2c_write_raw(dev, buf, count);
	r = i2c_write(dev, 0, 0, buf, count);
	i2c_set_bus_num(bnum);
	return r;
}
