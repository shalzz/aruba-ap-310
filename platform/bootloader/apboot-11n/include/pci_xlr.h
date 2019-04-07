#ifndef _PCI_H
#define _PCI_H

#include "list.h"
#include "linux/types.h"
#include "on_chip.h"

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
#define PCI_COMMAND		0x04	/* 16 bits */
#define  PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define  PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL	0x8	/* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE	0x10	/* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20	/* Enable palette snooping */
#define  PCI_COMMAND_PARITY	0x40	/* Enable parity checking */
#define  PCI_COMMAND_WAIT 	0x80	/* Enable address/data stepping */
#define  PCI_COMMAND_SERR	0x100	/* Enable SERR */
#define  PCI_COMMAND_FAST_BACK	0x200	/* Enable back-to-back writes */

#define PCI_STATUS		0x06	/* 16 bits */
#define  PCI_STATUS_CAP_LIST	0x10	/* Support Capability List */
#define  PCI_STATUS_66MHZ	0x20	/* Support 66 Mhz PCI 2.1 bus */
#define  PCI_STATUS_UDF		0x40	/* Support User Definable Features [obsolete] */
#define  PCI_STATUS_FAST_BACK	0x80	/* Accept fast-back to back */
#define  PCI_STATUS_PARITY	0x100	/* Detected parity error */
#define  PCI_STATUS_DEVSEL_MASK	0x600	/* DEVSEL timing */
#define  PCI_STATUS_DEVSEL_FAST	0x000
#define  PCI_STATUS_DEVSEL_MEDIUM 0x200
#define  PCI_STATUS_DEVSEL_SLOW 0x400
#define  PCI_STATUS_SIG_TARGET_ABORT 0x800	/* Set on target abort */
#define  PCI_STATUS_REC_TARGET_ABORT 0x1000	/* Master ack of " */
#define  PCI_STATUS_REC_MASTER_ABORT 0x2000	/* Set on master abort */
#define  PCI_STATUS_SIG_SYSTEM_ERROR 0x4000	/* Set when we drive SERR */
#define  PCI_STATUS_DETECTED_PARITY 0x8000	/* Set on parity error */

#define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8
					   revision */
#define PCI_REVISION_ID         0x08	/* Revision ID */
#define PCI_CLASS_PROG          0x09	/* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a	/* Device class */
#define PCI_CLASS_CODE      0x0b	/* Device class code */
#define PCI_CLASS_SUB_CODE  0x0a	/* Device sub-class code */

#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define  PCI_HEADER_TYPE_NORMAL	0
#define  PCI_HEADER_TYPE_BRIDGE 1
#define  PCI_HEADER_TYPE_CARDBUS 2

#define PCI_BIST		0x0f	/* 8 bits */
#define PCI_BIST_CODE_MASK	0x0f	/* Return result */
#define PCI_BIST_START		0x40	/* 1 to start BIST, 2 secs or less */
#define PCI_BIST_CAPABLE	0x80	/* 1 if BIST capable */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of 
 * 0xffffffff to the register, and reading it back.  Only 
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
#define  PCI_BASE_ADDRESS_SPACE	0x01	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO 0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define  PCI_BASE_ADDRESS_MEM_TYPE_MASK 0x06
#define  PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define  PCI_BASE_ADDRESS_MEM_TYPE_1M	0x02	/* Below 1M [obsolete] */
#define  PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define  PCI_BASE_ADDRESS_MEM_MASK	(~0x0fUL)
#define  PCI_BASE_ADDRESS_IO_MASK	(~0x03UL)
/* bit 1 is reserved if address_space = 1 */

/* Header type 0 (normal devices) */
#define PCI_CARDBUS_CIS		0x28
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e
#define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
#define  PCI_ROM_ADDRESS_ENABLE	0x01
#define PCI_ROM_ADDRESS_MASK	(~0x7ffUL)

#define PCI_CAPABILITY_LIST	0x34	/* Offset of first capability list entry */

/* Capability lists */
#define PCI_CAP_LIST_ID         0       /* Capability ID */
#define  PCI_CAP_ID_PM          0x01    /* Power Management */
#define  PCI_CAP_ID_AGP         0x02    /* Accelerated Graphics Port */
#define  PCI_CAP_ID_VPD         0x03    /* Vital Product Data */
#define  PCI_CAP_ID_SLOTID      0x04    /* Slot Identification */
#define  PCI_CAP_ID_MSI         0x05    /* Message Signalled Interrupts */
#define  PCI_CAP_ID_CHSWP       0x06    /* CompactPCI HotSwap */
#define  PCI_CAP_ID_PCIX        0x07    /* PCI-X */
#define  PCI_CAP_ID_HT          0x08    /* HyperTransport */
#define  PCI_CAP_ID_SHPC        0x0C    /* PCI Standard Hot-Plug Controller */
#define  PCI_CAP_ID_EXP         0x10    /* PCI Express */
#define  PCI_CAP_ID_MSIX        0x11    /* MSI-X */
#define PCI_CAP_LIST_NEXT       1       /* Next capability in the list */
#define PCI_CAP_FLAGS           2       /* Capability defined flags (16 bits) */
#define PCI_CAP_SIZEOF          4

