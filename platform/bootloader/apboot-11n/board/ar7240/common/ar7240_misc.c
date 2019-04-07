// vim:set ts=4 sw=4 expandtab:
#include <config.h>

#include <common.h>
#include <command.h>
#include <ar7240_soc.h>
#include <pci.h>
#include <asm/addrspace.h>

void
ar7240_gpio_out_val(int gpio, int val)
{
    if (val & 0x1) {
        ar7240_reg_rmw_set(AR7240_GPIO_OUT, (1 << gpio));
    }
    else {
        ar7240_reg_rmw_clear(AR7240_GPIO_OUT, (1 << gpio));
    }
}

void
ar7240_gpio_config_output(int gpio)
{
    if (is_wasp())  {
        ar7240_reg_rmw_clear(AR7240_GPIO_OE, (1 << gpio));
    }  else  {
        ar7240_reg_rmw_set(AR7240_GPIO_OE, (1 << gpio));
    }
}

void
ar7240_gpio_config_input(int gpio)
{
    if (is_wasp())  {
        ar7240_reg_rmw_set(AR7240_GPIO_OE, (1 << gpio));
    }  else  {
        ar7240_reg_rmw_clear(AR7240_GPIO_OE, (1 << gpio));
    }
}

char *wasp_ids[] = {
    "AR9341",
    "AR9342",
    "AR9344",
};

void
ar7240_cpu_id(void)
{
#ifdef CONFIG_WASP
    extern void ar934x_sys_frequency(u32 *, u32 *, u32 *);
#else
    extern void ar7240_sys_frequency(u32 *, u32 *, u32 *);
#endif
    __u32 rev;
    u32 cpu, ddr, ahb;

    rev = ar7240_reg_rd(AR7240_REV_ID);
    rev &= AR7240_REV_ID_MASK;

#ifdef CONFIG_WASP
    ar934x_sys_frequency(&cpu, &ddr, &ahb);
#else
    ar7240_sys_frequency(&cpu, &ddr, &ahb);
#endif

    if (is_ar7240()) {
        printf("CPU:   AR7240 revision: %X%u\nClock: %d MHz, DDR rate: %d MHz, Bus clock: %d MHz\n",
            (rev >> 4) & 0xf,
            rev & 0xf,
            cpu / 1000000, ddr / 1000000, ahb / 1000000);
    } else if (is_ar7241()) {
        printf("CPU:   AR7241 revision: %u\nClock: %d MHz, DDR rate: %d MHz, Bus clock: %d MHz\n",
            rev & 0xff,
            cpu / 1000000, ddr / 1000000, ahb / 1000000);
    } else if (is_ar7242()) {
        printf("CPU:   AR7242 revision: %u\nClock: %d MHz, DDR rate: %d MHz, Bus clock: %d MHz\n",
            rev & 0xff,
            cpu / 1000000, ddr / 1000000, ahb / 1000000);
    } else if (is_wasp()) {
        printf("CPU:   %s revision: %u.%u\nClock: %d MHz, DDR rate: %d MHz, Bus clock: %d MHz\n",
            wasp_ids[((rev & 0xf000) >> 12) & 0x3],
            ((rev & 0xf00) >> 8),
            (rev & 0xf),
            cpu / 1000000, ddr / 1000000, ahb / 1000000);
    }
}

#ifdef CONFIG_PCI
void
pci_bus_scan(int bus)
{
//	u32 iobase, status;
	int Device;
	int Function;
	unsigned char HeaderType;
	unsigned short VendorID;
	pci_dev_t dev;
	u16 device;
	u32 bar0, bar1, bar2, bar3;
	u8 class, subclass, rev;

    printf("PCI:   scanning bus %u ...\n", bus);
	printf("       dev fn venID devID class  rev    MBAR0    MBAR1    MBAR2    MBAR3\n");
	for (Device = 0; Device < PCI_MAX_PCI_DEVICES; Device++) {
		HeaderType = 0;
		VendorID = 0;
		for (Function = 0; Function < PCI_MAX_PCI_FUNCTIONS; Function++) {
			/*
			 * If this is not a multi-function device, we skip the rest.
			 */
			if (Function && !(HeaderType & 0x80))
				break;

			dev = PCI_BDF(bus, Device, Function);

			pci_read_config_word(dev, PCI_VENDOR_ID, &VendorID);
			if ((VendorID == 0xFFFF) || (VendorID == 0x0000))
				continue;

			if (!Function) pci_read_config_byte(dev, PCI_HEADER_TYPE, &HeaderType);

			pci_read_config_word(dev, PCI_DEVICE_ID, &device);
			pci_read_config_byte(dev, PCI_CLASS_CODE, &class);
			pci_read_config_byte(dev, PCI_CLASS_SUB_CODE, &subclass);
			pci_read_config_byte(dev, PCI_REVISION_ID, &rev);
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &bar0);
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_1, &bar1);
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_2, &bar2);
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_3, &bar3);

			printf("       %02x  %02x  %04x  %04x %05x   %02x %08x %08x %08x %08x\n", 
			   Device, Function, VendorID, device, class, rev,
			   bar0, bar1, bar2, bar3);
		}
	}
}
#endif

