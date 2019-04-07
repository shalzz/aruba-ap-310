#ifndef __BCM95301X_SVK_H
#define __BCM95301X_SVK_H

#include "iproc_board.h" 

#define CONFIG_BOARD_LATE_INIT
#define CONFIG_PHYS_SDRAM_1_SIZE			0x08000000 /* 128 MB */
#define IPROC_ETH_MALLOC_BASE 0xD00000

#undef CONFIG_ENV_IS_NOWHERE

#define NAND_MAX_CHIPS 1 /* deal with our screwed up NAND infrastructure */
#define WORD_COPY_COMPARE 1 /* use long to copy and compare instead of byte */

/* Enable generic u-boot SPI flash drivers and commands */
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SPI_FLASH_MACRONIX
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* SPI flash configurations for Northstar */
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0

/* SPI flash configuration - flash specific */
#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 50000000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3

/* Environment variables */
#ifdef CONFIG_NAND_IPROC_BOOT
#define CONFIG_ENV_IS_IN_NAND                   1
#define CONFIG_ENV_OFFSET                       0x200000
#define CONFIG_ENV_RANGE                        0x200000
#else
#ifdef CONFIG_SPI_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CFG_ENV_IS_IN_SPI_FLASH CONFIG_ENV_IS_IN_SPI_FLASH
#if (__SPRINGBANK_FLASH__ == 16)   /* 16MB flash */          
#define CONFIG_ENV_OFFSET                       0xff0000
#else    /* 32M flash */
#define CONFIG_ENV_OFFSET                       0x1ff0000
#endif
#define CONFIG_ENV_SPI_MAX_HZ                   40000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#endif /* CONFIG_SPI_FLASH */
#endif /* CONFIG_NAND_BOOT */


#ifdef IPROC_BOARD_DIAGS
#define CONFIG_SYS_HZ       1000 //1000*1000
#else
#define CONFIG_SYS_HZ       1000
#endif



/* Environment variables for NAND flash */
#undef CONFIG_CMD_NAND 
//#define CONFIG_IPROC_NAND 
#define CONFIG_SYS_MAX_NAND_DEVICE			1
#define CONFIG_SYS_NAND_BASE		        0xdeadbeef
#define CONFIG_SYS_NAND_ONFI_DETECTION

#include "iproc_common_configs.h"

#undef CONFIG_STANDALONE_LOAD_ADDR
#define CONFIG_STANDALONE_LOAD_ADDR 0x81000000

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE 9600
//#define CONFIG_BOOTARGS     	"console=ttyS0,115200n8 maxcpus=2 mem=512M"

//#define CONFIG_BOOTDELAY		5	/* User can hit a key to abort kernel boot and stay in uboot cmdline */
//#define CONFIG_BOOTCOMMAND 		"dhcp; run nfsargs; bootm;"	/* UBoot command issued on power up */

//#define CONFIG_DEFAULT_L2_CACHE_LATENCY (1) /* To use default value */ // this has not been proven to be working for all boards
#define CONFIG_NET_RETRY_COUNT 5

#define CONFIG_EXTRA_ENV_SETTINGS \
	"brcmtag=0\0"               \
	"console=ttyS0\0"           \
    "loglevel=7\0"              \
    "machid=0xbb8\0"            \
    "tftpblocksize=512\0"       \
    "hostname=northstar\0"      \
    "vlan1ports=0 1 2 3 8u\0"   \
    "nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs\0"        \
	"nfsargs=setenv bootargs "  \
        "loglevel=${loglevel} " \
        "console=${console},${baudrate}n8 "   \
        "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:northstar:: " \
        "maxcpus=2 "            \
		"mem=512M "             \
		"root=${nfsroot} "      \
		"\0"                    \
    "reflash="                  \
        "dhcp ${loadaddr} u-boot.bin; " \
        "sf probe 0; "          \
        "sf erase 0 +${filesize}; " \
        "sf write ${fileaddr} 0 ${filesize};" \
        "\0"                    \
    "clr_env="                  \
        "sf probe 0;"           \
        "sf erase 0xa0000 +1;"  \
        "\0"        

#undef CONFIG_GENERIC_MMC

#if 0
#define CONFIG_BOOTCOMMAND \
	"if mmc rescan ${mmcdev}; then " \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"else " \
			"if run loaduimage; then " \
				"run mmcboot; " \
			"fi; " \
		"fi; " \
	"fi"
#endif

#endif /* __BCM95301X_SVK_H */