/* 0x35-0x3b are reserved */
#define PCI_INTERRUPT_LINE	0x3c	/* 8 bits */
#define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */
#define PCI_MIN_GNT		0x3e	/* 8 bits */
#define PCI_MAX_LAT		0x3f	/* 8 bits */

/* Header type 1 (PCI-to-PCI bridges) */
#define PCI_PRIMARY_BUS     0x18	/* Primary bus number */
#define PCI_SECONDARY_BUS   0x19	/* Secondary bus number */
#define PCI_SUBORDINATE_BUS 0x1a	/* Highest bus number behind the bridge */
#define PCI_SEC_LATENCY_TIMER   0x1b	/* Latency timer for secondary interfa
					   ce */
#define PCI_IO_BASE     0x1c	/* I/O range behind the bridge */
#define PCI_IO_LIMIT        0x1d
#define  PCI_IO_RANGE_TYPE_MASK 0x0f	/* I/O bridging type */
#define  PCI_IO_RANGE_TYPE_16   0x00
#define  PCI_IO_RANGE_TYPE_32   0x01
#define  PCI_IO_RANGE_MASK  ~0x0f
#define PCI_SEC_STATUS      0x1e	/* Secondary status register, only bit 14
					   used */
#define PCI_MEMORY_BASE     0x20	/* Memory range behind */
#define PCI_MEMORY_LIMIT    0x22
#define  PCI_MEMORY_RANGE_TYPE_MASK 0x0f
#define  PCI_MEMORY_RANGE_MASK  ~0x0f
#define PCI_PREF_MEMORY_BASE    0x24	/* Prefetchable memory range behind */
#define PCI_PREF_MEMORY_LIMIT   0x26
#define  PCI_PREF_RANGE_TYPE_MASK 0x0f
#define  PCI_PREF_RANGE_TYPE_32 0x00
#define  PCI_PREF_RANGE_TYPE_64 0x01
#define  PCI_PREF_RANGE_MASK    ~0x0f
#define PCI_PREF_BASE_UPPER32   0x28	/* Upper half of prefetchable memory r
					   ange */
#define PCI_PREF_LIMIT_UPPER32  0x2c
#define PCI_IO_BASE_UPPER16 0x30	/* Upper half of I/O addresses */
#define PCI_IO_LIMIT_UPPER16    0x32
/* 0x34 same as for htype 0 */
/* 0x35-0x3b is reserved */
#define PCI_ROM_ADDRESS1    0x38	/* Same as PCI_ROM_ADDRESS, but for htype 1 */
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_BRIDGE_CONTROL  0x3e
#define  PCI_BRIDGE_CTL_PARITY  0x01	/* Enable parity detection on secondary interface */
#define  PCI_BRIDGE_CTL_SERR    0x02	/* The same for SERR forwarding */
#define  PCI_BRIDGE_CTL_NO_ISA  0x04	/* Disable bridging of ISA ports */
#define  PCI_BRIDGE_CTL_VGA 0x08	/* Forward VGA addresses */
#define  PCI_BRIDGE_CTL_MASTER_ABORT 0x20	/* Report master aborts */
#define  PCI_BRIDGE_CTL_BUS_RESET 0x40	/* Secondary bus reset */
#define  PCI_BRIDGE_CTL_FAST_BACK 0x80	/* Fast Back2Back enabled on secondary interface */

