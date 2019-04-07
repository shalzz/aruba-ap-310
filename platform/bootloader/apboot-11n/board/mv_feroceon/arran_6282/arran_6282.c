/* vim:set ts=4 sw=4 expandtab: */
#include <config.h>
#include <common.h>
#include <command.h>
#include <miiphy.h>
#include <exports.h>
#include <linux/ctype.h>
#include <aruba_manuf.h>
#include <watchdog.h>
#include "../mv_hal/uart/mvUart.h"

extern void gpio_set_as_input(unsigned);
extern void gpio_set_as_output(unsigned);
extern void gpio_out(unsigned, unsigned);
extern unsigned gpio_value(unsigned);

#if (CONFIG_COMMANDS & CFG_CMD_MII)
static void
arran_phy_init(void)
{
    uint16_t page;
    int idx;

    /* initialize PHY LEDs per PRD */
    for (idx = 0; idx < 2; idx++) {
        miiphy_read("eth0", idx, 0x16, &page);
        miiphy_write("eth0", idx, 0x16, 0x0003);
        miiphy_write("eth0", idx, 0x10, 0x0017);
        /* enable interrupt, and set polarity to active low */
        miiphy_write("eth0", idx, 0x12, 0x0080);
        /* restore page to its previous value */
        miiphy_write("eth0", idx, 0x16, page);
    }
}
#endif

int
checkboard(void)
{
#ifdef CONFIG_HW_WATCHDOG
    extern void hw_watchdog_init(void);

    hw_watchdog_init();
    WATCHDOG_RESET();
#endif
    /* drive reset pin high, but don't enable it as an output */
    gpio_out(GPIO_HW_RESET, 1);

    return 0;
}

#if defined(CONFIG_RESET_BUTTON)
static void
flash_status_led(void)
{
    int i;

    for (i = 0; i < 10; i++) {
        gpio_out(GPIO_GREEN_STATUS_LED, 1);
        mdelay(100);
        gpio_out(GPIO_GREEN_STATUS_LED, 0);
        WATCHDOG_RESET();
        mdelay(100);
    }
}

static void
check_reset_button(void)
{
    MV_UART_PORT *p0, *p1;

    p0 = mvUartBase(0);
    p1 = mvUartBase(1);

    if (!gpio_value(GPIO_CONFIG_CLEAR)) {
        /* unleash hell */
        p0->scr = 'R';
        p1->scr = 'B';
        flash_status_led();
    } else {
        /* no config clear */
        p0->scr = 'N';
        p1->scr = 'R';
    }
}
#endif

int
early_board_init(void)
{
    /* turn off red LED */
    gpio_out(GPIO_RED_STATUS_LED, 1);

    gpio_set_as_output(GPIO_RED_STATUS_LED);
    gpio_set_as_output(GPIO_GREEN_STATUS_LED);
#ifdef GPIO_FLASH_WRITE
    gpio_set_as_output(GPIO_FLASH_WRITE);
#endif
    gpio_set_as_output(GPIO_2G_RED_LED);
    gpio_set_as_output(GPIO_5G_RED_LED);
    gpio_set_as_output(GPIO_2G_GREEN_LED);
    gpio_set_as_output(GPIO_5G_GREEN_LED);
    gpio_set_as_output(GPIO_2G_RADIO_RESET);
    gpio_set_as_output(GPIO_5G_RADIO_RESET);
    gpio_set_as_output(GPIO_PHY_0_RESET);
    gpio_set_as_output(GPIO_PHY_1_RESET);

    /* turn red/green radio signals off as early as possible */
    gpio_out(GPIO_5G_RED_LED, 1);
    gpio_out(GPIO_2G_RED_LED, 1);
    gpio_out(GPIO_5G_GREEN_LED, 1);
    gpio_out(GPIO_2G_GREEN_LED, 1);

    /* take PHYs out of reset */
    gpio_out(GPIO_PHY_0_RESET, 1);
    gpio_out(GPIO_PHY_1_RESET, 1);

    /* drive PA at 4.2v */
    gpio_set_as_output(GPIO_PA_VOLTAGE);
    gpio_out(GPIO_PA_VOLTAGE, 0);

    /* take radios out of reset */
    gpio_out(GPIO_2G_RADIO_RESET, 1);
    gpio_out(GPIO_5G_RADIO_RESET, 1);

    gpio_set_as_input(GPIO_POWER_DC);
    gpio_set_as_input(GPIO_POWER_AT);
    gpio_set_as_input(GPIO_PHY_0_INT);
    gpio_set_as_input(GPIO_PHY_1_INT);
    gpio_set_as_input(GPIO_CONFIG_CLEAR);

    check_reset_button();
    return 0;
}