int
do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int pin, val;
    if (argc != 3) {
        printf("Usage: \n%s\n", cmdtp->usage);
        return 1;
    }
    pin = simple_strtoul(argv[1], (char **)0, 10);
    val = simple_strtoul(argv[2], (char **)0, 10);

    printf("Driving pin %u as output %s\n", pin, val & 1 ? "high" : "low");
    ar7240_gpio_config_output(pin);
    ar7240_gpio_out_val(pin, val);
    return 0;
}

U_BOOT_CMD(
	gpio,    3,    1,     do_gpio,
	"gpio    - poke GPIO pin\n",                   
	" Usage: gpio \n"
);

#if defined(CONFIG_CARDHU)
int
do_poe(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int pin, val;
    if (argc != 2) {
        printf("Usage: \n%s\n", cmdtp->usage);
        return 1;
    }
    pin = 1;  /* pin1 = PSE enable/disable GPIO for cardhu (1=enabled, 0=disabled) */
    if (!strcmp(argv[1], "on"))  {
        val=1;
    } else if (!strcmp(argv[1], "off")) {
        val=0;
    } else {
        printf("Unrecognized poe command: '%s'\n", argv[1]);
        return 1;
    }
    /* pin1 = PSE enable/disable (1=enabled, 0=disabled) */
    ar7240_gpio_config_output(pin);
    ar7240_gpio_out_val(pin, val);
#if CONFIG_JTAG_DISABLE
    ar7240_reg_wr(AR934X_GPIO_FUNC, 2);    /* disable jtag */
#endif
    return 0;
}

U_BOOT_CMD(
	poe,    2,    1,     do_poe,
	"poe    - enable/disable poe\n",
	" Usage: poe <on|off>\n"
);
#endif

#if AP_PROVISIONING_IN_BANK2
int
do_bank(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u32 mux1, mux2;
    int current_bank, new_bank;

    if (argc > 2) {
        printf("Usage: \n%s\n", cmdtp->usage);
        return 1;
    }

    mux1 = *((volatile u32 *)KSEG1ADDR(DUAL_FLASH_MUX1_REG));
    mux2 = *((volatile u32 *)KSEG1ADDR(DUAL_FLASH_MUX2_REG));
    if ((mux1 & DUAL_FLASH_MUX1_MASK_BANK1) == 0x0900)
        current_bank = 1;
    else if ((mux2 & DUAL_FLASH_MUX2_MASK_BANK2) == 0x0900)
        current_bank = 2;
    else  {
        /* for Hazelburn, only mux1 is used for both bank 1 and 2*/
        if ((mux1 & DUAL_FLASH_MUX2_MASK_BANK2) == 0x09) {
            current_bank = 2;
        } else {
            printf("Cannot determine current flash bank\n");
            return 1;
        }
    }

    if (argc == 1)  {
        printf("Current flash bank: %d (of 2)\n", current_bank);
        return 0;
    }

    if (!strcmp(argv[1], "1"))  {
        new_bank=1;
    } else if (!strcmp(argv[1], "2")) {
        new_bank=2;
    } else {
        printf("Unrecognized flash bank: '%s'\n", argv[1]);
        return 1;
    }

    if (new_bank == 1)  {
        // switch to bank 1
        *((volatile u32 *)KSEG1ADDR(DUAL_FLASH_MUX2_REG)) = DUAL_FLASH_MUX2_VAL_BANK1;
        *((volatile u32 *)KSEG1ADDR(DUAL_FLASH_MUX1_REG)) = DUAL_FLASH_MUX1_VAL_BANK1;
    }  else if (new_bank == 2)  {
        // switch to bank 2
        *((volatile u32 *)KSEG1ADDR(DUAL_FLASH_MUX1_REG)) = DUAL_FLASH_MUX1_VAL_BANK2;
        *((volatile u32 *)KSEG1ADDR(DUAL_FLASH_MUX2_REG)) = DUAL_FLASH_MUX2_VAL_BANK2;
    }
    *((volatile u32 *)KSEG1ADDR(0xbf000000));  /* flush what appears to be a 32-byte */
    *((volatile u32 *)KSEG1ADDR(0xbf001000));  /* spi flash buffer on the CPU itself */

    if (new_bank != current_bank)
        printf("Switching to flash bank: %d\n", new_bank);

    return 0;
}

U_BOOT_CMD(
	bank,    2,    1,     do_bank,
	"bank   - select flash bank (addressable at 0xbf000000)\n",
	" Usage: bank [1|2]\n"
);
#endif
