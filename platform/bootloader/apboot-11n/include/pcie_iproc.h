#include <common.h>
#include <pci_fsl.h>


int iproc_pcie_rd_conf_dword(struct pci_controller *hose, pci_dev_t dev, int offset, u32 * value);
int iproc_pcie_rd_conf_word(struct pci_controller *hose, pci_dev_t dev, int offset, u16 * value);
int iproc_pcie_rd_conf_byte(struct pci_controller *hose, pci_dev_t dev, int offset, u8 * value);

int iproc_pcie_wr_conf(struct pci_controller *hose, pci_dev_t dev, int offset, u32 value);

int pci_skip_dev(struct pci_controller *hose, pci_dev_t dev);
struct pci_controller * pci_get_local_hose(void);

/*
 * Broadcom IPROC address maps.
 *
 * phys		virt		size
 * c8000000	fdb00000	1M		Cryptographic SRAM
 * 08000000	@runtime	128M	PCIe-0 Memory space
 * 40000000	@runtime	128M	PCIe-1 Memory space
 * 48000000	@runtime	128M	PCIe-2 Memory space
 * f1000000	fde00000	8M		on-chip south-bridge registers
 * f1800000	fe600000	8M		on-chip north-bridge registers
 * f2000000	fee00000	1M		PCIe-0 I/O space
 * f2100000	fef00000	1M		PCIe-1 I/O space
 * 18000000				4KB		Chip Common A
 * 18001000				60KB	Chip Common B
 * 18011000				4KB		DMA core register region
 * 
 * 0x1802_4000 - 0x1802_4FFF	GMAC0 Core register region	4KB
 * 0x1802_5000 - 0x1802_5FFF	GMAC1 Core register region	4KB
 * 0x1802_6000 - 0x1802_6FFF	GMAC2 Core register region	4KB
 * 0x1802_7000 - 0x1802_7FFF	GMAC3 Core register region	4KB
 * 0x1801_2000 - 0x1801_2FFF	PCIE-AXI Bridge 0 Core register region	4KB
 * 0x1801_3000 - 0x1801_3FFF	PCIE-AXI Bridge 1 Core register region	4KB
 * 0x1801_4000 - 0x1801_4FFF	PCIE-AXI Bridge 2 Core register region	4KB
 * 0x1802_1000 - 0x1802_1FFF	USB 2.0 Core register region	4KB
 * 0x1802_2000 - 0x1802_2FFF	USB 3.0 Core register region	4KB
 * 0x1802_3000 - 0x1802_3FFF	USB 2.0 PHY Control register region	4KB
 * 0x1802_0000 - 0x1802_0FFF	SDIO 3.0 Core register region	4KB
 * 0x1801_0000 - 0x1801_0FFF	DDR2/DDR3 Controller register region	4KB
 * 0x180x_xxxx - 0x180x_xxxx	NAND Flash Controller register region	4KB
 * 0x180x_xxxx - 0x180x_xxxx	SPI flash controller register region	4KB
 * 0x1800_9000 - 0x1800_9FFF	TDM/I2S core register region	4KB
*/


#define PCIBIOS_SUCCESSFUL    			0
#define PCIBIOS_BAD_REGISTER_NUMBER		-3


/* Header type 1 (PCI-to-PCI bridges) */
#define PCI_PRIMARY_BUS		0x18	/* Primary bus number */
#define PCI_SECONDARY_BUS	0x19	/* Secondary bus number */
#define PCI_SUBORDINATE_BUS	0x1a	/* Highest bus number behind the bridge */
#define PCI_SEC_LATENCY_TIMER	0x1b	/* Latency timer for secondary interface */
#define PCI_IO_BASE		0x1c	/* I/O range behind the bridge */
#define PCI_IO_LIMIT		0x1d
#undef  PCI_IO_RANGE_TYPE_MASK
#define  PCI_IO_RANGE_TYPE_MASK	0x0fUL	/* I/O bridging type */
#define  PCI_IO_RANGE_TYPE_16	0x00
#define  PCI_IO_RANGE_TYPE_32	0x01
#undef  PCI_IO_RANGE_MASK
#define  PCI_IO_RANGE_MASK	(~0x0fUL)
#define PCI_SEC_STATUS		0x1e	/* Secondary status register, only bit 14 used */
#define PCI_MEMORY_BASE		0x20	/* Memory range behind */
#define PCI_MEMORY_LIMIT	0x22
#undef  PCI_MEMORY_RANGE_TYPE_MASK
#define  PCI_MEMORY_RANGE_TYPE_MASK 0x0fUL
#undef  PCI_MEMORY_RANGE_MASK
#define  PCI_MEMORY_RANGE_MASK	(~0x0fUL)
#define PCI_PREF_MEMORY_BASE	0x24	/* Prefetchable memory range behind */
#define PCI_PREF_MEMORY_LIMIT	0x26
#undef  PCI_PREF_RANGE_TYPE_MASK
#define  PCI_PREF_RANGE_TYPE_MASK 0x0fUL
#define  PCI_PREF_RANGE_TYPE_32	0x00
#define  PCI_PREF_RANGE_TYPE_64	0x01
#undef  PCI_PREF_RANGE_MASK
#define  PCI_PREF_RANGE_MASK	(~0x0fUL)
#define PCI_PREF_BASE_UPPER32	0x28	/* Upper half of prefetchable memory range */
#define PCI_PREF_LIMIT_UPPER32	0x2c
#define PCI_IO_BASE_UPPER16	0x30	/* Upper half of I/O addresses */
#define PCI_IO_LIMIT_UPPER16	0x32
/* 0x34 same as for htype 0 */
/* 0x35-0x3b is reserved */
#define PCI_ROM_ADDRESS1	0x38	/* Same as PCI_ROM_ADDRESS, but for htype 1 */
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_BRIDGE_CONTROL	0x3e
#define  PCI_BRIDGE_CTL_PARITY	0x01	/* Enable parity detection on secondary interface */
#define  PCI_BRIDGE_CTL_SERR	0x02	/* The same for SERR forwarding */
#define  PCI_BRIDGE_CTL_ISA	0x04	/* Enable ISA mode */
#define  PCI_BRIDGE_CTL_VGA	0x08	/* Forward VGA addresses */
#define  PCI_BRIDGE_CTL_MASTER_ABORT	0x20  /* Report master aborts */
#define  PCI_BRIDGE_CTL_BUS_RESET	0x40	/* Secondary bus reset */
#define  PCI_BRIDGE_CTL_FAST_BACK	0x80	/* Fast Back2Back enabled on secondary interface */

