// vim:set ts=4 sw=4 expandtab:
/*
 * (C) Copyright 2002
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <asm/io.h>
#include <pci.h>
#include <miiphy.h>

#undef DEBUG

#define N_DEVS 24

#if (CONFIG_COMMANDS & CFG_CMD_NET) && defined(CONFIG_AR5416)
static char *names[N_DEVS] = { 
    /* QCA */
    "ar9160", "ar9160", "ar922x", "ar9280", "ar9285", "ar9390", "ar9590",
    "qca9890", "qca9890", "qca9990", "qca9983","qca6335",
    /* BRCM */
    "bcm43421", "bcm43431", "bcm4321", "bcm4331", "bcm43460", "bcm43460",
    "bcm43460", "bcm43460", "bcm43520", "bcm43520", "bcm43520",
    0 
};

static struct pci_device_id supported[N_DEVS] = {
    /* QCA */
    {0x168c, 0xff1d},      // Sowl, AR9160, without EEPROM
    {0x168c, 0x0027},      // Sowl, AR9160, as EEPROM reports it
    {0x168c, 0x0029},      // Merlin, AR9220, PCI
    {0x168c, 0x002a},      // Merlin, AR9280, PCIe
    {0x168c, 0x002b},      // Kite, AR9285, PCIe
    {0x168c, 0x0030},      // Osprey, AR9390, PCIe
    {0x168c, 0x0033},      // Peacock, AR9590, PCIe
    {0x168c, 0xabcd},      // Perigrine, AR9890, PCIe (preliminary)
    {0x168c, 0x003c},      // Perigrine, AR9890, PCIe
    {0x168c, 0x0040},      // Beeliner, AR9990, PCIe
    {0x168c, 0x0046},      // Cascade, AR9983, PCIe
    /* QCA 11ad */
    {0x1ae9, 0x0310},      // Sparrow, QCA6335, PCIe
    /* BRCM */
    {0x14e4, 0xa99d},      // BCM 43421
    {0x14e4, 0xa9a7},      // BCM 43431
    {0x14e4, 0x4321},      // BCM 4321
    {0x14e4, 0x4331},      // BCM 4331
    {0x14e4, 0x43a0},      // BCM 43460
    {0x14e4, 0x43a1},      // BCM 43460
    {0x14e4, 0x43a2},      // BCM 43460
    {0x14e4, 0x4360},      // BCM 43460
    {0x14e4, 0x43b0},      // BCM 43520
    {0x14e4, 0x43b1},      // BCM 43520
    {0x14e4, 0x43b2},      // BCM 43520
    {0     , 0     }
};

#define bus_to_phys(a)  pci_mem_to_phys((pci_dev_t)devno, a)

static char *
get_name(unsigned short vn, unsigned short id)
{
    int i;
    for (i = 0; names[i] && supported[i].vendor; i++) {
        if (supported[i].vendor == vn
            && supported[i].device == id) {
                return names[i];
        }
    }
    return "unk";
}

static void (*__callback)(int) = 0;

void
ar5416_install_callback(void *f)
{
    __callback = (void (*)(int))f;
}

int
ar5416_initialize (void)
{
    pci_dev_t devno;
    int card_number = 0;
    u32 iobase, status;
    u8 pci_cmd = 0;
    int idx = 0;
    static int first = 1;
    unsigned short vn, id;

    printf("Radio: ");

    while (1) {
        /* Find PCI device
         */
        if ((devno = pci_find_devices (supported, idx++)) < 0) {
            break;
        }

        pci_read_config_dword (devno, PCI_BASE_ADDRESS_0, &iobase);
        iobase &= ~0xf;

        if (!first) {
            printf(", ");
        }
        pci_read_config_word(devno, PCI_VENDOR_ID, &vn);
        pci_read_config_word(devno, PCI_DEVICE_ID, &id);

        printf ("%s#%u", get_name(vn, id), card_number);
        first = 0;

        pci_cmd = PCI_COMMAND_MASTER;
#ifndef CONFIG_AR5416_DONT_ENABLE
        pci_cmd |= PCI_COMMAND_MEMORY;
#endif
        pci_write_config_dword (devno,
                    PCI_COMMAND, pci_cmd);

        /* Check if I/O accesses and Bus Mastering are enabled.
         */
        pci_read_config_dword (devno, PCI_COMMAND, &status);
#ifndef CONFIG_AR5416_DONT_ENABLE
        if (!(status & PCI_COMMAND_MEMORY)) {
            printf ("Error: Can not enable MEM access.\n");
            continue;
        }
#endif

        if (!(status & PCI_COMMAND_MASTER)) {
            printf ("Error: Can not enable Bus Mastering.\n");
            continue;
        }

#ifndef CONFIG_IPROC    /* XXX */
        /* Set the latency timer for value.
         */
        pci_write_config_byte (devno, PCI_LATENCY_TIMER, 0x20);
#endif

        udelay (10 * 1000);

        card_number++;
    }
    if (card_number == 0) {
        // no cards found
        printf("none");
    }
    if (__callback) {
       (*__callback)(card_number);
    }
#ifndef CONFIG_ATH_WMAC
    printf("\n");
#endif

    return card_number;
}

int
ar5416_init(void)
{
    return 0;
}
#endif