int n_radios = 0;

static void
arran_radio_callback(int n)
{
   n_radios = n;
}

#if defined(CONFIG_RESET_BUTTON)
static void
execute_config_clear(void)
{
    extern int do_factory_reset(cmd_tbl_t *, int, int, char **);
    char *av[3];
    MV_UART_PORT *p0, *p1;

    p0 = mvUartBase(0);
    p1 = mvUartBase(1);

    if (p0->scr != 'R' || p1->scr != 'B') {
        return;
    }
    printf("**** Configuration Reset Requested by User ****\n");

    av[0] = "factory_reset";
    av[1] = 0;
    do_factory_reset(0, 0, 1, av);
}
#endif

void
late_board_init(void)
{
    extern void ar5416_initialize(void);
    extern void ar5416_install_callback(void *);

#if (CONFIG_COMMANDS & CFG_CMD_MII)
    /*
     * we need the network devices attached for this to work, so we can't do it
     * in checkboard()
     */
    arran_phy_init();
#endif

    ar5416_install_callback(arran_radio_callback);
    ar5416_initialize();
    /*
     * turn off red/green LED signals to be safe; we already did this, so it's 
     * probably not necessary, but it's also harmless
     */
    gpio_out(GPIO_5G_RED_LED, 1);
    gpio_out(GPIO_2G_RED_LED, 1);
    gpio_out(GPIO_5G_GREEN_LED, 1);
    gpio_out(GPIO_2G_GREEN_LED, 1);

    execute_config_clear();
}

void
board_print_power_status(void)
{
    int dc = gpio_value(GPIO_POWER_DC);
    int at = !gpio_value(GPIO_POWER_AT);

    if (dc) {
        printf("Power: DC\n");
    } else {
        printf("Power: 802.3%s POE\n", at ? "at" : "af");
    }
}

int
do_gpio_pins(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    printf("DC:                       %s\n", gpio_value(GPIO_POWER_DC) ? "high" : "low");
    printf("AT:                       %s\n", gpio_value(GPIO_POWER_AT) ? "high" : "low");
    printf("Config reset:             %s\n", gpio_value(GPIO_CONFIG_CLEAR) ? "inactive" : "pending");
    printf("Port 0 (PHY 1) interrupt: %s\n", gpio_value(GPIO_PHY_1_INT) ? "pending" : "inactive");
    printf("Port 1 (PHY 0) interrupt: %s\n", gpio_value(GPIO_PHY_0_INT) ? "pending" : "inactive");
    return 0;
}

U_BOOT_CMD(
    gpio_pins,    1,    1,     do_gpio_pins,
    "gpio_pins    - show GPIO pin status\n",                   
    " Usage: gpio_pins \n"
);

DECLARE_GLOBAL_DATA_PTR;
extern volatile uchar PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];
int mvEgigaTx(struct eth_device *dev, volatile void *packet, int length);
int mvEgigaRx_test(struct eth_device *dev,  uchar *pkt); /* Check for received packets */

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
              fprintf (stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
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
        mvEgigaTx(en_xmit, NetTxPacket, PKT_LEN);
        pkt_xmit++;
        timeout = 0x100;
        do {
            if ((rec_len = mvEgigaRx_test(en_recv, rec_buf)) != 0) {
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
            if ((rec_len = mvEgigaRx_test(en_recv, rec_buf)) != 0) {
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

static int
do_phy_temp(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    uint16_t page, val;
    int idx;

    /* retrieve PHY temperatures */
    for (idx = 0; idx < 2; idx++) {
        miiphy_read("eth0", idx, 0x16, &page);
        miiphy_write("eth0", idx, 0x16, 0x0006);
        miiphy_read("eth0", idx, 0x1a, &val);
        /* see 1310 datasheet, register page 6, register 26 */
        val &= 0x1f;
        printf("PHY address 0x%x temperature is %d C\n", idx, val * 5 - 25);
        /* restore page to its previous value */
        miiphy_write("eth0", idx, 0x16, page);
    }
    return 0;
}

U_BOOT_CMD(
    phyTemp,    7,    1,     do_phy_temp,
    "phyTemp    - display PHY temperatures\n",                   
    " Usage: phyTemp \n"
);