/*
 * Register offset definitions
 */

#define	SOC_PCIE_CONTROL	0x000	/* a.k.a. CLK_CONTROL reg */
#define	SOC_PCIE_PM_STATUS	0x008
#define	SOC_PCIE_PM_CONTROL	0x00c	/* in EP mode only ! */

#define	SOC_PCIE_EXT_CFG_ADDR	0x120
#define	SOC_PCIE_EXT_CFG_DATA	0x124
#define	SOC_PCIE_CFG_ADDR	0x1f8
#define	SOC_PCIE_CFG_DATA	0x1fc

#define	SOC_PCIE_SYS_RC_INTX_EN		0x330
#define	SOC_PCIE_SYS_RC_INTX_CSR	0x334
#define	SOC_PCIE_SYS_HOST_INTR_EN	0x344
#define	SOC_PCIE_SYS_HOST_INTR_CSR	0x348

#define	SOC_PCIE_HDR_OFF	0x400	/* 256 bytes per function */

/* 32-bit 4KB in-bound mapping windows for Function 0..3, n=0..7 */
#define	SOC_PCIE_SYS_IMAP0(f,n)		(0xc00+((f)<<9)((n)<<2)) 
/* 64-bit in-bound mapping windows for func 0..3 */
#define	SOC_PCIE_SYS_IMAP1(f)		(0xc80+((f)<<3))
#define	SOC_PCIE_SYS_IMAP2(f)		(0xcc0+((f)<<3))
/* 64-bit in-bound address range n=0..2 */
#define	SOC_PCIE_SYS_IARR(n)		(0xd00+((n)<<3))
/* 64-bit out-bound address filter n=0..2 */
#define	SOC_PCIE_SYS_OARR(n)		(0xd20+((n)<<3))
/* 64-bit out-bound mapping windows n=0..2 */
#define	SOC_PCIE_SYS_OMAP(n)		(0xd40+((n)<<3))

#define PCI_MAX_BUS		4
#define pcieHostPrimSecBusNum		(0x00000100 | (PCI_MAX_BUS<<16))
#define pcieSwitchPrimSecBusNum		(0x00000201 | (PCI_MAX_BUS<<16))


/*
 * PCIe unit register offsets.
 */
#define PCIE_DEV_ID_OFF		0x0000
#define PCIE_CMD_OFF		0x0004
#define PCIE_DEV_REV_OFF	0x0008
#define PCIE_BAR_LO_OFF(n)	(0x0010 + ((n) << 3))
#define PCIE_BAR_HI_OFF(n)	(0x0014 + ((n) << 3))
#define PCIE_HEADER_LOG_4_OFF	0x0128
#define PCIE_BAR_CTRL_OFF(n)	(0x1804 + ((n - 1) * 4))
#define PCIE_WIN04_CTRL_OFF(n)	(0x1820 + ((n) << 4))
#define PCIE_WIN04_BASE_OFF(n)	(0x1824 + ((n) << 4))
#define PCIE_WIN04_REMAP_OFF(n)	(0x182c + ((n) << 4))
#define PCIE_WIN5_CTRL_OFF	0x1880
#define PCIE_WIN5_BASE_OFF	0x1884
#define PCIE_WIN5_REMAP_OFF	0x188c
#define PCIE_CONF_ADDR_OFF	0x1f8
#define  PCIE_CONF_ADDR_EN		0x80000000
#define  PCIE_CONF_REG(r)		((r) & 0xfc)
#define  PCIE_CONF_BUS(b)		(((b) & 0x1ff) << 20)
#define  PCIE_CONF_DEV(d)		(((d) & 0x1f) << 15)
#define  PCIE_CONF_FUNC(f)		(((f) & 0x7) << 12)
#define PCIE_CONF_DATA_OFF	0x1fc
#define PCIE_MASK_OFF		0x1910
#define PCIE_CTRL_OFF		0x1a00
#define  PCIE_CTRL_X1_MODE		0x0001
#define PCIE_STAT_OFF		0x1a04
#define  PCIE_STAT_DEV_OFFS		20
#define  PCIE_STAT_DEV_MASK		0x1f
#define  PCIE_STAT_BUS_OFFS		8
#define  PCIE_STAT_BUS_MASK		0xff
#define  PCIE_STAT_LINK_DOWN		1


