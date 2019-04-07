#ifndef __aruba_6282_config_h__
#define __aruba_6282_config_h__

#define	CFG_64BIT_VSPRINTF

#define ARUBA_BOARD_TYPE	"DB6560"

#ifdef notyet
#define AP_PROVISIONING_IMAGE   0xbf500000

#define AP_PROVISIONING_IMAGE_SIZE      (4 * 1024 * 1024)
#endif

#define AP_PRODUCTION_IMAGE_SIZE        (14 * 1024 * 1024)

#define __SIGNED_IMAGES__ 1
#define __ENFORCE_SIGNED_IMAGES__ 1
#define APBOOT_SIZE     (1024 * 1024)
#define ARUBA_HEADER_SIZE       512
#define CFG_64BIT_STRTOUL 1

#define CONFIG_NET_MULTI
#define CONFIG_MEMSIZE_IN_BYTES
#define CONFIG_PCI

#define CONFIG_MII
#define CONFIG_PHY_GIGE

#define CONFIG_NET_RETRY_COUNT 5

#undef CONFIG_AUTO_COMPLETE
#define CONFIG_AUTO_COMPLETE 1
#undef CFG_CONSOLE_INFO_QUIET
#define CFG_CONSOLE_INFO_QUIET 1

#undef  CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	2
//#define CFG_LATE_BOARD_INIT	1

// reset if no command received in two minutes
#define CONFIG_BOOT_RETRY_TIME 12000 	// seconds
#define CONFIG_RESET_TO_RETRY 1

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=DB6560\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=armv5te.ari\0"		        \
	""

#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "

#define CFG_TFTP_STAGING_AREA (32 * 1024 * 1024)

#define	nvram_manuf_t std_manuf_t

//#define CFG_TPM_CERT        0xbffa0000
#define CFG_MANUF_BASE      0xf6f80000
#undef CFG_ENV_ADDR
#define CFG_ENV_ADDR        0xf6fc0000
#define CONFIG_ENV_OFFSET      (CFG_FLASH_SIZE - CFG_ENV_SIZE)
//#define CFG_ENV_SIZE        0x40000
#define CFG_MFG_SIZE        0x40000

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#undef CONFIG_BOOTP_MASK
#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT

#define	__MODEL_STR__	"DB-6560"

#define CONFIG_AR5416

//#define       CONFIG_HW_WATCHDOG      1

// work around lack of CONFIG_ARRAN define
#define __BOOT_IMAGE_TYPE	ARUBA_IMAGE_TAMDHU
#define __OS_IMAGE_TYPE		ARUBA_IMAGE_ARRAN

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE         9600

#define	__TLV_INVENTORY__	1
#define INVENT_MAX_INVENTORIES  1
#define __TLV_INVENTORY_BUF_SIZE__ 1024
#define __CARD_WITH_VARIANT__   0
#define __ARUBA_WIRELESS_MAC__  1

#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS  2
#define CONFIG_DTT_LM75
#define CONFIG_DTT_SENSORS { 0x4b, 0x4c, 0x4d }
//#define LM75_I2C_CHANNEL 0

#define	CFG_LATE_BOARD_INIT

#define	MV_INC_BOARD_SPI_FLASH
#define CFG_ENV_IS_IN_SPI_FLASH
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH_STMICRO
//#define ARUBA_MARVELL_SWITCH
#define CONFIG_MV_PORT_SWAP 1
#endif