#if (__SPRINGBANK_FLASH__ == 16)   /* 16MB flash */
#define CONFIG_SYS_MAX_FLASH_SECT 256
#define CONFIG_SYS_FLASH_SIZE (16 * 1024 * 1024)
#define CONFIG_SYS_MANUF_BASE 0x1efe0000
#define CONFIG_SYS_TPM_CERT 0x1efa0000
#define CONFIG_SYS_APBOOT_SIZE (1024 * 1024) /* 16 sectors */
#define CONFIG_SYS_AP_PRODUCTION_IMAGE 0x1e100000
#define CONFIG_SYS_AP_PRODUCTION_IMAGE_SIZE (10 * 1024 * 1024) /* 10MB */
#define CONFIG_SYS_AP_PROVISIONING_IMAGE 0x1eb00000
#define CONFIG_SYS_AP_PROVISIONING_IMAGE_SIZE (4 * 1024 * 1024) /* 4MB */
#else /* 32MB flash */
#define CONFIG_SYS_MAX_FLASH_SECT 512
#define CONFIG_SYS_FLASH_SIZE (32 * 1024 * 1024)
#define CONFIG_SYS_MANUF_BASE 0x1ffe0000
#define CONFIG_SYS_TPM_CERT 0x1ffa0000
#define CONFIG_SYS_APBOOT_SIZE (1024 * 1024) /* 16 sectors */
#define CONFIG_SYS_AP_PRODUCTION_IMAGE 0x1e100000
#define CONFIG_SYS_AP_PRODUCTION_IMAGE_SIZE (14 * 1024 * 1024) /* 14MB */
#define CONFIG_SYS_AP_PROVISIONING_IMAGE 0x1ef00000
#define CONFIG_SYS_AP_PROVISIONING_IMAGE_SIZE (14 * 1024 * 1024) /* 14MB */
#endif

#define CONFIG_SYS_MAX_FLASH_BANKS  1
#define CONFIG_SYS_FLASH_BASE 0x1e000000

#define TOTAL_MALLOC_LEN CONFIG_SYS_MALLOC_LEN

/* XXX: check this */
#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS  2
#define CONFIG_DTT_LM75
#define CONFIG_DTT_SENSORS { 0x48, 0x49, 0x4b }

#define CFG_64BIT_VSPRINTF
#define CFG_64BIT_STRTOUL
#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS  2
//#define CFG_ENV_IS_IN_FLASH
#define CONFIG_SYS_MONITOR_BASE CONFIG_SYS_TEXT_BASE    /* start of monitor */
#define CFG_MONITOR_BASE CONFIG_SYS_MONITOR_BASE
#if defined(CONFIG_ENV_IS_IN_SPI_FLASH)
#undef CONFIG_ENV_SIZE
/*
 * override 4K set by CONFIG_L2C_AS_RAM, since we won't access the
 * environment prior to relocation
 */
#define CONFIG_ENV_SIZE 0x10000
#endif
#define CFG_ENV_SIZE	CONFIG_ENV_SIZE
#define CFG_ENV_ADDR	CONFIG_ENV_ADDR
#define CFG_MAX_FLASH_SECT CONFIG_SYS_MAX_FLASH_SECT
#define CFG_MAX_FLASH_BANKS CONFIG_SYS_MAX_FLASH_BANKS
#define CFG_FLASH_BASE CONFIG_SYS_FLASH_BASE
#define	CFG_FLASH_SIZE CONFIG_SYS_FLASH_SIZE
#define	CFG_MAXARGS	CONFIG_SYS_MAXARGS
#define	CFG_LONGHELP
#define	CFG_MEMTEST_START CONFIG_SYS_MEMTEST_START
#define	CFG_MEMTEST_END CONFIG_SYS_MEMTEST_END
#define CFG_HZ CONFIG_SYS_HZ
#define CFG_NS16550_COM1 CONFIG_SYS_NS16550_COM1
//#define CFG_NS16550_COM2 CONFIG_SYS_NS16550_COM2
#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE	1
#define CFG_NS16550_CLK          CONFIG_SYS_NS16550_CLK
#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "
#define CFG_LONGHELP
//#define CFG_NO_FLASH CONFIG_SYS_NO_FLASH
#define	CFG_CBSIZE	1024		/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_I2C_SPEED CONFIG_SYS_I2C_SPEED
#define CFG_I2C_SLAVE CONFIG_SYS_I2C_SLAVE
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/
#define	CFG_BOOTMAPSZ		CONFIG_SYS_BOOTMAPSZ
#define CONFIG_COMMANDS	(  CFG_CMD_DFL \
			 | CFG_CMD_DHCP	\
                         | CFG_CMD_I2C \
                         | CFG_CMD_PCI \
                         | CFG_CMD_NET \
                         | CFG_CMD_PING \
                         | CFG_CMD_BSP	\
			 | CFG_CMD_FLASH	\
			 | CFG_CMD_MEMORY	\
             		 | CFG_CMD_IMI \
			 | CFG_CMD_ENV	\
			 | CFG_CMD_BOOTD	\
			 | CFG_CMD_RUN	\
			 | CFG_CMD_MII	\
			 | CFG_CMD_DTT \
			 )
#include <cmd_confdefs.h>

