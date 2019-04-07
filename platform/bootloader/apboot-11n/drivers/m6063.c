// vim:set ts=4 sw=4 expandtab:
#include <config.h>
#ifdef CONFIG_MARVELL_6063

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <asm/io.h>
#include <pci.h>
#include <miiphy.h>
#include "m6063.h"

extern int octeon_miiphy_write(char *, unsigned char,
    unsigned char, unsigned short);
extern int  octeon_miiphy_read(char *, unsigned char, 
    unsigned char, unsigned short *);

#define mdio_write(x,y,z) octeon_miiphy_write("en0", x,y,z)
#define mdio_read(x,y,z) octeon_miiphy_read("en0", x,y,z)

int
m6063_detect(int *rev)
{
    int result;
    uint16_t val;

    *rev = 0;
    // read the first port to get its identifier
    result = mdio_read(M6063_PORT_ADDR(0), M6063_PORT_ID_REG, &val);

    // table 53; the 88E6063 device is identified by the value 0x153
    if ((val & 0xfff0) == 0x1530) {
        *rev = val & 0xf;
        return 1;
    }
    return 0;
}

// iterate ports, set LEDs as desired
void
m6063_led_init(void)
{
    int idx;

    for (idx = M6063_PHY_ADDR(0); idx < M6063_PHY_ADDR(4); idx++) {
        // register 0x16 is parallel LED control
        // LED2 pin controls link/activity (0xa)
        // LED1 pin is NC
        // LED0 pin controls speed (4)
        mdio_write(idx, M6063_PHY_PARALLEL_LED_REG, 0xa04);
    }
}

// iterate ports and shutdown
static void
m6063_shutdown_usable_ports(void)
{
    int idx;
    uint16_t val;

    for (idx = M6063_PHY_ADDR(0); idx < M6063_PHY_ADDR(4); idx++) {
        mdio_read(idx, M6063_PHY_CONTROL_REG, &val);
        val |= (1 << 11);
        mdio_write(idx, M6063_PHY_CONTROL_REG, val);
    }
}

void
m6063_init(uint8_t *mac)
{
    m6063_shutdown_unused_ports();
    // since we decided to not boot from them, I'm disabling them all until
    // the OS comes up
    m6063_shutdown_usable_ports();
    m6063_led_init();
    m6063_set_macaddr(mac);
    m6063_init_cpu_port();
    m6063_set_port_state(M6063_CPU_PORT, M6063_PORT_STATE_DISABLED);
}

void
m6063_announce(uint8_t *mac)
{
    int rev;
    uint16_t val;

    if (m6063_detect(&rev)) {
        int i;

        printf("LAN:   88E6063 (revision %u)... ", rev);

        for (i = 0; i < 1000; i++) {
            mdio_read(M6063_PORT_ADDR(M6063_CPU_PORT), M6063_PORT_STATUS_REG, &val);
            if ((val & M6063_CPU_LINK_STATUS) == M6063_CPU_LINK_STATUS) {
                printf("up\n");
                m6063_init(mac);
                return;
            }
        }
        printf("failed [0x%x]\n", val);
    } else {
        printf("LAN:   not found\n");
    }
}

void
m6063_init_cpu_port(void)
{
    uint16_t val;

    //
    // CPU port is in PHY mode by HW strapping
    // set the ingress mode, trailer mode, etc
    //
    mdio_read(M6063_PORT_ADDR(M6063_CPU_PORT), M6063_PORT_CONTROL_REG, &val);

    mdio_write(M6063_PORT_ADDR(M6063_CPU_PORT), M6063_PORT_CONTROL_REG, val);
}

void
m6063_shutdown_unused_ports(void)
{
    // hold port 4 in reset; it will not be used
    mdio_write(M6063_PHY_ADDR(4), M6063_PHY_CONTROL_REG, (1<<11));
    // port 5 is supposed to be disabled by HW strapping; nothing to do
}

void
m6063_set_macaddr(uint8_t *mac)
{
    uint16_t val;

    // set diffaddr (bit 8)
    val = (mac[0] << 8) | (1<<8) | mac[1];
    mdio_write(M6063_SWITCH_ADDR, M6063_SWITCH_MAC_ADDRESS_REGISTER(0), val);
    val = (mac[2] << 8) | mac[3];
    mdio_write(M6063_SWITCH_ADDR, M6063_SWITCH_MAC_ADDRESS_REGISTER(1), val);
    val = (mac[4] << 8) | mac[5];
    mdio_write(M6063_SWITCH_ADDR, M6063_SWITCH_MAC_ADDRESS_REGISTER(2), val);
}

void
m6063_set_port_state(int port, int state)
{
    uint16_t val;

    mdio_read(M6063_PORT_ADDR(port), M6063_PORT_CONTROL_REG, &val);
    val &= 0x3;
    val |= state;
    mdio_write(M6063_PORT_ADDR(port), M6063_PORT_CONTROL_REG, val);
}
#endif