/* Header type 2 (CardBus bridges) */
#define PCI_CB_CAPABILITY_LIST  0x14
/* 0x15 reserved */
#define PCI_CB_SEC_STATUS   0x16	/* Secondary status */
#define PCI_CB_PRIMARY_BUS  0x18	/* PCI bus number */
#define PCI_CB_CARD_BUS     0x19	/* CardBus bus number */
#define PCI_CB_SUBORDINATE_BUS  0x1a	/* Subordinate bus number */
#define PCI_CB_LATENCY_TIMER    0x1b	/* CardBus latency timer */
#define PCI_CB_MEMORY_BASE_0    0x1c
#define PCI_CB_MEMORY_LIMIT_0   0x20
#define PCI_CB_MEMORY_BASE_1    0x24
#define PCI_CB_MEMORY_LIMIT_1   0x28
#define PCI_CB_IO_BASE_0    0x2c
#define PCI_CB_IO_BASE_0_HI 0x2e
#define PCI_CB_IO_LIMIT_0   0x30
#define PCI_CB_IO_LIMIT_0_HI    0x32
#define PCI_CB_IO_BASE_1    0x34
#define PCI_CB_IO_BASE_1_HI 0x36
#define PCI_CB_IO_LIMIT_1   0x38
#define PCI_CB_IO_LIMIT_1_HI    0x3a
#define  PCI_CB_IO_RANGE_MASK   ~0x03
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_CB_BRIDGE_CONTROL   0x3e
#define  PCI_CB_BRIDGE_CTL_PARITY   0x01	/* Similar to standard bridge control register */
#define  PCI_CB_BRIDGE_CTL_SERR     0x02
#define  PCI_CB_BRIDGE_CTL_ISA      0x04
#define  PCI_CB_BRIDGE_CTL_VGA      0x08
#define  PCI_CB_BRIDGE_CTL_MASTER_ABORT 0x20
#define  PCI_CB_BRIDGE_CTL_CB_RESET 0x40	/* CardBus reset */
#define  PCI_CB_BRIDGE_CTL_16BIT_INT    0x80	/* Enable interrupt for 16-bit cards */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM0 0x100	/* Prefetch enable for both memory regions */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM1 0x200
#define  PCI_CB_BRIDGE_CTL_POST_WRITES  0x400
#define PCI_CB_SUBSYSTEM_VENDOR_ID 0x40
#define PCI_CB_SUBSYSTEM_ID 0x42
#define PCI_CB_LEGACY_MODE_BASE 0x44	/* 16-bit PC Card legacy mode base address (ExCa) */
/* 0x48-0x7f reserved */

#define PCI_VENDOR_ID_PHOENIX   0x182e
#define PCI_DEVICE_ID_PHOENIX   0

#define PCI_BUS(d)      (((d) >> 16) & 0xff)
#define PCI_DEV(d)      (((d) >> 11) & 0x1f)
#define PCI_FUNC(d)     (((d) >> 8) & 0x7)
#define PCI_BDF(b,d,f)  ((b) << 16 | (d) << 11 | (f) << 8)

#define PCI_MAX_PCI_DEVICES 32
#define PCI_MAX_PCI_FUNCTIONS   8

typedef unsigned int pci_bdf_t;

struct pcidev_driver;
struct pci_dev;

struct pci_dev {
	struct list_head list;

	unsigned char bus_num;
	unsigned int dev_num;
	unsigned int func_num;

	unsigned short vendor;
	unsigned short device;
	unsigned char hdr_type;

	char name[90];
	char slot_name[8];

	struct pcidev_driver *drv;

	unsigned int *mmio;
	unsigned int mmio_size;
};

struct pcidev_driver {
	char name[90];
	unsigned short vendor;
	unsigned short device;
	void (*init) (struct pci_dev *);
	struct pcidev_driver *next;
};

#define PCI_CTRL_HOST_MODE                  1
#define PCI_CTRL_DEVICE_MODE                0
#define PCI_CTRL_SIM_MODE                   2

#define PCI_CTRL_PCI_MODE                   0
#define PCI_CTRL_PCIX_MODE                  1

#define PCI_CTRL_REG_MODE                   0
#define PCI_CTRL_REG_DEV_MODE_BASE          1
#define PCI_CTRL_REG_DEV_MODE_SIZE          2

/* PCIX extended reg base starts at byte offset 256 */
#ifdef PHOENIX_LITTLE_ENDIAN
#define PCIX_REG_BASE                       64
#else
#define PCIX_REG_BASE                       (512 + 64)
#endif

#define PCIX_HBAR0_REG 				        (PCIX_REG_BASE + 0)
#define PCIX_HBAR1_REG 				        (PCIX_REG_BASE + 1)
#define PCIX_HBAR2_REG 				        (PCIX_REG_BASE + 2)
#define PCIX_HBAR3_REG 				        (PCIX_REG_BASE + 3)
#define PCIX_HBAR4_REG 				        (PCIX_REG_BASE + 4)
#define PCIX_HBAR5_REG 				        (PCIX_REG_BASE + 5)		

#define PCIX_ENBAR0_REG 			        (PCIX_REG_BASE + 6)
#define PCIX_ENBAR1_REG 			        (PCIX_REG_BASE + 7)
#define PCIX_ENBAR2_REG	 			        (PCIX_REG_BASE + 8)
#define PCIX_ENBAR3_REG 			        (PCIX_REG_BASE + 9)
#define PCIX_ENBAR4_REG 			        (PCIX_REG_BASE + 10)
#define PCIX_ENBAR5_REG 			        (PCIX_REG_BASE + 11)		