#define CFG_TFTP_STAGING_AREA	(64 * 1024 * 1024)
#define AP_SLOW_FLASH_STAGING_AREA CFG_TFTP_STAGING_AREA
#define CFG_64BIT_VSPRINTF
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200, 230400, 460800 }
#define	CFG_LOAD_ADDR	CFG_TFTP_STAGING_AREA
#define CFG_FLASH_CFI
#define CFG_CONSOLE_INFO_QUIET
#define CFG_LATE_BOARD_INIT
#define CFG_MANUF_BASE      CONFIG_SYS_MANUF_BASE
#define CFG_MFG_SIZE	    0x10000
#define CFG_TPM_CERT 	    CONFIG_SYS_TPM_CERT
#define CONFIG_SYS_WATCHDOG_FREQ CFG_HZ

#define ARUBA_ARMV7

#define CONFIG_APBOOT
#define	nvram_manuf_t	std_manuf_t

#define ARUBA_BOARD_TYPE	"Balvenie"
#define __MODEL_STR__		"AP-205H"

#define	APBOOT_IMAGE	0x1e000000
#define	APBOOT_SIZE	CONFIG_SYS_APBOOT_SIZE

#define __SIGNED_IMAGES__ 1
#define __ENFORCE_SIGNED_IMAGES__ 1

#define	AP_PRODUCTION_IMAGE		CONFIG_SYS_AP_PRODUCTION_IMAGE
#define	AP_PRODUCTION_IMAGE_SIZE	CONFIG_SYS_AP_PRODUCTION_IMAGE_SIZE
#define	AP_PROVISIONING_IMAGE		CONFIG_SYS_AP_PROVISIONING_IMAGE
#define	AP_PROVISIONING_IMAGE_SIZE	CONFIG_SYS_AP_PROVISIONING_IMAGE_SIZE

#define	__OS_IMAGE_TYPE	ARUBA_IMAGE_SPRINGBANK
//#define	__BOOT_IMAGE_TYPE ARUBA_IMAGE_SPRINGBANK
#define ARUBA_HEADER_SIZE 512

/* reset if no command received in two minutes */
#define CONFIG_BOOT_RETRY_TIME 120 	/* seconds */
#define CONFIG_RESET_TO_RETRY 1

#define __TLV_INVENTORY__
#define __TLV_INVENTORY_BUF_SIZE__        (24 * 1024)	/* XXX */
#define __TLV_INVENTORY_NO_RELOCATE__	/* ARMv7 -pie will take care of this */
#define __CARD_WITH_VARIANT__ 1
#define __ARUBA_WIRELESS_MAC__  1

#define CONFIG_ENV_ADDR (CONFIG_ENV_OFFSET + CONFIG_SYS_FLASH_BASE)
#define CONFIG_ENV_OVERWRITE

#define CONFIG_CMD_MII
#define CONFIG_PHY_GIGE

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#undef CONFIG_BOOTARGS
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY 2
#undef CONFIG_ETHADDR
#undef CONFIG_IPADDR
#undef CONFIG_SERVERIP
#undef CONFIG_LOADADDR
#undef CONFIG_BOOTFILE
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_NETMASK

#undef	CONFIG_EXTRA_ENV_SETTINGS
#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Balvenie\0"			\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=armv7ns.ari\0"		        \
	""

#define CONFIG_PCIE0
#define CONFIG_PCIE1

/* GPIO outputs */
#define GPIO_RED_STATUS_LED	7
#define GPIO_AMBER_STATUS_LED   8
#define GPIO_GREEN_STATUS_LED	6
#define GPIO_GREEN_PSE_LED	0
#define GPIO_AMBER_PSE_LED	1

#define GPIO_PSE_CONTROL	2

#define GPIO_HW_RESET		11
#define GPIO_USB_PWR		12

/* TPM */
#define GPIO_TPM_CLK            4
#define GPIO_TPM_DATA           5

/* GPIO inputs */
#define	GPIO_POWER_DC		15
#define	GPIO_POWER_AT		3
#define	GPIO_CONFIG_CLEAR	10

#define CONFIG_AR5416
#define CONFIG_HW_WATCHDOG
#define CONFIG_DISPLAY_BOARDINFO	/* for late_board_init() */
#define USE_FLASH_READ_SUPPORT
#define CONFIG_FLASH_MAP_SPLIT_ADDR 0x1f000000
#define __BOARD_HAS_POWER_INFO__

#define CONFIG_HARD_I2C
#define TPM_DIAG

#define CONFIG_RESET_BUTTON

#define CONFIG_DISPLAY_CPUINFO
#define IPROC_PHY_ADDR 0

//#define SHMOO_DEBUG
#define CONFIG_BOOTM_LINUX
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_IMI
#define CONFIG_SYS_NUM_CORES	1

#define AP_CAL_IN_FLASH
