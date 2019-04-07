/* vim:set ts=4 sw=4 expandtab: */
#include <config.h>
#ifdef CONFIG_MARVELL_6172

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <asm/io.h>
#include <pci.h>
#include <miiphy.h>
#include "m6172.h"

extern void mvEthSwitchRegRead(uint32_t ethPortNum, uint32_t phyAddr, uint32_t regOffs, uint16_t *data);
extern void mvEthSwitchRegWrite(uint32_t ethPortNum, uint32_t phyAddr, uint32_t regOffs, uint16_t data);
extern void mvEthPhyRegRead(uint32_t phyAddr, uint32_t regOffs, uint16_t *data);
extern void mvEthPhyRegWrite(uint32_t phyAddr, uint32_t regOffs, uint16_t data);

static uint16_t
m6172_mdio_read(uint16_t addr, uint16_t reg, uint16_t *data)
{
/*    mvEthSwitchRegRead(__MARVELL_6172_ETH_PORT, addr, reg, data); */
    mvEthPhyRegRead(addr, reg, data);
    return 0;
}

static void
m6172_mdio_write(uint16_t addr, uint16_t reg, uint16_t data)
{
/*    mvEthSwitchRegWrite(__MARVELL_6172_ETH_PORT, addr, reg, data); */
    mvEthPhyRegWrite(addr, reg, data);
}

static void
m6172_wait_for_smi_done(void)
{
    uint16_t val;

    do {
        m6172_mdio_read(M6172_GLOBAL2_ADDR, 0x18, &val);
    } while (val & 0x8000);
}

static uint16_t
m6172_phy_mdio_read(uint16_t addr, uint16_t reg, uint16_t *data)
{
    uint16_t val;

    reg &= 0x1f;
    addr &= 0x1f;

    m6172_wait_for_smi_done();

    /* table 170 */
    val = reg | (addr << 5);

    /* clause 22, read */
    val |= ((1 << 15) | (2 << 10) | (1 << 12));

    m6172_mdio_write(M6172_GLOBAL2_ADDR, 0x18, val);

    m6172_wait_for_smi_done();

    m6172_mdio_read(M6172_GLOBAL2_ADDR, 0x19, data);
    return 0;
}

static void
m6172_phy_mdio_write(uint16_t addr, uint16_t reg, uint16_t data)
{
    uint16_t val;

    reg &= 0x1f;
    addr &= 0x1f;

    m6172_wait_for_smi_done();

    m6172_mdio_write(M6172_GLOBAL2_ADDR, 0x19, data);

    /* table 170 */
    val = reg | (addr << 5);

    /* clause 22, write */
    val |= ((1 << 15) | (1 << 10) | (1 << 12));

    m6172_mdio_write(M6172_GLOBAL2_ADDR, 0x18, val);

    m6172_wait_for_smi_done();
}

int
m6172_detect(int *rev)
{
    int result;
    uint16_t val;

    *rev = 0;
    /* read the first port to get its identifier */
    result = m6172_mdio_read(M6172_PORT_ADDR(1), M6172_PORT_ID_REG, &val);

    /* table 61; the 88E6172 device is identified by the value 0x172 */
    if ((val & 0xfff0) == 0x1720) {
        *rev = val & 0xf;
        return 1;
    }
    return 0;
}

/* iterate ports, set LEDs as desired */
void
m6172_led_init(void)
{
    int idx;

    for (idx = M6172_PORT_ADDR(1); idx < M6172_PORT_ADDR(5); idx++) {
        /* 
         * register 0x16 is parallel LED control
         * LED0 pin controls gig on/off (0x2)
         * LED1 pin controls link/act/off (0x2)
         *
         * Set bit 15 to indicate update
         */
        m6172_mdio_write(idx, M6172_PORT_LED_CONTROL_REG, 0x8022);
    }
}

/* iterate ports and shutdown */
static void
m6172_shutdown_usable_ports(void)
{
    int idx;
    uint16_t val;

    for (idx = M6172_PHY_ADDR(1); idx < M6172_PHY_ADDR(5); idx++) {
        m6172_phy_mdio_read(idx, M6172_PHY_CONTROL_REG, &val);
        val |= (1 << 11);
        m6172_phy_mdio_write(idx, M6172_PHY_CONTROL_REG, val);
    }
}