#define PCIX_MATCHBIT_ADDR_REG 			    (PCIX_REG_BASE + 12)
#define PCIX_MATCHBIT_SIZE_REG 			    (PCIX_REG_BASE + 13)
#define PCIX_PHOENIX_CONTROL_REG 		    (PCIX_REG_BASE + 14)

#define PCIX_INTRPT_CONTROL_REG 		    (PCIX_REG_BASE + 15)
#define PCIX_INTRPT_STATUS_REG 			    (PCIX_REG_BASE + 16)

#define PCIX_ERRSTATUS_REG 			        (PCIX_REG_BASE + 17)

#define PCIX_INT_MSILADDR0_REG			    (PCIX_REG_BASE + 18)
#define PCIX_INT_MSIHADDR0_REG			    (PCIX_REG_BASE + 19)
#define PCIX_INT_MSIDATA0_REG 			    (PCIX_REG_BASE + 20)

#define PCIX_INT_MSILADDR1_REG			    (PCIX_REG_BASE + 21)
#define PCIX_INT_MSIHADDR1_REG			    (PCIX_REG_BASE + 22)
#define PCIX_INT_MSIDATA1_REG 			    (PCIX_REG_BASE + 23)				

#define PCIX_TARGET_DBG0_REG 			    (PCIX_REG_BASE + 24)
#define PCIX_TARGET_DBG1_REG 			    (PCIX_REG_BASE + 25)

#define PCIX_INIT_DBG0_REG 			        (PCIX_REG_BASE + 26)
#define PCIX_INIT_DBG1_REG 			        (PCIX_REG_BASE + 27)		

#define PCIX_EVENT_EN_REG 			        (PCIX_REG_BASE + 28)
#define PCIX_EVENT_CNT_REG 			        (PCIX_REG_BASE + 29)

#define PCIX_HOST_MODE_STATUS_REG 		    (PCIX_REG_BASE + 30)	
#define PCIX_DEFEATURE_MODE_CTLSTATUS_REG 	(PCIX_REG_BASE + 31)

#define PCIX_EXPROM_ADDR_REG 			    (PCIX_REG_BASE + 32)
#define PCIX_EXPROM_SIZE_REG 			    (PCIX_REG_BASE + 33)		

#define PCIX_DMA_TIMEOUT_REG 			    (PCIX_REG_BASE + 34)		
#define PCIX_HOST_MODE_CTRL_REG 		    (PCIX_REG_BASE + 35)		

#define PCIX_READEX_BAR_REG			        (PCIX_REG_BASE + 36)
#define PCIX_READEX_BAR_SIZE_REG		    (PCIX_REG_BASE + 37)
#define PCIX_DMA_UNALIGNED_LO_REG		    (PCIX_REG_BASE + 38)
#define PCIX_DMA_UNALIGNED_HI_REG		    (PCIX_REG_BASE + 39)

#define PCIX_TX_CALIB_PRESET_REG		    (PCIX_REG_BASE + 40)
#define PCIX_TX_CALIB_PRESET_COUNT_REG	    (PCIX_REG_BASE + 41)
#define PCIX_TX_CALIB_BIAS_REG			    (PCIX_REG_BASE + 42)
#define PCIX_TX_CALIB_CODE_REG			    (PCIX_REG_BASE + 43)

#define PCIX_DEVMODE_TBL_BAR0_REG		    (PCIX_REG_BASE + 44)
#define PCIX_DEVMODE_TBL_BAR1_REG		    (PCIX_REG_BASE + 45)
#define PCIX_DEVMODE_TBL_BAR2_REG		    (PCIX_REG_BASE + 46)
#define PCIX_DEVMODE_TBL_BAR3_REG		    (PCIX_REG_BASE + 47)

#define PCIX_L2_ALLOC_BAR_REG			    (PCIX_REG_BASE + 48)
#define PCIX_L2_ALLOC_BAR_SIZE_REG		    (PCIX_REG_BASE + 49)

extern int pci_ctrl_mode;
extern unsigned long pci_config_base;
extern unsigned long pci_mmio_base;
extern phoenix_reg_t *pci_ctrl_mmio;

extern int pci_read_config_byte(pci_bdf_t bdf, int where, uint8_t * val);

extern int pci_read_config_word(pci_bdf_t bdf, int where, uint16_t * val);

extern int pci_read_config_dword(pci_bdf_t bdf, int where, uint32_t * val);

extern int pci_write_config_byte(pci_bdf_t bdf, int where, uint8_t val);

extern int pci_write_config_word(pci_bdf_t bdf, int where, uint16_t val);

extern int pci_write_config_dword(pci_bdf_t bdf, int where, uint32_t val);

#define USE_NATSEMI_HT  1
#define USE_NATSEMI_PCI  2

#endif
