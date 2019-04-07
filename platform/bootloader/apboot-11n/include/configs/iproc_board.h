#ifndef __IPROC_BOARD_H
#define __IPROC_BOARD_H

#include <asm/sizes.h>

/* Architecture, CPU, etc */
#define CONFIG_ARMV7
#define CONFIG_IPROC (1)
#define CONFIG_NORTHSTAR (1)

/*
Fix me

When DEBUG is enabled, need to disable both CACHE to make u-boot running
#define CONFIG_SYS_NO_ICACHE
#define CONFIG_SYS_NO_DCACHE
#define DEBUG

*/
#define CONFIG_IPROC_MMU	(1)
#define CONFIG_L2_OFF				/* Disable L2 cache */
#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH (1)
#define CONFIG_MACH_TYPE			0xbb8

#define CONFIG_MISC_INIT_R			/* Call board's misc_init_r function */

/* Interrupt configuration */
#define CONFIG_USE_IRQ          1	/* we need IRQ stuff for timer	*/

/* Memory Info */
#if (defined(CONFIG_L2C_AS_RAM ) && defined(CONFIG_NO_CODE_RELOC))
#define CONFIG_SYS_MALLOC_LEN 			0x8000  	/* see armv7/start.S. */
#else
#define CONFIG_SYS_MALLOC_LEN 			SZ_256K  	/* see armv7/start.S. */
#endif
#define CONFIG_STACKSIZE_IRQ			(4096)
#define CONFIG_STACKSIZE_FIQ			(4096)
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_PHYS_SDRAM_0				0xc0000000
#define CONFIG_L2_CACHE_SIZE			0x40000  
#define CONFIG_PHYS_SDRAM_1				0x00000000
#define CONFIG_LOADADDR					0x90000000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x0 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#else
#define CONFIG_PHYS_SDRAM_0				0x00000000
#define CONFIG_PHYS_SDRAM_1				0x00000000
#define CONFIG_LOADADDR					0x90000000 /* default destination location for tftp file (tftpboot cmd) */
#define CONFIG_PHYS_SDRAM_RSVD_SIZE		0x00100000 /* bytes reserved from CONFIG_PHYS_SDRAM_1 for custom use */
#endif

/* Where kernel is loaded to in memory */
#define CONFIG_SYS_LOAD_ADDR				0x90000000
/* this must match arch/arm/mach-brcm-hnd/Kconfig, which we have modfied */
#define LINUX_BOOT_PARAM_ADDR				0x80001000 /* default mapped location to store the atags pointer */

#define CONFIG_SYS_MEMTEST_START			CONFIG_PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END			(CONFIG_PHYS_SDRAM_1+CONFIG_PHYS_SDRAM_1_SIZE)
#define CONFIG_NR_DRAM_BANKS				1

#define CONFIG_SYS_SDRAM_BASE		(CONFIG_PHYS_SDRAM_1 + CONFIG_PHYS_SDRAM_RSVD_SIZE)
/* CONFIG_SYS_TEXT_BASE is where u-boot is loaded by boot1 */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_SYS_STACK_SIZE		(0x00010000) /* 64K */       
#else
#define CONFIG_SYS_STACK_SIZE		(0x00010000) /* 64K */       
#endif

#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_0 + CONFIG_SYS_STACK_SIZE - 16)
//#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_PHYS_SDRAM_1 + CONFIG_SYS_STACK_SIZE - 16)

/* Clocks */
#define CONFIG_SYS_REF_CLK			(25000000) /*Reference clock = 25MHz */
#define CONFIG_SYS_REF2_CLK			(200000000) /*Reference clock = 25MHz */

#if 0     /* Define the CONFIG_ENV_SIZE in bcm95301x_svk file */
#define CONFIG_ENV_OVERWRITE	/* Allow serial# and ethernet mac address to be overwritten in nv storage */
#ifdef CONFIG_L2C_AS_RAM
#define CONFIG_ENV_SIZE			0x1000 /* 4K */
#else
#define CONFIG_ENV_SIZE			0x10000 /* 64K */
#endif
#endif

/* NO flash */
//#define CONFIG_SYS_NO_FLASH		/* Not using NAND/NOR unmanaged flash */

/* Ethernet configuration */
#ifndef CONFIG_NO_CODE_RELOC
#define CONFIG_BCMIPROC_ETH
#endif
//#define CONFIG_BCMHANA_ETH
#define CONFIG_NET_MULTI

/* DMA configuration */
//#define CONFIG_BCM5301X_DMA

/* General U-Boot configuration */
#define CONFIG_SYS_CBSIZE			1024	/* Console buffer size */
#define CONFIG_SYS_PBSIZE			(CONFIG_SYS_CBSIZE +	sizeof(CONFIG_SYS_PROMPT) + 16) /* Printbuffer size */
#define CONFIG_SYS_MAXARGS			64
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

//#define CONFIG_VERSION_VARIABLE	/* Enabled UBOOT build date/time id string */
#define CONFIG_AUTO_COMPLETE
//#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP

#define CONFIG_CRC32_VERIFY		/* Add crc32 to memory verify commands */
#define CONFIG_MX_CYCLIC			/* Memory display cyclic */

#define CONFIG_CMDLINE_TAG				/* ATAG_CMDLINE setup */
//#define CONFIG_SETUP_MEMORY_TAGS		/* ATAG_MEM setup */