static void
m6172_reset(void)
{
    uint16_t val;

    /* 
     * a soft-reset of the board does not reset the chip
     * to its power-on defaults, so do that here
     */
    m6172_mdio_read(M6172_GLOBAL1_ADDR, M6172_SWITCH_CONTROL_REGISTER, &val);
    val |= (1 << 15);
    m6172_mdio_write(M6172_GLOBAL1_ADDR, M6172_SWITCH_CONTROL_REGISTER, val);

    /* force link down */
    m6172_mdio_read(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_PHYSICAL_CONTROL_REG, &val);
    val |= (1 << 4);
    m6172_mdio_write(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_PHYSICAL_CONTROL_REG, val);

    udelay(1000);

    /* enable TX/RX delay on RGMII */
    m6172_mdio_read(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_PHYSICAL_CONTROL_REG, &val);
    val |= 0xc000;
    m6172_mdio_write(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_PHYSICAL_CONTROL_REG, val);
    udelay(1000);

    val &= ~(1 << 4);
    m6172_mdio_write(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_PHYSICAL_CONTROL_REG, val);
    udelay(1000);

}

void
m6172_init(uint8_t *mac)
{
    m6172_reset();
    m6172_shutdown_unused_ports();
    /* 
     * since we decided to not boot from them, I'm disabling them all until
     * the OS comes up
     */
    m6172_shutdown_usable_ports();
    m6172_led_init();
    m6172_set_macaddr(mac);
    m6172_init_cpu_port();
    m6172_set_port_state(M6172_CPU_PORT, M6172_PORT_STATE_DISABLED);
}

void
m6172_announce(uint8_t *mac)
{
    int rev;
    uint16_t val;

    if (m6172_detect(&rev)) {
        int i;

        printf("LAN:   88E6172 (revision %u)... ", rev);

        for (i = 0; i < 1000; i++) {
            m6172_mdio_read(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_STATUS_REG, &val);
            if ((val & M6172_CPU_LINK_STATUS) == M6172_CPU_LINK_STATUS) {
                printf("up\n");
                m6172_init(mac);
                return;
            }
        }
        printf("failed [0x%x]\n", val);
    } else {
        printf("LAN:   not found\n");
    }
}

void
m6172_init_cpu_port(void)
{
#if 0
    uint16_t val;

    /*
     * CPU port is in PHY mode by HW strapping
     * set the ingress mode, trailer mode, etc
     */
    m6172_mdio_read(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_CONTROL_REG, &val);

    m6172_mdio_write(M6172_PORT_ADDR(M6172_CPU_PORT), M6172_PORT_CONTROL_REG, val);
#endif
}

void
m6172_shutdown_unused_ports(void)
{
return;
    /* hold ports 0, 6, and 7 in reset; they will not be used */
    m6172_phy_mdio_write(M6172_PHY_ADDR(0), M6172_PHY_CONTROL_REG, (1<<11));
    m6172_phy_mdio_write(M6172_PHY_ADDR(6), M6172_PHY_CONTROL_REG, (1<<11));
    m6172_phy_mdio_write(M6172_PHY_ADDR(7), M6172_PHY_CONTROL_REG, (1<<11));
}

/*
 * This is confusing; see tables 139 and 140 in the spec.
 * An indirect write scheme is used, and bits 0-7 in the address
 * go in the last location indexed via the "pointer"
 */
void
m6172_set_macaddr(uint8_t *mac)
{
    uint16_t val;
    int i;

    for (i = 5; i >= 0; i--) {
        val = mac[i];
        if (i == 5) {
            /* set diffaddr (bit 0) along with address bits 41-47 */
            val |= 1;
        }
        val |= ((5 - i) << 8);    /* pointer index, 41-47 at 0 */
        val |= 0x8000;      /* force update */

        m6172_mdio_write(M6172_GLOBAL2_ADDR, M6172_SWITCH_MAC_ADDRESS_REGISTER, val);
    }
}

void
m6172_set_port_state(int port, int state)
{
    uint16_t val;

    m6172_mdio_read(M6172_PORT_ADDR(port), M6172_PORT_CONTROL_REG, &val);
    val &= 0x3;
    val |= state;
    m6172_mdio_write(M6172_PORT_ADDR(port), M6172_PORT_CONTROL_REG, val);
}

/* m6172 read|write port addr [val] */
int
do_m6172(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned addr;
    unsigned port;
    unsigned val;
    int write;

    if (argc < 4) {
usage:
        printf("usage: m6172 read|write port addr [val]\n");
        return 0;
    }
    write = !strcmp(argv[1], "write");
    if (write && argc != 5) {
        goto usage;
    }

    port = simple_strtoul(argv[2], (char **)0, 0);
    addr = simple_strtoul(argv[3], (char **)0, 0);

    if (write) {
        val = simple_strtoul(argv[4], (char **)0, 0);
        m6172_phy_mdio_write(M6172_PHY_ADDR(port), addr, val);
    } else {
        uint16_t data;
        m6172_phy_mdio_read(M6172_PHY_ADDR(port), addr, &data);
        printf("Value at port %u, reg 0x%x is 0x%x\n", port, addr, data);
    }

    return 0;
}

U_BOOT_CMD(
    m6172,     5,     1,      do_m6172,
    "m6172 - read/write m6172 PHY registers\n",
    "\n"
);
#endif
