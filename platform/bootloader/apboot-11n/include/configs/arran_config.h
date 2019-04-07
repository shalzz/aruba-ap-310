#ifndef __arran_config_h__
#define __arran_config_h__

#define	CFG_64BIT_VSPRINTF

#define ARUBA_BOARD_TYPE	"Arran"

#define AP_PROVISIONING_IMAGE   0xf6800000

#define AP_PROVISIONING_IMAGE_SIZE      (7 * 1024 * 1024)

#define AP_PRODUCTION_IMAGE_SIZE        (7 * 1024 * 1024)

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

// reset if no command received in two minutes
#define CONFIG_BOOT_RETRY_TIME 120 	// seconds
#define CONFIG_RESET_TO_RETRY 1

#define	CONFIG_EXTRA_ENV_SETTINGS			\
	"autoload=n\0"					\
	"boardname=Arran\0"				\
	"servername=aruba-master\0"			\
	"bootcmd=boot ap\0"			        \
	"autostart=yes\0"			        \
	"bootfile=armv5te.ari\0"		        \
	""

#undef CFG_PROMPT
#define CFG_PROMPT "apboot> "

#define CFG_TFTP_STAGING_AREA (32 * 1024 * 1024)	// XXX

#define	nvram_manuf_t std_manuf_t

#define CFG_TPM_CERT        0xf6fa0000
#define CFG_MANUF_BASE      0xf6fe0000
#undef CFG_ENV_ADDR
#define CFG_ENV_ADDR        0xf6ff0000
#define CONFIG_ENV_OFFSET      (CFG_FLASH_SIZE - CFG_ENV_SIZE)
//#define CFG_ENV_SIZE        0x10000
#define CFG_MFG_SIZE	    0x10000

#undef CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DEFAULT		(CONFIG_BOOTP_SUBNETMASK | \
					CONFIG_BOOTP_GATEWAY	 | \
					CONFIG_BOOTP_DNS)

#undef CONFIG_BOOTP_MASK
#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT

#define	__MODEL_STR__	"AP-13x"

#define CONFIG_AR5416

/* GPIO outputs */
#define GPIO_RED_STATUS_LED	74
#define GPIO_GREEN_STATUS_LED	75

#define GPIO_TPM_CLK            79
#define GPIO_TPM_DATA           80

#define GPIO_HW_RESET           76

#define	GPIO_2G_RED_LED		70
#define	GPIO_2G_GREEN_LED	71
#define	GPIO_5G_RED_LED		72
#define	GPIO_5G_GREEN_LED	73

#define GPIO_5G_RADIO_RESET	86
#define GPIO_2G_RADIO_RESET	88

#define GPIO_PHY_0_RESET	77

#define GPIO_SMART_ANT_CLOCK	81
#define GPIO_SMART_ANT_ENABLE	82
#define GPIO_SMART_ANT_DATA	83

#define GPIO_PA_VOLTAGE         0

/* GPIO inputs */
#define	GPIO_POWER_DC		84
#define	GPIO_POWER_AT		85

#define	GPIO_PHY_0_INT		78
#define	GPIO_CONFIG_CLEAR	87

/* end GPIO */

#define FLASH_WRITE_ENABLE	1
#define FLASH_WRITE_DISABLE	0

#define	CONFIG_HW_WATCHDOG	1

#undef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE		9600

#define __TLV_INVENTORY__	1
#define INVENT_MAX_INVENTORIES  1
#define __TLV_INVENTORY_BUF_SIZE__ 1024
#define __CARD_WITH_VARIANT__   2

#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS  2
#define CONFIG_DTT_LM75
#define CONFIG_DTT_SENSORS { 0x4b, 0x4c, 0x4d }
//#define LM75_I2C_CHANNEL 0

#define CONFIG_RESET_BUTTON
#define __BOARD_HAS_POWER_INFO__

#undef CONFIG_SYS_DUART_CHAN
#define CONFIG_SYS_DUART_CHAN          1               /* channel to use for console */

/* for now, we don't enable this since Linux isn't set up for it */
#undef CFG_FLASH_PROTECTION

#define CFG_LATE_BOARD_INIT

#define	MV_INC_BOARD_SPI_FLASH
#define CFG_ENV_IS_IN_SPI_FLASH
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH_MACRONIX
#define ARUBA_MARVELL_SWITCH

#define	AP_SLOW_FLASH_STAGING_AREA CFG_TFTP_STAGING_AREA

#define CONFIG_EEE
#define CONFIG_MV6560_INTERNAL_SWITCH
#define CONFIG_PORTLOOP_TEST

#endif