#define CONFIG_SYS_PROMPT					"u-boot> "  

//#include <config_cmd_default.h>
#define CONFIG_CMD_CONSOLE
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_CMD_NFS
#undef CONFIG_GENERIC_MMC
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_MEMORY
//#define CONFIG_CMD_MISC
#define CONFIG_CMD_LICENSE

#define CONFIG_CMD_NAND 
#define CONFIG_IPROC_NAND 

#define CONFIG_ETHADDR		00:21:61:56:05:07
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_IPADDR		192.168.1.91
//#define CONFIG_GATEWAYIP	192.168.0.100
#define CONFIG_SERVERIP		192.168.1.24


#ifdef UBOOT_MDK
#define CONFIG_GMAC_NUM		2 
#else
#define CONFIG_GMAC_NUM		1 
#endif

#ifdef CONFIG_IPROC_BOARD_DIAGS
#define IPROC_BOARD_DIAGS 	/* Enabling this fails booting on HR board */
#endif

#define IPROC_BOARD_DIAGS
#ifdef IPROC_BOARD_DIAGS
//POST related defines
#define CONFIG_HAS_POST
//define post to support diags available, this is a ORed list
//See bitmask definition in post.h
#if 0 //ndef CONFIG_APBOOT
#define CONFIG_POST      (CONFIG_SYS_POST_PWM | CONFIG_SYS_POST_QSPI|CONFIG_SYS_POST_NAND|\
                          CONFIG_SYS_POST_UART |CONFIG_SYS_POST_MEMORY|CONFIG_SYS_POST_GPIO|\
                          CONFIG_SYS_POST_GSIO_SPI | CONFIG_SYS_POST_USB20 | CONFIG_SYS_POST_PCIE |\
                          CONFIG_SYS_POST_I2C | CONFIG_SYS_POST_MMC | CONFIG_SYS_POST_I2S | CONFIG_SYS_POST_USB30|\
                          CONFIG_SYS_POST_I2S_W8955 | CONFIG_SYS_POST_SPDIF | CONFIG_SYS_POST_RGMII |CONFIG_SYS_POST_VOIP)// | CONFIG_SYS_POST_SATA)

#else /* APBOOT */
#undef CONFIG_POST
#define CONFIG_POST (CONFIG_SYS_POST_MEMORY)
#include <asm/arch/socregs.h>
#define CONFIG_SYS_POST_WORD_ADDR QSPI_bspi_registers_SCRATCH
#endif /* APBOOT */

#define CONFIG_CMD_DIAG
//#define CONFIG_SWANG_DEBUG_BUILD

#ifdef CONFIG_SWANG_DEBUG_BUILD
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif
/*
#define CONFIG_DIAGS_MEMTEST_START                (CONFIG_PHYS_SDRAM_1 + 0x1000000 )//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0x8000000
*/
#define CONFIG_DIAGS_MEMTEST_START                0x81000000//offset by 16 MB if not running from L2
#define CONFIG_DIAGS_MEMTEST_END                  0x87FFFFFF

//Chip Common A UART 0
//#define CONFIG_SYS_NS16550_COM1                       (0x18000300)
#ifdef CONFIG_NORTHSTAR_COM2
// Enabling COM2 will hang the HR board. 
//Chip Common A UART 1
#define CONFIG_SYS_NS16550_COM2                 (0x18000400)
#endif
//Chip Common B UART 0
#define CONFIG_SYS_NS16550_COM3                 (0x18008000)

#define  CONFIG_UART0_INDEX              1
#define  CONFIG_UART1_INDEX              2
#define  CONFIG_UART2_INDEX              3


#define CONFIG_HAS_PWM
#define CONFIG_IPROC_GPIO
#define CONFIG_IPROC_GSIOSPI


/* USB */
//#define CONFIG_CMD_USB
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_IPROC
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_LEGACY_USB_INIT_SEQ
#endif /* CONFIG_CMD_USB */

/* PCIE */
#define CONFIG_CMD_PCI
#define CONFIG_CMD_PCI_ENUM
#define CONFIG_PCI
#define CONFIG_PCI_SCAN_SHOW
#define CONFIG_IPROC_PCIE

/* I2C */
#define CONFIG_CMD_I2C
#define CONFIG_IPROC_I2C
#define CONFIG_SYS_I2C_SPEED    0    	/* Default on 100KHz */
#define CONFIG_SYS_I2C_SLAVE	0xff	/* No slave address */

/* SDIO */
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_IPROC_MMC

/* Sound */
#define CONFIG_IPROC_I2S
#define CONFIG_IPROC_PCM

/* USB3.0 */
#define CONFIG_USB_XHCI_IPROC

/* QSPI */
#define CONFIG_CMD_SPI
#define CONFIG_IPROC_QSPI


/* MDIO */
#define CONFIG_IPROC_MDIO

/* SATA */
//#define CONFIG_CMD_SATA
//#define CONFIG_SCSI_AHCI
//#define CONFIG_SYS_SATA_MAX_DEVICE    1
//#define CONFIG_SYS_SCSI_MAX_SCSI_ID   0xff

#define CONFIG_NS_DMA
#ifndef CONFIG_NO_SLIC
#define CONFIG_IPROC_SLIC
#endif
#endif
#endif /* __IPROC_BOARD_H */

