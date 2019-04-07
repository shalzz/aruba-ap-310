// vim:set ts=4 sw=4 expandtab:
#include <config.h>

#include <common.h>
#include <command.h>
#include <ar7100_soc.h>
#include <pci.h>
#include <asm/addrspace.h>

void
ar7100_gpio_out_val(int gpio, int val)
{
    if (val & 0x1) {
        ar7100_reg_rmw_set(AR7100_GPIO_OUT, (1 << gpio));
    }
    else {
        ar7100_reg_rmw_clear(AR7100_GPIO_OUT, (1 << gpio));
    }
}

void
ar7100_gpio_config_output(int gpio)
{
    ar7100_reg_rmw_set(AR7100_GPIO_OE, (1 << gpio));
}

#define AR7100_RESET_REVISION_ID 0x18060090
char *ids[] = {
    "AR7131",
    "AR7141",
    "AR7161",
};

void
ar7100_cpu_id(void)
{
    extern void ar7100_sys_frequency(u32 *, u32 *, u32 *);
    __u32 rev;
    u32 cpu, ddr, ahb;

    rev = ar7100_reg_rd(AR7100_RESET_REVISION_ID);
    ar7100_sys_frequency(&cpu, &ddr, &ahb);

    printf("CPU:   %s revision: %X%u\nClock: %d MHz, DDR clock: %d MHz, Bus clock: %d MHz\n", 
        ids[rev & 0x3], 
        (rev >> 4) & 0xf,
        (rev >> 2) & 0x3,
        cpu / 1000000, ddr / 1000000, ahb / 1000000);
}

#ifdef JURA_PCI_SCAN_WORKAROUND
extern int ar7100_local_write_config(int where, int size, uint32_t value);
extern int ar7100_check_error(int verbose);
extern void plat_dev_init(void);
#endif

#if defined(CONFIG_PCI)
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
#ifdef JURA_PCI_SCAN_WORKAROUND
    uint32_t cmd;
    int scan_next=1;
#endif

    printf("PCI:   scanning bus %u ...\n", bus);
	printf("       dev fn venID devID class  rev    MBAR0    MBAR1    MBAR2    MBAR3\n");
#ifdef JURA_PCI_SCAN_WORKAROUND
	for (Device = 0; Device < PCI_MAX_PCI_DEVICES; Device=Device+scan_next) {
#else
	for (Device = 0; Device < PCI_MAX_PCI_DEVICES; Device++) {
#endif
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

#ifdef JURA_PCI_SCAN_WORKAROUND
            if (device == 0xff1d)  {   /* Atheros-speak for "can't read eeprom" */
                /* reset PCI bus and try again */
                scan_next=0;
                ar7100_reg_rmw_set(AR7100_RESET, (AR7100_RESET_PCI_BUS|AR7100_RESET_PCI_CORE));
                udelay(100 * 1000);
                ar7100_reg_rmw_clear(AR7100_RESET, (AR7100_RESET_PCI_BUS|AR7100_RESET_PCI_CORE));
                udelay(100 * 1000);
                cmd = PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE |
                      PCI_COMMAND_PARITY | PCI_COMMAND_SERR | PCI_COMMAND_FAST_BACK;
                ar7100_local_write_config(PCI_COMMAND, 4, cmd);
                udelay(100 * 1000);
                ar7100_check_error(0);
                plat_dev_init();
                break;
            }  else  {
                scan_next=1;
            }
#endif
			printf("       %02x  %02x  %04x  %04x %05x   %02x %08x %08x %08x %08x\n", 
			   Device, Function, VendorID, device, class, rev,
			   bar0, bar1, bar2, bar3);
		}
	}
}
#endif /* CONFIG_PCI */
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
    ar7100_gpio_config_output(pin);
    ar7100_gpio_out_val(pin, val);
    return 0;
}

U_BOOT_CMD(
	gpio,    3,    1,     do_gpio,
	"gpio    - poke GPIO pin\n",                   
	" Usage: gpio \n"
);

